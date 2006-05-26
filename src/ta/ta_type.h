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
#ifdef TA_NO_GUI
class taiType;			// #IGNORE
class taiEdit;			// #IGNORE
class taiViewType;		// #IGNORE
class taiMember;		// #IGNORE
class taiMethod;		// #IGNORE
#else
class taiDataLink; //
class taiType; //
class taiEdit; //
class taiViewType; //
class taiMember; //
class taiMethod; //
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
public:
  int	FindContains(const char* op, int start=0) const;
  void	Add(const String& it)			{ taPlainArray<String>::Add(it); }
  void	Add(const char* it)
  { String fad=it; taPlainArray<String>::Add(fad); }

  bool	AddUnique(const String& it)	{ return taPlainArray<String>::AddUnique(it); }
  bool	AddUnique(const char* it)
  { String fad=it; return taPlainArray<String>::AddUnique(fad); }

#ifdef __MAKETA__
  const String 	AsString(const char* sep) const;
#else
  const String 	AsString(const char* sep = ", ") const;
#endif
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

class TA_API IApp {
  // basic methods that the root/app object must support
public:
  virtual void  Settings() = 0;		// #MENU #MENU_ON_Object edit global settings/parameters (taMisc)
  virtual void	SaveConfig() = 0;		// #MENU #CONFIRM save current configuration to file ~/.pdpconfig that is automatically loaded at startup: IMPORTANT: DO NOT HAVE A PROJECT LOADED!
  virtual void	LoadConfig() = 0;		// #MENU #CONFIRM load current configuration from file ~/.pdpconfig that is automatically loaded at startup
  virtual void	Info() = 0;			// #MENU get information/copyright notice
  // #MENU #ARGC_0 #USE_RVAL #NO_REVERT_AFTER use object browser to find an object, starting with initial path if given
  virtual void	Quit() = 0;
  // #MENU #CONFIRM #MENU_SEP_BEFORE #NO_REVERT_AFTER quit from software..
  virtual void	SaveAll() = 0; // saves all the contents of the app object
  virtual ~IApp() {}
};

#ifndef NO_TA_BASE
class TA_API taiMiscCore: public QObject { 
  // ##NO_TOKENS ##NO_INSTANCE object for Qt Core event processing, etc. taiMisc inherits; taiM is always instance
INHERITED(QObject)
Q_OBJECT
public:
  
  static taiMiscCore*	New(QObject* parent = NULL);
    // either call this or call taiMisc::New 
  
  static void		WaitProc(); // the core idle loop process
  
  static int		RunPending();	// run any pending qt events that might need processed
  static void		Quit(); // call to quit, invokes Quit_impl on instance first
  
  const String		classname(); // 3.x compatability, basically the app name
  
  taiMiscCore(QObject* parent = NULL);
  ~taiMiscCore();
protected slots:
  virtual void	timer_timeout(); // called when timer times out, for waitproc processing
  
protected:
  QTimer*		timer; // for idle processing
  virtual void			Init(bool gui = false); // NOTE: called from static New
  virtual void		Quit_impl();
};

extern TA_API taiMiscCore* taiMC_; // note: use taiM macro instead
#ifdef TA_GUI
# define taiM taiM_
#else
# define taiM taiMC_
#endif
#endif

class TA_API taMisc {
  // #NO_TOKENS #INSTANCE miscellanous global parameters and functions for type access system
public:
  enum ShowMembs { // #BITS
    NO_HIDDEN 		= 0x01,
    NO_READ_ONLY 	= 0x02,
    NO_DETAIL 		= 0x04,
    NO_NORMAL		= 0x08,
    NO_EXPERT		= 0x10,

    ALL_MEMBS		= 0x00, // #NO_BIT
    NO_HID_RO 		= 0x03, // #NO_BIT
    NO_HID_DET 		= 0x05, // #NO_BIT
    NO_RO_DET 		= 0x06, // #NO_BIT
    NO_HID_RO_DET 	= 0x07, // #NO_BIT
    NORM_MEMBS 		= 0x17, // #NO_BIT

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

  static IApp*		app; 		// #IGNORE application object -- usually same as taiMisc::root object
  static String		version_no; 	// #READ_ONLY #NO_SAVE #SHOW version number of ta/css
  static TypeSpace 	types;		// #READ_ONLY #NO_SAVE list of all the active types

