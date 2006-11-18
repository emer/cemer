/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// Type Access: Automatic Access to C Types

#ifndef TA_TYPE_H
#define TA_TYPE_H

#include "ta_def.h"
#include "ta_list.h"
#include "ta_variant.h"

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
class TA_API MethodSpace;
class TA_API TypeSpace;
class TA_API EnumDef;
class TA_API MemberDef;
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
  provide a guarded typedef for this (for Windows) in the taglobal.h header file.
  
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
extern TA_API TypeDef TA_signed_char;
extern TA_API TypeDef TA_unsigned_char;
extern TA_API TypeDef TA_short;
  extern TA_API TypeDef TA_signed_short;
  extern TA_API TypeDef TA_short_int;
  extern TA_API TypeDef TA_signed_short_int;
extern TA_API TypeDef TA_unsigned_short;
  extern TA_API TypeDef TA_unsigned_short_int;
extern TA_API TypeDef TA_int;
  extern TA_API TypeDef TA_signed;
  extern TA_API TypeDef TA_signed_int;
//extern TA_API TypeDef TA_intptr_t; // on 32-bit systems
//extern TA_API TypeDef TA_long; // where long is 32-bit
extern TA_API TypeDef TA_unsigned_int;
  extern TA_API TypeDef TA_unsigned;
//extern TA_API TypeDef TA_unsigned_long; // where long is 32-bit
extern TA_API TypeDef TA_long;
  extern TA_API TypeDef TA_signed_long;
  extern TA_API TypeDef TA_long_int;
  extern TA_API TypeDef TA_signed_long_int;
extern TA_API TypeDef TA_unsigned_long;
  extern TA_API TypeDef TA_unsigned_long_int;
extern TA_API TypeDef TA_int64_t;
  extern TA_API TypeDef TA_long_long;
  extern TA_API TypeDef TA_signed_long_long;
//extern TA_API TypeDef TA_intptr_t; // on 64-bit systems
//extern TA_API TypeDef TA_long; // where long is 64-bit
extern TA_API TypeDef TA_uint64_t;
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
extern TA_API TypeDef TA_MemberDef;
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

typedef int ta_memb_ptr_class::* ta_memb_ptr;

class taBase;
typedef taBase* TAPtr;		// pointer to a taBase type

// a plain-array of strings
class TA_API String_PArray : public taPlainArray<String> {
INHERITED(taPlainArray<String>)
public:
  static const String	def_sep; // ", "
  
  int	FindContains(const String& op, int start=0) const;
  // find item that contains string -- start < 0 = start from end

  const String 	AsString(const String& sep = def_sep) const;
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

class int_PArray: public taPlainArray<int> {
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

// a plain-array of strings
class TA_API NameVar_PArray : public taPlainArray<NameVar> {
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
  bool		SetVal(const String& nm, const Variant& vl);
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


#ifndef NO_TA_BASE
enum CancelOp { // ops for passing cancel status and instructions, typically for Window closing 
  CO_PROCEED,		// tells caller to proceed with operation (typical default)
  CO_CANCEL,		// client can set this to tell caller to cancel the operation
  CO_NOT_CANCELLABLE	// preset, to tell client that operation will go ahead unconditionally
};


class TA_API taiMiscCore: public QObject { 
  // ##NO_TOKENS ##NO_INSTANCE object for Qt Core event processing, etc. taiMisc inherits; taiM is always instance
INHERITED(QObject)
Q_OBJECT
public:
  
  static taiMiscCore*	New(QObject* parent = NULL);
    // either call this or call taiMisc::New 
  
  static void		WaitProc(); // the core idle loop process
  
  static int		RunPending();	// run any pending qt events that might need processed
  static void		Quit(CancelOp cancel_op = CO_NOT_CANCELLABLE); 
   // call to quit, invokes Quit_impl on instance first
  static void		OnQuitting(CancelOp& cancel_op); // call this when a quit situation is detected -- does all the save logic
  
  const String		classname(); // 3.x compatability, basically the app name
  
  virtual void		Busy_(bool busy) {} // impl for gui in taiMisc
  virtual void		CheckConfigResult_(bool ok);
    // this is the nogui version; taiMisc does the gui version
    
  taiMiscCore(QObject* parent = NULL);
  ~taiMiscCore();
protected slots:
  void			app_aboutToQuit();
  virtual void		timer_timeout(); // called when timer times out, for waitproc processing
  
protected:
  QTimer*		timer; // for idle processing
  virtual void		Init(bool gui = false); // NOTE: called from static New
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
  operator bool() {return (cnt);}
  signed char 	operator++() {return ++cnt;}
  signed char 	operator++(int) {return cnt++;} // post
  signed char 	operator--() {return --cnt;}
  signed char 	operator--(int)  {return cnt--;} // post
  
