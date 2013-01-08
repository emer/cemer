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

#ifndef TypeDef_h
#define TypeDef_h 1

// parent includes:
#include <TypeItem>

// member includes:
#include <EnumSpace>
#include <TokenSpace>
#include <MemberSpace>
#include <Member_List>
#include <PropertySpace>
#include <MethodSpace>
#include <TypeSpace>

// declare all other types mentioned but not required to include:

class taBase_List; //

// Order of includes problem -- the Windows header file "WinUser.h" defines this
// as a macro.  If some file indirectly includes that file and then this file,
// it causes a compilation error on Windows.
// See also the comment in src/emergent/network/netdata.h
#ifdef SC_DEFAULT
  //#define SC_DEFAULT // uncomment to see previous macro definition
  #pragma message("Warning: undefining SC_DEFAULT macro")
  #undef SC_DEFAULT
#endif


class TA_API TypeDef : public TypeItem {
  // defines a type -- everything from int to classes, enums, typedefs, etc
INHERITED(TypeItem)
public:
  enum StrContext { // context for getting or setting a string value
    SC_DEFAULT,         // default (for compat) -- if taMisc::is_loading/saving true, then STREAMING else VALUE
    SC_STREAMING,       // value is being used for streaming, ex. strings are quoted/escaped
    SC_VALUE,           // value is being manipulated programmatically, ex. strings are not quoted/escaped
    SC_DISPLAY,         // value is being used for display purposes, and result includes html (rich text) formatting tags, etc
  };

  static TypeDef*       GetCommonSubtype(TypeDef* typ1, TypeDef* typ2); // get the common primary (1st parent class) subtype between the two

#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE) && !defined(__MAKETA__)
  void*         dmem_type; // actually ptr to: MPI_Datatype_PArray
  void          AssertDMem_Type(); // creates the dmem_type array if not already
  virtual MPI_Datatype  GetDMemType(int share_set);
#endif
  TypeSpace*    owner;          // the owner of this one

  uint          size;           // size (in bytes) of item
  short         ptr;            // number of pointers
  bool          ref;            // true if a reference variable
  bool          internal;       // true if an internal type (auto generated)
  bool          formal;         // true if a formal type (e.g. class, const, enum..)
#ifdef NO_TA_BASE
  bool          pre_parsed;     // true if previously parsed by maketa
#else
  bool          is_subclass;    // true if is a class, and inherits from another
  TypeDef*      plugin;         // TypeDef of plugin object, if in a plugin (else NULL)
  void**        instance;       // pointer to the instance ptr of this type
  taBase_List*  defaults;       // default values registered for this type
  UserDataItem_List* schema;    // default schema (only created if used)
#endif
  String_PArray inh_opts;       // inherited options (##xxx)

  TypeSpace     parents;        // type(s) this inherits from
  int_PArray    par_off;        // parent offsets
  TypeSpace     par_formal;     // formal parents (e.g. class, const, enum..)
  TypeSpace     par_cache;      // cache of *all* parents for optimized inheritance checking (not avail during maketa)
  TypeSpace     children;       // inherited from this
  TokenSpace    tokens;         // tokens of this type (if kept)

#ifdef TA_GUI
  taiType*      it;             // single widget gui representation of type
  taiEdit*      ie;             // gui editing window rep. of type
  taiViewType*  iv;             // gui browser representation of type
#endif

  // the following only apply to enums or classes
  EnumSpace     enum_vals;      // if type is an enum, these are the labels
  TypeSpace     sub_types;      // sub types scoped within class (incl enums)
  MemberSpace   members;        // member variables for class
  PropertySpace properties;     // properties for class
  MethodSpace   methods;        // member functions (methods) for class
  String_PArray ignore_meths;   // methods to be ignored
  TypeSpace     templ_pars;     // template parameters
  String        c_name;         // C name, when diff from name (ex 'unsigned_char' vs 'unsigned char")
  
  String	source_file;	// source file name where defined -- no path information, just file name
  int		source_start;	// starting source code line number
  int		source_end;	// ending source code line number

