   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.40  07/30/16            */
   /*                                                     */
   /*         FACT RETE PRINT FUNCTIONS HEADER FILE       */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
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
/*      6.40: Removed LOCALE definition.                     */
/*                                                           */
/*            Pragma once and other inclusion changes.       */
/*                                                           */
/*            Removed use of void pointers for specific      */
/*            data structures.                               */
/*                                                           */
/*************************************************************/

#ifndef _H_factprt

#pragma once

#define _H_factprt

   void                           PrintFactJNCompVars1(Environment *,const char *,void *);
   void                           PrintFactJNCompVars2(Environment *,const char *,void *);
   void                           PrintFactPNCompVars1(Environment *,const char *,void *);
   void                           PrintFactJNGetVar1(Environment *,const char *,void *);
   void                           PrintFactJNGetVar2(Environment *,const char *,void *);
   void                           PrintFactJNGetVar3(Environment *,const char *,void *);
   void                           PrintFactPNGetVar1(Environment *,const char *,void *);
   void                           PrintFactPNGetVar2(Environment *,const char *,void *);
   void                           PrintFactPNGetVar3(Environment *,const char *,void *);
   void                           PrintFactSlotLength(Environment *,const char *,void *);
   void                           PrintFactPNConstant1(Environment *,const char *,void *);
   void                           PrintFactPNConstant2(Environment *,const char *,void *);
#if FUZZY_DEFTEMPLATES
   void                           PrintPNFUZZY_VALUE(Environment *,const char *,void *);
#endif

#endif /* _H_factprt */


