   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*             CLIPS Version 6.00  05/12/93            */
   /*                                                     */
   /*             FUZZY REASONING COMMANDS MODULE         */
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


/******************************************************************
    Fuzzy Logic Module

    This file contains the following categories of functions:
    
    Fuzzy reasoning related commands (functions)

 ******************************************************************/
 
#define _FUZZYCOM_SOURCE_


#include "setup.h"
#include "math.h"


#if FUZZY_DEFTEMPLATES

#include <stdio.h>
#define _CLIPS_STDIO_

#include <string.h>

#include "extnfunc.h"
#include "evaluatn.h"
#include "router.h"
#include "prntutil.h"
#include "argacces.h"
#include "memalloc.h"
#include "router.h"
#include "factrhs.h"
#include "factmngr.h"
#include "symbol.h"
#include "modulutl.h"
#include "pprint.h"

#include "fuzzyval.h"
#include "fuzzylv.h"
#include "fuzzycom.h"
#include "fuzzymod.h"
#include "fuzzydef.h"
#include "fuzzypsr.h"
#include "fuzzyrhs.h"
#include "fuzzymod.h"
#include "fuzzyutl.h"

#define ONE_THIRD    (1.0/3.0)
#define TWO_THIRDS   (2.0/3.0)

/******************************************************************
    Global Internal Function Declarations
    
    Defined in fuzzycom.h
 ******************************************************************/



/******************************************************************
    Local Internal Function Declarations
 ******************************************************************/
 
    static struct fuzzyLv *getFuzzyUniversePtr(Environment *theEnv,UDFValue *theResult,const char *functionName, int argn);
    static struct fuzzy_value *getFuzzyValuePtr(Environment *theEnv,UDFValue *theResult, const char *functionName, int argn);
    static char          *u_to_string (Environment *theEnv, struct fuzzyLv *up, size_t *length );
    static double         moment_defuzzification(Environment *,struct fuzzy_value *fvPtr );
    static void           get_moment_and_area ( double *moment_ptr, double *area_ptr, double x1,
                                                double y1, double x2, double y2 );
    static double         maximum_defuzzification(Environment *, struct fuzzy_value *fvPtr );
    static char          *fv_to_string (Environment *theEnv, struct fuzzy_value *fv_ptr, size_t *length_ptr );
    static struct expr   *CreateFuzzyValueParse( Environment *theEnv, struct expr *top, const char *logName );
    static struct fuzzy_value *seeIfFuzzyFact(Environment *theEnv,UDFValue *theResult,const char *functionName,int argn);

/******************************************************************
    Local Internal Variable Declarations
 ******************************************************************/
   
   
/******************************************************************
    Global External Variable Declarations
 ******************************************************************/



/******************************************************************
    Global Internal Variable Declarations
 ******************************************************************/


/*************************************************************

        Fuzzy Commands
        
    - initialize the commands related to fuzzy reasoning 

**************************************************************/

void DeffuzzyCommands(
  Environment *theEnv)
{

#if ! RUN_TIME
 
 AddUDF(theEnv,"moment-defuzzify","d",1,1,"flz",moment_defuzzify, "moment_defuzzify", NULL); // √
 AddUDF(theEnv,"maximum-defuzzify","d",1,1,"flz", maximum_defuzzify, "maximum_defuzzify", NULL); // √
 AddUDF(theEnv,"get-u", "y", 1,1,"flzy",  getu, "getu", NULL);  // √ TBD
 AddUDF(theEnv,"get-u-from", "d", 1,1,"flzy",  getu_from, "getu_from", NULL);  // √
 AddUDF(theEnv,"get-u-to", "d", 1,1,"flzy",  getu_to, "getu_to", NULL);  // √
 AddUDF(theEnv,"get-u-units", "y",1,1,"flzy",   getu_units, "getu_units", NULL);  // √
 AddUDF(theEnv,"get-fs", "y",  1,1,"flz", get_fs, "get_fs", NULL); // √
 AddUDF(theEnv,"get-fs-lv", "y",1,1,"flz",   get_fs_lv, "get_fs_lv", NULL); // √
 AddUDF(theEnv,"get-fs-template", "y", 1,1,"flz",  get_fs_template, "get_fs_template", NULL); // √
 AddUDF(theEnv,"get-fs-value", "d", 2,2,";flz;dl", get_fs_value, "get_fs_value", NULL); // √
 AddUDF(theEnv,"get-fs-length", "l",1,1,"flz",   get_fs_length, "get_fs_length", NULL); // √
 AddUDF(theEnv,"get-fs-x", "d",2,2,";flz;ld",  get_fs_x, "get_fs_x", NULL); // √
 AddUDF(theEnv,"get-fs-y", "d", 2,2,";flz;ld", get_fs_y, "get_fs_y", NULL); // √
 AddUDF(theEnv,"add-fuzzy-modifier", "v", 2,2,"y;y", add_fuzzy_modifier, // <-
                 "add_fuzzy_modifier", NULL);
 AddUDF(theEnv,"remove-fuzzy-modifier", "v", 1,1,"y", remove_fuzzy_modifier,
                 "remove_fuzzy_modifier", NULL);
 AddUDF(theEnv,"set-fuzzy-inference-type", "v", 1,1,"y", set_fuzzy_inference_type, // √
                 "set_fuzzy_inference_type", NULL);
 AddUDF(theEnv,"get-fuzzy-inference-type", "y", 0,0,NULL, get_fuzzy_inference_type, // √
                 "get_fuzzy_inference_type", NULL);
 AddUDF(theEnv,"set-fuzzy-display-precision", "v", 1,1,"l", set_fuzzy_display_precision, // √
                 "set_fuzzy_display_precision", NULL);
 AddUDF(theEnv,"get-fuzzy-display-precision", "l", 0,0,NULL, get_fuzzy_display_precision, // √
                 "get_fuzzy_display_precision", NULL);
 AddUDF(theEnv,"plot-fuzzy-value", "v",5,UNBOUNDED,"flz;ldsyn;ys;dly;dly;flz" , plot_fuzzy_value,
                 "plot_fuzzy_value", NULL); // √
 AddUDF(theEnv,"set-alpha-value", "v", 1,1,"ld", set_alpha_value, "set_alpha_value",NULL); // √
 AddUDF(theEnv,"get-alpha-value", "d", 0,0,NULL ,get_alpha_value, "get_alpha_value", NULL); // √
 AddUDF(theEnv,"get-fuzzy-slot", "z", 1,2,";fl;y", get_fuzzy_slot, "get_fuzzy_slot", NULL); // √
 AddUDF(theEnv,"create-fuzzy-value", "z", 1,1,"z", create_fuzzy_value, "create_fuzzy_value", NULL); // √
 AddUDF(theEnv,"fuzzy-union", "z", 2,2,"z" ,fuzzy_union, "fuzzy_union", NULL); // √
 AddUDF(theEnv,"fuzzy-intersection", "z", 2,2,"z", fuzzy_intersection, "fuzzy_intersection", NULL); // √
 AddUDF(theEnv,"fuzzy-modify", "z", 2,2,";z;y", fuzzy_modify, "fuzzy_modify", NULL); // √
 AddUDF(theEnv,"is-defuzzify-value-valid", "b",0,0,NULL, is_defuzzify_value_valid, "is_defuzzify_value_valid", NULL); // √


 /* Special parser for the create-fuzzy-value function -- needs to handle
    linguistic expressions and standard or singleton fuzzy set expressions
 */
 AddFunctionParser(theEnv,"create-fuzzy-value", CreateFuzzyValueParse);

#endif

}

/****************************************/
/* is_defuzzify_value_valid:            */
/****************************************/
void is_defuzzify_value_valid(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   if (FuzzyData(theEnv)->is_last_defuzzify_valid)
     { returnValue->lexemeValue = TrueSymbol(theEnv); }
   else
     { returnValue->lexemeValue = FalseSymbol(theEnv); }
  }

/********************************************************************
    Functions to access the universe of discourse 
 ********************************************************************/                                                                    


/************************************************************
    seeIfFuzzyFact():                                            

    given a ptr to a DATA_OBJECT of type integer or fact-address
    see if it is designates a fuzzy fact or not
    
    returns a ptr to the fuzzy value in the fuzzy fact 
    or NULL if error occurred
************************************************************/
static struct fuzzy_value *seeIfFuzzyFact( 
  Environment *theEnv,
  UDFValue *theResult,
  const char *functionName,
  int argn)
  {
   long long factIndex;
   bool found_fact;
   Fact *factPtr;
   char tempBuffer[100];
   CLIPSValue *fieldPtr;

   if (theResult->header->type == INTEGER_TYPE)
     { 
      factIndex =  theResult->integerValue->contents;
      if (factIndex < 0)
        {
         ExpectedTypeError1(theEnv,functionName,argn,"fact-index must be positive");
         return NULL;
        }
      
      found_fact = false;
      factPtr = GetNextFact(theEnv,NULL);
      while (factPtr != NULL)
        {
         if (factPtr->factIndex == factIndex)
           {
            found_fact = true;
            break;
           }
         factPtr = factPtr->nextFact;
        }
       
      if (found_fact == false)
        {
         sprintf(tempBuffer,"f-%lld",factIndex);
         CantFindItemErrorMessage(theEnv,"fuzzy fact",tempBuffer,false);
         return(NULL);
        }
     }
   else
     { /* arg type is fact address */
      factPtr = theResult->factValue;
     }

    /* must check with (factPtr->whichDeftemplate->fuzzyTemplate != NULL) */
   if (factPtr->whichDeftemplate->fuzzyTemplate != NULL)
     {
      fieldPtr = &(factPtr->theProposition.contents[0]);
      return fieldPtr->fuzzyValue->contents;
     }
   else
     {
      sprintf(tempBuffer,"f-%lld (or NOT a fuzzy fact)",factPtr->factIndex);
      CantFindItemErrorMessage(theEnv,"fact",tempBuffer,false);
      return NULL;
     }
  }