  ContextFlag() {cnt = 0;} // NOTE: default binary copy constructor and copy operator are fine
private:
  signed char	cnt; // keep same size as bool -- should never be nesting this deep
};
#endif // __MAKETA__


/////////////////////////////////////////////////////////////////////
// 	taMisc

class TA_API taMisc {
  // #NO_TOKENS #INSTANCE miscellanous global parameters and functions for type access system
public:
friend class InitProcRegistrar;

  enum ShowMembs { // #BITS
    NO_HIDDEN 		= 0x01, // don't show items marked HIDDEN
    NO_READ_ONLY 	= 0x02, // don't show items marked READ_ONLY
    NO_DETAIL 		= 0x04, // don't show items marked DETAIL (usually not relevant)
    NO_NORMAL		= 0x08, // don't show items normally shown (helps indicate, ex. EXPERT items)
    NO_EXPERT		= 0x10, // don't show items marked EXPERT (often only for advanced sims)

    ALL_MEMBS		= 0x00, // #NO_BIT
    NO_HID_RO 		= 0x03, // #NO_BIT
    NO_HID_DET 		= 0x05, // #NO_BIT
    NO_RO_DET 		= 0x06, // #NO_BIT
    NO_HID_RO_DET 	= 0x07, // #NO_BIT
    NORM_MEMBS 		= 0x17, // #NO_BIT
    
    IS_HIDDEN 		= 0x01, // #IGNORE used in MemberDef::ShowMember to flag HIDDEN guys
    IS_READ_ONLY 	= 0x02, // #IGNORE used in MemberDef::ShowMember to flag RO guys
    IS_DETAIL 		= 0x04, // #IGNORE used in MemberDef::ShowMember to flag DETAIL guys
    IS_NORMAL		= 0x08, // #IGNORE used in MemberDef::ShowMember to flag NORMAL guys
    IS_EXPERT		= 0x10, // #IGNORE used in MemberDef::ShowMember to flag EXPERT guys
    IS_SHOW_ALWAYS	= 0x20, // #IGNORE used in MemberDef::ShowMember to flag SHOW guys
    SHOW_CHECK_MASK	= 0x2F, // #IGNORE #NO_BIT used in MemberDef::ShowMember checks

    USE_SHOW_GUI_DEF 	= 0x40,	// #NO_BIT use default from taMisc::show_gui
    USE_SHOW_DEF 	= 0x80 	// #NO_BIT use default from taMisc::show
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
    TIK_TYPE,
    TIK_ENUMSPACE,
    TIK_TOKENSPACE,
    TIK_MEMBERSPACE,
    TIK_METHODSPACE,
    TIK_TYPESPACE,
    TIK_UNKNOWN
  };
  
  enum QuitFlag { // helps during shutdown to know whether we can cancel
    QF_RUNNING		= 0, // 
    QF_USER_QUIT,	// we can still cancel while in this state of quitting
    QF_FORCE_QUIT	// too late to turn back now...
  };

  static String		version_no; 	// #READ_ONLY #NO_SAVE #SHOW version number of ta/css

  ////////////////////////////////////////////////////////
  // 	TA GUI parameters

  static String		font_name;	// #SAVE #CAT_GUI default font name to use
  static int		font_size;	// #SAVE #CAT_GUI default font size to use
  static String		console_font_name;	// #SAVE #CAT_GUI font name for the css console
  static int		console_font_size;	// #SAVE #CAT_GUI font size for the css console

  static int		display_width;	// #SAVE #HIDDEN #CAT_GUI width of console display (in chars) -- set automatically by gui console
  static int		sep_tabs;	// #SAVE #CAT_GUI number of tabs to separate items by
  static int		max_menu;	// #SAVE #CAT_GUI maximum number of items in a menu
  static int		search_depth;   // #SAVE #CAT_GUI depth recursive find will search for a path object
  static int		color_scale_size; // #SAVE #CAT_GUI number of colors to put in a color scale
  static int		mono_scale_size;  // #SAVE #CAT_GUI number of monochrome bit-patterns to put in a color scale
  static int		jpeg_quality; 	// #SAVE #CAT_GUI jpeg quality for dumping jpeg files (1-100; 95 default)