  static bool		in_init;	// #READ_ONLY #NO_SAVE true if in ta initialization function
  static bool		quitting;	// #READ_ONLY #NO_SAVE true once we are quitting
  static bool		not_constr;	// #READ_ONLY true if ta types are not yet constructed (or are destructed)

  static bool		gui_active;	// #READ_ONLY #NO_SAVE if gui has been started up or not
  static bool		is_loading;	// #READ_ONLY #NO_SAVE true if currently loading an object
  static bool		is_saving;	// #READ_ONLY #NO_SAVE true if currently saving an object
  static bool		is_duplicating;	// #READ_ONLY #NO_SAVE true if currently duplicating an object
  static int		strm_ver;	// #READ_ONLY #NO_SAVE during dump or load, version # (app v4.x=v2 stream)

  static int		dmem_proc; 	// #READ_ONLY #NO_SAVE #SHOW distributed memory process number (rank in MPI, always 0 for no dmem)
  static int		dmem_nprocs; 	// #READ_ONLY #NO_SAVE #SHOW distributed memory number of processes (comm_size in MPI, 1 for no dmem)

  static int		display_width;	// #SAVE width of shell display (in chars)
  static int		sep_tabs;	// #SAVE number of tabs to separate items by
  static int		max_menu;	// #SAVE maximum number of items in a menu
  static int		search_depth;   // #SAVE depth recursive find will search for a path object
  static int		color_scale_size; // #SAVE number of colors to put in a color scale
  static int		mono_scale_size;  // #SAVE number of monochrome bit-patterns to put in a color scale
  static float		window_decor_offset_x; // #SAVE some window managers (e.g., KDE) add an offset to location of windows -- add this amount to x position to compensate
  static float		window_decor_offset_y;  // #SAVE some window managers (e.g., KDE) add an offset to location of windows -- add this amount to y position to compensate
  static float		mswin_scale; 	// #SAVE window size scaling parameter for MS Windows
  static int		jpeg_quality; 	// #SAVE jpeg quality for dumping jpeg files (1-100; 85 default)

  static ShowMembs	show;		// #SAVE what to show in general (eg. css)
  static ShowMembs	show_gui;	// #SAVE what to show in the gui
  static TypeInfo	type_info;	// #SAVE what to show when displaying type information
  static KeepTokens	keep_tokens;	// #SAVE default for keeping tokens
  static SaveFormat	save_format;	// #SAVE format to use when saving things (dump files)
  static LoadVerbosity	verbose_load;	// #SAVE report the names of things during loading
  static LoadVerbosity  gui_verbose_load; // #SAVE what to report in the load dialog
  static bool		dmem_debug; 	// #SAVE turn on debug messages for distributed memory processing
  static TypeDef*	default_scope;  // type of object to use to determine if two objects are in the same scope

  static bool		auto_edit; 	// #SAVE automatic edit dialog after creation?
  static AutoRevert	auto_revert;    // #SAVE when dialogs are automatically updated (reverted), what to do about changes?

  static String_PArray 	include_paths;  // #SAVE paths to be used for finding files

  static String		pdp_dir;	// normal install path ("/usr/local/pdp++" on unix)
  static String		tmp_dir;	// #SAVE location of temporary files
  static String		compress_cmd;	// #SAVE command to use for compressing files
  static String		uncompress_cmd;	// #SAVE for uncompressing files
  static String		compress_sfx;	// #SAVE suffix to use for compressing files
  static String		help_file_tmplt; // #SAVE template for converting type name into a help file (%t = type name)
  static String		help_cmd;	// #SAVE how to run html browser to get help, %s is entire path to help file
  static ostream*	record_script;
  static bool		beep_on_error; // #SAVE #DEF_false beep when an error message is printed on the console
  // stream to use for recording a script of interface activity (NULL if no record)
  static void	(*WaitProc)();
  // set this to a work process for idle time processing
  static void (*Busy_Hook)(bool); // #IGNORE gui callback when prog goes busy/unbusy; var is 'busy'
  static void (*ScriptRecordingGui_Hook)(bool); // #IGNORE gui callback when script starts/stops; var is 'start'
  static void (*DelayedMenuUpdate_Hook)(taBase*); // #IGNORE gui callback -- avoids zillions of gui ifdefs everywhere

