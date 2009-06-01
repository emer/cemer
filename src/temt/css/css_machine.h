/* -*- C++ -*- */
// Copyright, 1995-2007, Regents of the University of Colorado,
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


// welcome to the machine.h, backbone of the css language

#ifndef machine_h
#define machine_h 1

#include "css_extern_support.h"

#include "ta_string.h"
#include "ta_variant.h"
#include "ta_type.h"
#include "ta_base.h"

#ifdef TA_USE_QT
# include <QObject>
#endif

#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <memory.h>
#include <fstream>
#include <sstream>

#include <signal.h>
// #include <setjmp.h>

typedef int Int;
typedef double Real;
typedef int css_progdx;		// program index type

typedef cssEl* (*css_fun_stub_ptr)(void*, int, cssEl**);

// forwards
class cssElPtr;
class cssEl;
class cssInt;
class cssChar;
class cssReal;
class cssString;
class cssVariant;
class cssBool;

class cssPtr;
class cssRef;
class cssArray;
class cssArrayType;
class cssClassType;
class cssClassInst;
class cssEnum;
class cssEnumType;

class cssIOStream;
class cssIStream;
class cssOStream;
class cssFStream;

// function types
class cssElFun;
class cssElCFun;
class cssMbrCFun;
class cssCodeBlock;
class cssScriptFun;
class cssMbrScriptFun;

class cssCPtr;
class cssCPtr_int;
class cssCPtr_short;
class cssCPtr_long;
class cssCPtr_char;
class cssCPtr_enum;
class cssCPtr_double;
class cssCPtr_float;
class cssCPtr_String;
class cssCPtr_Variant;

class cssDef;

class cssSpace;
class cssInst;
class cssIJump;
class cssProg;
class cssProgSpace;
class cssCmdShell;

class pager_ostream {
  // class that provides one page at a time output of streamed inputs
public:
  int		n_lines;	// number of lines to output on a page
  bool		no_page;	// do not run the pager: just acts like a regular output stream
  ostream*	fout;		// output stream
  istream*	fin;		// input stream for prompting

  void	start();		// call this when starting new output to the screen; inits counter
  
  virtual pager_ostream& operator<<(const char* str); 
  virtual pager_ostream& operator<<(const String& str);  // this is the main call

  pager_ostream();
  pager_ostream(ostream* fo, istream* fi, int n_ln);
  virtual ~pager_ostream() { };

protected:
  int 	cur_line;
  bool	quitting;
};


class CSS_API cssMisc { // misc stuff for css
public:

//   static jmp_buf 	begin;

  static cssSpace 	Internal;	// for use in internal, not searched
  static cssSpace	Parse;		// just for parsing stuff
  static cssSpace 	PreProcessor;	// functions of the cpp
  static cssSpace 	TypesSpace;	// Types, searched
  static cssSpace 	TypesSpace_ptrs; // a mirror of types that are ptrs to types
  static cssSpace 	TypesSpace_refs; // a mirror of types that are refs to types
  static cssSpace 	Externs;	// external variables (externs)
  static cssSpace 	HardFuns;	// hard-coded functions, searched
  static cssSpace 	HardVars;	// hard-coded variables, searched
  static cssSpace 	Commands;	// commands, activate <cr> based entry
  static cssSpace 	Functions;	// internal functions
  static cssSpace 	Constants;	// constants
  static cssSpace 	Enums;		// enums, searched
  static cssSpace 	Settings;	// user accessible parameters, etc.
  static cssSpace	Defines;	// pre-proc. defines
  static cssSpace	VoidSpace;	// a space with nothing in it (don't search)

  // stuff for parsing..
  static cssProgSpace*	ConstExprTop; 	// top-level control for ConstExpr prog
  static cssProg* 	ConstExpr; 	// constant-expression evaluation space
  static cssProg* 	CDtorProg; 	// constructor-destructor program space
  static cssProg*	CallFunProg;	// program space for dialog callfun
  static cssElPtr  	cur_type;	// current type
  static cssClassType* 	cur_class; 	// current class
  static cssEl* 	cur_scope; 	// current class scope from most recent ::
  static cssMbrScriptFun* cur_method;	// current class method
  static cssEnumType* 	cur_enum; 	// current enum type
  static int		anon_type_cnt; 	// anonymous type counter
  static cssSpace	default_args;	// space that holds current default arg vals
  static bool           parsing_membdefn;       // true if parsing membdefn
  static bool           parsing_args;   // true if parsing arguments to functions

  static cssProgSpace* 	Top;		// top level space
  static cssProgSpace*	cur_top;	// current top-level (set for parsing, running)
  static cssProgSpace*	code_cur_top;	// cur_top for coding (e.g. if top switched for ConstExpr)
  static cssCmdShell* 	TopShell;	// top level command shell

  static cssArray*	s_argv;		// args passed by shell to scripts
  static cssInt*	s_argc;		// number of args passed by shell to scripts
  static String		prompt;		// default prompt for system (defaults to argv[0])
  static String		startup_file;	// file to be run at startup (by -f or -file arg from user)
  static String		startup_code;	// code to be executed at startup (by -e or -exec arg)
  static int		init_debug;	// initial debug level (by -v arg from user)
  static int		init_bpoint;	// initial breakpoint location (-b arg)
  static bool		init_interactive; // user wants to run interactively (-i from arg)
  static int		refcnt_trace; // user wants refcnt tracing (-rct from arg)
  static bool		obey_read_only; // #DEF_true actually pay attention to read-only comment directive information on pointers to C (taBase) objects
  static bool		call_update_after_edit;	// #DEF_false call UpdateAfterEdit on ta objects when they are modified (this slows things down a lot; user should call it when they really want it)

  static cssEl 		Void; 		// a void element
  static cssElPtr 	VoidElPtr;	// a void el pointer (to a void element)
  static cssPtr 	VoidPtr;	// a void pointer (for maketoken)
  static cssRef		VoidRef;	// a void reference (for maketoken)
  static cssArray	VoidArray;	// a void array (for maketoken)
  static cssArrayType   VoidArrayType;  // a void array type (for maketoken)
  static cssString	VoidString;
  static cssVariant	VoidVariant;
  static cssEnumType	VoidEnumType; 	// a void enum type
  static cssClassType	VoidClassType; 	// a void class type

  static int		readline_waitproc(); // note: not used in GUI mode, see css_qt.h

  static bool 		Initialize();
  // this is called to install builtin funcs, setup system, etc.; returns true if ok
  static bool 		Shutdown();
  // call this when quitting system -- resets all the above lists etc.

  static String	    	Indent(int indent_level, int indent_spc = 2);
  static String	    	IndentLines(const String& lines, int indent_level); 
    // indent every line by the indent amount
  static bool		IsNameValid(const String& nm); // validates a css name
  static cssProgSpace*	SetCurTop(cssProgSpace* pspc)
  { cssProgSpace* rval = cur_top; cur_top = pspc; return rval; }
  // set cur_top to given value, return previous cur_top, used for PopCurTop
  static void		PopCurTop(cssProgSpace* pspc)	{ cur_top = pspc; }
  // reset cur_top to previous value (returned by SetCurTop)
  static void		CodeConstExpr(); // use const expr for coding
  static void		CodeTop(); 	// use saved cur top for coding

  static cssElPtr 	ParseName(const String& nm);
  // lookup a name on any of the global lists of css names -- returns cssMisc::VoidElPtr if not found

  static String		GetSourceLoc(cssProg* prg);
  // get current source location information (for running program -- e.g., to report errs)
  static void 		OutputSourceLoc(cssProg* prg);
  // output current source location information (for running program -- e.g., to report errs)

  static String		last_err_msg;
  // last message from the Error function
  static void 		Error(cssProg* prg, const char* a, const char* b="", const char* c="", const char* d="",
			      const char* e="", const char* f="", const char* g="", const char* h="",
			      const char* i="");
  static String		last_warn_msg;
  // last message from the Warning function
  static void		Warning(cssProg* prg, const char* a, const char* b="", const char* c="", const char* d="",
				const char* e="", const char* f="", const char* g="", const char* h="",
				const char* i="");

public:
#if (!defined(TA_OS_WIN))
  static void 		intrcatch(int);	// interrupt exception handling
#endif
};

// for readline
extern "C" {
  extern char** css_attempted_completion(const char* text, int start, int end);
}

class CSS_API cssElPtr {
  // a pointer to an el used in code, makes pointers context relative to current frame
public:
  enum	PtrType {
    DIRECT,			// pointer points directly to the cssEl element
    CLASS_MEMBER,		// pointer is cssClassType, index is offset of member
    NVIRT_METHOD,		// pointer is cssClassType, index is offset of non-virtual method
    VIRT_METHOD,		// pointer is cssClassType, index is offset of virutal method
    PROG_AUTO,			// pointer is cssProg, index is offset into autos
    SPACE,			// pointer is cssSpace, index is offset into space
    NULL_PTR			// pointer is NULL
  };

  PtrType	ptr_type;	// type of pointer
  int		dx;		// the index (-1 means void)
  void*		ptr;		// the pointer itself

  cssEl* El() const;		// gets the el
  String PrintStr() const;
  void 	Print(ostream& fh) const;

  bool	IsNull() const	{
    bool nul = false; if(ptr_type == DIRECT) nul = !(bool)ptr;
    else nul = (!((bool)ptr) || (dx < 0)); return nul; }
  bool	NotNull() const	{ return !IsNull(); }

  operator bool() const { return NotNull(); }

  bool 	operator!=(int cv) const
  { return (cv == 0) ? !IsNull() : IsNull(); }
  bool 	operator==(int cv) const
  { return (cv == 0) ? IsNull() : !IsNull(); }
  bool 	operator==(cssElPtr& cv) const
  { return ((ptr_type == cv.ptr_type) && (dx == cv.dx) && (ptr == cv.ptr)); }
  void 	operator+=(int indx);
  void 	operator-=(int indx);