  static ShowMembs	show;		// #SAVE #CAT_GUI what to show in general (eg. css)
  static ShowMembs	show_gui;	// #SAVE #CAT_GUI what to show in the gui
  static TypeInfo	type_info;	// #SAVE #CAT_GUI what to show when displaying type information
  static KeepTokens	keep_tokens;	// #SAVE #CAT_GUI default for keeping tokens
  static SaveFormat	save_format;	// #SAVE #CAT_GUI format to use when saving things (dump files)
  static bool		auto_edit; 	// #SAVE #CAT_GUI automatic edit dialog after creation?
  static AutoRevert	auto_revert;    // #SAVE #CAT_GUI when dialogs are automatically updated (reverted), what to do about changes?
  static bool		beep_on_error; // #SAVE #DEF_false #CAT_GUI beep when an error message is printed on the console

  ////////////////////////////////////////////////////////
  // 	File/Paths Info

  static int		strm_ver;	// #READ_ONLY #NO_SAVE during dump or load, version # (app v4.x=v2 stream)
  static bool		save_compress;	// #SAVE #DEF_false #CAT_File compress by default for files that support it (ex .proj, .net)\nNOTE: starting with v4.0, compression is no longer recommended except for large weight files or large nets with saved units
  static LoadVerbosity	verbose_load;	// #SAVE #CAT_File report the names of things during loading
  static LoadVerbosity  gui_verbose_load; // #SAVE #CAT_File what to report in the load dialog

  static String		inst_prefix;
  // #SAVE #CAT_File prefix for software installation (e.g., /usr/local)
  static String	       	pkg_dir;
  // #SAVE #CAT_File directory name for current software package (e.g., ta_css or pdp++)
  static String		pkg_home;
  // #SAVE #CAT_File path to location of installed system files for current software package (e.g, /usr/local/ta_css) (should be inst_prefix + pkg_dir)
  static String		user_home;
  // #SAVE #CAT_File location of user's home directory
  static String		web_home;
  // #SAVE #CAT_File url for location of web repository of package information
  static String		tmp_dir;
  // #SAVE #CAT_File location of temporary files (e.g., inst_prefix/tmp)

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

  static String		compress_cmd;	// #SAVE #CAT_File command to use for compressing files
  static String		uncompress_cmd;	// #SAVE #CAT_File for uncompressing files
  static String		compress_sfx;	// #SAVE #CAT_File suffix to use for compressing files

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


  ////////////////////////////////////////////////////////
  // 	DMEM: Distributed Memory

  static int		dmem_proc; 	
  // #READ_ONLY #NO_SAVE #SHOW #CAT_DMem distributed memory process number (rank in MPI, always 0 for no dmem)
  static int		dmem_nprocs;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_DMem distributed memory number of processes (comm_size in MPI, 1 for no dmem)
  static int		cpus;
  // #READ_ONLY #NO_SAVE #SHOW #CAT_DMem number of cpus to use (<= physical cpus)
  static bool		dmem_debug;
  // #SAVE #CAT_DMem turn on debug messages for distributed memory processing

  ////////////////////////////////////////////////////////
  // 	Global State, Flags Etc

  static TypeSpace 	types;		// #READ_ONLY #NO_SAVE list of all the active types
  static TypeDef*	default_scope;  // #READ_ONLY #NO_SAVE type of object to use to determine if two objects are in the same scope

  static taPtrList_impl* init_hook_list; // #IGNORE list of init hook's to call during initialization

  static bool		in_init;	// #READ_ONLY #NO_SAVE #NO_SHOW true if in ta initialization function
  static signed char	quitting;	// #READ_ONLY #NO_SAVE #NO_SHOW true, via one of QuitFlag values, once we are quitting
  static bool		not_constr;	// #READ_ONLY #NO_SAVE #NO_SHOW true if ta types are not yet constructed (or are destructed)

  static bool		use_gui;	// #READ_ONLY #NO_SAVE #NO_SHOW whether the user has specified to use the gui or not (default = true)
  static bool		gui_active;	// #READ_ONLY #NO_SAVE #NO_SHOW if gui has been started up or not
  static ContextFlag	is_loading;	// #READ_ONLY #NO_SAVE #NO_SHOW true if currently loading an object
  static ContextFlag	is_saving;	// #READ_ONLY #NO_SAVE #NO_SHOW true if currently saving an object
  static ContextFlag	is_duplicating;	// #READ_ONLY #NO_SAVE #NO_SHOW true if currently duplicating an object
  static ContextFlag	is_checking;	// #READ_ONLY #NO_SAVE #NO_SHOW true if currently doing batch CheckConfig on objects

