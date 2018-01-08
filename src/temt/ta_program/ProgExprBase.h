// Copyright 2013-2018, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef ProgExprBase_h
#define ProgExprBase_h 1

// parent includes:
#include <taOBase>

// member includes:
#include <taBase_List>
#include <ProgVarRef_List>
#include <String_Array>
#include <taList_impl>
#include <taGroup_impl>
#include <EnumSpace>
#include <MemberSpace>
#include <MethodSpace>

// declare all other types mentioned but not required to include:
class cssElPtr; //
class MemberDef; //
class TypeDef; //
class Program; //
class Function; //
class ProgEl; //
class ProgEl_List; //
class cssProgSpace; // #IGNORE
class cssSpace; // #IGNORE
class Completions;


taTypeDef_Of(ProgExprBase);

class TA_API ProgExprBase : public taOBase {
  // ##NO_TOKENS ##INSTANCE ##EDIT_INLINE ##CAT_Program an expression in a program -- manages variable references so they are always updated when program variables change -- base doesn't have any lookup functionality
INHERITED(taOBase)
public:
  enum ExprFlags { // #BITS flags for program expression setting
    PE_NONE             = 0, // #NO_BIT
    NO_VAR_ERRS         = 0x0001, // do not generate error messages for variables that cannot be found (e.g., for more complex expressions that might create local variables)
    FULL_STMT           = 0x0002, // expression is full css statement(s), not just isolated expressions -- this affects how the parsing works
    FOR_LOOP_EXPR       = 0x0004, // expression is an initializer or increment for a for loop -- requires different parsing due to possibility of commas..
    MATRIX_COL_EXPR     = 0x0008, // matrix column expression, could have : etc
    NO_PARSE            = 0x0010, // do not parse expression in UAE
    IN_PARSE            = 0x0020, // currently doing a parse -- prevent loops!
  };
  
  enum LookUpType {
    NOT_SET,
    ENUM,           // just enums - basically all you can have for case:
    VARIOUS,        // could be one of several types - e.g. at start of line - could be a data type, program variable, program element, etc.
    OBJ_MEMB_METH,  // object. or object-> list will be members and methods
    SCOPED,         // class::
    CALL,           // call of program or call of function
    PROGRAM_FUNCTION, //  call of a function in another program
    METHOD,         // method. list will be members
    ASSIGN,         // could be ProgVar, method, function but not program
    EQUALITY,       // either == or !=
    STRING_INDEX,   // access of list/group item by name i.e. "string" or access of datatable column by name
  };
  
  enum ExpressionStart {
    LINE_START,                 // start of line
    LINE_MID,
    LEFT_PARENS,                // following a left parens
  };
  
  enum GlobalLocal {
    PROGVAR_LOCAL,
    PROGVAR_GLOBAL,
  };
  
  String        expr;           // #ADD_COMPLETER_EXPR #EDIT_DIALOG #EDIT_WIDTH_40 #LABEL_ enter the expression here -- use Ctrl-L to pull up a lookup dialog for members, methods, types, etc -- or you can just type in names of program variables or literal values.  enclose strings in double quotes.  variable names will be checked and automatically updated

  ExprFlags     flags;          // #HIDDEN #NO_SAVE Flags for controlling expression behavior -- should not be saved because they are set by the owning program every time
  String        var_expr;       // #READ_ONLY #HIDDEN #NO_SAVE expression with variables listed as $#1#$, etc. used for generating the actual code (this is the 'official' version that generates the full expr)

  ProgVarRef_List vars;         // #READ_ONLY #HIDDEN #NO_SAVE list of program variables that appear in the expression
  String_Array  var_names;      // #READ_ONLY #HIDDEN #NO_SAVE original variable names associated with vars list -- useful for user info if a variable goes out of existence..
  String_Array  bad_vars;       // #READ_ONLY #HIDDEN #NO_SAVE list of variable names that are not found in the expression (may be fine if declared locally elsewhere, or somewhere hidden -- just potentially bad)

