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
class TA_API taBase;
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
class TA_API TypeDef;
class TA_API taObjDiffRec;
class TA_API taObjDiff_List; //

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
extern TA_API TypeDef TA_const; // const is not formal...
extern TA_API TypeDef TA_enum;          // par_formal
extern TA_API TypeDef TA_struct;        // par_formal
extern TA_API TypeDef TA_union; // par_formal
extern TA_API TypeDef TA_class; // par_formal
extern TA_API TypeDef TA_template;      // par_formal
extern TA_API TypeDef TA_templ_inst;    // par_formal template instantiation
extern TA_API TypeDef TA_ta_array;      // par_formal (indicates "arrayness")
extern TA_API TypeDef TA_taBase;
extern TA_API TypeDef TA_taRegFun;      // registered functions
extern TA_API TypeDef TA_TypeItem;      // give these to the user...
extern TA_API TypeDef   TA_TypeDef;
extern TA_API TypeDef   TA_EnumDef;
extern TA_API TypeDef   TA_MemberDefBase;
extern TA_API TypeDef     TA_MemberDef;
extern TA_API TypeDef     TA_PropertyDef;
extern TA_API TypeDef   TA_MethodDef;
extern TA_API TypeDef TA_ta_Globals;
extern TA_API TypeDef TA_taString;
extern TA_API TypeDef TA_Variant;
extern TA_API TypeDef TA_QAtomicInt;
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
class cssEl;                    // #IGNORE

typedef cssEl* (*css_fun_stub_ptr)(void*, int, cssEl**);
typedef void (*ta_void_fun)();

class TA_API ta_memb_ptr_class {
public:
  virtual ~ta_memb_ptr_class()  { }; // make sure it has a vtable..
};

typedef int ta_memb_ptr_class::* ta_memb_ptr; //

// for Properties

typedef Variant (*ta_prop_get_fun)(const void*);
typedef void (*ta_prop_set_fun)(void*, const Variant&);

class TA_API NamedURL {
  // ##INLINE a named URL
public:
  String        name;           // the name of the website
  String        url;            // the URL -- uniform resource location path

  NamedURL()    { };
  NamedURL(const String& nm, const String& rl)  { name = nm; url = rl; }
  NamedURL(const NamedURL& cp)  { name = cp.name; url = cp.url; }
  ~NamedURL()   { };
};

class TA_API NameVar_PArray : public taPlainArray<NameVar> {
  // #NO_TOKENS a plain-array of name value (variant) items
INHERITED(taPlainArray<NameVar>)
public:
  static const String   def_sep; // ", "

  int   FindName(const String& nm, int start=0) const;
  // find by name  (start < 0 = from end)
  int   FindNameContains(const String& nm, int start=0) const;
  // find by name containing nm (start < 0 = from end)
  int   FindValue(const Variant& var, int start=0) const;
  // find by value (start < 0 = from end)
  int   FindValueContains(const String& vl, int start=0) const;
  // find by value.toString() containing vl (start < 0 = from end)

  Variant       GetVal(const String& nm);
  // get value from name; isNull if not found
  Variant       GetValDef(const String& nm, const Variant& def);
  // get value from name; def if not found
  bool          GetAllVals(const String& nm, String_PArray& vals);
  // get all values having given name (converts to strings)
  bool          SetVal(const String& nm, const Variant& val);
  // set value by name; if name already on list, it is updated (rval = true); else new item added

  const String  AsString(const String& sep = def_sep) const;
  void  operator=(const NameVar_PArray& cp)     { Copy_Duplicate(cp); }
  NameVar_PArray()                              { };
  NameVar_PArray(const NameVar_PArray& cp)      { Copy_Duplicate(cp); }
protected:
  int           El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(((NameVar*)a)->value > ((NameVar*)b)->value) rval=1; else if(((NameVar*)a)->value == ((NameVar*)b)->value) rval=0; return rval; }
  bool          El_Equal_(const void* a, const void* b) const
  { return (((NameVar*)a)->value == ((NameVar*)b)->value); }
  String        El_GetStr_(const void* it) const { return ((NameVar*)it)->GetStr(); }
  void          El_SetFmStr_(void* it, const String& val) { ((NameVar*)it)->SetFmStr(val); }
};

class TA_API DumpFileCvt {
  // ##NO_TOKENS #CAT_File parameters to convert a dump file
public:
  String        proj_type_base; // base name  of project (e.g., "Leabra" for "LeabraProject")
  String        key_srch_str;   // search string to identify this project type
  NameVar_PArray repl_strs; // search/replace strings (name -> value)

  DumpFileCvt(const String& prj_typ, const String& srch_st)
    { proj_type_base = prj_typ; key_srch_str = srch_st; }
};

class TA_API DumpFileCvtList : public taPtrList<DumpFileCvt> {
  // #CAT_File list of available dump file converters
protected:
  void          El_Done_(void* it)      { delete (DumpFileCvt*)it; }
  String El_GetName_(void* it) const { return ((DumpFileCvt*)it)->proj_type_base; }
public:
  ~DumpFileCvtList()                            { Reset(); }
};

#ifndef NO_TA_BASE

class UserDataItem_List;
class UserDataItemBase;
class ViewColor_List;

enum CancelOp { // ops for passing cancel status and instructions, typically for Window closing
  CO_PROCEED,           // tells caller to proceed with operation (typical default)
  CO_CANCEL,            // client can set this to tell caller to cancel the operation
  CO_NOT_CANCELLABLE    // preset, to tell client that operation will go ahead unconditionally
};


class TA_API taiMiscCore: public QObject {
  // ##NO_TOKENS ##NO_INSTANCE object for Qt Core event processing, etc. taiMisc inherits; taiM is always instance
INHERITED(QObject)
  Q_OBJECT
friend class TypeDef; // for the post_load guy
public:
#ifndef __MAKETA__
  enum CustomEvents {
    CE_QUIT             = QEvent::User + 1 // sent from RootWin::CloseEvent to avoid issues
  };
#endif
  static int            rl_callback(); // a readline-compatible callback -- calls event loop

  static taiMiscCore*   New(QObject* parent = NULL);
    // either call this or call taiMisc::New

  static void           WaitProc(); // the core idle loop process

  static int            ProcessEvents();
  // run any pending qt events that might need processed
  static int            RunPending();
  // check to see if any events are pending, and run if true -- MUCH faster than processevents, but also likely to miss some events along the way.

  static void           Quit(CancelOp cancel_op = CO_NOT_CANCELLABLE);
   // call to quit, invokes Quit_impl on instance first
  static void           OnQuitting(CancelOp& cancel_op); // call this when a quit situation is detected -- does all the save logic

  const String          classname(); // 3.x compatability, basically the app name

  int                   Exec(); // enter the event loop, either QCoreApplication or QApplication
  virtual void          Busy_(bool busy) {} // impl for gui in taiMisc
  virtual void          CheckConfigResult_(bool ok);
    // this is the nogui version; taiMisc does the gui version

  taiMiscCore(QObject* parent = NULL);
  ~taiMiscCore();

public slots:
  void                  PostUpdateAfter(); // run the dumpMisc::PostUpdateAfter; called asynchronously via a timer

protected slots:
  void                  app_aboutToQuit();
  virtual void          timer_timeout(); // called when timer times out, for waitproc processing

protected:
  QTimer*               timer; // for idle processing
  override void         customEvent(QEvent* ev);
  virtual void          Init(bool gui = false); // NOTE: called from static New
  virtual int           Exec_impl();
  virtual void          OnQuitting_impl(CancelOp& cancel_op); // allow to cancel
  virtual void          Quit_impl(CancelOp cancel_op); // non-gui guy does nothing
};

extern TA_API taiMiscCore* taiMC_; // note: use taiM macro instead
#ifdef TA_GUI
# define taiM taiM_
#else
# define taiM taiMC_
#endif
#endif

typedef  void (*init_proc_t)() ;        // initialization proc

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
  signed char   operator++() {return ++cnt;}
  signed char   operator++(int) {return cnt++;} // post
  signed char   operator--() {return --cnt;}
  signed char   operator--(int)  {return cnt--;} // post

  ContextFlag() {cnt = 0;} // NOTE: default binary copy constructor and copy operator are fine
private:
  signed char   cnt; // keep same size as bool -- should never be nesting this deep
};
#endif // __MAKETA__


class TA_API taVersion {
  // #EDIT_INLINE simple value class for version info
public:
  ushort        major;
  ushort        minor;
  ushort        step;
  ushort        build;

  void          set(ushort mj, ushort mn, ushort st = 0, ushort bld = 0)
    {major = mj; minor = mn; step = st; build = bld;}
  void          setFromString(String ver); // parse, mj.mn.st-build
  const String  toString()
    {return String(major).cat(".").cat(String(minor)).cat(".").
       cat(String(step)).cat(".").cat(String(build));}

  void          Clear() {major = minor = step = build = 0;} //
  bool          GtEq(ushort mj, ushort mn, ushort st = 0) {
    return (major > mj) || ((major == mj) && (minor > mn)) ||
      ((major == mj) && (minor == mn) && (step >= st));
  }
  // true if the version is greater than or equal to the indicated version

  bool          operator>=(taVersion& cmp)
  { return GtEq(cmp.major, cmp.minor, cmp.step); }
  bool          operator==(taVersion& cmp)
  { return (major == cmp.major && minor == cmp.minor && step == cmp.step); }
  bool          operator!=(taVersion& cmp)
  { return (major != cmp.major || minor != cmp.minor || step == cmp.step); }
  bool          operator>(taVersion& cmp)
  { return (major > cmp.major) || ((major == cmp.major) && (minor > cmp.minor)) ||
      ((major == cmp.major) && (minor == cmp.minor) && (step > cmp.step)); }
  bool          operator<(taVersion& cmp)
  { return !GtEq(cmp.major, cmp.minor, cmp.step); }

  taVersion() {Clear();} //
  taVersion(ushort mj, ushort mn, ushort st = 0, ushort bld = 0)
    {set(mj, mn, st, bld);} //
  taVersion(const String& ver) {setFromString(ver);}
  // implicit copy and assign

  static int    BeforeOrOf(char sep, String& in);
  // #IGNORE for parsing
};

class TA_API taThreadDefaults {
  // #EDIT_INLINE threading default parameters
public:
  int           cpus;           // #READ_ONLY #SHOW #NO_SAVE number of physical processors (cores) on this system -- typically you want to set n_threads equal to this, but not necessarily always -- may need to use fewer threads in some cases (almost never more)
  int           n_threads;      // #MIN_1 number of threads to actually use -- typically the number of physical processors (cores) available (see cpus), and is initialized to that.
  float         alloc_pct;      // #MIN_0 #MAX_1 proportion (0-1) of total to process by pre-allocating a set of computations to a given thread -- the remainder of the load is allocated dynamically through a nibbling mechanism, where each thread takes a nibble_chunk at a time until the job is done.  current experience is that this should be no greater than .2, unless the load is quite large, as there is a high degree of variability in thread start times, so the automatic load balancing of nibbling is important, and it has very little additional overhead.
  int           nibble_chunk;   // #MIN_1 how many units does each thread grab to process while nibbling?  Too small a value results in increased contention and inefficiency, while too large a value results in poor load balancing across processors.  roughly 8 seems good in many cases
  float         compute_thr;    // #MIN_0 #MAX_1 threshold value for amount of computation in a given function to actually deploy on threads, as opposed to just running it on main thread -- value is normalized (0-1) with 1 being the most computationally intensive task, and 0 being the least -- as with min_units, it may not be worth it to parallelize very lightweight computations.  See Thread_Params page on emergent wiki for relevant comparison values.
  int           min_units;      // #MIN_1 minimum number of computational units (e.g., network units) to apply parallel threading to -- if less than this number, all will be computed on the main thread to avoid threading overhead which may be more than what is saved through parallelism, if there are only a small number of things to compute.

  taThreadDefaults();
// implicit copy and assign
};

/////////////////////////////////////////////////////////////////////
//      taMisc

class TA_API taMisc {
  // #NO_TOKENS #INSTANCE global parameters and functions for the application
friend class InitProcRegistrar;
public:

  // note: don't rationalize the memb bits, because it breaks the user prefs file
  // the NO_xxx guys are for use in 'forbidden' contexts (legacy 'show')
  // the IS_xxx guys are for use in 'allowed' contexts, and/or categorizing guys
  enum ShowMembs { // #BITS
    NO_HIDDEN           = 0x01, // don't show items marked READ_ONLY w/o SHOW or HIDDEN
    NO_unused1          = 0x02, // #IGNORE
    NO_unused2          = 0x04, // #IGNORE
    NO_NORMAL           = 0x08, // #NO_SHOW don't show items normally shown (helps indicate, ex. EXPERT items)
    NO_EXPERT           = 0x10, // don't show items marked EXPERT (often only for advanced sims)

    ALL_MEMBS           = 0x00, // #NO_BIT
    NORM_MEMBS          = 0x11, // #NO_BIT
    EXPT_MEMBS          = 0x09, // #NO_BIT
    HIDD_MEMBS          = 0x18, // #NO_BIT

    IS_HIDDEN           = 0x01, // #IGNORE used in MemberDef::ShowMember to flag RO w/o SHOW or HIDDEN guys
    IS_NORMAL           = 0x08, // #IGNORE used in MemberDef::ShowMember to flag NORMAL guys
    IS_EXPERT           = 0x10, // #IGNORE used in MemberDef::ShowMember to flag EXPERT guys
#ifndef __MAKETA__
    SHOW_CHECK_MASK     = IS_HIDDEN | IS_NORMAL | IS_EXPERT, // #IGNORE #NO_BIT used in MemberDef::ShowMember checks, default for "allowed" param
#endif
    USE_SHOW_GUI_DEF    = 0x40  // #NO_BIT use default from taMisc::show_gui, only applies to forbidden
  };

  enum TypeInfo {
    MEMB_OFFSETS,               // display all including member offsets
    ALL_INFO,                   // display all type information
    NO_OPTIONS,                 // don't display options
    NO_LISTS,                   // don't display lists
    NO_OPTIONS_LISTS            // don't display options or lists
  };