  void	SaveConfig();		// #BUTTON #CONFIRM save configuration defaults to ~/.taconfig file that is loaded automatically at startup
  void	LoadConfig();		// #BUTTON #CONFIRM load configuration defaults from ~/.taconfig file (which is loaded automatically at startup)

  static void 	Error(const char* a, const char* b="", const char* c="",
		      const char* d="", const char* e="", const char* f="",
		      const char* g="", const char* h="", const char* i="");
  // displays error either in a window+stderr if gui_active or to stderr only

  static void 	Warning(const char* a, const char* b="", const char* c="",
		      const char* d="", const char* e="", const char* f="",
		      const char* g="", const char* h="", const char* i="");
  // displays warning to stderr and/or other logging mechanism

  static int 	Choice(const char* text="Choice", const char* a="Ok", const char* b="",
		       const char* c="", const char* d="", const char* e="",
		       const char* f="", const char* g="", const char* h="",
		       const char* i="");
  // allows user to choose among different options in window if iv_active or stdin/out

  static void	DelayedMenuUpdate(TAPtr obj);
  // add object to list to be updated later (by Wait_UpdateMenus)

  static void 	Busy();		// puts system in a 'busy' state
  static void	DoneBusy();	// when no longer busy, call this function

  static void	Initialize();	// initialize type system, called in ta_TA.cc
  static void	InitializeTypes();// called after all type info has been loaded into types
  static void	DMem_Initialize(); // #IGNORE initialize distributed memory stuff

  static void	MallocInfo(ostream& strm);
  // generate malloc memory statistic information to given stream
  static void	ListAllTokens(ostream& strm);
  // generate a list and count of all types that keep tokens, with a count of tokens
  static int 	ReplaceAllPtrs(TypeDef* obj_typ, void* old_ptr, void* new_ptr);
  // search through all tokens in all types and replace any pointer to object of type obj_typ address old_ptr with new_ptr
  static int 	ReplaceAllPtrsWithToken(TypeDef* obj_typ, void* old_ptr);
  // search through all tokens in all types and replace any pointer to old_ptr with first other token in same scope of same type

#if ((defined(TA_OS_UNIX)))
#ifndef __MAKETA__
  static void	Register_Cleanup(SIGNAL_PROC_FUN_ARG(fun));
  // register a cleanup process in response to all terminal signals
#endif
  static void	Decode_Signal(int err);	// printout translation of signal on cerr
#endif // WINDOWS

  static void	CharToStrArray(String_PArray& sa, const char* ch);
  // convert space-delimeted character string to a string array
  static String	StrArrayToChar(const String_PArray& sa);
  // convert a string array to a space-delimeted character string

  static void	SpaceLabel(String& lbl);
  // add spaces to a label in place of _'s and upper-lower transitions

  static String	LeadingZeros(int num, int len);
  // returns num converted to a string with leading zeros up to len

  static String	FormatValue(float val, int width, int precision);
  // format output of value according to width and precision

  static String	StringMaxLen(const String& str, int len);
  // returns string up to maximum length given (enforces string to be len or less in length)
  static String	StringEnforceLen(const String& str, int len);
  // returns string enforced to given length (spaces added to make length)

  static void	StartRecording(ostream* strm); // sets record_strm and record_cursor
  static void   StopRecording();	       // unsets record_strm and record_cursor
  static bool	RecordScript(const char* cmd);
  // record the given script command, if the script is open (just sends cmd to stream)
#ifndef NO_TA_BASE
  static void  ScriptRecordAssignment(taBase* tab,MemberDef* md);
  // record last script assignment of tab's md value;
  static void 	SRIAssignment(taBase* tab,MemberDef* md);
  // record inline md assignment
  static void 	SREAssignment(taBase* tab,MemberDef* md);
  // record enum md assignment
#endif
  // path manips
  static String	remove_name(String& path);
  static String	FindFileInclude(const char* fname);
  // try to find file fnm in one of the include paths -- returns complete path to file

  // parsing stuff
  static String	LexBuf;	// #HIDDEN a buffer, contains last thing read by read_ funs

  // return value is the next character in the stream
  // peek=true means that return value was not read, but was just peek'd

