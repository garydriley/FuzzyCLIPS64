   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*             CLIPS Version 6.00  05/12/93            */
   /*                                                     */
   /*             FUZZY MODIFIER (HEDGES) MODULE          */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*      Brian L. Donnell                                     */
/*      Bob Orchard (NRCC - Nat'l Research Council of Canada)*/
/*                  (Fuzzy reasoning extensions)             */
/*                  (certainty factors for facts and rules)  */
/*                  (extensions to run command)              */
/*                                                           */
/* Contributing Programmer(s):                               */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*************************************************************/

 
#define _FUZZYMOD_SOURCE_

#include <math.h>
#include <ctype.h>

#include "setup.h"


#if FUZZY_DEFTEMPLATES

#include "entities.h"

#include "evaluatn.h" // TBD Needed up/down
#include "expressn.h"
#include "symbol.h"
#include "userdata.h"

#include "constant.h"
#include "envrnmnt.h"
#include "extnfunc.h"

#include "fuzzydef.h"
#include "fuzzyrhs.h"
#include "fuzzyutl.h"
#include "fuzzymod.h"
#include "fuzzypsr.h"
#include "memalloc.h"
#include "prntutil.h"
#include "router.h"


/******************************************************************
    Global Internal Function Declarations
    
    Defined in fuzzymod.h
 ******************************************************************/



/******************************************************************
    Local Internal Function Declarations
 ******************************************************************/
 
  static void                  Yexpand_set(Environment *theEnv,struct fuzzy_value *fv);
  static void                  GenericModFunction(Environment *theEnv,struct fuzzy_value *fv,
                                                  FunctionDefinition *,Deffunction *);
  static void                  veryModFunction(Environment *theEnv,struct fuzzy_value *fv);
  static void                  notModFunction(Environment *theEnv,struct fuzzy_value *fv);
  static void                  more_or_lessModFunction(Environment *theEnv,struct fuzzy_value *fv);
  static void                  somewhatModFunction(Environment *theEnv,struct fuzzy_value *fv);
  static void                  normModFunction(Environment *theEnv,struct fuzzy_value *fv);
  static void                  intensifyModFunction(Environment *theEnv,struct fuzzy_value *fv);
  static void                  plusModFunction(Environment *theEnv,struct fuzzy_value *fv);
  static void                  slightlyModFunction(Environment *theEnv,struct fuzzy_value *fv);
  static void                  extremelyModFunction(Environment *theEnv,struct fuzzy_value *fv);
  static void                  belowModFunction(Environment *theEnv,struct fuzzy_value *fv);
  static void                  aboveModFunction(Environment *theEnv,struct fuzzy_value *fv);
  static void                  concentrate_dilute(Environment *theEnv,struct fuzzy_value *fv, double con);
  static void                  ClearModifiers(Environment *theEnv,void *context);

/******************************************************************
    Local Internal Variable Definitions
 ******************************************************************/

/*************************************************************/ 
/* initFuzzyModifierList:                                    */ 
/*                                                           */
/* initialize the list of available fuzzy modifiers          */
/*************************************************************/
void initFuzzyModifierList(
  Environment *theEnv)
  {
   AddFuzzyModifier(theEnv,"not", notModFunction, NULL
#if DEFFUNCTION_CONSTRUCT
                    , NULL
#endif
                   );

   AddFuzzyModifier(theEnv,"very", veryModFunction, NULL
#if DEFFUNCTION_CONSTRUCT
                    , NULL
#endif
                   );

   AddFuzzyModifier(theEnv,"more-or-less", more_or_lessModFunction, NULL
#if DEFFUNCTION_CONSTRUCT
                    , NULL
#endif
                   );

   AddFuzzyModifier(theEnv,"somewhat", somewhatModFunction, NULL
#if DEFFUNCTION_CONSTRUCT
                    , NULL
#endif
                   );

   AddFuzzyModifier(theEnv,"norm", normModFunction, NULL
#if DEFFUNCTION_CONSTRUCT
                    , NULL
#endif
                   );

   AddFuzzyModifier(theEnv,"intensify",  intensifyModFunction, NULL
#if DEFFUNCTION_CONSTRUCT
                    , NULL
#endif
                   );

   AddFuzzyModifier(theEnv,"plus", plusModFunction, NULL
#if DEFFUNCTION_CONSTRUCT
                    , NULL
#endif
                    );

   AddFuzzyModifier(theEnv,"slightly", slightlyModFunction, NULL
#if DEFFUNCTION_CONSTRUCT
                    , NULL
#endif
                   );

   AddFuzzyModifier(theEnv,"extremely", extremelyModFunction, NULL
#if DEFFUNCTION_CONSTRUCT
                    , NULL
#endif
                   );

   AddFuzzyModifier(theEnv,"above", aboveModFunction, NULL
#if DEFFUNCTION_CONSTRUCT
                    , NULL
#endif
                   );

   AddFuzzyModifier(theEnv,"below", belowModFunction, NULL
#if DEFFUNCTION_CONSTRUCT
                    , NULL
#endif
                   );


  /* Also add the function to do the clear of the user defined
     modifiers 
  */
   AddClearFunction(theEnv,"FuzzyModifiers", ClearModifiers, 2001,NULL);
  }