  enum KeepTokens {
    Tokens,                     // keep tokens as specified by the type
    NoTokens,                   // don't keep any tokens
    ForceTokens                 // force to keep all tokens
  };

  enum SaveFormat {
    PLAIN,                      // dump files are not formatted for easy reading
    PRETTY                      // dump files should be more readable by humans
  };

  enum LoadVerbosity {
    QUIET,                      // don't say anything except errors
    VERSION_SKEW,               // display mismatches in names of objects in loading file that are likely due to changes in newer versions of the software
    MESSAGES,                   // display informative messages on css console during load
    TRACE,                      // and show a trace of objects loaded
    SOURCE                      // and show the source of the load as its loaded
  };

  enum AutoRevert {
    AUTO_APPLY,                 // automatically apply changes before auto-reverting
    AUTO_REVERT,                // automatically revert, losing changes
    CONFIRM_REVERT              // put up a confirmatory message before reverting
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
    QF_RUNNING          = 0, //
    QF_USER_QUIT,       // we can still cancel while in this state of quitting
    QF_FORCE_QUIT       // too late to turn back now...
  };

  enum ConsoleType { // the type of console window and how to show it; ignored in non-gui mode (either uses OS shell, or no console, depending on startup mode)
    CT_OS_SHELL = 0, // #LABEL_OS_Shell use the operating system's shell or console (with readline library on unix)
#if defined(HAVE_QT_CONSOLE) && !defined(TA_OS_WIN) // qt console not supported on windows, needs to be ported to Win32
    CT_GUI = 1, // #LABEL_Gui uses a gui-based console, either docked in the main app window, or floating (see console_options)
#else
    CT_GUI = 1, // #NO_SHOW uses a gui-based console, either docked in the main app window, or floating (see console_options)
#endif
    CT_NONE = 4 // #NO_SHOW no console, usually only used internally, such as for batch or dmem operation
  };

  enum ConsoleOptions { // #BITS options that can be used with the console
    CO_0 = 0, // #NO_BIT #IGNORE dummy item, and to clear
#ifdef TA_OS_WIN // pager causes crashes and deadlocks in windows
    CO_USE_PAGING_GUI   = 0x0001, // #NO_BIT use paging, like the "more" command -- NOT RECOMMENDED FOR WINDOWS
#else
    CO_USE_PAGING_GUI   = 0x0001, // #LABEL_Use_Paging_Gui use paging in the gui console, like the 'more' command
#endif
    CO_USE_PAGING_NOGUI = 0x0002, // #LABEL_Use_Paging_NoGui use paging in the nogui console/shell, like the 'more' command
#ifdef HAVE_QT_CONSOLE
    CO_GUI_TRACKING     = 0x0004, // #LABEL_Gui_Tracking in GUI mode, the console floats below the active project -- this only operates if DockRoot is not checked
    CO_GUI_DOCK         = 0x0008, // #LABEL_Gui_DockRoot in GUI mode, dock the console in the root window -- if this checked, then tracking is not done
#else
    CO_GUI_TRACKING     = 0x0004, // #NO_SHOW in GUI mode, the console floats below the active project -- this only operates if DockRoot is not checked
    CO_GUI_DOCK         = 0x0008, // #NO_SHOW in GUI mode, dock the console in the root window -- if this checked, then tracking is not done
#endif
  };

  enum ColorHints { // #BITS what types of color hinting to use in the application
    CH_EDITS            = 0x0001, // color the background of property editors according to the type of the item
    CH_BROWSER          = 0x0002 // color browser tree text according to the type of the item
  };

  enum BuildType { // #BITS what type of build this is
    BT_0                = 0x00, // #NO_BIT constant for when no other flags set
    BT_DEBUG            = 0x01, // a debug build
    BT_DMEM             = 0x02, // compiled for MPI (clustered use)
    BT_NO_GUI           = 0x04  // compiled without gui support
  };

  enum ClickStyle { // how to select editable items, such as in T3
    CS_SINGLE,  // #LABEL_Single single click opens properties
    CS_CONTEXT  // #LABEL_Context choose from context menu
  };

  enum MatrixView {     // order of display for matrix cols
    DEF_ZERO    = -1, // #IGNORE use the current global default
    BOT_ZERO,   // row zero is displayed at bottom of view (default)
    TOP_ZERO    // row zero is displayed at top of view (ex. for images)
  };

  enum ProjViewPref { // project viewing preference
    PVP_2x2,    // #LABEL_2x2_Panes one window has a browser and edit pane, the second has an edit pane and a 3-d viewer
    PVP_3PANE   // #LABEL_3_Pane one window with a browser, edit pane, and a 3d-viewer
  };

  enum ViewerOptions { // #BITS options for the viewer
    VO_0                = 0, // #IGNORE
    VO_DOUBLE_CLICK_EXP_ALL = 0X001, // #LABEL_DoubleClickExpAll double click expands or contracts all tree items -- use at your own risk on big projects...
    VO_AUTO_SELECT_NEW  = 0x002, // #LABEL_AutoSelectNew automatically select (the first) of a new tree item that is made with New or similar menu commands
    VO_AUTO_EXPAND_NEW  = 0x004, // #LABEL_AutoExpandNew automatically expand new tree items that are made with New or similar menu commands
    VO_NO_TOOLBAR = 0x008,       // #LABEL_NoToolbar do not turn on the toolbar by default in new project views
    VO_NO_TOOLBOX = 0x010,       // #LABEL_NoToolbox do not turn on the program toolbox by default in new projectd views
  };

  enum  GuiStyle {      // style options provided by the gui system (not all are available on all platforms)
    GS_DEFAULT,         // #LABEL_Default use the default style for whatever platform you're running on
    GS_PLASTIQUE,       // #LABEL_Plastique default style on linux -- a good choice to try
    GS_CLEANLOOKS,      // #LABEL_CleanLooks similar overall to plastique, with a bit more of a windows look
    GS_MOTIF,           // #LABEL_Motif a classic look from the 90's -- one of the first 3d-looks
    GS_CDE,             // #LABEL_CDE Common Desktop Environment -- a bit lighter and cleaner than MOTIF, but overall very similar to it
#ifdef TA_OS_MAC
    GS_MACINTOSH,       // #LABEL_Macintosh only available on a macintosh!
#else
    GS_MACINTOSH,       // #NO_SHOW
#endif
    GS_WINDOWS,         // #LABEL_Windows standard old-school Microsoft Windows (pre XP)
#ifdef TA_OS_WIN
    GS_WINDOWSXP,       // #LABEL_WindowsXP Windows XP look -- only available on Windows XP or higher
    GS_WINDOWSVISTA,    // #LABEL_WindowsVista Windows Vista look -- only available on Windows Vista or higher
#else
    GS_WINDOWSXP,       // #NO_SHOW
    GS_WINDOWSVISTA,    // #NO_SHOW
#endif
  };

  enum EditStyle { // style of editor
    ES_ALL_CONTROLS,   // #LABEL_All_Controls all the edit controls are displayed at the same time, which is slower but can present the data in a somewhat more readable fashion -- this is the original default for all edit dialogs
    ES_ACTIVE_CONTROL, // #LABEL_Active_Control only display the currently-active edit control, with the rest of the information displayed as formatted text values -- this is much faster and is preferred for the select edit (control panel) displays, which otherwise can really slow down the system
  };

  enum HelpDetail { // level of detail for generated help information
    HD_DEFAULT,         // standard level of help detail
    HD_DETAILS,         // full programming details -- offsets, comments, etc
  };

  enum  StdLicense {          // standard licenses in wide use
    NO_LIC,                     // no license granted -- All Rights Reserved
    GPLv2,                      // GNU General Public License (GPL), version 2
    GPLv3,                      // GNU General Public License (GPL), version 3 -- note that this has patent grant issues -- version 2 is probably safer
    BSD,                        // Berkeley Software Distribution License
    OSL3,                       // Open Software License, version 3
    AFL3,                       // Academic Free License, version 3
    MIT,                        // MIT License
    APACHE,                     // Apache License, version 2.0
    RESEARCH,                   // Research-Only License based on CU Boulder template
    CUSTOM_LIC,                 // enter custom license text
  };

  static String         app_name;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_App the root name of the app, ex. "emergent" or "css"
  static String         app_prefs_key;
  // #READ_ONLY #NO_SAVE #HIDDEN #CAT_App the string used for the prefs folders etc. -- emergent and css should share this, but external apps (ex physics sim) should make a new one
  static String         default_app_install_folder_name;
  // #READ_ONLY #NO_SAVE #HIDDEN #CAT_App the default folder name for installation, ex. "Emergent"
  static String         org_name;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_App the name of the organization, ex. ccnlab (used mostly in Windows paths)
  static String         version;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_App version number of ta/css
  static taVersion      version_bin;
   //  #READ_ONLY #NO_SAVE #EXPERT #CAT_App version number of ta/css
  static String         svn_rev;
   // #READ_ONLY #NO_SAVE #SHOW #CAT_App svn revision number
  static const BuildType build_type;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_App build type, mostly for determining plugin subfolders to search
  static const String    build_str;
  // #READ_ONLY #NO_SAVE #EXPERT #CAT_App an extension string based on build type, mostly for plugin subfolders (none for release gui no-dmem)
  static bool            save_old_fmt;
  // #SAVE #CAT_File save project and other 'dump' files in the old emergent format (version 2 of the file format) which was used up through version 5.0.1 -- files are unlikely to be very functional in older versions anyway, but this would allow them to at least be loaded -- only use this on a temporary basis when absolutely necessary! (note: old format files can be loaded without problem in current version)

  ////////////////////////////////////////////////////////
  //    User-tunable compute params

  static taThreadDefaults thread_defaults;
  // #SAVE #CAT_MultiProc defaults for parallel threading -- these are used to initialize values in any specific parallel threading context, as they should be specific to a given machine, not to a given model or project

  ////////////////////////////////////////////////////////
  //    TA GUI parameters

  static GuiStyle       gui_style;      // #SAVE #CAT_GUI #DEF_GS_DEFAULT style options provided by the gui system, affecting how the widgets are drawn, etc (not all are available on all platforms) -- change only takes effect on restarting the program
  static String         font_name;      // #SAVE #CAT_GUI default font name to use
  static int            font_size;      // #SAVE #CAT_GUI default font size to use
  static String         t3d_bg_color;   // #SAVE #CAT_GUI default background color for 3d view -- standard X11 color names are supported, most of which are also web/html standard color names
  static String         t3d_text_color; // #SAVE #CAT_GUI default text color for 3d view  -- standard X11 color names are supported, most of which are also web/html standard color names
  static String         t3d_font_name;  // #SAVE #CAT_GUI #EXPERT default font name to use in the 3D display (default is Arial -- not many options supported depending on platform -- set the environment variable COIN_DEBUG_FONTSUPPORT to debug)
  static ConsoleType    console_type; // #SAVE #CAT_GUI style of the console to display -- **REQUIRES APP RESTART
#ifdef TA_OS_WIN // none on windows (yet), so we omit for clarity
  static ConsoleOptions console_options; // #IGNORE #CAT_GUI #EXPERT options for the console **REQUIRES APP RESTART
#else
  static ConsoleOptions console_options; // #SAVE #CAT_GUI #EXPERT options for the console **REQUIRES APP RESTART
#endif
  static String         console_font_name;      // #SAVE #CAT_GUI font name for the css console
  static int            console_font_size;      // #SAVE #CAT_GUI font size for the css console
  static float          doc_text_scale; // #SAVE #CAT_GUI scale factor for text displayed in doc objects (including web pages) -- multiplies base setting from font_size parameter (above), plus any doc-specific text_size parameter -- values > 1 make the text bigger, < 1 = smaller
  static int            display_width;  // #SAVE #HIDDEN #MIN_40 #MAX_132 #CAT_GUI width of console display (in chars) -- set automatically by gui console

  static bool           emacs_mode;     // #SAVE #CAT_GUI use full emacs key bindings -- all non-conflicting emacs keys are available regardless, but with this turned on, Ctrl+V is page down instead of Paste -- use Ctrl+Y (emacs yank) for paste instead -- on a Mac, Command+V is usually paste, so Ctrl+V can be used for page down without conflict, so this setting defaults to ON for macs
  static int            undo_depth;     // #SAVE #CAT_GUI how many steps of undo are maintained -- the system is very efficient so large numbers (default 100) are usually acceptable -- see Project UndoStats menu item for memory usage statistics
  static int            undo_data_max_cells; // #SAVE #CAT_GUI maximum number of cells in a data table to save an undo copy -- if above this number of cells, it won't be saved for undo (only the column structure will be retained)
  static float          undo_new_src_thr; // #SAVE #CAT_GUI #EXPERT threshold for how big (as a proportion of total file size) the diff's need to get before a new undo source record is created (default of around .3 is usually fine)
  static int            auto_save_interval;     // #SAVE #CAT_GUI how many seconds to wait between automatic saves of opened projects that have been modified?  auto save files go to project file name + _autosave
  static int            wait_proc_delay; // #SAVE #CAT_GUI #DEF_20 #EXPERT delay in milliseconds before starting the wait processing function to process misc stuff after all of the current gui events have been processed -- a smaller number makes the system more responsive but also consumes a bit more CPU -- setting to 0 consumes a lot of CPU as the wait processing loop is constantly revisited
  static int            css_gui_event_interval; // #SAVE #CAT_GUI #DEF_200 #EXPERT how many milliseconds between processing of gui events in css -- lower number = more responsive interface, but worse performance, while things are running
  static bool           delete_prompts;  //  #SAVE #CAT_GUI should a prompt be provided to confirm when deleting an item?  with the undo system available, this is not neccessary
  static int            tree_indent;    // #SAVE #CAT_GUI #EXPERT number of pixels to indent in the tree browser gui interface
  static int            program_editor_width;   // #SAVE #CAT_GUI #EXPERT width in characters of the main listing field of the program editor -- if this is too large, then you can't see the description comments
  static int            program_editor_lines;   // #SAVE #CAT_GUI #MIN_4 #MAX_20 default number of lines in the mini editor within the program editor, where program elements and other objects are edited.

  static HelpDetail     help_detail;    // #SAVE #CAT_GUI #EXPERT level of detail to display in the help system