  static int	skip_white(istream& strm, bool peek = false);
  static int	skip_white_noeol(istream& strm, bool peek = false); // don't skip end-of-line
  static int	skip_till_start_quote_or_semi(istream& strm, bool peek = false);      
    // used to seek up to an opening " for a string; will terminate on a ;
  static int	read_word(istream& strm, bool peek = false);
  static int	read_alnum(istream& strm, bool peek = false); 		// alpha-numeric
  static int    read_alnum_noeol(istream& strm, bool peek = false);
  static int	read_till_eol(istream& strm, bool peek = false);
  static int	read_till_semi(istream& strm, bool peek = false);
  static int	read_till_lbracket(istream& strm, bool peek = false);
  static int	read_till_lb_or_semi(istream& strm, bool peek = false);
  static int	read_till_rbracket(istream& strm, bool peek = false);
  static int	read_till_rb_or_semi(istream& strm, bool peek = false);
  static int 	read_till_end_quote(istream& strm, bool peek = false);
    // read-counterpart to write_quoted_string; read-escaping, until "
  static int	read_till_end_quote_semi(istream& strm, bool peek = false); 
    // read-counterpart to write_quoted_string; read-escaping, until "; (can be ws btwn " and ;)
  static int	skip_past_err(istream& strm, bool peek = false);
  // skips to next rb or semi (robust)
  static int	skip_past_err_rb(istream& strm, bool peek = false);
  // skips to next rbracket (robust)

  // output functions
  static ostream& indent(ostream& strm, int indent, int tsp=2);
  static ostream& write_quoted_string(ostream& strm, const String& str, 
    bool write_if_empty = false);
    // writes the string, including enclosing quotes, escaping so we can read back using read_till_end_quote funcs
  static ostream& fmt_sep(ostream& strm, const String& itm, int no, int indent,
			  int tsp=2);
  static ostream& fancy_list(ostream& strm, const String& itm, int no, int prln,
			     int tabs);
};

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
//   IDataLinkClient	//
//////////////////////////

// Mixin interface for Node that uses datalinks, ex. tree node or Inventor node

class TA_API IDataLinkClient : public virtual ITypedObject {//#NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE
  // this is the interface available to the host data object
friend class taDataLink;
public:
#ifndef TA_NO_GUI
  taiDataLink*		link() const {return (taiDataLink*)m_link;}
#else
  taDataLink*		link() const {return m_link;}
#endif
  virtual TypeDef*	GetDataTypeDef() const; // convenience function, default gets data type from link
  virtual void		DataLinkDestroying(taDataLink* dl) = 0; // called by DataLink when destroying; it will remove datalink ref in dlc upon return
  virtual void		DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) = 0; //
  virtual bool		IsDataView() {return false;} // 'true' for dlc's that are DataViews (see taBase)
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

  virtual int		NumListCols() {return 1;} // number of columns in a list view for this item type
  virtual String	GetColHeading(int col) {return String("Item");} // header text for the indicated column
  virtual String	GetColText(int col, int itm_idx = -1) {return GetName();} // text for the indicated column
  virtual String	ChildGetColText(taDataLink* child, int col, int itm_idx = -1) {return child->GetColText(col, itm_idx);}
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

  virtual MethodDef*	FindAddr(ta_void_fun funa, int& idx) const;
  // find fun by addr, idx is actual index in method space
  virtual MethodDef*	FindOnListAddr(ta_void_fun funa, const String_PArray& lst, int& lidx) const;
  // find fun on given list by addr, lidx is 'index' of funs on same list
  virtual MethodDef*	FindOnListIdx(int lidx, const String_PArray& lst) const;
  // find fun on given list by index, as given by FindOnListAddr()

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
  virtual int 	ReplaceAllPtrs(TypeDef* obj_typ, void* old_ptr, void* new_ptr);
  // search through all tokens in all types and replace any pointer to old_ptr of object type obj_typ with new_ptr

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
  bool		HasOption(const char* op) const { return (opts.Find(op) >= 0); }
    // check if option is set
  virtual String	OptionAfter(const char* op) const;
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

  bool		CheckList(const String_PArray& lst) const;
  // check if member has a list in common with given one

  bool		ShowMember(taMisc::ShowMembs show = taMisc::USE_SHOW_DEF) const;
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

