   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.40  07/30/16            */
   /*                                                     */
   /*         DEFTEMPLATE BSAVE/BLOAD HEADER FILE         */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Brian L. Dantes                                      */
/*      Bob Orchard (NRCC - Nat'l Research Council of Canada)*/
/*                  (Fuzzy reasoning extensions)             */
/*                  (certainty factors for facts and rules)  */
/*                  (extensions to run command)              */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*      6.23: Added support for templates maintaining their  */
/*            own list of facts.                             */
/*                                                           */
/*      6.30: Changed integer type/precision.                */
/*                                                           */
/*            Support for deftemplate slot facets.           */
/*                                                           */
/*      6.40: Removed LOCALE definition.                     */
/*                                                           */
/*            Pragma once and other inclusion changes.       */
/*                                                           */
/*            Removed use of void pointers for specific      */
/*            data structures.                               */
/*                                                           */
/*************************************************************/

#ifndef _H_tmpltbin

#pragma once

#define _H_tmpltbin

#if (! RUN_TIME)

struct bsaveTemplateSlot
  {
   unsigned long slotName;
   unsigned int multislot : 1;
   unsigned int noDefault : 1;
   unsigned int defaultPresent : 1;
   unsigned int defaultDynamic : 1;
   unsigned long constraints;
   unsigned long defaultList;
   unsigned long facetList;
   unsigned long next;
  };

struct bsaveDeftemplate;
struct bsaveDeftemplateModule;

#include "cstrcbin.h"

struct bsaveDeftemplate
  {
   struct bsaveConstructHeader header;
   unsigned long slotList;
   unsigned int implied : 1;
   unsigned int numberOfSlots : 15;
   unsigned long patternNetwork;
#if FUZZY_DEFTEMPLATES
   unsigned int hasFuzzySlots : 1;
   unsigned long fuzzyTemplateList;
#endif
  };
  
#if FUZZY_DEFTEMPLATES

struct bsaveLvPlusUniverse
  {
    double from;
    double to;
    unsigned long unitsName;
    unsigned long ptPtr;
  };

struct bsaveFuzzyPrimaryTerm
  {
    long fuzzyValue; // TBD unsigned
    unsigned long next;
  };

#endif

#include "modulbin.h"

struct bsaveDeftemplateModule
  {
   struct bsaveDefmoduleItemHeader header;
  };

#define TMPLTBIN_DATA 61

#include "tmpltdef.h"

struct deftemplateBinaryData
  {
   Deftemplate *DeftemplateArray;
   unsigned long NumberOfDeftemplates;
   unsigned long NumberOfTemplateSlots;
   unsigned long NumberOfTemplateModules;
   struct templateSlot *SlotArray;
   struct deftemplateModule *ModuleArray;
#if FUZZY_DEFTEMPLATES
   struct primary_term *PrimaryTermArray;
   struct fuzzyLv *LvPlusUniverseArray;
   unsigned long NumberOfFuzzyTemplates;
   unsigned long NumberOfFuzzyPrimaryTerms;
#endif
  };

#define DeftemplateBinaryData(theEnv) ((struct deftemplateBinaryData *) GetEnvironmentData(theEnv,TMPLTBIN_DATA))

#define DeftemplatePointer(i) ((Deftemplate *) (&DeftemplateBinaryData(theEnv)->DeftemplateArray[i]))

#ifndef _H_tmpltdef
#include "tmpltdef.h"
#endif

   void                           DeftemplateBinarySetup(Environment *);
   void                          *BloadDeftemplateModuleReference(Environment *,unsigned long);

#endif /* (! RUN_TIME) */

#endif /* _H_tmpltbin */



