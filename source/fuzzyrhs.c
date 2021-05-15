   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*             CLIPS Version 6.00  05/12/93            */
   /*                                                     */
   /*             FUZZY RHS PARSER MODULE                 */
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
    
    functions to parse fuzzy facts asserted on rhs
    allocation and de-allocation of arrays from CLIPS memory

    The calling of functions has the folowing hierarchy:

     ParseAssertFuzzyFact (used in deffacts and asserts)
               |
               |
               |---> assertParseFuzzySet ---|
               |                            |
               |                            |
               |---> ParseLinguisticExpr    |---> assertParseSingletonSet
                     (used by fuzzylhs.c    |
                      to parse patterns     |---> assertParseStandardSet
                      and in fuzzypsr.c to
                      parse terms in fuzzy
                      deftemplates)

 ******************************************************************/
 
#define _FUZZYRHS_SOURCE_


#include "setup.h"


#include <stdio.h>
#define _CLIPS_STDIO_

#include <math.h>
#include <string.h>

#if FUZZY_DEFTEMPLATES
#include "fuzzyrhs.h"
#include "fuzzyval.h"
#include "fuzzylv.h"
#include "fuzzyutl.h"
#include "fuzzypsr.h"
#include "fuzzymod.h"
#endif

#if FUZZY_DEFTEMPLATES || CERTAINTY_FACTORS
#include "symbol.h"
#include "expressn.h"
#include "evaluatn.h"
#include "exprnops.h"
#include "exprnpsr.h"
#include "extnfunc.h"
#include "tmpltdef.h"
#include "pprint.h"
#include "scanner.h"
#include "memalloc.h"
#include "constant.h"
#include "prntutil.h"
#include "router.h"
#endif



#if FUZZY_DEFTEMPLATES




/******************************************************************
    Global Internal Function Declarations
    
    Defined in fuzzyrhs.h
 ******************************************************************/



/******************************************************************
    Local Internal Function Declarations
 ******************************************************************/
 
  static struct fuzzy_value   *ParseLExpr(Environment *theEnv,
                                          const char *readSource,
                                          struct token *tempToken,
                                          struct fuzzyLv *lvp,
                                          bool *error);
  static struct fuzzy_value   *ParseLTerm(Environment *theEnv,
                                          const char *readSource,
                                          struct token *tempToken,
                                          struct fuzzyLv *lvp,
                                          bool *error);
  static struct fuzzy_value   *ParseModExpr(Environment *theEnv,
                                            const char *readSource,
                                            struct token *tempToken,
                                            struct fuzzyLv *lvp,
                                            bool *error);
  static struct fuzzy_value   *PrimaryTerm(Environment *theEnv,
                                           const char *readSource,
                                           struct token *tempToken,
                                           struct fuzzyLv *lvp,
                                           bool *error);
  static char                 *modifyName(Environment *theEnv,const char *str1, const char *str2);
  static struct primary_term  *FindPrimaryTerm(struct fuzzyLv *lvp, CLIPSLexeme *pt_name);
  static struct expr          *assertParseFuzzySet(Environment *theEnv,
                                                   const char *readSource,
                                                   struct token *tempToken,
                                                   bool  *error,
                                                   struct deftemplate *t,
                                                   int constantsOnly,
                                                   int *onlyConstantsFound);
  static struct expr          *assertParseStandardSet(Environment *theEnv,
                                                      const char *readSource,
                                                      struct token *tempToken,
                                                      bool  *error,
                                                      struct deftemplate *t,
                                                      int constantsOnly,
                                                      int *onlyConstantsFound,
                                                      int function_type);
  static struct expr          *assertParseSingletonSet(Environment *theEnv,
                                                       const char *readSource,
                                                      struct token *tempToken,
                                                      bool  *error,
                                                      struct deftemplate *t,
                                                      int constantsOnly,
                                                      int *onlyConstantsFound);
  static struct fuzzy_value   *convertStandardSet(Environment *theEnv,struct expr *top, bool *error);
  static struct fuzzy_value   *convertSingletonSet(Environment *theEnv,struct expr *top, bool *error);
  static void                  expressionToFloat(Environment *theEnv,struct expr *test_ptr,
                                                 double *result,
                                                 bool *error);
  static void                  expressionToInteger(Environment *theEnv,struct expr *test_ptr,
                                                  int *result,
                                                  bool *error);

   
/******************************************************************
    Global External Variable Declarations
 ******************************************************************/



/******************************************************************
    Global Internal Variable Declarations
 ******************************************************************/



/************************************************************************
    FUNCTIONS that Copy and Compact Fuzzy Values

 ************************************************************************/
 

/*************************************************************/ 
/* CopyFuzzyValue:                                           */
/* copies a Fuzzy Value structure                            */
/*************************************************************/
struct fuzzy_value *CopyFuzzyValue(
  Environment *theEnv,
  struct fuzzy_value *fv)
{
    int i, num;
    struct fuzzy_value *new_fv;
   
    new_fv = get_struct(theEnv,fuzzy_value);
    new_fv->name = (char *) gm2(theEnv,strlen(fv->name)+1);
    strcpy(new_fv->name, fv->name);
    new_fv->whichDeftemplate = fv->whichDeftemplate;
    num = fv->n;
    
    new_fv->n = num;
    new_fv->maxn = num;
    new_fv->x = FgetArray(theEnv,num);
    new_fv->y = FgetArray(theEnv,num);
    
    for ( i = 0; i < num; i++)
     {
        new_fv->x[i] = fv->x[i];
        new_fv->y[i] = fv->y[i];
     }
    return (new_fv);
}



/*************************************************************/ 
/* CompactFuzzyValue:                                        */
/* compacts the membership values of a Fuzzy Value structure */
/* so that n and maxn are the same                           */
/*************************************************************/
void CompactFuzzyValue(
  Environment *theEnv,
  struct fuzzy_value *fv)
{
    int i, num, maxnum;
    double *xptr, *yptr;   
    
    num = fv->n;
    maxnum = fv->maxn;
    
    if (maxnum == num) return;
    
    fv->maxn = num;
    xptr = FgetArray(theEnv,num);
    yptr = FgetArray(theEnv,num);
    
    for ( i = 0; i < num; i++)
     {
        xptr[i] = fv->x[i];
        yptr[i] = fv->y[i];
     }
    
    FrtnArray(theEnv,fv->x, maxnum);
    FrtnArray(theEnv,fv->y, maxnum);
    
    fv->x = xptr;
    fv->y = yptr;
}




