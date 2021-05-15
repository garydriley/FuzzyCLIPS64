   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*             CLIPS Version 6.00  05/12/93            */
   /*                                                     */
   /*             FUZZY RHS PARSE HEADER FILE             */
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

#ifndef _H_fuzzyrhs

#pragma once

#define _H_fuzzyrhs

#include "tmpltdef.h"
#include "fuzzymod.h"
#include "fuzzylv.h"
#include "scanner.h"

   struct expr        *ParseAssertFuzzyFact(Environment *theEnv,const char *readSource,
                                            struct token *tempToken,
                                            bool *error, TokenType endType, int constantsOnly,
                                            Deftemplate *theDeftemplate,
                                            int variablesAllowed);
   struct fuzzy_value *ParseLinguisticExpr(Environment *theEnv,const char *readSource,
                                           struct token *tempToken,
                                           struct fuzzyLv *lvp,
                                           bool *error);
   struct fuzzy_value *CopyFuzzyValue(Environment *theEnv,struct fuzzy_value *fv);
   void                CompactFuzzyValue(Environment *theEnv,struct fuzzy_value *fv);
   struct fuzzy_value *getConstantFuzzyValue(Environment *theEnv,struct expr *top, bool *error);
   void                ModifyFuzzyValue(Environment *theEnv,struct modifierListItem *mptr,
                                               struct fuzzy_value *elem);
   double             *FgetArray ( Environment *theEnv, int length );
   void                FrtnArray ( Environment *theEnv, double *p, int length );
   int                *IgetArray ( Environment *theEnv, int length );
   void                IrtnArray ( Environment *theEnv, int *p, int length );
   char               *CgetArray (Environment *theEnv, int length);
   void                CrtnArray (Environment *theEnv, char *p, int length);
   struct expr        *tokenToFloatExpression(Environment *theEnv,const char *readSource,
                                                struct token *tempToken,
                                                bool  *error,
                                                int constantsOnly);

#endif

