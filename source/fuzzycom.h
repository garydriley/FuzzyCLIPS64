   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*                  A Product Of The                   */
   /*             Software Technology Branch              */
   /*             NASA - Johnson Space Center             */
   /*                                                     */
   /*             CLIPS Version 6.00  05/12/93            */
   /*                                                     */
   /*             FUZZY COMMANDS HEADER FILE              */
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



#ifndef _H_fuzzycom
#define _H_fuzzycom




#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _FUZZYCOM_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

    void                  DeffuzzyCommands(Environment *);
    void                  getu(Environment *,UDFContext *,UDFValue *);
    void                  getu_from(Environment *,UDFContext *,UDFValue *);
    void                  getu_to(Environment *,UDFContext *,UDFValue *);
    void                  getu_units(Environment *,UDFContext *,UDFValue *);
    void                  get_fs(Environment *,UDFContext *,UDFValue *);
    void                  get_fs_template(Environment *,UDFContext *,UDFValue *);
    void                  get_fs_lv(Environment *,UDFContext *,UDFValue *);
    void                  get_fs_length(Environment *,UDFContext *,UDFValue *);
    void                  get_fs_value(Environment *,UDFContext *,UDFValue *);
    void                  get_fs_x(Environment *,UDFContext *,UDFValue *);
    void                  get_fs_y(Environment *,UDFContext *,UDFValue *);
    void                  moment_defuzzify(Environment *,UDFContext *,UDFValue *);
    void                  maximum_defuzzify(Environment *,UDFContext *,UDFValue *);
    void                  add_fuzzy_modifier(Environment *,UDFContext *,UDFValue *);
    void                  remove_fuzzy_modifier(Environment *,UDFContext *,UDFValue *);
    void                  set_fuzzy_inference_type(Environment *,UDFContext *,UDFValue *);
    void                  get_fuzzy_inference_type(Environment *,UDFContext *,UDFValue *);
    void                  set_fuzzy_display_precision(Environment *,UDFContext *,UDFValue *);
    void                  get_fuzzy_display_precision(Environment *,UDFContext *,UDFValue *);
    void                  set_alpha_value(Environment *,UDFContext *,UDFValue *);
    void                  get_alpha_value(Environment *,UDFContext *,UDFValue *);
    void                  plot_fuzzy_value(Environment *,UDFContext *,UDFValue *);
    void                  get_fuzzy_slot(Environment *,UDFContext *,UDFValue *);
    void                  fuzzy_union(Environment *,UDFContext *,UDFValue *);
    void                  fuzzy_intersection(Environment *,UDFContext *,UDFValue *);
    void                  fuzzy_modify(Environment *,UDFContext *,UDFValue *);
    void                  create_fuzzy_value(Environment *,UDFContext *,UDFValue *);
    void                  is_defuzzify_value_valid(Environment *,UDFContext *,UDFValue *);

#endif