/************************************************************************
    FUNCTIONS THAT PARSE ASSERT ARGUMENTS for FUZZY FACTS

 ************************************************************************/
 
 
 
/***************************************************************/
/* ParseAssertFuzzyFact:                                       */
/* Returns assert argument in expr format.                     */ 
/* Error flag is set to true if an error occurs.               */
/*                                                             */
/* NOTE: don't use endType arg since it appears that it is     */
/*       ALWAYS RPAREN -- if this changes then should change   */
/*       this code to use endType rather than RPAREN           */
/***************************************************************/

struct expr *ParseAssertFuzzyFact(
  Environment *theEnv,
  const char *readSource,
  struct token *tempToken,
  bool *error,
  TokenType endType,
  int constantsOnly, /* TRUE if only Constants allowed in fuzzy set descriptions */
  Deftemplate *theDeftemplate,
  int variablesAllowed)
{
    struct fuzzyLv *lvp = theDeftemplate->fuzzyTemplate;
    struct expr *next_one, *temp;
    struct fuzzy_value *fv;
    int onlyConstantsFound; /* TRUE if find only Constants in fuzzy set description */
    
    *error = false;

    /*=======================================*/
    /* Put a space between the template name */
    /* and the fuzzy set definition.         */
    /*=======================================*/

    SavePPBuffer(theEnv," ");
    GetToken(theEnv,readSource,tempToken);
    
    if (tempToken->tknType == SF_VARIABLE_TOKEN ||
        tempToken->tknType == GBL_VARIABLE_TOKEN)
      {
        struct token dummyToken;
		
        /* single field variables are allowed BUT they must be of type
           FUZZY_VALUE when finally evaluated AND must have same
           fuzzy deftemplate as required by the slot
        */
        if (constantsOnly || !variablesAllowed)
          {
            *error = true;
            SyntaxErrorMessage(theEnv,"deftemplate pattern (Variables not allowed)");
            return( NULL );
          }
        
        GetToken(theEnv,readSource,&dummyToken);
        if (dummyToken.tknType != RIGHT_PARENTHESIS_TOKEN)
          {
            *error = true;
            SyntaxErrorMessage(theEnv,"Fuzzy Expression (expecting ')' to terminate)");
            return( NULL );
          }
        else
            return( GenConstant(theEnv,TokenTypeToType(tempToken->tknType), tempToken->value) );
      }
    else if ((tempToken->tknType == LEFT_PARENTHESIS_TOKEN) ||
             ((tempToken->tknType == SYMBOL_TOKEN) &&
              (strcmp(tempToken->lexemeValue->contents,"#") == 0)
             )
            )
       {
         /* fuzzy set specified - # is optional so expect  '# (' or  '('  */
         if (tempToken->tknType == SYMBOL_TOKEN)
           {  
             SavePPBuffer(theEnv," ");
             /* assertParseFuzzySet is expecting to see a LPAREN as current token */
             GetToken(theEnv,readSource,tempToken);
           }

         next_one = assertParseFuzzySet(theEnv,readSource, tempToken, error,
                                         theDeftemplate,
                                         constantsOnly, &onlyConstantsFound);
         if (*error == true)
           {
            return(NULL);
           }
         else
           {            
            if (onlyConstantsFound)
              {
                fv = getConstantFuzzyValue (theEnv,next_one, error);
                if (*error == true)
                  {
                   ReturnExpression(theEnv,next_one);
                   return(NULL);
                  }
                temp = get_struct(theEnv,expr);
                temp->argList = NULL;
                temp->nextArg = NULL;
                temp->type = FUZZY_VALUE_TYPE;
                temp->fuzzyValue = CreateFuzzyValue(theEnv,fv);
                /* CreateFuzzyValue makes a copy of fv .. so return it */
                rtnFuzzyValue(theEnv,fv);
                ReturnExpression(theEnv,next_one);
                  return(temp);
              }
            else
              {
                return(next_one);
              }
           }        
       }
    else if ((fv = ParseLinguisticExpr(theEnv,readSource,tempToken,lvp,error))
              == NULL)
       {
         *error = true;
         return(NULL);
       }
    else
       {
         next_one = get_struct(theEnv,expr);
         next_one->argList = NULL;
         next_one->nextArg = NULL;
         next_one->type = FUZZY_VALUE_TYPE;
         next_one->fuzzyValue = CreateFuzzyValue(theEnv,fv);
         /* CreateFuzzyValue makes a copy of fv .. so return it */
         rtnFuzzyValue(theEnv,fv);
         return(next_one);
       } 
}    

/***************************************************************/
/* ParseLinguisticExpr:                                        */
/*                                                             */
/* Parses fuzzy expression with fuzzy terms, modifiers, AND,   */
/* OR and brackets ([ and ]) ONLY                              */
/*       eg.  very cold                                        */
/* Returns fuzzy value structure.                              */
/* Error flag is set to true if an error occurs.               */
/*                                                             */
/* The BNF of the linguistic expressions is:                   */
/*                                                             */
/*  <LExpr> ::= <LTerm> | <LTerm> OR <LExpr>                   */
/*                                                             */
/*  <LTerm> ::= <modExpr> | <LTerm> AND <modExpr>              */
/*                                                             */
/*  <modExpr> ::= MODIFIER <modExpr> | <element>               */
/*                                                             */
/*  <element> ::= PRIMARY-TERM | [ <LExpr> ]                   */
/*                                                             */
/* Note: This gives AND higher precedence than OR.             */
/*       MODIFIER is a valid modifier (not, very, etc.)        */
/*       PRIMARY-TERM is a term defined in a fuzzy deftemplate */
/*                                                             */
/***************************************************************/

struct fuzzy_value *ParseLinguisticExpr(
  Environment *theEnv,
  const char *readSource,
  struct token *tempToken,
  struct fuzzyLv *lvp,
  bool *error)
{
   struct fuzzy_value *fvptr;

   fvptr = ParseLExpr(theEnv,readSource,tempToken,lvp,error);

   if (*error == true)
      return( NULL );

   if (tempToken->tknType != RIGHT_PARENTHESIS_TOKEN)
     {
       *error = true;
       SyntaxErrorMessage(theEnv,"Fuzzy Expression (expecting ')' to terminate)");
       rtnFuzzyValue(theEnv,fvptr);
       return( NULL );
     }

   PPBackup(theEnv); PPBackup(theEnv); /* remove extra space after last token */
   SavePPBuffer(theEnv,")");

   return( fvptr );
}



