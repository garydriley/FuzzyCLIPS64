   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*             CLIPS Version 6.00  05/12/93            */
   /*                                                     */
   /*             FUZZY REASONING HEADER FILE             */
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

#ifndef _H_fuzzyval

#pragma once

#define _H_fuzzyval

#include "entities.h"
#include "tmpltdef.h"

/*********************************************************************/
/* FUZZY_VALUE STRUCTURE:                                            */
/*  pointer to the deftemplate (fuzzy) associated with fuzzy value   */
/*  name of the fuzzy value (linguistic expression pointer)          */
/*  pointer to the deftemplate (fuzzy) associated with fuzzy value   */
/*  maxn  - size of x and y arrays                                   */
/*  n     - number of elements in x and y in use                     */
/*  x,y   - the membership values                                    */
/*                                                                   */
/*  NOTE: at some time FuzzyValues should become atomic types. We    */
/*        go most of the way by making them hashed etc.              */
/*                                                                   */
/*********************************************************************/
struct fuzzy_value
  {
   TypeHeader header;
   Deftemplate *whichDeftemplate; /* the template (fuzzy) */
   char *name;                    /* the fuzzy value linguistic */
                                  /* expression eg. "very cold" */
   int maxn;
   int n;
   double *x;
   double *y;
  };
  
  


#endif
