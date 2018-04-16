   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*             CLIPS Version 6.00  05/12/93            */
   /*                                                     */
   /*             FUZZY UTILITY HEADER                    */
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



#ifndef _H_fuzzyutl
#define _H_fuzzyutl


#ifndef _H_factmngr
#include "factmngr.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _FUZZYUTL_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE void                 fcompliment( struct fuzzy_value *fv );
   LOCALE struct fuzzy_value  *funion(Environment *theEnv, struct fuzzy_value *f1, struct fuzzy_value *f2 );
   LOCALE int                  nonintersectiontest( double *Ax, double *Ay, 
                                                    double *Bx, double *By,
                                                    int Asize, int Bsize );
   LOCALE void                 computeFuzzyConsequence(Environment *theEnv, struct fact *new_fact );
   LOCALE void                 changeValueOfFuzzySlots(Environment *theEnv,Fact *fact1,Fact *fact2 );
   LOCALE void                 PrintFuzzyTemplateFact(Environment *theEnv,const char *logName, struct fuzzy_value *fv
#if CERTAINTY_FACTORS
                                                     ,double CF
#endif
                                                     );
   LOCALE void                 PrintFuzzySet(Environment *theEnv,const char *logName, struct fuzzy_value *fv);
   LOCALE double               maxmin_intersect(Environment *theEnv,struct fuzzy_value *f1,
                                                struct fuzzy_value *f2, 
                                                int DoIntersect,
                                                struct fuzzy_value **intersectSet);
   LOCALE struct fuzzy_value  *fintersect(Environment *theEnv,struct fuzzy_value *f1, struct fuzzy_value *f2);
   LOCALE double               max_of_min(Environment *theEnv,struct fuzzy_value *f1, struct fuzzy_value *f2);
   LOCALE int                  FZ_EQUAL(double f1, double f2);


#endif