  void	SetDirect(cssEl* el)	{ ptr_type = DIRECT; ptr = (void*)el; }
  void	SetClassMember(cssClassType* cls, int idx = -1)
  { ptr_type = CLASS_MEMBER; ptr = (void*)cls; if(idx >= 0) dx = idx; }
  void	SetNVirtMethod(cssClassType* cls, int idx = -1)
  { ptr_type = NVIRT_METHOD; ptr = (void*)cls; if(idx >= 0) dx = idx; }
  void	SetVirtMethod(cssClassType* cls, int idx = -1)
  { ptr_type = VIRT_METHOD; ptr = (void*)cls; if(idx >= 0) dx = idx; }
  void	SetProgAuto(cssProg* prg, int idx = -1)
  { ptr_type = PROG_AUTO; ptr = (void*)prg; if(idx >= 0) dx = idx; }
  void	SetSpace(cssSpace* spc, int idx = -1)
  { ptr_type = SPACE; ptr = (void*)spc; if(idx >= 0) dx = idx; }

  void 	Reset() 	{ ptr_type = NULL_PTR; dx = -1; ptr = NULL; }
  cssElPtr()            { Reset(); }
  cssElPtr(cssEl* el)	{ Reset(); SetDirect(el); }
  cssElPtr(cssProg* prg, int idx = -1)	{ Reset(); SetProgAuto(prg, idx); }
  cssElPtr(cssSpace* spc, int idx = -1)	{ Reset(); SetSpace(spc, idx); }
};

class CSS_API cssElPlusIVal {
  // this is used for parsing only, where a return value and an obj are needed
public:
  cssElPtr	el;
  int		ival;
};

#ifndef CSS_NUMBER
#include "css_parse.h"
#endif

// macros for defining the cloning functions
#define cssCloneFuns(x, init) \
  cssEl* 	Clone()		{ return new x (*this); }	      \
  cssEl* 	AnonClone()  	{ return new x (*this, _nilString); }	      \
  cssEl* 	BlankClone() 	{ x* rval = new x; rval->CopyType(*this); return rval; } \
  cssEl*	MakeToken_stub(int, cssEl* arg[])		      \
  { return new x ( init , arg[1]->GetStr()); }		      \

#define cssCloneOnly(x) \
  cssEl* 	Clone()		{ return new x (*this); }	      \
  cssEl* 	AnonClone() 	{ return new x (*this, _nilString); }	      \
  cssEl* 	BlankClone() 	{ x* rval = new x(); rval->CopyType(*this); return rval; } \


class CSS_API cssEl {
  // the basic css program element
public:
  enum cssTypes {
    T_Void,			// No Type
    T_Int,			// Integer
    T_Real,			// real-valued
    T_String,			// character string type
    T_Bool,			// boolean
    T_Ptr,			// a pointer to an El
    T_Array,			// an array of els
    T_ArrayType,                // an array type definition
    T_Enum,			// instance of an enumerated type
    T_EnumType,			// an enumerated type (collection of enums)
    T_Class,			// class instance
    T_ClassType,		// class type definition
    T_ClassMbr,			// class member definition
    T_ElCFun,                	// C function returning *cssEl
    T_MbrCFun,			// C member function
    T_CodeBlock,                // Block of script code
    T_ScriptFun,                // Script function returning *cssEl
    T_MbrScriptFun,		// Script member function
    T_C_Ptr,			// a pointer to a C object
    T_TA,			// TypeAccess type
    T_PP_Def, 			// pre-processor define
    T_SubShell,			// sub-shell prog space
    T_Variant,			// Variant
    T_Int64,			// 64-bit integer
    // used for CPtr-types
    T_Short,
    T_Long,
    T_LongLong,
    T_Char,
    T_Float,
    T_DynEnum,
  };

  enum RunStat {	// css running status
    Waiting,		// waiting for input
    Running,		// running normally
    Stopping,		// got to end of program
    NewProgShoved,	// a new prog was shoved onto stack 
    Returning,		// received a return
    Breaking,		// received a break
    Continuing,		// received a continue
    BreakPoint,		// stopped in a breakpoint
    ExecError,		// execution error
    Bailing		// stop running due to shell cmd or external exit
  };

  enum ArgFlags {
    VarArg = -1,	// #IGNORE variable no of args
    NoArg  = -2 	// #IGNORE no args at all
  };

  int		refn;		// number of times referred to
  mutable String tmp_str;	// string used by parsing system
  cssElPtr*	addr;		// address of this item (if other than 'this')
  String 	name;
  cssProg*	prog;		// cur program (filled in each time)

  // all static just for consistency.. (inline below when not DEBUG compile)
  static void  		Ref(cssEl* it);
  static void	   	unRef(cssEl* it); // reduces count, but doesn't delete
  static void		Done(cssEl*); // deletes if refn<=0 
  static void		unRefDone(cssEl* it); 

public:
  // routines for reference pointers
  static void		SetRefPointer(cssEl** ptr, cssEl* it)
  { if (*ptr == it) return;  if (*ptr != &cssMisc::Void) cssEl::unRefDone(*ptr);
    *ptr = it;  if (it && (it != &cssMisc::Void)) cssEl::Ref(it); }
    // can be safely used with cssEl subtype variables of any kind
  static void		DelRefPointer(cssEl** ptr)
  { if (*ptr && (*ptr != &cssMisc::Void)) cssEl::unRefDone(*ptr); *ptr = &cssMisc::Void; }
    // can only be used with cssEl* variables

  // and reference ElPtr's
  static void		SetRefElPtr(cssElPtr& ptr, const cssElPtr& it)
  { if(ptr.El() != &cssMisc::Void)  cssEl::unRefDone(ptr.El());
    ptr = it; if(ptr.El() != &cssMisc::Void)  cssEl::Ref(ptr.El()); }
  static void		DelRefElPtr(cssElPtr& ptr)
  { if(ptr.El() != &cssMisc::Void)  cssEl::unRefDone(ptr.El()); ptr.Reset(); }

  virtual uint		GetSize() const     	{ return sizeof(*this); }
  virtual const char* 	GetName() const    	{ return (const char*)name; }
  virtual cssElPtr	GetAddr() const;
  virtual void		SetAddr(const cssElPtr& adr);
  virtual cssTypes 	GetType() const   	{ return T_Void; }
  virtual cssTypes 	GetPtrType() const   	{ return GetType(); }
  // get the type of the object pointed/referred to by this, if a this is a pointer/ref type
  virtual const char*	GetTypeName() const 	{ return "(void)"; }
  virtual cssEl*	GetTypeObject() const; // gets type object corresponding to this
  virtual int		GetParse() const	{ return CSS_VAR; }
  virtual int		IsRef() const	      	{ return false; }
  // check to see if a var is actually a ref to a var
  virtual bool		IsStringType() const   	{ return false; }
    // true for string-ish types, including cssString, cssChar, and a Variant of those types
  virtual bool		IsNumericTypeStrict() const   { return false; }
    // true for number types, including cssReal, cssInt, cssChar,  a Variant of those types
    // NOTE: Char is true for both, so know your context before assuming...
  virtual bool		HasSubProg() const	{ return false; }
  // does this object have a sub-program that gets put on the stack and run (e.g., code block, script fun)
  virtual cssProg*	GetSubProg() const	{ return NULL; }
  // get the sub-program inside this guy
  virtual cssEl*	GetActualObj() const	{ return (cssEl*)this; }
  // get non-reference, non-pointer object
  virtual cssEl*	GetNonRefObj() const	{ return (cssEl*)this; }
  // get non-reference object -- use this for all attempts to cast an object based on GetType()
  // because only cssRef defines that to be that of the non-ref-obj, which can cause
  // a crash if you then try to cast the thing..
  virtual cssEl*	GetNonPtrObj() const	{ return (cssEl*)this; }
  // get non-pointer object

  // elaborate printing (with type information, etc..
  virtual String 	PrintStr() const		// the output string
  { return String(GetTypeName()) + " " + name; }
  virtual void 		Print(ostream& fh = cout) const	{ fh << PrintStr(); }

  virtual void		PrintR(ostream& fh = cout) const { Print(fh); }
  // recursive print item (item and all its sub-elements)
  virtual int		Edit(bool =false)	{ return false; }
  // pull up in the interface and edit this one

  // "value" printing
  virtual String	PrintFStr() const 		 { return "void"; }
  virtual void 		PrintF(ostream& fh = cout) const { fh << PrintFStr(); }

  virtual void  	TypeInfo(ostream& fh = cout) const
  { fh << GetTypeName() << " " << name; }
  virtual void		InheritInfo(ostream& fh = cout) const { TypeInfo(fh); }

  // saving and loading objects to/from files (special format)
  virtual void		Save(ostream& strm = cout);
  virtual void		Load(istream& strm = cin);

  // token information about a certain type
  virtual void		TokenInfo(ostream&) const	{ }; // show tokens of arg type
  virtual cssEl*	GetToken(int) const		{ return (cssEl*)this; }

  virtual cssEl::RunStat 	Do(cssProg* prg);  // run this object
  virtual cssEl::RunStat 	FunDone(cssProg* prg);
  // after function has been run, do final stuff

  // constructors
  void 		Constr();
#ifdef CSS_DEBUG_REGISTER
  void		Register(); 	// for debugging purposes
#define CSS_REGISTER  Register();
#else
#define CSS_REGISTER 
#endif

  cssEl()			{ Constr(); }
  cssEl(const String& nm) 	{ Constr(); name = nm; }
  cssEl(const cssEl& cp)	{ Constr(); Copy(cp); name = cp.name; }
  cssEl(const cssEl& cp, const String& nm) { Constr(); Copy(cp); name = nm; }

  virtual ~cssEl();

  void   		Copy(const cssEl& cp);
  // for copying to existing structs -- does NOT copy name -- must call inherited
  void   		CopyType(const cssEl& cp) { };
  // only copies type-level information, not value-level information (for els that carry type information around)