  static int            max_menu;       // #SAVE #CAT_GUI #EXPERT maximum number of items in a menu -- largely obsolete at this point
  static int            search_depth;   // #SAVE #CAT_GUI #EXPERT depth recursive find will search for a path object
  static int            color_scale_size; // #SAVE #CAT_GUI #EXPERT number of colors to put in a color scale
  static int            jpeg_quality;   // #SAVE #CAT_GUI #EXPERT jpeg quality for dumping jpeg files (1-100; 95 default) -- in general it is better to use PNG format which is lossless and yields small compressed file sizes for saved view images, etc
  static ColorHints     color_hints; // #SAVE #CAT_GUI #EXPERT what types of color hinting to use in the application
  static ProjViewPref   proj_view_pref; // #SAVE #CAT_GUI #EXPERT the default way to view projects
  static ViewerOptions  viewer_options; // #SAVE #CAT_GUI #EXPERT misc options for the viewer
#ifndef NO_TA_BASE
//NOTE: following not keeping tokens so cannot be viewed in any mode
  static ViewColor_List* view_colors;   // #NO_SAVE #NO_SHOW colors to use in the view displays -- looked up by name emitted by GetTypeDecoKey and GetStateDecoKey on objects
#endif
  static EditStyle      std_edit_style; // #SAVE #SHOW #READ_ONLY #CAT_GUI #EXPERT style to use for standard edit dialogs (i.e., non select-edit dialogs)
  static EditStyle      select_edit_style; // #SAVE #CAT_GUI #EXPERT style to use for select edit dialogs (which are typically used for control panels, etc)

  static int            antialiasing_level; // #SAVE #CAT_GUI level of smoothing to perform in the 3d display -- values depend on hardware acceleration, but 2 or 4 are typical values.  1 or lower disables entirely.  modern hardware can do typically do level 4 with little slowdown in speed.
  static float          text_complexity;     // #SAVE #CAT_GUI #EXPERT complexity value (between 0 and 1) for rendering 3D text -- values above .5 are usually not noticibly better and slow rendering
  static ShowMembs      show_gui;       // #SAVE #CAT_GUI #EXPERT what to show in the gui
  static TypeInfo       type_info_;     // #SAVE #CAT_GUI #EXPERT #LABEL_type_info what to show when displaying type information
  //note: 'type_info' is a reserved word in C++, it is the type of rtti data
  static KeepTokens     keep_tokens;    // #SAVE #CAT_GUI #EXPERT default for keeping tokens
  static SaveFormat     save_format;    // #SAVE #CAT_GUI #EXPERT format to use when saving things (dump files)
  static bool           auto_edit;      // #SAVE #CAT_GUI #EXPERT automatic edit dialog after creation?
  static AutoRevert     auto_revert;    // #SAVE #CAT_GUI #EXPERT when dialogs are automatically updated (reverted), what to do about changes?
  static MatrixView     matrix_view;    // #SAVE #CAT_GUI #EXPERT #DEF_BOT_ZERO whether to show matrices with 0 row at top or bottom of view
  static bool           beep_on_error; // #SAVE #DEF_false #CAT_GUI beep when an error message is printed on the console
  static short          num_recent_files; // #SAVE #DEF_10 #MIN_0 #MAX_50 number of recent files to save
  static short          num_recent_paths; // #SAVE #DEF_10 #MIN_0 #MAX_50 number of recent paths to save
  static short          num_browse_history; // #SAVE #DEF_20 #MIN_10 #MAX_50 number of browse history items to keep

  ////////////////////////////////////////////////////////
  //    File/Paths Info

  static int            strm_ver;       // #READ_ONLY #NO_SAVE during dump or load, version # (app v4.x=v2 stream)
  static bool           save_compress;  // #SAVE #DEF_false #CAT_File compress by default for files that support it (ex .proj, .net)\nNOTE: starting with v4.0, compression is no longer recommended except for large weight files or large nets with saved units
  static TypeDef*       default_proj_type; // #SAVE #CAT_File #TYPE_taProject default type of project to create
  static StdLicense     license_def;       // #SAVE #CAT_File default license to use for new projects that are created -- can change for specific projects -- see license field on projects
  static String         license_owner;     // #SAVE #CAT_File default legal owner of new projects that are created by this user (e.g., Regents of University of xyz) -- used for copyright and licensing information -- see project license field for where to change or update on existing projects
  static String         license_org;       // #SAVE #CAT_File default organization that actually created the project for new projects that are created by this user (e.g., MyLab at University of xyz) -- used for copyright and licensing information -- see project license field for where to change or update on existing projects -- defaults to license_owner if left blank

  static LoadVerbosity  verbose_load;   // #SAVE #CAT_File report the names of things during loading

  static String         app_dir;
  // #SHOW #READ_ONLY #CAT_File base of installed app directory -- override with "-a <path>" command line switch
  static String         app_plugin_dir;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_File location of installed system plugins
  static String         app_dir_default;
  // #OBSOLETE #NO_SHOW #READ_ONLY #NO_SAVE #CAT_File obs as of 4.0.19 -- nuke at some point
  static String         user_dir;
  // #SHOW #READ_ONLY #CAT_File location of user's home directory -- override with "-u <path>" command line switch
  static String         prefs_dir;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_File location of preference files
  static String         user_app_dir;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_File user's location of application, for Program libraries, etc. can be overridden in {APPNAME}_USER_APP_DIR env variable
  static String         user_plugin_dir;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_File location of installed user plugins
  static String         user_log_dir;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_File location of log files, such as plugin log
  static String         exe_cmd;
  // #SHOW #READ_ONLY #CAT_File executable command path and filename -- how was this program invoked (from argv0)
  static String         exe_path;
  // #SHOW #READ_ONLY #CAT_File full absoluate path to executable

  static String         web_home;
  // #NO_SAVE #READ_ONLY #SHOW #EXPERT #CAT_File url for location of main web home page for this application
  static String         web_help_wiki;
  // #NO_SAVE #READ_ONLY #SHOW #EXPERT #CAT_File wiki name for web application help such that appending the name of the object in question will produce help for that specific object or topic
  static String         web_help_general;
  // #NO_SAVE #READ_ONLY #SHOW #EXPERT #CAT_File url for general web application help, not associated with a specific object

  static NamedURL       wiki1_url;
  // #SAVE #CAT_File short name and url for wiki project repository, for sync'ing local project and other information (just base address without index.php or anything like that) -- full url is looked up by name to refer to specific sites
  static NamedURL       wiki2_url;
  // #SAVE #CAT_File short name and url for wiki project repository, for sync'ing local project and other information (just base address without index.php or anything like that) -- full url is looked up by name to refer to specific sites
  static NamedURL       wiki3_url;
  // #SAVE #CAT_File short name and url for wiki project repository, for sync'ing local project and other information (just base address without index.php or anything like that) -- full url is looked up by name to refer to specific sites
  static NamedURL       wiki4_url;
  // #SAVE #CAT_File short name and url for wiki project repository, for sync'ing local project and other information (just base address without index.php or anything like that) -- full url is looked up by name to refer to specific sites
  static NamedURL       wiki5_url;
  // #SAVE #CAT_File short name and url for wiki project repository, for sync'ing local project and other information (just base address without index.php or anything like that) -- full url is looked up by name to refer to specific sites
  static NamedURL       wiki6_url;
  // #SAVE #CAT_File short name and url for wiki project repository, for sync'ing local project and other information (just base address without index.php or anything like that) -- full url is looked up by name to refer to specific sites

  static NameVar_PArray wikis;
  // #NO_SAVE #READ_ONLY #HIDDEN the non-empty wiki names and urls from the above set of wikis -- for actual programmatic use -- above list is just for simple gui editing in preferences/options

  // don't save these paths: they are generated from above which are saved, and can
  // be modified more reliably in a .cssinitrc or similar..
  static String_PArray  css_include_paths;
  // #NO_SAVE #HIDDEN #CAT_File paths to be used for finding css files (e.g., in #include or load statements -- searched in order)
  static String_PArray  load_paths;
  // #NO_SAVE #HIDDEN #CAT_File paths to be used for loading object files for the ta dump file system
  static NameVar_PArray prog_lib_paths;
  // #NO_SAVE #HIDDEN #CAT_File paths/url's for specific categories of program library files (e.g., System, User, Web)
  static NameVar_PArray proj_template_paths;
  // #NO_SAVE #HIDDEN #CAT_File paths/url's for collections of project template files (e.g., System, User, Web)
  static NameVar_PArray named_paths;
  // #NO_SAVE #HIDDEN #CAT_File paths/url's for misc purposes -- search by name, value = path

  static DumpFileCvtList file_converters;   // #CAT_File #HIDDEN conversion parameters (from v.3 to v.4)

  static String         compress_sfx;   // #SAVE #CAT_File #EXPERT suffix to use for compressing files

  static ostream*       record_script;  // #IGNORE #CAT_File stream to use for recording a script of interface activity (NULL if no record)

  static String         edit_cmd;       // #SAVE #CAT_File how to run editor
#ifdef TA_OS_WIN
  static String     plugin_make_env_cmd;
  // #SAVE #CAT_File command to set the environment for making (compiling) a plugin -- default is: call \"C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\vcvarsall.bat\" x86 -- change last arg to amd64 for a 64bit platform
#endif

  ////////////////////////////////////////////////////////
  //    Args

  static String_PArray  args_raw;
  // #READ_ONLY #NO_SAVE #HIDDEN #CAT_Args raw list of arguments passed to program at startup (in order, no filtering or modification)
  static NameVar_PArray arg_names;
  // #READ_ONLY #NO_SAVE #HIDDEN #CAT_Args conversions between arg flags (as a String in name field, e.g., -f or --file) and a canonical functional name (in value field, e.g., CssScript)
  static NameVar_PArray arg_name_descs;
  // #READ_ONLY #NO_SAVE #HIDDEN #CAT_Args descriptions of arg names for help -- name is canonical functional name (e.g., CssScript) and value is string describing what this arg does
  static NameVar_PArray args;
  // #READ_ONLY #NO_SAVE #HIDDEN #CAT_Args startup arguments processed by arg_names into name/value pairs -- this is the list that should be used!
  static String_PArray  args_tmp;
  // #NO_SAVE #HIDDEN #CAT_Args temporary list of args; can be passed to GetAllArgsNamed in scripts..

  ////////////////////////////////////////////////////////
  //    DMEM: Distributed Memory

  static int            dmem_proc;
  // #READ_ONLY #EXPERT #NO_SAVE #SHOW #CAT_DMem distributed memory process number (rank in MPI, always 0 for no dmem)
  static int            dmem_nprocs;
  // #READ_ONLY #EXPERT #NO_SAVE #SHOW #CAT_DMem distributed memory number of processes (comm_size in MPI, 1 for no dmem)
  static bool           dmem_debug;
  // #SAVE #EXPERT #CAT_DMem turn on debug messages for distributed memory processing

  ////////////////////////////////////////////////////////
  //    Global State, Flags Etc

  static TypeSpace      types;          // #READ_ONLY #NO_SAVE list of all the active types
  static TypeSpace      aka_types;      // #READ_ONLY #NO_SAVE list of types that have AKA for other types that are no longer supported
  static TypeDef*       default_scope;  // #READ_ONLY #NO_SAVE type of object to use to determine if two objects are in the same scope

  static taPtrList_impl* init_hook_list; // #IGNORE list of init hook's to call during initialization

  static bool           in_init;        // #READ_ONLY #NO_SAVE #NO_SHOW true if in ta initialization function
  static bool           in_event_loop;  // #READ_ONLY #NO_SAVE #NO_SHOW true when in the main event loop (ex. now ok to do ProcessEvents)
  static signed char    quitting;       // #READ_ONLY #NO_SAVE #NO_SHOW true, via one of QuitFlag values, once we are quitting
  static bool           not_constr;     // #READ_ONLY #NO_SAVE #NO_SHOW true if ta types are not yet constructed (or are destructed)

  static bool           use_gui;        // #READ_ONLY #NO_SAVE #NO_SHOW whether the user has specified to use the gui or not (default = true)
  static bool           gui_active;     // #READ_ONLY #NO_SAVE #NO_SHOW if gui has actually been started up or not -- this is the one that should be checked for gui modality in all non-startup code
  static bool           gui_no_win;     // #READ_ONLY #NO_SAVE #NO_SHOW an intermediate form of gui operation where the gui system is fully initialized, but no windows are created, and gui_active remains false -- this is useful for batch (background) jobs that need to do offscreen rendering or other gui-dependent functions
  static bool           in_dev_exe;     // #READ_ONLY #NO_SAVE #NO_SHOW are we running a development executable -- running out of the build directory of the source code -- do some things differently in this case (e.g., no plugins)
  static bool           use_plugins;    // #READ_ONLY #NO_SAVE #NO_SHOW whether to use plugins
  static bool           server_active;  // #READ_ONLY #NO_SAVE #NO_SHOW if remote server has been started up or not
  static ContextFlag    is_loading;     // #READ_ONLY #NO_SAVE #NO_SHOW true if currently loading an object
  static taVersion      loading_version;
  //  #READ_ONLY #NO_SAVE #EXPERT version number associated with file currently being loaded
  static ContextFlag    is_post_loading;// #READ_ONLY #NO_SAVE #NO_SHOW true if currently in the post load routine (DUMP_POST_LOAD)
  static ContextFlag    is_saving;      // #READ_ONLY #NO_SAVE #NO_SHOW true if currently saving an object
  static bool           save_use_name_paths; // #READ_ONLY #NO_SAVE #NO_SHOW use name-based paths (GetPathNames) for saving paths
  static ContextFlag    is_undo_saving; // #READ_ONLY #NO_SAVE #NO_SHOW true if currently saving an object for undo data -- objects with extensive "leaf" level data (i.e., having no signficant undoable data under them, e.g., data table rows) should NOT save that data in this context
  static ContextFlag    is_undo_loading;// #READ_ONLY #NO_SAVE #NO_SHOW true if currently loading an object from undo data
  static ContextFlag    is_duplicating; // #READ_ONLY #NO_SAVE #NO_SHOW true if currently duplicating an object
  static ContextFlag    is_changing_type;       // #READ_ONLY #NO_SAVE #NO_SHOW true if currently doing a ChangeType on object
  static ContextFlag    is_checking;    // #READ_ONLY #NO_SAVE #NO_SHOW true if currently doing batch CheckConfig on objects
  static ContextFlag    in_gui_call;    // #READ_ONLY #NO_SAVE #NO_SHOW true if we are running a function call from the gui (used to modalize warning dialogs)
  static ContextFlag    in_gui_multi_action; // #READ_ONLY #NO_SAVE #NO_SHOW we are currently in a gui multiple item action (e.g., drag/drop or cut/paste multiple items) -- good to suspend various update actions that might otherwise occur at this time.  The last item in the sequence does NOT have this flag set, so it can trigger relevant updates etc
  static ContextFlag    in_plugin_init; // #READ_ONLY #NO_SAVE #NO_SHOW true if currently loading typeinfo for a plugin
  static ContextFlag    in_shutdown;    // #READ_ONLY #NO_SAVE #NO_SHOW true if currently shutting down and cleaning up
  static ContextFlag    no_auto_expand; // #READ_ONLY #NO_SAVE #NO_SHOW true to suppress auto-expanding (esp during code that makes a lot of objs)
  static TypeDef*       plugin_loading; // #READ_ONLY #NO_SAVE #NO_SHOW the TypeDef of the plugin currently loading -- we stamp this into all formal classes

