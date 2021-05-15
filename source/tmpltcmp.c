   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*            CLIPS Version 6.40  07/30/16             */
   /*                                                     */
   /*          DEFTEMPLATE CONSTRUCTS-TO-C MODULE         */
   /*******************************************************/

/*************************************************************/
/* Purpose: Implements the constructs-to-c feature for the   */
/*    deftemplate construct.                                 */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Brian L. Dantes                                      */
/*      Bob Orchard (NRCC - Nat'l Research Council of Canada)*/
/*                  (Fuzzy reasoning extensions)             */
/*                  (certainty factors for facts and rules)  */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*      6.23: Added support for templates maintaining their  */
/*            own list of facts.                             */
/*                                                           */
/*      6.30: Added support for path name argument to        */
/*            constructs-to-c.                               */
/*                                                           */
/*            Removed conditional code for unsupported       */
/*            compilers/operating systems (IBM_MCW,          */
/*            MAC_MCW, and IBM_TBC).                         */
/*                                                           */
/*            Support for deftemplate slot facets.           */
/*                                                           */
/*            Added code for deftemplate run time            */
/*            initialization of hashed comparisons to        */
/*            constants.                                     */
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

#include "setup.h"

#if DEFTEMPLATE_CONSTRUCT && CONSTRUCT_COMPILER && (! RUN_TIME)

#define SlotPrefix() ArbitraryPrefix(DeftemplateData(theEnv)->DeftemplateCodeItem,2)

#if FUZZY_DEFTEMPLATES
/* add 2 more files to store the Fuzzy template definitions */
#define LvUniversePrefix()  ArbitraryPrefix(DeftemplateData(theEnv)->DeftemplateCodeItem,3)
#define PrimaryTermPrefix() ArbitraryPrefix(DeftemplateData(theEnv)->DeftemplateCodeItem,4)
#endif

#include <stdio.h>

#include "conscomp.h"
#include "cstrncmp.h"
#include "envrnmnt.h"
#include "factcmp.h"
#include "tmpltdef.h"

#if FUZZY_DEFTEMPLATES
#include "fuzzyval.h"
#include "fuzzylv.h"
#include "prntutil.h"
#endif

#include "tmpltcmp.h"

/***************************************/
/* LOCAL INTERNAL FUNCTION DEFINITIONS */
/***************************************/

   static bool                    ConstructToCode(Environment *,const char *,const char *,char *,
                                                  unsigned int,FILE *,unsigned int,unsigned int);
   static void                    SlotToCode(Environment *,FILE *,struct templateSlot *,
                                             unsigned int,unsigned int,unsigned int);
   static void                    DeftemplateModuleToCode(Environment *,FILE *,Defmodule *,unsigned int,
                                                          unsigned int,unsigned int);
#if FUZZY_DEFTEMPLATES
   static void                    DeftemplateToCode(Environment *,FILE *,Deftemplate *,
                                                    unsigned int,unsigned int,unsigned int,unsigned int,unsigned int,unsigned int);
   static void                    CloseDeftemplateFiles(Environment *,FILE *,FILE *,FILE *,FILE *,FILE *,unsigned int);
#else
   static void                    DeftemplateToCode(Environment *,FILE *,Deftemplate *,
                                                    unsigned int,unsigned int,unsigned int,unsigned int);
   static void                    CloseDeftemplateFiles(Environment *,FILE *,FILE *,FILE *,unsigned int);
#endif
   static void                    InitDeftemplateCode(Environment *,FILE *,unsigned int,unsigned int);
#if FUZZY_DEFTEMPLATES
   static void                    LvUniverseToCode(Environment *,FILE *,struct fuzzyLv *,
                                                   unsigned int,unsigned int,unsigned int,unsigned int);
   static void                    primaryTermToCode(Environment *,FILE *,struct primary_term *,
                                                    unsigned int,unsigned int,unsigned int *,unsigned int);
#endif