/************************************************************
    getFuzzyValuePtr():                                            

    given a ptr to an argument of a function that is expected
    to be a fact address or a fact index get a ptr to the fact
    or a FuzzyValue
    
    returns a ptr to a fact or NULL if error occurred
************************************************************/
static struct fuzzy_value *getFuzzyValuePtr(
  Environment *theEnv,
  UDFValue *theResult,
  const char *functionName,
  int argn)
  {
   if ((theResult->header->type == INTEGER_TYPE) ||
       (theResult->header->type == FACT_ADDRESS_TYPE))
     { return seeIfFuzzyFact(theEnv,theResult,functionName,argn); }
   else if (theResult->header->type == FUZZY_VALUE_TYPE)
     { return theResult->fuzzyValue->contents; }
   
   ExpectedTypeError1(theEnv,functionName,argn,"fact-index/fact-address of fuzzy fact or FUZZY_VALUE");
   
   return NULL;
  }

/************************************************************
    get_fs_template():                                   

    given a ptr to an argument of a function that is expected
    to be a fact address or a fact index or a fuzzy value
    get the name of the fuzzy deftemplate associated with
    the fuzzy value
    
************************************************************/
void get_fs_template(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   struct fuzzy_value *fvPtr;
   UDFValue theArgument;
  
   if (! UDFFirstArgument(context,INTEGER_BIT | FACT_ADDRESS_BIT | FUZZY_BIT,&theArgument))
     {
      returnValue->lexemeValue = CreateSymbol(theEnv,"");
      return;
     }

   fvPtr = getFuzzyValuePtr(theEnv,&theArgument,"get-fs-template",1);
         
   if (fvPtr != NULL && fvPtr->whichDeftemplate != NULL)
     {
      returnValue->lexemeValue = fvPtr->whichDeftemplate->header.name;
      return;
     }
      
   SetEvaluationError(theEnv,true);
   returnValue->lexemeValue = CreateSymbol(theEnv,"");
  }

/************************************************************
    getFuzzyUniversePtr():                                   

    given a ptr to an argument of a function that is expected
    to be a fact address or a fact index or the name of a fuzzy
    deftemplate get a ptr to the universe of discourse (in the
    fuzzyLv structure)
    
    returns a ptr to the fuzzyLv struct  or NULL if an 
    error occurred
************************************************************/
static struct fuzzyLv *getFuzzyUniversePtr(
  Environment *theEnv,
  UDFValue *theResult,
  const char *functionName,
  int argn)
  {
   struct fuzzy_value *fvPtr;
   Deftemplate *theDeftemplate;
         
   if ((theResult->header->type == INTEGER_TYPE) ||
       (theResult->header->type == FACT_ADDRESS_TYPE))
     { /* Fuzzy fact being referenced */
       fvPtr = seeIfFuzzyFact(theEnv,theResult, functionName, argn);
       if (fvPtr == NULL)
         return NULL;
       else
         return fvPtr->whichDeftemplate->fuzzyTemplate;
     }
   else if (theResult->header->type == SYMBOL_TYPE)
     { /* fuzzy deftemplate being referenced */
      const char * theName;
      unsigned int count;

      theName = theResult->lexemeValue->contents;
      if (FindModuleSeparator(theName))
        {
         theDeftemplate = FindDeftemplate(theEnv,theName);
        }
      else
        {
         theDeftemplate = (Deftemplate *)
                 FindImportedConstruct(theEnv,"deftemplate",NULL,theName,
                                        &count,true,NULL);
         if (count > 1)
           {
            AmbiguousReferenceErrorMessage(theEnv,"deftemplate",theName);
            return(NULL);
           }
        }

      if (theDeftemplate != NULL)
        {
         if (theDeftemplate->fuzzyTemplate != NULL)
           { return( theDeftemplate->fuzzyTemplate ); }
         else
	       {
            CantFindItemErrorMessage(theEnv,"fuzzy fact", theName,true);
            return NULL;
           }
        }
     }
   else if (theResult->header->type == FUZZY_VALUE_TYPE)
     {
      fvPtr = theResult->fuzzyValue->contents;
      return fvPtr->whichDeftemplate->fuzzyTemplate;
     }   

   ExpectedTypeError1(theEnv,functionName,argn,"fact-index/fact-address of \nfuzzy fact, fuzzy deftemplate name, or FUZZY_VALUE\n(fuzzy deftemplate name may be out of scope)");
   return NULL;
  }

/************************************************************
    getu():                                            

    returns the universe limits and units of a single fact in
    SYMBOL format
************************************************************/
void getu(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   struct fuzzyLv *uPtr;
   UDFValue theArgument;
   char *ustring;
   size_t ulength = 0;

   if (! UDFFirstArgument(context,INTEGER_BIT | SYMBOL_BIT | FACT_ADDRESS_BIT | FUZZY_BIT,&theArgument))
     {
      returnValue->lexemeValue = CreateSymbol(theEnv,"");
      return;
     }

   uPtr = getFuzzyUniversePtr(theEnv,&theArgument, "get-u", 1);
       
   if (uPtr != NULL)
     {
      ustring = u_to_string(theEnv,uPtr,&ulength);
      returnValue->lexemeValue = CreateSymbol(theEnv,ustring);
      rm(theEnv,ustring, ulength + 1);
      return;
     }

   SetEvaluationError(theEnv,true);
   returnValue->lexemeValue = CreateSymbol(theEnv,"");
  }

/************************************************************
    u_to_string():                                            

    given a fuzzy universe pointer returns the universe limits 
    and units of a single fact as a string
    
************************************************************/
static char *u_to_string(
  Environment *theEnv,
  struct fuzzyLv *up,
  size_t *length_ptr)
  {
   char *string = NULL;
    
   if (up->units != NULL)
     {
      *length_ptr = 36 + strlen(up->units->contents);
      string = (char *) gm2(theEnv, (*length_ptr + 1));
      sprintf( string, "%.2f - %.2f %s", up->from, up->to, up->units->contents);
     }
   else    /* no units specified */
     {
      *length_ptr = 36;
      string = (char *) gm2(theEnv,(*length_ptr + 1));
      sprintf( string, "%.2f - %.2f", up->from, up->to);
     }

   return string;
  }

/************************************************************
    getu_from():                                            

    returns the universe minimum limit of a single fact in
    double format
************************************************************/
void getu_from(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   struct fuzzyLv *uPtr;
   UDFValue theArgument;

   if (! UDFFirstArgument(context,INTEGER_BIT | SYMBOL_BIT | FACT_ADDRESS_BIT | FUZZY_BIT,&theArgument))
     {
      returnValue->floatValue = CreateFloat(theEnv,0.0);
      return;
     }

   uPtr = getFuzzyUniversePtr(theEnv,&theArgument,"get-u-from",1);
       
   if (uPtr != NULL)
     { returnValue->floatValue = CreateFloat(theEnv,uPtr->from); }
   else
     {
      SetEvaluationError(theEnv,true);
      returnValue->floatValue = CreateFloat(theEnv,0.0);
     }
  }

 /************************************************************
    getu_to():                                            

    returns the universe maximum limit of a single fact in
    double format
************************************************************/
void getu_to(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   struct fuzzyLv *uPtr;
   UDFValue theArgument;

   if (! UDFFirstArgument(context,INTEGER_BIT | SYMBOL_BIT | FACT_ADDRESS_BIT | FUZZY_BIT,&theArgument))
     {
      returnValue->floatValue = CreateFloat(theEnv,0.0);
      return;
     }

   uPtr = getFuzzyUniversePtr(theEnv,&theArgument,"get-u-to",1);
       
   if (uPtr != NULL)
     { returnValue->floatValue = CreateFloat(theEnv,uPtr->to); }
   else
     {
      SetEvaluationError(theEnv,true);
      returnValue->floatValue = CreateFloat(theEnv,0.0);
     }
  }

