// Copyright, 1995-2013, Regents of the University of Colorado,
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
#include <ProgVarRef_List>
#include <String_Array>


// declare all other types mentioned but not required to include:
class cssElPtr; //
class MemberDef; //
class Program; //
class Function; //
class ProgEl; //
class cssProgSpace; // #IGNORE
class cssSpace; // #IGNORE


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
    NO_PARSE            = 0x0008, // do not parse expression in UAE
  };
  
  enum LookUpType {
    NOT_SET,
    VARIOUS,         // no path or delimiter - could be one of several types
    OBJ_MEMB_METH,
    SCOPED,           // Class::
    ARRAY_INDEX,
    CALL,
    PROGRAM_FUNCTION
  };

  String        expr;           // #EDIT_DIALOG #EDIT_WIDTH_40 #LABEL_ enter the expression here -- use Ctrl-L to pull up a lookup dialog for members, methods, types, etc -- or you can just type in names of program variables or literal values.  enclose strings in double quotes.  variable names will be checked and automatically updated

  ExprFlags     flags;          // #HIDDEN #NO_SAVE Flags for controlling expression behavior -- should not be saved because they are set by the owning program every time
  String        var_expr;       // #READ_ONLY #HIDDEN #NO_SAVE expression with variables listed as $#1#$, etc. used for generating the actual code (this is the 'official' version that generates the full expr)

  ProgVarRef_List vars;         // #READ_ONLY #HIDDEN #NO_SAVE list of program variables that appear in the expression
  String_Array  var_names;      // #READ_ONLY #HIDDEN #NO_SAVE original variable names associated with vars list -- useful for user info if a variable goes out of existence..
  String_Array  bad_vars;       // #READ_ONLY #HIDDEN #NO_SAVE list of variable names that are not found in the expression (may be fine if declared locally elsewhere, or somewhere hidden -- just potentially bad)

  static cssProgSpace*  parse_prog; // #IGNORE program space for parsing
  static cssSpace*      parse_tmp;  // #IGNORE temporary el's created during parsing (for types)
  int                   parse_ve_off; // #IGNORE offset to position information (for expressions = 10, otherwise 0)
  int                   parse_ve_pos; // #IGNORE position within expr during parsing for copying to var_expr

  bool          empty() const {return expr.empty();}
    // #IGNORE quicky test for whether has anything or not, without needing to render
  bool          nonempty() const {return expr.nonempty();}
    // #IGNORE quicky test for whether has anything or not, without needing to render

  virtual bool  SetExpr(const String& ex);
  // set to use given expression -- calls ParseExpr followed by UpdateAfterEdit_impl

  static int    cssExtParseFun_pre(void* udata, const char* nm, cssElPtr& el_ptr);
  // #IGNORE external parsing function for css: pre for initial parsing
  static int    cssExtParseFun_post(void* udata, const char* nm, cssElPtr& el_ptr);
  // #IGNORE external parsing function for css: post if nothing else gets it (bad var)

  virtual bool  ParseExpr();
  // parse the current expr for variables and update vars and var_expr accordingly (returns false if there are some bad_vars)
  virtual String GetFullExpr() const;
  // get full expression with variable names substituted appropriately

  inline void           SetExprFlag(ExprFlags flg)   { flags = (ExprFlags)(flags | flg); }
  // set flag state on
  inline void           ClearExprFlag(ExprFlags flg) { flags = (ExprFlags)(flags & ~flg); }
  // clear flag state (set off)
  inline bool           HasExprFlag(ExprFlags flg) const { return (flags & flg); }
  // check if flag is set
  inline void           SetExprFlagState(ExprFlags flg, bool on)
  { if(on) SetExprFlag(flg); else ClearExprFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  virtual void  UpdateProgExpr_NewOwner();
  // update program expression after it has been moved/copied to a new owner -- this will identify any variables that are not present in the new program and copy them from the old owner -- must be called before messing with any of the vars progvarref's pointers (should be a copy/same as prior ones)

  int  UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par) override;
  int  UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr) override;

  String GetDisplayName() const override;
  String GetName() const override;
  String GetTypeDecoKey() const override { return "ProgExpr"; }

  static String   ExprLookupFun(const String& cur_txt, int cur_pos, int& new_pos,
                                taBase*& path_own_obj, TypeDef*& path_own_typ,
                                MemberDef*& path_md, ProgEl* own_pel,
                                Program* own_prg, Function* own_fun,
                                taBase* path_base=NULL, TypeDef* path_base_typ=NULL);
  // generic lookup function for any kind of expression -- very powerful!  takes current text and position where the lookup function was called, and returns the new text filled in with whatever the user looked up, with a new cursor position (new_pos) -- if this is a path expression then path_own_typ is the type of object that owns the member path_md at the end of the path -- if path_md is NULL then path_own_typ is an object in a list or other container where member def is not relevant.  path_base is a base anchor point for paths if that is implied instead of needing to be fully contained within the expression (path_base_typ is type of that guy, esp needed if base is null) -- in this case only path expressions are allowed.

  String StringFieldLookupFun(const String& cur_txt, int cur_pos,
                              const String& mbr_name, int& new_pos) override;
  
  static LookUpType ParseForLookup(const String& cur_txt, int cur_pos, String& prepend_txt, String& path_prepend_txt, String& append_txt, String& prog_el_txt, String& base_path, String& lookup_seed, String& path_var, String& path_rest, bool path_base_not_null, int& expr_start);
  // return the lookup type
  
  // Signature must match that of the item_filter_fun typedef.
  static bool   ExprLookupVarFilter(void* base, void* var); // special filter used in ExprLookupFun
  static bool   ExprLookupIsFunc(const String& txt);  // is it a function or program lookup
  
  void  InitLinks();
  void  CutLinks();
  TA_BASEFUNS(ProgExprBase);
protected:

  virtual void  ParseExpr_SkipPath(int& pos);
  // skip over a path expression

  void UpdateAfterEdit_impl() override;
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