/*********************************************************/
/* DeftemplateCompilerSetup: Initializes the deftemplate */
/*   construct for use with the constructs-to-c command. */
/*********************************************************/
void DeftemplateCompilerSetup(
  Environment *theEnv)
  {
#if FUZZY_DEFTEMPLATES
   DeftemplateData(theEnv)->DeftemplateCodeItem = AddCodeGeneratorItem(theEnv,"deftemplate",0,NULL,InitDeftemplateCode,ConstructToCode,5);
#else
   DeftemplateData(theEnv)->DeftemplateCodeItem = AddCodeGeneratorItem(theEnv,"deftemplate",0,NULL,InitDeftemplateCode,ConstructToCode,3);
#endif
  }

/*************************************************************/
/* ConstructToCode: Produces deftemplate code for a run-time */
/*   module created using the constructs-to-c function.      */
/*************************************************************/
static bool ConstructToCode(
  Environment *theEnv,
  const char *fileName,
  const char *pathName,
  char *fileNameBuffer,
  unsigned int fileID,
  FILE *headerFP,
  unsigned int imageID,
  unsigned int maxIndices)
  {
   unsigned int fileCount = 1;
   Defmodule *theModule;
   Deftemplate *theTemplate;
   struct templateSlot *slotPtr;
   unsigned int slotCount = 0, slotArrayCount = 0, slotArrayVersion = 1;
   unsigned int moduleCount = 0, moduleArrayCount = 0, moduleArrayVersion = 1;
   unsigned int templateArrayCount = 0, templateArrayVersion = 1;
   FILE *slotFile = NULL, *moduleFile = NULL, *templateFile = NULL;
#if FUZZY_DEFTEMPLATES
   unsigned int lvUniverseArrayCount = 0, lvUniverseArrayVersion = 1;
   unsigned int primaryTermArrayCount = 0, primaryTermArrayVersion = 1;
   FILE *lvUniverseFile = NULL, *primaryTermFile = NULL;
   struct fuzzyLv *lvPtr;
   struct primary_term *primaryTermPtr;
#endif

   /*==================================================*/
   /* Include the appropriate deftemplate header file. */
   /*==================================================*/

   fprintf(headerFP,"#include \"tmpltdef.h\"\n");

   /*=============================================================*/
   /* Loop through all the modules, all the deftemplates, and all */
   /* the deftemplate slots writing their C code representation   */
   /* to the file as they are traversed.                          */
   /*=============================================================*/

   theModule = GetNextDefmodule(theEnv,NULL);

   while (theModule != NULL)
     {
      SetCurrentModule(theEnv,theModule);

      moduleFile = OpenFileIfNeeded(theEnv,moduleFile,fileName,pathName,fileNameBuffer,fileID,imageID,&fileCount,
                                    moduleArrayVersion,headerFP,
                                    "struct deftemplateModule",ModulePrefix(DeftemplateData(theEnv)->DeftemplateCodeItem),
                                    false,NULL);

      if (moduleFile == NULL)
        {
#if FUZZY_DEFTEMPLATES
         CloseDeftemplateFiles(theEnv,moduleFile,templateFile,slotFile,
                               lvUniverseFile,primaryTermFile,maxIndices);
#else
         CloseDeftemplateFiles(theEnv,moduleFile,templateFile,slotFile,maxIndices);
#endif
         return false;
        }

      DeftemplateModuleToCode(theEnv,moduleFile,theModule,imageID,maxIndices,moduleCount);
      moduleFile = CloseFileIfNeeded(theEnv,moduleFile,&moduleArrayCount,&moduleArrayVersion,
                                     maxIndices,NULL,NULL);

      /*=======================================================*/
      /* Loop through each of the deftemplates in this module. */
      /*=======================================================*/

      theTemplate = GetNextDeftemplate(theEnv,NULL);

      while (theTemplate != NULL)
        {
         templateFile = OpenFileIfNeeded(theEnv,templateFile,fileName,pathName,fileNameBuffer,fileID,imageID,&fileCount,
                                         templateArrayVersion,headerFP,
                                         "Deftemplate",ConstructPrefix(DeftemplateData(theEnv)->DeftemplateCodeItem),
                                         false,NULL);
         if (templateFile == NULL)
           {
#if FUZZY_DEFTEMPLATES
            CloseDeftemplateFiles(theEnv,moduleFile,templateFile,slotFile,
                                  lvUniverseFile,primaryTermFile,maxIndices);
#else
            CloseDeftemplateFiles(theEnv,moduleFile,templateFile,slotFile,maxIndices);
#endif
            return false;
           }

#if FUZZY_DEFTEMPLATES
         DeftemplateToCode(theEnv,templateFile,theTemplate,imageID,maxIndices,
                           moduleCount,slotCount,
                           lvUniverseArrayCount, lvUniverseArrayVersion);
#else
         DeftemplateToCode(theEnv,templateFile,theTemplate,imageID,maxIndices,
                        moduleCount,slotCount);
#endif
         templateArrayCount++;
         templateFile = CloseFileIfNeeded(theEnv,templateFile,&templateArrayCount,&templateArrayVersion,
                                          maxIndices,NULL,NULL);

#if FUZZY_DEFTEMPLATES
         /* write out the fuzzyLv with universe */
         lvPtr = theTemplate->fuzzyTemplate;
         if (lvPtr != NULL)
           {
             lvUniverseFile = OpenFileIfNeeded(theEnv,lvUniverseFile,fileName,pathName,fileNameBuffer,fileID,
                                               imageID,&fileCount,
                                               lvUniverseArrayVersion,headerFP,
                                               "struct fuzzyLv",
                                               LvUniversePrefix(),false,NULL);
             if (lvUniverseFile == NULL)
                {
                 CloseDeftemplateFiles(theEnv,moduleFile,templateFile,slotFile,
                                       lvUniverseFile,primaryTermFile,maxIndices);
                 return(0);
                }
              LvUniverseToCode(theEnv,lvUniverseFile,lvPtr,imageID,maxIndices,
                               primaryTermArrayCount,primaryTermArrayVersion);
              lvUniverseArrayCount++;
              lvUniverseFile = CloseFileIfNeeded(theEnv,lvUniverseFile,&lvUniverseArrayCount,
                                                &lvUniverseArrayVersion,
                                                maxIndices,NULL,NULL);

              /* now write out the primaryTerm list*/
              primaryTermPtr = lvPtr->primary_term_list;
              primaryTermFile = OpenFileIfNeeded(theEnv,primaryTermFile,fileName,pathName,fileNameBuffer,fileID,
                                                 imageID,&fileCount,
                                                 primaryTermArrayVersion,headerFP,
                                                 "struct primary_term",
                                                 PrimaryTermPrefix(),false,NULL);
              if (primaryTermFile == NULL)
                {
                 CloseDeftemplateFiles(theEnv,moduleFile,templateFile,slotFile,
                                       lvUniverseFile,primaryTermFile,maxIndices);
                 return(0);
                }

              primaryTermToCode(theEnv,primaryTermFile,primaryTermPtr,imageID,maxIndices,
                                &primaryTermArrayCount,primaryTermArrayVersion);
              primaryTermFile = CloseFileIfNeeded(theEnv,primaryTermFile,&primaryTermArrayCount,
                                               &primaryTermArrayVersion,maxIndices,NULL,NULL);




            } /* end of if (lvPtr != NULL) */
#endif

         /*======================================================*/
         /* Loop through each of the slots for this deftemplate. */
         /*======================================================*/

         slotPtr = theTemplate->slotList;
         while (slotPtr != NULL)
           {
            slotFile = OpenFileIfNeeded(theEnv,slotFile,fileName,pathName,fileNameBuffer,fileID,imageID,&fileCount,
                                        slotArrayVersion,headerFP,
                                       "struct templateSlot",SlotPrefix(),false,NULL);
            if (slotFile == NULL)
              {
#if FUZZY_DEFTEMPLATES
               CloseDeftemplateFiles(theEnv,moduleFile,templateFile,slotFile,
                                     lvUniverseFile,primaryTermFile,maxIndices);
#else
               CloseDeftemplateFiles(theEnv,moduleFile,templateFile,slotFile,maxIndices);
#endif
               return false;
              }

            SlotToCode(theEnv,slotFile,slotPtr,imageID,maxIndices,slotCount);
            slotCount++;
            slotArrayCount++;
            slotFile = CloseFileIfNeeded(theEnv,slotFile,&slotArrayCount,&slotArrayVersion,
                                         maxIndices,NULL,NULL);
            slotPtr = slotPtr->next;
           }

         theTemplate = GetNextDeftemplate(theEnv,theTemplate);
        }

      theModule = GetNextDefmodule(theEnv,theModule);
      moduleCount++;
      moduleArrayCount++;

     }
     
#if FUZZY_DEFTEMPLATES
   CloseDeftemplateFiles(theEnv,moduleFile,templateFile,slotFile,
                         lvUniverseFile,primaryTermFile,maxIndices);
#else
   CloseDeftemplateFiles(theEnv,moduleFile,templateFile,slotFile,maxIndices);
#endif

   return true;
  }

