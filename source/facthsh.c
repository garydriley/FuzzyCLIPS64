   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*            CLIPS Version 6.40  12/30/16             */
   /*                                                     */
   /*                 FACT HASHING MODULE                 */
   /*******************************************************/

/*************************************************************/
/* Purpose: Provides routines for maintaining a fact hash    */
/*   table so that duplication of facts can quickly be       */
/*   determined.                                             */
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
/*      6.24: Removed LOGICAL_DEPENDENCIES compilation flag. */
/*                                                           */
/*            Renamed BOOLEAN macro type to intBool.         */
/*                                                           */
/*      6.30: Fact hash table is resizable.                  */
/*                                                           */
/*            Changed integer type/precision.                */
/*                                                           */
/*            Added FactWillBeAsserted.                      */
/*                                                           */
/*            Converted API macros to function calls.        */
/*                                                           */
/*      6.40: Pragma once and other inclusion changes.       */
/*                                                           */
/*            Added support for booleans with <stdbool.h>.   */
/*                                                           */
/*            Removed use of void pointers for specific      */
/*            data structures.                               */
/*                                                           */
/*            ALLOW_ENVIRONMENT_GLOBALS no longer supported. */
/*                                                           */
/*            UDF redesign.                                  */
/*                                                           */
/*            Modify command preserves fact id and address.  */
/*                                                           */
/*            Assert returns duplicate fact. FALSE is now    */
/*            returned only if an error occurs.              */
/*                                                           */
/*************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "setup.h"

#if DEFTEMPLATE_CONSTRUCT

#include "constant.h"
#include "envrnmnt.h"
#include "factmngr.h"
#include "memalloc.h"
#include "multifld.h"
#include "router.h"
#include "sysdep.h"
#include "utility.h"

#if DEFRULE_CONSTRUCT
#include "lgcldpnd.h"
#endif

#if FUZZY_DEFTEMPLATES
#include "fuzzyutl.h"
#endif

#if CERTAINTY_FACTORS
#include "cfdef.h"
#endif

#include "facthsh.h"

/***************************************/
/* LOCAL INTERNAL FUNCTION DEFINITIONS */
/***************************************/

   static Fact                   *FactExists(Environment *,Fact *,size_t);
   static struct factHashEntry  **CreateFactHashTable(Environment *,size_t);
   static void                    ResizeFactHashTable(Environment *);
   static void                    ResetFactHashTable(Environment *);

/************************************************/
/* HashFact: Returns the hash value for a fact. */
/************************************************/
size_t HashFact(
  Fact *theFact)
  {
   size_t count = 0;

   /*============================================*/
   /* Get a hash value for the deftemplate name. */
   /*============================================*/

   count += theFact->whichDeftemplate->header.name->bucket * 73981;

   /*=================================================*/
   /* Add in the hash value for the rest of the fact. */
   /*=================================================*/

   count += HashMultifield(&theFact->theProposition,0);

   /*================================*/
   /* Make sure the hash value falls */
   /* in the appropriate range.      */
   /*================================*/

   theFact->hashValue = (unsigned long) count;

   /*========================*/
   /* Return the hash value. */
   /*========================*/

   return count;
  }

/**********************************************/
/* FactExists: Determines if a specified fact */
/*   already exists in the fact hash table.   */
/**********************************************/
static Fact *FactExists(
  Environment *theEnv,
  Fact *theFact,
  size_t hashValue)
  {
   struct factHashEntry *theFactHash;

   hashValue = (hashValue % FactData(theEnv)->FactHashTableSize);

   for (theFactHash = FactData(theEnv)->FactHashTable[hashValue];
        theFactHash != NULL;
        theFactHash = theFactHash->next)
     {
      if (theFact->hashValue != theFactHash->theFact->hashValue)
        { continue; }

      if ((theFact->whichDeftemplate == theFactHash->theFact->whichDeftemplate) ?
          MultifieldsEqual(&theFact->theProposition,
                           &theFactHash->theFact->theProposition) : false)
#if CERTAINTY_FACTORS
          /* standard facts -- same templates means same fact --
             perform global contribution calculation for CF and return
             ptr to existing fact indicating fact exists
          */
        {
         changeCFofExistingFact(theEnv,theFact,theFactHash->theFact);
         return(theFactHash->theFact);
        }
#else
        { return(theFactHash->theFact); }
#endif
     }

   return NULL;
  }

/************************************************************/
/* AddHashedFact: Adds a fact entry to the fact hash table. */
/************************************************************/
void AddHashedFact(
  Environment *theEnv,
  Fact *theFact,
  size_t hashValue)
  {
   struct factHashEntry *newhash, *temp;

   if (FactData(theEnv)->NumberOfFacts > FactData(theEnv)->FactHashTableSize)
     { ResizeFactHashTable(theEnv); }

   newhash = get_struct(theEnv,factHashEntry);
   newhash->theFact = theFact;

   hashValue = (hashValue % FactData(theEnv)->FactHashTableSize);

   temp = FactData(theEnv)->FactHashTable[hashValue];
   FactData(theEnv)->FactHashTable[hashValue] = newhash;
   newhash->next = temp;
  }