  bool          IsEnum() const; // true if an enum
  bool          IsClass() const; // true if it is a class
  bool          IsAnchor() const; // true if this is the non-ptr, non-ref, non-const type
  bool          IsVarCompat() const; // true if read/write compatible with Variant
  taMisc::TypeInfoKind typeInfoKind() const {return taMisc::TIK_TYPE;}

  /////////////////////////////////////////////////////////////
  //		Constructors and misc industrial

  override void*        This() {return this;}
  override TypeDef*     GetTypeDef() const {return &TA_TypeDef;}
  void          Copy(const TypeDef& cp);
  TypeDef();
  TypeDef(const char* nm);
#ifdef NO_TA_BASE
  TypeDef(const char* nm, const char* dsc, const char* inop, const char* op,
	  const char* lis,
          uint siz, int ptrs=0, bool refnc=false,
          bool global_obj=false); // global_obj=true for global (non new'ed) typedef objs
#else
  TypeDef(const char* nm, const char* dsc, const char* inop, const char* op,
	  const char* lis, const char* src_file, int src_st, int src_ed,
          uint siz, void** inst, bool toks=false, int ptrs=0, bool refnc=false,
          bool global_obj=false); // global_obj=true for global (non new'ed) typedef objs
#endif
  TypeDef(const char* nm, bool intrnl, int ptrs=0, bool refnc=false, bool forml=false,
          bool global_obj=false, uint siz = 0, const char* c_nm = NULL
          ); // global_obj=ture for global (non new'ed) typedef objs; c_name only needed when diff from nm
  TypeDef(const TypeDef& td);
  ~TypeDef();
  TypeDef*              Clone()         { return new TypeDef(*this); }
  TypeDef*              MakeToken()     { return new TypeDef(); }

  void                  CleanupCats(bool save_last);
  // cleanup the #CAT_ category options (allow for derived types to set new categories); if save_last it saves the last cat on the list (for typedef initialization); otherwise saves first (for addparent)

  void                  DuplicateMDFrom(const TypeDef* old);
  // duplicates members, methods from given type
  void                  UpdateMDTypes(const TypeSpace& ol, const TypeSpace& nw);
  // updates pointers within members, methods to new types from old

  bool                  CheckList(const String_PArray& lst) const;
  // check if have a list in common

  /////////////////////////////////////////////////////////////
  //		Parents, Inheritance

  override TypeDef*     GetOwnerType() const
  { if (owner) return owner->owner; else return NULL; }
  TypeDef*              GetParent() const { return parents.SafeEl(0); }
  // gets (first) parent of this type (assumes no multiple inheritance)

  TypeDef*              GetNonPtrType() const;
  // gets base type (ptr=0) parent of this type
  TypeDef*              GetPtrType() const;
  // gets child type that is a ptr to this type -- makes one if necessary
  TypeDef*              GetNonRefType() const;
  // gets base type (not ref) parent of this type
  TypeDef*              GetNonConstType() const;
  // gets base type (not const) parent of this type
  TypeDef*              GetNonConstNonRefType() const;
  // gets base type (not const or ref) parent of this type
  TypeDef*              GetClassType() const;
  // gets a parent type that is a basic class object (i.e., InheritsFormal from TA_class) -- this is the best way to cut through all the type qualifiers and just find out what kind of object you've got (preferred over GetNonConstNonRefType for class objects) -- returns NULL if this does NOT so inherit
  TypeDef*              GetTemplType() const;
  // gets base template parent of this type
  TypeDef*              GetTemplInstType() const;
  // gets base template instantiation parent of this type
  TypeDef*              GetPluginType() const;
    // if in_plugin, this is the IPlugin-derivitive plugin type
  String                GetPtrString() const;
  // gets a string of pointer symbols (*) corresponding to the number ptrs
  String                Get_C_Name() const;
  // get the C-code name for this type
  override const String GetPathName() const;

  bool                  HasEnumDefs() const; // true if any subtypes are enums
  bool                  HasSubTypes() const; // true if any non-enum subtypes