  static String		last_check_msg; // #READ_ONLY #NO_SAVE #SHOW #EDIT_DIALOG last error, or last batch of errors (if checking) by CheckConfig
  static bool		check_quiet; 	// #IGNORE mode we are in; set by CheckConfigStart
  static bool		check_confirm_success; // #IGNORE mode we are in; set by CheckConfigStart
  static bool		check_ok; 	// #IGNORE cumulative AND of all nested oks

#ifdef TA_GUI
  static QMainWindow*	console_win;	// #IGNORE the console window 
#endif

  static void	(*WaitProc)();
  // #IGNORE set this to a work process for idle time processing
  static void (*ScriptRecordingGui_Hook)(bool); // #IGNORE gui callback when script starts/stops; var is 'start'

  /////////////////////////////////////////////////
  //	Configuration -- object as settings

  void	SaveConfig();
  // #BUTTON #CONFIRM #CAT_Config save configuration defaults to ~/.taconfig file that is loaded automatically at startup
  void	LoadConfig();
  // #BUTTON #CONFIRM #CAT_Config load configuration defaults from ~/.taconfig file (which is loaded automatically at startup)

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

  static void 	CheckError(const char* a, const char* b=0, const char* c=0,
		      const char* d=0, const char* e=0, const char* f=0,
		      const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog called by CheckConfig routines; enables batching up of errors for display
  
  static void 	Warning(const char* a, const char* b=0, const char* c=0,
		      const char* d=0, const char* e=0, const char* f=0,
		      const char* g=0, const char* h=0, const char* i=0);
  // #CAT_Dialog displays warning to stderr and/or other logging mechanism

  static int 	Choice(const char* text="Choice", const char* a="Ok", const char* b=0,
		       const char* c=0, const char* d=0, const char* e=0,
		       const char* f=0, const char* g=0, const char* h=0,
		       const char* i=0);
  // #CAT_Dialog allows user to choose among different options in window if iv_active or stdin/out

  /////////////////////////////////////////////////
  //	Global state management

  static void	FlushConsole();
  // #CAT_GlobalState flush any pending console output (cout, cerr) -- call this in situations that generate a lot of console output..

  static void 	Busy();
  // #CAT_GlobalState puts system in a 'busy' state
  static void	DoneBusy();
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
  
#if ((defined(TA_OS_UNIX)))
#ifndef __MAKETA__
  static void	Register_Cleanup(SIGNAL_PROC_FUN_ARG(fun));
  // #IGNORE register a cleanup process in response to all terminal signals
#endif
  static void	Decode_Signal(int err);
  // #IGNORE printout translation of signal on cerr
#endif // WINDOWS

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
  static void	Init_DMem(int argc, const char* argv[]);
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
  static bool	CheckArgValContains(const String& vl);
  // #CAT_Args check if there is an arg that contains string fragment in its value
  static String	FindArgValContains(const String& vl);
  // #CAT_Args get full arg value that contains string fragment
#endif // NO_TA_BASE

  /////////////////////////////////////////////////
  //	Commonly used utility functions on strings/arrays/values
  // todo: these perhaps indicate shortcomings of associated objects -- fix them!

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

  /////////////////////////////////////////////////
  //	File Paths etc

  // path manips
  static String	remove_name(String& path);
  // #IGNORE 

  static String	FindFileOnPath(String_PArray& paths, const char* fname);
  // #CAT_File helper function: try to find file fnm in one of the load_include paths -- returns complete path to file (or empty str if not found)

  static String	FindFileOnLoadPath(const char* fname);
  // #CAT_File try to find file fnm in one of the load_include paths -- returns complete path to file  (or empty str if not found)

  /////////////////////////////////////////////////
  //	Recording GUI actions to css script

  static void	StartRecording(ostream* strm);
  // #CAT_Script sets record_strm and record_cursor
  static void   StopRecording();
  // #CAT_Script unsets record_strm and record_cursor
  static bool	RecordScript(const char* cmd);
  // #CAT_Script record the given script command, if the script is open (just sends cmd to stream)
#ifndef NO_TA_BASE
  static void  ScriptRecordAssignment(taBase* tab,MemberDef* md);
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

// Mixin interface for Node that uses datalinks, ex. tree node or Inventor node

class TA_API IDataLinkClient : public virtual IDataLinkProxy {//#NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE
  // this is the interface available to the host data object
friend class taDataLink;
public:
#ifndef TA_NO_GUI
  taiDataLink*		link() const {return (taiDataLink*)m_link;}
#else
  taDataLink*		link() const {return m_link;}
#endif
  virtual void		DataLinkDestroying(taDataLink* dl) = 0; // called by DataLink when destroying; it will remove datalink ref in dlc upon return
  virtual void		DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) = 0; //
  
  IDataLinkClient() {m_link = NULL;}
  ~IDataLinkClient();
protected:
  taDataLink*		m_link; // NOTE: will always be a taiDataLink or subclass
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
  virtual bool		isBase() const {return false;} // true if data is of type taBase (note: could still be null)
  
  void			AddDataClient(IDataLinkClient* dlc);
  bool			RemoveDataClient(IDataLinkClient* dlc); // WARNING: link is undefined after this call; CAUSES US TO DESTROY IF CLIENTS=0

//  void			DestroyPanels(); // note: don't make virtual, called from destructor
  virtual TypeDef*	GetDataTypeDef() const {return NULL;} // TypeDef of the data
  virtual MemberDef*	GetDataMemberDef() const {return NULL;} // if a member in a class, then the MemberDef
  virtual String	GetName() const {return _nilString;}
  virtual String	GetDisplayName() const; // default return Member name if has MemberDef, else GetName
  void			DataDestroying(); //CAUSES US TO DESTROY
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
protected:
  void*			m_data; // subclasses usually replace with strongly typed version
  taDataLink**		m_link_ref; // #IGNORE address of our reference in the data item
  IDataLinkClient_PtrList clients; // clients of this item (ex. either primary, or where it is aliased or linked)
  virtual ~taDataLink(); // we only ever implicitly destroy, when 0 clients
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

class TA_API DataChangeHelper {
  // class to help track data changes to help clients defer updates, etc;
public:
  bool		doStructUpdate(); // after update, will be true if struct changed; CLEARED ON READ, and if true, clears DataUpdate, so check this one first if separately handling struct changes
  bool		doDataUpdate(); // after update, will be true if data or structure changed; CLEARED ON READ

  void		UpdateFromDataChanged(int dcr); // pass the DataDataChanged dcr value in
  void		Reset(); // reset everything; normally not needed
  DataChangeHelper() {Reset();}
  
protected:
  short		struct_up_cnt; // count of struct updates in progress
  short		data_up_cnt; // count of data updates in progress
  bool		su;
  bool		du;
};


//////////////////////////
//   EnumSpace		//
//////////////////////////

class TA_API EnumSpace : public taPtrList<EnumDef> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS space of enums
protected:
  String	GetListName_() const		{ return name; }
  String	El_GetName_(void* it) const;
  TALPtr 	El_GetOwner_(void* it) const;
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

class TA_API MemberSpace: public taPtrList<MemberDef> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS space of members
protected:
  String	GetListName_() const		{ return name; }
  String El_GetName_(void* it) const;
  TALPtr 	El_GetOwner_(void* it) const;
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
  MemberSpace()				{ Initialize(); }
  MemberSpace(const MemberSpace& cp)	{ Initialize(); Borrow(cp); }
  ~MemberSpace();

  void operator=(const MemberSpace& cp)	{ Borrow(cp); }

  virtual MemberDef*	FindCheck(const char* nm, void* base, void*& ptr) const;
  // breadth-first find pass for the recursive procedures

  int			Find(MemberDef* it) const { return taPtrList<MemberDef>::Find(it); }
  int			Find(const char* nm) const;
  // checks name and type name in 2 passes
  virtual int		FindTypeName(const char* nm) const;
  // find by name of type
  virtual MemberDef*	FindNameR(const char* nm) const;
  // recursive find of name (or type name)
  virtual MemberDef* 	FindNameAddr(const char* nm, void* base, void*& ptr) const;
  // find of name returning address of found member
  virtual MemberDef*	FindNameAddrR(const char* nm, void* base, void*& ptr) const;
  // recursive find of name returning address of found member

  virtual int		Find(TypeDef* it) const;
  virtual MemberDef*	FindType(TypeDef* it, int& idx=Idx) const;
  // find by type, inherits from
  virtual MemberDef*	FindTypeR(TypeDef* it) const;
  // recursive find of type
  virtual MemberDef* 	FindTypeAddr(TypeDef* it, void* base, void*& ptr) const;
  // find of type returning address of found member
  virtual MemberDef*	FindTypeAddrR(TypeDef* it, void* base, void*& ptr) const;
  // recursive find of type returning address of found member

  virtual int		FindDerives(TypeDef* it) const;
  virtual MemberDef*	FindTypeDerives(TypeDef* it,  int& idx=Idx) const;
  // find by type, derives from

  virtual int		Find(void* base, void* mbr) const;
  virtual MemberDef*	FindAddr(void* base, void* mbr, int& idx=Idx) const;
  // find by address given base of class and address of member
  virtual int		FindPtr(void* base, void* mbr) const;
  virtual MemberDef*	FindAddrPtr(void* base, void* mbr, int& idx=Idx) const;
  // find by address of a member that is a pointer given base and pointer addr

  virtual void		CopyFromSameType(void* trg_base, void* src_base);
  // copy all members from class of the same type as me
  virtual void		CopyOnlySameType(void* trg_base, void* src_base);
  // copy only those members in my type (no inherited ones)

  // IO
  virtual ostream&   	OutputType(ostream& strm, int indent = 0) const;

  virtual ostream&   	Output(ostream& strm, void* base, int indent) const;
  virtual ostream&   	OutputR(ostream& strm, void* base, int indent) const;

  // for dump files
  virtual int   	Dump_Save(ostream& strm, void* base, void* par, int indent);
  virtual int   	Dump_SaveR(ostream& strm, void* base, void* par, int indent);
  virtual int   	Dump_Save_PathR(ostream& strm, void* base, void* par, int indent);

  virtual int   	Dump_Load(istream& strm, void* base, void* par,
				  const char* prv_read_nm = NULL, int prv_c = 0);
};


//////////////////////////
//   MethodSpace	//
//////////////////////////

class TA_API MethodSpace: public taPtrList<MethodDef> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS space of methods
protected:
  String	GetListName_() const		{ return name; }
  String El_GetName_(void* it) const;
  TALPtr 	El_GetOwner_(void* it) const;
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
  TALPtr 	El_GetOwner_(void* it) const;
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

class TA_API TypeItem: public taRefN {
  // ##INSTANCE ##NO_TOKENS ##NO_MEMBERS ##NO_CSS base class for TypeDef, MemberDef, MethodDef, EnumDef, and TypedefDef
INHERITED(taRefN)
public:
  enum ShowContext {
    SC_ANY,		// any context -- directives like "SHOW"
    SC_EDIT,		// for in edit dialogs -- directives like "SHOW_EDIT"
    SC_TREE		// in tree views (browsing) -- directives like "SHOW_TREE"
  };
  