/***************************************************************/
/* ParseLExpr:                                                 */
/*                                                             */
/* Parses a linguistic expression (see BNF in                  */
/* ParseLinguisticExpr routine above).                         */
/*                                                             */
/***************************************************************/
static struct fuzzy_value *ParseLExpr(
  Environment *theEnv,
  const char *readSource,
  struct token *tempToken,
  struct fuzzyLv *lvp,
  bool *error)
{
   struct fuzzy_value *fvLeft, *fvRight, *fvTemp;
   char *tmpstr, *tmpstr2;
   
   fvLeft = ParseLTerm(theEnv,readSource,tempToken,lvp,error);

   if (*error)
     return( NULL );

   if ((tempToken->tknType == SYMBOL_TOKEN) &&
       ((strcmp(tempToken->lexemeValue->contents, "OR") == 0) ||
        (strcmp(tempToken->lexemeValue->contents, "or") == 0)
      ))
     {
       SavePPBuffer(theEnv," "); /* space after the OR */
       GetToken(theEnv,readSource, tempToken);
       fvRight = ParseLExpr(theEnv,readSource,tempToken,lvp,error);
       
       if (*error)
         {
           rtnFuzzyValue(theEnv,fvLeft);
           return(NULL);
         }

       /* perform the OR (union) operation of the 2 fuzzy values */
       fvTemp = funion(theEnv,fvLeft, fvRight);

       tmpstr = modifyName(theEnv,"OR",fvRight->name);
       tmpstr2 = modifyName(theEnv,fvLeft->name, tmpstr);
       if (fvTemp->name != NULL) rm(theEnv,fvTemp->name, strlen(fvTemp->name)+1);
       fvTemp->name = tmpstr2;
       rm(theEnv,tmpstr, strlen(tmpstr)+1);
       rtnFuzzyValue(theEnv,fvLeft);
       rtnFuzzyValue(theEnv,fvRight);
       fvLeft = fvTemp;
     }
   return(fvLeft);
}


/***************************************************************/
/* ParseLTerm:                                                 */
/*                                                             */
/* Parses terms in a linguistic expression (see BNF in         */
/* ParseLinguisticExpr routine above).                         */
/*                                                             */
/***************************************************************/
static struct fuzzy_value *ParseLTerm(
  Environment *theEnv,
  const char *readSource,
  struct token *tempToken,
  struct fuzzyLv *lvp,
  bool *error)
{
   struct fuzzy_value *fvLeft, *fvRight, *fvTemp;
   char *tmpstr, *tmpstr2;
   
   fvLeft = ParseModExpr(theEnv,readSource,tempToken,lvp,error);

   if (*error)
     return( NULL );

   if ((tempToken->tknType == SYMBOL_TOKEN) &&
       ((strcmp(tempToken->lexemeValue->contents, "AND") == 0) ||
        (strcmp(tempToken->lexemeValue->contents, "and") == 0)
      ))
     {
       SavePPBuffer(theEnv," "); /* space after the AND */
       GetToken(theEnv,readSource, tempToken);
       fvRight = ParseModExpr(theEnv,readSource,tempToken,lvp,error);
       
       if (*error)
         {
           rtnFuzzyValue(theEnv,fvLeft);
           return(NULL);
         }

       /* perform the AND (intersection) operation of the 2 fuzzy values */
       fvTemp = fintersect(theEnv,fvLeft, fvRight);

       tmpstr = modifyName(theEnv,"AND",fvRight->name);
       tmpstr2 = modifyName(theEnv,fvLeft->name, tmpstr);
       if (fvTemp->name != NULL) rm(theEnv,fvTemp->name, strlen(fvTemp->name)+1);
       fvTemp->name = tmpstr2;
       rm(theEnv,tmpstr, strlen(tmpstr)+1);
       rtnFuzzyValue(theEnv,fvLeft);
       rtnFuzzyValue(theEnv,fvRight);
       fvLeft = fvTemp;
     }

   return(fvLeft);
}



/***************************************************************/
/* ParseLModExpr:                                              */
/*                                                             */
/* Parses modifier expressions in a linguistic expression (see */
/* ParseLinguisticExpr routine above).                         */
/*                                                             */
/***************************************************************/
static struct fuzzy_value *ParseModExpr(
  Environment *theEnv,
  const char *readSource,
  struct token *tempToken,
  struct fuzzyLv *lvp,
  bool *error)
{
   struct modifierListItem *mptr;
   struct fuzzy_value *fvptr;
   char *tmpstr, *tmpstr2;

   /* next token must be a symbol -- modifier or primary term or [ */
   if (tempToken->tknType != SYMBOL_TOKEN)
     {
       *error = true;
       SyntaxErrorMessage(theEnv,"Fuzzy Expression (expecting modifier, primary term or '[' )");
       return( NULL );
     }

   /* next token could be a [ to bracket the expressions */
   if (tempToken->tknType == SYMBOL_TOKEN &&
       strcmp(tempToken->lexemeValue->contents, "[") == 0
      )
     {
       SavePPBuffer(theEnv," "); /* space after the [ */
       GetToken(theEnv,readSource, tempToken);

       fvptr = ParseLExpr(theEnv,readSource,tempToken,lvp,error);

       if (*error == true)
           return( NULL );

       /* next token must now be the closing ']' */
       if (tempToken->tknType == SYMBOL_TOKEN &&
	   strcmp(tempToken->lexemeValue->contents, "]") == 0
	  )
         {
           SavePPBuffer(theEnv," "); /* space after the ] */
           GetToken(theEnv,readSource, tempToken);
          
           /* save the [ and ] in the fv name */
           tmpstr = modifyName(theEnv,"[",fvptr->name);
           tmpstr2 = modifyName(theEnv,tmpstr, "]");
           if (fvptr->name != NULL) rm(theEnv,fvptr->name, strlen(fvptr->name)+1);
           fvptr->name = tmpstr2;
           rm(theEnv,tmpstr, strlen(tmpstr)+1);

           return( fvptr );
         }

       *error = true;
       SyntaxErrorMessage(theEnv,"Fuzzy Expression (expecting ']' )");
       rtnFuzzyValue(theEnv,fvptr);
       return( NULL );
     }

   /* next token could be a modifier */
   mptr = FindModifier(theEnv,tempToken->lexemeValue->contents);
   if (mptr != NULL)
     {
      SavePPBuffer(theEnv," "); /* space after the modifier */
      GetToken(theEnv,readSource,tempToken);

      fvptr = ParseModExpr(theEnv,readSource,tempToken,lvp,error);
      if (*error == true) 
          return(NULL);
        
      /* perform the modifier function of the current fuzzy value */
      ModifyFuzzyValue(theEnv,mptr,fvptr);
      return(fvptr);
     }

   /* if not a [ ... ] or a modifier it had better be a primary term */
   return( PrimaryTerm(theEnv,readSource,tempToken,lvp,error) );
}