/******************************************************************
   Routines that implement the modifiers (hedges) for Fuzzy
     liguistic expressions.

     To add a new one just add to this set of functions and
     add a new entry to the initFuzzyModifierList function above.
     (also add to the declarations above:

     e.g. 

        static void slightlyModFunction(struct fuzzy_value *fv);

   Note that GenericModFunction is a special version that is used
   to modifiy the y values of a fuzzy set with a CLIPS defined
   function (deffunction or CLIPS system supplied function) 
   that takes an float and returns a float.
   The function to be used is given in the call to the Fuzzy CLIPS
   function add-fuzzy-modifier:

    e.g.   (add-fuzzy-modifier  my-mod  my-mod-function)

******************************************************************/

/*************************************************************/ 
/* GenericModFunction:                                       */ 
/*                                                           */
/* implements the generic hedge                              */
/*                                                           */
/*      each elelment is modified by the function supplied   */
/*      -- either a deffunction or a standard CLIPS function */
/*                                                           */
/*      only 1 of the 2nd or 3rd arg will be non NULL        */
/*                                                           */
/*************************************************************/
static void GenericModFunction(
  Environment *theEnv,
  struct fuzzy_value *fv,
#if DEFFUNCTION_CONSTRUCT
  FunctionDefinition *clipsfun,
  Deffunction *deffun)
#else
  FunctionDefinition *clipsfun)
#endif
{
 int i;
 int num;
 void (*fp)(Environment *,UDFContext *,UDFValue *) = NULL;
 struct expr *tmpExpr;
 UDFValue result;
 bool IsStandardFunction, res;

 Yexpand_set(theEnv,fv);  /* expands the set without making copy of the fuzzy value */

 num=fv->n;  /* do this AFTER expand set!! */

 /* CurrentExpression points to current expression to be evaluated
    so we set it up temporarily to point to a dummy expression 
    holding our fuzzy set y values (1 at a time) to be modified.
    CurrentExpression is a GLOBAL value!!
 */
 tmpExpr = EvaluationData(theEnv)->CurrentExpression;
 /* this expr is where the FCALL/PCALL would normally be */
 EvaluationData(theEnv)->CurrentExpression = get_struct(theEnv,expr); 

 IsStandardFunction = (clipsfun == NULL) ? false : true;
    
 if (IsStandardFunction)
    { /* system defined or user defined internal function */
      fp = clipsfun->functionPointer;
    }

 for (i=0; i<num; i++)
     { /* note: assume y values all in range 0 to 1 */

        double newval;
        
        EvaluationData(theEnv)->CurrentExpression->argList = GenConstant(theEnv,FLOAT_TYPE, CreateFloat(theEnv,fv->y[i]));
        
        if (IsStandardFunction)
          {
           (*fp)(theEnv,NULL,&result); // TBD Use EvaluateExpression for UDFContext;
           newval = result.floatValue->contents;
          }
        else
          {
#if DEFFUNCTION_CONSTRUCT
            res = (*EvaluationData(theEnv)->PrimitivesArray[PCALL]->evaluateFunction)(theEnv,deffun,&result);
            if (!res || (result.header->type != FLOAT_TYPE))
              {
                SyntaxErrorMessage(theEnv,"Fuzzy Modifier function must accept FLOAT arg and return FLOAT)");
                break;               
              }
            newval = result.floatValue->contents;
#else
            newval = fv->y[i];
#endif
          }
        
        /* membership values must lie between 0 and 1 */
        if (newval > 1.0) 
           newval = 1.0;
        else if (newval < 0.0)
           newval = 0.0;
        
        fv->y[i] = newval;
        rtn_struct(theEnv,expr, EvaluationData(theEnv)->CurrentExpression->argList);
     }

 /*DO NOT use ReturnExpression here! Haven't set up ptrs in this expression */
 rtn_struct(theEnv,expr, EvaluationData(theEnv)->CurrentExpression); 
 EvaluationData(theEnv)->CurrentExpression = tmpExpr;
}