  static const String opt_show; // "SHOW"
  static const String opt_no_show; // "NO_SHOW"
  static const String opt_hidden; // "HIDDEN"
  static const String opt_read_only; // "READ_ONLY"
  static const String opt_detail; // "DETAIL"
  static const String opt_expert; // "EXPERT"
  static const String opt_edit_show; // "EDIT_SHOW"
  static const String opt_edit_no_show; // "EDIT_NO_SHOW"
  static const String opt_edit_hidden; // "EDIT_HIDDEN"
  static const String opt_edit_read_only; // "EDIT_READ_ONLY"
  static const String opt_edit_detail; // "EDIT_DETAIL"
  static const String opt_edit_expert; // "EDIT_EXPERT"
  
  static const String opt_bits; // "BITS"
  static const String opt_instance; // "INSTANCE"
  
  int		idx;		// the index number for this type
  String	name;
  String	desc;		// a description
  String_PArray	opts;		// user-spec'd options (#xxx)
  String_PArray	lists;		// user-spec'd lists   (#LIST_xxx)
  taDataLink*	data_link;

  virtual TypeDef* 	GetOwnerType() const {return NULL;}
  virtual const String	GetPathName() const {return name;} 
    // name used for saving a reference in stream files, can be used to lookup again
    
  void		Copy(const TypeItem& cp);
  bool		HasOption(const String& op) const { return (opts.Find(op) >= 0); }
    // check if option is set
  bool		HasOptionAfter(const String& prefix, const String& op) const;
    // returns true if any prefix (Xxx_) has the value after of op; enables multi options of same prefix
  virtual String	OptionAfter(const String& op) const;
  // return portion of option after given option header
  virtual String	GetLabel() const;
  // checks for option of LABEL_xxx and returns it or name
  
