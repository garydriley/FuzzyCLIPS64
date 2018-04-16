   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*             CLIPS Version 6.00  05/12/93            */
   /*                                                     */
   /*             DEFFUZZY PARSER HEADER FILE             */
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

#ifndef _H_fuzzypsr

#pragma once

#define _H_fuzzypsr

#include "tmpltdef.h"
#include "scanner.h"

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _FUZZYPSR_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   struct fuzzyLv                *ParseFuzzyTemplate(Environment *theEnv,const char *readSource,
                                                     struct token *inputToken,
                                                     bool *DeftemplateError);
   void                           RtnFuzzyTemplate(struct fuzzyLv *lv);
   void                           rtnFuzzyValue(Environment *theEnv,struct fuzzy_value *fv);
   void                           InstallFuzzyValue(CLIPSFuzzyValue *fv);
   void                           DeinstallFuzzyValue(Environment *theEnv,CLIPSFuzzyValue *fv);
   void                           InstallFuzzyTemplate(struct deftemplate *theDeftemplate);
   void                           DeinstallFuzzyTemplate(Environment *theEnv,struct fuzzyLv *fzTemplate);
   double                         sFunction(double x, double alfa, double beta, double gamma);
   void                           Init_S_Z_PI_yvalues(Environment *theEnv);
   struct fuzzy_value            *Get_S_Z_or_PI_FuzzyValue(Environment *theEnv,double alfa, double beta,
                                                           double gamma, int function_type);

#endif