  virtual cssEl* 	Clone() 			{ return new cssEl(*this); }
  virtual cssEl* 	AnonClone() 			{ return new cssEl(*this, _nilString); }
  virtual cssEl* 	BlankClone() 			{ return new cssEl; }

  virtual cssEl*	MakeToken_stub(int, cssEl**) 	{ return &cssMisc::Void; }
  virtual cssEl*	MakePtrType(int ptrs);
  virtual cssEl*	MakeRefType();

  virtual cssEl::RunStat 	MakeToken(cssProg* prg);
  virtual cssEl::RunStat 	MakeTempToken(cssProg* prg); //

  void NopErr(const char* opr="") const
  { cssMisc::Error(prog, "Operation:", opr, "not defined for type:", GetTypeName()); }

  // use this for conversions that don't work (safer)
  void CvtErr(const char* opr="") const
  { cssMisc::Error(prog, "Conversion:", opr, "not defined for type:", GetTypeName()); }

  // these are the core converters that classes should define.
  // the base cssEl converts like a void.
  virtual String GetStr() const 		{ return _nilString; }
  virtual Variant GetVar() const 		{ return _nilVariant; }
  virtual operator Real() const	 		{ return 0; }
  virtual operator Int() const	 		{ return 0; }
  virtual operator ta_int64_t() const	 	{ return 0LL; }
  virtual operator ta_uint64_t() const	 	{ return 0ULL; }
  virtual operator bool() const			{ return (Int)*this; }

  // these are never redefined: every class defines a GetStr and GetVar instead --
  // strings and variants are the lowest common denominator and thus always avail
          operator String() const		{ return GetStr(); }
	  operator const char*() const 		{ return (const char*)GetStr(); }
          operator Variant() const		{ return GetVar(); }

  // these misc "nuisance" equivalent type converters and are typically not overwritten
  // except for exact c_ptr types
  virtual operator char*() const		{ return (char*)(const char*)GetStr(); }
  virtual operator float() const 		{ return (float)(Real)*this; }
  virtual operator unsigned int() const 	{ return (unsigned int)(Int)*this; }
  virtual operator char() const	 		{ return (char)(Int)*this; }
  virtual operator signed char() const 		{ return (signed char)(Int)*this; }
  virtual operator unsigned char() const 	{ return (unsigned char)(Int)*this; }
  virtual operator short() const    		{ return (short)(Int)*this; }
  virtual operator unsigned short() const	{ return (unsigned short)(Int)*this; }
  virtual operator long() const	 		{ return (long)(Int)*this; }
  virtual operator unsigned long() const	{ return (unsigned long)(Int)*this; }
  virtual operator unsigned char*() const	{ return (unsigned char*)(const char*)*this; }
#ifndef NO_SIGNED
  virtual operator signed char*() const		{ return (signed char*)(const char*)*this; }
#endif
#if NEED_STREAMOFF_CSS_CONV
  virtual operator streamoff() const 		{ return (streamoff)(Int)*this; }
#endif

  // pointer types
  virtual void* GetVoidPtrOfType(TypeDef* td) const 	{ CvtErr(td->name); return NULL; }
  virtual void* GetVoidPtrOfType(const String& td) const 	{ CvtErr(td); return NULL; }
  // these are type-safe ways to convert a cssEl into a ptr to object of given type

  virtual operator void*() const		{ CvtErr("(void*)"); return NULL; }
  virtual operator void**() const		{ CvtErr("(void**)"); return NULL; }

  virtual operator int*() const		{ CvtErr("(int*)"); return NULL; }
  virtual operator short*() const	{ CvtErr("(short*)"); return NULL; }
  virtual operator long*() const	{ CvtErr("(long*)"); return NULL; }
  virtual operator double*() const	{ CvtErr("(double*)"); return NULL; }
  virtual operator float*() const	{ CvtErr("(float*)"); return NULL; }
  virtual operator String*() const	{ CvtErr("(String*)"); return NULL; }
  virtual operator Variant*() const	{ CvtErr("(Variant*)"); return NULL; }
  virtual operator bool*() const	{ CvtErr("(bool*)"); return NULL; }

  virtual operator int**() const	{ CvtErr("(int**)"); return NULL; }
  virtual operator short**() const	{ CvtErr("(short**)"); return NULL; }
  virtual operator long**() const	{ CvtErr("(long**)"); return NULL; }
  virtual operator double**() const	{ CvtErr("(double**)"); return NULL; }
  virtual operator float**() const	{ CvtErr("(float**)"); return NULL; }
  virtual operator String**() const	{ CvtErr("(String**)"); return NULL; }
  virtual operator Variant**() const	{ CvtErr("(Variant**)"); return NULL; }
  virtual operator bool**() const	{ CvtErr("(bool**)"); return NULL; }

  virtual operator ostream*() const	{ CvtErr("(ostream*)"); return NULL; }
  virtual operator istream*() const	{ CvtErr("(istream*)"); return NULL; }
  virtual operator iostream*() const	{ CvtErr("(iostream*)"); return NULL; }
  virtual operator fstream*() const	{ CvtErr("(fstream*)"); return NULL; }
  virtual operator stringstream*() const { CvtErr("(stringstream*)"); return NULL; }

  virtual operator ostream**() const	{ CvtErr("(ostream**)"); return NULL; }
  virtual operator istream**() const	{ CvtErr("(istream**)"); return NULL; }
  virtual operator iostream**()	const	{ CvtErr("(iostream**)"); return NULL; }
  virtual operator fstream**() const	{ CvtErr("(fstream**)"); return NULL; }
  virtual operator stringstream**() const { CvtErr("(stringstream**)"); return NULL; }

  // support for external types
  virtual operator taBase*() const	{ CvtErr("(taBase*)"); return NULL; }
  virtual operator taBase**() const 	{ CvtErr("(taBase**)"); return NULL; }
  virtual operator TypeDef*() const	{ CvtErr("(TypeDef*)"); return NULL; }
  virtual operator MemberDef*() const	{ CvtErr("(MemberDef*)"); return NULL; }
  virtual operator MethodDef*() const	{ CvtErr("(MethodDef*)"); return NULL; }

  // assign from types
  virtual void operator=(Real)	 		{ NopErr("=(Real)"); }
  virtual void operator=(Int)			{ NopErr("=(Int)"); }
  virtual void operator=(ta_int64_t cp)		{ operator=((Int)cp); }
  virtual void operator=(ta_uint64_t cp)		{ operator=((Int)cp); }
  virtual void operator=(const String&)	 	{ NopErr("=(String)"); }
  virtual void operator=(const Variant& val); 
  // usually not overridden, just dispatches according to type

  virtual void operator=(void*)	 		{ NopErr("=(void*)"); }
  virtual void operator=(void**)		{ NopErr("=(void**)"); }
  virtual void operator=(taBase*) 		{ NopErr("=(taBase*)"); }
  virtual void operator=(taBase**) 		{ NopErr("=taBase**)"); }

  virtual void operator=(const cssEl&) 		{ NopErr("="); }

  // operators
  virtual void CastFm(const cssEl& cp)	{ operator=(cp); } // default cast is a copy
  virtual void InitAssign(const cssEl& cp) { operator=(cp); } // default initial assign is cpy
  virtual void AssignFromType(TypeDef* td, void*)	{ CvtErr(td->name); }
  virtual void AssignFromType(const char* td, void*)	{ CvtErr(td); }
  // type-safe way to assign a void ptr of given type to a cssEl

  virtual void UpdateAfterEdit() { };

  virtual cssEl* operator+(cssEl&) { NopErr("+"); return &cssMisc::Void; }
  virtual cssEl* operator-(cssEl&) { NopErr("-"); return &cssMisc::Void; }
  virtual cssEl* operator*(cssEl&) { NopErr("*"); return &cssMisc::Void; }
  virtual cssEl* operator/(cssEl&) { NopErr("/"); return &cssMisc::Void; }
  virtual cssEl* operator%(cssEl&) { NopErr("%"); return &cssMisc::Void; }
  virtual cssEl* operator<<(cssEl&){ NopErr("<<"); return &cssMisc::Void; }
  virtual cssEl* operator>>(cssEl&){ NopErr(">>"); return &cssMisc::Void; }
  virtual cssEl* operator&(cssEl&) { NopErr("&"); return &cssMisc::Void; }
  virtual cssEl* operator^(cssEl&) { NopErr("^"); return &cssMisc::Void; }
  virtual cssEl* operator|(cssEl&) { NopErr("|"); return &cssMisc::Void; }
  virtual cssEl* operator-()       { NopErr("-"); return &cssMisc::Void; } // unary minus
  virtual cssEl* operator*()	   { NopErr("*"); return &cssMisc::Void; } // unary de-ptr
  virtual cssEl* operator[](Variant) const { NopErr("[]"); return &cssMisc::Void; }

  static cssEl* GetElFromTA(TypeDef* td, void* itm, const String& nm, 
			    MemberDef* md = NULL, cssEl* class_parent = NULL);
  // Call this function to get an appropriate cssEl object based on typedef information

  virtual bool	MembersDynamic()	{ return false; }
  // are members always dynamically looked up?  if so, don't issue warnings about this
  virtual int	GetMemberNo(const String&) const { NopErr(".,->"); return -1; }
  // this is called during parsing to compile in an index for the member, instead of looking up by name -- return -1 if member lookup should be dynamic (e.g., if a pointer and type might change later)
  virtual cssEl* GetMemberFmNo(int) const  { NopErr(".,->"); return &cssMisc::Void; }
  // subsequent function to actually get the member el from the number
  virtual cssEl* GetMemberFmName(const String& nm) const  { NopErr(".,->"); return &cssMisc::Void; }
  // dynamic version that takes the name and gets the el

  virtual int	 GetMethodNo(const String&) const { NopErr(".,->()"); return -1; }
  // see above for members: get index to method for strong types
  virtual cssEl* GetMethodFmNo(int) const { NopErr(".,->()"); return &cssMisc::Void; }
  virtual cssEl* GetMethodFmName (const String& nm) const { NopErr(".,->()"); return &cssMisc::Void; }