  static String         last_err_msg;
  // #READ_ONLY #NO_SAVE #NO_SHOW last message from the taMisc::Error function
  static String         last_warn_msg;
  // #READ_ONLY #NO_SAVE #NO_SHOW last message from the taMisc::Warning function

  static String         last_check_msg; // #READ_ONLY #NO_SAVE #EDIT_DIALOG last error, or last batch of errors (if checking) by CheckConfig
  static bool           check_quiet;    // #IGNORE mode we are in; set by CheckConfigStart
  static bool           check_confirm_success; // #IGNORE mode we are in; set by CheckConfigStart
  static bool           check_ok;       // #IGNORE cumulative AND of all nested oks
  static int            err_cnt; //  #READ_ONLY #NO_SAVE cumulative error count; can be used/reset by Server to detect for errors after it calls a routine
  static int            CheckClearErrCnt(); // gets current value, and clears

  static fstream        log_stream; // #IGNORE current logging output stream -- updated to project name + .plog extension whenever a program is opened or saved with a new name -- all significant events are logged to this stream via logging interface functions below
  static String         log_fname;  // #READ_ONLY #NO_SAVE current log file output name

#if (defined(TA_GUI) && !(defined(__MAKETA__) || defined(NO_TA_BASE)))
  static QPointer<QMainWindow>  console_win;    // #IGNORE the console window
#endif

  static void   (*WaitProc)();
  // #IGNORE set this to a work process for idle time processing
  static bool   do_wait_proc;
  // #IGNORE any case where something is added to the wait processing queue MUST set this flag -- it is reset again at the START of the wait proc so that subsequent stuff within the waitproc can request another waitproc visit the next time through
  static void   (*ScriptRecordingGui_Hook)(bool); // #IGNORE gui callback when script starts/stops; var is 'start'

  /////////////////////////////////////////////////
  //    Configuration -- object as settings

  void  SaveConfig();
  // #CAT_Config save configuration defaults to <appdata>/taconfig file that is loaded automatically at startup
  void  LoadConfig();
  // #CAT_Config load configuration defaults from <appdata>/.taconfig file (which is loaded automatically at startup)
  static void  UpdateAfterEdit();
  // #CAT_Config called before saving and before loading -- updates any derived fields based on current settings

  /////////////////////////////////////////////////
  //    Errors, Warnings, Simple Dialogs

  static String SuperCat(const char* a, const char* b, const char* c,
                      const char* d, const char* e, const char* f,
                      const char* g, const char* h, const char* i);
  // #CAT_Dialog concatenate strings with spaces between