/************************************************************
    getu_units():                                            

    returns the universe units of a single fact in
    word format

    returns "" if no units specified in deffuzzy
************************************************************/
void getu_units(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   struct fuzzyLv *uPtr;
   UDFValue theArgument;
  
   if (! UDFFirstArgument(context,INTEGER_BIT | SYMBOL_BIT | FACT_ADDRESS_BIT | FUZZY_BIT,&theArgument))
     {
      returnValue->lexemeValue = CreateSymbol(theEnv,"");
      return;
     }

   uPtr = getFuzzyUniversePtr(theEnv,&theArgument, "get-u-units", 1);
    
   if (uPtr != NULL)
     {
      if (uPtr->units != NULL)
        { returnValue->lexemeValue = uPtr->units; }
      else
        { returnValue->lexemeValue = CreateSymbol(theEnv,""); }
     }
   else
     {
      SetEvaluationError(theEnv,true);
      returnValue->lexemeValue = CreateSymbol(theEnv,"");
     }
  }

/**********************************************************************
    FUNCTIONS TO ACCESS FUZZY SET ELEMENTS
 **********************************************************************/
 
 /************************************************************
    get_fs():                                            

    returns the fuzzy set array of a fuzzy fact or a fuzzy
    value in string format

    if the fact is not fuzzy, a warning is printed and 
    the word "" is returned.
************************************************************/
void get_fs(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   struct fuzzy_value *fvPtr;
   UDFValue theArgument;
   char *ustring;
   size_t ulength = 0;

   if (! UDFFirstArgument(context,INTEGER_BIT | FACT_ADDRESS_BIT | FUZZY_BIT,&theArgument))
     {
      returnValue->lexemeValue = CreateSymbol(theEnv,"");
      return;
     }
     
   fvPtr = getFuzzyValuePtr(theEnv,&theArgument,"get-fs",1);
       
   if (fvPtr != NULL)
     {
      ustring = fv_to_string(theEnv,fvPtr,&ulength);
      returnValue->lexemeValue = CreateSymbol(theEnv,ustring);
      rm(theEnv,ustring, ulength + 1);
     }
   else
     {
      SetEvaluationError(theEnv,true);
      returnValue->lexemeValue = CreateSymbol(theEnv,"");
     }
  }

 /************************************************************
    get_fs_lv():                                            

    returns the fuzzy set's linguistic value
    value in string format

    if the fact is not fuzzy, a warning is printed and 
    the word "" is returned.
************************************************************/
void get_fs_lv(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   struct fuzzy_value *fvPtr;
   UDFValue theArgument;

   if (! UDFFirstArgument(context,INTEGER_BIT | FACT_ADDRESS_BIT | FUZZY_BIT,&theArgument))
     {
      returnValue->lexemeValue = CreateSymbol(theEnv,"");
      return;
     }

   fvPtr = getFuzzyValuePtr(theEnv,&theArgument, "get-fs-lv", 1);
       
   if (fvPtr != NULL && fvPtr->name != NULL)
     { returnValue->lexemeValue = CreateSymbol(theEnv,fvPtr->name); }
   else
     {
      SetEvaluationError(theEnv,true);
      returnValue->lexemeValue = CreateSymbol(theEnv,"");
     }
  }

/************************************************************
    fv_to_string():                                            

    given a fuzzy fact pointer returns the fuzzy set of 
    a single fact as a string
    
************************************************************/
static char *fv_to_string(
  Environment *theEnv,
  struct fuzzy_value *fv_ptr,
  size_t *length_ptr)
  {
   int i, num;
   char *string, *strindex;

   num = fv_ptr->n;
    
   /* NOTE: We should really use a MAXDIGITS that tells max float digits */
   /* allows for 10 digits in front of decimal point plus space for ... to
      indicate that space exceeded and not all numbers printed */
       
   *length_ptr = num*33 + 50;

   string = (char *) gm2(theEnv,*length_ptr + 1);
   strindex = string;
    
   for (i=0; i < num; i++)
     {
      if (((size_t) (strindex-string)) > *length_ptr-50)
        { /* we are in the string safety zone -- stop before overrun space */
          /* perhaps we should do this in a better way -- get each pair and
             append them to each other as needed
          */
         strcpy(strindex, " ... ");
         WriteString(theEnv,STDERR,"Internal Problem *****\n");
         WriteString(theEnv,STDERR,"String space exceeded in fv_to_string (FUZZYCOM.C)\n");
         break;
        }
             
      sprintf(strindex, "(%.3f, %.3f) ", fv_ptr->x[i], fv_ptr->y[i]);
      strindex = string + strlen(string);
     }
         
   return(string);
  }

 /************************************************************
    get_fs_length():                                            

    returns the size of a fuzzy set array in
    INTEGER format

    if the fact is not fuzzy, a warning is printed and 
    a value of 0 is returned.
************************************************************/
void get_fs_length(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   UDFValue theArgument;
   struct fuzzy_value *fv_ptr;

   if (! UDFFirstArgument(context,INTEGER_BIT | FACT_ADDRESS_BIT | FUZZY_BIT,&theArgument))
     {
      returnValue->integerValue = CreateInteger(theEnv,0);
      return;
     }
     
   fv_ptr = getFuzzyValuePtr(theEnv,&theArgument, "get-fs-length", 1);
       
   if (fv_ptr != NULL)
     { returnValue->integerValue = CreateInteger(theEnv,fv_ptr->n); }
   else
     {
      SetEvaluationError(theEnv,true);
      returnValue->integerValue = CreateInteger(theEnv,0);
     }
  }

/************************************************************************
    get_fs_value()

    syntax: (get-fs-value <?var> <numeric expression>)

    Returns the y value at the x values specified by the numeric expr.
    If the expression evaluates to a number which is out of range of the
    universe of discourse, the value 0.0 is returned.

 ************************************************************************/
void get_fs_value(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   UDFValue theArgument;
   struct fuzzy_value *fv_ptr;
   UDFValue result;
   double xvalue, yvalue;
   double from, to;
   int n, i;
   double lastx = 0, lasty, currentx = 0, currenty;
      
   if (! UDFFirstArgument(context,INTEGER_BIT | FACT_ADDRESS_BIT | FUZZY_BIT,&theArgument))
     {
      returnValue->floatValue = CreateFloat(theEnv,0.0);
      return;
     }

   fv_ptr = getFuzzyValuePtr(theEnv,&theArgument, "(get-fs-value", 1);
      
   if (fv_ptr == NULL)
     {
      SetEvaluationError(theEnv,true);
      returnValue->floatValue = CreateFloat(theEnv,0.0);
      return;
     }
   
   if (! UDFNextArgument(context,NUMBER_BITS,&result))
     {
      SetEvaluationError(theEnv,true);
      returnValue->floatValue = CreateFloat(theEnv,0.0);
      return;
     }

   from = fv_ptr->whichDeftemplate->fuzzyTemplate->from;
   to = fv_ptr->whichDeftemplate->fuzzyTemplate->to;

   if (result.header->type == INTEGER_TYPE)
     { xvalue = (double) result.integerValue->contents; }
   else
     { xvalue = result.floatValue->contents; }
                 
   if (xvalue < from || xvalue > to)
     { /* error - out of range */
      WriteString(theEnv,STDERR,"Function get-fs-value, x value out of range\n");
      SetEvaluationError(theEnv,true);
      returnValue->floatValue = CreateFloat(theEnv,0.0);
      return;
     }

   /* find the y value that corresponds to the given x value */
   n = fv_ptr->n;

   if (n == 1)
     {
      returnValue->floatValue = CreateFloat(theEnv,fv_ptr->y[0]);
      return;
     }
   
   /* find the correct x points that span the required x position */

   if (xvalue < fv_ptr->x[0]) /* before 1st point */
     {
      returnValue->floatValue = CreateFloat(theEnv,fv_ptr->y[0]);
      return;
     }
   
   if (xvalue > fv_ptr->x[n-1]) /* after last point */
     {
      returnValue->floatValue = CreateFloat(theEnv,fv_ptr->y[n-1]);
      return;
     }
     
   i = 1;
                       
   while (i < n)
     {
      lastx = fv_ptr->x[i-1];
      currentx = fv_ptr->x[i];
      if (xvalue >= lastx && xvalue <= currentx)
        break;
      i++;
     }
                            
   if (i == n)  /* major problem -- didn't find it! */
     {
      WriteString(theEnv,STDERR,"Function get-fs-value, internal problem, see system manager\n");
      SetEvaluationError(theEnv,true);
      returnValue->floatValue = CreateFloat(theEnv,0.0);
      return;
     }

   /* if either x value is the same as the one we are looking
      for then scan from that point on looking for the
      possibility of multiple points with the same x value
      and return the max of these values
   */
            
   if (lastx == xvalue || currentx == xvalue)
     {
      int k;
                               
      if (lastx == xvalue)
        { k = i-1; }
      else
        { k = i; }
                       
      yvalue = fv_ptr->y[k++];
      while (k < n && fv_ptr->x[k] == xvalue)
        {
         double tmp = fv_ptr->y[k];
         if (tmp > yvalue)
           { yvalue = tmp; }
         k++;
        }
     }
   else
     {
      lasty = fv_ptr->y[i-1];
      currenty = fv_ptr->y[i];
      if (lastx == currentx)
        { yvalue = lasty >= currenty ? lasty : currenty; }
      else
        { yvalue = lasty+(currenty-lasty)*(xvalue-lastx)/(currentx-lastx); }
     }

   returnValue->floatValue = CreateFloat(theEnv,yvalue);
  }