  virtual cssEl* GetScoped(const String& nm) const { NopErr("::"); return &cssMisc::Void; }
  // get  a scoped type element (type::thing)

  virtual cssEl* NewOpr();
  virtual void 	 DelOpr()	{ NopErr("delete"); } // delete operator

  // these generally should not be overwritten:
  bool operator! () 	    { return !(bool)*this; }
  bool operator&&(cssEl& s) { return (bool)*this && (bool)s; }
  bool operator||(cssEl& s) { return (bool)*this || (bool)s; }

  virtual bool operator< (cssEl&) { NopErr("<"); return 0; }
  virtual bool operator> (cssEl&) { NopErr(">"); return 0; }
  virtual bool operator<=(cssEl&) { NopErr("<="); return 0; }
  virtual bool operator>=(cssEl&) { NopErr(">="); return 0; }
  virtual bool operator==(cssEl&) { NopErr("=="); return 0; }
  virtual bool operator!=(cssEl&) { NopErr("!="); return 0; }

  virtual void operator+=(cssEl&) { NopErr("+="); }
  virtual void operator-=(cssEl&) { NopErr("-="); }
  virtual void operator*=(cssEl&) { NopErr("*="); }
  virtual void operator/=(cssEl&) { NopErr("/="); }
  virtual void operator%=(cssEl&) { NopErr("%="); }
  virtual void operator<<=(cssEl&){ NopErr("<<="); }
  virtual void operator>>=(cssEl&){ NopErr(">>="); }
  virtual void operator&=(cssEl&) { NopErr("&="); }
  virtual void operator^=(cssEl&) { NopErr("^="); }
  virtual void operator|=(cssEl&) { NopErr("|="); }

protected:
  int	 GetMemberNo_impl(TypeDef* typ, const String&) const;
  cssEl* GetMemberFmNo_impl(TypeDef* typ, void* base, int memb) const;
  cssEl* GetMemberFmName_impl(TypeDef* typ, void* base, const String& memb) const;
  cssEl* GetMemberEl_impl(TypeDef* typ, void* base, MemberDef* md) const;

  int	 GetMethodNo_impl(TypeDef* typ, const String& meth) const;
  cssEl* GetMethodFmNo_impl(TypeDef* typ, void* base, int meth) const;
  cssEl* GetMethodFmName_impl(TypeDef* typ, void* base, const String& meth) const;
  cssEl* GetMethodEl_impl(TypeDef* typ, void* base, MethodDef* md) const;

  cssEl* GetScoped_impl(TypeDef* typ, void* base, const String& nm) const;

  cssEl* GetVariantEl_impl(const Variant& val, Variant idx) const; // helper for operator[]
};

#ifndef DEBUG
inline void cssEl::Ref(cssEl* it) { ++(it->refn); }
inline void cssEl::unRef(cssEl* it) { if(it) --(it->refn); }
inline void cssEl::unRefDone(cssEl* it) { if (it && --(it->refn) <= 0) delete it;}
inline void cssEl::Done(cssEl* it) { if (it->refn <= 0) delete it;}
#endif

class CSS_API cssSpace {
  // a name space of some sort, can be a stack, or a list
protected:
friend	class cssArray;
friend  class cssClassType;
friend  class cssClassInst;
  int 		alloc_size;		// allocated size of space
public:
  cssElPtr 	el_retv;		// return value for a Find
  String 	name;
  int 		size;			// number of actual elements
  cssEl**	els;			// the elements themselves

  static ostream& fancy_list(ostream& fh, const String& itm, int no, int prln,
			     int tabs, int indent = 0);
  static pager_ostream& fancy_list(pager_ostream& fh, const String& itm, int no, int prln,
			     int tabs, int indent = 0);
  static String& fancy_list(String& fh, const String& itm, int no, int prln,
			    int tabs, int indent = 0);

  void 		Constr();
  cssSpace()				{ alloc_size = 2;  Constr(); }
  cssSpace(const String& nm)		{ alloc_size = 2;  name = nm;  Constr(); }
  cssSpace(int no, const String& nm)	{ alloc_size = no; name = nm;  Constr(); }
  cssSpace(const cssSpace& cp)		{ alloc_size = cp.alloc_size; Constr();  Copy(cp); }
  virtual ~cssSpace()			{ Reset();  free(els); }

  void Copy(const cssSpace& cp);
  void Copy_NoNames(const cssSpace& cp);    // copy items but not any names
  void Copy_Blanks(const cssSpace& cp);	    // copy item types but no content

  void CopyUniqNameNew(const cssSpace& cp); // copy only unique items, keep new one
  void CopyUniqNameOld(const cssSpace& cp); // copy only unique items, keep old one

  cssElPtr&	FindName(const String& nm); // lookup by name
  cssElPtr&	Find(Int nm);		// lookup by number value
  cssElPtr&	Find(Real nm);		// lookup by number value
  cssElPtr&	Find(const String& nm); // lookup by string value
  int		GetIndex(cssEl* it);	// find it, return index
  int		IndexOfName(const String& nm) const; // return index of name, -1 if not found

  void 		List(ostream& fh = cout, int indent = 0, int per_line = -1) const;	// (elaborate print format)
  void 		List(pager_ostream& fh, int indent = 0, int per_line = -1) const;	// (elaborate print format)
  void 		NameList(ostream& fh = cout, int indent = 0, int per_line = -1) const;   // just the names
  void 		NameList(pager_ostream& fh, int indent = 0, int per_line = -1) const;   // just the names
  void		ValList(ostream& fh = cout, int indent = 0, int per_line = -1) const;    // just the values (printf format)
  void		TypeNameList(ostream& fh = cout, int indent = 0) const; // "fancy" type/name output
  void		TypeNameValList(ostream& fh = cout, int indent = 0) const; // "fancy" type/name/val output
  String	PrintStr(int indent = 0, int per_line = -1) const;
  String	PrintFStr(int indent = 0, int per_line = -1) const;

  void		Alloc(int sz);		// allocate space on the list..
  void 		Reset();		// clear list
  void		Sort();			// sort list

  // for stack-like operations
  cssElPtr&	Push(cssEl* it);
  cssElPtr&	PushUniqNameNew(cssEl* it);
  cssElPtr&	PushUniqNameOld(cssEl* it);
  cssEl*	Pop()
  { if(size == 0) return &cssMisc::Void; cssEl* rval = els[--size]; cssEl::unRef(rval);
    return rval; }
  // get last guy, remove from space, and unRef it, but not done -- assumes item is multiply reffed or will be Done'd later (probably not the best design..)
  cssEl*	Pop_NoUnRef()
  { if(size == 0) return &cssMisc::Void; cssEl* rval = els[--size]; return rval; }
  // just gets the last guy and doesn't unref -- for arg binding -- unrefdone later!
  void		DelPop()
  { cssEl* rval = Pop(); if(rval != &cssMisc::Void) cssEl::Done(rval); }
  // pop and delete (done) item
  bool		Remove(cssEl* it); 	// not a good idea, because ptrs are index based
  bool		Replace(cssEl* old, cssEl* nw); // replace at index with it
  cssEl*	Peek() const
  { cssEl* rval=&cssMisc::Void; if(size > 0) rval = els[size-1]; return rval; }

  // operators
  cssEl* 	FastEl(int i) const	{ return els[i]; }
  cssEl*	El(int i) const
  { cssEl* rval=&cssMisc::Void; if(i < size) rval=els[i]; return rval; }
  cssEl* operator[](int i) const	{ return El(i); }
};


class CSS_API cssElFun : public cssEl {
  // basic things for function-type objects (this is a base type for other classes)
public:
  static const int ArgMax;      // maximum number of arguments
  cssSpace	arg_defs;	// default values for arguments
  String_Array	arg_vals;	// current arg values (as strings), for gui stuff
  int		def_start;	// where in args do the defaults start
  int		argc;		// number of args desired
  RunStat	dostat;		// return status value for do
  cssEl*	retv_type;	// return value type

  void		SetRetvType(cssEl* rvt) { cssEl::SetRefPointer(&retv_type, rvt); }

  int		GetParse() const	{ return CSS_FUN; }
  cssEl*	GetTypeObject() const	{ return (cssEl*)this; }

  virtual int   BindArgs(cssEl** args, int& act_argc);     // returns actual no of args
  virtual void  DoneArgs(cssEl** args, int& act_argc);     // Done with args
  virtual void	GetArgDefs();	// get argument defaults

  void		Copy(const cssElFun& cp);
  void		CopyType(const cssElFun& cp) { Copy(cp); }
  cssElFun();
  ~cssElFun();
};

class CSS_API cssElCFun : public cssElFun {
  // a C function returning a cssEl pointer
public:
  int		parse;				// parser token code
  cssEl*	(*funp)(int ac, cssEl* args[]); // function pointer
  String	help_str;			// help string for function

  int		GetParse() const	{ return parse; }
  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const		{ return T_ElCFun; }
  const char*	GetTypeName() const	{ return "(ElCFun)"; }
  cssEl*	GetTypeObject() const	{ return (cssEl*)this; }

  cssEl::RunStat 	Do(cssProg* prog);

  // constructors
  void 		Constr();
  void		Copy(const cssElCFun& cp);
  void		CopyType(const cssElCFun& cp) { Copy(cp); }

  cssElCFun();
  cssElCFun(int ac, cssEl* (*fp)(int, cssEl* args[]));
  cssElCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const String& nm);
  cssElCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const String& nm, int pt, const String& hstr=NULL);
  cssElCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const String& nm, cssEl* rtype, const String& hstr=NULL);
  cssElCFun(const cssElCFun& cp);
  cssElCFun(const cssElCFun& cp, const String& nm);
  ~cssElCFun();

  cssCloneOnly(cssElCFun);
  cssEl*	MakeToken_stub(int na, cssEl* arg[]);
  // return retv_type token, else cssInt
  String GetStr() const	{ return name; } // types can give strings...
  Variant GetVar() const { return name; }
};