/*************************************************************/ 
/* notModFunction:                                           */ 
/*                                                           */
/* implements the 'not' hedge                                */
/*                                                           */
/*      each elelment is complemented (1.0 - Y)              */
/*                                                           */
/*************************************************************/
static void notModFunction(
  Environment *theEnv,
  struct fuzzy_value *fv)
  {
   int i;
   int num;

   num=fv->n;

   for (i = 0; i < num; i++)
     {  /* note: assume y values all in range 0 to 1 */
      fv->y[i] = 1.0 - fv->y[i];
     }
  }

/*************************************************************/ 
/* concentrate_dilute:                                       */ 
/*                                                           */
/* implements the concentrate and dilute operations          */
/*                                                           */
/*      each elelment is modified by some power              */
/*                                                           */
/*  for example    y**2, y**3      -- concentration          */
/*                 y**0.5, y**0.3  -- dilution               */
/*                                                           */
/*                                                           */
/*************************************************************/
static void concentrate_dilute(
  Environment *theEnv,
  struct fuzzy_value *fv,
  double power)
  {
   int i;
   int num;

   Yexpand_set(theEnv,fv);  /* expands the set without making copy of the fuzzy value */

   num=fv->n;  /* do this AFTER expanding set!!! */

   for (i = 0; i < num; i++)
     {  /* note: assume y values all in range 0 to 1 */
      fv->y[i] = pow(fv->y[i], power);
     }
  }

/*************************************************************/ 
/* veryModFunction:                                          */ 
/*                                                           */
/* implements the 'very' hedge                               */
/*                                                           */
/*      each elelment is squared (y**2)                      */
/*                                                           */
/*************************************************************/
static void veryModFunction(
  Environment *theEnv,
  struct fuzzy_value *fv)
  {
   concentrate_dilute(theEnv,fv, 2.0);
  }

/*************************************************************/ 
/* extremelyModFunction:                                     */ 
/*                                                           */
/* implements the 'extremely' hedge                          */
/*                                                           */
/*      each elelment is squared (y**3)                      */
/*                                                           */
/*************************************************************/
static void extremelyModFunction(
  Environment *theEnv,
  struct fuzzy_value *fv)
  {
   concentrate_dilute(theEnv,fv, 3.0);
  }

/*************************************************************/ 
/* more_or_lessModFunction:                                  */ 
/*                                                           */
/* implements the 'more-or-less' hedge                       */
/*                                                           */
/*      each elelment is square-rooted (y**0.333333)         */
/*                                                           */
/*************************************************************/
static void more_or_lessModFunction(
  Environment *theEnv,
  struct fuzzy_value *fv)
  {
   concentrate_dilute(theEnv,fv, 1.0/3.0);
  }

/*************************************************************/ 
/* somewhatModFunction:                                      */ 
/*                                                           */
/* implements the 'somewhat' hedge                           */
/*                                                           */
/*      each elelment is square-rooted (y**0.5)              */
/*                                                           */
/*************************************************************/
static void somewhatModFunction(
  Environment *theEnv,
  struct fuzzy_value *fv)
  {
   concentrate_dilute(theEnv,fv, 0.5);
  }