/***************************************************************/
/* PrimaryTerm:                                                */
/* Returns assert argument in fuzzy_value format.              */
/* Error flag is set to true if an error occurs.               */
/***************************************************************/
static struct fuzzy_value *PrimaryTerm(
  Environment *theEnv,
  const char *readSource,
  struct token *tempToken,
  struct fuzzyLv *lvp,
  bool *error)
{
   struct primary_term *pt;
   struct fuzzy_value *fv;

   if ((pt = FindPrimaryTerm(lvp,tempToken->lexemeValue)) == NULL)
     {
       *error = true;
       SyntaxErrorMessage(theEnv,"Fuzzy Expression (expecting a Primary Term or Modifier)");
       return(NULL);
     }
   else
     {
       fv = CopyFuzzyValue(theEnv,pt->fuzzy_value_description->contents);
       SavePPBuffer(theEnv," ");
       GetToken(theEnv,readSource,tempToken);
       return(fv);
     }
}




/***************************************************************/
/* ModifyFuzzyValue:                                           */
/*                                                             */
/* Takes a fuzzy value and modifies it according by operating  */
/* on each value of the fuzzy set with the function identified */
/* by the modifier                                             */
/*                                                             */
/* Modifies fuzzy_value without making a new one.              */
/***************************************************************/
void ModifyFuzzyValue(
  Environment *theEnv,
  struct modifierListItem *mptr,
  struct fuzzy_value *fv)
{
   char *tmpstr;

   if (fv == NULL)  /*should never happen!! */
     {  return; }

   /* modify the name eg. cold --> very cold */
   tmpstr = modifyName(theEnv,mptr->name,fv->name);
   if (fv->name != NULL) rm(theEnv,fv->name, strlen(fv->name)+1);
   fv->name = tmpstr;

   /* modify the fuzzy set */
   executeModifyFunction(theEnv,fv,mptr);

}



/*************************************************************/
/* modifyName:                                              */
/*************************************************************/
static char *modifyName(
  Environment *theEnv,
  const char *str1, 
  const char *str2)
{
   char *temp;
   size_t str1len = strlen(str1);

   temp = (char *) gm2(theEnv,sizeof(char) * (str1len + strlen(str2) + 2));
   strcpy(temp,str1);
   temp[str1len]   = ' ';
   temp[str1len+1] = '\0';
   strcpy(temp+str1len+1, str2);
   return(temp);
}

/*************************************************************/
/* FindPrimaryTerm: Searches for primary term.               */
/* Returns a pointer to the primary term if found,           */
/* otherwise NULL.                                           */
/*************************************************************/
static struct primary_term *FindPrimaryTerm(
  struct fuzzyLv *lvp,
  CLIPSLexeme *pt_name)
{
   struct primary_term *ptptr;

   ptptr = lvp->primary_term_list;
   while (ptptr != NULL)
     {
      struct fuzzy_value *fvptr = ptptr->fuzzy_value_description->contents;
      if (strcmp(fvptr->name,pt_name->contents) == 0)
        {  return(ptptr);  }

      ptptr = ptptr->next;
     }
   return(NULL);
}

/******************************************************************
    assertParseFuzzySet()

    Returns fuzzy set in expanded expr format for assert
    command.  Parses constant, variable, and function type 
    fuzzy set parameters.
    
    Parses assert of fuzzy sets of the form
    
        (fuzzyvar # (1 0) (5 1) (7 0) )  -- a singleton set OR
        (fuzzyvar # (z 4 8) )            -- a standard set
        
        NOTE: the # is optional !!! This routine expects that the
              current token should be a LPAREN [ '(' ]
        
        numeric values may be expressions if constantsOnly is false
        
        sets onlyConstantsFound to TRUE if no expressions used
        
 ******************************************************************/

static struct expr *assertParseFuzzySet(
  Environment *theEnv,
  const char *readSource,
  struct token *tempToken,
  bool  *error,
  struct deftemplate *theDeftemplate,
  int constantsOnly,
  int *onlyConstantsFound)
{
      struct expr *parse_result = NULL;
      int function_type = -1;
      
      if (tempToken->tknType == LEFT_PARENTHESIS_TOKEN)
        {
          GetToken(theEnv,readSource,tempToken);
          if (tempToken->tknType == SYMBOL_TOKEN) /* check for the s, z, or PI functions */
            {
              const char *tokenStr = tempToken->lexemeValue->contents;
              
              if (strcmp(tokenStr,"S") == 0 ||
                  strcmp(tokenStr,"s") == 0)
                {
                 function_type = S_FUNCTION;
                }
              else if (strcmp(tokenStr,"Z") == 0 ||
                       strcmp(tokenStr,"z") == 0)
                {
                 function_type = Z_FUNCTION;
                }
              else if (strcmp(tokenStr,"PI") == 0 ||
                       strcmp(tokenStr,"pi") == 0 ||
                       strcmp(tokenStr,"Pi") == 0)
                {
                 function_type = PI_FUNCTION;
                }
             }
          if (function_type != -1)
            {
              parse_result = assertParseStandardSet(theEnv,readSource, tempToken, error,
                                                    theDeftemplate, constantsOnly,
                                                    onlyConstantsFound, function_type);
            }
          else
            {
              parse_result = assertParseSingletonSet(theEnv,readSource, tempToken, error,
                                                     theDeftemplate,
                                                     constantsOnly, onlyConstantsFound );
            }
         }  
      else
        {
          *error = true;
          SyntaxErrorMessage(theEnv,"Fuzzy Term (expecting Fuzzy Set description)");
        }
      if (*error == true)
        {
         return(NULL);
        }
        
      return(parse_result);
}


/**********************************************************************
    assertParseStandardSet()
    
    Parses fuzzy sets of the form   (fuzzyvar (S 4 8))
    
    Function_type has aleady been determined as one of PI_FUNCTION, 
    S_FUNCTION, or Z_FUNCTION
 **********************************************************************/