  bool                  FindParentName(const char* nm) const {
    if(par_cache.size) return (par_cache.FindNameIdx(nm) >= 0); // if cache active, use it exclusively
    if(parents.FindNameIdx(nm) >= 0) return true;
    for(int i=0; i < parents.size; i++)
      if(parents.FastEl(i)->FindParentName(nm)) return true;
    return false;
  }
  bool                  FindParent(const TypeDef* it) const {
    if(par_cache.size) return (par_cache.FindEl(it) >= 0); // if cache active use it exclusively
    if(parents.FindEl(it) >= 0) return true;
    for(int i=0; i < parents.size; i++)
      if(parents.FastEl(i)->FindParent(it))     return true;
    return false;
  }

  // you inherit from yourself.  This ensures that you are a "base" class (ptr == 0)
  bool                  InheritsFromName(const char *nm) const
  { if((ptr == 0) && ((name == nm) || FindParentName(nm))) return true; return false; }
  bool                  InheritsFrom(const TypeDef* td) const
  { if((ptr == 0) && ((this == td) || FindParent(td))) return true; return false; }
  bool                  InheritsFrom(const TypeDef& it) const
  { return InheritsFrom((TypeDef*)&it); }

  // pointers to a type, etc, can be Derives from a given type (looser than inherits)
  bool                  DerivesFromName(const char *nm) const
  { if((name == nm) || FindParentName(nm)) return true; return false; }
  bool                  DerivesFrom(TypeDef* td) const
  { if((this == td) || FindParent(td)) return true; return false; }
  bool                  DerivesFrom(const TypeDef& it) const
  { return DerivesFrom((TypeDef*)&it); }

  // inheritance from a formal class (e.g. const, static, class) -- NOTE: these are NOT inherited from parent types, so only an actual class object InheritsFormal(&TA_class)
  bool                  InheritsFormal(TypeDef* it) const
  { if((ptr == 0) && (par_formal.FindEl(it)>=0)) return true; return false; }
  bool                  InheritsFormal(const TypeDef& it) const
  { return InheritsFormal((TypeDef*)&it); }
  bool                  DerivesFormal(TypeDef* it) const
  { if(par_formal.FindEl(it)>=0) return true; return false; }
  bool                  DerivesFormal(const TypeDef& it) const
  { return DerivesFormal((TypeDef*)&it); }

  bool                  InheritsNonAtomicClass() const;
  // true *only* for classes that are not considered atoms by the streaming system, i.e. does not include taString and Variant
  bool                  IsBasePointerType() const;
  // true for taBase* and smartref and smartptr types -- any kind of effective pointer class

  TypeDef*              GetStemBase() const;
  // for class types: get first (from me) parent with STEM_BASE directive -- defines equivalence class -- returns NULL if not found

  TypeDef*      AddParent(TypeDef* it, int p_off=0);
  // adds parent and inherits all the stuff from it

  // these are for construction
  void          AddParents(TypeDef* p1=NULL, TypeDef* p2=NULL,
                           TypeDef* p3=NULL, TypeDef* p4=NULL,
                           TypeDef* p5=NULL, TypeDef* p6=NULL);
  void          AddClassPar(TypeDef* p1=NULL, int p1_off=0, TypeDef* p2=NULL, int p2_off=0,
                            TypeDef* p3=NULL, int p3_off=0, TypeDef* p4=NULL, int p4_off=0,
                            TypeDef* p5=NULL, int p5_off=0, TypeDef* p6=NULL, int p6_off=0);
  void          AddParFormal(TypeDef* p1=NULL, TypeDef* p2=NULL,
                             TypeDef* p3=NULL, TypeDef* p4=NULL,
                             TypeDef* p5=NULL, TypeDef* p6=NULL);
  void          AddTemplPars(TypeDef* p1=NULL, TypeDef* p2=NULL,
                             TypeDef* p3=NULL, TypeDef* p4=NULL,
                             TypeDef* p5=NULL, TypeDef* p6=NULL);

