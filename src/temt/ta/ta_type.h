// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

// Type Access: Automatic Access to C Types

#ifndef TA_TYPE_H
#define TA_TYPE_H

#include "ta_def.h"
#include "ta_list.h"
#include "ta_variant.h"
#if !defined(NO_TA_BASE) && !defined(__MAKETA__)
# if defined(TA_GUI)
#   include <QPointer> // guarded same pointers
# endif
# if defined(DMEM_COMPILE)
#   include "mpi.h"
# endif
#endif

#ifdef TA_USE_QT
# ifndef __MAKETA__
#   include <QObject>
# endif
#endif


// comment directives which are parsed 'internally' (add a # sign before each)
// "NO_TOKENS"
// "IGNORE"
// "REG_FUN"
// "INSTANCE"
// "NO_INSTANCE"
// "HIDDEN"
// "READ_ONLY"
// "NO_CSS"
// "NO_MEMBERS"

// forwards
class TA_API IDataLinkClient;
class TA_API taDataLink;
class TA_API taDataLinkItr;
class TA_API EnumSpace;
class TA_API MemberSpace;
class TA_API PropertySpace;
class TA_API MethodSpace;
class TA_API TypeSpace;
class TA_API EnumDef;
class TA_API MemberDefBase;
class TA_API MemberDef;
class TA_API PropertyDef;
class TA_API MethodDef;
class TA_API TypeDef; //

/*
  These are the basic pre-defined built in type definitions. Many of these are synonyms
  for each other (ex. int==signed==signed int== etc.)
  
  The indentation here indicates a DerivesFrom parentage hierarchy (for synonyms.)
  In any context where you want to treat all synonyms of a type the same way, you only
  need to check for "DerivesFrom(TA_xxx)" of the most primary type below.
  ex. if you check for TA_int, you'll get all the synonyms of it.
  
  
  'long' is the same size as 'int' on all our 32 and 64-bit platforms. We consider 'long'
  types deprecated.
  
  intptr_t is an int type whose size is the same as a pointer on your platform. It is not
  actually a basic type, but will be a synonym for either int or int64_t.
  
  'long long' is 64-bits, but we prefer to use the ansi designation 'int64_t' -- we
  provide a guarded typedef for this (for Windows) in the ta_global.h header file.
  
  Note that our code is not well tested against any use of unsigned types, and apart from
  'byte' we suggest not using them, particularly in gui contexts.
  
  Note that the C standard specifies that 'char' 'unsigned char' and 'signed char' are
  distinct types. Most C's (including the ones we support) treat char as signed.
  In tacss/pdp, we use 'char' for its normal purpose (ansi character), and char* for 
  C-style strings.
  We use 'byte' as a synonym for 'unsigned char' and treat it as an 8 bit unsigned int. 
  This is principally used for color values and in network data patterns. We don't use
  'signed char' but if you use it in your code, it will be treated in the gui as a
  8-bit signed numeric type, not an ansi character.
*/
#ifndef __MAKETA__
extern TA_API TypeDef TA_void;
extern TA_API TypeDef TA_void_ptr;
extern TA_API TypeDef TA_char;
//extern TA_API TypeDef TA_int8_t; // on Windows
extern TA_API TypeDef TA_signed_char;
  extern TA_API TypeDef TA_int8_t;
extern TA_API TypeDef TA_unsigned_char;
  extern TA_API TypeDef TA_uint8_t;  // note: seemingly absent in MSVC
extern TA_API TypeDef TA_short;
  extern TA_API TypeDef TA_signed_short;
  extern TA_API TypeDef TA_short_int;
  extern TA_API TypeDef TA_signed_short_int;
  extern TA_API TypeDef TA_int16_t;
extern TA_API TypeDef TA_unsigned_short;
  extern TA_API TypeDef TA_unsigned_short_int;
  extern TA_API TypeDef TA_uint16_t; // note: seemingly absent in MSVC
extern TA_API TypeDef TA_int;
  extern TA_API TypeDef TA_signed;
  extern TA_API TypeDef TA_signed_int;
  extern TA_API TypeDef TA_int32_t;
//extern TA_API TypeDef TA_intptr_t; // on 32-bit systems
//extern TA_API TypeDef TA_long; // where long is 32-bit
extern TA_API TypeDef TA_unsigned_int;
  extern TA_API TypeDef TA_unsigned;
  extern TA_API TypeDef TA_uint;
//extern TA_API TypeDef TA_unsigned_long; // where long is 32-bit
  extern TA_API TypeDef TA_uint32_t; // note: seemingly absent in MSVC
extern TA_API TypeDef TA_long;
  extern TA_API TypeDef TA_signed_long;
  extern TA_API TypeDef TA_long_int;
  extern TA_API TypeDef TA_signed_long_int;
extern TA_API TypeDef TA_unsigned_long;
  extern TA_API TypeDef TA_unsigned_long_int;
  extern TA_API TypeDef TA_ulong;
extern TA_API TypeDef TA_int64_t;
  extern TA_API TypeDef TA_long_long;
  extern TA_API TypeDef TA_signed_long_long;
//extern TA_API TypeDef TA_intptr_t; // on 64-bit systems
//extern TA_API TypeDef TA_long; // where long is 64-bit
extern TA_API TypeDef TA_uint64_t; // note: seemingly absent in MSVC
  extern TA_API TypeDef TA_unsigned_long_long;
//extern TA_API TypeDef TA_unsigned_long; // where long is 64-bit
extern TA_API TypeDef TA_intptr_t; //NOTE: synonym, will either be 'int' or 'int64_t'
extern TA_API TypeDef TA_uintptr_t;
extern TA_API TypeDef TA_float;
extern TA_API TypeDef TA_double;
extern TA_API TypeDef TA_bool;
extern TA_API TypeDef TA_const;	// const is not formal...
extern TA_API TypeDef TA_enum;		// par_formal
extern TA_API TypeDef TA_struct;	// par_formal
extern TA_API TypeDef TA_union;	// par_formal
extern TA_API TypeDef TA_class;	// par_formal
extern TA_API TypeDef TA_template;	// par_formal
extern TA_API TypeDef TA_templ_inst;	// par_formal template instantiation
extern TA_API TypeDef TA_ta_array;	// par_formal (indicates "arrayness")
extern TA_API TypeDef TA_taBase;
extern TA_API TypeDef TA_taRegFun;	// registered functions
extern TA_API TypeDef TA_TypeDef;	// give these to the user...
extern TA_API TypeDef TA_EnumDef;
extern TA_API TypeDef TA_MemberDef;
extern TA_API TypeDef TA_MemberDefBase;
extern TA_API TypeDef TA_PropertyDef;
extern TA_API TypeDef TA_MethodDef;
extern TA_API TypeDef TA_ta_Globals;
extern TA_API TypeDef TA_taString;
extern TA_API TypeDef TA_Variant;
#endif

// externals
// for gui support
#ifdef TA_GUI
class taiDataLink; //
class taiType; //
class taiEdit; //
class taiViewType; //
class taiMember; //
class taiMethod; //
class QMainWindow;
#endif

// for css support
class cssEl; 			// #IGNORE

typedef cssEl* (*css_fun_stub_ptr)(void*, int, cssEl**);
typedef void (*ta_void_fun)();

class TA_API ta_memb_ptr_class {
public:
  virtual ~ta_memb_ptr_class()	{ }; // make sure it has a vtable..
};

typedef int ta_memb_ptr_class::* ta_memb_ptr; //

// for Properties

typedef Variant (*ta_prop_get_fun)(const void*);
typedef void (*ta_prop_set_fun)(void*, const Variant&);


class taBase;
typedef taBase* TAPtr;		// pointer to a taBase type

class TA_API String_PArray : public taPlainArray<String> {
  // #NO_TOKENS a plain-array of strings
INHERITED(taPlainArray<String>)
public:
  static const String	def_sep; // ", "
  
  int	FindContains(const String& op, int start=0) const;
  // find item that contains string -- start < 0 = start from end of array (not strings!)
  int	FindStartsWith(const String& op, int start=0) const;
  // find item that starts with string -- start < 0 = start from end of array (not strings!)

  const String 	AsString(const String& sep = def_sep) const;
  void		SetFromString(String str, const String& sep = def_sep);
  void	operator=(const String_PArray& cp)	{ Copy_Duplicate(cp); }
  String_PArray()				{ };
  String_PArray(const String_PArray& cp)	{ Copy_Duplicate(cp); }
  // returns first item which contains given string (-1 if none)
protected:
  int		El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((String*)a) > *((String*)b)) rval=1; else if(*((String*)a) == *((String*)b)) rval=0; return rval; }
  bool		El_Equal_(const void* a, const void* b) const
    { return (*((String*)a) == *((String*)b)); }
  String	El_GetStr_(const void* it) const { return (*((String*)it)); }
  void		El_SetFmStr_(void* it, const String& val)
  {*((String*)it) = val; }
};

class TA_API int_PArray: public taPlainArray<int> {
  // #NO_TOKENS a plain-array of ints
public:
  void	operator=(const int_PArray& cp)	{ Copy_Duplicate(cp); }
  int_PArray()				{ };
  int_PArray(const int_PArray& cp)	{ Copy_Duplicate(cp); }

protected:
  int		El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((int*)a) > *((int*)b)) rval=1; else if(*((int*)a) == *((int*)b)) rval=0; return rval; }
  bool		El_Equal_(const void* a, const void* b) const
    { return (*((int*)a) == *((int*)b)); }
  String	El_GetStr_(const void* it) const { return (*((int*)it)); }
  void		El_SetFmStr_(void* it, const String& val)
  { int tmp = (int)val; *((int*)it) = tmp; }
};

class TA_API NameVar_PArray : public taPlainArray<NameVar> {
  // #NO_TOKENS a plain-array of name value (variant) items
INHERITED(taPlainArray<NameVar>)
public:
  static const String	def_sep; // ", "
  
  int	FindName(const String& nm, int start=0) const;
  // find by name  (start < 0 = from end)
  int	FindNameContains(const String& nm, int start=0) const;
  // find by name containing nm (start < 0 = from end)
  int	FindValue(const Variant& var, int start=0) const;
  // find by value (start < 0 = from end)  
  int	FindValueContains(const String& vl, int start=0) const;
  // find by value.toString() containing vl (start < 0 = from end)

  Variant	GetVal(const String& nm);
  // get value from name; isNull if not found
  Variant	GetValDef(const String& nm, const Variant& def);
  // get value from name; def if not found
  bool		GetAllVals(const String& nm, String_PArray& vals);
  // get all values having given name (converts to strings)
  bool		SetVal(const String& nm, const Variant& val);
  // set value by name; if name already on list, it is updated (rval = true); else new item added

  const String 	AsString(const String& sep = def_sep) const;
  void	operator=(const String_PArray& cp)	{ Copy_Duplicate(cp); }
  NameVar_PArray()				{ };
  NameVar_PArray(const String_PArray& cp)	{ Copy_Duplicate(cp); }
protected:
  int		El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(((NameVar*)a)->value > ((NameVar*)b)->value) rval=1; else if(((NameVar*)a)->value == ((NameVar*)b)->value) rval=0; return rval; }
  bool		El_Equal_(const void* a, const void* b) const
  { return (((NameVar*)a)->value == ((NameVar*)b)->value); }
  String	El_GetStr_(const void* it) const { return ((NameVar*)it)->GetStr(); }
  void		El_SetFmStr_(void* it, const String& val) { ((NameVar*)it)->SetFmStr(val); }
};

#ifdef __MAKETA__
class TA_API void_PArray {
#else
class TA_API void_PArray: public taPlainArray<void*> {
#endif
  // #NO_TOKENS #NO_MEMBERS #NO_CSS a plain-array of void* pointers
public:
  void	operator=(const void_PArray& cp)	{ Copy_Duplicate(cp); }
  void_PArray()				{ };
  void_PArray(const void_PArray& cp)	{ Copy_Duplicate(cp); }

protected:
  int		El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((void**)a) > *((void**)b)) rval=1; else if(*((void**)a) == *((void**)b)) rval=0; return rval; }
  bool		El_Equal_(const void* a, const void* b) const
    { return (*((void**)a) == *((void**)b)); }
  String	El_GetStr_(const void* it) const { return (ta_intptr_t)(*((void**)it)); }
  void		El_SetFmStr_(void* it, const String& val)
  { int tmp = (int)val; *((void**)it) = (void*)tmp; }
};

class TA_API DumpFileCvt {
  // ##NO_TOKENS #CAT_File parameters to convert a dump file
public:
  String	proj_type_base;	// base name  of project (e.g., "Leabra" for "LeabraProject")
  String	key_srch_str;	// search string to identify this project type
  NameVar_PArray repl_strs; // search/replace strings (name -> value) 

  DumpFileCvt(const String& prj_typ, const String& srch_st)
    { proj_type_base = prj_typ; key_srch_str = srch_st; }
};

class TA_API DumpFileCvtList : public taPtrList<DumpFileCvt> {
  // #CAT_File list of available dump file converters
protected:
  void		El_Done_(void* it)	{ delete (DumpFileCvt*)it; }
  String El_GetName_(void* it) const { return ((DumpFileCvt*)it)->proj_type_base; }
public:
  ~DumpFileCvtList()                            { Reset(); }
};

#ifndef NO_TA_BASE

class UserDataItem_List;
class UserDataItemBase;
class ViewColor_List;