// pt can be either the parsetype or the retv_type
#define cssElCFun_inst(l,x,n,pt,hl)	l .Push(new cssElCFun(n, cssElCFun_ ## x ## _stub, #x, pt, hl))
#define cssElCFun_inst_nm(l,x,n,s,pt,hl) l .Push(new cssElCFun(n, cssElCFun_ ## x ## _stub, s, pt, hl))
#define cssElCFun_inst_ptr(l,x,n,pt,hl)	l .Push(cssBI::x = new cssElCFun(n, cssElCFun_ ## x ## _stub, #x, pt, hl))
#define cssElCFun_inst_ptr_nm(l,x,n,s,pt,hl) l .Push(cssBI::x = new cssElCFun(n, cssElCFun_ ## x ## _stub, s, pt, hl))

class CSS_API cssElInCFun : public cssElCFun {
  // a simple internal C function (having fixed number of parameters)
public:
  int		BindArgs(cssEl** args, int& act_argc);
  // has a simpler, faster version of bind args for internal functions

  // constructors
  cssElInCFun();
  cssElInCFun(int ac, cssEl* (*fp)(int, cssEl* args[]));
  cssElInCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const String& nm);
  cssElInCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const String& nm, int pt);
  cssElInCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const String& nm, cssEl* rtype);
  cssElInCFun(const cssElInCFun& cp);
  cssElInCFun(const cssElInCFun& cp, const String& nm);

  cssCloneOnly(cssElInCFun);
};

#define cssElInCFun_inst(l,x,n,pt)	l .Push(new cssElInCFun(n, cssElCFun_ ## x ## _stub, #x, pt))
#define cssElInCFun_inst_nm(l,x,n,s,pt) l .Push(new cssElInCFun(n, cssElCFun_ ## x ## _stub, s, pt))
#define cssElInCFun_inst_ptr(l,x,n,pt) 	l .Push(cssBI::x = new cssElInCFun(n, cssElCFun_ ## x ## _stub, #x, pt))
#define cssElInCFun_inst_ptr_nm(l,x,n,s,pt) l .Push(cssBI::x = new cssElInCFun(n, cssElCFun_ ## x ## _stub, s, pt))

class CSS_API cssMbrCFun : public cssElFun {
  // a C code member function returning a pointer to a cssEl
public:
  cssEl*	(*funp)(void* ths, int na, cssEl* args[]); // function pointer
  void* 	ths;					   // type instance

  cssTypes 	GetType() const		{ return T_MbrCFun; }
  const char*	GetTypeName() const	{ return "(MbrCFun)"; }
  cssEl*	GetTypeObject() const	{ return (cssEl*)this; }

  cssEl::RunStat Do(cssProg* prog);

  // constructors
  void		Constr();
  void		Copy(const cssMbrCFun& cp);
  void		CopyType(const cssMbrCFun& cp) { Copy(cp); }

  cssMbrCFun();
  cssMbrCFun(int ac, void* th, cssEl* (*fp)(void*, int, cssEl**));
  cssMbrCFun(int ac, void* th, cssEl* (*fp)(void*, int, cssEl**), const String& nm);
  cssMbrCFun(const cssMbrCFun& cp);
  cssMbrCFun(const cssMbrCFun& cp, const String& nm);
  ~cssMbrCFun();

  cssCloneOnly(cssMbrCFun);

  cssEl*	MakeToken_stub(int na, cssEl* arg[]);
};

// return variable name
#define cssRetv_Name "_retv_this"
// block function name
#define cssBlock_Name "_block"
// block conditional variable
#define cssCondBlock_Name "_cond"
// if_true block
#define cssIfTrueBlock_Name "_if_true"
// if_false block
#define cssElseBlock_Name "_else"
// for loop
#define cssForLoop_Name "_for_loop"
// for incr
#define cssForIncr_Name "_for_incr"
// for loop stmts
#define cssForLoopStmt_Name "_for_loop_stmt"
// do loop
#define cssDoLoop_Name "_do_loop"
// switch block cond variable
#define cssSwitchVar_Name "_switch_this"
// switch block name
#define cssSwitchBlock_Name "_switch_block"
// switch jump table values
#define cssSwitchJump_Name "_switch_jump"
// switch default case name
#define cssSwitchDefault_Name "_switch_default"

class CSS_API cssCodeBlock : public cssElFun {
  // a block of code between { }
public:
  enum Actions {
    JUST_CODE,			// don't do anything special; just a regular code block
    PUSH_RVAL,			// push return value on stack (for conditional expression)
    IF_TRUE,			// only run if stack value evaluates to non-0
    ELSE,			// only run if stack value evaluates to 0 (else)
  };

  enum LoopType {
    NOT_LOOP,			// not a loop block
    WHILE,
    DO,
    FOR,
    SWITCH,			// not a loop technically, but processes break
  };

  cssProg*	owner_prog;	// next higher prog that I belong to
  cssProg*	code;		// sub prog containing the actual code
  Actions	action;		// special action to perform
  int		loop_back;	// if run, set the PC() of the calling prog back this number after running
  LoopType	loop_type;	// type of loop function
  
  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const 	{ return T_CodeBlock; }
  const char*	GetTypeName() const 	{ return "(CodeBlock)"; }
  cssEl*	GetTypeObject() const	{ return (cssEl*)this; }
  bool		HasSubProg() const    	{ return true; }
  cssProg*	GetSubProg() const 	{ return code; }

  cssEl::RunStat Do(cssProg* prg);
  cssEl::RunStat FunDone(cssProg* prg);

  bool		CleanDoubleBlock(); // if block just contains an embedded block, move it up

  String	PrintStr() const;
  String	PrintFStr() const		{ return PrintStr(); }

  // constructors
  void		Constr();
  void		Copy(const cssCodeBlock& cp);
  void		CopyType(const cssCodeBlock& cp) { Copy(cp); }

  cssCodeBlock();
  cssCodeBlock(const String& nm);
  cssCodeBlock(const cssCodeBlock& cp);
  cssCodeBlock(const cssCodeBlock& cp, const String& nm);
  ~cssCodeBlock();

  cssCloneOnly(cssCodeBlock);
  cssEl*	MakeToken_stub(int na, cssEl* arg[]);
  // return retv_type token, else cssInt
};

class CSS_API cssScriptFun : public cssElFun {
  // a function defined in the script language
public:
  cssElPtr* 	argv;		// the actual argument holders (0 is retv)
  cssProg*	fun;		// sub prog containing function code

  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const 	{ return T_ScriptFun; }
  const char*	GetTypeName() const 	{ return "(ScriptFun)"; }
  cssEl*	GetTypeObject() const	{ return (cssEl*)this; }
  bool		HasSubProg() const      { return true; }
  cssProg*	GetSubProg() const 	{ return fun; }

  cssEl::RunStat Do(cssProg* prg);
  cssEl::RunStat FunDone(cssProg* prg);

  String	PrintStr() const;
  String	PrintFStr() const		{ return PrintStr(); }

  virtual void	Define(cssProg* prg, bool decl = false, const String& nm = _nilString);
  // initialize the function (decl = true if in declaration, not definition)

  // constructors
  void		Constr();
  void		Copy(const cssScriptFun& cp);
  void		CopyType(const cssScriptFun& cp) { Copy(cp); }

  cssScriptFun();
  cssScriptFun(const String& nm);
  cssScriptFun(const cssScriptFun& cp);
  cssScriptFun(const cssScriptFun& cp, const String& nm);
  ~cssScriptFun();

  cssCloneOnly(cssScriptFun);
  cssEl*	MakeToken_stub(int na, cssEl* arg[]);
  // return retv_type token, else cssInt
};

class CSS_API cssMbrScriptFun : public cssScriptFun {
  // a function defined in the script language
public:
  cssClassType*	type_def;	// the class fun belongs to
  String	desc;		// a description of this function
  String	opts;		// user-settable comment directives (from desc)
  bool		is_tor;		// is a constructor or destructor
  bool		is_virtual;	// is a virtual function

  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const 	{ return T_MbrScriptFun; }
  const char*	GetTypeName() const 	{ return "(MbrScriptFun)"; }

  cssEl::RunStat Do(cssProg* prg);
  cssEl::RunStat FunDone(cssProg* prg);

  String	PrintStr() const;

  void		Define(cssProg* prg, bool decl = false, const String& nm = _nilString);
  // initialize the function

  void		SetDesc(const String& des); // get options from desc
  bool		HasOption(const String& opt) { return opts.contains(opt); }
  String	OptionAfter(const String& opt);

  void		Constr();
  void		Copy(const cssMbrScriptFun& cp);
  void		CopyType(const cssMbrScriptFun& cp) { Copy(cp); }

  cssMbrScriptFun();
  cssMbrScriptFun(const String& nm, cssClassType* cls);
  cssMbrScriptFun(const cssMbrScriptFun& cp);
  cssMbrScriptFun(const cssMbrScriptFun& cp, const String& nm);
  ~cssMbrScriptFun();

  cssCloneOnly(cssMbrScriptFun);
  cssEl*	MakeToken_stub(int na, cssEl* arg[]);
  // return retv_type token, else cssInt
};

#define cssCPtr_CloneFuns(x, init) \
  cssEl* 	Clone()		{ return new x (*this); }	      \
  cssEl* 	AnonClone() 	{ return new x (*this, _nilString); }	      \
  cssEl* 	BlankClone() 	{ x* rval = new x; rval->CopyType(*this); return rval; } \
  cssEl*	MakeToken_stub(int, cssEl* arg[])		      \
    { return new x ( init, ptr_cnt, arg[1]->GetStr()); }	      \


class CSS_API cssCPtr : public cssEl {
  // base class for ptrs to C objects
public:
  enum PtrFlags {
    NO_PTR_FLAGS = 0x00,	// no flags set
    READ_ONLY = 0x01,		// object comes from read-only member: contents should not change (see cssMisc::obey_read_only -- css ignores this by default)
    OWN_OBJ = 0x02,		// I own the object pointed to (e.g., if ptr_cnt == 0)
  };
    