static struct expr *assertParseStandardSet(
  Environment *theEnv,
  const char *readSource,
  struct token *tempToken,
  bool  *error,
  struct deftemplate *theDeftemplate,
  int constantsOnly,
  int *onlyConstantsFound,
  int function_type)
{
    struct expr *top, *deft, *arg1, *arg2;
    *onlyConstantsFound = false;
        
    SavePPBuffer(theEnv," ");

    top = get_struct(theEnv,expr);
    top->value  = NULL;
    top->type = function_type; /* standard fuzzy function */
    top->nextArg = NULL;
        
    deft = GenConstant(theEnv,DEFTEMPLATE_PTR, (void *)theDeftemplate);
    top->argList = deft;

    /* get first parameter */ 
    GetToken(theEnv,readSource, tempToken);
    SavePPBuffer(theEnv," ");
    
    arg1 = tokenToFloatExpression ( theEnv, readSource, tempToken, error, constantsOnly);
    if ( *error == true)
      {
        ReturnExpression(theEnv,top);
        return(NULL);
      }
    else
      {
        deft->nextArg = arg1;    
      }

    /* get 2nd parameter */
    GetToken(theEnv,readSource, tempToken);   
    arg2 = tokenToFloatExpression ( theEnv, readSource, tempToken, error, constantsOnly);
    if ( *error == true)
      {
        ReturnExpression(theEnv,top);
        return(NULL);
      }
    else
      {
        arg1->nextArg = arg2;
      }
        
    GetToken(theEnv,readSource,tempToken);
    if (tempToken->tknType == RIGHT_PARENTHESIS_TOKEN)
      {    
        if (arg1->type == FLOAT_TYPE && arg2->type == FLOAT_TYPE )
          { 
            *onlyConstantsFound = true; 
          }
        GetToken(theEnv,readSource,tempToken);
        return(top);
      }
    else
      {
        *error = true;
        SyntaxErrorMessage(theEnv,"standard set (expecting ')' )");
        ReturnExpression(theEnv,top);
        return(NULL);
      }
}

/**********************************************************************
    assertParseSingletonSet()

    Parses fuzzy sets of form   (fuzzyvar (0 0) (5 .5) (7 1) (12 0))
    
 **********************************************************************/
static struct expr *assertParseSingletonSet(
  Environment *theEnv,
  const char *readSource,
  struct token *tempToken,
  bool  *error,
  struct deftemplate *theDeftemplate,
  int constantsOnly,
  int *onlyConstantsFound)
{
   
    int  count;    /* number of (x,y) pairs input */
    struct expr *top, *first, *next, *deft, *countExpr;

    *onlyConstantsFound = true;
    
    /********************************************************
     Initialize start of linked list and assign first element.
     Token should be x coordinate.
     ********************************************************/
    first  = tokenToFloatExpression (theEnv,readSource, tempToken, error, constantsOnly);
    if (*error == true)
    {
        SyntaxErrorMessage(theEnv,"Singleton specification (Error in parsing Fuzzy Set x coordinate)");
        return(NULL);
    }
    next = first;
    if (next->type != FLOAT_TYPE)
        *onlyConstantsFound = false;

    count = 0;

    while (true)
      {
        /*************************************************
        Get the next token, which should be y coordinate
        *************************************************/
        SavePPBuffer(theEnv," ");
        GetToken(theEnv,readSource,tempToken);
        next->next_arg = tokenToFloatExpression(theEnv,readSource, tempToken, error, constantsOnly);
        if (*error == true)
          {
            SyntaxErrorMessage(theEnv,"Singleton specification (Error in parsing Fuzzy Set y coordinate)");
            ReturnExpression(theEnv,first);
            return(NULL);
          }
        next = next->nextArg;
        if (next->type != FLOAT_TYPE)
            *onlyConstantsFound = false;
 
        /*********************************************************************
        Get the next token, which should be closing bracket for the(x,y) pair
        ********************************************************************/
        GetToken(theEnv,readSource,tempToken);

        if (tempToken->tknType == RIGHT_PARENTHESIS_TOKEN)
          {
            count++;
            SavePPBuffer(theEnv," ");
          }     
        else
          {
             *error = true;
             SyntaxErrorMessage(theEnv,"Singleton specification (Expected ')' )");
            ReturnExpression(theEnv,first);
            return(NULL);
          }
        /**************************************************************
        Get the next token, which should be either a closing bracket
        indicating the end of the set, or an opening bracket indicating
        the start of another (x,y) pair.
        *************************************************************/
        GetToken(theEnv,readSource,tempToken);
        if ((tempToken->tknType == RIGHT_PARENTHESIS_TOKEN) ||
            (tempToken->tknType == STOP_TOKEN))
          {
            top = get_struct(theEnv,expr);
            top->type = SINGLETON_EXPRESSION;
            top->value = NULL;
            top->nextArg = NULL;

            deft = GenConstant(theEnv,DEFTEMPLATE_PTR, (void *)theDeftemplate);
            top->argList = deft;

            /* put the count of the x,y pairs as the 2nd arg in list of args */
            countExpr = GenConstant(theEnv,INTEGER_TYPE,CreateInteger(theEnv,count));
            deft->nextArg = countExpr;
            countExpr->nextArg = first;
        
               return(top);
          }
          else if (tempToken->tknType != LEFT_PARENTHESIS_TOKEN)
          {
             *error = true;
             SyntaxErrorMessage(theEnv,"Singleton specification (Expected '(' )");
            ReturnExpression(theEnv,first);
             return(NULL);
             }
            
        /************************************************
        Get next token, which should be x coordinate.
        ************************************************/
        SavePPBuffer(theEnv," ");
        GetToken(theEnv,readSource, tempToken);
        next->next_arg = tokenToFloatExpression (theEnv,readSource, tempToken, error, constantsOnly);
        if (*error)
          {
            SyntaxErrorMessage(theEnv,"Singleton specification (Error in parsing Fuzzy Set x coordinate)");
            ReturnExpression(theEnv,first);
            return(NULL);
          }
        
        next = next->nextArg;
        if (next->type != FLOAT_TYPE)
                *onlyConstantsFound = false;

    } /* end of while (true)  */
}


/**********************************************************************
    getConstantFuzzyValue()

    Given a singleton or standard fuzzy value in expanded expr format,
    this function evaluates the parameter expressions and returns 
    a fuzzy value structure.
 **********************************************************************/
struct fuzzy_value *getConstantFuzzyValue(
  Environment *theEnv,
  struct expr *top,
  bool *error)
{
    struct fuzzy_value *new_fv = NULL;
    
    if (top->type == PI_FUNCTION ||
        top->type == Z_FUNCTION  ||
        top->type == S_FUNCTION)
      {
        new_fv = convertStandardSet (theEnv, top, error );
        if (*error)
            return (NULL);
      }
    else if (top->type == SINGLETON_EXPRESSION)
      {
        new_fv = convertSingletonSet ( theEnv, top, error );
        if (*error == true)
            return (NULL);
        
      }
    return(new_fv);
}