enum CancelOp { // ops for passing cancel status and instructions, typically for Window closing 
  CO_PROCEED,		// tells caller to proceed with operation (typical default)
  CO_CANCEL,		// client can set this to tell caller to cancel the operation
  CO_NOT_CANCELLABLE	// preset, to tell client that operation will go ahead unconditionally
};


class TA_API taiMiscCore: public QObject { 
  // ##NO_TOKENS ##NO_INSTANCE object for Qt Core event processing, etc. taiMisc inherits; taiM is always instance
INHERITED(QObject)
  Q_OBJECT
friend class TypeDef; // for the post_load guy
public:
#ifndef __MAKETA__
  enum CustomEvents {
    CE_QUIT		= QEvent::User + 1 // sent from RootWin::CloseEvent to avoid issues
  };
#endif
  static int		rl_callback(); // a readline-compatible callback -- calls event loop

  static taiMiscCore*	New(QObject* parent = NULL);
    // either call this or call taiMisc::New 
  
  static void		WaitProc(); // the core idle loop process
  
  static int		ProcessEvents();
  // run any pending qt events that might need processed
  static int		RunPending();
  // check to see if any events are pending, and run if true -- MUCH faster than processevents, but also likely to miss some events along the way.
  
  static void		Quit(CancelOp cancel_op = CO_NOT_CANCELLABLE); 
   // call to quit, invokes Quit_impl on instance first
  static void		OnQuitting(CancelOp& cancel_op); // call this when a quit situation is detected -- does all the save logic
  
  const String		classname(); // 3.x compatability, basically the app name
  
  int			Exec(); // enter the event loop, either QCoreApplication or QApplication
  virtual void		Busy_(bool busy) {} // impl for gui in taiMisc
  virtual void		CheckConfigResult_(bool ok);
    // this is the nogui version; taiMisc does the gui version
    
  taiMiscCore(QObject* parent = NULL);
  ~taiMiscCore();
  
public slots:
  void	   		PostUpdateAfter(); // run the dumpMisc::PostUpdateAfter; called asynchronously via a timer  

protected slots:
  void			app_aboutToQuit();
  virtual void		timer_timeout(); // called when timer times out, for waitproc processing
  
protected:
  QTimer*		timer; // for idle processing
  override void		customEvent(QEvent* ev);
  virtual void		Init(bool gui = false); // NOTE: called from static New
  virtual int		Exec_impl();
  virtual void		OnQuitting_impl(CancelOp& cancel_op); // allow to cancel
  virtual void		Quit_impl(CancelOp cancel_op); // non-gui guy does nothing
};

extern TA_API taiMiscCore* taiMC_; // note: use taiM macro instead
#ifdef TA_GUI
# define taiM taiM_
#else
# define taiM taiMC_
#endif
#endif

typedef  void (*init_proc_t)() ;	// initialization proc

class TA_API InitProcRegistrar {
  // #IGNORE object used as a module static instance to register an init_proc
public:
  InitProcRegistrar(init_proc_t init_proc);
private:
  InitProcRegistrar(const InitProcRegistrar&);
  InitProcRegistrar& operator =(const InitProcRegistrar&);
};


#ifdef __MAKETA__
//typedef unsigned char ContextFlag; // binary-compat alias for CSS
#define ContextFlag unsigned char
#else
class TA_API ContextFlag { // replacement for is_xxx flags that retains bool test semantics, but does proper enter/exit counting; use in ++ ... -- pairs
public:
  operator bool() {return (cnt != 0);}
  signed char 	operator++() {return ++cnt;}
  signed char 	operator++(int) {return cnt++;} // post
  signed char 	operator--() {return --cnt;}
  signed char 	operator--(int)  {return cnt--;} // post
  
  ContextFlag() {cnt = 0;} // NOTE: default binary copy constructor and copy operator are fine
private:
  signed char	cnt; // keep same size as bool -- should never be nesting this deep
};
#endif // __MAKETA__


class TA_API taVersion { // #EDIT_INLINE simple value class for version info
public:
  ushort	major;
  ushort	minor;
  ushort	step;
  ushort	build;
  
  void		set(ushort mj, ushort mn, ushort st = 0, ushort bld = 0)
    {major = mj; minor = mn; step = st; build = bld;}
  void		setFromString(String ver); // parse, mj.mn.st-build
  const String	toString() 
    {return String(major).cat(".").cat(String(minor)).cat(".").
       cat(String(step)).cat(".").cat(String(build));}

  void		Clear() {major = minor = step = build = 0;} //
  
  
  taVersion() {Clear();} //
  taVersion(ushort mj, ushort mn, ushort st = 0, ushort bld = 0) 
    {set(mj, mn, st, bld);} //
  taVersion(const String& ver) {setFromString(ver);}
// implicit copy and assign
private:
  int		BeforeOrOf(char sep, String& in);
};

/////////////////////////////////////////////////////////////////////
// 	taMisc

class TA_API taMisc {
  // #NO_TOKENS #INSTANCE global parameters and functions for the application
friend class InitProcRegistrar;
public:

  // note: don't rationalize the memb bits, because it breaks the user prefs file
  // the NO_xxx guys are for use in 'forbidden' contexts (legacy 'show')
  // the IS_xxx guys are for use in 'allowed' contexts, and/or categorizing guys
  enum ShowMembs { // #BITS
    NO_HIDDEN 		= 0x01, // don't show items marked READ_ONLY w/o SHOW or HIDDEN
    NO_unused1		= 0x02, // #IGNORE
    NO_unused2		= 0x04, // #IGNORE
    NO_NORMAL		= 0x08, // #NO_SHOW don't show items normally shown (helps indicate, ex. EXPERT items)
    NO_EXPERT		= 0x10, // don't show items marked EXPERT (often only for advanced sims)

    ALL_MEMBS		= 0x00, // #NO_BIT
    NORM_MEMBS 		= 0x11, // #NO_BIT
    EXPT_MEMBS		= 0x09, // #NO_BIT
    HIDD_MEMBS		= 0x18, // #NO_BIT
    
    IS_HIDDEN 		= 0x01, // #IGNORE used in MemberDef::ShowMember to flag RO w/o SHOW or HIDDEN guys
    IS_NORMAL		= 0x08, // #IGNORE used in MemberDef::ShowMember to flag NORMAL guys
    IS_EXPERT		= 0x10, // #IGNORE used in MemberDef::ShowMember to flag EXPERT guys
#ifndef __MAKETA__
    SHOW_CHECK_MASK	= IS_HIDDEN | IS_NORMAL | IS_EXPERT, // #IGNORE #NO_BIT used in MemberDef::ShowMember checks, default for "allowed" param
#endif
    USE_SHOW_GUI_DEF 	= 0x40	// #NO_BIT use default from taMisc::show_gui, only applies to forbidden
  };

  enum TypeInfo {
    MEMB_OFFSETS,		// display all including member offsets
    ALL_INFO,			// display all type information
    NO_OPTIONS,			// don't display options
    NO_LISTS,			// don't display lists
    NO_OPTIONS_LISTS 		// don't display options or lists
  };

  enum KeepTokens {
    Tokens,			// keep tokens as specified by the type
    NoTokens,			// don't keep any tokens
    ForceTokens 		// force to keep all tokens
  };

  enum SaveFormat {
    PLAIN,			// dump files are not formatted for easy reading
    PRETTY 			// dump files should be more readable by humans
  };

  enum LoadVerbosity {
    QUIET,			// don't say anything except errors
    MESSAGES,			// display informative messages during load
    TRACE,			// and show a trace of objects loaded
    SOURCE 			// and show the source of the load as its loaded
  };

  enum AutoRevert {
    AUTO_APPLY,			// automatically apply changes before auto-reverting
    AUTO_REVERT,		// automatically revert, losing changes
    CONFIRM_REVERT 		// put up a confirmatory message before reverting
  };

  enum TypeInfoKind { // used in switch statements to particularize instances
    TIK_ENUM,
    TIK_MEMBER,
    TIK_METHOD,
    TIK_PROPERTY,
    TIK_TYPE,
    TIK_ENUMSPACE,
    TIK_TOKENSPACE,
    TIK_MEMBERSPACE,
    TIK_METHODSPACE,
    TIK_PROPERTYSPACE, // note: holds mix of PropertyDef and MemberDef
    TIK_TYPESPACE,
    TIK_UNKNOWN
  };
  
  enum QuitFlag { // helps during shutdown to know whether we can cancel
    QF_RUNNING		= 0, // 
    QF_USER_QUIT,	// we can still cancel while in this state of quitting
    QF_FORCE_QUIT	// too late to turn back now...
  };
  
  enum ConsoleType { // the type of console window and how to show it; ignored in non-gui mode (either uses OS shell, or no console, depending on startup mode)
    CT_OS_SHELL = 0, // #LABEL_OS_Shell use the operating system's shell or console (with readline library on unix)
#ifdef HAVE_QT_CONSOLE // qt console not supported on windows, needs to be ported to Win32
    CT_GUI = 1, // #LABEL_Gui uses a gui-based console, either docked in the main app window, or floating (see console_options)
#else
    CT_GUI = 1, // #NO_SHOW uses a gui-based console, either docked in the main app window, or floating (see console_options)
#endif
    CT_NONE = 4 // #NO_SHOW no console, usually only used internally, such as for batch or dmem operation
  };
  
  enum ConsoleOptions { // #BITS options that can be used with the console
    CO_0 = 0, // #NO_BIT #IGNORE dummy item, and to clear
#ifdef TA_OS_WIN // pager causes crashes and deadlocks in windows
    CO_USE_PAGING_GUI	= 0x0001, // #NO_BIT use paging, like the "more" command -- NOT RECOMMENDED FOR WINDOWS
#else
    CO_USE_PAGING_GUI	= 0x0001, // #LABEL_Use_Paging_Gui use paging in the gui console, like the 'more' command
#endif
    CO_USE_PAGING_NOGUI = 0x0002, // #LABEL_Use_Paging_NoGui use paging in the nogui console/shell, like the 'more' command
#ifdef HAVE_QT_CONSOLE
    CO_GUI_TRACKING	= 0x0004, // #LABEL_Gui_Tracking in GUI mode, the console floats below the active project
#else
    CO_GUI_TRACKING	= 0x0004, // #NO_SHOW in GUI mode, the console floats below the active project
#endif
  };
  
  enum ColorHints { // #BITS what types of color hinting to use in the application
    CH_EDITS		= 0x0001, // color the background of property editors according to the type of the item
    CH_BROWSER		= 0x0002 // color browser tree text according to the type of the item
  };
  
  enum BuildType { // #BITS what type of build this is
    BT_0		= 0x00, // #NO_BIT constant for when no other flags set
    BT_DEBUG		= 0x01, // a debug build
    BT_DMEM		= 0x02, // compiled for MPI (clustered use)
    BT_NO_GUI		= 0x04  // compiled without gui support
  };
  
  enum ClickStyle { // how to select editable items, such as in T3
    CS_SINGLE,	// #LABEL_Single single click opens properties
    CS_CONTEXT  // #LABEL_Context choose from context menu 
  };

  enum MatrixView { 	// order of display for matrix cols
    BOT_ZERO, 	// row zero is displayed at bottom of view (default)
    TOP_ZERO 	// row zero is displayed at top of view (ex. for images)
  };
  
  enum ProjViewPref { // project viewing preference
    PVP_2x2,	// #LABEL_2x2_Panes one window has a browser and edit pane, the second has an edit pane and a 3-d viewer
    PVP_3PANE   // #LABEL_3_Pane one window with a browser, edit pane, and a 3d-viewer
  };
  
  enum ViewerOptions { // #BITS options for the viewer
    VO_0		= 0, // #IGNORE
    VO_DOUBLE_CLICK_EXP_ALL = 0X001, // #LABEL_DoubleClickExpAll double click expands or contracts all tree items -- use at your own risk on big projects...
    VO_AUTO_SELECT_NEW	= 0x002, // #LABEL_AutoSelectNew automatically select (the first) of a new tree item that is made with New or similar menu commands
    VO_AUTO_EXPAND_NEW	= 0x004, // #LABEL_AutoExpandNew automatically expand new tree items that are made with New or similar menu commands
  };

  enum	GuiStyle {	// style options provided by the gui system (not all are available on all platforms)
    GS_DEFAULT,		// #LABEL_Default use the default style for whatever platform you're running on
    GS_PLASTIQUE,	// #LABEL_Plastique default style on linux -- a good choice to try
    GS_CLEANLOOKS,	// #LABEL_CleanLooks similar overall to plastique, with a bit more of a windows look
    GS_MOTIF,		// #LABEL_Motif a classic look from the 90's -- one of the first 3d-looks
    GS_CDE,		// #LABEL_CDE Common Desktop Environment -- a bit lighter and cleaner than MOTIF, but overall very similar to it
    GS_MACINTOSH,	// #LABEL_Macintosh only available on a macintosh!
    GS_WINDOWS,		// #LABEL_Windows standard old-school Microsoft Windows (pre XP)
    GS_WINDOWSXP,	// #LABEL_WindowsXP Windows XP look -- only available on Windows XP or higher
  };    
  