  void* 	ptr;		// pointer to the C++ object
  int		ptr_cnt;	// number of ptrs (0 = obj itself (ptr is still a *), 1= obj*, 2= obj**) values higher than 2 are NOT supported
  PtrFlags	flags;		// flags controlling ptr
  cssEl*	class_parent;	// if this pointer was derived from a class structure

  int		GetParse() const	{ return CSS_PTR; }
  uint		GetSize() const		{ return 0; } // use for ptrs
  cssTypes 	GetType() const		{ return T_C_Ptr; }
  cssTypes	GetPtrType() const	{ return T_C_Ptr; } // still just a c ptr
  const char*	GetTypeName() const 	{ return "(C_Ptr)"; }
  String 	PrintStr() const;
  String	PrintFStr() const 	{ return GetStr(); }

  // constructors
  void		Constr();
  void		Copy(const cssCPtr& cp);
  void		CopyType(const cssCPtr& cp);

  cssCPtr();
  cssCPtr(void* it, int pc, const String& nm = _nilString, cssEl* cls_par = NULL, bool ro = false);
  cssCPtr(const cssCPtr& cp);
  cssCPtr(const cssCPtr& cp, const String& nm);
  ~cssCPtr();

  cssCPtr_CloneFuns(cssCPtr, (void*)NULL);

  virtual void	SetClassParent(cssEl* cp);
  virtual void	UpdateClassParent();
  // call updateafteredit on class parent (only if 

  // converters
  virtual void*	GetVoidPtr(int cnt = 1) const;	// goes down till ptr_cnt == cnt
  virtual void* GetNonNullVoidPtr(const char* opr="", int cnt = 1) const;
  // generates error if null

  // GetStr and GetVar are both nil as in base
  operator bool() const		{ return (bool)ptr; } // test for ptr null..
 
  operator Real() const		{ CvtErr("(Real)"); return 0.0; }
  operator Int() const		{ CvtErr("(Int)"); return 0; }
  operator ta_int64_t() const	{ CvtErr("(ta_int64_t)"); return 0LL; }
  operator ta_uint64_t() const	{ CvtErr("(ta_uint64_t)"); return 0ULL; }

  operator void*() const	{ return GetVoidPtr(1); }
  operator void**() const	{ return (void**)GetVoidPtr(2); }

  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }

  // operators
  virtual bool	ROCheck();	// do read_only check, true if ok to modify, else err
  virtual void 	PtrAssignPtr(const cssEl& s); // call this in operator= when ptr_cnt > 0
  virtual bool 	PtrAssignNullInt(const cssEl& s);
  // check to see if source is an int with value 0, if so, set pointer to null and return true
  virtual void 	PtrAssignNull();
  // assign a null to pointer depends on ptr_cnt, calls PtrAssignPtrPtr if needed
  virtual bool 	AssignCheckSource(const cssEl& s);
  // do basic type checks on source for assign -- return false if not matching ptr type
  virtual bool 	PtrAssignPtrPtr(void* new_ptr_val);
  // we are a pointer-pointer -- set our pointer to new ptr value (special smart pointer refcounting etc might be needed) -- returns false if failed

  void operator=(const cssEl& s);

  void	UpdateAfterEdit();

  cssEl* operator*();
  cssEl* operator*(cssEl&)	{ NopErr("*"); return &cssMisc::Void; }

  virtual bool 	SamePtrLevel(cssCPtr* s); // if this and s have diff cnt, emit warning

  bool operator==(cssEl& s);	// these two check for sameptrlevel
  bool operator!=(cssEl& s);
};

#define cssCPtr_inst(l,n)		l .Push(new cssCPtr(& n,1,#n))
#define cssCPtr_inst_nm(l,n,c,s)	l .Push(new cssCPtr(n,c, s))
#define cssCPtr_inst_ptr(l,n,x)	l .Push(cssBI::x = new cssCPtr(& n,1,#x))
#define cssCPtr_inst_ptr_nm(l,n,c,x,s) l .Push(cssBI::x = new cssCPtr(n,c,s))


// lexer & preprocessor stuff (#define, etc)

class CSS_API cssLex {
public:
  static String Buf;

  static int readword(cssProg* prog, int c);
};

class CSS_API cssDef : public cssElFun {
  // a pre-processor define
public:
  String_Array	val;		// contains the bits and pieces that surround the args
  int_Array	which_arg;	// determines which arg to insert before each val item

  int		GetParse() const	{ return CSS_PP_DEF; }
  uint		GetSize() const 	{ return sizeof(*this); }
  cssTypes 	GetType() const 	{ return T_PP_Def; }
  const char*	GetTypeName() const  	{ return "#define"; }

  cssEl::RunStat 	Do(cssProg* prog);

  static void	Skip_To_Endif(cssProg* prog); // skip text to next endif

  void	Constr();

  cssDef();
  cssDef(int ac);
  cssDef(int ac, const String& nm);
  cssDef(const cssDef& cp);
  cssDef(const cssDef& cp, const String& nm);

  cssCloneFuns(cssDef, 0);
};

class CSS_API cssInst {
public:
  enum CodeFlags {
    Stop = -1 				// signal to stop execution
  };

  cssProg*	prog;			// program I belong to
  int 		idx;			// index within the program
  int 		line;			// points to source code line number
  int 		col;			// column in line
  cssElPtr	inst;			// instruction this points to

  virtual String	PrintStr() const;
  virtual void 		ListSrc(pager_ostream& fh, int indent = 0) const; // source code
  virtual void		ListMachine(pager_ostream& fh, int indent = 0) const; // machine impl

  virtual cssEl::RunStat Do();
  virtual void 		SetJump(css_progdx it) 	{ };
  virtual css_progdx	GetJump()		{ return -1; }
  virtual bool		IsJump()		{ return false; }

  void 			SetInst(const cssElPtr& it);

  // constructors
  void 		Constr();
  void		Copy(const cssInst& cp);
  cssInst();
  cssInst(const cssProg* prg, const cssElPtr& it);
  cssInst(const cssProg* prg, const cssElPtr& it, int lno, int clno);
  cssInst(const cssInst& cp);
  virtual ~cssInst();

  virtual cssInst* Clone() { return new cssInst(*this); }
};

class CSS_API cssIJump : public cssInst {
public:
  css_progdx    jumpto;			// idx to jump to

  override String	PrintStr() const;
  override void		ListMachine(pager_ostream& fh, int = 0) const;
  override cssEl::RunStat 	Do();
  override void 	SetJump(css_progdx it) 		{ jumpto = it; }
  override css_progdx	GetJump()			{ return jumpto; }
  override bool		IsJump()			{ return true; }

  void		Copy(const cssIJump& cp);
  cssIJump(const cssProg* prg, css_progdx jmp);
  cssIJump(const cssProg* prg, css_progdx jmp, int lno, int clno);
  cssIJump(const cssIJump& cp);

  virtual cssInst* Clone() { return new cssIJump(*this); }
};

class CSS_API cssFrame {
public:
  cssProg*	prog;			// program I belong to
  int		fr_no;		        // frame number
  css_progdx 	pc;			// program counter
  cssSpace	stack;			// current stack
  cssSpace	autos;			// current autos
  cssEl**	args; 			// args for function ([size = cssElFun::ArgMax + 1])
  int 		act_argc;		// actual number of args received
  cssClassInst*	cur_this;		// current this pointer

  cssFrame(cssProg* prg);
  
  void	AllocArgs();		// allocate args

  virtual ~cssFrame();
};

class CSS_API cssWatchPoint {
  // a watch point: stop when expression changes
public:
  cssEl*	watch;		// what guy to watch
  String	prv_val;	// previous value of guy
  String	cur_val;

  void		SetWatch(cssEl* wp);
  String	GetStr();
  inline void	GetAsPrvVal() { if(watch) prv_val = watch->GetStr(); }
  inline void	GetAsCurVal() { if(watch) cur_val = watch->GetStr(); }

  cssWatchPoint();
  virtual ~cssWatchPoint();
};

class CSS_API cssWatchList : public taPtrList<cssWatchPoint> {
  // list of watch points
protected:
  void	El_Done_(void* item)	{ delete (cssWatchPoint*)item; }
public:  
  virtual ~cssWatchList() { Reset(); }
};

class CSS_API cssProg {
  // a single chunk of script code (a block, a function, etc.)
protected:
  friend class	cssProgSpace;
  int 		alloc_size;		// allocated size of program
  int		fr_alloc_size;		// allocated size of frames
  int 		src_alloc_size;		// allocated size of source
  cssElPtr	el_retv;		// return value for lookup, etc
  int		refn;			// reference count
public:
  // flag state values
  enum State_Flags {
    State_Run		= 0x0001,
    State_IsTmpProg	= 0x0004, 	// is a temporary program, not part of main progs
    State_NoBreak	= 0x0008, 	// do not break while running this guy
  };
 
  enum YY_Flags {
    YY_Exit		= 0,	// #IGNORE script is done being parsed
    YY_Ok		= 1,	// #IGNORE everything is fine
    YY_NoSrc		= -2,	// #IGNORE don't code last line as source
    YY_Err		= -3,	// #IGNORE error
    YY_Blank     	= -4,	// #IGNORE blank line
    YY_Parse     	= -5 	// #IGNORE need to parse more
  };

  String 	name;			// name of program
  cssScriptFun*	owner_fun;		// if owned by a cssScriptFun
  cssCodeBlock*	owner_blk;		// if owned by a cssCodeBlock
  cssProgSpace* top;			// top-level space holding this one

  cssInst**	insts;			// the instructions themselves
  css_progdx 	size;			// number of instructions

  cssFrame** 	frame;			// anything dynamic goes here
  int		fr_size;		// how deep is your frame?

  cssSpace	literals;		// literal constants used during parsing
  cssSpace	statics;		// static variables
  cssSpace	saved_stack;		// saved stack state