/************************************************************************
    get_fs_x()

    syntax: (get-fs-x <?var> <integer expression>)

    Returns the x value of the ith point in the fuzzy set array.
    If the expression evaluates to a number which is not an integer
    value, the expression result is truncated to an integer.
 ************************************************************************/
void get_fs_x(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   UDFValue theArgument;
   struct fuzzy_value *fv_ptr;
   UDFValue result;
   long long index;
   
   if (! UDFFirstArgument(context,INTEGER_BIT | FACT_ADDRESS_BIT | FUZZY_BIT,&theArgument))
     {
      returnValue->floatValue = CreateFloat(theEnv,0.0);
      return;
     }

   fv_ptr = getFuzzyValuePtr(theEnv,&theArgument,"get-fs-x",1);
       
   if (fv_ptr == NULL)
     {
      SetEvaluationError(theEnv,true);
      returnValue->floatValue = CreateFloat(theEnv,0.0);
      return;
     }

   if (! UDFNextArgument(context,NUMBER_BITS,&result))
     {
      SetEvaluationError(theEnv,true);
      returnValue->floatValue = CreateFloat(theEnv,0.0);
      return;
     }

   if (result.header->type == INTEGER_TYPE)
     { index = result.integerValue->contents; }
   else
     { index = (long long) result.floatValue->contents; }

   if (index < 0 || index >= fv_ptr->n)
     { /* error - out of range */
      WriteString(theEnv,STDERR,"Function get-fs-x, index out of range\n");
      SetEvaluationError(theEnv,true);
      returnValue->floatValue = CreateFloat(theEnv,0.0);
     }
   else
     { returnValue->floatValue = CreateFloat(theEnv,fv_ptr->x[index]); }
  }

/************************************************************************
    get_fs_y()

    syntax: (get-fs-y <?var> <integer expression>)

    Returns the y value of the ith point in the fuzzy set array.
    If the expression evaluates to a number which is not an integer
    value, the expression result is truncated to an integer.
 ************************************************************************/
void get_fs_y(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   UDFValue theArgument;
   struct fuzzy_value *fv_ptr;
   UDFValue result;
   long long index;
   
   if (! UDFFirstArgument(context,INTEGER_BIT | FACT_ADDRESS_BIT | FUZZY_BIT,&theArgument))
     {
      returnValue->floatValue = CreateFloat(theEnv,0.0);
      return;
     }

   fv_ptr = getFuzzyValuePtr(theEnv,&theArgument, "get-fs-y", 1);
      
   if (fv_ptr == NULL)
     {
      SetEvaluationError(theEnv,true);
      returnValue->floatValue = CreateFloat(theEnv,0.0);
      return;
     }
     
   if (! UDFNextArgument(context,NUMBER_BITS,&result))
     {
      SetEvaluationError(theEnv,true);
      returnValue->floatValue = CreateFloat(theEnv,0.0);
      return;
     }

   if (result.header->type == INTEGER_TYPE)
     { index = result.integerValue->contents; }
   else
     { index = (long long) result.floatValue->contents; }
                      
   if (index < 0 || index >= fv_ptr->n)
     { /* error - out of range */
      WriteString(theEnv,STDERR,"Function get-fs-y, index out of range\n");
      SetEvaluationError(theEnv,true);
      returnValue->floatValue = CreateFloat(theEnv,0.0);
     }
   else
     { returnValue->floatValue = CreateFloat(theEnv,fv_ptr->y[index]); }
  }

/***********************************************************************
    DEFUZZIFYING FUNCTIONS
 ***********************************************************************/

/************************************************************/
/* moment-defuzzify function:                               */
/************************************************************/
void moment_defuzzify(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   struct fuzzy_value *fv_ptr;
   UDFValue theArgument;
   
   if (! UDFFirstArgument(context,INTEGER_BIT | FACT_ADDRESS_BIT | FUZZY_BIT,&theArgument))
     {
      returnValue->floatValue = CreateFloat(theEnv,0.0);
      return;
     }
     
   fv_ptr = getFuzzyValuePtr(theEnv,&theArgument, "moment-defuzzify", 1);
       
   if (fv_ptr != NULL)
     { returnValue->floatValue = CreateFloat(theEnv,moment_defuzzification(theEnv,fv_ptr)); }
   else
     {
      SetEvaluationError(theEnv,true);
      returnValue->floatValue = CreateFloat(theEnv,0.0);
     }
  }

/**********************************************************************
    moment_defuzzification()

    Calculates the first moment of area of a fuzzy set about the
    y axis.  The set is subdivided into different shapes by partitioning
    vertically at each point in the set, resulting in rectangles, 
    triangles, and trapezoids.  The centre of gravity and area of
    each subdivision is calculated using the appropriate formulas
    for each shape.  The first moment of area of the whole set is
    then:

        sum of ( moment(i) * area(i) )          <-- top
        ------------------------------
            sum of (area(i))                    <-- bottom

    If the total area is 0 then use the mid-point of the universe.
    This is not too appropriate but we have to return something ... 
    could have had an error message display since it is like a
    division by zero.

 **********************************************************************/   
static double moment_defuzzification(
  Environment *theEnv,
  struct fuzzy_value *fv)  
  {
   int i,  num;
       
   double result, local_moment, local_area, xmin, xmax;
   double currentx, currenty, nextx, nexty;
   double top = 0.0, bottom = 0.0;
    
   double *fsx, *fsy; /* ptrs to fuzzy set x and y arrays */

   /**********************************************
    Start calculating the c.o.g.
    **********************************************/

   xmin = fv->whichDeftemplate->fuzzyTemplate->from;
   xmax = fv->whichDeftemplate->fuzzyTemplate->to;
    
   FuzzyData(theEnv)->is_last_defuzzify_valid = true;

   fsx = fv->x;
   fsy = fv->y;
   num = fv->n;

   if ( num <= 1 )  /* single point is constant fuzzy set over universe */
     {
      result = 0.5 * (xmax - xmin) + xmin;
      /* if no points in the set (should NEVER happen)
         or the y-value of the single point is 0
         then the area is zero -- invalid moment
      */
      if (num < 1 || fsy[0] == 0.0)
        { FuzzyData(theEnv)->is_last_defuzzify_valid = false; }
     }
   else
     {
      currentx = fsx[0];
      currenty = fsy[0];

      /* Check for open-ended set & add initial rectangle if needed */
      if ( currenty != 0.0 && currentx != xmin )
        {
         local_moment = 0.5 * (currentx+xmin);
         local_area = (currentx - xmin) * currenty;
         top += local_moment * local_area;
         bottom += local_area;
        }

      for ( i = 1; i < num; i++ )
        {
         nextx = fsx[i];
         nexty = fsy[i];

         get_moment_and_area ( &local_moment, &local_area, currentx, currenty, nextx, nexty );
         top += local_moment * local_area;
         bottom += local_area;

         currentx = nextx;
         currenty = nexty;
        }
    
      /* Check for open-ended set & add final rectangle if needed */
      if ( currenty != 0.0 && currentx < xmax )
        {
         local_moment = 0.5 * (currentx + xmax);
         local_area = (xmax - currentx) * currenty;
         top += local_moment * local_area;
         bottom += local_area;
        }
      
      /*************************************************************
       Calculate the final result but check for zero area set.
       **************************************************************/
      
      if ( bottom == 0.0 )
        {
         result = 0.5 * ( xmax - xmin ) + xmin;
         /* return a default but indicate that it was an invalid moment */
         FuzzyData(theEnv)->is_last_defuzzify_valid = false;
        }
      else
        { result = top/bottom; }
     }

   return result;
  }

/***********************************************************************
    get_moment_and_area(moment_ptr, area_ptr, x1, y1, x2, y2)

    Given a polygon defined by the vertices (x1, 0), (x1, y1),
    (x2, y2), (x2, 0), the first moment of area of the polygon is
    returned in moment_ptr, and the area of the polygon in area_ptr.

    Conditions: x2 > x1.
 ***********************************************************************/
static void get_moment_and_area(
  double *moment_ptr,
  double *area_ptr,
  double x1,
  double y1,
  double x2,
  double y2)
  {
   /* rectangle of zero height or zero width? */
   if (((y1 == 0.0) && (y2 == 0.0)) ||
       (x1 == x2))
     {
      *moment_ptr = 0.0;
      *area_ptr = 0.0;
     }
   else if ( y1 == y2 )            /* rectangle */
     {
      *moment_ptr = 0.5 * ( x1 + x2 );
      *area_ptr = ( x2 - x1 ) * y1;
     }
   else if ( y1 == 0.0 && y2 != 0.0 )    /* triangle, height y2 */
     {
      *moment_ptr = TWO_THIRDS * ( x2 - x1 ) + x1;
      *area_ptr = 0.5 * ( x2 - x1 ) * y2;
     }
   else if ( y2 == 0.0 && y1 != 0.0 )    /* triangle, height y1 */
     {
      *moment_ptr = ONE_THIRD * ( x2 - x1 ) + x1;
      *area_ptr = 0.5 * (x2 - x1 ) * y1;
     }
   else                     /* trapezoid */
     {
      *moment_ptr = ( TWO_THIRDS * (x2-x1) * (y2+0.5*y1))/(y1+y2) + x1;
      *area_ptr = 0.5 * ( x2 - x1 ) * ( y1 + y2 );
     }
  }