  static String		app_name; // #READ_ONLY #NO_SAVE #SHOW the root name of the app, ex. "pdp++"
  static String		app_lib_name; // #READ_ONLY #NO_SAVE #EXPERT the root name of the app's library, if any, ex. "pdp" (none for css)
  static String		default_app_install_folder_name; // #READ_ONLY #NO_SAVE #HIDDEN the default folder name for installation, ex. "Emergent"
  static String		org_name; // #READ_ONLY #NO_SAVE #SHOW the name of the organization, ex. ccnlab (used mostly in Windows paths)
  static String		version; // #READ_ONLY #NO_SAVE #SHOW version number of ta/css
  static taVersion 	version_bin; 
   //  #READ_ONLY #NO_SAVE #EXPERT version number of ta/css
#ifdef SVN_REV
  static String		svn_rev; 
   // #READ_ONLY #NO_SAVE #SHOW svn revision number (only valid when configure has been rerun)
#endif
  static const BuildType build_type; // #READ_ONLY #NO_SAVE #SHOW build type, mostly for determining plugin subfolders to search
  static const String	build_str; // #READ_ONLY #NO_SAVE #EXPERT an extension string based on build type, mostly for plugin subfolders (none for release gui no-dmem) 
  
  ////////////////////////////////////////////////////////
  // 	TA GUI parameters

  static GuiStyle	gui_style;	// #SAVE #CAT_GUI #DEF_GS_DEFAULT style options provided by the gui system, affecting how the widgets are drawn, etc (not all are available on all platforms) -- change only takes effect on restarting the program
  static String		font_name;	// #SAVE #CAT_GUI default font name to use
  static int		font_size;	// #SAVE #CAT_GUI default font size to use
  static ConsoleType	console_type; // #SAVE #CAT_GUI style of the console to display -- **REQUIRES APP RESTART
#ifdef TA_OS_WIN // none on windows (yet), so we omit for clarity
  static ConsoleOptions	console_options; // #IGNORE #CAT_GUI options for the console **REQUIRES APP RESTART
#else
  static ConsoleOptions	console_options; // #SAVE #CAT_GUI options for the console **REQUIRES APP RESTART
#endif
  static String		console_font_name;	// #SAVE #CAT_GUI font name for the css console
  static int		console_font_size;	// #SAVE #CAT_GUI font size for the css console
  static int		display_width;	// #SAVE #HIDDEN #MIN_40 #MAX_132 #CAT_GUI width of console display (in chars) -- set automatically by gui console
  
  static int		tree_indent; 	// #SAVE #CAT_GUI number of pixels to indent in the tree browser gui interface

  static int		max_menu;	// #SAVE #CAT_GUI #EXPERT maximum number of items in a menu
  static int		search_depth;   // #SAVE #CAT_GUI #EXPERT depth recursive find will search for a path object
  static int		color_scale_size; // #SAVE #CAT_GUI #EXPERT number of colors to put in a color scale
  static int		jpeg_quality; 	// #SAVE #CAT_GUI jpeg quality for dumping jpeg files (1-100; 95 default)
  static ColorHints 	color_hints; // #SAVE #CAT_GUI what types of color hinting to use in the application
  static ClickStyle	click_style; // #SAVE #CAT_GUI how to select editable items in the gui, particularly in the 3d gui
  static ProjViewPref	proj_view_pref; // #SAVE #CAT_GUI the default way to view projects
  static ViewerOptions	viewer_options; // #SAVE #CAT_GUI misc options for the viewer
#ifndef NO_TA_BASE
//NOTE: following not keeping tokens so cannot be viewed in any mode
  static ViewColor_List* view_colors; 	// #NO_SAVE #NO_SHOW colors to use in the view displays -- looked up by name emitted by GetTypeDecoKey and GetStateDecoKey on objects
#endif
  static int		antialiasing_level; // #SAVE #CAT_GUI level of smoothing to perform in the 3d display -- values depend on hardware acceleration, but 2 or 4 are typical values.  1 or lower disables entirely.  modern hardware can do typically do level 4 with little slowdown in speed.
  static float		text_complexity;     // #SAVE #CAT_GUI #EXPERT complexity value (between 0 and 1 for rendering text -- a lower number (e.g., .1) should make things faster, without much cost in the display quality

  static ShowMembs	show_gui;	// #SAVE #CAT_GUI what to show in the gui
  static TypeInfo	type_info_;	// #SAVE #CAT_GUI #EXPERT #LABEL_type_info what to show when displaying type information
  //note: 'type_info' is a reserved word in C++, it is the type of rtti data
  static KeepTokens	keep_tokens;	// #SAVE #CAT_GUI #EXPERT default for keeping tokens
  static SaveFormat	save_format;	// #SAVE #CAT_GUI #EXPERT format to use when saving things (dump files)
  static bool		auto_edit; 	// #SAVE #CAT_GUI #EXPERT automatic edit dialog after creation?
  static AutoRevert	auto_revert;    // #SAVE #CAT_GUI #EXPERT when dialogs are automatically updated (reverted), what to do about changes?
  static MatrixView	matrix_view;	// #SAVE #CAT_GUI #EXPERT #DEF_BOT_ZERO whether to show matrices with 0 row at top or bottom of view
  static bool		beep_on_error; // #SAVE #DEF_false #CAT_GUI beep when an error message is printed on the console
  static short		num_recent_files; // #SAVE #DEF_10 #MIN_0 #MAX_12 number of recent files to save
  static short		num_recent_paths; // #SAVE #DEF_10 #MIN_0 #MAX_20 number of recent paths to save

  ////////////////////////////////////////////////////////
  // 	File/Paths Info

  static int		strm_ver;	// #READ_ONLY #NO_SAVE during dump or load, version # (app v4.x=v2 stream)
  static bool		save_compress;	// #SAVE #DEF_false #CAT_File compress by default for files that support it (ex .proj, .net)\nNOTE: starting with v4.0, compression is no longer recommended except for large weight files or large nets with saved units
  static TypeDef*	default_proj_type; // #SAVE #CAT_File #TYPE_taProject default type of project to create
  static LoadVerbosity	verbose_load;	// #SAVE #CAT_File #EXPERT report the names of things during loading

  static String		app_dir; 
  // #SHOW #READ_ONLY #CAT_File base of installed app directory -- override with "-a <path>" command line switch
  static String		app_dir_default; 
  // #HIDDEN #READ_ONLY #SAVE #CAT_File an override to use ONLY if we can't determine the app_dir
  static String		user_dir;
  // #SHOW #READ_ONLY #CAT_File location of user's home directory -- override with "-u <path>" command line switch
  static String		web_home;
  // #SAVE #CAT_File url for location of web repository of package information
//NOTE: help url is not saved, for the moment, but s/b for release
  static String		web_help_url;
  // #NO_SAVE #CAT_File url for base of web application help
  static String		prefs_dir;
  // #READ_ONLY #SHOW #CAT_File location of preference files (e.g., ~/.appname)
  static String		user_app_dir;
  // #SAVE #SHOW #CAT_File user's location of application, for user plugins, etc. 

  // don't save these paths: they are generated from above which are saved, and can
  // be modified more reliably in a .cssinitrc or similar..
  static String_PArray 	css_include_paths;
  // #NO_SAVE #HIDDEN #CAT_File paths to be used for finding css files (e.g., in #include or load statements -- searched in order)
  static String_PArray 	load_paths;
  // #NO_SAVE #HIDDEN #CAT_File paths to be used for loading object files for the ta dump file system
  static NameVar_PArray	prog_lib_paths;
  // #NO_SAVE #HIDDEN #CAT_File paths/url's for specific categories of program library files (e.g., System, User, Web)
  static NameVar_PArray	named_paths;
  // #NO_SAVE #HIDDEN #CAT_File paths/url's for misc purposes -- search by name, value = path

  static DumpFileCvtList file_converters;   // #CAT_File #HIDDEN conversion parameters (from v.3 to v.4)

  static String		compress_sfx;	// #SAVE #CAT_File #EXPERT suffix to use for compressing files

  static ostream*	record_script;  // #IGNORE #CAT_File stream to use for recording a script of interface activity (NULL if no record)

  static String		help_file_tmplt; // #SAVE #CAT_File template for converting type name into a help file (%t = type name)
  static String		help_cmd;	// #SAVE #CAT_File how to run html browser to get help, %s is entire path to help file
  static String		edit_cmd;	// #SAVE #CAT_File how to run editor
  
  ////////////////////////////////////////////////////////
  // 	Args

  static String_PArray	args_raw;
  // #READ_ONLY #NO_SAVE #HIDDEN #CAT_Args raw list of arguments passed to program at startup (in order, no filtering or modification)
  static NameVar_PArray	arg_names;
  // #READ_ONLY #NO_SAVE #HIDDEN #CAT_Args conversions between arg flags (as a String in name field, e.g., -f or --file) and a canonical functional name (in value field, e.g., CssScript)
  static NameVar_PArray	arg_name_descs;
  // #READ_ONLY #NO_SAVE #HIDDEN #CAT_Args descriptions of arg names for help -- name is canonical functional name (e.g., CssScript) and value is string describing what this arg does
  static NameVar_PArray	args;
  // #READ_ONLY #NO_SAVE #HIDDEN #CAT_Args startup arguments processed by arg_names into name/value pairs -- this is the list that should be used!
  static String_PArray	args_tmp;
  // #NO_SAVE #HIDDEN #CAT_Args temporary list of args; can be passed to GetAllArgsNamed in scripts..

  ////////////////////////////////////////////////////////
  // 	DMEM: Distributed Memory

  static int		dmem_proc; 	
  // #READ_ONLY #EXPERT #NO_SAVE #SHOW #CAT_DMem distributed memory process number (rank in MPI, always 0 for no dmem)
  static int		dmem_nprocs;
  // #READ_ONLY #EXPERT #NO_SAVE #SHOW #CAT_DMem distributed memory number of processes (comm_size in MPI, 1 for no dmem)
  static int		cpus;
  // #READ_ONLY #EXPERT #NO_SAVE #SHOW #CAT_MultiProc number of cpus to use (<= physical cpus)
  static bool		dmem_debug;
  // #SAVE #EXPERT #CAT_DMem turn on debug messages for distributed memory processing

  ////////////////////////////////////////////////////////
  // 	Global State, Flags Etc

  static TypeSpace 	types;		// #READ_ONLY #NO_SAVE list of all the active types
  static TypeDef*	default_scope;  // #READ_ONLY #NO_SAVE type of object to use to determine if two objects are in the same scope

  static taPtrList_impl* init_hook_list; // #IGNORE list of init hook's to call during initialization

  static bool		in_init;	// #READ_ONLY #NO_SAVE #NO_SHOW true if in ta initialization function
  static bool		in_event_loop;	// #READ_ONLY #NO_SAVE #NO_SHOW true when in the main event loop (ex. now ok to do ProcessEvents)
  static signed char	quitting;	// #READ_ONLY #NO_SAVE #NO_SHOW true, via one of QuitFlag values, once we are quitting
  static bool		not_constr;	// #READ_ONLY #NO_SAVE #NO_SHOW true if ta types are not yet constructed (or are destructed)

  static bool		use_gui;	// #READ_ONLY #NO_SAVE #NO_SHOW whether the user has specified to use the gui or not (default = true)
  static bool		gui_active;	// #READ_ONLY #NO_SAVE #NO_SHOW if gui has been started up or not
  static bool		server_active;	// #READ_ONLY #NO_SAVE #NO_SHOW if remote server has been started up or not
  static ContextFlag	is_loading;	// #READ_ONLY #NO_SAVE #NO_SHOW true if currently loading an object
  static ContextFlag	is_post_loading;// #READ_ONLY #NO_SAVE #NO_SHOW true if currently in the post load routine (DUMP_POST_LOAD)
  static ContextFlag	is_saving;	// #READ_ONLY #NO_SAVE #NO_SHOW true if currently saving an object
  static ContextFlag	is_duplicating;	// #READ_ONLY #NO_SAVE #NO_SHOW true if currently duplicating an object
  static ContextFlag	is_checking;	// #READ_ONLY #NO_SAVE #NO_SHOW true if currently doing batch CheckConfig on objects
  static ContextFlag	in_gui_call;	// #READ_ONLY #NO_SAVE #NO_SHOW true if we are running a function call from the gui (used to modalize warning dialogs)
  static ContextFlag	in_plugin_init;	// #READ_ONLY #NO_SAVE #NO_SHOW true if currently loading typeinfo for a plugin
  static TypeDef*	plugin_loading; // #READ_ONLY #NO_SAVE #NO_SHOW the TypeDef of the plugin currently loading -- we stamp this into all formal classes

  static String		last_check_msg; // #READ_ONLY #NO_SAVE #EDIT_DIALOG last error, or last batch of errors (if checking) by CheckConfig
  static bool		check_quiet; 	// #IGNORE mode we are in; set by CheckConfigStart
  static bool		check_confirm_success; // #IGNORE mode we are in; set by CheckConfigStart
  static bool		check_ok; 	// #IGNORE cumulative AND of all nested oks
  static int		err_cnt; //  #READ_ONLY #NO_SAVE cumulative error count; can be used/reset by Server to detect for errors after it calls a routine
  static int		CheckClearErrCnt(); // gets current value, and clears
  
#if (defined(TA_GUI) && !(defined(__MAKETA__) || defined(NO_TA_BASE)))
  static QPointer<QMainWindow>	console_win;	// #IGNORE the console window 
#endif

  static void	(*WaitProc)();
  // #IGNORE set this to a work process for idle time processing
  static void (*ScriptRecordingGui_Hook)(bool); // #IGNORE gui callback when script starts/stops; var is 'start'

  /////////////////////////////////////////////////
  //	Configuration -- object as settings

  void	SaveConfig();
  // #CAT_Config save configuration defaults to <appdata>/taconfig file that is loaded automatically at startup
  void	LoadConfig();
  // #CAT_Config load configuration defaults from <appdata>/.taconfig file (which is loaded automatically at startup)