  static cssProgSpace*  parse_prog; // #IGNORE program space for parsing
  static cssSpace*      parse_tmp;  // #IGNORE temporary el's created during parsing (for types)
  int                   parse_ve_off; // #IGNORE offset to position information (for expressions = 10, otherwise 0)
  int                   parse_ve_pos; // #IGNORE position within expr during parsing for copying to var_expr
  
  // these are for the code completer
  static Completions            completions;           // #READ_ONLY #HIDDEN #NO_SAVE
  
  static taBase_List            completion_progvar_global_list;  // #READ_ONLY #HIDDEN #NO_SAVE
  static taBase_List            completion_progvar_local_list;  // #READ_ONLY #HIDDEN #NO_SAVE
  static taBase_List            completion_dynenum_list;  // #READ_ONLY #HIDDEN #NO_SAVE
  static taBase_List            completion_function_list; // #READ_ONLY #HIDDEN #NO_SAVE
  static taBase_List            completion_program_list;  // #READ_ONLY #HIDDEN #NO_SAVE
  static taBase_List            completion_list_items_list;  // #READ_ONLY #HIDDEN #NO_SAVE
  static taBase_List            completion_group_items_list;  // #READ_ONLY #HIDDEN #NO_SAVE
  static MemberSpace            completion_member_list; // #READ_ONLY #HIDDEN #NO_SAVE
  static MethodSpace            completion_method_list; // #READ_ONLY #HIDDEN #NO_SAVE
  static EnumSpace              completion_enum_list;   // #READ_ONLY #HIDDEN #NO_SAVE
  static TypeSpace              completion_statics_list;// #READ_ONLY #HIDDEN #NO_SAVE built once - found in taMisc
  static TypeSpace              completion_misc_list;   // #READ_ONLY #HIDDEN #NO_SAVE built once - found in taMisc
  static String_Array           completion_progels_list;// #READ_ONLY #HIDDEN #NO_SAVE built once
  static String_Array           completion_bool_list;   // #READ_ONLY #HIDDEN #NO_SAVE built once
  static String_Array           completion_null_list;   // #READ_ONLY #HIDDEN #NO_SAVE built once
  static String_Array           completion_type_list;   // #READ_ONLY #HIDDEN #NO_SAVE built once
  static LookUpType             completion_lookup_type; // #READ_ONLY #HIDDEN #NO_SAVE
  static String                 completion_prog_el_text;// #READ_ONLY #HIDDEN #NO_SAVE completers copy because it needs to be static so we can get it later
  static String                 completion_text_before; // #READ_ONLY #HIDDEN #NO_SAVE completers copy - this is text before cursor
  static String                 completion_lookup_seed; // #READ_ONLY #HIDDEN #NO_SAVE static so we can get it later
  static bool                   include_statics;        // #READ_ONLY #HIDDEN #NO_SAVE should lookup/completion include the static classes
  static bool                   include_progels;        // #READ_ONLY #HIDDEN #NO_SAVE should lookup/completion include the ProgEls
  static bool                   include_types;          // #READ_ONLY #HIDDEN #NO_SAVE should lookup/completion include the ProgEls
  static bool                   include_bools;          // #READ_ONLY #HIDDEN #NO_SAVE should lookup/completion include the bools
  static bool                   include_null;           // #READ_ONLY #HIDDEN #NO_SAVE should lookup/completion include null
  static bool                   include_css_functions;  // #READ_ONLY #HIDDEN #NO_SAVE should lookup/completion include css_functions
  static bool                   include_misc;           // #READ_ONLY #HIDDEN #NO_SAVE should lookup/completion include scope classes
  static TypeDef*               current_typedef;        // #READ_ONLY #HIDDEN #NO_SAVE save for use after completer selection
  static Program*               current_program;        // #READ_ONLY #HIDDEN #NO_SAVE save for use after completer selection
  