/************************************************************/
/* maximum-defuzzify function:                              */
/************************************************************/
void maximum_defuzzify(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   struct fuzzy_value *fvPtr;
   UDFValue theArgument;
   
   if (! UDFFirstArgument(context,INTEGER_BIT | FACT_ADDRESS_BIT | FUZZY_BIT,&theArgument))
     {
      returnValue->floatValue = CreateFloat(theEnv,0.0);
      return;
     }

   fvPtr = getFuzzyValuePtr(theEnv,&theArgument, "maximum-defuzzify", 1);
       
   if (fvPtr != NULL)
     { returnValue->floatValue = CreateFloat(theEnv,maximum_defuzzification(theEnv,fvPtr)); }
   else
     {
      SetEvaluationError(theEnv,true);
      returnValue->floatValue = CreateFloat(theEnv,0.0);
     }
  }

/********************************************************************
    maximum_defuzzification()

    finds the mean of maxima of a fuzzy set
    
    NOTE: This really doesn't work well because there can be x ranges
          where the y value is constant at the max value and other 
          places where the max is only reached for a single x value.
          When this happens the single value gets too much of a say
          in the defuzzified value.
          
              /------\                   /\
             /        \                 /  \
          --/          \---------------/    \-------------
                  ^       ^
                  |       |
                  |       | gives this as the mean of maximum
                  | this is more reasonable???
                  
         Centre of gravity is likely more useful
 ********************************************************************/
 
static double maximum_defuzzification(
  Environment *theEnv,
  struct fuzzy_value *fv)  
  {
   int i,  num, count;
       
   double result,  xmin, xmax;
   double maxy, sum;

   double *fsx, *fsy; /* ptrs to fuzzy set x and y arrays */

   /* always valid result for the maximun defuzzify */
   FuzzyData(theEnv)->is_last_defuzzify_valid = true;

   /******************************************
       Find Mean of Maxima
    ******************************************/

   maxy = 0.0;
   num = fv->n;
    
   fsx = fv->x;
   fsy = fv->y;
    
   for (i=0; i < num; i++ )
     {
      if (fsy[i] > maxy)
        { maxy = fsy[i]; }
     }

   xmin = fv->whichDeftemplate->fuzzyTemplate->from;
   xmax = fv->whichDeftemplate->fuzzyTemplate->to;
    
   count = 0;
   sum = 0.0;

   /* Check for a zero max value or a single point in the set */
   if (maxy == 0.0 || num == 1)
     { result = (xmax - xmin) * 0.5 + xmin; }
        
   else /* Set has at least two points */
     {
      /* check for maximum at beginning of open-ended set */
      if (fsy[0] == maxy)
        {
         sum += xmin;
         count++;
         if ((fsx[0] != xmin) && (fsy[1] != maxy))
           {
            sum += fsx[0];
            count++;
           }
        }
          
      for ( i = 1; i < num - 1; i++ )
        {
         if (fsy[i] == maxy)
           {
            if ((fsy[i-1] != maxy) || (fsy[i+1] != maxy))
              {
               sum += fsx[i];
               count++;
              }
            }
        }
          
      /* check for maximum at end of open-ended set */
      if (fsy[num-1] == maxy)
        {
         if ((fsx[num-1] != xmax) && (fsy[num-2] != maxy))
           {
            sum += fsx[num-1];
            count++;
           }
         sum += xmax;
         count++;
        }
        
      result = sum/count;
     }
    
   return result;
  }

/**********************************************************************
    FUNCTIONS TO ADD/REMOVE FUZZY MODIFIERS (HEDGES)
 **********************************************************************/
 
 /************************************************************
    add_fuzzy_modifier():                                            

    adds a new fuzzy modifier to the list of modifiers

************************************************************/
void add_fuzzy_modifier(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   const char *modname, *modfuncname;
   FunctionDefinition *fun = NULL;
   Deffunction *deffun = NULL;
   UDFValue theArgument;
   
    if (! UDFFirstArgument(context,SYMBOL_BIT,&theArgument))
     {
      return;
     }
    modname = theArgument.lexemeValue->contents;     /* get name of the modifier */
    
    if (! UDFNextArgument(context,SYMBOL_BIT,&theArgument))
     {
      return;
     }
    modfuncname = theArgument.lexemeValue->contents; /* get name of the modifier function */

#if DEFFUNCTION_CONSTRUCT
      deffun = (Deffunction *)LookupDeffunctionInScope(theEnv,modfuncname);
      
      if (deffun == NULL)
        {
#endif
         /* should be name of a function of 1 arg of type float, returns float */
         fun = FindFunction(theEnv,modfuncname);

          if (fun == NULL)
            {
              WriteString(theEnv,STDERR,"Function add-fuzzy-modifier (undefined modifier function)\n");
              return;
            }
          if ((fun->minArgs != 1) ||
              (fun->maxArgs != 1) ||
              (fun->restrictions == NULL) ||
              ((strcmp(fun->restrictions->contents,"d") != 0) &&
               (strcmp(fun->restrictions->contents,"l") != 0) &&
               (strcmp(fun->restrictions->contents,"ld") != 0) &&
               (strcmp(fun->restrictions->contents,"dl") != 0)) ||
              ((fun->unknownReturnValueType != INTEGER_BIT) &&
               (fun->unknownReturnValueType != FLOAT_BIT) &&
               (fun->unknownReturnValueType != NUMBER_BITS)))
             {
               WriteString(theEnv,STDERR,"Function add-fuzzy-modifier (incorrect function type in arg 2)\n");
               return;               
             }
        }
#if DEFFUNCTION_CONSTRUCT
      else
        {    
          struct expr *top, *temp;
          bool res;
          UDFValue result;
          
          if (deffun->minNumberOfParameters != 1 || deffun->maxNumberOfParameters != 1)
             {
               WriteString(theEnv,STDERR,"Function add-fuzzy-modifier (modifier function requires exactly 1 arg)");
               return;               
             }
          /* try out the deffunction and make sure it accepts a float arg and returns a float */
          top = GenConstant(theEnv,PCALL, (void *)deffun);
          top->argList = GenConstant(theEnv,FLOAT_TYPE, CreateFloat(theEnv,0.0));
          temp = EvaluationData(theEnv)->CurrentExpression;
          EvaluationData(theEnv)->CurrentExpression = top;
          res = (*EvaluationData(theEnv)->PrimitivesArray[PCALL]->evaluateFunction)(theEnv,deffun,&result);
          EvaluationData(theEnv)->CurrentExpression = temp;
          ReturnExpression(theEnv,top);
          if (!res || (result.header->type != FLOAT_TYPE))
             {
               WriteString(theEnv,STDERR,"Function add-fuzzy-modifier (modifier function must accept and return FLOAT)");
               return;               
             }
        }
#endif
 
       if (AddFuzzyModifier(theEnv,modname, NULL, fun, deffun) == true)
          { return; }
      
    SetEvaluationError(theEnv,true);
  }

 /************************************************************
    remove_fuzzy_modifier():                                            

    removes a fuzzy modifier from the list of modifiers
    (note: cannot remove the system provided ones)

************************************************************/
void remove_fuzzy_modifier(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   UDFValue theArgument;
   
   if (! UDFFirstArgument(context,SYMBOL_BIT,&theArgument))
     { return; }
      
   RemoveFuzzyModifier(theEnv,theArgument.lexemeValue->contents);
  }

/**********************************************************************
    FUNCTIONS TO CONTROL INFERENCING & PRINTING
 **********************************************************************/
 


 /************************************************************
    set_fuzzy_inference_type():                                            

    sets the type of fuzzy inferencing to 1 of:
 
       max-min     - default
       max-prod

************************************************************/
void set_fuzzy_inference_type(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   const char *inference_type;
   UDFValue theArgument;
   
   if (! UDFFirstArgument(context,SYMBOL_BIT,&theArgument))
     { return; }
     
   inference_type = theArgument.lexemeValue->contents; /* get type of inferencing */
      
   if ((strcmp(inference_type,"max-min") == 0) ||
       (strcmp(inference_type,"MAX-MIN") == 0))
     { FuzzyData(theEnv)->FuzzyInferenceType = MAXMIN; }
   else if ((strcmp(inference_type,"max-prod") == 0) ||
            (strcmp(inference_type,"MAX-PROD") == 0))
     { FuzzyData(theEnv)->FuzzyInferenceType = MAXPROD; }
   else
     {
      SetEvaluationError(theEnv,true);
      WriteString(theEnv,STDERR,"Function set-fuzzy-inference-type (argument must be max-min or max-prod)\n");
     }
  }

 /************************************************************
    get_fuzzy_inference_type():                                            

    gets the type of fuzzy inferencing as:
 
       "max-min"  OR   "max-prod"

************************************************************/
void get_fuzzy_inference_type(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   if (FuzzyData(theEnv)->FuzzyInferenceType == MAXMIN)
     { returnValue->lexemeValue = CreateSymbol(theEnv,"max-min"); }
   else
     { returnValue->lexemeValue = CreateSymbol(theEnv,"max-prod"); }
  }