/*************************************************************/ 
/* normModFunction:                                          */ 
/*                                                           */
/* implements the 'norm' hedge                               */
/*                                                           */
/*      scale the y values to be between 0.0 and 1.0         */
/*      with at least 1 value at 1.0 --                      */
/*      using the highest value (highy) to determine the     */
/*      scale as:                                            */
/*                  1.0 / highy                              */
/*                                                           */
/*************************************************************/
static void normModFunction(
  Environment *theEnv,
  struct fuzzy_value *fv)
  {
   int i;
   int num=fv->n;
   double scale;
   double highy = 0.0;

   /* find the maximum value */
   for (i=0; i<num; i++)
     { 
      if (highy < fv->y[i])
        highy = fv->y[i];
     }

   /* if not all membership values 0.0 and largest value
      is not 1.0 then do scaling
   */
   if (highy < 1.0 && highy > 0.0)
     {
      scale = 1.0 / highy;

      for (i=0; i<num; i++)
        { 
          fv->y[i] = fv->y[i] * scale;
        }
     }
  }

/*************************************************************/ 
/* intensifyModFunction:                                     */ 
/*                                                           */
/* implements the 'intensify' hedge                          */
/*                                                           */
/*   for y values in range [0, 0.5]    2(y**2)               */
/*   for y values in range (0.5, 1.0]  1-2(1-y)**2           */
/*                                                           */
/*************************************************************/
static void intensifyModFunction(
  Environment *theEnv,
  struct fuzzy_value *fv)
  {
   int i;
   int num;

   Yexpand_set(theEnv,fv);  /* expands the set without making copy of the fuzzy value */

   num=fv->n;  /* do this AFTER expanding set!!! */

   for (i=0; i<num; i++)
     {  /* note: assume y values all in range 0 to 1 */
      if (fv->y[i] <= 0.5)
        { fv->y[i] = 2.0 * (fv->y[i] * fv->y[i]); }
      else
        {
         double temp = 1.0 - fv->y[i];
         fv->y[i] = 1.0 - (2.0 * (temp * temp));
        }
     }
  }

/*************************************************************/ 
/* plusModFunction:                                          */ 
/*                                                           */
/* implements the 'plus' hedge                               */
/*                                                           */
/*      each elelment is square-rooted (y**1.25)             */
/*                                                           */
/*************************************************************/
static void plusModFunction(
  Environment *theEnv,
  struct fuzzy_value *fv)
  {
   concentrate_dilute(theEnv,fv, 1.25);
  }

/*************************************************************/ 
/* aboveModFunction:                                         */ 
/*                                                           */
/* implements the 'above' hedge                              */
/*                                                           */
/*                                                           */
/*************************************************************/
static void aboveModFunction(
  Environment *theEnv,
  struct fuzzy_value *fv)
  {
   int i, n, maxpos;
   double maxval;
  
   n = fv->n;

   /* get position where max value occurs */
   maxval = 0.0;
   maxpos = 0;

   for (i=0; i<n; i++)
     {
      if (fv->y[i] >= maxval)
        {
         maxpos = i;
         maxval = fv->y[i];
         if (maxval == 1.0) break;
        }
     }

   for (i=0; i<n; i++)
     {
      if (i <= maxpos)
        { fv->y[i] = 0.0; }
      else
        { fv->y[i] = 1.0 - fv->y[i]; }
     }
  }

/*************************************************************/ 
/* belowModFunction:                                         */ 
/*                                                           */
/* implements the 'below' hedge                              */
/*                                                           */
/*                                                           */
/*************************************************************/
static void belowModFunction(
  Environment *theEnv,
  struct fuzzy_value *fv)
  {
   int i, n, maxpos;
   double maxval;
  
   n = fv->n;

   /* get position where max value occurs */
   maxval = 0.0;
   maxpos = n-1;

   for (i=n-1; i>=0; i--)
     {
       if (fv->y[i] >= maxval)
         {
          maxpos = i;
          maxval = fv->y[i];
          if (maxval == 1.0) break;
         }
     }

   for (i=n-1; i>=0; i--)
     {
       if (i >= maxpos)
          fv->y[i] = 0.0;
       else
          fv->y[i] = 1.0 - fv->y[i];

     }
  }