  void          CacheParents();
  // populate par_cache with *all* the parents and set hash table -- call this after all types are loaded etc -- in InitializeTypes
  void          CacheParents_impl(TypeDef* src_typ);
  // recursive impl for loading parents into src_typ

  void          ComputeMembBaseOff();
  // only for MI types, after adding parents, get new members & compute base_off
  bool          IgnoreMeth(const String& nm) const;
  // check if given method should be ignored (also checks parents, etc)

  void*         GetParAddr(const char* par, void* base) const;
  void*         GetParAddr(TypeDef* par, void* base) const;
  // return the given parent's address given the base address (par must be a parent!)
  int           GetParOff(TypeDef* par, int boff=-1) const;
  // return the given parent's offset (par must be a parent!)
  bool          ReplaceParent(TypeDef* old_tp, TypeDef* new_tp);
  // replace parent of old_tp with parent of new_tp (recursive)
  bool          FindChildName(const char* nm) const;
  bool          FindChild(TypeDef* it) const;
  // recursively tries to  find child, returns true if successful

  TypeDef*      GetTemplParent() const;
  // returns immediate parent which is a template (or NULL if none found)
  String        GetTemplName(const TypeSpace& inst_pars) const;
  void          SetTemplType(TypeDef* templ_par, const TypeSpace& inst_pars);
  // set type of a template class

  /////////////////////////////////////////////////////////////
  //		Finding stuff within type

  static MemberDef* FindMemberPathStatic(TypeDef*& own_td, int& net_base_off,
                                         ta_memb_ptr& net_mbr_off,
                                         const String& path, bool warn = true);
  // you must supply the initial own_td as starting type -- looks for a member or sequence of members based on static type information for members (i.e., does not walk the structural tree and cannot go into lists or other containers, but can find any static paths for object members and their members, etc) -- if warn, emits warning message for bad paths -- net offsets provide overall offset from original own_td obj
  TypeDef*      FindTypeWithMember(const char* nm, MemberDef** md);
  // returns the type or child type with memberdef md

  EnumDef*      FindEnum(const String& enum_nm) const;
  // find an enum and return its definition (or NULL if not found).  searches in enum_vals, then subtypes
  int           GetEnumVal(const String& enum_nm, String& enum_tp_nm) const;
  // find an enum and return its enum_no value, and set enum_tp_nm at the type name of the enum.  if not found, returns -1 and enum_tp_nm is empty
  String        GetEnumString(const String& enum_tp_nm, int enum_val) const;
  // get the name of enum with given value in enum list of given type (e.g., enum defined within class)
  const String  Get_C_EnumString(int enum_val, bool show_scope=true) const;
  // get a C-code string representing the given value in enum list of given type (e.g., enum defined within class); for BIT types, will compose the bits and cast; worst case will cast int to type
  String        GetEnumPrefix() const;
  // if all enum names start with the same prefix before an _ (underbar), then this returns that prefix, else an empty string -- useful for removing that prefix etc

#ifndef NO_TA_BASE
  void*         GetInstance() const { return instance ? *instance : 0; }
  int           FindTokenR(void* addr, TypeDef*& ptr) const;
  int           FindTokenR(const char* nm, TypeDef*& ptr) const;
  // recursive search for token among children
  void          AddUserDataSchema(UserDataItemBase* item); // adds the item as schema
#endif
  // for token management
  void          RegisterFinal(void* it); // call to just register the most derived (can only call once per it)
  void          unRegisterFinal(void* it); // call to just unregister the most derived (can only call once per it)

  /////////////////////////////////////////////////////////////
  //            Get/Set From String

  String        GetValStr(const void* base, void* par=NULL,
                          MemberDef* memb_def = NULL, StrContext vc = SC_DEFAULT,
                          bool force_inline = false) const;
  // get a string representation of value -- this is very widely used in the code
  void          SetValStr(const String& val, void* base, void* par = NULL,
                          MemberDef* memb_def = NULL, StrContext vc = SC_DEFAULT,
                          bool force_inline = false);
  // set the value from a string representation -- this is very widely used in the code

  ////////////  Helpers for specific cases