/************************************************************
    set_fuzzy_display_precision():                                            

    sets the precision used when displaying fuzzy set values
 
************************************************************/
void set_fuzzy_display_precision(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   UDFValue arg;
   long long precision;

   if (! UDFFirstArgument(context,INTEGER_BIT,&arg))
     { return; }
    
   precision = arg.integerValue->contents;

   if (precision > 16) precision = 16;
   else if (precision <= 1) precision = 2;

   FuzzyData(theEnv)->FuzzyFloatPrecision = (int) precision;
  }

/************************************************************
    get_fuzzy_display_precision():                                            

    gets the precision used when displaying fuzzy set values  

************************************************************/
void  get_fuzzy_display_precision(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   returnValue->integerValue = CreateInteger(theEnv,FuzzyData(theEnv)->FuzzyFloatPrecision);
  }

#ifndef nint

static int nint( double );

/* Define a rounding function if not already available */

static int nint(
   double f)
{
  return( (int)floor(f + 0.5) );
}

#endif



#define NROWS 20          /* number of rows in the plot */
#define NCOLS 50          /* number of columns in the plot */
#define NTICKS 5          /* number of ticks per division in x axis */

 /************************************************************
    plot_fuzzy_value():                                            

    plots the fuzzy value in an ascii format


   (plot-fuzzy-value logname plotchar(s) low high fuzzyvalue [ fuzzyvalue... ])

   where
         logname     - router name
         plotchar(s) - character(s) to use for plotting
         low         - low limit in universe for graph (if symbol use from)
         high        - high limit in universe for graph (if symbol use to)
         fuzzyvalue  - fuzzy value to plot 

   NOTE: There may be multiple fuzzy values plotted on the same graph.
         The plotchar is selected form the list of chars provided. When
         more fuzzy values than plotchars the last is used over again.
 
************************************************************/
void plot_fuzzy_value(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   const char *dummyid;
   int i, j, k, m, lastk = 0, previousLastk, numArgs, n;
   UDFValue theArgument;
   char theSymbol = 0;
   const char *theString;
   struct fuzzy_value *fv_ptr, *fv_ptr_first;
   char PlotSpace[NROWS+1][NCOLS+2];
   double from, to, range, deltax;
   double fromArg, toArg;
   double *fvx, *fvy, y;
   double thisx, thisy = 0.0, lastx = 0.0, lasty = 0.0;
   double nextx = 0.0, nexty = 0.0, previousThisy;
   double miny, maxy;
   char str[200];
   size_t numSymbols;
   int fvNum;

   /*=====================================================*/
   /* Get the logical name to which output is to be sent. */
   /*=====================================================*/

   numArgs = UDFArgumentCount(context);

   dummyid = GetLogicalName(context,STDOUT);
   if (dummyid == NULL)
     {
       PrintErrorID(theEnv,"IOFUN",1,false);
       WriteString(theEnv,STDERR,"Illegal logical name used for plot-fuzzy-value function.\n");
       SetHaltExecution(theEnv,true);
       SetEvaluationError(theEnv,true);
       return;
     }

   /*============================================================*/
   /* Determine if any router recognizes the output destination. */
   /*============================================================*/

   if (QueryRouters(theEnv,dummyid) == false)
     {
      UnrecognizedRouterMessage(theEnv,dummyid);
      return;
     }

   /*============================================================*/
   /* Get the symbol(s) for plotting                             */
   /*============================================================*/

   if (! UDFNextArgument(context,SYMBOL_BIT | STRING_BIT,&theArgument))
     { return; }

   theString = theArgument.lexemeValue->contents;

   if (strlen(theString) < 1)
     {
       WriteString(theEnv,STDERR,"ERROR: Function plot_fuzzy_value (argument 3 must be a character)\n");
       return;
     }

   numSymbols = strlen(theString);

   /*============================================================*/
   /* Get the limit arguments                                    */
   /*============================================================*/
   
   if (! UDFNthArgument(context,5,INTEGER_BIT | FACT_ADDRESS_BIT | FUZZY_BIT,&theArgument))  /* need deftemplate of 1st fuzzy value */
     { return; }

   fv_ptr = getFuzzyValuePtr(theEnv,&theArgument, "plot-fuzzy-value", 5);
   if (fv_ptr == NULL)  return;

   from = fv_ptr->whichDeftemplate->fuzzyTemplate->from;
   to = fv_ptr->whichDeftemplate->fuzzyTemplate->to;

   fv_ptr_first = fv_ptr;

   if (! UDFNthArgument(context,3,NUMBER_BITS | SYMBOL_BIT,&theArgument))
     { return; }

   if ((theArgument.header->type == INTEGER_TYPE) || (theArgument.header->type == FLOAT_TYPE))
     {
       fromArg = (theArgument.header->type == INTEGER_TYPE) ? (double) theArgument.integerValue->contents :
                                                              theArgument.floatValue->contents;
       if (fromArg > from) from = fromArg;
     }

   if (! UDFNthArgument(context,4,NUMBER_BITS | SYMBOL_BIT,&theArgument))
     { return; }

   if ((theArgument.header->type == INTEGER_TYPE) || (theArgument.header->type == FLOAT_TYPE))
     {
       toArg = (theArgument.header->type == INTEGER_TYPE) ? (double) theArgument.integerValue->contents :
                                                            theArgument.floatValue->contents;
       if (toArg < to) to = toArg;
     }

   if (from >= to)
     {
       WriteString(theEnv,STDERR,"ERROR: Function plot_fuzzy_value (low limit for plot >= high limit)\n");
       return;
     }

   /*============================================================*/
   /* Set up to Do the plotting                                  */
   /*============================================================*/

   for (i=0; i<=NROWS; i++)
      { for (j=0; j<NCOLS+1; j++)
           PlotSpace[i][j] = ' ';

        PlotSpace[i][j] = '\0';
      }

   range = to - from;
   deltax = range/NCOLS;

   WriteString(theEnv,dummyid, "\nFuzzy Value: ");
   WriteString(theEnv,dummyid, fv_ptr->whichDeftemplate->header.name->contents);
   WriteString(theEnv,dummyid, "\nLinguistic Value: ");

   /*============================================================*/
   /* Get the fuzzy value(s)  and do the plotting                */
   /*============================================================*/

   for (fvNum = 0; fvNum <= numArgs-5; fvNum++)
     {

      if (fvNum == 0)
        fv_ptr = fv_ptr_first;
      else
        {
         if (! UDFNthArgument(context,5+fvNum,INTEGER_BIT | FACT_ADDRESS_BIT | FUZZY_BIT,&theArgument))  /* need deftemplate of 1st fuzzy value */
           { return; }

          fv_ptr = getFuzzyValuePtr(theEnv,&theArgument, "plot-fuzzy-value", 5+fvNum);
        }
       
      if (fv_ptr == NULL)  return;

      if (numSymbols > ((size_t) fvNum)) 
         theSymbol = theString[fvNum];

      /* MUST have same deftemplates for all fuzzy values to plot properly */
      if (fv_ptr->whichDeftemplate != fv_ptr_first->whichDeftemplate)
        {
          WriteString(theEnv,STDERR,"ERROR: Function plot_fuzzy_value (all fuzzy values must have same fuzzy deftemplate)\n");
          break;
        }

      { char tmpStr[10];

        if (fvNum > 0) WriteString(theEnv,dummyid, ",  ");
        WriteString(theEnv,dummyid, fv_ptr->name);
        sprintf(tmpStr, " (%c)", theSymbol);
        WriteString(theEnv,dummyid, tmpStr);
      }
 
      /* make arrays with point added to beginning and end to 
         capture the extreme values of the universe of discourse
      */
      n = fv_ptr->n + 2;
      fvx = FgetArray(theEnv,n);
      fvy = FgetArray(theEnv,n);
      for (i=1; i<n-1; i++)
        {
         fvx[i] = fv_ptr->x[i-1];  fvy[i] = fv_ptr->y[i-1];
        }
      fvx[0]   = fv_ptr->whichDeftemplate->fuzzyTemplate->from;
      fvy[0]   = fvy[1];
      fvx[n-1] = fv_ptr->whichDeftemplate->fuzzyTemplate->to;
      fvy[n-1] = fvy[n-2];

      /* NOTE:  thisx, thisy  are current points -- at plotting points
                lastx, lasty  is 1st pt of line that crosses thisx 
                nextx, nexty  is end pt of line that crosses thisx 
      */

      thisx = from;
      previousLastk = 0;
      previousThisy = 0.0; 
      k = 0; 

      /* plot the points -- when a point is plotted it is
         necessary to go back over the 'actual' points in the array
         to see if any lie outside the y range of the point just plotted
         and the last one; if they do then plot them as well; if we
         don't do this then some points that 'fall betwen the cracks' will
         not show on the plot
      */
      while (thisx <= to && k <= NCOLS)
       {
        for (i=previousLastk; i<n-1; i++)
          {    
            lastx = fvx[i]; lasty = fvy[i];
            lastk = i;
            nextx = fvx[i+1]; nexty = fvy[i+1];
            if (thisx == lastx)
              { /* print points with same x values -- should have diff y values */
               thisy = fvy[lastk];
               j = nint(thisy*NROWS);
               PlotSpace[NROWS-j][k] = theSymbol;
               if (nextx != thisx) 
                   break;
               else
                { /* we have a vertical line -- plot it */
                  m = nint(nexty*NROWS);
                  if (m > j)
                     while (m > j)
                       { PlotSpace[NROWS-m][k] = theSymbol; m--; }
                  else
                     while (m < j)
                       { PlotSpace[NROWS-m][k] = theSymbol; m++; }
                }
              }
            else if (thisx > lastx && thisx < nextx)
              break;
          }
        if (nextx == lastx) /* same x points -- just find min and max y values */
          {
            miny = thisy; maxy = thisy;
            if (previousThisy < miny) miny = previousThisy;
            if (nexty < miny) miny = nexty;
            if (previousThisy > maxy) previousThisy = maxy;
            if (nexty > maxy) maxy = nexty;
          }
        else
          {
           /* found the points that span this point - calc y value and plot it*/
           thisy = lasty + (thisx - lastx)*(nexty - lasty)/(nextx - lastx);
           j = nint(thisy*NROWS);
           PlotSpace[NROWS-j][k] = theSymbol;
       
           /* now plot any points that between this point and the previous this
              point that lie outside of the y range of these 2 points OR
              are at same x location (vertical line)
           */
           if (thisy < previousThisy)
             { miny = thisy; maxy = previousThisy; }
           else
             { maxy = thisy; miny = previousThisy; }
	 }

        /* this second test (fvx[i] >= from) is included so that we 
           don't wander back before the start of the plot
        */
        for (i = lastk; i > previousLastk  && fvx[i] >= from; i--)
         {
          if (fvy[i] > maxy || fvy[i] < miny)
            {
              y = fvy[i];
              j = nint(y*NROWS);
              m = nint((fvx[i]-from)/deltax);
              PlotSpace[NROWS-j][m] = theSymbol;
            }
          if (fvx[i] == fvx[i-1])
            { /* we have a straight line plot it! */
              int j1, j2;
              m = nint((fvx[i]-from)/deltax);
              j1 = nint(fvy[i]*NROWS);
              j2 = nint(fvy[i-1]*NROWS);
              if (j1 > j2)
                 while (j1 > j2)
                   { PlotSpace[NROWS-j1][m] = theSymbol; j1--; }
              else
                 while (j1 < j2)
                   { PlotSpace[NROWS-j1][m] = theSymbol; j1++; }
            }
         }  

        previousLastk = lastk;
        previousThisy = thisy;
        thisx = thisx + deltax;
        k++;

       }  /* end of   while (thisx <= to && k <= NCOLS)   */

      FrtnArray(theEnv,fvx, n);
      FrtnArray(theEnv,fvy, n);

     }  /* end of  for (fvNum = 0 ...    */
  
   WriteString(theEnv,dummyid, "\n\n");

   for (i=0; i<=NROWS; i++)
      { sprintf(str, "%5.2f", 1.0 - ((float)i /(float)NROWS));
        WriteString(theEnv,dummyid, str);
        WriteString(theEnv,dummyid, &PlotSpace[i][0]);
        WriteString(theEnv,dummyid, "\n");
      }
   
   WriteString(theEnv,dummyid, "     ");
   for (i=0; i<NCOLS/NTICKS; i++)
      {
        WriteString(theEnv,dummyid, "|");
        for (j=1; j<NTICKS; j++)
            WriteString(theEnv,dummyid, "-");
      }
   WriteString(theEnv,dummyid, "|");
   WriteString(theEnv,dummyid, "\n ");
   for (i=0; i<=NCOLS; i=i+2*NTICKS)
      {
        sprintf(str, "%7.2f", from+i*deltax);
        WriteString(theEnv,dummyid, str);
        for (j=0; j<NTICKS*2-7; j++) str[j] = ' ';
        str[j] = '\0';
        WriteString(theEnv,dummyid, str);
      }

   sprintf(str, "\n\nUniverse of Discourse:  From %6.2f  to  %6.2f\n\n",
           fv_ptr->whichDeftemplate->fuzzyTemplate->from,
           fv_ptr->whichDeftemplate->fuzzyTemplate->to);
   WriteString(theEnv,dummyid, str);

}