  static void   Error(const char* a, const char* b=0, const char* c=0,
                      const char* d=0, const char* e=0, const char* f=0,
                      const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog displays error either in a window+stderr if gui_active or to stderr only
  static void   Error_nogui(const char* a, const char* b=0, const char* c=0,
                            const char* d=0, const char* e=0, const char* f=0,
                            const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog explicit no-gui version of error: displays error to stderr only

#ifndef NO_TA_BASE
  static bool   TestError(const taBase* obj, bool test, const char* fun_name,
                          const char* a, const char* b=0, const char* c=0,
                          const char* d=0, const char* e=0, const char* f=0,
                          const char* g=0, const char* h=0);
  // #CAT_Dialog if test, then report error, including object name, type, and path information if non-null; returns test -- use e.g. if(taMisc::TestError(this, (condition), "fun", "msg")) return false;
  static bool   TestWarning(const taBase* obj, bool test, const char* fun_name,
                            const char* a, const char* b=0, const char* c=0,
                            const char* d=0, const char* e=0, const char* f=0,
                            const char* g=0, const char* h=0);
  // #CAT_Dialog if test, then report warning, including object name, type, and path information if non-null; returns test -- use e.g. if(taMisc::TestWarning(this, (condition), "fun", "msg")) return false;
#endif

  static void   CheckError(const char* a, const char* b=0, const char* c=0,
                      const char* d=0, const char* e=0, const char* f=0,
                      const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog called by CheckConfig routines; enables batching up of errors for display

  static void   Warning(const char* a, const char* b=0, const char* c=0,
                      const char* d=0, const char* e=0, const char* f=0,
                      const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog displays warning to stderr and/or other logging mechanism

  static void   Info(const char* a, const char* b=0, const char* c=0,
                      const char* d=0, const char* e=0, const char* f=0,
                      const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog displays informative msg to stdout and/or other logging mechanism
  static void   DebugInfo(const char* a, const char* b=0, const char* c=0,
                          const char* d=0, const char* e=0, const char* f=0,
                          const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog displays informative msg to stdout and/or other logging mechanism -- always save to the log, but only displays to stdout if DEBUG mode is active
  static void   LogInfo(const char* a, const char* b=0, const char* c=0,
                        const char* d=0, const char* e=0, const char* f=0,
                        const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog easy method for calling LogEvent with discrete args like the rest of the dialog options -- just cats the args with LOG: tag and sends resulting string to LogEvent

  static int    Choice(const char* text="Choice", const char* a="Ok", const char* b=0,
                       const char* c=0, const char* d=0, const char* e=0,
                       const char* f=0, const char* g=0, const char* h=0,
                       const char* i=0);
  // #CAT_Dialog allows user to choose among different options in a popup dialog window if gui_active (else stdin/out) -- option a returns 0, b = 1, etc.

  static void   Confirm(const char* a, const char* b=0, const char* c=0,
                      const char* d=0, const char* e=0, const char* f=0,
                      const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog displays informative msg in a dialog -- use this instead of Choice for such confirmations

  static void   LogEvent(const String& log_data);
  // #CAT_Log record data to current log stream file -- log all significant data using this (errors and warnings above are logged for example)
  static void   SetLogFile(const String& log_fname);
  // #CAT_Log set log file to given file name -- always overwrites any existing log file

  static void   EditFile(const String& filename);
  // #CAT_Dialog edit the file in the external editor

  /////////////////////////////////////////////////
  //    Global state management

  static void   FlushConsole();
  // #CAT_GlobalState flush any pending console output (cout, cerr) -- call this in situations that generate a lot of console output..

  static int    ProcessEvents();
  // #CAT_GlobalState run any pending qt events that might need processed
  static int    RunPending();
  // #CAT_GlobalState check to see if any events are pending, and run if true -- MUCH faster than processevents, but also likely to miss some events along the way.

  static void   Busy(bool busy = true);
  // #CAT_GlobalState puts system in a 'busy' state
  static inline void    DoneBusy() {Busy(false);}
  // #CAT_GlobalState when no longer busy, call this function

  static void   CheckConfigStart(bool confirm_success = true, bool quiet = false);
  // #CAT_GlobalState we are starting checkconfig, nestable, 1st guy controls params
  static void   CheckConfigEnd(bool ok = true);
  // #CAT_GlobalState ending checkconfig, last exit handles display etc.

  static void   MallocInfo(ostream& strm);
  // #CAT_GlobalState generate malloc memory statistic information to given stream
  static void   ListAllTokens(ostream& strm);
  // #CAT_GlobalState generate a list and count of all types that keep tokens, with a count of tokens
  static TypeInfoKind TypeToTypeInfoKind(TypeDef* typ);
  // #CAT_GlobalState

#ifndef __MAKETA__
  static void   Register_Cleanup(SIGNAL_PROC_FUN_ARG(fun));
  // #IGNORE register a cleanup process in response to all terminal signals
#endif
  static void   Decode_Signal(int err);
  // #IGNORE printout translation of signal on cerr

  /////////////////////////////////////////////////
  //    Startup/Args

  static void   Initialize();
  // #IGNORE very first initialize of type system prior to loading _TA.cpp information (called by ta_TA.cpp file -- hardcoded into maketa

  static void   AddInitHook(init_proc_t init_proc);
  // #IGNORE add an init hook -- invoked by InitProccalled during module initialization, before main()

#ifndef NO_TA_BASE
  static void   Init_Hooks();
  // #IGNORE calls initialization hooks for plugins
  static void   Init_Defaults_PreLoadConfig();
  // #IGNORE sets up default parameters for taMisc settings, prior to loading from config file
  static void   Init_Defaults_PostLoadConfig();
  // #IGNORE sets up default parameters for taMisc settings, after loading from config file
  static void   Init_Args(int argc, const char* argv[]);
  // #IGNORE initialize taMisc startup argument information (note: arg_names must be initialized prior to this!)
  static void   Init_Types();
  // #IGNORE called after all type info has been loaded into types -- calls initClass methods on classes that have them (and possibly other type post-init info)
  static void   Init_DMem(int& argc, const char* argv[]);
  // #IGNORE initialize distributed memory stuff

  static void   HelpMsg(ostream& strm = cerr);
  // #CAT_Args generate a help message about program args, usage, etc

  static void   AddArgName(const String& flag, const String& name);
  // #CAT_Args add an argument flag name to be processed from startup args (e.g., flag = -f, name = CssScript; see arg_names)
  static void   AddEqualsArgName(const String& arg_name);
  // #CAT_Args add an argument that uses equals to set value, as in <arg_name>=<value> -- uses arg_name also for the logical name of the argument for later reference (without the ='s sign)
  static void   AddArgNameDesc(const String& name, const String& desc);
  // #CAT_Args add a description of an argument flag name (see arg_name_descs)

  static void   UpdateArgs();
  // #CAT_Args update arg information after adding new arg names

  static String FullArgString();
  // #CAT_Args return the full string of arguments passed to the program by the user
  static bool   FullArgStringToFile(const String& fname);
  // #CAT_Args write the full string of arguments passed to the program by the user to given file name
  static String FullArgStringName(bool exclude_flags=true, const String& exclude_names="",
                                  bool shorten_names=true, int max_len=6, int seg_len=3,
                                  int rm_vowels_thr=8,
                                  const String& nm_val_sep="_", const String& arg_sep="",
                                  const String& space_repl="", const String& period_repl="",
                                  const String& slash_repl="%");
  // #CAT_Args return user-provided args as a concatenation of name_value pairs, in a form suitable for use as a file name -- exclude_flags = do not include args that didn't have a value passed (tend to be system control flags) -- exclude_names is a comma-separated list of name strings that will be exlcuded (uses contains, so can be part of a name) -- lots of options to control output, including shortening arg names (see ShortName for meaning of max_len and seg_len, and rm_vowels_thr), separators between name and value, and between args (note: for obscure reasons, the separator for an empty string is actually the pound sign), and what to replace spaces, periods and slashes (either direction) with (these are important file-system separators)

  static bool   CheckArgByName(const String& nm);
  // #CAT_Args was the given arg name set?
  static String FindArgByName(const String& nm);
  // #CAT_Args get the value for given named argument (argv[x] for unnamed args)
  static bool   GetAllArgsNamed(const String& nm, String_PArray& vals);
  // #CAT_Args get the values for all args with given name tag
  static bool   CheckArgValContains(const String& vl);
  // #CAT_Args check if there is an arg that contains string fragment in its value
  static String FindArgValContains(const String& vl);
  // #CAT_Args get full arg value that contains string fragment
  static void   AddUserDataSchema(const String& type_name, UserDataItemBase* item);
  // #CAT_UserData adds the item as schema, putting on deferred list if type not avail yet
  static void   AddDeferredUserDataSchema(); // #IGNORE call during init to resolve
protected:
  static String_PArray* deferred_schema_names; // for early startup if type not defined yet
  static UserDataItem_List* deferred_schema_items; // for early startup if type not defined yet
public:
#endif // NO_TA_BASE

  /////////////////////////////////////////////////
  //    Commonly used utility functions on strings/arrays/values

  static void   CharToStrArray(String_PArray& sa, const char* ch);
  // #CAT_Utility convert space-delimeted character string to a string array
  static String StrArrayToChar(const String_PArray& sa);
  // #CAT_Utility convert a string array to a space-delimeted character string

  static void   SpaceLabel(String& lbl);
  // #CAT_Utility add spaces to a label in place of _'s and upper-lower transitions

  static String LeadingZeros(int num, int len);
  // #CAT_Utility returns num converted to a string with leading zeros up to len

  static String FormatValue(float val, int width, int precision);
  // #CAT_Utility format output of value according to width and precision

  static String StringMaxLen(const String& str, int len);
  // #CAT_Utility returns string up to maximum length given (enforces string to be len or less in length)
  static String StringEnforceLen(const String& str, int len);
  // #CAT_Utility returns string enforced to given length (spaces added to make length)
  static String ShortName(const String& full_name, int max_len=6, int seg_len=3, int rm_vowels_thr=8);
  // #CAT_Utility return a shortened version of a full name -- looks for lower-upper case transitions, _'s as ways to segment names, then abbreviates segments to given segment len (only enforced if multiple segments) -- rm_vowels_thr is length above which RemoveVowels is called first
  static String RemoveVowels(const String& str);
  // #CAT_Utility remove vowels from given string -- useful for shortening while still producing legible text

  static String StringCVar(const String& str);
  // #CAT_Utility make return string in a form that would be valid as a variable name in C (i.e., alpha + numeric (not at start) + _

  /////////////////////////////////////////////////
  //    File Paths etc

  static TypeDef* FindTypeName(const String& typ_nm);
  // #CAT_File looks up typedef by name on global list of types, using AKA to find replacement types if original name not found
  static String GetFileFmPath(const String& path);
  // #CAT_File get file name component from full path
  static String GetDirFmPath(const String& path, int n_up = 0);
  // #CAT_File get directory component from full path, n_up is number of directories to go up from the final directory
  static String GetHomePath();
  // #CAT_File get user's home directory path
  static String GetUserPluginDir();
  // #CAT_File get the directory where user plugins are stored (just the dir name, not full path)
  static String GetSysPluginDir();
  // #CAT_File get the directory where system plugins are stored (just the dir name, not full path)
  static bool   FileExists(const String& filename);
  // #CAT_File returns true if the file exists in current working directory (or absolute path)
#ifndef NO_TA_BASE
  static int64_t FileSize(const String& filename);
  // #CAT_File returns size of given file (0 if it does not exist -- see also FileExists)
  static bool   FileWritable(const String& filename);
  // #CAT_File returns true if file is writable according to file system permissions
  static bool   FileReadable(const String& filename);
  // #CAT_File returns true if file is readable according to file system permissions
  static bool   FileExecutable(const String& filename);
  // #CAT_File returns true if file is executable according to file system permissions
  static bool   SetFilePermissions(const String& filename, bool user=true, bool group=false,
                                   bool other=false, bool readable=true, bool writable=true,
                                   bool executable=false);
  // #CAT_File set file permissions for different classes of users
  static bool   RenameFile(const String& old_filename, const String& new_filename);
  // #CAT_File rename file from old to new name in current working directory (or absolute path) -- returns success
  static bool   RemoveFile(const String& filename);
  // #CAT_File remove file with given name in current working directory (or absolute path) -- returns success
  static String GetCurrentPath();
  // #CAT_File get current working directory path
  static bool   SetCurrentPath(const String& path);
  // #CAT_File set current working directory to given path -- returns success
  static bool   MakeDir(const String& dir);
  // #CAT_File make new subdirectory in current working directory -- returns success
  static bool   MakePath(const String& path);
  // #CAT_File make full path relative to current working directory (or absolute path) including all intermediate directories along the way as needed
  static bool   RemoveDir(const String& dir);
  // #CAT_File remove subdirectory in current working directory -- must be empty -- returns success
  static bool   RemovePath(const String& path);
  // #CAT_File remove full path relative to current working directory (or absolute path) including all *empty* intermediate directories along the way -- only removes directories that are empty -- returns success
  static String GetTemporaryPath();
  // #CAT_File return path to system temporary file directory (e.g., /tmp)
#endif
  static String FindFileOnPath(String_PArray& paths, const char* fname);
  // #CAT_File helper function: try to find file fnm in one of the load_include paths -- returns complete path to file (or empty str if not found)

  static String FindFileOnLoadPath(const char* fname);
  // #CAT_File try to find file fnm in one of the load_include paths -- returns complete path to file  (or empty str if not found)

  static int    GetUniqueFileNumber(int st_no, const String& prefix, const String& suffix);
  // #CAT_File get a unique file number by adding numbers in between prefix and suffix until such a file does not exist

  static String FileDiff(const String& fname_a, const String& fname_b,
                         bool trimSpace = false, bool ignoreSpace = false,
                         bool ignoreCase = false);
  // #CAT_File return a string showing the differences between two files -- uses taStringDiff

  static String GetWikiURL(const String& wiki_name, bool add_index=true);
  // #CAT_File get the url for a given wiki name, optionally adding /index.php/ if add_index is true
  static String FixURL(const String& url_str);
  // #CAT_File do some basic things to fix a url to make it at least somewhat viable (e.g., add http:// if no 'scheme' already there, add .com if no . present in an http:// url)

  static bool   InternetConnected();
  // #CAT_File determine if the system has at least one active network interface -- i.e., is it connected to the internet?
  static int    ExecuteCommand(const String& cmd);
  // #CAT_File execute given command -- currently just uses the "system" function call on all platforms, which seems to work well

  /////////////////////////////////////////////////
  //    Recording GUI actions to css script

  static void   StartRecording(ostream* strm);
  // #CAT_Script sets record_strm and record_cursor
  static void   StopRecording();
  // #CAT_Script unsets record_strm and record_cursor
  static bool   RecordScript(const char* cmd);
  // #CAT_Script record the given script command, if the script is open (just sends cmd to stream)
#ifndef NO_TA_BASE
  static void   ScriptRecordAssignment(taBase* tab,MemberDef* md);
  // #CAT_Script record last script assignment of tab's md value;
  static void   SRIAssignment(taBase* tab,MemberDef* md);
  // #CAT_Script record inline md assignment
  static void   SREAssignment(taBase* tab,MemberDef* md);
  // #CAT_Script record enum md assignment
#endif

  ////////////////////////////////////////////////////////////////////////
  //    File Parsing Stuff for Dump routines: Input

  static String LexBuf; // #HIDDEN a buffer, contains last thing read by read_ funs

  // return value is the next character in the stream
  // peek=true means that return value was not read, but was just peek'd

  static int    skip_white(istream& strm, bool peek = false);
  // #CAT_Parse skip over all whitespace
  static int    skip_white_noeol(istream& strm, bool peek = false);
  // #CAT_Parse don't skip end-of-line
  static int    skip_till_start_quote_or_semi(istream& strm, bool peek = false);
  // #CAT_Parse used to seek up to an opening " for a string; will terminate on a ;
  static int    read_word(istream& strm, bool peek = false);
  // #CAT_Parse reads only contiguous 'isalnum' and _ -- does skip_white first
  static int    read_nonwhite(istream& strm, bool peek = false);
  // #CAT_Parse read any contiguous non-whitespace string -- does skip_white first
  static int    read_nonwhite_noeol(istream& strm, bool peek = false);
  // #CAT_Parse read any contiguous non-whitespace string, does skip_white_noeol first (string must be on this line)
  static int    read_till_eol(istream& strm, bool peek = false);
  // #CAT_Parse eol = end of line
  static int    read_till_semi(istream& strm, bool peek = false);
  // #CAT_Parse semi = ;
  static int    read_till_lbracket(istream& strm, bool peek = false);
  // #CAT_Parse lbracket = {
  static int    read_till_lb_or_semi(istream& strm, bool peek = false);
  // #CAT_Parse lb = { or ;
  static int    read_till_rbracket(istream& strm, bool peek = false);
  // #CAT_Parse rbracket = } -- does depth counting to skip over intervening paired { }
  static int    read_till_rb_or_semi(istream& strm, bool peek = false);
  // #CAT_Parse rbracket } or ; -- does depth counting to skip over intervening paired { }
  static int    read_till_end_quote(istream& strm, bool peek = false); // #CAT_Parse
  // #CAT_Parse read-counterpart to write_quoted_string; read-escaping, until "
  static int    read_till_end_quote_semi(istream& strm, bool peek = false);
  // #CAT_Parse read-counterpart to write_quoted_string; read-escaping, until "; (can be ws btwn " and ;)
  static int    skip_past_err(istream& strm, bool peek = false);
  // #CAT_Parse skips to next rb or semi (robust)
  static int    skip_past_err_rb(istream& strm, bool peek = false);
  // #CAT_Parse skips to next rbracket (

  static int    find_not_in_quotes(const String& str, char c, int start = 0);
  // #CAT_Parse find character c in the string, starting at given index (- = from end), making sure that the character is not contained within a quoted string within the overall string

  static int    replace_strings(istream& istrm, ostream& ostrm, NameVar_PArray& repl_list);
  // #CAT_File replace a list of strings (no regexp) in input file istrm to output file ostrm (name -> value) -- reads one line at a time; returns number replaced
  static int    find_strings(istream& istrm, String_PArray& strs);
  // #CAT_File find first occurrence of any of the given strings in file (reading one line at a time); returns index of string or -1 if none found


  ////////////////////////////////////////////////////////////////////////
  //    HTML-style tags

  enum ReadTagStatus {
    TAG_GOT,                    // got a starting tag <xxx...>
    TAG_END,                    // got an ending tag </xx>
    TAG_NONE,                   // no start of < tag there
    TAG_EOF,                    // got an EOF
  };

  static ReadTagStatus read_tag(istream& strm, String& tag, String& val);
  // #CAT_Parse read an html-style tag from the file: <XXX ...> tag = XXX, val = ... (optional)
  static int    read_till_rangle(istream& strm, bool peek = false);
  // #CAT_Parse rangle = >

  ////////////////////////////////////////////////////////////////////////
  //    File Parsing Stuff for Dump routines: Output

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
//      taRefN

class TA_API taRefN {
  // #NO_TOKENS #NO_MEMBERS #NO_CSS reference counting base class
public:
  static void           Ref(taRefN* it) { it->refn.ref(); }
  static void           SafeRef(taRefN* it)     { if(it) it->refn.ref(); }
  static void           Ref(taRefN& it) { it.refn.ref(); }
  static void           unRef(taRefN* it)       { it->refn.deref(); }
  static void           Done(taRefN* it)        { if(it->refn == 0) delete it; }
  static void           unRefDone(taRefN* it)
    {if (!it->refn.deref()) delete it; }
  static void           SafeUnRefDone(taRefN* it) { if(it) unRefDone(it); }
  static void           SetRefDone(taRefN*& var, taRefN* it) // does it fast, but safe, even for var==it
    {if (it) Ref(it); if (var != NULL) unRefDone(var); var = it;}

  taRefN()              {}
protected:
  QAtomicInt            refn;
  virtual ~taRefN()     { }; // all instances should consistently use ref counting for lifetime management
};


// Interfaces

//////////////////////////
//   ITypedObject       //
//////////////////////////

// Contains basic virtual methods for getting type information, and an instance pointer
class TA_API ITypedObject {//#NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE
  // this is the interface available to the host data object
public:
  virtual void*         This() = 0; // reference to the 'this' pointer of the client object
  virtual TypeDef*      GetTypeDef() const = 0; // typedef of the dlc
  virtual ~ITypedObject() {}
};

#define TYPED_OBJECT(T) \
  void* This() {return this;} \
  TypeDef* GetTypeDef() const {return &TA_##T;}

//////////////////////////
//   IDataLinkProxy     //
//////////////////////////

class TA_API IDataLinkProxy : public virtual ITypedObject {//#NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE
public:
#ifndef TA_NO_GUI
  virtual taiDataLink*  link() const = 0;
#else
  virtual taDataLink*   link() const = 0;
#endif
  virtual TypeDef*      GetDataTypeDef() const; // convenience function, default gets data type from link
};

//////////////////////////
//   IDataLinkClient    //
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
  inline taiDataLink*   link() const {return (taiDataLink*)m_link;}
#else
  inline taDataLink*    link() const {return m_link;}
#endif
  inline taDataLink*    link_() const {return m_link;}
  virtual bool          ignoreDataChanged() const {return false;}
    // hidden guys can ignore changes (but they always get Destroyed)
  virtual bool          isDataView() const {return false;} // true for dataviews
  virtual void          DataLinkDestroying(taDataLink* dl) = 0; // called by DataLink when destroying; it will remove datalink ref in dlc upon return
  virtual void          DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) = 0; //
  virtual void          IgnoredDataChanged(taDataLink* dl, int dcr,
    void* op1, void* op2) {} // called instead if ignoreDataChanged was true

  IDataLinkClient() {m_link = NULL;}
  ~IDataLinkClient();
protected:
  virtual bool          AddDataLink(taDataLink* dl);
    // #IGNORE true if added, false if already set (usually a bug); overridden in Multi
  virtual bool          RemoveDataLink(taDataLink* dl);
    // #IGNORE true if removed, false if not (likely not a bug, just redunancy) overridden in Multi

  taDataLink*           m_link; // NOTE: will always be a taiDataLink or subclass
};


class TA_API IMultiDataLinkClient : public virtual IDataLinkClient {//#NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE
public:

  IMultiDataLinkClient() {}
  ~IMultiDataLinkClient();
protected:
  taPtrList<taDataLink> dls;
  override bool         AddDataLink(taDataLink* dl);
  override bool         RemoveDataLink(taDataLink* dl);
};



//////////////////////////
//   taDataLink         //
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

#define DL_FUNS(y)      TypeDef* GetTypeDef() const {return &TA_ ## y;} \


class TA_API IDataLinkClient_PtrList: public taPtrList<IDataLinkClient> {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
public:
    ~IDataLinkClient_PtrList() {} // MAKETA requires at least one member item
};

class TA_API taDataLink {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS #VIRT_BASE interface for taiDataLink and descendants
friend class taDataLinkItr;
public:
  void*                 data() const {return m_data;} // subclasses usually replace with strongly typed version
#ifndef NO_TA_BASE
  taBase*               taData() const
    {if (isBase()) return (taBase*)m_data; else return NULL;}
#endif
  virtual bool          isBase() const {return false;} // true if data is of type taBase (note: could still be null)
  inline int            dbuCnt() const {return m_dbu_cnt;} // batch update: -ve:data, 0:none, +ve:struct
  virtual bool          isEnabled() const {return true;} // status of item

  bool                  AddDataClient(IDataLinkClient* dlc); // true if added, and it had not previously been added (false is probably a bug)
  bool                  RemoveDataClient(IDataLinkClient* dlc); // returns true if removed; false is likely not a bug, just redundancy

  virtual TypeDef*      GetDataTypeDef() const {return NULL;} // TypeDef of the data
  virtual MemberDef*    GetDataMemberDef() const {return NULL;} // if a member in a class, then the MemberDef
  virtual String        GetName() const {return _nilString;}
  virtual String        GetDisplayName() const; // default return Member name if has MemberDef, else GetName
  void                  DataDestroying(); // called by host when destroying, but it is still responsible for deleting us
  virtual void          DataDataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
//na  virtual void              FillContextMenu(BrListViewItem* sender, taiMenu* menu);
//na  virtual void              FillContextMenu_EditItems(BrListViewItem* sender, taiMenu* menu, int allowed) {}
  virtual bool          HasChildItems() {return false;} // used when node first created, to control whether we put a + expansion on it or not

  virtual int           NumListCols() const {return 1;} // number of columns in a list view for this item type
  static const KeyString key_name; // "name" note: also on taBase
  virtual const KeyString GetListColKey(int col) const {return key_name;} // key of default list view
  virtual String        GetColHeading(const KeyString& key) const {return KeyString("Item");}
    // header text for the indicated column
  virtual String        GetColText(const KeyString& key, int itm_idx = -1) const
    {return GetName();} // text for the indicated column
  virtual String        ChildGetColText(taDataLink* child, const KeyString& key,
    int itm_idx = -1) const  {return child->GetColText(key, itm_idx);}
    // default delegates to child; lists can override to control this

  virtual TypeDef*      GetTypeDef() const;
  taDataLink(void* data_, taDataLink* &link_ref_);
  virtual ~taDataLink();
protected:
  void*                 m_data; // subclasses usually replace with strongly typed version
  taDataLink**          m_link_ref; // #IGNORE address of our reference in the data item
  IDataLinkClient_PtrList clients; // clients of this item (ex. either primary, or where it is aliased or linked)
  int                   m_dbu_cnt; // data batch update count; +ve is Structural, -ve is Parameteric only
private:
  void                  DoNotify(int dcr, void* op1_, void* op2_);
    // don't even DREAM of making this non-private!!!!
};

// macros for iterating over the dataclients or views of a data item

class TA_API taDataLinkItr {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS iterator for datalink clients -- use as a value type
public:
  void*         NextEl(taDataLink* dl, const TypeDef* typ);
  void          Reset() {i = 0;}
  taDataLinkItr() {Reset();}
protected:
  int           i;
};

// iterates through a datalink, returning only object refs to objects of indicated type (or descendant)
#define FOR_DLC_EL_OF_TYPE(T, el, dl, itr) \
for(itr.Reset(), el = (T*) itr.NextEl(dl, &TA_ ## T); el; el = (T*) itr.NextEl(dl, &TA_ ## T))


//////////////////////////
//   EnumSpace          //
//////////////////////////

class TA_API EnumSpace : public taPtrList<EnumDef> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS space of enums
protected:
  String        GetListName_() const            { return name; }
  String        El_GetName_(void* it) const;
  TALPtr        El_GetOwnerList_(void* it) const;
  void*         El_SetOwner_(void* it);
  void          El_SetIndex_(void* it, int i);

  void*         El_Ref_(void* it);
  void*         El_unRef_(void* it);
  void          El_Done_(void* it);
  void*         El_MakeToken_(void* it);
  void*         El_Copy_(void* trg, void* src);

public:
  String        name;           // of the space
  TypeDef*      owner;          // owner is a typedef
  taDataLink*   data_link;

  void          Initialize()    { owner = NULL; data_link = NULL;}
  EnumSpace()                   { Initialize(); }
  EnumSpace(const EnumSpace& cp) { Initialize(); Borrow(cp); }
  ~EnumSpace();

  void operator=(const EnumSpace& cp)   { Borrow(cp); }

  // adding manages the values of the enum-values
  void                  Add(EnumDef* it);
  virtual EnumDef*      Add(const char* nm, const char* dsc="", const char* op="",
                            int eno=0);

  virtual EnumDef*      FindNo(int eno) const;
  // finds for a given enum_no

  virtual ostream&      OutputType(ostream& strm, int indent = 1) const;
};


//////////////////////////
//   TokenSpace         //
//////////////////////////

class TA_API TokenSpace : public taPtrList<void> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
protected:
  String        GetListName_() const            { return name; }
  String        El_GetName_(void* it) const;

public:
  static String tmp_el_name;    // for element names that need to be created

  String        name;           // of the space
  TypeDef*      owner;          // owner is a typedef
  bool          keep;           // true if tokens are kept
  QAtomicInt    sub_tokens;     // number of tokens in sub-types
  taDataLink*   data_link;

  virtual void  Initialize();
  TokenSpace()                          { Initialize(); }
  TokenSpace(const TokenSpace& cp)      { Initialize(); Borrow(cp); }
  ~TokenSpace();
  void operator=(const TokenSpace& cp)  { Borrow(cp); }

  void          List(ostream& strm=cout) const;
};


//////////////////////////
//   MemberSpace        //
//////////////////////////

class TA_API Member_List: public taPtrList<MemberDef> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS simple list of members
public:
  Member_List() {}
};

class TA_API MemberDefBase_List: public taPtrList<MemberDefBase> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS common subtype for Member and PropertySpace
public:
  String        name;           // of the space
  TypeDef*      owner;          // owner is a typedef
  taDataLink*   data_link;

  MemberDefBase_List() {Initialize();}
  ~MemberDefBase_List();
protected:
  String        GetListName_() const            { return name; }
  String        El_GetName_(void* it) const;
  TALPtr        El_GetOwnerList_(void* it) const;
  void*         El_SetOwner_(void* it);
  void          El_SetIndex_(void* it, int i);

  void*         El_Ref_(void* it);
  void*         El_unRef_(void* it);
  void          El_Done_(void* it);
  void*         El_MakeToken_(void* it); // makes a proper token of the concrete type
  void*         El_Copy_(void* trg, void* src); // note: copies like guys correctly

private:
  void          Initialize()            { owner = NULL; data_link = NULL;}
};

class TA_API MemberSpace: public MemberDefBase_List {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS space of members
INHERITED(MemberDefBase_List)
public:
  MemberSpace()                         {  }
  MemberSpace(const MemberSpace& cp)    { Borrow(cp); }

  void operator=(const MemberSpace& cp) { Borrow(cp); }

  MemberDef*    FindCheck(const char* nm, void* base, void*& ptr) const;
  // breadth-first find pass for the recursive procedures

  int           FindNameOrType(const char* nm) const;
  // checks name and type name in 2 passes
  int           FindTypeName(const char* nm) const;
  // find by name of type
  MemberDef*    FindNameR(const char* nm) const;
  // recursive find of name (or type name)
  MemberDef*    FindNameAddr(const char* nm, void* base, void*& ptr) const;
  // find of name returning address of found member

  MemberDef*    FindType(TypeDef* it) const;
  // find by type, inherits from
  MemberDef*    FindTypeR(TypeDef* it) const;
  // recursive find of type
  MemberDef*    FindTypeAddr(TypeDef* it, void* base, void*& ptr) const;
  // find of type returning address of found member

  int           FindDerives(TypeDef* it) const;
  MemberDef*    FindTypeDerives(TypeDef* it) const;
  // find by type, derives from

  MemberDef*    FindAddr(void* base, void* mbr, int& idx) const;
  // find by address given base of class and address of member
  int           FindPtr(void* base, void* mbr) const;
  MemberDef*    FindAddrPtr(void* base, void* mbr, int& idx) const;
  // find by address of a member that is a pointer given base and pointer addr

  void          CopyFromSameType(void* trg_base, void* src_base);
  // copy all members from class of the same type as me
  void          CopyOnlySameType(void* trg_base, void* src_base);
  // copy only those members in my type (no inherited ones)

  bool          CompareSameType(Member_List& mds, TypeSpace& base_types,
                                voidptr_PArray& trg_bases, voidptr_PArray& src_bases,
                                TypeDef* base_typ, void* trg_base, void* src_base,
                                int show_forbidden = taMisc::NO_HIDDEN,
                                int show_allowed = taMisc::SHOW_CHECK_MASK,
                                bool no_ptrs = true, bool test_only = false);
  // compare all member values from class of the same type as me, adding ones that are different to the mds, trg_bases, src_bases lists (unless test_only == true, in which case it just does the tests and returns true if any diffs -- for inline objects)

  // IO
  ostream&      OutputType(ostream& strm, int indent = 0) const;

  ostream&      Output(ostream& strm, void* base, int indent) const;
  ostream&      OutputR(ostream& strm, void* base, int indent) const;

  // for dump files
  int           Dump_Save(ostream& strm, void* base, void* par, int indent);
  int           Dump_SaveR(ostream& strm, void* base, void* par, int indent);
  int           Dump_Save_PathR(ostream& strm, void* base, void* par, int indent);

  int           Dump_Load(istream& strm, void* base, void* par,
                                  const char* prv_read_nm = NULL, int prv_c = 0); //

public: // lexical hacks
  inline MemberDef*     operator[](int i) const {return (MemberDef*)inherited::FastEl(i);}
  inline MemberDef*     FastEl(int i) const {return (MemberDef*)inherited::FastEl(i);}
  inline MemberDef*     SafeEl(int i) {return (MemberDef*)inherited::SafeEl(i);}
  inline MemberDef*     PosSafeEl(int i) {return (MemberDef*)inherited::PosSafeEl(i);}
  //#IGNORE
  inline MemberDef*     FindName(const String& item_nm) const
    {return (MemberDef*)inherited::FindName(item_nm);}
};


//////////////////////////
//   PropertySpace      //
//////////////////////////

class TA_API PropertySpace: public MemberDefBase_List {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS space of properties -- note: will hold PropertyDef (owned) and MemberDef (linked) objects
INHERITED(MemberDefBase_List)
public:
  PropertySpace()                               { }
  PropertySpace(const PropertySpace& cp)        { Borrow(cp); }

  void operator=(const PropertySpace& cp)       { Borrow(cp); }

  int                   FindNameOrType(const char* nm) const;
  // checks name and type name in 2 passes
  int                   FindTypeName(const char* nm) const;
  // find by name of type
  MemberDefBase*                FindNameR(const char* nm) const;
  // recursive find of name (or type name)
#ifdef NO_TA_BASE
  PropertyDef*          AssertProperty(const char* nm, bool& is_new,
    bool get_nset, MemberDef* mbr)
    {return AssertProperty_impl(nm, is_new, get_nset, mbr, NULL);}
  // assert property, supplying accessor
  PropertyDef*          AssertProperty(const char* nm, bool& is_new,
    bool get_nset, MethodDef* mth)
    {return AssertProperty_impl(nm, is_new, get_nset, NULL, mth);}
  // assert property, supplying accessor
protected:
  PropertyDef*          AssertProperty_impl(const char* nm, bool& is_new,
    bool get_nset, MemberDef* mbr, MethodDef* mth);
#endif
};


//////////////////////////
//   MethodSpace        //
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
  String        GetListName_() const            { return name; }
  String El_GetName_(void* it) const;
  TALPtr        El_GetOwnerList_(void* it) const;
  void*         El_SetOwner_(void* it);
  void          El_SetIndex_(void* it, int i);

  void*         El_Ref_(void* it);
  void*         El_unRef_(void* it);
  void          El_Done_(void* it);
  void*         El_MakeToken_(void* it);
  void*         El_Copy_(void* trg, void* src);

public:
  String        name;           // of the space
  TypeDef*      owner;          // owner is a typedef
  taDataLink*   data_link;

  void          Initialize()            { owner = NULL; data_link = NULL;}
  MethodSpace()                         { Initialize(); }
  MethodSpace(const MethodSpace& cp)    { Initialize(); Borrow(cp); }
  ~MethodSpace();

  void operator=(const MethodSpace& cp) { Borrow(cp); }

  bool          AddUniqNameNew(MethodDef* it);

  MethodDef*    FindAddr(ta_void_fun funa, int& idx) const;
  // find fun by addr, idx is actual index in method space
  MethodDef*    FindOnListAddr(ta_void_fun funa, const String_PArray& lst, int& lidx) const;
  // find fun on given list by addr, lidx is 'index' of funs on same list
  MethodDef*    FindOnListIdx(int lidx, const String_PArray& lst) const;
  // find fun on given list by index, as given by FindOnListAddr()
  MethodDef*    FindVirtualBase(MethodDef* it, int& idx);
    // find the virtual method with same name and signature

  // IO
  virtual ostream&      OutputType(ostream& strm, int indent = 0) const;

};


//////////////////////////
//   TypeSpace          //
//////////////////////////

class TA_API TypeSpace: public taPtrList<TypeDef> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS space of types; uses default string-based hashing
protected:
  String        GetListName_() const            { return name; }
  String        El_GetName_(void* it) const;
  TALPtr        El_GetOwnerList_(void* it) const;
  void*         El_SetOwner_(void* it);
  void          El_SetIndex_(void* it, int i);

  void*         El_Ref_(void* it);
  void*         El_unRef_(void* it);
  void          El_Done_(void* it);
  void*         El_MakeToken_(void* it);
  void*         El_Copy_(void* trg, void* src);

public:
  String        name;           // of the space
  TypeDef*      owner;          // owner is a typedef
  taDataLink*   data_link;

  void          Initialize()            { owner = NULL; data_link = NULL;}

  TypeSpace()                           { Initialize(); }
  TypeSpace(const char* nm)             { Initialize(); name = nm; }
  TypeSpace(const char* nm, int hash_sz) { Initialize(); name = nm; BuildHashTable(hash_sz); }
  TypeSpace(const TypeSpace& cp)        { Initialize(); Borrow(cp); }
  ~TypeSpace();

  void operator=(const TypeSpace& cp)   { Borrow(cp); }

  TypeDef*      FindTypeR(const String& fqname) const; // find a possibly nested subtype based on :: name
  virtual bool  ReplaceLinkAll(TypeDef* ol, TypeDef* nw);
  virtual bool  ReplaceParents(const TypeSpace& ol, const TypeSpace& nw);
  // replace any parents on the old list with those on the new for all types

  virtual void  ListAllTokens(ostream& strm);
  // list count for all types that are keeping tokens
};


//////////////////////////
//   TypeItem           //
//////////////////////////

class TA_API TypeItem: public taRefN, public ITypedObject {
  // ##INSTANCE ##NO_TOKENS ##NO_MEMBERS ##NO_CSS ##MEMB_NO_SHOW_TREE base class for TypeDef, MemberDef, MethodDef, EnumDef, and TypedefDef
INHERITED(taRefN)
public:
  enum ShowContext {
    SC_ANY,             // any context -- directives like "SHOW"
    SC_EDIT,            // for in edit dialogs -- directives like "SHOW_EDIT"
    SC_TREE             // in tree views (browsing) -- directives like "SHOW_TREE"
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
  static const String opt_NO_APPLY_IMMED; // "NO_APPLY_IMMED"
  static const String opt_inline; // "INLINE"
  static const String opt_edit_inline; // "EDIT_INLINE"
  static const String opt_EDIT_DIALOG; // "EDIT_DIALOG"

  static const String opt_bits; // "BITS"
  static const String opt_instance; // "INSTANCE"

  int           idx;            // the index number for this type
  String        name;
  String        desc;           // a description
  String_PArray opts;           // user-spec'd options (#xxx)
  String_PArray lists;          // user-spec'd lists   (#LIST_xxx)
  taDataLink*   data_link;

  override void*        This() {return this;}
  override TypeDef*     GetTypeDef() const {return &TA_void;}
  virtual taMisc::TypeInfoKind typeInfoKind() const {return taMisc::TIK_UNKNOWN;}
  virtual TypeDef*      GetOwnerType() const {return NULL;}
  virtual const String  GetPathName() const {return name;}
    // name used for saving a reference in stream files, can be used to lookup again

  void          Copy(const TypeItem& cp);
  bool          HasOption(const String& op) const { return (opts.FindEl(op) >= 0); }
    // check if option is set
  bool          HasOptionAfter(const String& prefix, const String& op) const;
    // returns true if any prefix (Xxx_) has the value after of op; enables multi options of same prefix
  virtual String        OptionAfter(const String& prefix) const;
  // return portion of option after given option header
  virtual bool          NextOptionAfter(const String& prefix, int& itr, String& result) const; // enables enumeration of multi-valued prefixes; start w/ itr=0; true if a new value was returned in result
  virtual String        GetLabel() const;
  // checks for option of LABEL_xxx and returns it or name
  virtual String        GetCat() const { return OptionAfter("CAT_"); }
  // get category of this item, from CAT_ option -- empty if not specified

  virtual String        GetOptsHTML() const;
  // get options in HTML format
  virtual bool          GetCondOpt(const String condkey, const TypeDef* base_td, const void* base,
                                   bool& is_on, bool& val_is_eq) const;
  // return true if item has the condkey (e.g., CONDEDIT, CONDSHOW, GHOST), and if so, sets is_on if the cond type is ON (else OFF) and if the match value was in the list (val_is_eq) -- format: condkey_[ON|OFF]_member[:value{,value}[&&,||member[:value{,value}...]] -- must all be && or || for logic -- base and base_td refer to the the parent object that owns this one -- used for searching for the member to conditionalize on
  virtual bool          GetCondOptTest(const String condkey, const TypeDef* base_td, const void* base) const;
  // returns true if item does NOT have condkey (e.g., CONDEDIT, CONDSHOW, GHOST), or the condition evaluates to true, otherwise false -- this is a simpler interface than GetCondOpt, suitable for most uses -- format: condkey_[ON|OFF]_member[:value{,value}[&&,||member[:value{,value}...]] -- must all be && or || for logic  -- base and base_td refer to the the parent object that owns this one -- used for searching for the member to conditionalize on

  TypeItem();
  TypeItem(const TypeItem& cp); // copy constructor
  ~TypeItem();

private:
  void          init(); // #IGNORE
  void          Copy_(const TypeItem& cp);
};

class TA_API EnumDef : public TypeItem { //  defines an enum member
INHERITED(TypeItem)
public:
  EnumSpace*    owner;          // the owner of this one

  int           enum_no;        // number (value) of the enum


  override void*        This() {return this;}
  override TypeDef*     GetTypeDef() const {return &TA_EnumDef;}
  taMisc::TypeInfoKind typeInfoKind() const {return taMisc::TIK_ENUM;}

  void          Copy(const EnumDef& cp);

  EnumDef();
  EnumDef(const char* nm);
  EnumDef(const char* nm, const char* dsc, int eno, const char* op, const char* lis);
  EnumDef(const EnumDef& cp);
  EnumDef*      Clone()         { return new EnumDef(*this); }
  EnumDef*      MakeToken()     { return new EnumDef(); }

  TypeDef*      GetOwnerType() const
  { TypeDef* rval=NULL; if((owner) && (owner->owner)) rval=owner->owner; return rval; }
  bool          CheckList(const String_PArray& lst) const;
  // check if enum has a list in common with given one
private:
  void          Initialize();
  void          Copy_(const EnumDef& cp);
};


class TA_API MemberDefBase : public TypeItem { // #VIRT_BASE #NO_INSTANCE common subclass of MemberDef and PropertyDef
INHERITED(TypeItem)
public:
  MemberDefBase_List*   owner;
  TypeDef*              type;     // of this item
  String_PArray         inh_opts; // inherited options ##xxx
  bool                  is_static; // true if this member is static
#ifdef TA_GUI
  taiMember*            im;             // gui structure for edit representation -- if this is a memberdef that is the storage for a property, then the im is assigned to the property
#endif

  virtual bool          isReadOnly() const = 0; // absolutely read-only
  virtual bool          isGuiReadOnly() const = 0; // read-only in the gui

  override void*        This() {return this;}
  override TypeDef*     GetTypeDef() const {return &TA_MemberDefBase;}
  virtual bool          ValIsDefault(const void* base,
    int for_show = taMisc::IS_EXPERT) const = 0; // true if the member contains its default value, either DEF_ or the implicit default; for_show is only for types, to choose which members to recursively include; we are usually only interested in Expert guys

  void          Copy(const MemberDefBase& cp);
  void          Copy(const MemberDefBase* cp); // this is a "pseudo-virtual" type guy, that will copy a like source (Member or Property)
  override TypeDef*     GetOwnerType() const
  { TypeDef* rval=NULL; if((owner) && (owner->owner)) rval=owner->owner; return rval; }
  MemberDefBase();
  MemberDefBase(const char* nm);
  MemberDefBase(TypeDef* ty, const char* nm, const char* dsc, const char* op,
    const char* lis, bool is_stat = false);
  MemberDefBase(const MemberDefBase& cp);
  ~MemberDefBase();

  virtual const Variant GetValVar(const void* base) const = 0;
  virtual void  SetValVar(const Variant& val, void* base,
    void* par = NULL) = 0;

  bool          CheckList(const String_PArray& lst) const;
  // check if member has a list in common with given one

  bool          ShowMember(int show_forbidden = taMisc::USE_SHOW_GUI_DEF,
    TypeItem::ShowContext show_context = TypeItem::SC_ANY,
    int show_allowed = taMisc::SHOW_CHECK_MASK) const;
  // decide whether to output or not based on options (READ_ONLY, HIDDEN, etc)

protected:
  // note: bits in the show* vars are set to indicate the value, ie READ_ONLY has that bit set
  mutable byte  show_any; // bits for show any -- 0 indicates not determined yet, 0x80 is flag
  mutable byte  show_edit;
  mutable byte  show_tree;

  void          ShowMember_CalcCache() const; // called when show_any=0, ie, not configured yet
  void          ShowMember_CalcCache_impl(byte& show, const String& suff) const;
private:
  void          Initialize();
  void          Copy_(const MemberDefBase& cp);
};


class TA_API MemberDef : public MemberDefBase { //  defines a class member
INHERITED(MemberDefBase)
public:
  enum DefaultStatus { //#BITS  status of default value comparison
    HAS_DEF     = 0x01, // member specified a default value
     IS_DEF     = 0x02, // member's value is the default

     NO_DEF     = 0x00, // #NO_BIT none defined
    NOT_DEF     = 0x01, // #NO_BIT default specified, current is not equal
    EQU_DEF     = 0x03, // #NO_BIT default specified, current is default
  };

  static void           GetMembDesc(MemberDef* md, String& dsc_str, String indent); // gets a detailed description, typically for tooltip

  ta_memb_ptr   off;            // offset of member from owner type
  int           base_off;       // offset for base of owner
  void*         addr;           // address of static member
  bool          fun_ptr;        // true if this is a pointer to a function

  override bool         isReadOnly() const;
  override bool         isGuiReadOnly() const;

  override void*        This() {return this;}
  override TypeDef*     GetTypeDef() const {return &TA_MemberDef;}
  taMisc::TypeInfoKind typeInfoKind() const {return taMisc::TIK_MEMBER;}

  override bool ValIsDefault(const void* base,
    int for_show = taMisc::IS_EXPERT) const; // true if the member contains its default value, either DEF_ or the implicit default; for_show is only for types, to choose which members to recursively include; we are usually only interested in Expert guys

  void          Copy(const MemberDef& cp);
  MemberDef();
  MemberDef(const char* nm);
  MemberDef(TypeDef* ty, const char* nm, const char* dsc, const char* op, const char* lis,
            ta_memb_ptr mptr, bool is_stat = false, void* maddr=NULL, bool funp = false);
  MemberDef(const MemberDef& cp);
  ~MemberDef();
  MemberDef*    Clone()         { return new MemberDef(*this); }
  MemberDef*    MakeToken()     { return new MemberDef(); }

  void*                 GetOff(const void* base) const;
  // get offset of member relative to overall class base pointer
  static void*          GetOff_static(const void* base, int base_off_, ta_memb_ptr off_);
  // get offset of member -- static version that takes args
  override const String GetPathName() const;
    // name used for saving a reference in stream files, can be used to lookup again

  override const Variant GetValVar(const void* base) const;
  override void SetValVar(const Variant& val, void* base, void* par = NULL);
    // note: par is only needed really needed for owned taBase ptrs)

  DefaultStatus         GetDefaultStatus(const void* base);
  // get status of value of member at given base addr of class object that this member is in compared to DEF_ value(s) defined in directive

  void          CopyFromSameType(void* trg_base, void* src_base);
  // copy all members from same type
  void          CopyOnlySameType(void* trg_base, void* src_base);
  // copy only those members from same type (no inherited)
  bool          CompareSameType(Member_List& mds, TypeSpace& base_types,
                                voidptr_PArray& trg_bases, voidptr_PArray& src_bases,
                                TypeDef* base_typ, void* trg_base, void* src_base,
                                int show_forbidden = taMisc::NO_HIDDEN,
                                int show_allowed = taMisc::SHOW_CHECK_MASK,
                                bool no_ptrs = true, bool test_only = false);
  // compare all member values from class of the same type as me, adding ones that are different to the mds, trg_bases, src_bases lists (unless test_only == true, in which case it just does the tests and returns true if any diffs -- for inline objects)

  ostream&      OutputType(ostream& strm, int indent = 1) const;

  ostream&      Output(ostream& strm, void* base, int indent) const;
  ostream&      OutputR(ostream& strm, void* base, int indent) const;

  String        GetHTML(bool gendoc=false, bool short_fmt=false) const;
  // gets an HTML representation of this object -- for help view etc -- gendoc = external html file rendering instead of internal help browser, short_fmt = no details, for summary guys

  // for dump files
  bool          DumpMember(void* par);          // decide whether to dump or not
  int           Dump_Save(ostream& strm, void* base, void* par, int indent);
  int           Dump_SaveR(ostream& strm, void* base, void* par, int indent);
  int           Dump_Save_PathR(ostream& strm, void* base, void* par, int indent);

  int           Dump_Load(istream& strm, void* base, void* par); //
private:
  void          Initialize();
  void          Copy_(const MemberDef& cp);
};


class TA_API PropertyDef : public MemberDefBase { //  defines a class member
INHERITED(MemberDefBase)
public:

#ifdef NO_TA_BASE
  MemberDef*            get_mbr; // if a member getter found
  MethodDef*            get_mth; // if a method getter found
  MemberDef*            set_mbr; // if a member setter found
  MethodDef*            set_mth; // if a method setter found
#endif
  ta_prop_get_fun       prop_get; // stub function to get the property (as Variant)
  ta_prop_set_fun       prop_set; // stub function to set the property (as Variant)

  override bool         isReadOnly() const;
  override bool         isGuiReadOnly() const;
  void                  setType(TypeDef* typ); // use this, to check for consistency between the various source -- should NOT be null!
  taMisc::TypeInfoKind typeInfoKind() const {return taMisc::TIK_PROPERTY;}

  override void*        This() {return this;}
  override TypeDef*     GetTypeDef() const {return &TA_PropertyDef;}
  override bool         ValIsDefault(const void* base,
    int for_show = taMisc::IS_EXPERT) const; // true if the member contains its default value, either DEF_ or the implicit default; for_show is only for types, to choose which members to recursively include; we are usually only interested in Expert guys

  void                  Copy(const PropertyDef& cp);
  PropertyDef();
  PropertyDef(const char* nm);
  PropertyDef(TypeDef* ty, const char* nm, const char* dsc, const char* op,
    const char* lis, ta_prop_get_fun get, ta_prop_set_fun set,
    bool is_stat = false);
  PropertyDef(const PropertyDef& cp);
  ~PropertyDef();
  PropertyDef*  Clone()         { return new PropertyDef(*this); }
  PropertyDef*  MakeToken()     { return new PropertyDef(); }

  override const Variant GetValVar(const void* base) const;
  override void SetValVar(const Variant& val, void* base, void* par = NULL);
    // note: par is only needed really needed for owned taBase ptrs)

  String        GetHTML(bool gendoc=false, bool short_fmt=false) const;
  // gets an HTML representation of this object -- for help view etc -- gendoc = external html file rendering instead of internal help browser, short_fmt = no details, for summary guys

private:
  void          Initialize();
  void          Copy_(const PropertyDef& cp);
};


class TA_API MethodDef : public TypeItem {// defines a class method
#ifndef __MAKETA__
typedef TypeItem inherited;
#endif
public:
  MethodSpace*  owner;

  TypeDef*      type;           // of the return value
  bool          is_static;      // true if this method is static
  bool          is_virtual;     // true if virtual (1st or subsequent overrides)
  bool          is_override;    // true if (virtual) override of a base
  bool          is_lexhide;     // true if lexically hides a non-virtual base -- could be an error
  ta_void_fun   addr;           // address (only for static or reg_fun functions)
  String_PArray inh_opts;       // inherited options ##xxx
#ifdef TA_GUI
  taiMethod*    im;             // gui structure for edit representation
#endif // def TA_GUI
  short         fun_overld;     // number of times function is overloaded (i.e., diff args)
  short         fun_argc;       // nofun, or # of parameters to the function
  short         fun_argd;       // indx for start of the default args (-1 if none)
  TypeSpace     arg_types;      // argument types
  String_PArray arg_names;      // argument names
  String_PArray arg_defs;       // argument default values
  String_PArray arg_vals;       // argument values (previous)

  css_fun_stub_ptr stubp;       // css function stup pointer

  taMisc::TypeInfoKind typeInfoKind() const {return taMisc::TIK_METHOD;}

  const String          prototype() const; // text depiction of fun, ex "void MyFun(int p)"
  override void*        This() {return this;}
  override TypeDef*     GetTypeDef() const {return &TA_MethodDef;}
  void          Initialize();
  void          Copy(const MethodDef& cp);
  MethodDef();
  MethodDef(const char* nm);
  MethodDef(TypeDef* ty, const char* nm, const char* dsc, const char* op, const char* lis,
            int fover, int farc, int fard, bool is_stat = false, ta_void_fun funa = NULL,
            css_fun_stub_ptr stb = NULL, bool is_virt = false);
  MethodDef(const MethodDef& md);       // copy constructor
  ~MethodDef();

  MethodDef*            Clone()         { return new MethodDef(*this); }
  MethodDef*            MakeToken()     { return new MethodDef(); }
  override TypeDef*     GetOwnerType() const
    { TypeDef* rval=NULL; if((owner) && (owner->owner)) rval=owner->owner; return rval; }
  override const String GetPathName() const;
  bool                  CheckList(const String_PArray& lst) const;
  // check if method has a list in common with given one
  bool                  CompareArgs(MethodDef* it) const;       // true if same, false if not
  ostream&              OutputType(ostream& strm, int indent = 1) const;
  void                  CallFun(void* base) const;
  // call the function, using gui dialog if need to get args
  const String          ParamsAsString() const; // returns what would be in () for a definition
  bool                  ShowMethod(taMisc::ShowMembs show = taMisc::USE_SHOW_GUI_DEF) const;

  String        GetHTML(bool gendoc=false, bool short_fmt=false) const;
  // gets an HTML representation of this object -- for help view etc -- gendoc = external html file rendering instead of internal help browser, short_fmt = no details, for summary guys

protected:
  mutable byte  show_any; // bits for show any -- 0 indicates not determined yet, 0x80 is flag
  void          ShowMethod_CalcCache() const; // called when show_any=0, ie, not configured yet
  void          ShowMethod_CalcCache_impl(byte& show) const;
};

class taBase_List;

#define IF_ENUM_STRING(enm_var, enm_val) \
((enm_var == enm_val) ? #enm_val : "")


class TA_API TypeDef : public TypeItem {// defines a type itself
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
  taiType*      it;             // single glyph representation of type (was 'iv')
  taiEdit*      ie;             // editing window rep. of type (was 'ive')
  taiViewType*  iv;             // browser representation of type
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

  bool          is_enum() const; // true if an enum
  bool          is_class() const; // true if it is a class
  bool          is_anchor() const; // true if this is the non-ptr, non-ref, non-const type
  bool          isVarCompat() const; // true if read/write compatible with Variant
  taMisc::TypeInfoKind typeInfoKind() const {return taMisc::TIK_TYPE;}

  override void*        This() {return this;}
  override TypeDef*     GetTypeDef() const {return &TA_TypeDef;}
  void          Copy(const TypeDef& cp);
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

  // inheritance from a formal class (e.g. const, static, class)
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

  static MemberDef* FindMemberPathStatic(TypeDef*& own_td, int& net_base_off,
                                         ta_memb_ptr& net_mbr_off,
                                         const String& path, bool warn = true);
  // you must supply the initial own_td as starting type -- looks for a member or sequence of members based on static type information for members (i.e., does not walk the structural tree and cannot go into lists or other containers, but can find any static paths for object members and their members, etc) -- if warn, emits warning message for bad paths -- net offsets provide overall offset from original own_td obj

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
  void*         GetInstance() const
  { void* rval=NULL; if(instance != NULL) rval = *instance; return rval; }
  int           FindTokenR(void* addr, TypeDef*& ptr) const;
  int           FindTokenR(const char* nm, TypeDef*& ptr) const;
  // recursive search for token among children
  void          AddUserDataSchema(UserDataItemBase* item); // adds the item as schema
#endif
  // for token management
//  void                Register(void* it); // legacy, typically called once per ctor in inheritance chain
//  void                unRegister(void* it); // legacy, typically called once per dtor in inheritance chain
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

  // IO
  ostream&      Output(ostream& strm, void* base, int indent=0) const;
  // output value information for display purposes
  ostream&      OutputR(ostream& strm, void* base, int indent=0) const;
  // output value information for display purposes, recursive

  ostream&      OutputType(ostream& strm, int indent = 0) const;
  // output type information only
  ostream&      OutputInherit(ostream& strm) const;
  ostream&      OutputInherit_impl(ostream& strm) const;

  String        GetHTML(bool gendoc=false) const;
  // gets an HTML representation of this type -- for help view etc -- gendoc = external html file rendering instead of internal help browser
  String        GetHTMLLink(bool gendoc=false) const;
  // get HTML code for a link to this type -- only generates a link if InheritsNonAtomicClass -- otherwise it just returns the Get_C_Name representation
  String        GetHTMLSubType(bool gendoc=false, bool short_fmt=false) const;
  // gets an HTML representation of a sub type (typdef or enum) -- for help view etc -- gendoc = external html file rendering instead of internal help browser, short_fmt = no details, for summary guys

  void          GetObjDiffVal(taObjDiff_List& odl, int nest_lev, const void* base,
                        MemberDef* memb_def=NULL, const void* par=NULL, TypeDef* par_typ=NULL,
                        taObjDiffRec* par_od=NULL) const;
  // add this object and all its members (if a class) to the object diff list
  void          GetObjDiffVal_class(taObjDiff_List& odl, int nest_lev, const void* base,
                    MemberDef* memb_def=NULL, const void* par=NULL, TypeDef* par_typ=NULL,
                    taObjDiffRec* par_od=NULL) const;
  // just add members of a class object to the diff list

  // saving and loading of type instances to/from streams
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

  TypeDef*      FindTypeWithMember(const char* nm, MemberDef** md); // returns the type or child type with memberdef md
protected:
  mutable signed char    m_cacheInheritsNonAtomicClass;
private:
  void          Initialize();
  void          Copy_(const TypeDef& cp);
};

class TA_API taObjDiffRecExtra  {
  // ##INSTANCE ##NO_TOKENS ##NO_MEMBERS ##NO_CSS ##MEMB_NO_SHOW_TREE extra data for TA object difference record
public:
  virtual TypeDef* GetTypeDef() { return NULL; } // subclasses should put correct val here

  taObjDiffRecExtra() { };
  virtual ~taObjDiffRecExtra() { };
};

class TA_API taObjDiffRec : public taRefN {
  // ##INSTANCE ##NO_TOKENS ##NO_MEMBERS ##NO_CSS ##MEMB_NO_SHOW_TREE TA object difference record -- records information about objects for purposes of diffing object structures
INHERITED(taRefN)
public:
  enum DiffFlags {              // #BITS
    DF_NONE  = 0x000000,        // nothing
    DIFF_DEL = 0x000001,        // diff edit = delete from src_a
    DIFF_ADD = 0x000002,        // diff edit = add from src_b to src_a
    DIFF_CHG = 0x000004,        // diff edit = change from a to b
    DIFF_PAR_A = 0x000008,      // parent on A branch of lower-level diff, but not itself different
    DIFF_PAR_B = 0x000010,      // parent on B branch of lower-level diff, but not itself different
    ACT_DEL_A  = 0x000100,      // action to take: delete obj from a
    ACT_DEL_B  = 0x000200,      // action to take: delete obj from b
    ACT_ADD_A  = 0x000400,      // action to take: add obj to a after paired diff_odr
    ACT_ADD_B  = 0x000800,      // action to take: add obj to b after paired diff_odr
    ACT_COPY_AB = 0x001000,     // action to take: copy from a to paired diff_odr b
    ACT_COPY_BA = 0x002000,     // action to take: copy from paired diff_odr b to a
#ifndef __MAKETA__
    DIFF_MASK = DIFF_DEL | DIFF_ADD | DIFF_CHG,
    DIFF_ADDEL = DIFF_DEL | DIFF_ADD,
    DIFF_PAR = DIFF_PAR_A | DIFF_PAR_B,
    ACT_MASK = ACT_DEL_A | ACT_DEL_B | ACT_ADD_A | ACT_ADD_B | ACT_COPY_AB | ACT_COPY_BA,
#endif
    SUB_NO_ACT = 0x010000,      // this is a sub-object of an add or delete and thus not something that an action can be driven from (just follows whatever the parent has selected)
    VAL_PATH_REL = 0x020000,    // value is a path relative to tab_obj, not a global path
  };

  DiffFlags     flags;          // flags for diff status

  taObjDiff_List* owner;        // the owner of this one
  int           idx;            // the index number in owning list
  int           nest_level;     // how deeply nested or embedded is this object in the obj hierarchy
  String        name;           // object name (member name, object type) -- this is used in diffing and is not always best for display -- see GetDisplayName()
  String        value;          // string representation of this object
  taHashVal     hash_code;      // hash-code of name&value + nest_level -- this is what diff is based on
  TypeDef*      type;           // type of this object (same as mdef->type if a member) -- not type of parent
  MemberDef*    mdef;           // memberdef if this is a member of a parent object
  void*         addr;           // address in memory of this object
  void*         par_addr;       // address in memory of parent of this object, if a member of a containing object
  TypeDef*      par_type;       // type of parent object, if a member of a containing object
  taObjDiffRec* par_odr;        // parent diff record
  taObjDiffRec* diff_odr;       // paired diff record from other source
  taObjDiffRecExtra* extra;     // extra data for object
  void*         widget;         // points to the widget associated with this record

  inline void           SetDiffFlag(DiffFlags flg)   { flags = (DiffFlags)(flags | flg); }
  // #CAT_ObjectMgmt set data column flag state on
  inline void           ClearDiffFlag(DiffFlags flg) { flags = (DiffFlags)(flags & ~flg); }
  // #CAT_ObjectMgmt clear data column flag state (set off)
  inline bool           HasDiffFlag(DiffFlags flg) const {
    return 0 != (flags & flg);
  }
  // #CAT_ObjectMgmt check if data column flag is set
  inline void           SetDiffFlagState(DiffFlags flg, bool on)
  { if(on) SetDiffFlag(flg); else ClearDiffFlag(flg); }
  // #CAT_ObjectMgmt set data column flag state according to on bool (if true, set flag, if false, clear it)

  String        GetDisplayName();
  // returns a name suitable for gui display purposes -- taBase->GetDisplayName else name

  bool          ActionAllowed();
  // is an action allowed for this item?  checks flags and types to make sure

  bool          GetCurAction(int a_or_b, String& lbl);
  // get currently set action for this guy, depending on its flag status, and a_or_b (a=0, b=1) -- also fills in label describing action
  void          SetCurAction(int a_or_b, bool on_off);
  // set action for this guy, depending on its flag status, and a_or_b (a=0, b=1)

  void          GetValue(taObjDiff_List& odl);
  // gets the value and hash code (and name) fields based on the other information already set -- also uses information on overall obj diff list (tab_obj_a for paths)

  void          ComputeHashCode();
  // computes the hash code based on name + & + value + nest_level -- called by GetValue, but call this manually if anything changes

  taObjDiffRec();
  taObjDiffRec(taObjDiff_List& odl, int nest, TypeDef* td, MemberDef* md, void* adr, void* par_adr = NULL,
               TypeDef* par_typ = NULL, taObjDiffRec* par_od = NULL);
  // this is the main interface for making a new item -- sets info and calls GetValue

  taObjDiffRec(const taObjDiffRec& cp); // copy constructor
  ~taObjDiffRec();

  taObjDiffRec* Clone()         { return new taObjDiffRec(*this); }
  taObjDiffRec* MakeToken()     { return new taObjDiffRec(); }
  void          Copy(const taObjDiffRec& cp);

private:
  void          Initialize();
  void          Copy_(const taObjDiffRec& cp);
};

class TA_API taObjDiff_List: public taPtrList<taObjDiffRec> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS list of object difference records
protected:
  String        GetListName_() const            { return name; }
  String        El_GetName_(void* it) const;
  TALPtr        El_GetOwnerList_(void* it) const;
  void*         El_SetOwner_(void* it);
  void          El_SetIndex_(void* it, int i);

  void*         El_Ref_(void* it);
  void*         El_unRef_(void* it);
  void          El_Done_(void* it);
  void*         El_MakeToken_(void* it);
  void*         El_Copy_(void* trg, void* src);

public:
  String        name;           // of the list
#ifndef NO_TA_BASE
  taBase*       tab_obj_a;      // initial diff object for GetObjDiffVal and A comparison object for diff
  taBase*       tab_obj_b;      // original B comparison object as a taBase
#endif

  void          Initialize();

  taObjDiff_List()                              { Initialize(); }
  taObjDiff_List(const taObjDiff_List& cp)      { Initialize(); Borrow(cp); }
  ~taObjDiff_List();
  void operator=(const taObjDiff_List& cp)      { Borrow(cp); }

  void          HashToIntArray(int_PArray& array);
  // copy all hash values to given array -- for use in differencing

  void          Diff(taObjDiff_List& diff_ods, taObjDiff_List& cmp_list);
  // perform a diff operation between this list and comparison list (cmp_list), linking diff recs into diff_ods list with flags set to indicate nature of differences

protected:
  voidptr_PArray        nest_pars;
  // keeps track of current parents at each nest level

  bool          CheckAddParents(taObjDiff_List& diff_ods, taObjDiffRec* rec, bool a_list);
  // add parents of rec item as necessary depending on nest_pars parents already current
};

#endif // ta_type_h