  String        GetValStr_enum(const void* base, void* par=NULL,
                               MemberDef* memb_def = NULL, StrContext vc = SC_DEFAULT,
                               bool force_inline = false) const;
  // get a string representation of enum
  String        GetValStr_class_inline(const void* base, void* par=NULL,
                                       MemberDef* memb_def = NULL, StrContext vc = SC_DEFAULT,
                                       bool force_inline = false) const;
  // get a string representation of inline class

  void          SetValStr_enum(const String& val, void* base, void* par = NULL,
                               MemberDef* memb_def = NULL, StrContext vc = SC_DEFAULT,
                               bool force_inline = false);
  // set the enum value from a string representation
  void          SetValStr_class_inline(const String& val, void* base, void* par = NULL,
                                       MemberDef* memb_def = NULL, StrContext vc = SC_DEFAULT,
                                       bool force_inline = false);
  // set the inline class value from a string representation

  int           ReplaceValStr(const String& srch, const String& repl, const String& mbr_filt,
                              void* base, void* par=NULL, TypeDef* par_typ=NULL,
                              MemberDef* memb_def = NULL, StrContext vc = SC_DEFAULT);
  // replace string value -- does a GetValStr, replace srch with repl in that string, then does a SetValStr -- always iterates over members of classes instead of doing inline to prevent replacing member names -- returns number replaced (0 = none) -- mbr_filt = filter for members to replace in -- if non-empty, member name for terminal value members where replace actually occurs (as opposed to owner class objects) must contain this string

  int           ReplaceValStr_class(const String& srch, const String& repl, const String& mbr_filt,
                                    void* base, void* par=NULL, TypeDef* par_typ=NULL,
                                    MemberDef* memb_def = NULL, StrContext vc = SC_DEFAULT);
  // for a class type: replace string value for each member accordidng to streaming context

  /////////////////////////////////////////////////////////////
  //            Get/Set From Variant

  const Variant GetValVar(const void* base, const MemberDef* memb_def = NULL) const;
  // get a Variant representation of value; primarily for value types (int, etc.); NOTE: returns taBase* types as the Base value (not a pointer to the pointer), which is usually what you want (see source for more detail)
  void          SetValVar(const Variant& val, void* base, void* par = NULL,
                          MemberDef* memb_def = NULL);
  // sets value from a Variant representation; primarily for value types (int, etc.);
  bool          ValIsDefault(const void* base, const MemberDef* memb_def,
                             int for_show = taMisc::IS_EXPERT) const;
  // true if the type contains its defaults
  bool          ValIsEmpty(const void* base_, const MemberDef* memb_def) const;
  // true only if value is empty, ex 0 or ""

  /////////////////////////////////////////////////////////////
  //		Copying, Comparing

  void          CopyFromSameType(void* trg_base, void* src_base,
                                         MemberDef* memb_def = NULL);
  // copy all mmbers from same type
  void          CopyOnlySameType(void* trg_base, void* src_base,
                                         MemberDef* memb_def = NULL);
  // copy only those members from same type (no inherited)
  void          MemberCopyFrom(int memb_no, void* trg_base, void* src_base);
  // copy a particular member from same type
  bool          CompareSameType(Member_List& mds, TypeSpace& base_types,
                                voidptr_PArray& trg_bases, voidptr_PArray& src_bases,
                                void* trg_base, void* src_base,
                                int show_forbidden = taMisc::NO_HIDDEN,
                                int show_allowed = taMisc::SHOW_CHECK_MASK,
                                bool no_ptrs = true, bool test_only = false);
  // compare all member values from class of the same type as me, adding ones that are different to the mds, trg_bases, src_bases lists (unless test_only == true, in which case it just does the tests and returns true if any diffs -- for inline objects)

#ifndef NO_TA_BASE
  void          GetObjDiffVal(taObjDiff_List& odl, int nest_lev, const void* base,
                        MemberDef* memb_def=NULL, const void* par=NULL, TypeDef* par_typ=NULL,
                        taObjDiffRec* par_od=NULL) const;
  // add this object and all its members (if a class) to the object diff list
  void          GetObjDiffVal_class(taObjDiff_List& odl, int nest_lev, const void* base,
                    MemberDef* memb_def=NULL, const void* par=NULL, TypeDef* par_typ=NULL,
                    taObjDiffRec* par_od=NULL) const;
  // just add members of a class object to the diff list
#endif