  int		first_src_ln;		// first source code line represented in this program
  int		last_src_ln;		// last source code line

  int		state;			// prog-specific state

  int_Array	breaks;			// breakpoints
  css_progdx	lastif;			// last if statment index (for else)
  bool		lastelseif;		// last if was an else if -- need to check for next else

  void 		Constr();
  void		Copy(const cssProg& cp);
  cssProg();
  cssProg(const String& nm);
  cssProg(const cssProg& cp);
  virtual ~cssProg();

  // all static just for consistency..
  static void   Done(cssProg* it)	{ if(it->refn <= 0) delete it; }
  static void  	Ref(cssProg* it)	{ it->refn++; }
  static void   unRef(cssProg* it)	{ it->refn--; }
  static void	unRefDone(cssProg* it)	{ unRef(it); Done(it); }

  // internal functions
  void		AllocInst(int sz);
  void		AllocFrame(int sz);

  cssInst* 	Inst(int i) const
  { cssInst* rval = NULL; if((int)i<size) rval = insts[i]; return rval; }

  cssFrame*	Frame() const 		{ return frame[fr_size-1]; }
  cssFrame*	Frame(int frdx) const 	{ return frame[frdx]; }
  cssSpace*	Autos() const		{ return &(Frame()->autos); }
  cssSpace*	Autos(int frdx) const	{ return &(Frame(frdx)->autos); }
  cssSpace*	Stack() const		{ return &(Frame()->stack); }
  cssSpace*	Stack(int frdx) const 	{ return &(Frame(frdx)->stack); }
  css_progdx 	PC() const	     	{ return Frame()->pc; }
  css_progdx	PC(int frdx) const     	{ return Frame(frdx)->pc; }
  cssEl**	Args() const		{ return Frame()->args; }
  cssEl**	Args(int frdx) const	{ return Frame(frdx)->args; }
  int&		ActArgc() const		{ return Frame()->act_argc; }
  int&		ActArgc(int frdx) const	{ return Frame(frdx)->act_argc; }
  cssClassInst* CurThis() const		{ return Frame()->cur_this; }
  cssClassInst* CurThis(int frdx) const	{ return Frame(frdx)->cur_this; }
  void		SetCurThis(cssClassInst* ths)	{ Frame()->cur_this = ths; }

  int 		AddFrame();		// copies autos to new frame
  int		DelFrame();		// removes a frame
  void 		Reset();		// reset code, autos, frames, etc
  void		ResetCode();		// get rid of all insts and source

  cssScriptFun*	GetCurrentFun();	// find current function (or null if not in one)

  cssProgSpace*	SetTop(cssProgSpace* pspc)
  { cssProgSpace* rval = top; top = pspc; return rval; }
  // set the top pointer, returning current one, which should be saved and used to pop
  void		PopTop(cssProgSpace* pspc)  	{ top = pspc; }
  // pop the top pointer, set to value returned by SetTop upon exit of scope

  // source, debugging
  int		GetSrcLn(css_progdx pcval) const; // get source line number for given code index
  int		CurSrcLn() const	{ return GetSrcLn(PC()); }
  int		FindSrcLn(int ln) const;	// find first program inst idx for overall source line ln

  void 		ListSrc() const; // list source code for prog (esp for functions)
  void 		ListMachine(pager_ostream& fh, int indent = 0, int stinst = -1) const; // machine impl
  void		ListLocals(pager_ostream& fh, int frdx = -1, int indent = 0);

  // coding
  int 		AddCode(cssInst* it);
  cssElPtr&	AddAuto(cssEl* it);
  cssElPtr&	AddLiteral(cssEl* it);

  int 		Code(cssEl* it);
  int 		Code(cssElPtr &it);
  int 		Code(const String& nm);
  int 		Code(css_progdx it);
  int 		Code(cssIJump* it);
  int		ReplaceCode(int idx, cssEl* it);
  void		UnCode()		{ if(size > 0) delete insts[--size]; }
  void		ResetLasts() 		{ lastif = -1; lastelseif = false; }
  int		Undo(int srcln);      // undo coding of given source line
  void		ZapFrom(int zp_size); // zap (remove) program code from zp_size to end of current size
  int		OptimizeCode();	      // 2nd pass that optimizes code -- returns # of optimizations

  cssElPtr&	FindAutoName(const String& nm);	// lookup by name
  cssElPtr&	FindLiteral(Int it)	{ return literals.Find(it); }
  cssElPtr&	FindLiteral(Real it)	{ return literals.Find(it); }
  cssElPtr&	FindLiteral(const String& it) { return literals.Find(it); }

  // execution
  void 		SetPC(css_progdx npc) 	{ Frame()->pc = MIN(npc, size); }
  cssProg*	SetSrcPC(int srcln); 	// this one goes by src ln
  cssEl*	Cont();	 		// continue with rest of program
  cssEl*	Cont(css_progdx st)	{ SetPC(st); return Cont(); }
  cssEl*	ContSrc(int srcln);	// continue with rest of program
  cssEl*	Run()	 		{ Restart(); return Cont(); }
  void 		Restart();		// restart execution
  void		EndRunPop()		{ Stack()->DelPop(); }
  void		SaveStack();		// save current stack
  void		ReloadStack();		// reload current stack from saved
  void		RunDebugInfo(cssInst* nxt); // output debugging info while running

  // breakpoints
  bool 		SetBreak(int srcln);
  bool		DelBreak(int srcln);
  bool		DelBreakIdx(int idx);
  bool		IsBreak(css_progdx pcval);
  bool		IsBreak()		{ return IsBreak(PC()); }
  void		ShowBreaks(ostream& fh = cout);

  // breakpoints
  bool 		SetWatch(cssEl* watch);
  bool 		DelWatch(cssEl* watch);
  bool 		DelWatchIdx(int idx);
  bool 		CheckWatch();
  void		ShowWatchpoints(ostream& fh = cout);

};

class CSS_API cssProgStack {
  // contains an index of what frame a given program is in
public:
  cssProg*	prog;
  int		fr_no;
};

// for an external parsing function that is called first -- can use the css parser
// to parse expressions -- used in ProgExpr 
typedef int (*css_progspace_ext_parse_fun)(void* udata, const char* parse_nm, cssElPtr& el_ptr);

class CSS_API cssProgSpace: public QObject {
  // an entire program space, including all functions defined, variables etc.
INHERITED(QObject)
friend class cssProg;
  Q_OBJECT
protected:
  enum CompileCtrl {		// compiling control state (delayed compile actions)
    CC_None,
    CC_Push,			// prog to push in cc_push_this
    CC_Pop,
    CC_Include			// file name is in cc_include_this
  };
  
  enum InputMode { // current input mode, either file (fin) or interactive from console
    IM_File,
    IM_Console
  };

public:
  String 	name;

  int 		size;			// size of progs, in els
  cssProgStack** progs;			// stack of sub programs, used for both parsing and running

  int		state;			// current run state information
  int		parse_depth;		// depth of progs for current parsing run

  cssSpace	prog_vars;		// external Program or Script vars (just like hard_vars, but in a special space to make it easier to update)
  cssSpace	hard_vars;		// space-specific extern vars
  cssSpace	hard_funs;		// space-specific extern funs
  cssSpace	statics;		// global variables (in space)
  cssSpace	enums;			// enums defined in space
  cssSpace	types;			// types defined in space

  int		step_mode;		// step mode: if > 0, next Cont will run this # of lines
  cssEl::RunStat run_stat; 		// flag to tell if running: set to Stopping to stop, or BreakPoint
  int 		debug;			// debug level: 1 = src trace, 2 = machine code trace, 3 = + stack trace
  cssProg*	last_bp_prog;		// last breakpoint prog
  css_progdx	last_bp_pc;		// last breakpoint pc
  cssWatchList	watchpoints;		// watch points
  String	exec_err_msg;		// error message for when run_stat == ExecError

  istream*	src_fin;		// source input stream
  String	cur_fnm;		// current source file name
  int		cur_fnm_lno;		// line number within current file name

  String_Array	src_list;		// complete source code listing across all files (including #includes), by line number
  String_Array	src_list_fnm;		// source file name for each line in src_list
  int_Array	src_list_lno;		// original line number within source file for each line in src_list

  int 		src_ln;			// present source line no in src_list (parsing)
  int 		src_col;		// current source column number in src_list (parsing)
  int		src_pos;		// absolute source position from start of file/string (parsing)
  int		list_ln;		// present source line no in src_list (listing)
  int		list_n;			// number of lines to display in the listing

  int		tok_src_ln;		// where the current token started
  int		tok_src_col;

  bool		parsing_command; 	// true if we are presently parsing a command
  bool		parse_path_expr;	// currently parsing a path expression (disable looking up other variables!)

  bool		external_stop;		// to stop execution externally, set this

  css_progspace_ext_parse_fun ext_parse_fun_pre;
  // external parsing function -- called with a symbol name before parsing any internal names (pre)
  css_progspace_ext_parse_fun ext_parse_fun_post;
  // external parsing function -- called with a symbol name after attempting everything to parse and failing (post)
  void*		ext_parse_user_data; 	// user data passed to parse fun as first arg (typically object pointer)

  cssCmdShell*	cmd_shell;		// controlling command shell


  void Constr();
  cssProgSpace();
  cssProgSpace(const String& nm);
  virtual ~cssProgSpace();

  bool		AmCmdProg();		// am I a cmd shell cmd_prog?
  bool		HaveCmdShell();		// check if cmd_shell is set, issue warning if not
  cssProgSpace*	GetSrcProg();		// if I am a cmd prog, then return my corresponding src_prog, else NULL

  cssProgStack* ProgStack() const	{ return progs[size-1]; }
  cssProgStack* ProgStack(int prdx) const { return progs[prdx]; }
  cssProg*	Prog() const		{ return ProgStack()->prog; }
  cssProg*	Prog(int prdx) const	{ return ProgStack(prdx)->prog; }
  int		Prog_Fr() const		{ return ProgStack()->fr_no; }
  int		Prog_Fr(int prdx) const	{ return ProgStack(prdx)->fr_no; }
  cssProg*	PrvProg() const		{ if(size <= 1) return NULL; return ProgStack(size-2)->prog; }