/******************************************/
/* RemoveHashedFact: Removes a fact entry */
/*   from the fact hash table.            */
/******************************************/
bool RemoveHashedFact(
  Environment *theEnv,
  Fact *theFact)
  {
   size_t hashValue;
   struct factHashEntry *hptr, *prev;

   hashValue = HashFact(theFact);
   hashValue = (hashValue % FactData(theEnv)->FactHashTableSize);

   for (hptr = FactData(theEnv)->FactHashTable[hashValue], prev = NULL;
        hptr != NULL;
        hptr = hptr->next)
     {
      if (hptr->theFact == theFact)
        {
         if (prev == NULL)
           {
            FactData(theEnv)->FactHashTable[hashValue] = hptr->next;
            rtn_struct(theEnv,factHashEntry,hptr);
            if (FactData(theEnv)->NumberOfFacts == 1)
              { ResetFactHashTable(theEnv); }
            return true;
           }
         else
           {
            prev->next = hptr->next;
            rtn_struct(theEnv,factHashEntry,hptr);
            if (FactData(theEnv)->NumberOfFacts == 1)
              { ResetFactHashTable(theEnv); }
            return true;
           }
        }
      prev = hptr;
     }

   return false;
  }

/****************************************************/
/* FactWillBeAsserted: Determines if a fact will be */
/*   asserted based on the duplication settings.    */
/****************************************************/
bool FactWillBeAsserted(
  Environment *theEnv,
  Fact *theFact)
  {
   Fact *tempPtr;
   size_t hashValue;

   if (FactData(theEnv)->FactDuplication) return true;

   hashValue = HashFact(theFact);

   tempPtr = FactExists(theEnv,theFact,hashValue);
   if (tempPtr == NULL) return true;

   return false;
  }

#if FUZZY_DEFTEMPLATES
/*******************************************************/
/* HandleExistingFuzzyFact: Determines if new fact to  */
/*   be added to the fact-list is existing fuzzy fact  */
/*   and if so modifies it accordingly.  Will also     */
/*   retract the existing fact if it finds one         */
/*                                                     */
/*   Returns hashvalue for the NEW fact                */
/*                                                     */
/* NOTE: a fact with fuzzy slots exists already if the */
/*       non-fuzzy slots are identical in value and    */
/*       the fuzzy slots all have the same FUZZY_VALUE */
/*       type -- i.e. both temperature deftemplates    */
/*******************************************************/
size_t HandleExistingFuzzyFact(
  Environment *theEnv,
  Fact **theFact)
  {
   struct fact *tempFact;
   struct factHashEntry *theFactHash;
   size_t hashValue, bucket;
   struct fact *theFactPtr = (struct fact *)*theFact;

   hashValue = HashFact(theFactPtr);
   bucket = (hashValue % FactData(theEnv)->FactHashTableSize);
   
   /* Fuzzy facts never get duplicated ... they just get modified if they
      already exist ... always allow duplication for them...
      do the required modification to the fact if it already exists
   */

   theFactHash = FactData(theEnv)->FactHashTable[bucket];
   tempFact = NULL;

   while (theFactHash != NULL)
     {
      if (theFactPtr->whichDeftemplate == theFactHash->theFact->whichDeftemplate)
        { /* same template definitions for fact */
          if (MultifieldsEqual(&theFactPtr->theProposition,
                               &theFactHash->theFact->theProposition))
            { /* MultiFieldsEqual will compare fuzzy fields to be equal
                 if they are both of the same FUZZY_VALUE type
              */
              tempFact = theFactHash->theFact;
#if CERTAINTY_FACTORS
              /* If facts are the same we need to perform
                 global contribution calculation for CF
              */
              changeCFofNewVsExistingFact(theFactPtr, tempFact);
#endif

              /* fuzzy facts (ie. any fuzzy slots) perform global
                 contribution calculation(s) for each fuzzy slot
              */
              changeValueOfFuzzySlots(theEnv,tempFact, theFactPtr);
                          break;
            }
        }
      theFactHash = theFactHash->next;
     }

   if (tempFact != NULL) /* existing fact! retract it before new one gets asserted */
       Retract(tempFact);

   return(hashValue);
  }

#endif

/*****************************************************/
/* HandleFactDuplication: Determines if a fact to be */
/*   added to the fact-list is a duplicate entry and */
/*   takes appropriate action based on the current   */
/*   setting of the fact-duplication flag.           */
/*****************************************************/
size_t HandleFactDuplication(
  Environment *theEnv,
  Fact *theFact,
  Fact **duplicate,
  long long reuseIndex)
  {
   size_t hashValue;
   *duplicate = NULL;

   hashValue = HashFact(theFact);

   if (FactData(theEnv)->FactDuplication)
     { return hashValue; }

   *duplicate = FactExists(theEnv,theFact,hashValue);
   if (*duplicate == NULL) return hashValue;

   if (reuseIndex == 0)
     { ReturnFact(theEnv,theFact); }
   else
     {
      theFact->nextFact = FactData(theEnv)->GarbageFacts;
      FactData(theEnv)->GarbageFacts = theFact;
      UtilityData(theEnv)->CurrentGarbageFrame->dirty = true;
      theFact->garbage = true;
     }

#if DEFRULE_CONSTRUCT
   AddLogicalDependencies(theEnv,(struct patternEntity *) *duplicate,true);
#endif

   return 0;
  }