  /////////////////////////////////////////////////
  //	Errors, Warnings, Simple Dialogs

  static String	SuperCat(const char* a, const char* b, const char* c,
		      const char* d, const char* e, const char* f,
		      const char* g, const char* h, const char* i);
  // #CAT_Dialog concatenate strings with spaces between
  
  static void 	Error(const char* a, const char* b=0, const char* c=0,
		      const char* d=0, const char* e=0, const char* f=0,
		      const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog displays error either in a window+stderr if gui_active or to stderr only
  static void 	Error_nogui(const char* a, const char* b=0, const char* c=0,
			    const char* d=0, const char* e=0, const char* f=0,
			    const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog explicit no-gui version of error: displays error to stderr only

#ifndef NO_TA_BASE
  static bool 	TestError(const taBase* obj, bool test, const char* fun_name,
			  const char* a, const char* b=0, const char* c=0,
			  const char* d=0, const char* e=0, const char* f=0,
			  const char* g=0, const char* h=0);
  // #CAT_Dialog if test, then report error, including object name, type, and path information if non-null; returns test -- use e.g. if(taMisc::TestError(this, (condition), "fun", "msg")) return false;
  static bool 	TestWarning(const taBase* obj, bool test, const char* fun_name,
			    const char* a, const char* b=0, const char* c=0,
			    const char* d=0, const char* e=0, const char* f=0,
			    const char* g=0, const char* h=0);
  // #CAT_Dialog if test, then report warning, including object name, type, and path information if non-null; returns test -- use e.g. if(taMisc::TestWarning(this, (condition), "fun", "msg")) return false;
#endif

  static void 	CheckError(const char* a, const char* b=0, const char* c=0,
		      const char* d=0, const char* e=0, const char* f=0,
		      const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog called by CheckConfig routines; enables batching up of errors for display
  
  static void 	Warning(const char* a, const char* b=0, const char* c=0,
		      const char* d=0, const char* e=0, const char* f=0,
		      const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog displays warning to stderr and/or other logging mechanism

  static void 	Info(const char* a, const char* b=0, const char* c=0,
		      const char* d=0, const char* e=0, const char* f=0,
		      const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog displays informative msg to stdout and/or other logging mechanism

  static int 	Choice(const char* text="Choice", const char* a="Ok", const char* b=0,
		       const char* c=0, const char* d=0, const char* e=0,
		       const char* f=0, const char* g=0, const char* h=0,
		       const char* i=0);
  // #CAT_Dialog allows user to choose among different options in a popup dialog window if gui_active (else stdin/out) -- option a returns 0, b = 1, etc.
  
  static void 	Confirm(const char* a, const char* b=0, const char* c=0,
		      const char* d=0, const char* e=0, const char* f=0,
		      const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog displays informative msg in a dialog -- use this instead of Choice for such confirmations

  static void	EditFile(const String& filename); 
  // CATT_Dialog edit the file in the external editor

  /////////////////////////////////////////////////
  //	Global state management

  static void	FlushConsole();
  // #CAT_GlobalState flush any pending console output (cout, cerr) -- call this in situations that generate a lot of console output..

  static void 	Busy(bool busy = true);
  // #CAT_GlobalState puts system in a 'busy' state
  static inline void	DoneBusy() {Busy(false);}
  // #CAT_GlobalState when no longer busy, call this function

  static void 	CheckConfigStart(bool confirm_success = true, bool quiet = false); 
  // #CAT_GlobalState we are starting checkconfig, nestable, 1st guy controls params
  static void	CheckConfigEnd(bool ok = true);
  // #CAT_GlobalState ending checkconfig, last exit handles display etc.

  static void	MallocInfo(ostream& strm);
  // #CAT_GlobalState generate malloc memory statistic information to given stream
  static void	ListAllTokens(ostream& strm);
  // #CAT_GlobalState generate a list and count of all types that keep tokens, with a count of tokens
  static TypeInfoKind TypeToTypeInfoKind(TypeDef* typ);
  // #CAT_GlobalState 
  
#ifndef __MAKETA__
  static void	Register_Cleanup(SIGNAL_PROC_FUN_ARG(fun));
  // #IGNORE register a cleanup process in response to all terminal signals
#endif
  static void	Decode_Signal(int err);
  // #IGNORE printout translation of signal on cerr

  /////////////////////////////////////////////////
  //	Startup/Args

  static void	Initialize();
  // #IGNORE very first initialize of type system prior to loading _TA.cpp information (called by ta_TA.cpp file -- hardcoded into maketa

  static void	AddInitHook(init_proc_t init_proc);
  // #IGNORE add an init hook -- invoked by InitProccalled during module initialization, before main()

#ifndef NO_TA_BASE
  static void 	Init_Hooks();
  // #IGNORE calls initialization hooks for plugins
  static void 	Init_Defaults_PreLoadConfig();
  // #IGNORE sets up default parameters for taMisc settings, prior to loading from config file
  static void 	Init_Defaults_PostLoadConfig();
  // #IGNORE sets up default parameters for taMisc settings, after loading from config file
  static void	Init_Args(int argc, const char* argv[]);
  // #IGNORE initialize taMisc startup argument information (note: arg_names must be initialized prior to this!)
  static void	Init_Types();
  // #IGNORE called after all type info has been loaded into types -- calls initClass methods on classes that have them (and possibly other type post-init info)
  static void	Init_DMem(int& argc, const char* argv[]);
  // #IGNORE initialize distributed memory stuff

  static void	HelpMsg(ostream& strm = cerr);
  // #CAT_Args generate a help message about program args, usage, etc

  static void	AddArgName(const String& flag, const String& name);
  // #CAT_Args add an argument flag name to be processed from startup args (e.g., flag = -f, name = CssScript; see arg_names)
  static void	AddArgNameDesc(const String& name, const String& desc);
  // #CAT_Args add a description of an argument flag name (see arg_name_descs)

  static void	UpdateArgs();
  // #CAT_Args update arg information after adding new arg names

  static bool	CheckArgByName(const String& nm);
  // #CAT_Args was the given arg name set?
  static String	FindArgByName(const String& nm);
  // #CAT_Args get the value for given named argument (argv[x] for unnamed args)
  static bool	GetAllArgsNamed(const String& nm, String_PArray& vals);
  // #CAT_Args get the values for all args with given name tag
  static bool	CheckArgValContains(const String& vl);
  // #CAT_Args check if there is an arg that contains string fragment in its value
  static String	FindArgValContains(const String& vl);
  // #CAT_Args get full arg value that contains string fragment
  static void	AddUserDataSchema(const String& type_name, UserDataItemBase* item); 
  // #CAT_UserData adds the item as schema, putting on deferred list if type not avail yet
  static void	AddDeferredUserDataSchema(); // #IGNORE call during init to resolve
protected:
  static String_PArray* deferred_schema_names; // for early startup if type not defined yet
  static UserDataItem_List* deferred_schema_items; // for early startup if type not defined yet
public:
#endif // NO_TA_BASE

  /////////////////////////////////////////////////
  //	Commonly used utility functions on strings/arrays/values

  static void	CharToStrArray(String_PArray& sa, const char* ch);
  // #CAT_Utility convert space-delimeted character string to a string array
  static String	StrArrayToChar(const String_PArray& sa);
  // #CAT_Utility convert a string array to a space-delimeted character string

  static void	SpaceLabel(String& lbl);
  // #CAT_Utility add spaces to a label in place of _'s and upper-lower transitions

  static String	LeadingZeros(int num, int len);
  // #CAT_Utility returns num converted to a string with leading zeros up to len

  static String	FormatValue(float val, int width, int precision);
  // #CAT_Utility format output of value according to width and precision

  static String	StringMaxLen(const String& str, int len);
  // #CAT_Utility returns string up to maximum length given (enforces string to be len or less in length)
  static String	StringEnforceLen(const String& str, int len);
  // #CAT_Utility returns string enforced to given length (spaces added to make length)

  static String	StringCVar(const String& str);
  // #CAT_Utility make return string in a form that would be valid as a variable name in C (i.e., alpha + numeric (not at start) + _

  
  /////////////////////////////////////////////////
  //	File Paths etc

  static String GetFileFmPath(const String& path);
  // #CAT_File get file name component from full path
  static String GetDirFmPath(const String& path, int n_up = 0);
  // #CAT_File get directory component from full path, n_up is number of directories to go up from the final directory
  static String GetHomePath();
  // #CAT_File get user's home directory path
#ifndef NO_TA_BASE
  static String GetCurrentPath();
  // #CAT_File get current working directory path
  static bool 	SetCurrentPath(const String& path);
  // #CAT_File set current working directory to given path; returns success
#endif
  static String	FindFileOnPath(String_PArray& paths, const char* fname);
  // #CAT_File helper function: try to find file fnm in one of the load_include paths -- returns complete path to file (or empty str if not found)

  static String	FindFileOnLoadPath(const char* fname);
  // #CAT_File try to find file fnm in one of the load_include paths -- returns complete path to file  (or empty str if not found)

  static int 	GetUniqueFileNumber(int st_no, const String& prefix, const String& suffix);
  // get a unique file number by adding numbers in between prefix and suffix until such a file does not exist

  /////////////////////////////////////////////////
  //	Recording GUI actions to css script

  static void	StartRecording(ostream* strm);
  // #CAT_Script sets record_strm and record_cursor
  static void   StopRecording();
  // #CAT_Script unsets record_strm and record_cursor
  static bool	RecordScript(const char* cmd);
  // #CAT_Script record the given script command, if the script is open (just sends cmd to stream)
#ifndef NO_TA_BASE
  static void   ScriptRecordAssignment(taBase* tab,MemberDef* md);
  // #CAT_Script record last script assignment of tab's md value;
  static void 	SRIAssignment(taBase* tab,MemberDef* md);
  // #CAT_Script record inline md assignment
  static void 	SREAssignment(taBase* tab,MemberDef* md);
  // #CAT_Script record enum md assignment
#endif

  ////////////////////////////////////////////////////////////////////////
  // 	File Parsing Stuff for Dump routines: Input

  static String	LexBuf;	// #HIDDEN a buffer, contains last thing read by read_ funs

  // return value is the next character in the stream
  // peek=true means that return value was not read, but was just peek'd

  static int	skip_white(istream& strm, bool peek = false);
  // #CAT_Parse skip over all whitespace
  static int	skip_white_noeol(istream& strm, bool peek = false);
  // #CAT_Parse don't skip end-of-line
  static int	skip_till_start_quote_or_semi(istream& strm, bool peek = false);      
  // #CAT_Parse used to seek up to an opening " for a string; will terminate on a ;
  static int	read_word(istream& strm, bool peek = false);
  // #CAT_Parse reads only contiguous 'isalnum' and _ -- does skip_white first
  static int	read_nonwhite(istream& strm, bool peek = false);
  // #CAT_Parse read any contiguous non-whitespace string -- does skip_white first
  static int    read_nonwhite_noeol(istream& strm, bool peek = false);
  // #CAT_Parse read any contiguous non-whitespace string, does skip_white_noeol first (string must be on this line)
  static int	read_till_eol(istream& strm, bool peek = false);
  // #CAT_Parse eol = end of line
  static int	read_till_semi(istream& strm, bool peek = false);
  // #CAT_Parse semi = ;
  static int	read_till_lbracket(istream& strm, bool peek = false);
  // #CAT_Parse lbracket = {
  static int	read_till_lb_or_semi(istream& strm, bool peek = false);
  // #CAT_Parse lb = { or ;
  static int	read_till_rbracket(istream& strm, bool peek = false);
  // #CAT_Parse rbracket = } -- does depth counting to skip over intervening paired { }
  static int	read_till_rb_or_semi(istream& strm, bool peek = false);
  // #CAT_Parse rbracket } or ; -- does depth counting to skip over intervening paired { }
  static int 	read_till_end_quote(istream& strm, bool peek = false); // #CAT_Parse 
  // #CAT_Parse read-counterpart to write_quoted_string; read-escaping, until "
  static int	read_till_end_quote_semi(istream& strm, bool peek = false); 
  // #CAT_Parse read-counterpart to write_quoted_string; read-escaping, until "; (can be ws btwn " and ;)
  static int	skip_past_err(istream& strm, bool peek = false);
  // #CAT_Parse skips to next rb or semi (robust)
  static int	skip_past_err_rb(istream& strm, bool peek = false);
  // #CAT_Parse skips to next rbracket (robust)

  static int	replace_strings(istream& istrm, ostream& ostrm, NameVar_PArray& repl_list);
  // #CAT_File replace a list of strings (no regexp) in input file istrm to output file ostrm (name -> value) -- reads one line at a time; returns number replaced
  static int  	find_strings(istream& istrm, String_PArray& strs);
  // #CAT_File find first occurrence of any of the given strings in file (reading one line at a time); returns index of string or -1 if none found

  ////////////////////////////////////////////////////////////////////////
  //	HTML-style tags

  enum ReadTagStatus {
    TAG_GOT,			// got a starting tag <xxx...>
    TAG_END,			// got an ending tag </xx> 
    TAG_NONE,			// no start of < tag there
    TAG_EOF,			// got an EOF
  };

  static ReadTagStatus read_tag(istream& strm, String& tag, String& val);
  // #CAT_Parse read an html-style tag from the file: <XXX ...> tag = XXX, val = ... (optional)
  static int	read_till_rangle(istream& strm, bool peek = false);
  // #CAT_Parse rangle = >
  
  ////////////////////////////////////////////////////////////////////////
  // 	File Parsing Stuff for Dump routines: Output

  static ostream& indent(ostream& strm, int indent, int tsp=2);
  // #CAT_File
  static ostream& write_quoted_string(ostream& strm, const String& str, 
				      bool write_if_empty = false);
  // #CAT_File writes the string, including enclosing quotes, escaping so we can read back using read_till_end_quote funcs
  static ostream& fmt_sep(ostream& strm, const String& itm, int no, int indent,
			  int tsp=2);
  // #CAT_File
  static ostream& fancy_list(ostream& strm, const String& itm, int no, int prln,
			     int tabs);
  // #CAT_File 
};

//////////////////////////////////////////////////////////
// 	taRefN

class TA_API taRefN {
  // #NO_TOKENS #NO_MEMBERS #NO_CSS reference counting base class
public:
  static uint		RefN(taRefN* it)	{ return it->refn; }
  static void  		Ref(taRefN* it)	{ it->refn++; }
  static void  		Ref(taRefN& it)	{ it.refn++; }
  static void   	unRef(taRefN* it)	{ it->refn--; }
  static void   	Done(taRefN* it)	{ if(it->refn == 0) delete it; }
  static void		unRefDone(taRefN* it)	{ unRef(it); Done(it); }
  static void		SetRefDone(taRefN*& var, taRefN* it) // does it fast, but safe, even for var==it
    {if (it) Ref(it); if (var != NULL) unRefDone(var); var = it;}

  taRefN()		{ refn = 0; }
protected:
  mutable uint 		refn;
  virtual ~taRefN()	{ }; // all instances should consistently use ref counting for lifetime management
};


// Interfaces

//////////////////////////
//   ITypedObject	//
//////////////////////////

// Contains basic virtual methods for getting type information, and an instance pointer
class TA_API ITypedObject {//#NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE
  // this is the interface available to the host data object
public:
  virtual void*		This() = 0; // reference to the 'this' pointer of the client object
  virtual TypeDef*	GetTypeDef() const = 0; // typedef of the dlc
  virtual ~ITypedObject() {}
};

#define TYPED_OBJECT(T) \
  void* This() {return this;} \
  TypeDef* GetTypeDef() const {return &TA_##T;}
  
//////////////////////////
//   IDataLinkProxy	//
//////////////////////////

class TA_API IDataLinkProxy : public virtual ITypedObject {//#NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE
public:
#ifndef TA_NO_GUI
  virtual taiDataLink*	link() const = 0;
#else
  virtual taDataLink*	link() const = 0;
#endif
  virtual TypeDef*	GetDataTypeDef() const; // convenience function, default gets data type from link
};

//////////////////////////
//   IDataLinkClient	//
//////////////////////////

/* ****WARNING****
  Do NOT put logging (ie calls to taMisc::Warning, etc.) in any data link mgt
  code because it can get routed through the pager or other buffering mechanism
  which may trigger event loop calls, which can cause deletions, mid-routine
*/

// Mixin interface for Node that uses datalinks, ex. tree node or Inventor node

class TA_API IDataLinkClient : public virtual IDataLinkProxy {//#NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE
  // this is the interface available to the host data object
friend class taDataLink;
public:
#ifndef TA_NO_GUI
  inline taiDataLink*	link() const {return (taiDataLink*)m_link;}
#else
  inline taDataLink*	link() const {return m_link;}
#endif
  inline taDataLink*	link_() const {return m_link;}
  virtual bool		ignoreDataChanged() const {return false;}
    // hidden guys can ignore changes (but they always get Destroyed)
  virtual bool		isDataView() const {return false;} // true for dataviews
  virtual void		DataLinkDestroying(taDataLink* dl) = 0; // called by DataLink when destroying; it will remove datalink ref in dlc upon return
  virtual void		DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) = 0; //
  virtual void		IgnoredDataChanged(taDataLink* dl, int dcr,
    void* op1, void* op2) {} // called instead if ignoreDataChanged was true
  
  IDataLinkClient() {m_link = NULL;}
  ~IDataLinkClient();
protected:
  virtual bool		AddDataLink(taDataLink* dl);
    // #IGNORE true if added, false if already set (usually a bug); overridden in Multi
  virtual bool		RemoveDataLink(taDataLink* dl);
    // #IGNORE true if removed, false if not (likely not a bug, just redunancy) overridden in Multi
  
  taDataLink*		m_link; // NOTE: will always be a taiDataLink or subclass
};


class TA_API IMultiDataLinkClient : public virtual IDataLinkClient {//#NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE
public:
  
  IMultiDataLinkClient() {}
  ~IMultiDataLinkClient();
protected:
  taPtrList<taDataLink> dls; 
  override bool		AddDataLink(taDataLink* dl);
  override bool		RemoveDataLink(taDataLink* dl);
};



//////////////////////////
//   taDataLink		//
//////////////////////////

/*
 * taDataLink is the interface between data objects, and the object viewing system.

 * One DataLink object is created for each data object that has active viewers.
 *
 * Any type of object can contain a taDataLink delegate -- it passes the reference to the
 * taDataLink within itself to the datalink when it creates it.
 * A DataLink will only remain alive while there are clients viewing it -- when the last
 * client removes itself, the datalink will destruct. When a DataLink object destructs,
 * it will remove itself from its data item.
 *
 * A DataLink object of the correct type for a data item is created by the taiViewType
 * object for the type. Note that the base type for all instances is taiDataLink.

 * There are two destruction scenarios, as outlined below:
 * 1) The data object is deleted (ex., user deletes from a menu item)
 *    - data will call taDataLink::DataDestroying() for each DataLink in its list
 *    - each datalink will IDataLinkClient::DataLinkDestroying for each IDataLinkClient in
 *      its clients list
 *    - if DataLink has a panel, it will:
 *       call the Browser's RemovePanel(); clear its Panel ref
 *    - (additional Viewer-dependent deleting, ex. in a tree view)
 *    - taBase will remove DataLink from the list, destroying it
 * 2) Dataclient object is deleted (ex. user closes its viewer)
 *    - CO will call IDataLink::RemoveDataClient()
 *    - if DataLink has no more COs, it will destroy itself per (1) above
 *
 * NOTE: this is an abstract type -- most of the rest of the system uses taiDataLink
 * (see ta_qtviewer.h).

*/

#define DL_FUNS(y) 	TypeDef* GetTypeDef() const {return &TA_ ## y;} \


class TA_API IDataLinkClient_PtrList: public taPtrList<IDataLinkClient> {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
public:
    ~IDataLinkClient_PtrList() {} // MAKETA requires at least one member item
};

class TA_API taDataLink {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS #VIRT_BASE interface for taiDataLink and descendants
friend class taDataLinkItr;
public:
  void*			data() const {return m_data;} // subclasses usually replace with strongly typed version
#ifndef NO_TA_BASE
  taBase*		taData() const 
    {if (isBase()) return (taBase*)m_data; else return NULL;}
#endif
  virtual bool		isBase() const {return false;} // true if data is of type taBase (note: could still be null)
  inline int		dbuCnt() const {return m_dbu_cnt;} // batch update: -ve:data, 0:none, +ve:struct
  virtual bool		isEnabled() const {return true;} // status of item
  
  bool			AddDataClient(IDataLinkClient* dlc); // true if added, and it had not previously been added (false is probably a bug)
  bool			RemoveDataClient(IDataLinkClient* dlc); // returns true if removed; false is likely not a bug, just redundancy

  virtual TypeDef*	GetDataTypeDef() const {return NULL;} // TypeDef of the data
  virtual MemberDef*	GetDataMemberDef() const {return NULL;} // if a member in a class, then the MemberDef
  virtual String	GetName() const {return _nilString;}
  virtual String	GetDisplayName() const; // default return Member name if has MemberDef, else GetName
  void			DataDestroying(); // called by host when destroying, but it is still responsible for deleting us
  virtual void		DataDataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
//na  virtual void		FillContextMenu(BrListViewItem* sender, taiMenu* menu);
//na  virtual void		FillContextMenu_EditItems(BrListViewItem* sender, taiMenu* menu, int allowed) {}
  virtual bool		HasChildItems() {return false;} // used when node first created, to control whether we put a + expansion on it or not

  virtual int		NumListCols() const {return 1;} // number of columns in a list view for this item type
  static const KeyString key_name; // "name" note: also on taBase 
  virtual const KeyString GetListColKey(int col) const {return key_name;} // key of default list view
  virtual String	GetColHeading(const KeyString& key) const {return KeyString("Item");} 
    // header text for the indicated column
  virtual String	GetColText(const KeyString& key, int itm_idx = -1) const 
    {return GetName();} // text for the indicated column
  virtual String	ChildGetColText(taDataLink* child, const KeyString& key, 
    int itm_idx = -1) const  {return child->GetColText(key, itm_idx);}
    // default delegates to child; lists can override to control this

  virtual TypeDef* 	GetTypeDef() const;
  taDataLink(void* data_, taDataLink* &link_ref_);
  virtual ~taDataLink(); 
protected:
  void*			m_data; // subclasses usually replace with strongly typed version
  taDataLink**		m_link_ref; // #IGNORE address of our reference in the data item
  IDataLinkClient_PtrList clients; // clients of this item (ex. either primary, or where it is aliased or linked)
  int			m_dbu_cnt; // data batch update count; +ve is Structural, -ve is Parameteric only
private:
  void 			DoNotify(int dcr, void* op1_, void* op2_);
    // don't even DREAM of making this non-private!!!!
};

// macros for iterating over the dataclients or views of a data item

class TA_API taDataLinkItr {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS iterator for datalink clients -- use as a value type
public:
  void* 	NextEl(taDataLink* dl, const TypeDef* typ);
  void		Reset() {i = 0;}
  taDataLinkItr() {Reset();}
protected:
  int 		i;
};

// iterates through a datalink, returning only object refs to objects of indicated type (or descendant)
#define FOR_DLC_EL_OF_TYPE(T, el, dl, itr) \
for(itr.Reset(), el = (T*) itr.NextEl(dl, &TA_ ## T); el; el = (T*) itr.NextEl(dl, &TA_ ## T))


//////////////////////////
//   EnumSpace		//
//////////////////////////

class TA_API EnumSpace : public taPtrList<EnumDef> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS space of enums
protected:
  String	GetListName_() const		{ return name; }
  String	El_GetName_(void* it) const;
  TALPtr 	El_GetOwnerList_(void* it) const;
  void*		El_SetOwner_(void* it);
  void		El_SetIndex_(void* it, int i);

  void*		El_Ref_(void* it);
  void* 	El_unRef_(void* it);
  void		El_Done_(void* it);
  void*		El_MakeToken_(void* it);
  void*		El_Copy_(void* trg, void* src);

public:
  String 	name;		// of the space
  TypeDef*	owner;		// owner is a typedef
  taDataLink*	data_link;
  
  void		Initialize()	{ owner = NULL; data_link = NULL;}
  EnumSpace()			{ Initialize(); }
  EnumSpace(const EnumSpace& cp) { Initialize(); Borrow(cp); }
  ~EnumSpace();

  void operator=(const EnumSpace& cp)	{ Borrow(cp); }

  // adding manages the values of the enum-values
  void			Add(EnumDef* it);
  virtual EnumDef*	Add(const char* nm, const char* dsc="", const char* op="",
			    int eno=0);

  virtual EnumDef*	FindNo(int eno) const;
  // finds for a given enum_no

  virtual ostream&   	OutputType(ostream& strm, int indent = 1) const;
};


//////////////////////////
//   TokenSpace		//
//////////////////////////

class TA_API TokenSpace : public taPtrList<void> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
protected:
  String	GetListName_() const		{ return name; }
  String	El_GetName_(void* it) const;

public:
  static String tmp_el_name;	// for element names that need to be created

  String 	name;		// of the space
  TypeDef*	owner;		// owner is a typedef
  bool		keep;		// true if tokens are kept
  int		sub_tokens;	// number of tokens in sub-types
  taDataLink*	data_link;

  virtual void 	Initialize();
  TokenSpace()				{ Initialize(); }
  TokenSpace(const TokenSpace& cp)	{ Initialize(); Borrow(cp); }
  ~TokenSpace();
  void operator=(const TokenSpace& cp)	{ Borrow(cp); }

  void 		List(ostream& strm=cout) const;
};


//////////////////////////
//   MemberSpace	//
//////////////////////////

class TA_API Member_List: public taPtrList<MemberDef> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS simple list of members
public:
  Member_List() {}
};

class TA_API MemberDefBase_List: public taPtrList<MemberDefBase> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS common subtype for Member and PropertySpace
public:
  String 	name;		// of the space
  TypeDef*	owner;		// owner is a typedef
  taDataLink*	data_link;

  MemberDefBase_List() {Initialize();}
  ~MemberDefBase_List();
protected:
  String	GetListName_() const		{ return name; }
  String 	El_GetName_(void* it) const;
  TALPtr 	El_GetOwnerList_(void* it) const;
  void*		El_SetOwner_(void* it);
  void		El_SetIndex_(void* it, int i);

  void*		El_Ref_(void* it);
  void* 	El_unRef_(void* it);
  void		El_Done_(void* it);
  void*		El_MakeToken_(void* it); // makes a proper token of the concrete type
  void*		El_Copy_(void* trg, void* src); // note: copies like guys correctly
  
private:
  void		Initialize()		{ owner = NULL; data_link = NULL;}
};

class TA_API MemberSpace: public MemberDefBase_List {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS space of members
INHERITED(MemberDefBase_List)
public:
  MemberSpace()				{  }
  MemberSpace(const MemberSpace& cp)	{ Borrow(cp); }

  void operator=(const MemberSpace& cp)	{ Borrow(cp); }

  MemberDef*	FindCheck(const char* nm, void* base, void*& ptr) const;
  // breadth-first find pass for the recursive procedures

  int			FindNameOrType(const char* nm) const;
  // checks name and type name in 2 passes
  int		FindTypeName(const char* nm) const;
  // find by name of type
  MemberDef*	FindNameR(const char* nm) const;
  // recursive find of name (or type name)
  MemberDef* 	FindNameAddr(const char* nm, void* base, void*& ptr) const;
  // find of name returning address of found member
  MemberDef*	FindNameAddrR(const char* nm, void* base, void*& ptr) const;
  // recursive find of name returning address of found member

  MemberDef*	FindType(TypeDef* it, int& idx=no_index) const;
  // find by type, inherits from
  MemberDef*	FindTypeR(TypeDef* it) const;
  // recursive find of type
  MemberDef* 	FindTypeAddr(TypeDef* it, void* base, void*& ptr) const;
  // find of type returning address of found member
  MemberDef*	FindTypeAddrR(TypeDef* it, void* base, void*& ptr) const;
  // recursive find of type returning address of found member

  int		FindDerives(TypeDef* it) const;
  MemberDef*	FindTypeDerives(TypeDef* it,  int& idx=no_index) const;
  // find by type, derives from

  MemberDef*	FindAddr(void* base, void* mbr, int& idx=no_index) const;
  // find by address given base of class and address of member
  int		FindPtr(void* base, void* mbr) const;
  MemberDef*	FindAddrPtr(void* base, void* mbr, int& idx=no_index) const;
  // find by address of a member that is a pointer given base and pointer addr

  void		CopyFromSameType(void* trg_base, void* src_base);
  // copy all members from class of the same type as me
  void		CopyOnlySameType(void* trg_base, void* src_base);
  // copy only those members in my type (no inherited ones)

  bool		CompareSameType(Member_List& mds, void_PArray& trg_bases,
					void_PArray& src_bases, 
					void* trg_base, void* src_base,
					int show_forbidden = taMisc::NO_HIDDEN,
					int show_allowed = taMisc::SHOW_CHECK_MASK,
					bool no_ptrs = true, bool test_only = false);
  // compare all member values from class of the same type as me, adding ones that are different to the mds, trg_bases, src_bases lists (unless test_only == true, in which case it just does the tests and returns true if any diffs -- for inline objects)

  // IO
  ostream&   	OutputType(ostream& strm, int indent = 0) const;

  ostream&   	Output(ostream& strm, void* base, int indent) const;
  ostream&   	OutputR(ostream& strm, void* base, int indent) const;

  // for dump files
  int   	Dump_Save(ostream& strm, void* base, void* par, int indent);
  int   	Dump_SaveR(ostream& strm, void* base, void* par, int indent);
  int   	Dump_Save_PathR(ostream& strm, void* base, void* par, int indent);

  int   	Dump_Load(istream& strm, void* base, void* par,
				  const char* prv_read_nm = NULL, int prv_c = 0); //

public: // lexical hacks
  inline MemberDef*	operator[](int i) const {return (MemberDef*)inherited::FastEl(i);}
  inline MemberDef*	FastEl(int i) const {return (MemberDef*)inherited::FastEl(i);}
  inline MemberDef*	SafeEl(int i) {return (MemberDef*)inherited::SafeEl(i);}
  inline MemberDef*	FindName(const String& item_nm, int& idx=no_index) const
    {return (MemberDef*)inherited::FindName(item_nm, idx);}
};


//////////////////////////
//   PropertySpace	//
//////////////////////////

class TA_API PropertySpace: public MemberDefBase_List {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS space of properties -- note: will hold PropertyDef (owned) and MemberDef (linked) objects
INHERITED(MemberDefBase_List)
public:  
  PropertySpace()				{ }
  PropertySpace(const PropertySpace& cp)	{ Borrow(cp); }

  void operator=(const PropertySpace& cp)	{ Borrow(cp); }

  int			FindNameOrType(const char* nm) const;
  // checks name and type name in 2 passes
  int			FindTypeName(const char* nm) const;
  // find by name of type
  MemberDefBase*		FindNameR(const char* nm) const;
  // recursive find of name (or type name)
#ifdef NO_TA_BASE  
  PropertyDef*		AssertProperty(const char* nm, bool& is_new,
    MemberDef* get_mbr = NULL, MemberDef* set_mbr = NULL,
    MethodDef* get_mth = NULL, MethodDef* set_mth = NULL);
  // assert property, optionally supplying one or more getter/setter 
#endif
};


//////////////////////////
//   MethodSpace	//
//////////////////////////

class TA_API Method_List: public taPtrList<MethodDef> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS simple list of methods
public:
  Method_List() {}
};

class TA_API MethodSpace: public Method_List {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS space of methods
INHERITED(Method_List)
protected:
  String	GetListName_() const		{ return name; }
  String El_GetName_(void* it) const;
  TALPtr 	El_GetOwnerList_(void* it) const;
  void*		El_SetOwner_(void* it);
  void		El_SetIndex_(void* it, int i);

  void*		El_Ref_(void* it);
  void* 	El_unRef_(void* it);
  void		El_Done_(void* it);
  void*		El_MakeToken_(void* it);
  void*		El_Copy_(void* trg, void* src);

public:
  String 	name;		// of the space
  TypeDef*	owner;		// owner is a typedef
  taDataLink*	data_link;

  void		Initialize()		{ owner = NULL; data_link = NULL;}
  MethodSpace()				{ Initialize(); }
  MethodSpace(const MethodSpace& cp)	{ Initialize(); Borrow(cp); }
  ~MethodSpace();

  void operator=(const MethodSpace& cp)	{ Borrow(cp); }

  bool		AddUniqNameNew(MethodDef* it);

  MethodDef*	FindAddr(ta_void_fun funa, int& idx) const;
  // find fun by addr, idx is actual index in method space
  MethodDef*	FindOnListAddr(ta_void_fun funa, const String_PArray& lst, int& lidx) const;
  // find fun on given list by addr, lidx is 'index' of funs on same list
  MethodDef*	FindOnListIdx(int lidx, const String_PArray& lst) const;
  // find fun on given list by index, as given by FindOnListAddr()
  MethodDef*	FindVirtualBase(MethodDef* it, int& idx);
    // find the virtual method with same name and signature

  // IO
  virtual ostream&   	OutputType(ostream& strm, int indent = 0) const;
  
};


//////////////////////////
//   TypeSpace		//
//////////////////////////

class TA_API TypeSpace: public taPtrList<TypeDef> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS space of types; uses default string-based hashing
protected:
  String	GetListName_() const 		{ return name; }
  String	El_GetName_(void* it) const;
  TALPtr 	El_GetOwnerList_(void* it) const;
  void*		El_SetOwner_(void* it);
  void		El_SetIndex_(void* it, int i);

  void*		El_Ref_(void* it);
  void* 	El_unRef_(void* it);
  void		El_Done_(void* it);
  void*		El_MakeToken_(void* it);
  void*		El_Copy_(void* trg, void* src);

public:
  String 	name;		// of the space
  TypeDef*	owner;		// owner is a typedef
  taDataLink*	data_link;

  void		Initialize()		{ owner = NULL; data_link = NULL;}

  TypeSpace()				{ Initialize(); }
  TypeSpace(const char* nm)		{ Initialize(); name = nm; }
  TypeSpace(const char* nm, int hash_sz) { Initialize(); name = nm; BuildHashTable(hash_sz); }
  TypeSpace(const TypeSpace& cp)	{ Initialize(); Borrow(cp); }
  ~TypeSpace();

  void operator=(const TypeSpace& cp)	{ Borrow(cp); }

  TypeDef*	FindTypeR(const String& fqname) const; // find a possibly nested subtype based on :: name
  virtual bool	ReplaceLinkAll(TypeDef* ol, TypeDef* nw);
  virtual bool 	ReplaceParents(const TypeSpace& ol, const TypeSpace& nw);
  // replace any parents on the old list with those on the new for all types

  virtual void	ListAllTokens(ostream& strm);
  // list count for all types that are keeping tokens
};


//////////////////////////
//   TypeItem		//
//////////////////////////

class TA_API TypeItem: public taRefN, public ITypedObject {
  // ##INSTANCE ##NO_TOKENS ##NO_MEMBERS ##NO_CSS ##MEMB_NO_SHOW_TREE base class for TypeDef, MemberDef, MethodDef, EnumDef, and TypedefDef
INHERITED(taRefN)
public:
  enum ShowContext {
    SC_ANY,		// any context -- directives like "SHOW"
    SC_EDIT,		// for in edit dialogs -- directives like "SHOW_EDIT"
    SC_TREE		// in tree views (browsing) -- directives like "SHOW_TREE"
  }; //
  
// const guys help speed up oft-looked up values by avoiding String churning
  static const String opt_show; // "SHOW"
  static const String opt_no_show; // "NO_SHOW"
  static const String opt_hidden; // "HIDDEN"
  static const String opt_read_only; // "READ_ONLY"
  static const String opt_expert; // "EXPERT"
  static const String opt_edit_show; // "EDIT_SHOW"
  static const String opt_edit_no_show; // "EDIT_NO_SHOW"
  static const String opt_edit_hidden; // "EDIT_HIDDEN"
  static const String opt_edit_read_only; // "EDIT_READ_ONLY"
  static const String opt_edit_detail; // "EDIT_DETAIL"
  static const String opt_edit_expert; // "EDIT_EXPERT"
  static const String opt_APPLY_IMMED; // "APPLY_IMMED"
  static const String opt_inline; // "INLINE"
  static const String opt_edit_inline; // "EDIT_INLINE"
  static const String opt_EDIT_DIALOG; // "EDIT_DIALOG"
  
  static const String opt_bits; // "BITS"
  static const String opt_instance; // "INSTANCE"
  
  int		idx;		// the index number for this type
  String	name;
  String	desc;		// a description
  String_PArray	opts;		// user-spec'd options (#xxx)
  String_PArray	lists;		// user-spec'd lists   (#LIST_xxx)
  taDataLink*	data_link;

  override void*	This() {return this;} 
  override TypeDef*	GetTypeDef() const {return &TA_void;}
  virtual taMisc::TypeInfoKind typeInfoKind() const {return taMisc::TIK_UNKNOWN;}
  virtual TypeDef* 	GetOwnerType() const {return NULL;}
  virtual const String	GetPathName() const {return name;} 
    // name used for saving a reference in stream files, can be used to lookup again
    
  void		Copy(const TypeItem& cp);
  bool		HasOption(const String& op) const { return (opts.FindEl(op) >= 0); }
    // check if option is set
  bool		HasOptionAfter(const String& prefix, const String& op) const;
    // returns true if any prefix (Xxx_) has the value after of op; enables multi options of same prefix
  virtual String	OptionAfter(const String& prefix) const;
  // return portion of option after given option header
  virtual bool		NextOptionAfter(const String& prefix, int& itr, String& result) const; // enables enumeration of multi-valued prefixes; start w/ itr=0; true if a new value was returned in result
  virtual String	GetLabel() const;
  // checks for option of LABEL_xxx and returns it or name
  
  TypeItem();
  TypeItem(const TypeItem& cp); // copy constructor
  ~TypeItem();

private:
  void		init(); // #IGNORE
  void		Copy_(const TypeItem& cp);
};

class TA_API EnumDef : public TypeItem { //  defines an enum member
INHERITED(TypeItem)
public:
  EnumSpace*	owner;		// the owner of this one

  int		enum_no;	// number (value) of the enum


  override void*	This() {return this;} 
  override TypeDef*	GetTypeDef() const {return &TA_EnumDef;}
  taMisc::TypeInfoKind typeInfoKind() const {return taMisc::TIK_ENUM;}
  
  void		Copy(const EnumDef& cp);

  EnumDef();
  EnumDef(const char* nm);
  EnumDef(const char* nm, const char* dsc, int eno, const char* op, const char* lis);
  EnumDef(const EnumDef& cp);
  EnumDef*	Clone()		{ return new EnumDef(*this); }
  EnumDef*	MakeToken()	{ return new EnumDef(); }

  TypeDef* 	GetOwnerType() const
  { TypeDef* rval=NULL; if((owner) && (owner->owner)) rval=owner->owner; return rval; }
  bool		CheckList(const String_PArray& lst) const;
  // check if enum has a list in common with given one
private:
  void		Initialize();
  void		Copy_(const EnumDef& cp);
};


class TA_API MemberDefBase : public TypeItem { // #VIRT_BASE #NO_INSTANCE common subclass of MemberDef and PropertyDef
INHERITED(TypeItem)
public:
  MemberDefBase_List*	owner;
  TypeDef*		type;	  // of this item
  String_PArray		inh_opts; // inherited options ##xxx
  bool			is_static; // true if this member is static
#ifdef TA_GUI
  taiMember*		im;		// gui structure for edit representation -- if this is a memberdef that is the storage for a property, then the im is assigned to the property
#endif

  virtual bool		isReadOnly() const = 0; // absolutely read-only
  virtual bool		isGuiReadOnly() const = 0; // read-only in the gui

  override void*	This() {return this;} 
  override TypeDef*	GetTypeDef() const {return &TA_MemberDefBase;}
  virtual bool		ValIsDefault(const void* base, 
    int for_show = taMisc::IS_EXPERT) const = 0; // true if the member contains its default value, either DEF_ or the implicit default; for_show is only for types, to choose which members to recursively include; we are usually only interested in Expert guys
  
  void		Copy(const MemberDefBase& cp);
  void		Copy(const MemberDefBase* cp); // this is a "pseudo-virtual" type guy, that will copy a like source (Member or Property)
  override TypeDef* 	GetOwnerType() const
  { TypeDef* rval=NULL; if((owner) && (owner->owner)) rval=owner->owner; return rval; }
  MemberDefBase();
  MemberDefBase(const char* nm);
  MemberDefBase(TypeDef* ty, const char* nm, const char* dsc, const char* op,
    const char* lis, bool is_stat = false);
  MemberDefBase(const MemberDefBase& cp);
  ~MemberDefBase();

  virtual const Variant	GetValVar(const void* base) const = 0;
  virtual void	SetValVar(const Variant& val, void* base,
    void* par = NULL) = 0;
  
  bool		CheckList(const String_PArray& lst) const;
  // check if member has a list in common with given one

  bool		ShowMember(int show_forbidden = taMisc::USE_SHOW_GUI_DEF,
    TypeItem::ShowContext show_context = TypeItem::SC_ANY,
    int show_allowed = taMisc::SHOW_CHECK_MASK) const;
  // decide whether to output or not based on options (READ_ONLY, HIDDEN, etc)

protected:
  // note: bits in the show* vars are set to indicate the value, ie READ_ONLY has that bit set
  mutable byte	show_any; // bits for show any -- 0 indicates not determined yet, 0x80 is flag
  mutable byte	show_edit;
  mutable byte	show_tree;

  void		ShowMember_CalcCache() const; // called when show_any=0, ie, not configured yet
  void		ShowMember_CalcCache_impl(byte& show, const String& suff) const;
private:
  void 		Initialize();
  void		Copy_(const MemberDefBase& cp);
};


class TA_API MemberDef : public MemberDefBase { //  defines a class member
INHERITED(MemberDefBase)
public:

  ta_memb_ptr	off;		// offset of member from owner type
  int		base_off;	// offset for base of owner
  void*         addr;		// address of static member
  bool		fun_ptr;	// true if this is a pointer to a function

  override bool		isReadOnly() const;
  override bool		isGuiReadOnly() const;
  
  override void*	This() {return this;} 
  override TypeDef*	GetTypeDef() const {return &TA_MemberDef;}
  taMisc::TypeInfoKind typeInfoKind() const {return taMisc::TIK_MEMBER;}
  
  override bool	ValIsDefault(const void* base, 
    int for_show = taMisc::IS_EXPERT) const; // true if the member contains its default value, either DEF_ or the implicit default; for_show is only for types, to choose which members to recursively include; we are usually only interested in Expert guys
  
  void		Copy(const MemberDef& cp);
  MemberDef();
  MemberDef(const char* nm);
  MemberDef(TypeDef* ty, const char* nm, const char* dsc, const char* op, const char* lis,
	    ta_memb_ptr mptr, bool is_stat = false, void* maddr=NULL, bool funp = false);
  MemberDef(const MemberDef& cp);
  ~MemberDef();
  MemberDef*	Clone()		{ return new MemberDef(*this); }
  MemberDef*	MakeToken()	{ return new MemberDef(); }

  void*			GetOff(const void* base) const;
  override const String	GetPathName() const; 
    // name used for saving a reference in stream files, can be used to lookup again

  override const Variant GetValVar(const void* base) const;
  override void	SetValVar(const Variant& val, void* base, void* par = NULL);
    // note: par is only needed really needed for owned taBase ptrs)
  
  void		CopyFromSameType(void* trg_base, void* src_base);
  // copy all members from same type
  void		CopyOnlySameType(void* trg_base, void* src_base);
  // copy only those members from same type (no inherited)
  bool		CompareSameType(Member_List& mds, void_PArray& trg_bases,
				void_PArray& src_bases, 
				void* trg_base, void* src_base,
				int show_forbidden = taMisc::NO_HIDDEN,
				int show_allowed = taMisc::SHOW_CHECK_MASK,
				bool no_ptrs = true, bool test_only = false);
  // compare all member values from class of the same type as me, adding ones that are different to the mds, trg_bases, src_bases lists (unless test_only == true, in which case it just does the tests and returns true if any diffs -- for inline objects)

  ostream&   	OutputType(ostream& strm, int indent = 1) const;

  ostream& 	Output(ostream& strm, void* base, int indent) const;
  ostream& 	OutputR(ostream& strm, void* base, int indent) const;

  // for dump files
  bool		DumpMember(void* par); 		// decide whether to dump or not
  int		Dump_Save(ostream& strm, void* base, void* par, int indent);
  int	 	Dump_SaveR(ostream& strm, void* base, void* par, int indent);
  int	 	Dump_Save_PathR(ostream& strm, void* base, void* par, int indent);

  int	 	Dump_Load(istream& strm, void* base, void* par); //
private:
  void 		Initialize();
  void		Copy_(const MemberDef& cp);
};


class TA_API PropertyDef : public MemberDefBase { //  defines a class member
INHERITED(MemberDefBase)
public:

#ifdef NO_TA_BASE
  MemberDef*		get_mbr; // if a member getter found
  MethodDef*		get_mth; // if a method getter found
  MemberDef*		set_mbr; // if a member setter found
  MethodDef*		set_mth; // if a method setter found
#endif
  ta_prop_get_fun 	prop_get; // stub function to get the property (as Variant)
  ta_prop_set_fun 	prop_set; // stub function to set the property (as Variant)
  
  override bool		isReadOnly() const;
  override bool		isGuiReadOnly() const;
  void			setType(TypeDef* typ); // use this, to check for consistency between the various source -- should NOT be null!
  taMisc::TypeInfoKind typeInfoKind() const {return taMisc::TIK_PROPERTY;}
  
  override void*	This() {return this;} 
  override TypeDef*	GetTypeDef() const {return &TA_PropertyDef;}
  override bool		ValIsDefault(const void* base, 
    int for_show = taMisc::IS_EXPERT) const; // true if the member contains its default value, either DEF_ or the implicit default; for_show is only for types, to choose which members to recursively include; we are usually only interested in Expert guys
  
  void			Copy(const PropertyDef& cp);
  PropertyDef();
  PropertyDef(const char* nm);
  PropertyDef(TypeDef* ty, const char* nm, const char* dsc, const char* op,
    const char* lis, ta_prop_get_fun get, ta_prop_set_fun set,
    bool is_stat = false);
  PropertyDef(const PropertyDef& cp);
  ~PropertyDef();
  PropertyDef*	Clone()		{ return new PropertyDef(*this); }
  PropertyDef*	MakeToken()	{ return new PropertyDef(); }

  override const Variant GetValVar(const void* base) const;
  override void	SetValVar(const Variant& val, void* base, void* par = NULL);
    // note: par is only needed really needed for owned taBase ptrs)
  
private:
  void 		Initialize();
  void		Copy_(const PropertyDef& cp);
};


class TA_API MethodDef : public TypeItem {// defines a class method
#ifndef __MAKETA__
typedef TypeItem inherited;
#endif
public:
  MethodSpace*	owner;

  TypeDef*	type;		// of the return value
  bool		is_static;	// true if this method is static
  bool		is_virtual;	// true if virtual (1st or subsequent overrides)
  bool		is_override;	// true if (virtual) override of a base
  bool		is_lexhide;	// true if lexically hides a non-virtual base -- could be an error
  ta_void_fun   addr;		// address (only for static or reg_fun functions)
  String_PArray	inh_opts;	// inherited options ##xxx
#ifdef TA_GUI
  taiMethod*	im;		// gui structure for edit representation
#endif // def TA_GUI
  short		fun_overld;	// number of times function is overloaded (i.e., diff args)
  short		fun_argc;	// nofun, or # of parameters to the function
  short		fun_argd;	// indx for start of the default args (-1 if none)
  TypeSpace	arg_types;	// argument types
  String_PArray	arg_names;	// argument names
  String_PArray	arg_defs;	// argument default values
  String_PArray	arg_vals;	// argument values (previous)

  css_fun_stub_ptr stubp;	// css function stup pointer

  taMisc::TypeInfoKind typeInfoKind() const {return taMisc::TIK_METHOD;}
  
  const String		prototype() const; // text depiction of fun, ex "void MyFun(int p)"
  override void*	This() {return this;} 
  override TypeDef*	GetTypeDef() const {return &TA_MethodDef;}
  void		Initialize();
  void		Copy(const MethodDef& cp);
  MethodDef();
  MethodDef(const char* nm);
  MethodDef(TypeDef* ty, const char* nm, const char* dsc, const char* op, const char* lis,
	    int fover, int farc, int fard, bool is_stat = false, ta_void_fun funa = NULL,
	    css_fun_stub_ptr stb = NULL, bool is_virt = false);
  MethodDef(const MethodDef& md);	// copy constructor

  MethodDef*		Clone()		{ return new MethodDef(*this); }
  MethodDef*		MakeToken()	{ return new MethodDef(); }
  override TypeDef* 	GetOwnerType() const
    { TypeDef* rval=NULL; if((owner) && (owner->owner)) rval=owner->owner; return rval; }
  override const String	GetPathName() const;
  bool			CheckList(const String_PArray& lst) const;
  // check if method has a list in common with given one
  bool			CompareArgs(MethodDef* it) const;	// true if same, false if not
  ostream&   		OutputType(ostream& strm, int indent = 1) const;
  void			CallFun(void* base) const;
  // call the function, using gui dialog if need to get args
  const String		ParamsAsString() const; // returns what would be in () for a definition
  bool			ShowMethod(taMisc::ShowMembs show = taMisc::USE_SHOW_GUI_DEF) const;
protected:
  mutable byte	show_any; // bits for show any -- 0 indicates not determined yet, 0x80 is flag
  void		ShowMethod_CalcCache() const; // called when show_any=0, ie, not configured yet
  void		ShowMethod_CalcCache_impl(byte& show) const;
};

class taBase_List;

#define IF_ENUM_STRING(enm_var, enm_val) \
((enm_var == enm_val) ? #enm_val : "")


class TA_API TypeDef : public TypeItem {// defines a type itself
INHERITED(TypeItem)
public:
  enum StrContext { // context for getting or setting a string value
    SC_DEFAULT,		// default (for compat) -- if taMisc::is_loading/saving true, then STREAMING else VALUE
    SC_STREAMING,	// value is being used for streaming, ex. strings are quoted/escaped
    SC_VALUE,		// value is being manipulated programmatically, ex. strings are not quoted/escaped
    SC_DISPLAY,		// value is being used for display purposes, ex. float value may be formatted prettily
    SC_CONTEXT_MASK	= 0x00FF, // masks out the value
    SC_FLAG_INLINE	= 0x0100 // forces gen of inline values for classes (ex used by search)
  };
  
  static TypeDef* 	GetCommonSubtype(TypeDef* typ1, TypeDef* typ2); // get the common primary (1st parent class) subtype between the two

#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE) && !defined(__MAKETA__)
  void*		dmem_type; // actually ptr to: MPI_Datatype_PArray
  void 		AssertDMem_Type(); // creates the dmem_type array if not already
  virtual MPI_Datatype 	GetDMemType(int share_set);
#endif
  TypeSpace*	owner;		// the owner of this one

  uint          size;		// size (in bytes) of item
  short		ptr;		// number of pointers
  bool 		ref;		// true if a reference variable
  bool		internal;	// true if an internal type (auto generated)
  bool		formal;		// true if a formal type (e.g. class, const, enum..)
#ifdef NO_TA_BASE
  bool		pre_parsed;	// true if previously parsed by maketa
#else
  bool		is_subclass;	// true if is a class, and inherits from another
  TypeDef*	plugin;		// TypeDef of plugin object, if in a plugin (else NULL) 
  void**	instance;	// pointer to the instance ptr of this type
  taBase_List*	defaults;	// default values registered for this type
  UserDataItem_List* schema;	// default schema (only created if used)
#endif
  String_PArray	inh_opts;	// inherited options (##xxx)

  TypeSpace	parents;	// type(s) this inherits from
  int_PArray	par_off;	// parent offsets
  TypeSpace	par_formal;	// formal parents (e.g. class, const, enum..)
  TypeSpace	par_cache;	// cache of certain parent types for speedup
  TypeSpace	children;	// inherited from this
  TokenSpace	tokens;		// tokens of this type (if kept)

#ifdef TA_GUI
  taiType*	it;		// single glyph representation of type (was 'iv')
  taiEdit*	ie;		// editing window rep. of type (was 'ive')
  taiViewType*	iv;		// browser representation of type
#endif

  // the following only apply to enums or classes
  EnumSpace	enum_vals;	// if type is an enum, these are the labels
  TypeSpace	sub_types;	// sub types scoped within class (incl enums)
  MemberSpace	members;	// member variables for class
  PropertySpace	properties;	// properties for class
  MethodSpace	methods;	// member functions (methods) for class
  String_PArray	ignore_meths;	// methods to be ignored
  TypeSpace	templ_pars;	// template parameters
  String	c_name;		// C name, when diff from name (ex 'unsigned_char' vs 'unsigned char")

  bool		is_enum() const; // true if an enum 
  bool		is_class() const; // true if it is a class
  bool		isVarCompat() const; // true if read/write compatible with Variant
  taMisc::TypeInfoKind typeInfoKind() const {return taMisc::TIK_TYPE;}
  
  override void*	This() {return this;} 
  override TypeDef*	GetTypeDef() const {return &TA_TypeDef;}
  void		Copy(const TypeDef& cp);
  TypeDef();
  TypeDef(const char* nm);
#ifdef NO_TA_BASE
  TypeDef(const char* nm, const char* dsc, const char* inop, const char* op, const char* lis,
	  uint siz, int ptrs=0, bool refnc=false,
	  bool global_obj=false); // global_obj=true for global (non new'ed) typedef objs
#else
  TypeDef(const char* nm, const char* dsc, const char* inop, const char* op, const char* lis,
	  uint siz, void** inst, bool toks=false, int ptrs=0, bool refnc=false,
	  bool global_obj=false); // global_obj=true for global (non new'ed) typedef objs
#endif
  TypeDef(const char* nm, bool intrnl, int ptrs=0, bool refnc=false, bool forml=false,
	  bool global_obj=false, uint siz = 0, const char* c_nm = NULL
	  ); // global_obj=ture for global (non new'ed) typedef objs; c_name only needed when diff from nm
  TypeDef(const TypeDef& td);
  virtual ~TypeDef();
  TypeDef*		Clone()		{ return new TypeDef(*this); }
  TypeDef*		MakeToken()	{ return new TypeDef(); }

  void			CleanupCats(bool save_last);
  // cleanup the #CAT_ category options (allow for derived types to set new categories); if save_last it saves the last cat on the list (for typedef initialization); otherwise saves first (for addparent)

  void			DuplicateMDFrom(const TypeDef* old);
  // duplicates members, methods from given type
  void			UpdateMDTypes(const TypeSpace& ol, const TypeSpace& nw);
  // updates pointers within members, methods to new types from old

  bool			CheckList(const String_PArray& lst) const;
  // check if have a list in common

  override TypeDef* 	GetOwnerType() const
    { if (owner) return owner->owner; else return NULL; }
  TypeDef*		GetParent() const { return parents.SafeEl(0); }
  // gets (first) parent of this type (assumes no multiple inheritance)

  TypeDef* 		GetNonPtrType() const;
  // gets base type (ptr=0) parent of this type
  TypeDef* 		GetPtrType() const;
  // gets child type that is a ptr to this type -- makes one if necessary
  TypeDef* 		GetNonRefType() const;
  // gets base type (not ref) parent of this type
  TypeDef* 		GetNonConstType() const;
  // gets base type (not const) parent of this type
  TypeDef* 		GetNonConstNonRefType() const;
  // gets base type (not const or ref) parent of this type
  TypeDef* 		GetTemplType() const;
  // gets base template parent of this type
  TypeDef* 		GetTemplInstType() const;
  // gets base template instantiation parent of this type
  TypeDef*		GetPluginType() const;
    // if in_plugin, this is the IPlugin-derivitive plugin type
  String 		GetPtrString() const;
  // gets a string of pointer symbols (*) corresponding to the number ptrs
  String		Get_C_Name() const;
  // returns the actual c-code name for this type
  override const String	GetPathName() const;
  
  bool			HasEnumDefs() const; // true if any subtypes are enums
  bool			HasSubTypes() const; // true if any non-enum subtypes

  // you inherit from yourself.  This ensures that you are a "base" class (ptr == 0)
  bool 			InheritsFromName(const char *nm) const
  { bool rval=0; if((ptr == 0) && ((name == nm) || (par_cache.FindName(nm)) || FindParentName(nm))) rval=1; return rval; }
  bool			InheritsFrom(const TypeDef* td) const
  { bool rval=0; if((ptr == 0) && ((this == td) || (par_cache.FindEl(td)>=0) || FindParent(td))) rval=1; return rval; }
  bool 			InheritsFrom(const TypeDef& it) const { return InheritsFrom((TypeDef*)&it); }

  // pointers to a type, etc, can be Derives from a given type (looser than inherits)
  bool 			DerivesFromName(const char *nm) const
  { bool rval=0; if((name == nm) || (par_cache.FindName(nm)) || FindParentName(nm)) rval=1; return rval; }
  bool 			DerivesFrom(TypeDef* td) const
  { bool rval=0; if((this == td) || (par_cache.FindEl(td)>=0) || FindParent(td)) rval=1; return rval; }
  bool 			DerivesFrom(const TypeDef& it) const { return DerivesFrom((TypeDef*)&it); }

  // inheritance from a formal class (e.g. const, static, class)
  bool			InheritsFormal(TypeDef* it) const
  { bool rval=0; if((ptr == 0) && (par_formal.FindEl(it)>=0)) rval=1; return rval; }
  bool 			InheritsFormal(const TypeDef& it) const { return InheritsFormal((TypeDef*)&it); }
  bool			DerivesFormal(TypeDef* it) const
  { bool rval=0; if(par_formal.FindEl(it)>=0) rval=1; return rval; }
  bool 			DerivesFormal(const TypeDef& it) const { return DerivesFormal((TypeDef*)&it); }

  bool			InheritsNonAtomicClass() const; // true *only* for classes that are not considered atoms by the streaming system, i.e. does not include taString and Variant 
  
  TypeDef*	AddParent(TypeDef* it, int p_off=0);
  // adds parent and inherits all the stuff from it

  // these are for construction
  void		AddParents(TypeDef* p1=NULL, TypeDef* p2=NULL,
			   TypeDef* p3=NULL, TypeDef* p4=NULL,
			   TypeDef* p5=NULL, TypeDef* p6=NULL);
  void		AddClassPar(TypeDef* p1=NULL, int p1_off=0, TypeDef* p2=NULL, int p2_off=0,
			    TypeDef* p3=NULL, int p3_off=0, TypeDef* p4=NULL, int p4_off=0,
			    TypeDef* p5=NULL, int p5_off=0, TypeDef* p6=NULL, int p6_off=0);
  void		AddParFormal(TypeDef* p1=NULL, TypeDef* p2=NULL,
			     TypeDef* p3=NULL, TypeDef* p4=NULL,
			     TypeDef* p5=NULL, TypeDef* p6=NULL);
  void		AddParCache(TypeDef* p1=NULL, TypeDef* p2=NULL,
			    TypeDef* p3=NULL, TypeDef* p4=NULL,
			    TypeDef* p5=NULL, TypeDef* p6=NULL);

  void		ComputeMembBaseOff();
  // only for MI types, after adding parents, get new members & compute base_off
  bool		IgnoreMeth(const String& nm) const;
  // check if given method should be ignored (also checks parents, etc)

  bool	 	FindParentName(const char* nm) const;
  bool 		FindParent(const TypeDef* it) const;
  // recursively tries to find parent, returns true if successful
  void*		GetParAddr(const char* par, void* base) const;
  void*		GetParAddr(TypeDef* par, void* base) const;
  // return the given parent's address given the base address (par must be a parent!)
  int		GetParOff(TypeDef* par, int boff=-1) const;
  // return the given parent's offset (par must be a parent!)
  bool 		ReplaceParent(TypeDef* old_tp, TypeDef* new_tp);
  // replace parent of old_tp with parent of new_tp (recursive)
  bool	 	FindChildName(const char* nm) const;
  bool 		FindChild(TypeDef* it) const;
  // recursively tries to  find child, returns true if successful

  TypeDef*	GetTemplParent() const;
  // returns immediate parent which is a template (or NULL if none found)
  String	GetTemplName(const TypeSpace& inst_pars) const;
  void		SetTemplType(TypeDef* templ_par, const TypeSpace& inst_pars);
  // set type of a template class

  static MemberDef* FindMemberPathStatic(TypeDef*& own_td, const String& path, bool warn = true);
  // looks for a member or sequence of members based on static type information for members (i.e., does not walk the structural tree and cannot go into lists or other containers, but can find any static paths for object members and their members, etc) -- if warn, emits warning message for bad paths

  EnumDef*	FindEnum(const String& enum_nm) const;
  // find an enum and return its definition (or NULL if not found).  searches in enum_vals, then subtypes
  int		GetEnumVal(const String& enum_nm, String& enum_tp_nm) const;
  // find an enum and return its enum_no value, and set enum_tp_nm at the type name of the enum.  if not found, returns -1 and enum_tp_nm is empty
  String	GetEnumString(const String& enum_tp_nm, int enum_val) const;
  // get the name of enum with given value in enum list of given type (e.g., enum defined within class)
  const String	Get_C_EnumString(int enum_val) const;
  // we must be the enumtypedef; get the C string for given value in enum list of given type (e.g., enum defined within class); for BIT types, will compose the bits and cast; worst case will cast int to type
  
#ifndef NO_TA_BASE  
  void*		GetInstance() const
  { void* rval=NULL; if(instance != NULL) rval = *instance; return rval; }
  int		FindTokenR(void* addr, TypeDef*& ptr) const;
  int		FindTokenR(const char* nm, TypeDef*& ptr) const;
  // recursive search for token among children
  void		AddUserDataSchema(UserDataItemBase* item); // adds the item as schema
#endif
  // for token management
  void 		Register(void* it);
  void 		unRegister(void* it);

  String	GetValStr(const void* base, void* par=NULL,
    MemberDef* memb_def = NULL, StrContext vc = SC_DEFAULT) const;
  // get a string representation of value
  const Variant	GetValVar(const void* base, const MemberDef* memb_def = NULL) const;
  // get a Variant representation of value; primarily for value types (int, etc.); NOTE: returns TAPtr types as the Base value (not a pointer to the pointer), which is usually what you want (see source for more detail)
  void		SetValVar(const Variant& val, void* base, void* par = NULL,
    MemberDef* memb_def = NULL);
  // sets value from a Variant representation; primarily for value types (int, etc.); 
  bool		ValIsDefault(const void* base, const MemberDef* memb_def, 
    int for_show = taMisc::IS_EXPERT) const; // true if the type contains its defaults
  bool 		ValIsEmpty(const void* base_, const MemberDef* memb_def) const;
    // true only if value is empty, ex 0 or "" 
  void		SetValStr(const String& val, void* base, void* par = NULL,
    MemberDef* memb_def = NULL, StrContext vc = SC_DEFAULT);
  // set the value from a string representation

  void		CopyFromSameType(void* trg_base, void* src_base,
					 MemberDef* memb_def = NULL);
  // copy all mmbers from same type
  void		CopyOnlySameType(void* trg_base, void* src_base,
					 MemberDef* memb_def = NULL);
  // copy only those members from same type (no inherited)
  void		MemberCopyFrom(int memb_no, void* trg_base, void* src_base);
  // copy a particular member from same type
  bool		CompareSameType(Member_List& mds, void_PArray& trg_bases,
				void_PArray& src_bases, 
				void* trg_base, void* src_base,
				int show_forbidden = taMisc::NO_HIDDEN,
				int show_allowed = taMisc::SHOW_CHECK_MASK,
				bool no_ptrs = true, bool test_only = false);
  // compare all member values from class of the same type as me, adding ones that are different to the mds, trg_bases, src_bases lists (unless test_only == true, in which case it just does the tests and returns true if any diffs -- for inline objects)

  // IO
  ostream& 	Output(ostream& strm, void* base, int indent=0) const;
  // output value information for display purposes
  ostream& 	OutputR(ostream& strm, void* base, int indent=0) const;
  // output value information for display purposes, recursive

  ostream&  	OutputType(ostream& strm, int indent = 0) const;
  // output type information only
  ostream&  	OutputInherit(ostream& strm) const;
  ostream&  	OutputInherit_impl(ostream& strm) const;

  // saving and loading of type instances to/from streams
  int		Dump_Save(ostream& strm, void* base, void* par=NULL, int indent=0);
  // called by the user to save an object
  int 		Dump_Save_impl(ostream& strm, void* base, void* par=NULL, int indent=0);
  int 		Dump_Save_inline(ostream& strm, void* base, void* par=NULL, int indent=0);
  // for saving objects as members of other objects
  int 		Dump_Save_Path(ostream& strm, void* base, void* par=NULL, int indent=0);
  // save the path of the object
  int 		Dump_Save_Value(ostream& strm, void* base, void* par=NULL, int indent=0);
  // save the value of this object (i.e. the members)
  int		Dump_SaveR(ostream& strm, void* base, void* par=NULL, int indent=0);
  // if there are sub-elements (i.e. groups), save them (return false if not)
  int 		Dump_Save_PathR(ostream& strm, void* base, void* par=NULL, int indent=0);
  // if there are sub-elements, save the path to them (return false if not)

  int		Dump_Load(istream& strm, void* base, void* par=NULL, void** el = NULL);
  // called by the user to load an object
  int		Dump_Load_impl(istream& strm, void* base, void* par=NULL,
			       const char* typnm=NULL);
  int		Dump_Load_Path(istream& strm, void*& base, void* par, TypeDef*& td,
			       String& path, const char* typnm=NULL);
  // loads a path (typename path) and fills in the base and td of object (false if err)
  int		Dump_Load_Path_impl(istream& strm, void*& base, void* par, String path);
  int		Dump_Load_Value(istream& strm, void* base, void* par=NULL);
  // loads the actual member values of the object (false if error)

  TypeDef*	FindTypeWithMember(const char* nm, MemberDef** md); // returns the type or child type with memberdef md
protected:
  mutable signed char	 m_cacheInheritsNonAtomicClass;
private:
  void 		Initialize();
  void		Copy_(const TypeDef& cp);
};

#endif // ta_type_h