  int	 	Dump_Load(istream& strm, void* base, void* par);
};

class TA_API MethodDef : public TypeItem {// defines a class method
#ifndef __MAKETA__
typedef TypeItem inherited;
#endif
public:
  MethodSpace*	owner;

  TypeDef*	type;		// of the return value
  bool		is_static;	// true if this method is static
  ta_void_fun   addr;		// address (only for static or reg_fun functions)
  String_PArray	inh_opts;	// inherited options ##xxx
#ifdef TA_GUI
  taiMethod*	im;		// gui structure for edit representation
#endif // def TA_GUI
  int		fun_overld;	// number of times function is overloaded
  int		fun_argc;	// nofun, or # of parameters to the function
  int		fun_argd;	// indx for start of the default args (-1 if none)
  TypeSpace	arg_types;	// argument types
  String_PArray	arg_names;	// argument names
  String_PArray	arg_defs;	// argument default values
  String_PArray	arg_vals;	// argument values (previous)

  css_fun_stub_ptr stubp;	// css function stup pointer

  void		Initialize();
  void		Copy(const MethodDef& cp);
  MethodDef();
  MethodDef(const char* nm);
  MethodDef(TypeDef* ty, const char* nm, const char* dsc, const char* op, const char* lis,
	    int fover, int farc, int fard, bool is_stat = false, ta_void_fun funa = NULL,
	    css_fun_stub_ptr stb = NULL);
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

  TypeDef*		GetTemplParent() const;
  // returns immediate parent which is a template (or NULL if none found)
  String	GetTemplName(const TypeSpace& inst_pars) const;
  void		SetTemplType(TypeDef* templ_par, const TypeSpace& inst_pars);
  // set type of a template class

  EnumDef*	FindEnum(const char* enum_nm) const;
  // find an enum and return its definition (or NULL if not found).  searches in enum_vals, then subtypes
  int		GetEnumVal(const char* enum_nm, String& enum_tp_nm) const;
  // find an enum and return its enum_no value, and set enum_tp_nm at the type name of the enum.  if not found, returns -1 and enum_tp_nm is empty
  String	GetEnumString(const char* enum_tp_nm, int enum_val) const;
  // get the name of enum with given value in enum list of given type (e.g., enum defined within class)
#ifndef NO_TA_BASE  
  void*			GetInstance() const
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
  int		ReplaceAllPtrs(TypeDef* obj_typ, void* old_ptr, void* new_ptr);
  // replace any member pointers that point to old_ptr with new_ptr (returns no. changed)
  int		ReplaceAllPtrsThis(void* base, TypeDef* obj_typ, void* old_ptr, void* new_ptr);
  // replace any member pointers that point to old_ptr with new_ptr (returns no. changed)

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

  int		Dump_Load(istream& strm, void* base, void* par=NULL);
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
  mutable int	 m_cacheInheritsNonAtomicClass;
};

//TODO: this class doesn't belong here -- move it to a more appropriate file, or its own

class TA_API MTRnd {
  // #IGNORE A container for the Mersenne Twister (MT19937) random number generator by Makoto Matsumoto and Takuji Nishimura
public:
#ifndef __MAKETA__
  static const int N = 624;
  static const int M = 397;
  static const uint MATRIX_A = 0x9908b0dfU;   /* constant vector a */
  static const uint UPPER_MASK = 0x80000000U; /* most significant w-r bits */
  static const uint LOWER_MASK = 0x7fffffffU; /* least significant r bits */

  static uint mt[N];
  static int mti;				/* mti==N+1 means mt[N] is not initialized */
#endif

  static void seed(uint s); 	// seed the generator with given seed value
  static uint seed_time_pid();	// seed the generator with a random seed produced from the time and the process id
  static void seed_array(uint init_key[], int key_length); // seed with given initial key

  static uint genrand_int32();	// generates a random number on [0,0xffffffff]-interval
  static int  genrand_int31();	// generates a random number on [0,0x7fffffff]-interval
  static double genrand_real1(); // generates a random number on [0,1]-real-interval
  static double genrand_real2(); // generates a random number on [0,1)-real-interval
  static double genrand_real3(); // generates a random number on (0,1)-real-interval
  static double genrand_res53(); // generates a random number on [0,1) with 53-bit resolution
};


#endif // ta_type_h