/**********************************************************************
    convertStandardSet()

    Evaluates parameters of a standard fuzzy value and returns a 
    fuzzy value structure.
    
    Assumes that top->type is one of PI_FUNCTION, S_FUNCTION, or
    Z_FUNCTION and that top->argList points to 4 arguments
    
       arg 1    ptr to the deftemplate associated with the FV (DEFTEMPLATE_PTR)
       arg 2    alpha value
       arg 3    value depends on function type

    NOTE: this function is similar to the function parseStandardFuzzyValue
          in fuzzypsr.c -- if this is changed consider changes to that
          routine as well

 **********************************************************************/
static struct fuzzy_value *convertStandardSet(
  Environment *theEnv,
  struct expr *top,
  bool *error)
{
    struct expr *next;
    struct fuzzy_value *fv;
    double xtolerance;
    struct deftemplate *deftPtr;
    struct fuzzyLv *fzLv;
    
    double from, to, alfa, beta = 0.0, gamma;
    int function_type = top->type;

    /* get the 1st parameter  - deftemplate ptr (fuzzy) */
    next = top->argList;
    deftPtr = (struct deftemplate *)next->value;
    fzLv = deftPtr->fuzzyTemplate;
    if (fzLv == NULL) /* should never happen */
      { *error = true;
        WriteString(theEnv,STDERR,
               "Standard Function (PI, S or Z) has no Fuzzy Deftemplate (possible internal error");
        return(NULL);
      }
    from = fzLv->from;
    to = fzLv->to;

    /* get the 2rd parameter  - alpha */
    next = next->nextArg;    
    expressionToFloat(theEnv,next, &alfa, error);
    if (*error == true)
        return(NULL);
    
    /* We want to allow specifications for Standard functions and singleton sets
       to NOT be thrown out with an error message due to a floating point
       representation problem so we will allow the points to be considered
       within the universe of discourse (range) if very close.

       FUZZY_TOLERANCE is defined in constant.h
    */
    xtolerance = ((to - from) >= 1.0) ? FUZZY_TOLERANCE : (to-from) * FUZZY_TOLERANCE;

    if (function_type == PI_FUNCTION)
      {
        if (alfa < 0.0)
          {
            *error = true;
            WriteString(theEnv,STDERR,"PI function 1st parameter must be >= 0\n");
            return(NULL);
          }
        else 
          { 
            beta = alfa; 
          }
      }
    else if (alfa < from)
      {
        if (from-alfa > xtolerance)
          {
            *error = true;
            WriteString(theEnv,STDERR,"S or Z function 1st parameter out of range (too small)\n");
            return(NULL);
          }
        alfa = from;
      }
     else if (alfa > to)
      {
        if (alfa-to < xtolerance)
          {
            *error = true;
            WriteString(theEnv,STDERR,"S or Z function 1st parameter out of range (too large)\n");
            return(NULL);
          }
        alfa = to;
      }
       
    /* get the 3rd parameter */
    next = next->next_arg;
    
    expressionToFloat(theEnv,next, &gamma, error);
    if (*error == true)
        return(NULL);

    if  (function_type == PI_FUNCTION)
      { 
        if ((gamma > to) || (gamma < from))
           {
             *error = true;
             WriteString(theEnv,STDERR,"PI function produces x values out of range\n");
             return(NULL);
           }    
        else if ((gamma - beta) < from)
           {
             if (from - (gamma - beta) > xtolerance)
               {
                 *error = true;
                 WriteString(theEnv,STDERR,"PI function produces x values too small\n");
                 return(NULL);
               }
              beta = gamma - from;
           }    
        else if ((gamma + beta) > to)
           {
             if (gamma + beta - to > xtolerance)
               {
                 *error = true;
                 WriteString(theEnv,STDERR,"PI function produces x values too large\n");
                 return(NULL);
               }
              beta = to - gamma;
           }    
      }
    else if (gamma < alfa)
      {
        *error = true;
        WriteString(theEnv,STDERR,"S or Z function 2nd parameter must be >= 1st parameter\n");
        return(NULL);
      }
    else if (gamma > to)
      {
        if (gamma-to > xtolerance)
          {
            *error = true;
            WriteString(theEnv,STDERR,"S or Z function 2nd parameter out of range (too large)\n");
            return(NULL);
          }
        gamma = to;
      }
    
    /* Construct the fuzzy value -- Get_S_Z_or_PI_FuzzyValue in fuzzypsr.c */

    fv = Get_S_Z_or_PI_FuzzyValue(theEnv,alfa, beta, gamma, function_type);
    fv->whichDeftemplate = deftPtr;
           
    return (fv);
}


/**********************************************************************
    convertSingletonSet()

    Evaluates parameters of a singleton fuzzy set and returns a 
    fuzzy set structure.
    Assumes that top points to an argList that has:
    
        1st arg is ptr to the deftemplate associated with the FV (DEFTEMPLATE_PTR)
        2nd arg is n    (number of fuzzy set pairs) (will be a constant integer)
        3rd, 4th args are 1st x,y pair
        5th, 6th args are 2nd x,y pair
        etc.

    NOTE: this function is similar to the function parseSingletonFuzzyValue
          in fuzzypsr.c -- if this is changed consider changes to that
          routine as well

 **********************************************************************/