/************************************************************/
/* CloseDeftemplateFiles: Closes all of the C files created */
/*   for deftemplates. Called when an error occurs or when  */
/*   the deftemplates have all been written to the files.   */
/************************************************************/
static void CloseDeftemplateFiles(
  Environment *theEnv,
  FILE *moduleFile,
  FILE *templateFile,
  FILE *slotFile,
#if FUZZY_DEFTEMPLATES
  FILE *lvUniverseFile,
  FILE *primaryTermFile,
#endif
  unsigned int maxIndices)
  {
   unsigned int count = maxIndices;
   unsigned int arrayVersion = 0;

   if (slotFile != NULL)
     {
      count = maxIndices;
      CloseFileIfNeeded(theEnv,slotFile,&count,&arrayVersion,maxIndices,NULL,NULL);
     }

   if (templateFile != NULL)
     {
      count = maxIndices;
      CloseFileIfNeeded(theEnv,templateFile,&count,&arrayVersion,maxIndices,NULL,NULL);
     }

   if (moduleFile != NULL)
     {
      count = maxIndices;
      CloseFileIfNeeded(theEnv,moduleFile,&count,&arrayVersion,maxIndices,NULL,NULL);
     }
     
#if FUZZY_DEFTEMPLATES
   if (lvUniverseFile != NULL)
     {
      count = maxIndices;
      lvUniverseFile = CloseFileIfNeeded(theEnv,lvUniverseFile,&count,&arrayVersion,maxIndices,NULL,NULL);
     }

   if (primaryTermFile != NULL)
     {
      count = maxIndices;
      primaryTermFile = CloseFileIfNeeded(theEnv,primaryTermFile,&count,&arrayVersion,maxIndices,NULL,NULL);
     }
#endif
  }