/*******************************************************************
    Functions for setting and accessing the alpha-cut for fuzzy
    patter matching
 *******************************************************************/

/*******************************************************************
    set_alpha_value()

    Sets the alpha cut value which determines the minimum amount of 
    match between a pattern and a fuzzy value that is required to
    consider it a match.
 *******************************************************************/
void set_alpha_value(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   UDFValue theArgument;
   double theAlphaValue;

   if (! UDFFirstArgument(context,NUMBER_BITS,&theArgument))
     { return; }

   if (theArgument.header->type == INTEGER_TYPE)
     { theAlphaValue = (double) theArgument.integerValue->contents; }
   else
     { theAlphaValue = theArgument.floatValue->contents; }
     
   if (theAlphaValue < 0.0 || theAlphaValue > 1.0)
     {
      SetEvaluationError(theEnv,true);
      PrintErrorID(theEnv,"Alpha Value ",909,true);
      WriteString(theEnv,STDERR,"Expected Value in Range 0.0 to 1.0");
      WriteString(theEnv,STDERR,".\n");
     }
   else
     { FuzzyData(theEnv)->FuzzyAlphaValue = theAlphaValue; }
  }

/********************************************************************
    get_alpha_value()

    Returns the Alpha value.
 ********************************************************************/
void get_alpha_value(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   returnValue->floatValue = CreateFloat(theEnv,FuzzyData(theEnv)->FuzzyAlphaValue);
  }

/*******************************************************************
    Functions to access slots of facts
 *******************************************************************/

/*******************************************************************
    get_fuzzy_slot()

    Get a fuzzy slot from a fact:

      arg 1   - the fact  (integer or fact address)
      arg 2   - the slot name (symbol) or if missing will expect
                arg 1 to be a fuzzy deftemplate fact and will
                get the only slot (fuzzy value)
 *******************************************************************/
void get_fuzzy_slot( // TBD returns FALSE for error
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   UDFValue theArgument;
   long long factIndex;
   bool found_fact;
   Fact *factPtr;
   const char *slotName;
   CLIPSValue theSlot;
   char tempBuffer[30];
   int numArgs;
    
   returnValue->lexemeValue = FalseSymbol(theEnv);
   
   numArgs = UDFArgumentCount(context);
     
   if (! UDFFirstArgument(context,INTEGER_BIT | FACT_ADDRESS_BIT,&theArgument))
     { return; }

   if (theArgument.header->type == INTEGER_TYPE)
     {
      factIndex = theArgument.integerValue->contents;
      if (factIndex >= 0)
        {
         found_fact = false;
         factPtr = GetNextFact(theEnv,NULL);
         while (factPtr != NULL)
           {
            if (factPtr->factIndex == factIndex)
              {
               found_fact = true;
               break;
              }
            factPtr = factPtr->nextFact;
           }
       
         if (found_fact == false)
           {
            sprintf(tempBuffer,"f-%lld",factIndex);
            CantFindItemErrorMessage(theEnv,"fact",tempBuffer,false);
            return;
           }
        }
      else
        {
         ExpectedTypeError1(theEnv,"get-fuzzy-slot",1,"fact-index must be positive");
         return;
        }
     }
   else if (theArgument.header->type == FACT_ADDRESS_TYPE)
     { /* arg type is fact address */
      factPtr = theArgument.factValue;
     }
   else
     {
      ExpectedTypeError1(theEnv,"get-fuzzy-slot",1,"fact-index/fact-address of fuzzy fact");
      return;
     }

   if (numArgs == 1)
     { slotName = NULL; } /* looking for Fuzzy Deftemplate fact slot */
   else
     {
      if (! UDFNextArgument(context,SYMBOL_BIT,&theArgument))
        { return; }
      slotName = theArgument.lexemeValue->contents;
     }

   if (GetFactSlot(factPtr,slotName,&theSlot) == GSE_NO_ERROR)
     {
      if (theSlot.header->type == FUZZY_VALUE_TYPE)
        {
         struct fuzzy_value *fv;
         fv = CopyFuzzyValue(theEnv,theSlot.fuzzyValue->contents);
         returnValue->fuzzyValue = CreateFuzzyValue(theEnv,fv);
         rtnFuzzyValue(theEnv,fv);
         return;
        }
      else
        {
         if (slotName == NULL)
           {
            CantFindItemErrorMessage(theEnv,"fuzzy slot (not Fuzzy Deftemplate fact)","",false);
            return;
           }
         else
           {
            CantFindItemErrorMessage(theEnv,"fuzzy slot (not Fuzzy Value)",slotName,true);
            return;
           }
        }
     }
   else
     {
      if (slotName == NULL)
        {
         sprintf(tempBuffer,"(no slot name specified)");
         CantFindItemErrorMessage(theEnv,"fact slot",tempBuffer,false);
        }
      else
       { CantFindItemErrorMessage(theEnv,"fact slot",slotName,true); }
     }
  }

