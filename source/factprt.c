   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*            CLIPS Version 6.40  10/01/16             */
   /*                                                     */
   /*           FACT RETE PRINT FUNCTIONS MODULE          */
   /*******************************************************/

/*************************************************************/
/* Purpose: Print routines for the fact rete primitives.     */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Bob Orchard (NRCC - Nat'l Research Council of Canada)*/
/*                  (Fuzzy reasoning extensions)             */
/*                  (certainty factors for facts and rules)  */
/*                  (extensions to run command)              */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*      6.30: Removed conditional code for unsupported       */
/*            compilers/operating systems (IBM_MCW,          */
/*            MAC_MCW, and IBM_TBC).                         */
/*                                                           */
/*            Changed integer type/precision.                */
/*                                                           */
/*            Updates to support new struct members.         */
/*                                                           */
/*            Added const qualifiers to remove C++           */
/*            deprecation warnings.                          */
/*                                                           */
/*      6.40: Pragma once and other inclusion changes.       */
/*                                                           */
/*            Added support for booleans with <stdbool.h>.   */
/*                                                           */
/*            Removed use of void pointers for specific      */
/*            data structures.                               */
/*                                                           */
/*************************************************************/

#include <stdio.h>

#include "setup.h"

#if DEFTEMPLATE_CONSTRUCT && DEFRULE_CONSTRUCT

#include "envrnmnt.h"
#include "factgen.h"
#include "prntutil.h"
#include "router.h"
#include "symbol.h"

#include "factprt.h"

/***************************************/
/* PrintFactJNCompVars1: Print routine */
/*   for the FactJNCompVars1 function. */
/***************************************/
void PrintFactJNCompVars1(
  Environment *theEnv,
  const char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factCompVarsJN1Call *hack;

   hack = (struct factCompVarsJN1Call *) ((CLIPSBitMap *) theValue)->contents;
   WriteString(theEnv,logicalName,"(fact-jn-cmp-vars1 ");
   if (hack->pass) WriteString(theEnv,logicalName,"= ");
   else WriteString(theEnv,logicalName,"<> ");

   WriteString(theEnv,logicalName,"p");
   PrintUnsignedInteger(theEnv,logicalName,hack->pattern1 + 1);

   if (hack->p1lhs)
     { WriteString(theEnv,logicalName," L"); }
   else if (hack->p1rhs)
     { WriteString(theEnv,logicalName," R"); }

   WriteString(theEnv,logicalName," s");
   PrintUnsignedInteger(theEnv,logicalName,hack->slot1);

   WriteString(theEnv,logicalName," p");
   PrintUnsignedInteger(theEnv,logicalName,hack->pattern2 + 1);

   if (hack->p2lhs)
     { WriteString(theEnv,logicalName," L"); }
   else if (hack->p2rhs)
     { WriteString(theEnv,logicalName," R"); }

   WriteString(theEnv,logicalName," s");
   PrintUnsignedInteger(theEnv,logicalName,hack->slot2);
   WriteString(theEnv,logicalName,")");
#else
#if MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/***************************************/
/* PrintFactJNCompVars2: Print routine */
/*   for the FactJNCompVars2 function. */
/***************************************/
void PrintFactJNCompVars2(
  Environment *theEnv,
  const char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factCompVarsJN2Call *hack;

   hack = (struct factCompVarsJN2Call *) ((CLIPSBitMap *) theValue)->contents;
   WriteString(theEnv,logicalName,"(fact-jn-cmp-vars2 ");
   if (hack->pass) WriteString(theEnv,logicalName,"= ");
   else WriteString(theEnv,logicalName,"<> ");

   WriteString(theEnv,logicalName,"p");
   PrintUnsignedInteger(theEnv,logicalName,hack->pattern1 + 1);

   if (hack->p1lhs)
     { WriteString(theEnv,logicalName," L"); }
   else if (hack->p1rhs)
     { WriteString(theEnv,logicalName," R"); }

   WriteString(theEnv,logicalName," s");
   PrintUnsignedInteger(theEnv,logicalName,hack->slot1);

   if (hack->fromBeginning1) WriteString(theEnv,logicalName, " b");
   else WriteString(theEnv,logicalName," e");

   WriteString(theEnv,logicalName," f");
   PrintUnsignedInteger(theEnv,logicalName,hack->offset1);

   WriteString(theEnv,logicalName," p");
   PrintUnsignedInteger(theEnv,logicalName,hack->pattern2 + 1);

   if (hack->p2lhs)
     { WriteString(theEnv,logicalName," L"); }
   else if (hack->p2rhs)
     { WriteString(theEnv,logicalName," R"); }

   WriteString(theEnv,logicalName," s");
   PrintUnsignedInteger(theEnv,logicalName,hack->slot2);

   if (hack->fromBeginning2) WriteString(theEnv,logicalName," b");
   else WriteString(theEnv,logicalName," e");

   WriteString(theEnv,logicalName," f");
   PrintUnsignedInteger(theEnv,logicalName,hack->offset2);
   WriteString(theEnv,logicalName,")");
#else
#if MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/***************************************/
/* PrintFactPNCompVars1: Print routine */
/*   for the FactPNCompVars1 function. */
/***************************************/
void PrintFactPNCompVars1(
  Environment *theEnv,
  const char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factCompVarsPN1Call *hack;

   hack = (struct factCompVarsPN1Call *) ((CLIPSBitMap *) theValue)->contents;
   WriteString(theEnv,logicalName,"(fact-pn-cmp-vars ");
   if (hack->pass) WriteString(theEnv,logicalName,"p ");
   else WriteString(theEnv,logicalName,"n ");
   PrintUnsignedInteger(theEnv,logicalName,hack->field1);
   WriteString(theEnv,logicalName," ");
   PrintUnsignedInteger(theEnv,logicalName,hack->field2);
   WriteString(theEnv,logicalName,")");
#else
#if MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/**************************************/
/* PrintFactSlotLength: Print routine */
/*   for the FactSlotLength function. */
/**************************************/
void PrintFactSlotLength(
  Environment *theEnv,
  const char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factCheckLengthPNCall *hack;

   hack = (struct factCheckLengthPNCall *) ((CLIPSBitMap *) theValue)->contents;

   WriteString(theEnv,logicalName,"(slot-length ");
   PrintUnsignedInteger(theEnv,logicalName,hack->whichSlot);
   WriteString(theEnv,logicalName," ");
   if (hack->exactly) WriteString(theEnv,logicalName,"= ");
   else WriteString(theEnv,logicalName,">= ");
   PrintUnsignedInteger(theEnv,logicalName,hack->minLength);
   WriteString(theEnv,logicalName,")");
#else
#if MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/*************************************/
/* PrintFactJNGetVar1: Print routine */
/*   for the FactJNGetvar1 function. */
/*************************************/
void PrintFactJNGetVar1(
  Environment *theEnv,
  const char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factGetVarJN1Call *hack;

   hack = (struct factGetVarJN1Call *) ((CLIPSBitMap *) theValue)->contents;
   WriteString(theEnv,logicalName,"(fact-jn-getvar-1 ");
   if (hack->factAddress) WriteString(theEnv,logicalName,"t ");
   else WriteString(theEnv,logicalName,"f ");
   if (hack->allFields) WriteString(theEnv,logicalName,"t ");
   else WriteString(theEnv,logicalName,"f ");

   WriteString(theEnv,logicalName,"p");
   PrintUnsignedInteger(theEnv,logicalName,hack->whichPattern + 1);
   WriteString(theEnv,logicalName," ");
   PrintUnsignedInteger(theEnv,logicalName,hack->whichField);
   WriteString(theEnv,logicalName," s");
   PrintUnsignedInteger(theEnv,logicalName,hack->whichSlot);

   if (hack->lhs)
     { WriteString(theEnv,logicalName," L"); }
   else if (hack->rhs)
     { WriteString(theEnv,logicalName," R"); }
   WriteString(theEnv,logicalName,")");
#else
#if MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/*************************************/
/* PrintFactJNGetVar2: Print routine */
/*   for the FactJNGetvar2 function. */
/*************************************/
void PrintFactJNGetVar2(
  Environment *theEnv,
  const char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factGetVarJN2Call *hack;

   hack = (struct factGetVarJN2Call *) ((CLIPSBitMap *) theValue)->contents;
   WriteString(theEnv,logicalName,"(fact-jn-getvar-2");

   WriteString(theEnv,logicalName," p");
   PrintUnsignedInteger(theEnv,logicalName,hack->whichPattern + 1);
   WriteString(theEnv,logicalName," s");
   PrintUnsignedInteger(theEnv,logicalName,hack->whichSlot);
   if (hack->lhs)
     { WriteString(theEnv,logicalName," L"); }
   else if (hack->rhs)
     { WriteString(theEnv,logicalName," R"); }
   WriteString(theEnv,logicalName,")");
#else
#if MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/*************************************/
/* PrintFactJNGetVar3: Print routine */
/*   for the FactJNGetVar3 function. */
/*************************************/
void PrintFactJNGetVar3(
  Environment *theEnv,
  const char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factGetVarJN3Call *hack;

   hack = (struct factGetVarJN3Call *) ((CLIPSBitMap *) theValue)->contents;
   WriteString(theEnv,logicalName,"(fact-jn-getvar-3 ");
   if (hack->fromBeginning) WriteString(theEnv,logicalName,"t ");
   else WriteString(theEnv,logicalName,"f ");
   if (hack->fromEnd) WriteString(theEnv,logicalName,"t ");
   else WriteString(theEnv,logicalName,"f ");

   PrintUnsignedInteger(theEnv,logicalName,hack->beginOffset);
   WriteString(theEnv,logicalName," ");
   PrintUnsignedInteger(theEnv,logicalName,hack->endOffset);
   WriteString(theEnv,logicalName," ");
   PrintUnsignedInteger(theEnv,logicalName,hack->whichSlot);

   WriteString(theEnv,logicalName," p");
   PrintUnsignedInteger(theEnv,logicalName,hack->whichPattern + 1);

   if (hack->lhs)
     { WriteString(theEnv,logicalName," L"); }
   else if (hack->rhs)
     { WriteString(theEnv,logicalName," R"); }

   WriteString(theEnv,logicalName,")");
#else
#if MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/*************************************/
/* PrintFactPNGetVar1: Print routine */
/*   for the FactPNGetvar1 function. */
/*************************************/
void PrintFactPNGetVar1(
  Environment *theEnv,
  const char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factGetVarPN1Call *hack;

   hack = (struct factGetVarPN1Call *) ((CLIPSBitMap *) theValue)->contents;
   WriteString(theEnv,logicalName,"(fact-pn-getvar-1 ");
   if (hack->factAddress) WriteString(theEnv,logicalName,"t ");
   else WriteString(theEnv,logicalName,"f ");
   if (hack->allFields) WriteString(theEnv,logicalName,"t F");
   else WriteString(theEnv,logicalName,"f F");

   PrintUnsignedInteger(theEnv,logicalName,hack->whichField);
   WriteString(theEnv,logicalName," S");
   PrintUnsignedInteger(theEnv,logicalName,hack->whichSlot);
   WriteString(theEnv,logicalName,")");
#else
#if MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/*************************************/
/* PrintFactPNGetVar2: Print routine */
/*   for the FactPNGetvar2 function. */
/*************************************/
void PrintFactPNGetVar2(
  Environment *theEnv,
  const char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factGetVarPN2Call *hack;

   hack = (struct factGetVarPN2Call *) ((CLIPSBitMap *) theValue)->contents;
   WriteString(theEnv,logicalName,"(fact-pn-getvar-2 S");
   PrintUnsignedInteger(theEnv,logicalName,hack->whichSlot);
   WriteString(theEnv,logicalName,")");
#else
#if MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/*************************************/
/* PrintFactPNGetVar3: Print routine */
/*   for the FactPNGetvar3 function. */
/*************************************/
void PrintFactPNGetVar3(
  Environment *theEnv,
  const char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factGetVarPN3Call *hack;

   hack = (struct factGetVarPN3Call *) ((CLIPSBitMap *) theValue)->contents;
   WriteString(theEnv,logicalName,"(fact-pn-getvar-3 ");

   if (hack->fromBeginning) WriteString(theEnv,logicalName,"t ");
   else WriteString(theEnv,logicalName,"f ");
   if (hack->fromEnd) WriteString(theEnv,logicalName,"t B");
   else WriteString(theEnv,logicalName,"f B");

   PrintUnsignedInteger(theEnv,logicalName,hack->beginOffset);
   WriteString(theEnv,logicalName," E");
   PrintUnsignedInteger(theEnv,logicalName,hack->endOffset);
   WriteString(theEnv,logicalName," S");
   PrintUnsignedInteger(theEnv,logicalName,hack->whichSlot);
   WriteString(theEnv,logicalName,")");
#else
#if MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/***************************************/
/* PrintFactPNConstant1: Print routine */
/*   for the FactPNConstant1 function. */
/***************************************/
void PrintFactPNConstant1(
  Environment *theEnv,
  const char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factConstantPN1Call *hack;

   hack = (struct factConstantPN1Call *) ((CLIPSBitMap *) theValue)->contents;

   WriteString(theEnv,logicalName,"(fact-pn-constant1 ");

   PrintUnsignedInteger(theEnv,logicalName,hack->whichSlot);

   if (hack->testForEquality) WriteString(theEnv,logicalName," = ");
   else WriteString(theEnv,logicalName," != ");

   PrintAtom(theEnv,logicalName,GetFirstArgument()->type,GetFirstArgument()->value);
   WriteString(theEnv,logicalName,")");
#else
#if MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/***************************************/
/* PrintFactPNConstant2: Print routine */
/*   for the FactPNConstant2 function. */
/***************************************/
void PrintFactPNConstant2(
  Environment *theEnv,
  const char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factConstantPN2Call *hack;

   hack = (struct factConstantPN2Call *) ((CLIPSBitMap *) theValue)->contents;

   WriteString(theEnv,logicalName,"(fact-pn-constant2 ");

   PrintUnsignedInteger(theEnv,logicalName,hack->whichSlot);

   WriteString(theEnv,logicalName," ");

   PrintUnsignedInteger(theEnv,logicalName,hack->offset);

   if (hack->testForEquality) WriteString(theEnv,logicalName," = ");
   else WriteString(theEnv,logicalName," != ");

   PrintAtom(theEnv,logicalName,GetFirstArgument()->type,GetFirstArgument()->value);
   WriteString(theEnv,logicalName,")");
#else
#if MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

#if FUZZY_DEFTEMPLATES

/***********************************************************************/
/* PrintPNFUZZY_VALUE: Print routine for the PN_FUZZY_VALUE function.  */
/***********************************************************************/
void PrintPNFUZZY_VALUE(
  Environment *theEnv,
  const char *logicalName,
  void *theValue)
  {
#if DEVELOPER

   WriteString(theEnv,logicalName,"(fact-pn-fuzzy_value ");

   PrintAtom(theEnv,logicalName,GetFirstArgument()->type,GetFirstArgument()->value);
   WriteString(theEnv,logicalName,")");
#endif
  }

#endif

#endif /* DEFTEMPLATE_CONSTRUCT && DEFRULE_CONSTRUCT */