  // internal coding, programs
  void		SetName(const String& nm); 		// updates all names
  void 		Reset();
  void		ClearAll();
  void		AllocProg(int sz);
  void		AddProg(cssProg* it);
  void		DelProg(); 		// delete program
  cssProg*	PopProg();		// pop (no delete) program
  void 		Push(cssProg* it);
  void		Shove(cssProg* it); 	// pushing while running
  cssProg*	Pop();
  cssProg*	Pull();			// popping while running
  cssScriptFun*	GetCurrentFun();	// find current function (or null if not in one)

  // internal coding, source
  int 		Getc();			// this is the one function for getting the next character from the input -- everything must go through this -- uses src_fin for input file -- returns EOF if at end
  int 		unGetc();		// undo last character get (backup 1 position in source)
  int           CurSrcCharsLeft();      // number of chars remaining in cur src ln
  void		StoreCurTokSrcPos();	// store current token source position in tok_src_ln,col

  String	GetSrcLnCol(int ln, int cl=0) const; // get source code given line and column
  String	GetSrcLn(int ln) const;		  // get source line with line number \t src
  String	GetFullSrcLn(int ln) const;
  // get full source listing on 2 lines with 1st line = file name and line number

  String	CurParseSrc() const; 	// current parsing source (src_ln, src_col)
  String	CurTokSrc() const; 	// current token src fm tok_src_ln, tok_src_col
  String	CurFullTokSrc() const; 	// current full token src fm tok_src_ln, tok_src_col
  String	CurFullRunSrc() const;	// current full running PC() source
  String	GetSrcListFnm(int i) const; // list fnm -- empty = name
  void 		ListSrc_impl(pager_ostream& fh, int stln = -1) const;
  // implementation of list source
  void 		ListMachine_impl(pager_ostream& fh, int ln) const;
  // list underlying machine code for given source code line

  // internal coding, variables
  cssElPtr&	AddAuto(cssEl* it) 	{ return Prog()->AddAuto(it); }
  cssElPtr&	AddLiteral(cssEl* it) 	{ return Prog()->AddLiteral(it); }
  cssElPtr&	AddLiteral(String& str);
  cssElPtr&	AddLiteral(int itm);
  cssElPtr&	AddLiteral(Real itm);
  cssElPtr&	AddVar(cssEl* it); 	// for variables, not literals, etc.
  cssElPtr&	AddStaticVar(cssEl* it);// for variables declared static
  cssElPtr&	AddStatic(cssEl* it);	// for other static objects (not variables)
  bool		ReplaceVar(cssEl* old, cssEl* nw);
  bool		RemoveVar(cssEl* it);	// this is somewhat dangerous..
  bool		DelVar(cssEl* it)	{ return ReplaceVar(it, &cssMisc::Void); }
  cssElPtr&	FindName(const String& nm); // lookup object by name (in autos, static, hards)
  cssSpace*	GetParseSpace(int idx);		// get parse spaces in order by index, NULL if over
  cssElPtr&	ParseName(const String& nm);	// parse name in all spaces (in order)

  cssElPtr&	FindTypeName(const String& nm); // find name based on type
  cssElPtr&	GetPtrType(cssEl* base_type, int ptrs); // get pointer type of base_type
  cssElPtr&	GetRefType(cssEl* base_type); // get reference type of base_type

  // compiling
  static  int	GetFile(fstream& fh, const String& fname); // get the file

  int		CompileLn(istream& fh = cin, bool* err = NULL);	// parse next line of stream, set optional err if error
  bool 		Compile(istream& fh = cin);	// parse a stream and produce a program, 'true' if successful
  bool 		Compile(const String& fname);	// parse a file and produce a program, 'true' if successful
  bool 		CompileCode(const String& code);// parse a string and produce a program, 'true' if successful
  void		Include(const String& fname);	// include a file
  void		CompileRunClear(const String& fname); // compile a file, run, then clearall
  void 		reCompile();			// parse same file and produce a program
  void		Undo(int st);
  void		Undo()			{ Undo(src_ln-2); }
  void		ResetParseFlags();
  int		OptimizeCode();	      // 2nd pass that optimizes code -- returns # of optimizations

  // compile control actions
  void		ClearCompileCtrl()	{ compile_ctrl = CC_None; }
  void		SetPop()		{ compile_ctrl = CC_Pop; }
  void		SetPush(cssProg* it)	{ compile_ctrl = CC_Push; cc_push_this = it; }
  void		SetInclude(const String& it) { compile_ctrl = CC_Include; cc_include_this = it; }
  bool		DoCompileCtrl(); 	// do compile control action based on flag
  bool		ParseElseCheck();	// check if next input token is the word 'else' -- needed for parsing if..else constructs
  bool 		PopElseBlocks();	// pop any remaining 'else' blocks off the stack

  // execution
  void 		Restart();
  cssEl* 	Cont();
  cssEl* 	Cont(css_progdx st);
  cssEl* 	ContSrc(int srcln);
  cssEl*	Run();
  void		Stop(); // can be called from inside or outside a program to cause it to stop
  void		EndRunPop()		{ Prog()->EndRunPop(); }
  
  bool		ContinueLoop();	// process continue command for loops
  bool		BreakLoop();	// process break command for loops
  bool		ReturnFun();	// process function return

  // display, status
  int		ListDebug() const
  { int rval=debug; if(debug >= 2) rval=2; return rval; }
  void		SetDebug(int dblev);
  void		Status();
  void		BackTrace(int levels_back=-1);

  void 		ListSrc(int stln = -1);	// list source code
  void 		ListFun(const String& fun_nm);	// list a function of given name
  void		ListConstants();
  void		ListDefines();
  void		ListEnums();
  void		ListFunctions();
  void		ListGlobals();
  void		ListLocals(int levels_back=0);
  void		ListObjHards();
  void		ListSettings();
  void		ListTypes();

  void		Info(const String& inf_type = "", cssEl* arg = NULL);
  void		Info_Generic();
  void		Help(cssEl* on_el = NULL);
  void		Help_Generic();

  // breakpoints
  bool 		SetBreak(int srcln);
  bool		DelBreak(int srcln);
  void		ShowBreaks();

  bool 		SetWatch(cssEl* watch);
  bool		DelWatch(cssEl* watch);
  bool		DelWatchIdx(int idx);
  void		ShowWatchpoints();

protected:
  int 		ReadLn(istream* fh);	// used in Getc -- read the line in from filein
  int		AddSrcList(const String& nw_lst=""); // usedin Getc -- add a new line to source code listing, returns line no

  int 		alloc_size;		// allocated number of prog_stacks
  int		old_debug;		// saved version
  cssElPtr	el_retv;		// return value for getel

  CompileCtrl	compile_ctrl;		// control flags for delayed compile actions
  cssProg*	cc_push_this;		// push this object
  String	cc_include_this;	// filename to be included
};

class CSS_API cssCmdShell : public QObject {
  // a command shell that controls a program space
  INHERITED(QObject);
  friend class cssProg;
  friend class cssProgSpace;
  Q_OBJECT
public:
  String 	name;
  String	prompt;
  String	act_prompt;		// the actual prompt
  taMisc::ConsoleType	console_type;		// what kind of console are we running?

  istream*	fin;			// input file (current)
  ostream*	fout;			// output file
  ostream*	ferr;			// error file
  pager_ostream	pgout;			// pager outstream
  
  bool		external_exit;		// set to true to break out of a shell...

  cssProgSpace*	src_prog;		// current program with source code for commands to operate on (I do not own this, nor is there refcounting!) DO NOT SET DIRECTLY: USE Push/Pop to manage
  cssProgSpace*	cmd_prog;		// program for commands, etc (I own this one!)

  cssProgSpace** src_prog_stack; 	// stack of src_prog's
  int 		stack_size;		// size of prog_stack

  void Constr();
  cssCmdShell();
  cssCmdShell(const String& nm);
  virtual ~cssCmdShell();

  // manage the src_prog_stack
  void		AllocSrcProg(int sz);
  void		PushSrcProg(cssProgSpace* ps); 	// push onto stack, set src_prog = ps
  cssProgSpace* PopSrcProg(cssProgSpace* ps = NULL); // pop current src prog, if ps is non-NULL, then only if src_prog == ps
  void		PopAllSrcProg(); // pop off all of the src progs

  void		StartupShellInit(istream& fhi = cin, ostream& fho = cout,
    taMisc::ConsoleType cons_typ = taMisc::CT_NONE);
  // do all the initialization stuff for a shell, but don't actually start a specific shell

  bool		RunStartupScript();
  // run any startup scripts that might have been specified by startup args

  void		FlushConsole();	// flush the console output
  void		SetPrompt(const String& prmpt, bool disp_prompt = false);
  void		UpdatePrompt(bool disp_prompt = false);
  //  void 		Source(const String& fname);	// run a file as if in a shell

  void		Exit();		// exit from the shell

public slots:
  void		AcceptNewLine_Qt(QString ln, bool eof); 
  // called when a new line of text becomes available -- all outer shells/consoles call this interface (qt version)
  void		Shell_NoConsole_Run();
  // this should be invoked by an event or timer after event loop has started, for no-console execution

protected:
  int		stack_alloc_size;	// allocated size of src_prog stack

  void		AcceptNewLine(const String& ln, bool eof); 
  // impl for slot
  void		Shell_No_Console(const String& prmpt);
  // configure a nogui readline-based shell
  void		Shell_OS_Console(const String& prmpt);
  // configure a quick-and-dirty shell 
#ifdef HAVE_QT_CONSOLE
  void		Shell_Gui_Console(const String& prmpt);
  // configure qt gui-based shell that links with QcssConsole
#endif
};

#endif // machine_h