  TypeItem();
  TypeItem(const TypeItem& cp); // copy constructor
  ~TypeItem();

private:
  void		init(); // #IGNORE
};

class TA_API EnumDef : public TypeItem { //  defines an enum member
#ifndef __MAKETA__
typedef TypeItem inherited;
#endif
public:
  EnumSpace*	owner;		// the owner of this one

  int		enum_no;	// number (value) of the enum


  void		Initialize();
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
};

class TA_API MemberDef : public TypeItem { //  defines a class member
#ifndef __MAKETA__
typedef TypeItem inherited;
#endif
public:
  MemberSpace*	owner;

  TypeDef*	type;		// of this item
  String_PArray	inh_opts;	// inherited options ##xxx
  ta_memb_ptr	off;		// offset of member from owner type
  int		base_off;	// offset for base of owner
  bool		is_static;	// true if this member is static
  void*         addr;		// address of static member
  bool		fun_ptr;	// true if this is a pointer to a function
#ifdef TA_GUI
  taiMember*	im;		// gui structure for edit representation (was: iv)
#endif
  void 		Initialize();
  void		Copy(const MemberDef& cp);
  MemberDef();
  MemberDef(const char* nm);
  MemberDef(TypeDef* ty, const char* nm, const char* dsc, const char* op, const char* lis,
	    ta_memb_ptr mptr, bool is_stat = false, void* maddr=NULL, bool funp = false);
  MemberDef(const MemberDef& cp);
  virtual ~MemberDef();
  MemberDef*	Clone()		{ return new MemberDef(*this); }
  MemberDef*	MakeToken()	{ return new MemberDef(); }