static struct fuzzy_value *convertSingletonSet(
  Environment *theEnv,
  struct expr *top,
  bool *error)
{
    struct expr *next;
    struct fuzzy_value *fv;
    int num, i, numpairs_retrieved;
    double newx, newy, previous;
    double from, to;
    double xtolerance;
    struct deftemplate *deftPtr;
    struct fuzzyLv *fzLv;
    
    /* get the 1st parameter  - deftemplate ptr (fuzzy) */
    next = top->argList;
    deftPtr = (struct deftemplate *)next->value;
    fzLv = deftPtr->fuzzyTemplate;
    if (fzLv == NULL) /* should never happen */
      { *error = true;
        WriteString(theEnv,STDERR,
               "Standard Function (PI, S or Z) has no Fuzzy Deftemplate (possible internal error");
        return(NULL);
      }
    from = fzLv->from;
    to = fzLv->to;

    /**************************************************************
     Initialize the fuzzy set - 2nd parameter gives required size
     **************************************************************/
    next = next->nextArg;
    expressionToInteger(theEnv,next, &num, error);
    if (*error == true)   /* should never happen */
        return(NULL);

    fv = get_struct(theEnv,fuzzy_value);
    fv->name = (char *) gm2(theEnv,4);
    strcpy(fv->name, "???");
    fv->whichDeftemplate = deftPtr;

    fv->n = num;
    fv->maxn = num;
    fv->x = FgetArray(theEnv,num);
    fv->y = FgetArray(theEnv,num);

    previous = from - 1.0;
    next = next->nextArg;
    
    /* We want to allow specifications for Standard functions and singleton sets
       to NOT be thrown out with an error message due to a floating point
       representation problem so we will allow the points to be considered
       within the universe of discourse (range) if very close.

       FUZZY_TOLERANCE is defined in constant.h
    */
    xtolerance = ((to - from) >= 1.0) ? FUZZY_TOLERANCE : (to-from) * FUZZY_TOLERANCE;

    i = 0; /* index into the array for storing values */
    numpairs_retrieved = 0; /* count of the pairs obtained from the expression */
    
    while ( numpairs_retrieved < num && next != NULL )
     {
        /************************************************
         Next Expression should be x coordinate.
         ************************************************/
        expressionToFloat(theEnv,next, &newx, error);
        if (*error == false)
          {
            if (newx > to)
              {
                if (newx-to > xtolerance)
                  {
                    *error = true;
                    WriteString(theEnv,STDERR,"X coordinate of Fuzzy Set out of range (too large)\n");
                  }
                newx = to;
              }
            else if (newx < from)
              {
                if (from-newx > xtolerance)
                  {
                    *error = true;
                    WriteString(theEnv,STDERR,"X coordinate of Fuzzy Set out of range (too small)\n");
                  }
                newx = from;
              }
 
            /* cannot have x value less than a previous one (if close we
               can consider it the same as previous -- rounding errors
               may have occurred)
            */
            if (newx < previous)
              {
               if (previous - newx > FUZZY_TOLERANCE)
                 {
                   *error = true;
                   WriteString(theEnv,STDERR,"(x,y) pairs should be in increasing x order in Fuzzy Set\n");
                   WriteString(theEnv,STDERR,"      and successive x values must not be the same\n");
                 }
               else
                 newx = previous;
              }
          }
        if (*error == true)
          {
            rtnFuzzyValue(theEnv,fv);
            return(NULL);
          }
        fv->x[i] = newx;
          previous = newx;
        
        /***********************************************
         Next Expression should be y coordinate.
         ***********************************************/
        next = next->nextArg;
        if ( next == NULL )
        {    
            *error = true;
            WriteString(theEnv,STDERR,"Y coordinate of fuzzy set missing (possible internal error)\n");
            rtnFuzzyValue(theEnv,fv);
            return(NULL);
        }
        expressionToFloat(theEnv,next, &newy, error);
        if (*error == false)
          {
              if (newy < 0.0)
              {
                if (newy < -FUZZY_TOLERANCE)
                  {
                    *error = true;
                    WriteString(theEnv,STDERR,"Fuzzy membership value (y coordinate) must be >= 0.0\n");
                  }
                newy = 0.0;
              }
              if (newy > 1.0)
              {
                if (newy-1.0 > FUZZY_TOLERANCE)
                  {
                    *error = true;
                    WriteString(theEnv,STDERR,"Fuzzy membership value (y coordinate) must be <= 1.0\n");
                  }
                newy = 1.0;
              }
          }
        if (*error == true)
          {
            rtnFuzzyValue(theEnv,fv);
            return(NULL);
          }
          
        /* if this point same as last don't store it */
        if (i==0 || !FZ_EQUAL(newx, fv->x[i-1]) || !FZ_EQUAL(newy,fv->y[i-1]))
          {
           /* when adding a y value make sure it is NOT the same as 1st value if
              it is the second one in the array OR that it is not same as previous 2
              if it is past the 3rd pair (ie. i > 2)
           */
           if ( (i == 1 && newy == fv->y[0]) ||
                (i > 2 && newy == fv->y[i-1] && newy == fv->y[i-2]) )
             {
               i--;
               fv->x[i] = newx;
               if (i == 0)
                  previous = from - 1.0;
             }
           
           fv->y[i] = newy;
           i++;
          }

          /* if last 3 points all have same x values and all y values
             are in increasing order or all in decreasing order, then
             replace 2nd one with the last one. If last 4 are the same
             replace 2nd last one with last one.
          */
          if (i >2 && newx == fv->x[i-2] && newx == fv->x[i-3])
            {
             if ((newy > fv->y[i-2] && fv->y[i-2] > fv->y[i-3]) ||
                 (newy < fv->y[i-2] && fv->y[i-2] < fv->y[i-3]) ||
                 (i > 3 && newx == fv->x[i-4])
                )
               { i--; fv->y[i-1] = fv->y[i]; }
            }
        next = next->nextArg;
        numpairs_retrieved++;
      }
    if ( numpairs_retrieved != num || next != NULL)
      {
        /* impossible error ??? */
        *error = true;
        WriteString(theEnv,STDERR,"Fuzzy set - incorrect number of (x,y) pairs - internal error\n");
        rtnFuzzyValue(theEnv,fv);
          return(NULL);
      }

    /* finally if more than 2 pairs check that the last 2 are not the same  -- if so
       just discard the last one */
    if (i > 2 && fv->y[i-1] == fv->y[i-2])
       i--;
      
    fv->n = i;

    if ((num - i) > 5)
       CompactFuzzyValue( theEnv,fv );
    
    return(fv);
}

/**********************************************************************
    expressionToFloat()

    Given an expr structure which has been formed by tokenToFloatExpression(),
    this function evaluates the expression and returns the float value.
 **********************************************************************/
static void expressionToFloat(
  Environment *theEnv,
  struct expr *exprPtr,
  double *result,
  bool *error)
{

  UDFValue exprValue;
  unsigned short type;
  
  EvaluateExpression( theEnv, exprPtr, &exprValue );
  type = exprValue.header->type;
  
  if (type == INTEGER_TYPE)
     *result = (double) exprValue.integerValue->contents;
     
  else if (type == FLOAT_TYPE)
     *result = exprValue.floatValue->contents;
     
  else 
    {
      *error = true;
      WriteString(theEnv,STDERR,"Fuzzy set value (expecting FLOAT value)\n");
    }
}