/*******************************************************************
    Functions to create and operate on fuzzy values
 *******************************************************************/

/*******************************************************************
    fuzzy_union()

    Perform a fuzzy union of 2 fuzzy values

    arg 1    - fuzzy value
    arg 2    - fuzzy value
 *******************************************************************/
void fuzzy_union(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {  
   UDFValue theArgument;
   struct fuzzy_value *f1, *f2, *fresult;
   size_t lenf1Name, lenf2Name;
   char *f1Name, *f2Name;
   char *str;
   
   returnValue->lexemeValue = FalseSymbol(theEnv);
    
   if (! UDFFirstArgument(context,FUZZY_BIT,&theArgument))
     { return; }
 
   f1 = theArgument.fuzzyValue->contents;

   if (! UDFNextArgument(context,FUZZY_BIT,&theArgument))
     { return; }
      
   f2 = theArgument.fuzzyValue->contents;

   if (f2->whichDeftemplate != f1->whichDeftemplate)
     {
      SetEvaluationError(theEnv,true);
      ExpectedTypeError1(theEnv,"fuzzy-union",2,"fuzzy value (with same fuzzy template as arg #1)");
      return;
     }
      
    fresult = funion(theEnv,f1, f2);
    f1Name = f1->name;
    f2Name = f2->name;
    lenf1Name = strlen(f1Name);
    lenf2Name = strlen(f2Name);
    str = (char *) gm2(theEnv,lenf1Name + lenf2Name + 13);
    strcpy(str, "[ ");
    strcpy(str+2, f1Name);
    strcpy(str+2+lenf1Name," ] OR [ ");
    strcpy(str+lenf1Name+10, f2Name);
    strcpy(str+lenf1Name+lenf2Name+10, " ]");
    if (fresult->name != NULL) rm(theEnv,fresult->name, strlen(fresult->name)+1);
    fresult->name = str;
    returnValue->fuzzyValue = CreateFuzzyValue(theEnv,fresult);
    rtnFuzzyValue(theEnv,fresult);
  }

/*******************************************************************
    fuzzy_intersection()

    Perform a fuzzy intersection of 2 fuzzy values

    arg 1    - fuzzy value
    arg 2    - fuzzy value
 *******************************************************************/
void fuzzy_intersection(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {  
   UDFValue theArgument;
   struct fuzzy_value *f1, *f2, *fresult;
   size_t lenf1Name, lenf2Name;
   char *f1Name, *f2Name;
   char *str;

   returnValue->lexemeValue = FalseSymbol(theEnv);
    
   if (! UDFFirstArgument(context,FUZZY_BIT,&theArgument))
     { return; }
 
   f1 = theArgument.fuzzyValue->contents;

   if (! UDFNextArgument(context,FUZZY_BIT,&theArgument))
     { return; }
      
   f2 = theArgument.fuzzyValue->contents;

   if (f2->whichDeftemplate != f1->whichDeftemplate)
     {
      SetEvaluationError(theEnv,true);
      ExpectedTypeError1(theEnv,"fuzzy-intersection",2,"fuzzy value (with same fuzzy template as arg #1)");
      return;
     }

   fresult = fintersect(theEnv,f1, f2);
   f1Name = f1->name;
   f2Name = f2->name;
   lenf1Name = strlen(f1Name);
   lenf2Name = strlen(f2Name);
   str = (char *) gm2(theEnv,lenf1Name + lenf2Name + 14);
   strcpy(str, "[ ");
   strcpy(str+2, f1Name);
   strcpy(str+2+lenf1Name," ] AND [ ");
   strcpy(str+lenf1Name+11, f2Name);
   strcpy(str+lenf1Name+lenf2Name+11, " ]");
   if (fresult->name != NULL) rm(theEnv,fresult->name, strlen(fresult->name)+1);
   fresult->name = str;
   returnValue->fuzzyValue = CreateFuzzyValue(theEnv,fresult);
   rtnFuzzyValue(theEnv,fresult);
  }

/*******************************************************************
    fuzzy_modify()

    Perform a fuzzy modification of a fuzzy value

    arg 1    - fuzzy value
    arg 2    - fuzzy modifier (eg. not, very, slightly, etc.
 *******************************************************************/
void fuzzy_modify(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {  
   UDFValue theArgument;
   struct fuzzy_value *f1, *fresult;
   const char *modifier;
   struct modifierListItem *mptr;

   returnValue->lexemeValue = FalseSymbol(theEnv);
   
   if (! UDFFirstArgument(context,FUZZY_BIT,&theArgument))
     { return; }
     
   f1 = theArgument.fuzzyValue->contents;
   
   if (! UDFNextArgument(context,SYMBOL_BIT,&theArgument))
     { return; }

   modifier = theArgument.lexemeValue->contents;

   if ((mptr = FindModifier(theEnv,modifier)) != NULL)
     {
      fresult = CopyFuzzyValue( theEnv,f1 );
      ModifyFuzzyValue(theEnv,mptr,fresult);
      returnValue->fuzzyValue = CreateFuzzyValue(theEnv,fresult);
      rtnFuzzyValue(theEnv,fresult);
     }
   else
     {
      SetEvaluationError(theEnv,true);
      ExpectedTypeError1(theEnv,"fuzzy-modify",2,"symbol AND a modifier name");
     }
  }

/*******************************************************************
    create_fuzzy_value

    Create a fuzzy value

    arg 1    - fuzzy value -- parser creates it!

 *******************************************************************/
void create_fuzzy_value(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {  
   UDFValue theArgument;
   struct fuzzy_value *fv;

   returnValue->lexemeValue = FalseSymbol(theEnv);

   if (! UDFFirstArgument(context,FUZZY_BIT,&theArgument))
     { return; }

   fv = CopyFuzzyValue(theEnv,theArgument.fuzzyValue->contents);
   returnValue->fuzzyValue = CreateFuzzyValue(theEnv,fv);
   rtnFuzzyValue(theEnv,fv);
  }

/*******************************************************************

 Functions to parse special functions

     create-fuzzy-value

 *******************************************************************/



#if (! RUN_TIME)

/*******************************************************************
  CreateFuzzyValueParse:

  Parses the create-fuzzy-value function

    (create-fuzzy-value fuzzyTemplateName fuzzy-expression)
 *******************************************************************/

static struct expr *CreateFuzzyValueParse( 
  Environment *theEnv,
  struct expr *top,
  const char *logName)
  {
   struct token theToken;
   struct deftemplate *theDeftemplate;
   const char * theName;
   unsigned int count;
   bool error;
   struct expr *fvExpr;
  
   SavePPBuffer(theEnv," ");  /* space after the function name */

   /* this is now just like parsing a fuzzy deftemplate fact assert ...
      the template name and then the fuzzy value expression
   */

   /* First we get the deftemplate name -- must be a fuzzy deftemplate */
   GetToken(theEnv,logName, &theToken);

   if (theToken.tknType != SYMBOL_TOKEN)
     {
      SyntaxErrorMessage(theEnv,"function create-fuzzy-value (1st argument must be a symbol)");
      ReturnExpression(theEnv,top);
      return( NULL );
     }

   theName = theToken.lexemeValue->contents;
   if (FindModuleSeparator(theName))
     { theDeftemplate = FindDeftemplate(theEnv,theName); }
   else
     {
      theDeftemplate = (Deftemplate *)
            FindImportedConstruct(theEnv,"deftemplate",NULL,theName,
                                   &count,true,NULL);
      if (count > 1)
        {
          AmbiguousReferenceErrorMessage(theEnv,"deftemplate",theName);
          ReturnExpression(theEnv,top);
          return(NULL);
        }
     }

   if ((theDeftemplate == NULL) ||
       ((theDeftemplate != NULL) &&
        (theDeftemplate->fuzzyTemplate == NULL)))
     {
      CantFindItemErrorMessage(theEnv,"fuzzy deftemplate", theName,true);
      ReturnExpression(theEnv,top);
      return NULL;
     }
  
   fvExpr = ParseAssertFuzzyFact(theEnv,logName, &theToken, &error, RIGHT_PARENTHESIS_TOKEN,
                                 false, theDeftemplate, false);

   if (error == true)
     {
       ReturnExpression(theEnv,top);
       return( NULL );
     }

   top->argList = fvExpr;
   return top;
  }

#endif  /* !RUN_TIME */


#endif  /* FUZZY_DEFTEMPLATES */