  void*			GetOff(const void* base) const;
  override TypeDef* 	GetOwnerType() const
  { TypeDef* rval=NULL; if((owner) && (owner->owner)) rval=owner->owner; return rval; }
  override const String	GetPathName() const; 
    // name used for saving a reference in stream files, can be used to lookup again

  const Variant		GetValVar(const void* base, void* par = NULL) const;
  
  bool		CheckList(const String_PArray& lst) const;
  // check if member has a list in common with given one

  bool		ShowMember(taMisc::ShowMembs show = taMisc::USE_SHOW_DEF,
    TypeItem::ShowContext show_context = TypeItem::SC_ANY) const;
  // decide whether to output or not based on options (READ_ONLY, HIDDEN, etc)

  void		CopyFromSameType(void* trg_base, void* src_base);
  // copy all members from same type
  void		CopyOnlySameType(void* trg_base, void* src_base);
  // copy only those members from same type (no inherited)

  ostream&   	OutputType(ostream& strm, int indent = 1) const;

  ostream& 	Output(ostream& strm, void* base, int indent) const;
  ostream& 	OutputR(ostream& strm, void* base, int indent) const;

  // for dump files
  bool		DumpMember(void* par); 		// decide whether to dump or not
  int		Dump_Save(ostream& strm, void* base, void* par, int indent);
  int	 	Dump_SaveR(ostream& strm, void* base, void* par, int indent);
  int	 	Dump_Save_PathR(ostream& strm, void* base, void* par, int indent);

  int	 	Dump_Load(istream& strm, void* base, void* par); //
protected:
  // note: bits in the show* vars are set to indicate the value, ie READ_ONLY has that bit set
  mutable byte	show_any; // bits for show any -- 0 indicates not determined yet, 0x80 is flag
  mutable byte	show_edit;
  mutable byte	show_tree;

  void		ShowMember_CalcCache() const; // called when show_any=0, ie, not configured yet
  void		ShowMember_CalcCache_impl(byte& show, const String& suff) const;
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

  const String		prototype() const; // text depiction of fun, ex "void MyFun(int p)"
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
  bool			ShowMethod(taMisc::ShowMembs show = taMisc::USE_SHOW_DEF) const;
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
    SC_DISPLAY		// value is being used for display purposes, ex. float value may be formatted prettily
  };
  
  static TypeDef* 	GetCommonSubtype(TypeDef* typ1, TypeDef* typ2); // get the common primary (1st parent class) subtype between the two

#if !defined(NO_TA_BASE) && defined(DMEM_COMPILE)
  int_PArray	dmem_type;
  virtual int 	GetDMemType(int share_set);
#endif
  TypeSpace*	owner;		// the owner of this one

  uint          size;		// size (in bytes) of item
  int		ptr;		// number of pointers
  bool 		ref;		// true if a reference variable
  bool		internal;	// true if an internal type (auto generated)
  bool		formal;		// true if a formal type (e.g. class, const, enum..)
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
#ifdef NO_TA_BASE
  bool		pre_parsed;	// true if previously parsed by maketa
#else
  bool		is_subclass;	// true if is a class, and inherits from another
  void**	instance;	// pointer to the instance ptr of this type
  taBase_List*	defaults;	// default values registered for this type
#endif