/**********************************************************************
    expressionToInteger()

    Given an expr structure which has been formed by tokenToFloatExpression(),
    this function evaluates the expression and returns the integer value.
 **********************************************************************/
static void expressionToInteger(
  Environment *theEnv,
  struct expr *exprPtr,
  int *result,
  bool *error)
{

  UDFValue exprValue;
  unsigned short type;
  
  EvaluateExpression( theEnv, exprPtr, &exprValue );
  type = exprValue.header->type;
  
  if (type == INTEGER_TYPE)
     *result = (int) exprValue.integerValue->contents;
     
  else 
    {
      *error = true;
      WriteString(theEnv,STDERR,"Fuzzy set internal evaluation (expecting int value)\n");
    }
}




/************************************************************************
    FUNCTIONS FOR ALLOCATING AND DE-ALLOCATING MEMORY 
************************************************************************/

/************************************************************************
    FgetArray(length)
 
    Allocates memory for a floating point array of size "length" and
    returns a pointer to the allocated array.
 ************************************************************************/
double *FgetArray ( 
  Environment *theEnv,
  int length)
{
    double *p;

    /* Use CLIPS memory management function gm1() */
    p = (double *) gm1 (theEnv, length * (sizeof(double)) );
    return (p);
}

/************************************************************************
    FrtnArray(p, length )
    
    Deallocates memory of floating point array p of size "length".
 ************************************************************************/
void FrtnArray (
  Environment *theEnv,
  double *p,
  int length)
{
    rm ( theEnv, p, length * (sizeof(double)) );    /* a CLIPS memory function */
}

/************************************************************************
    IgetArray(length)
 
    Allocates memory for an integer array of size "length" and
    returns a pointer to the allocated array.
 ************************************************************************/
int *IgetArray (
  Environment *theEnv,
  int length)
{
    int *p;

    /* Use CLIPS memory management function gm1() */
    p = (int *) gm1 ( theEnv, length * (sizeof(int)) );
    return (p);
}

/************************************************************************
    IrtnArray(p, length )
    
    Deallocates memory of integer array p of size "length".
 ************************************************************************/
void IrtnArray ( 
  Environment *theEnv,
  int *p,
  int length)
{
    rm ( theEnv, p, length * (sizeof(int)) );    /* a CLIPS memory function */
}

/************************************************************************
    CgetArray(length)
 ************************************************************************/
char *CgetArray (
  Environment *theEnv,
  int length)
{
    char *p = NULL;
    p = (char *) gm1 ( theEnv, length * (sizeof (char)) );
    return (p);
}

/************************************************************************
    CrtnArray ( p, length )
 ************************************************************************/
void CrtnArray (
Environment *theEnv,
char *p,
int length)
{
    rm ( theEnv, p, length * (sizeof (char) ) );
}



#endif  /* FUZZY_DEFTEMPLATES */


/***********************************************

 Following routine used by FUZZY and CERTAINTY 
 FACTOR code

***********************************************/

#if FUZZY_DEFTEMPLATES || CERTAINTY_FACTORS



/****************************************************************
    tokenToFloatExpression()

    Given the next token, this function
    parses constants, variables and functions and returns
    appropriate expr structures
 ****************************************************************/
struct expr *tokenToFloatExpression ( 
  Environment *theEnv,
  const char *readSource,
  struct token *tempToken,
  bool  *error,
  int constantsOnly)
  {
   struct expr *result=NULL;
   unsigned exprType;

   if (tempToken->tknType == FLOAT_TOKEN ||
       tempToken->tknType == INTEGER_TOKEN)
     {
      /******************************************************
       Constant - FLOAT or INTEGER allowed
       ******************************************************/
      if (tempToken->tknType == INTEGER_TOKEN)
        result = GenConstant(theEnv,FLOAT_TYPE,
                                 CreateFloat(theEnv,(double) tempToken->integerValue->contents));
      else
        result = GenConstant(theEnv,FLOAT_TYPE,
                                 CreateFloat(theEnv,tempToken->floatValue->contents));
            
      return(result);

     }
        
   /*=============================================================*/
   /* If an equal sign or left parenthesis was parsed, then parse */
   /* a function which is to be evaluated to determine the        */
   /* value. The equal sign corresponds to the return value       */
   /* constraint which can be used in LHS fact patterns. The      */
   /* equal sign is no longer necessary on either the LHS or RHS  */
   /* of a rule to indicate that a function is being evaluated to */
   /* determine its value either for assignment or pattern        */
   /* matching.                                                   */
   /*=============================================================*/
   
   if ((tempToken->tknType == SYMBOL_TOKEN) ?
       (strcmp(tempToken->lexemeValue->contents,"=") == 0) : 
       (tempToken->tknType == LEFT_PARENTHESIS_TOKEN))
     {
      if (constantsOnly)
        {
         SyntaxErrorMessage(theEnv,"numeric expression (Constants Only Allowed)");
         *error = true;
         return(NULL);
        }

#if ! RUN_TIME
      if (tempToken->tknType == LEFT_PARENTHESIS_TOKEN)
         result = Function1Parse(theEnv,readSource);
      else 
         result = Function0Parse(theEnv,readSource);
#endif
      if (result == NULL)
        {
         *error = true;
        }
      else
        {
          exprType = ExpressionUnknownFunctionType(result);
          if ((result->type == FCALL) &&
              ! (exprType & NUMBER_BITS))
            {
             SyntaxErrorMessage(theEnv,"numeric expression (Expected numeric result from function)");
             *error = true;
             ReturnExpression(theEnv,result);
             return( NULL );
            }
         }
       
      return(result);
     }

   /*========================================*/
   /* Variables are also allowed as RHS      */
   /* values under some circumstances.       */
   /*========================================*/
   
   if ((tempToken->tknType == SF_VARIABLE_TOKEN)
#if DEFGLOBAL_CONSTRUCT
       || (tempToken->tknType == GBL_VARIABLE_TOKEN) 
#endif
      )
     {
      if (constantsOnly)
        {
         *error = true;
         return(NULL);
        }

      return(GenConstant(theEnv,TokenTypeToType(tempToken->tknType),tempToken->value));
     }

   /*==========================================================*/
   /* If none of the other case have been satisfied, then the  */
   /* token parsed is not appropriate for a numeric expression */
   /*==========================================================*/
   
   SyntaxErrorMessage(theEnv,"singleton or standard fuzzy set (Numeric expression expected)");
   *error = true;
   return(NULL);

}



#endif  /*  FUZZY_DEFTEMPLATES || CERTAINTY_FACTORS  */