  bool          empty() const {return expr.empty();}
    // #IGNORE quicky test for whether has anything or not, without needing to render
  bool          nonempty() const {return expr.nonempty();}
    // #IGNORE quicky test for whether has anything or not, without needing to render
  

  virtual bool  SetExpr(const String& ex);
  // set to use given expression -- calls ParseExpr followed by UpdateAfterEdit_impl

  static int    cssExtParseFun_pre(void* udata, const String& nm, cssElPtr& el_ptr);
  // #IGNORE external parsing function for css: pre for initial parsing
  static int    cssExtParseFun_post(void* udata, const String& nm, cssElPtr& el_ptr);
  // #IGNORE external parsing function for css: post if nothing else gets it (bad var)

  virtual bool  ParseExpr();
  // parse the current expr for variables and update vars and var_expr accordingly (returns false if there are some bad_vars)
  virtual void  ReParseExpr(bool prompt_for_bad_vars = true);
  // calls ParseExpr if appropriate, prompts for bad vars if true
  virtual String GetFullExpr() const;
  // get full expression with variable names substituted appropriately
  virtual int   ReplaceVar(ProgVar* old_var, ProgVar* new_var);
  // replace old var with new var in code -- uses parsed variables

  inline void           SetExprFlag(ExprFlags flg)   { flags = (ExprFlags)(flags | flg); }
  // set flag state on
  inline void           ClearExprFlag(ExprFlags flg) { flags = (ExprFlags)(flags & ~flg); }
  // clear flag state (set off)
  inline bool           HasExprFlag(ExprFlags flg) const { return (flags & flg); }
  // check if flag is set
  inline void           SetExprFlagState(ExprFlags flg, bool on)
  { if(on) SetExprFlag(flg); else ClearExprFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  int  UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par) override;
  int  UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr) override;

  virtual void UpdateProgElVars(const taBase* old_scope, taBase* new_scope);
  // #IGNORE update prog el vars after scope change (move, copy)
  
  String GetDisplayName() const override;
  String GetName() const override;
  String GetTypeDecoKey() const override { return "ProgExpr"; }

  static Completions* ExprLookupCompleter(const String& cur_txt, int cur_pos, int& new_pos,
                                taBase*& path_own_obj, TypeDef*& path_own_typ,
                                MemberDef*& path_md, ProgEl* own_pel,
                                Program* own_prg, Function* own_fun,
                                taBase* path_base=NULL, TypeDef* path_base_typ=NULL);
  // #IGNORE generic lookup function for any kind of expression -- very powerful!  takes current text and position where the lookup function was called, and returns a list of possible completions -- if this is a path expression then path_own_typ is the type of object that owns the member path_md at the end of the path -- if path_md is NULL then path_own_typ is an object in a list or other container where member def is not relevant.  path_base is a base anchor point for paths if that is implied instead of needing to be fully contained within the expression (path_base_typ is type of that guy, esp needed if base is null) -- in this case only path expressions are allowed.
  static void          ExprLookupCompleterReset();
  // #IGNORE reset all the data structures and lists for completer --
  Completions*         StringFieldLookupForCompleter(const String& cur_txt,int cur_pos, const String& mbr_name, int& new_pos) override;
  static LookUpType    ParseForLookup(const String& cur_txt, int cur_pos, String& prepend_txt,
                                      String& path_prepend_txt, String& append_txt, String& prog_el_txt,
                                      String& base_path, String& lookup_seed, String& path_var, String& path_rest,
                                      bool path_base_not_null, ExpressionStart& expr_start, bool& lookup_group_default);
  // return the lookup type and set many arguments
  static bool           ExpressionTakesArgs(String expression);
  // if method then does it take arguments - completer uses this to position cursor intelligently

  static bool           FindPathSeparator(const String& path, int& separator_start, int& separator_end, bool backwards = true);
  // locate either '.' or '->'
  
  // Signature must match that of the item_filter_fun typedef.
  static bool           ExprLookupVarFilter(void* base, void* var); // special filter used in ExprLookupCompleter
  static bool           ExprLookupNoArgFuncFilter(void* base, void* function_); // special filter used in ExprLookupCompleter
  static bool           ExprLookupIsFunc(const String& txt);  // is it a function or program lookup
  static bool           ExprIsType(const String& txt);  // bool, int, float, etc
  static void           GetTokensOfType(TypeDef* td, taBase_List* tokens, taBase* scope = NULL,
                                        TypeDef* scope_type = NULL, ProgVar::VarType = ProgVar::T_UnDef);
  static void           GetListItems(taList_impl* list, taBase_List* tokens);
  static void           GetGroupItems(taGroup_impl* groups, taBase_List* tokens, bool top_group = true);
  // top_group should be true on first call so the subgroups are listed but after that false so subsubgroups are not listed but subsub leaves are listed
  static void           GetLocalVars(taBase_List* tokens, ProgEl* prog_el, taBase* scope = NULL,
                                       TypeDef* scope_type = NULL, ProgVar::VarType = ProgVar::T_UnDef);
  static void           GetGlobalVars(taBase_List* tokens, taBase* scope = NULL,
                                       TypeDef* scope_type = NULL, ProgVar::VarType = ProgVar::T_UnDef);
  static void           GetMembersForType(TypeDef* td, MemberSpace* members, bool just_static = false);
  static void           GetMethodsForType(TypeDef* td, MethodSpace* methods, bool just_static = false);
  static void           GetEnumsForType(TypeDef* td, EnumSpace* enums);
  static void           GetProgEls(String_Array* progels);
  static void           GetBools(String_Array* bools);
  static void           GetNull(String_Array* nulls);
  static void           GetTypes(String_Array* types);
  static void           GenProgElList(ProgEl_List& list, TypeDef* td);
  static TypeDef*       GetSpecialCaseType(const String& text);

  
  static ProgExprBase::LookUpType           Test_ParseForLookup(const String test_name, const String input_text, const int cursor_pos,
                                            String& lookup_seed, String& prepend_txt, String& append_txt,
                                            String& prog_el_txt, String& path_var, String& path_prepend_txt,
                                            String& path_rest, String& base_path, bool& lookup_group_default);
  // ONLY for testing - returns the lookup type and sets many variables

  void  InitLinks() override;
  void  CutLinks() override;
  TA_BASEFUNS(ProgExprBase);