/*************************************************************/ 
/* slightlyModFunction:                                      */ 
/*                                                           */
/* implements the 'slightly' hedge                           */
/*                                                           */
/*   intensify [ norm ( plus A AND not very A ) ]            */
/*                                                           */
/*   where A is the fuzzy set                                */ 
/*                                                           */
/*************************************************************/
static void slightlyModFunction(
  Environment *theEnv,
  struct fuzzy_value *fv)
  {
   struct fuzzy_value *tmpfv;
   struct fuzzy_value *andfv;

   /* get not very A */
   tmpfv = CopyFuzzyValue(theEnv,fv);
   veryModFunction(theEnv,tmpfv);

   /* get the complement (not) of very A */
   notModFunction(theEnv,tmpfv);

   /* get plus A */
   plusModFunction(theEnv,fv);

   /* perform the intersection (AND) of the 2 sets */
   andfv = fintersect(theEnv,fv, tmpfv);

   /* normalize the AND result */
   normModFunction(theEnv,andfv);

   /* finally do the 'int' of the normalized result */
   intensifyModFunction(theEnv,andfv);

   /* NOTE WELL:  the result is expected to be pointed to by the
      fv pointer so we must carefully remove unecessary stuff
      and make this so.
   */
   rtnFuzzyValue(theEnv,tmpfv);
   FrtnArray(theEnv,fv->x, fv->maxn);
   FrtnArray(theEnv,fv->y, fv->maxn);

   fv->n =    andfv->n;
   fv->maxn = andfv->maxn;
   fv->x =    andfv->x;
   fv->y =    andfv->y;
   if (andfv->name != NULL) rm(theEnv,andfv->name, strlen(andfv->name)+1);
   rtn_struct(theEnv,fuzzy_value,andfv);
  }

/*******************************************************************
  support routines for the modifier (hedge) functions follow
 ******************************************************************/

/********************************************************************
    Yexpand_set(fv)

    expands a fuzzy value with more points interpolated as necessary
    between segment endpoints such that the maximum difference in
    height between points in the new set is "YSPACING"
    
    NOTE: replaces the x and y arrays - does NOT create a new fuzzy value

    called before modifyExapndedFuzzyValue() in order to maintain 
    accuracy of f(x) ... for example if we have a fuzzy set that 
    consists of the 3 points
    
        (0,0) (5, 1) (10,0)
        
    and we apply the sqrt function to this fuzzy set then we get
    the same 3 points again (since we are storing pts and assuming
    that they are connected by a straight line). To get a better
    description of the function we want we need to make sure there
    are some other points between the extreme values of 0 and 1
    for the membership value.
 ********************************************************************/
 

/* YSPACING used to determine max Y difference between points when interpolating */
#define YSPACING 0.1

static void Yexpand_set(
  Environment *theEnv,
  struct fuzzy_value *fv)
  {
   int i, j, count;
   int num=fv->n;
   int newmaxn;
   int subdiv;
   double xdelta, ydelta, rise, run;
   double lastx, lasty, nextx, nexty, newx, newy;
   double *newxArray, *newyArray;

   newmaxn = (int) (num + ( floor(1.0/YSPACING) - 1 ) * ( num - 1 ));
   newxArray = FgetArray ( theEnv,newmaxn );
   newyArray = FgetArray ( theEnv,newmaxn );
    
   newxArray[0] = fv->x[0];
   newyArray[0] = fv->y[0];
    
   count = 1;
    
   lastx = fv->x[0];
   lasty = fv->y[0];
    
   for ( i = 1; i < num; i++ )
     {
      nextx = fv->x[i];
      nexty = fv->y[i];

      /* if the x values of the last and current (next) point are
         equal then this is a vertical line -- no need to expand
         the segment
      */
      if (lastx != nextx)
        {
         rise = fabs ( nexty - lasty );
         /* note: ceil will move 1.0000001 to 2 -- so subtract a
                  small value to prevent very close values from
                  going up
         */
         subdiv = (int) ceil ( rise / YSPACING - FUZZY_TOLERANCE);
      
         if (subdiv > 1)
           {
            run = nextx - lastx;
            xdelta = run / subdiv;
            ydelta = rise / subdiv;
            newx = lastx;
            newy = lasty;
            if ( nexty < lasty )
              { ydelta = -ydelta; }
                
            for ( j = 1; j < subdiv; j++ )
              {
               newx += xdelta;
               newy += ydelta;
               newxArray[count] = newx;
               newyArray[count] = newy;
               count++;
              }
           }
        }
      lastx = newxArray[count] = nextx;
      lasty = newyArray[count] = nexty;
      count++;
     }
    
   FrtnArray(theEnv,fv->x, fv->maxn);
   FrtnArray(theEnv,fv->y, fv->maxn);
   fv->n = count;
   fv->maxn = newmaxn;
   fv->x = newxArray;
   fv->y = newyArray;
    
   /* does compacting in place (ie. no copy) */
   CompactFuzzyValue(theEnv,fv);
  }

