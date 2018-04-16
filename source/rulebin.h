   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.40  07/30/16            */
   /*                                                     */
   /*           DEFRULE BSAVE/BLOAD HEADER FILE           */
   /*******************************************************/

/*************************************************************/
/* Purpose: Implements the binary save/load feature for the  */
/*    defrule construct.                                     */
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
/*      6.24: Removed CONFLICT_RESOLUTION_STRATEGIES,        */
/*            DYNAMIC_SALIENCE, and LOGICAL_DEPENDENCIES     */
/*            compilation flags.                             */
/*                                                           */
/*      6.30: Changed integer type/precision.                */
/*                                                           */
/*            Added support for alpha memories.              */
/*                                                           */
/*            Added salience groups to improve performance   */
/*            with large numbers of activations of different */
/*            saliences.                                     */
/*                                                           */
/*      6.40: Removed LOCALE definition.                     */
/*                                                           */
/*            Pragma once and other inclusion changes.       */
/*                                                           */
/*            Removed use of void pointers for specific      */
/*            data structures.                               */
/*                                                           */
/*************************************************************/

#ifndef _H_rulebin

#pragma once

#define _H_rulebin

#if (! RUN_TIME)

#include "cstrcbin.h"
#include "modulbin.h"
#include "network.h"

#if FUZZY_DEFTEMPLATES
/* These structs are added at the end of rule structs
   that have patterns that have FUZZY slot references.
   They record for each fuzzy slot the pattern number
   in the rule and the slot number within the pattern
   as well as the ptr to that fuzzy values hash node.
*/
struct bsaveFzSlotLocator
  {
    unsigned int patternNum;
    unsigned int slotNum : 13;
    long     fvhnPtr;
  };
#endif

struct bsaveDefrule
  {
   struct bsaveConstructHeader header;
   int salience;
   unsigned short localVarCnt;
   unsigned int complexity      : 12;
   unsigned int autoFocus       :  1;
   unsigned long dynamicSalience;
   unsigned long actions;
   unsigned long logicalJoin;
   unsigned long lastJoin;
   unsigned long disjunct;
#if CERTAINTY_FACTORS
   double CF;
   unsigned long  dynamicCF;
#endif
#if FUZZY_DEFTEMPLATES
   double       min_of_maxmins;
   unsigned int lhsRuleType;
   unsigned int numberOfFuzzySlots;
   unsigned long pattern_fv_arrayPtr;
#endif
  };

struct bsavePatternNodeHeader
  {
   unsigned long entryJoin;
   unsigned long rightHash;
   unsigned int singlefieldNode : 1;
   unsigned int multifieldNode : 1;
   unsigned int stopNode : 1;
   unsigned int blocked : 1;
   unsigned int initialize : 1;
   unsigned int marked : 1;
   unsigned int beginSlot : 1;
   unsigned int endSlot : 1;
   unsigned int selector : 1;
  };

struct bsaveDefruleModule
  {
   struct bsaveDefmoduleItemHeader header;
  };

struct bsaveJoinLink
  {
   char enterDirection;
   unsigned long join;
   unsigned long next;
  };

struct bsaveJoinNode
  {
   unsigned int firstJoin : 1;
   unsigned int logicalJoin : 1;
   unsigned int joinFromTheRight : 1;
   unsigned int patternIsNegated : 1;
   unsigned int patternIsExists : 1;
   unsigned int rhsType : 3;
   unsigned int depth : 7;
   unsigned long networkTest;
   unsigned long secondaryNetworkTest;
   unsigned long leftHash;
   unsigned long rightHash;
   unsigned long rightSideEntryStructure;
   unsigned long nextLinks;
   unsigned long lastLevel;
   unsigned long rightMatchNode;
   unsigned long ruleToActivate;
  };

#define RULEBIN_DATA 20

struct defruleBinaryData
  {
   unsigned long NumberOfDefruleModules;
   unsigned long NumberOfDefrules;
   unsigned long NumberOfJoins;
   unsigned long NumberOfLinks;
   unsigned long RightPrimeIndex;
   unsigned long LeftPrimeIndex;
   struct defruleModule *ModuleArray;
   Defrule *DefruleArray;
   struct joinNode *JoinArray;
   struct joinLink *LinkArray;
#if FUZZY_DEFTEMPLATES 
   unsigned long NumberOfPatternFuzzyValues;
   struct fzSlotLocator *PatternFuzzyValueArray;
#endif
  };

#define DefruleBinaryData(theEnv) ((struct defruleBinaryData *) GetEnvironmentData(theEnv,RULEBIN_DATA))

#define BloadDefrulePointer(x,i) ((Defrule *) ((i == ULONG_MAX) ? NULL : &x[i]))
#define BsaveJoinIndex(joinPtr) ((joinPtr == NULL) ? ULONG_MAX :  ((struct joinNode *) joinPtr)->bsaveID)
#define BloadJoinPointer(i) ((struct joinNode *) ((i == ULONG_MAX) ? NULL : &DefruleBinaryData(theEnv)->JoinArray[i]))
#define BsaveJoinLinkIndex(linkPtr) ((linkPtr == NULL) ? ULONG_MAX :  ((struct joinLink *) linkPtr)->bsaveID)
#define BloadJoinLinkPointer(i) ((struct joinLink *) ((i == ULONG_MAX) ? NULL : &DefruleBinaryData(theEnv)->LinkArray[i]))

   void                           DefruleBinarySetup(Environment *);
   void                           UpdatePatternNodeHeader(Environment *,struct patternNodeHeader *,
                                                                 struct bsavePatternNodeHeader *);
   void                           AssignBsavePatternHeaderValues(Environment *,struct bsavePatternNodeHeader *,
                                                                        struct patternNodeHeader *);
   void                          *BloadDefruleModuleReference(Environment *,unsigned long);

#endif /* (! RUN_TIME) */

#endif /* _H_rulebin */