#if FUZZY_DEFTEMPLATES

/* generate code for the fuzzy deftemplate definitions */

/************************************************************/
/* LvUniverseToCode:                                        */
/************************************************************/
static void LvUniverseToCode(
  Environment *theEnv,
  FILE *lvUniverseFile,
  struct fuzzyLv *lvPtr,
  unsigned int imageID,
  unsigned int maxIndices,
  unsigned int primaryTermArrayCount,
  unsigned int primaryTermArrayVersion)
  {
    fprintf(lvUniverseFile, "{%s, %s, ",
            FloatToString(theEnv,lvPtr->from), FloatToString(theEnv,lvPtr->to));
    PrintSymbolReference(theEnv,lvUniverseFile,lvPtr->units);
    fprintf(lvUniverseFile, ", &%s%d_%d[%d] }",
            PrimaryTermPrefix(), imageID, primaryTermArrayVersion, primaryTermArrayCount);
  }

/************************************************************/
/* primaryTermToCode:                                       */
/************************************************************/
static void primaryTermToCode(
  Environment *theEnv,
  FILE *primaryTermFile,
  struct primary_term *primaryTermPtr,
  unsigned int imageID,
  unsigned int maxIndices,
  unsigned int *primaryTermArrayCount,
  unsigned int primaryTermArrayVersion)
  {
    int count, arrayVersion;
    struct primary_term *nextPtr;

    while (primaryTermPtr != NULL)
       {
         nextPtr = primaryTermPtr->next;
         if ((nextPtr == NULL) && (*primaryTermArrayCount >= maxIndices))
           {
             count = 0;
             arrayVersion = primaryTermArrayVersion+1;
           }
         else
           {
             count = *primaryTermArrayCount+1;
             arrayVersion = primaryTermArrayVersion;
           }

         fprintf(primaryTermFile,"{");
         PrintFuzzyValueReference(theEnv, primaryTermFile, primaryTermPtr->fuzzy_value_description);
         if (nextPtr != NULL)
            fprintf(primaryTermFile,",&%s%d_%d[%d]}",
                    PrimaryTermPrefix(), imageID, arrayVersion, count);

         else
            fprintf(primaryTermFile, ",NULL}");

         *primaryTermArrayCount += 1;
         primaryTermPtr = nextPtr;

         if (primaryTermPtr != NULL)
         fprintf(primaryTermFile,",");
       }
  }