  /////////////////////////////////////////////////////////////
  // 		Value printing

  String&       Print(String& strm, void* base, int indent=0) const;
  // output value information for display purposes

  String&       PrintType(String& strm, int indent = 0) const;
  // output type information only
  String&       PrintInherit(String& strm) const;
  String&       PrintInherit_impl(String& strm) const;
  String&       PrintTokens(String& strm, int indent=0) const;

  /////////////////////////////////////////////////////////////
  //		HTML docs

  String        GetHTML(bool gendoc=false) const;
  // gets an HTML representation of this type -- for help view etc -- gendoc = external html file rendering instead of internal help browser
  String        GetHTMLLink(bool gendoc=false) const;
  // get HTML code for a link to this type -- only generates a link if InheritsNonAtomicClass -- otherwise it just returns the Get_C_Name representation
  String        GetHTMLSubType(bool gendoc=false, bool short_fmt=false) const;
  // gets an HTML representation of a sub type (typdef or enum) -- for help view etc -- gendoc = external html file rendering instead of internal help browser, short_fmt = no details, for summary guys
  String        GetHTMLMembMeth(String_PArray& memb_idx, String_PArray& meth_idx,
                const String& label_prefix, const String& link_prefix, bool gendoc=false) const;
  // render the members and methods for given lists of items -- can pre-filter the lists and render them separately (e.g., to separate regular from EXPERT items)

  /////////////////////////////////////////////////////////////
  // 		Dump: Saving and loading of type instances to/from streams

  int           Dump_Save(ostream& strm, void* base, void* par=NULL, int indent=0);
  // called by the user to save an object
  int           Dump_Save_impl(ostream& strm, void* base, void* par=NULL, int indent=0);
  int           Dump_Save_inline(ostream& strm, void* base, void* par=NULL, int indent=0);
  // for saving objects as members of other objects
  int           Dump_Save_Path(ostream& strm, void* base, void* par=NULL, int indent=0);
  // save the path of the object
  int           Dump_Save_Value(ostream& strm, void* base, void* par=NULL, int indent=0);
  // save the value of this object (i.e. the members)
  int           Dump_SaveR(ostream& strm, void* base, void* par=NULL, int indent=0);
  // if there are sub-elements (i.e. groups), save them (return false if not)
  int           Dump_Save_PathR(ostream& strm, void* base, void* par=NULL, int indent=0);
  // if there are sub-elements, save the path to them (return false if not)

  int           Dump_Load(istream& strm, void* base, void* par=NULL, void** el = NULL);
  // called by the user to load an object
  int           Dump_Load_impl(istream& strm, void* base, void* par=NULL,
                               const char* typnm=NULL);
  int           Dump_Load_Path(istream& strm, void*& base, void* par, TypeDef*& td,
                               String& path, const char* typnm=NULL);
  // loads a path (typename path) and fills in the base and td of object (false if err)
  int           Dump_Load_Path_impl(istream& strm, void*& base, void* par, String path);
  int           Dump_Load_Value(istream& strm, void* base, void* par=NULL);
  // loads the actual member values of the object (false if error)

  /////////////////////////////////////////////////////////////
  //		Generate source code stubs in standard format -- timesaver

  bool		CreateNewSrcFiles(const String& top_path, const String& src_dir);
  // #CAT_File create files for this class type in given location (top_path/src_dir and top_path/include) -- see taMisc::CreateNewSrcFiles for details -- also automatically adds proper set of includes and declarations based on inheritance and members
  String	Includes();
  // #CAT_File generate list of includes for this type = parents and full class members

protected:
  mutable signed char    m_cacheInheritsNonAtomicClass;
private:
  void          Initialize();
  void          Copy_(const TypeDef& cp);
};

#endif // TypeDef_h