/*************************************************************/ 
/* executeModifyFunction:                                    */ 
/*                                                           */
/* executes a modify function to change a fuzzy value        */
/*************************************************************/
void executeModifyFunction(
  Environment *theEnv,
  struct fuzzy_value *fvptr,
  struct modifierListItem *modListItem)
  {
   /* NOTE: only 1 of the 3 values (modfunc, modClipsfunc, or modDeffunc)
      will have a non NULL value ... this will determine the type of
      call that must be made to perform the modifier function

      modfunc  - indicates an internal function defined to accept the
                 fuzzy value and it operates on the fuzzy value
                 (e.g.  veryModFunction as defined avove)
      modClipsfunc - indicates a CLIPS function that has been associated
                 with a modifier (with the add-fuzzy-modifier function); this
                 will be handled by the GenericModFunction
      modDeffunc - indicates a user defined deffunction that has been associated
                 with a modifier (with the add-fuzzy-modifier function); this
                 will be handled by the GenericModFunction
   */

   if (modListItem->modfunc != NULL)
     { (*modListItem->modfunc)(theEnv,fvptr); }
   else
     {
      GenericModFunction(theEnv,fvptr, modListItem->modClipsfunc
#if DEFFUNCTION_CONSTRUCT
                         , modListItem->modDeffunc
#endif
                        );
     }
  }

/*************************************************************/ 
/* FindModifier:                                             */ 
/*                                                           */
/* finds the named modifier in the list of modifiers         */
/*************************************************************/
struct modifierListItem *FindModifier(
  Environment *theEnv,
  const char *modname)
  {
   struct modifierListItem *currentPtr;
   char *modLowerName;
   size_t i, len;

   len = strlen(modname);

   modLowerName = (char *) gm2(theEnv,sizeof(char) * len + 1);

   for (i=0; i<=len; i++)
        modLowerName[i] = (isalpha(modname[i])) ? tolower(modname[i]) :
                                                 modname[i];

   currentPtr = FuzzyData(theEnv)->ListOfModifierFunctions;

   while (currentPtr != NULL)
     {
      if (strcmp(modLowerName,currentPtr->name) == 0)
        { break; }
      currentPtr = currentPtr->next;
     }

   rm(theEnv,modLowerName, len+1);
   return(currentPtr);
  }

/*************************************************************/ 
/* AddFuzzyModifier:                                         */ 
/*                                                           */
/* Adds the named modifier to the list of modifiers          */
/*                                                           */
/* Returns true if add was successful (could fail if trying  */
/* add the same modifier as is already there).               */
/*                                                           */
/*************************************************************/
bool AddFuzzyModifier(
  Environment *theEnv,
  const char *modname,
  void (*modfun)(Environment *,struct fuzzy_value *fv),
#if DEFFUNCTION_CONSTRUCT
  FunctionDefinition *modClipsfun,
  Deffunction *modDeffun)
#else
  FunctionDefinition *modClipsfun)