#endif

/*************************************************************/
/* DeftemplateModuleToCode: Writes the C code representation */
/*   of a single deftemplate module to the specified file.   */
/*************************************************************/
static void DeftemplateModuleToCode(
  Environment *theEnv,
  FILE *theFile,
  Defmodule *theModule,
  unsigned int imageID,
  unsigned int maxIndices,
  unsigned int moduleCount)
  {
#if MAC_XCD
#pragma unused(moduleCount)
#endif

   fprintf(theFile,"{");

   ConstructModuleToCode(theEnv,theFile,theModule,imageID,maxIndices,
                         DeftemplateData(theEnv)->DeftemplateModuleIndex,ConstructPrefix(DeftemplateData(theEnv)->DeftemplateCodeItem));

   fprintf(theFile,"}");
  }

/************************************************************/
/* DeftemplateToCode: Writes the C code representation of a */
/*   single deftemplate construct to the specified file.    */
/************************************************************/
static void DeftemplateToCode(
  Environment *theEnv,
  FILE *theFile,
  Deftemplate *theTemplate,
  unsigned int imageID,
  unsigned int maxIndices,
  unsigned int moduleCount,
#if ! FUZZY_DEFTEMPLATES
  unsigned int slotCount)
#else
  unsigned int slotCount,
  unsigned int lvUniverseArrayCount,
  unsigned int lvUniverseArrayVersion)
#endif

  {
   /*====================*/
   /* Deftemplate Header */
   /*====================*/

   fprintf(theFile,"{");

   ConstructHeaderToCode(theEnv,theFile,&theTemplate->header,imageID,maxIndices,
                                  moduleCount,ModulePrefix(DeftemplateData(theEnv)->DeftemplateCodeItem),
                                  ConstructPrefix(DeftemplateData(theEnv)->DeftemplateCodeItem));
   fprintf(theFile,",");

   /*===========*/
   /* Slot List */
   /*===========*/

   if (theTemplate->slotList == NULL)
     { fprintf(theFile,"NULL,"); }
   else
     {
      fprintf(theFile,"&%s%d_%d[%d],",SlotPrefix(),
                 imageID,
                 (slotCount / maxIndices) + 1,
                 slotCount % maxIndices);
     }

   /*==========================================*/
   /* Implied Flag, Watch Flag, In Scope Flag, */
   /* Number of Slots, and Busy Count.         */
   /*==========================================*/

   fprintf(theFile,"%d,0,0,%d,%ld,",theTemplate->implied,theTemplate->numberOfSlots,theTemplate->busyCount);

   /*=================*/
   /* Pattern Network */
   /*=================*/

   if (theTemplate->patternNetwork == NULL)
     { fprintf(theFile,"NULL"); }
   else
     { FactPatternNodeReference(theEnv,theTemplate->patternNetwork,theFile,imageID,maxIndices); }
     
#if FUZZY_DEFTEMPLATES

   /*==========================================*/
   /* hasFuzzySlots field and fuzzyTemplate    */
   /*==========================================*/

   if (theTemplate->fuzzyTemplate == NULL)
     { fprintf(theFile,",%d,NULL", theTemplate->hasFuzzySlots); }
   else
     { fprintf(theFile, ",%d,(struct fuzzyLv *)&%s%d_%d[%d]",
                        theTemplate->hasFuzzySlots,
                        LvUniversePrefix(),imageID,
                        lvUniverseArrayVersion,lvUniverseArrayCount);
     }

#endif /* FUZZY_DEFTEMPLATES */


   /*============================================*/
   /* Print the factList and lastFact references */
   /* and close the structure.                   */
   /*============================================*/

   fprintf(theFile,",NULL,NULL}");
  }