  // the following only apply to enums or classes
  EnumSpace	enum_vals;	// if type is an enum, these are the labels
  TypeSpace	sub_types;	// sub types scoped within class (incl enums)
  MemberSpace	members;	// member variables for class
  MethodSpace	methods;	// member functions (methods) for class
  String_PArray	ignore_meths;	// methods to be ignored
  TypeSpace	templ_pars;	// template parameters
  String	c_name;		// C name, when diff from name (ex 'unsigned_char' vs 'unsigned char")

  bool		is_enum() const; // true if an enum 
  bool		is_class() const; // true if it is a class
  
  void 		Initialize();
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
  TypeDef* 		GetTemplType() const;
  // gets base template parent of this type
  TypeDef* 		GetTemplInstType() const;
  // gets base template instantiation parent of this type
  String 		GetPtrString() const;
  // gets a string of pointer symbols (*) corresponding to the number ptrs
  String		Get_C_Name() const;
  // returns the actual c-code name for this type
  override const String	GetPathName() const;
  
  bool			HasEnumDefs() const; // true if any subtypes are enums
  bool			HasSubTypes() const; // true if any non-enum subtypes

  // you inherit from yourself.  This ensures that you are a "base" class (ptr == 0)
  bool 			InheritsFrom(const char *nm) const
  { bool rval=0; if((ptr == 0) && ((name == nm) || (par_cache.Find(nm)>=0) || FindParent(nm))) rval=1; return rval; }
  bool			InheritsFrom(const TypeDef* td) const
  { bool rval=0; if((ptr == 0) && ((this == td) || (par_cache.Find(td)>=0) || FindParent(td))) rval=1; return rval; }
  bool 			InheritsFrom(const TypeDef& it) const { return InheritsFrom((TypeDef*)&it); }

  // pointers to a type, etc, can be Derives from a given type (looser than inherits)
  bool 			DerivesFrom(const char *nm) const
  { bool rval=0; if((name == nm) || (par_cache.Find(nm)>=0) || FindParent(nm)) rval=1; return rval; }
  bool 			DerivesFrom(TypeDef* td) const
  { bool rval=0; if((this == td) || (par_cache.Find(td)>=0) || FindParent(td)) rval=1; return rval; }
  bool 			DerivesFrom(const TypeDef& it) const { return DerivesFrom((TypeDef*)&it); }

  // inheritance from a formal class (e.g. const, static, class)
  bool			InheritsFormal(TypeDef* it) const
  { bool rval=0; if((ptr == 0) && (par_formal.Find(it)>=0)) rval=1; return rval; }
  bool 			InheritsFormal(const TypeDef& it) const { return InheritsFormal((TypeDef*)&it); }
  bool			DerivesFormal(TypeDef* it) const
  { bool rval=0; if(par_formal.Find(it)>=0) rval=1; return rval; }
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

  bool	 	FindParent(const char* nm) const;
  bool 		FindParent(const TypeDef* it) const;
  // recursively tries to find parent, returns true if successful
  void*		GetParAddr(const char* par, void* base) const;
  void*		GetParAddr(TypeDef* par, void* base) const;
  // return the given parent's address given the base address (par must be a parent!)
  int		GetParOff(TypeDef* par, int boff=-1) const;
  // return the given parent's offset (par must be a parent!)
  bool 		ReplaceParent(TypeDef* old_tp, TypeDef* new_tp);
  // replace parent of old_tp with parent of new_tp (recursive)
  bool	 	FindChild(const char* nm) const;
  bool 		FindChild(TypeDef* it) const;
  // recursively tries to  find child, returns true if successful

  TypeDef*	GetTemplParent() const;
  // returns immediate parent which is a template (or NULL if none found)
  String	GetTemplName(const TypeSpace& inst_pars) const;
  void		SetTemplType(TypeDef* templ_par, const TypeSpace& inst_pars);
  // set type of a template class

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
#endif
  // for token management
  void 		Register(void* it);
  void 		unRegister(void* it);

  String	GetValStr(const void* base, void* par=NULL,
    MemberDef* memb_def = NULL, StrContext vc = SC_DEFAULT) const;
  // get a string representation of value
  const Variant	GetValVar(const void* base, void* par=NULL,
    const MemberDef* memb_def = NULL) const;
  // get a Variant representation of value; primarily for value types (int, etc.); NOTE: returns TAPtr types as the Base value (not a pointer to the pointer), which is usually what you want (see source for more detail)
  void		SetValStr(const String& val, void* base, void* par=NULL,
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
};

#endif // ta_type_h