#endif
  {
   struct modifierListItem *newPtr, *currentPtr, *lastPtr = NULL;
   int compare;
   char *theName;

   newPtr = get_struct(theEnv,modifierListItem);

   newPtr->modfunc = modfun;
   newPtr->modClipsfunc = modClipsfun;
#if DEFFUNCTION_CONSTRUCT
   newPtr->modDeffunc = modDeffun;
#endif    
   newPtr->next = NULL;

   currentPtr = FuzzyData(theEnv)->ListOfModifierFunctions;
   while (currentPtr != NULL)
     {
      compare = strcmp(modname, currentPtr->name);
      if (compare == 0)
         { /* error -- trying to add same modifier function as is alreadty there! */
           WriteString(theEnv,STDERR,"add-fuzzy-modifier (modifier of same name already added\n");
           rtn_struct(theEnv,modifierListItem, newPtr);
           return(false);
         }

      if (compare < 0) break;

      lastPtr = currentPtr;
      currentPtr = currentPtr->next;
     }

   if (lastPtr == NULL)
     {
      newPtr->next = FuzzyData(theEnv)->ListOfModifierFunctions;
      FuzzyData(theEnv)->ListOfModifierFunctions = newPtr;
     }
   else
     {
      newPtr->next = currentPtr;
      lastPtr->next = newPtr;
     }
     
   /* store the name of the modifier -- must copy the string! */
   theName = (char *) gm2(theEnv,(int)strlen(modname)+1);
   strcpy(theName, modname);
   newPtr->name = theName;

#if DEFFUNCTION_CONSTRUCT
   if (modDeffun != NULL)
       (*EvaluationData(theEnv)->PrimitivesArray[PCALL]->incrementBusyCount)(theEnv,(void *)modDeffun);
#endif

   return(true);
  }

/*************************************************************/ 
/* RemoveFuzzyModifier:                                      */ 
/*                                                           */
/* Removes the named modifier from the list of modifiers     */
/* (note: cannot remove a system supplied modifier)          */
/*                                                           */
/*************************************************************/
void RemoveFuzzyModifier(
  Environment *theEnv,
  const char *modname)
  {
   struct modifierListItem *currentPtr, *lastPtr;

   lastPtr = NULL;
   currentPtr = FuzzyData(theEnv)->ListOfModifierFunctions;

   while (currentPtr != NULL)
     {
      if (currentPtr->modfunc == NULL) /* can't remove system modifiers */
        {
         if (strcmp(modname,currentPtr->name) == 0)
           {
            if (lastPtr == NULL)
              { FuzzyData(theEnv)->ListOfModifierFunctions = currentPtr->next; }
            else
              { lastPtr->next = currentPtr->next; }
            
#if DEFFUNCTION_CONSTRUCT
            if (currentPtr->modDeffunc != NULL)
               (*EvaluationData(theEnv)->PrimitivesArray[PCALL]->decrementBusyCount)(theEnv,(void *)currentPtr->modDeffunc);
#endif
            rm(theEnv,(void *)currentPtr->name, (int)strlen(currentPtr->name)+1);
            rtn_struct(theEnv,modifierListItem,currentPtr);
            return;
           }
        }
        
      lastPtr = currentPtr;
      currentPtr = currentPtr->next;
     }
  }

/*************************************************************/ 
/* ClearModifiers:                                           */ 
/*                                                           */
/* Removes all modifiers from the list of modifiers          */
/* (note: cannot remove a system supplied modifiers)         */
/*                                                           */
/*************************************************************/
static void ClearModifiers(
  Environment *theEnv,
  void *context)
  {
   struct modifierListItem *currentPtr, *lastPtr;

   lastPtr = NULL;
   currentPtr = FuzzyData(theEnv)->ListOfModifierFunctions;

   while (currentPtr != NULL)
     {
      if (currentPtr->modfunc == NULL) /* can't remove system modifiers */
        {
            if (lastPtr == NULL)
              { FuzzyData(theEnv)->ListOfModifierFunctions = currentPtr->next; }
            else
              { lastPtr->next = currentPtr->next; }
            
#if DEFFUNCTION_CONSTRUCT
            if (currentPtr->modDeffunc != NULL)
               (*EvaluationData(theEnv)->PrimitivesArray[PCALL]->decrementBusyCount)(theEnv,(void *)currentPtr->modDeffunc);
#endif
            rm(theEnv,(void *)currentPtr->name, (int)strlen(currentPtr->name)+1);
            rtn_struct(theEnv,modifierListItem,currentPtr);
            return;
         }
        
      lastPtr = currentPtr;
      currentPtr = currentPtr->next;
     }

}

#endif