/*****************************************************/
/* SlotToCode: Writes the C code representation of a */
/*   single deftemplate slot to the specified file.  */
/*****************************************************/
static void SlotToCode(
  Environment *theEnv,
  FILE *theFile,
  struct templateSlot *theSlot,
  unsigned int imageID,
  unsigned int maxIndices,
  unsigned int slotCount)
  {
   /*===========*/
   /* Slot Name */
   /*===========*/

   fprintf(theFile,"{");
   PrintSymbolReference(theEnv,theFile,theSlot->slotName);

   /*=============================*/
   /* Multislot and Default Flags */
   /*=============================*/

   fprintf(theFile,",%d,%d,%d,%d,",theSlot->multislot,theSlot->noDefault,
                                   theSlot->defaultPresent,theSlot->defaultDynamic);

   /*=============*/
   /* Constraints */
   /*=============*/

   PrintConstraintReference(theEnv,theFile,theSlot->constraints,imageID,maxIndices);

   /*===============*/
   /* Default Value */
   /*===============*/

   fprintf(theFile,",");
   PrintHashedExpressionReference(theEnv,theFile,theSlot->defaultList,imageID,maxIndices);

   /*============*/
   /* Facet List */
   /*============*/

   fprintf(theFile,",");
   PrintHashedExpressionReference(theEnv,theFile,theSlot->facetList,imageID,maxIndices);
   fprintf(theFile,",");

   /*===========*/
   /* Next Slot */
   /*===========*/

   if (theSlot->next == NULL)
     { fprintf(theFile,"NULL}"); }
   else
     {
      fprintf(theFile,"&%s%d_%d[%d]}",SlotPrefix(),imageID,
                               ((slotCount+1) / maxIndices) + 1,
                                (slotCount+1) % maxIndices);
     }
  }

/*****************************************************************/
/* DeftemplateCModuleReference: Writes the C code representation */
/*   of a reference to a deftemplate module data structure.      */
/*****************************************************************/
void DeftemplateCModuleReference(
  Environment *theEnv,
  FILE *theFile,
  unsigned long count,
  unsigned int imageID,
  unsigned int maxIndices)
  {
   fprintf(theFile,"MIHS &%s%u_%lu[%lu]",ModulePrefix(DeftemplateData(theEnv)->DeftemplateCodeItem),
                      imageID,
                      (count / maxIndices) + 1,
                      (count % maxIndices));
  }

/********************************************************************/
/* DeftemplateCConstructReference: Writes the C code representation */
/*   of a reference to a deftemplate data structure.                */
/********************************************************************/
void DeftemplateCConstructReference(
  Environment *theEnv,
  FILE *theFile,
  Deftemplate *theDeftemplate,
  unsigned int imageID,
  unsigned int maxIndices)
  {
   if (theDeftemplate == NULL)
     { fprintf(theFile,"NULL"); }
   else
     {
      fprintf(theFile,"&%s%u_%lu[%lu]",ConstructPrefix(DeftemplateData(theEnv)->DeftemplateCodeItem),
                      imageID,
                      (theDeftemplate->header.bsaveID / maxIndices) + 1,
                      theDeftemplate->header.bsaveID % maxIndices);
     }

  }

/*******************************************/
/* InitDeftemplateCode: Writes out runtime */
/*   initialization code for deftemplates. */
/*******************************************/
static void InitDeftemplateCode(
  Environment *theEnv,
  FILE *initFP,
  unsigned int imageID,
  unsigned int maxIndices)
  {
#if MAC_XCD
#pragma unused(theEnv)
#pragma unused(imageID)
#pragma unused(maxIndices)
#endif

   fprintf(initFP,"   DeftemplateRunTimeInitialize(theEnv);\n");
  }

#endif /* DEFTEMPLATE_CONSTRUCT && CONSTRUCT_COMPILER && (! RUN_TIME) */