/*******************************************/
/* GetFactDuplication: C access routine    */
/*   for the get-fact-duplication command. */
/*******************************************/
bool GetFactDuplication(
  Environment *theEnv)
  {
   return FactData(theEnv)->FactDuplication;
  }

/*******************************************/
/* SetFactDuplication: C access routine    */
/*   for the set-fact-duplication command. */
/*******************************************/
bool SetFactDuplication(
  Environment *theEnv,
  bool value)
  {
   bool ov;

   ov = FactData(theEnv)->FactDuplication;
   FactData(theEnv)->FactDuplication = value;
   return ov;
  }

/**************************************************/
/* InitializeFactHashTable: Initializes the table */
/*   entries in the fact hash table to NULL.      */
/**************************************************/
void InitializeFactHashTable(
   Environment *theEnv)
   {
    FactData(theEnv)->FactHashTable = CreateFactHashTable(theEnv,SIZE_FACT_HASH);
    FactData(theEnv)->FactHashTableSize = SIZE_FACT_HASH;
   }

/*******************************************************************/
/* CreateFactHashTable: Creates and initializes a fact hash table. */
/*******************************************************************/
static struct factHashEntry **CreateFactHashTable(
   Environment *theEnv,
   size_t tableSize)
   {
    unsigned long i;
    struct factHashEntry **theTable;

    theTable = (struct factHashEntry **)
               gm2(theEnv,sizeof (struct factHashEntry *) * tableSize);

    if (theTable == NULL) ExitRouter(theEnv,EXIT_FAILURE);

    for (i = 0; i < tableSize; i++) theTable[i] = NULL;

    return theTable;
   }

/************************/
/* ResizeFactHashTable: */
/************************/
static void ResizeFactHashTable(
   Environment *theEnv)
   {
    unsigned long i, newSize, newLocation;
    struct factHashEntry **theTable, **newTable;
    struct factHashEntry *theEntry, *nextEntry;

    theTable = FactData(theEnv)->FactHashTable;

    newSize = (FactData(theEnv)->FactHashTableSize * 2) + 1;
    newTable = CreateFactHashTable(theEnv,newSize);

    /*========================================*/
    /* Copy the old entries to the new table. */
    /*========================================*/

    for (i = 0; i < FactData(theEnv)->FactHashTableSize; i++)
      {
       theEntry = theTable[i];
       while (theEntry != NULL)
         {
          nextEntry = theEntry->next;

          newLocation = theEntry->theFact->hashValue % newSize;
          theEntry->next = newTable[newLocation];
          newTable[newLocation] = theEntry;

          theEntry = nextEntry;
         }
      }

    /*=====================================================*/
    /* Replace the old hash table with the new hash table. */
    /*=====================================================*/

    rm(theEnv,theTable,sizeof(struct factHashEntry *) * FactData(theEnv)->FactHashTableSize);
    FactData(theEnv)->FactHashTableSize = newSize;
    FactData(theEnv)->FactHashTable = newTable;
   }

/***********************/
/* ResetFactHashTable: */
/***********************/
static void ResetFactHashTable(
   Environment *theEnv)
   {
    struct factHashEntry **newTable;

    /*=============================================*/
    /* Don't reset the table unless the hash table */
    /* has been expanded from its original size.   */
    /*=============================================*/

    if (FactData(theEnv)->FactHashTableSize == SIZE_FACT_HASH)
      { return; }

    /*=======================*/
    /* Create the new table. */
    /*=======================*/

    newTable = CreateFactHashTable(theEnv,SIZE_FACT_HASH);

    /*=====================================================*/
    /* Replace the old hash table with the new hash table. */
    /*=====================================================*/

    rm(theEnv,FactData(theEnv)->FactHashTable,sizeof(struct factHashEntry *) * FactData(theEnv)->FactHashTableSize);
    FactData(theEnv)->FactHashTableSize = SIZE_FACT_HASH;
    FactData(theEnv)->FactHashTable = newTable;
   }

#if DEVELOPER

/****************************************************/
/* ShowFactHashTableCommand: Displays the number of */
/*   entries in each slot of the fact hash table.   */
/****************************************************/
void ShowFactHashTableCommand(
  Environment *theEnv,
  UDFContext *context,
  UDFValue *returnValue)
  {
   unsigned long i, count;
   struct factHashEntry *theEntry;
   char buffer[20];

   for (i = 0; i < FactData(theEnv)->FactHashTableSize; i++)
     {
      for (theEntry =  FactData(theEnv)->FactHashTable[i], count = 0;
           theEntry != NULL;
           theEntry = theEntry->next)
        { count++; }

      if (count != 0)
        {
         gensprintf(buffer,"%4lu: %4d\n",i,count);
         WriteString(theEnv,STDOUT,buffer);
        }
     }
  }

#endif /* DEVELOPER */

#endif /* DEFTEMPLATE_CONSTRUCT */