protected:
  virtual void  ParseExpr_SkipPath(int& pos);
  // skip over a path expression

  void UpdateAfterEdit_impl() override;
  void UpdateAfterMove_impl(taBase* old_owner) override;
  void CheckThisConfig_impl(bool quiet, bool& rval) override;
  void SmartRef_SigDestroying(taSmartRef* ref, taBase* obj) override;
  void SmartRef_SigEmit(taSmartRef* ref, taBase* obj,
                        int sls, void* op1_, void* op2_) override;
  
private:
  void  Copy_(const ProgExprBase& cp);
  void  Initialize();
  void  Destroy();
};


// short expression version -- for shorter expressions -- a gui /maketa difference

taTypeDef_Of(ProgExprShort);

class TA_API ProgExprShort : public ProgExprBase {
INHERITED(ProgExprBase)
public:
#ifdef __MAKETA__
  String        expr;           // #EDIT_DIALOG #EDIT_WIDTH_5 #LABEL_ enter the expression here -- use Ctrl-L to pull up a lookup dialog for members, methods, types, etc -- or you can just type in names of program variables or literal values.  enclose strings in double quotes.  variable names will be checked and automatically updated
#endif

  TA_BASEFUNS_NOCOPY(ProgExprShort);
private:
  void  Initialize() { };
  void  Destroy()    { };
};



#endif // ProgExprBase_h
