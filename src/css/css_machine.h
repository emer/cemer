/* -*- C++ -*- */
// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <memory.h>
#include <fstream>
#include <sstream>

#include "ta_string.h"
#include "ta_variant.h"
#include "css_extern_support.h"

// support for ta_type is neccessary to get file and string member funcs
// TYPEA is an internal type access library

#define CSS_SUPPORT_TYPEA 1
#include "ta_type.h"
#include "ta_base.h"

#ifdef TA_USE_QT
# include <QObject>
#endif

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

// todo: move this somewhere else
// todo: cannot inherit from ostream because ostream << methods are not virtual!!
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

  static int		argc;		// number of args passed by commandline to app
  static char**		argv;		// args passed by commandline to app
  static bool		gui;		// startup switch, whether using gui or not
  
  static int		readline_waitproc(); // note: not used in GUI mode, see css_qt.h
  static void		PreInitialize(int argc_, char** argv_);
  // this must be called immediately after main(), and before any other css/ta initialization
//  static void 		Initialize(int argc, const char** argv);
  static int 		Initialize();
  // this is called to install builtin funcs, setup system, etc.; returns 0 if ok
  static bool		HasCmdLineSwitch(const String& sw_name, bool starts_with = false); // looks for the switch value (include the '-' if applicable)
  static bool		HasCmdLineSwitch(const String& sw_name, int& index, bool starts_with = false); // looks for the switch value (include the '-' if applicable) starting from index; index advanced
  static bool		CmdLineSwitchValue(const String& sw_name, int& index, 
    String& sw_value, bool starts_with = false); // looks for the switch value from index and returns following string; advances index
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

  static void 		Error(cssProg* prg, const char* a, const char* b="", const char* c="", const char* d="",
			      const char* e="", const char* f="", const char* g="", const char* h="",
			      const char* i="", const char* j="", const char* k="", const char* l="");

  static void		Warning(cssProg* prg, const char* a, const char* b="", const char* c="", const char* d="",
				const char* e="", const char* f="", const char* g="", const char* h="",
				const char* i="", const char* j="", const char* k="", const char* l="");
public:
#if (!defined(TA_OS_WIN32))
  static void 		fpecatch(int);	// floating point exception handling
  static void 		intrcatch(int);	// interrupt exception handling
#endif
};


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
    bool nul = false; if(ptr_type == DIRECT) nul = (ptr == NULL);
    else nul = ((ptr == NULL) || (dx < 0)); return nul; }
  bool	NotNull() const	{ return !IsNull(); }

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
  cssEl* 	AnonClone()  	{ return new x (*this, ""); }	      \
  cssEl*	MakeToken_stub(int, cssEl* arg[])		      \
  { return new x ( init , (const char*)*(arg[1])); }		      \

#define cssCloneOnly(x) \
  cssEl* 	Clone()		  { return new x (*this); }	      \
  cssEl* 	AnonClone() 	  { return new x (*this, ""); }	      \


class CSS_API cssEl {
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
    T_Int64			// 64-bit integer
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
// todo: tmp_str should be mutable, can get rid of all those const casts..
  mutable String tmp_str;	// string used by parsing system
//obs  String 	tmp_str;	// temporary string when a char* cast is needed
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
  virtual cssEl*	GetActualObj() 		{ return this; }
  // get non-reference, non-pointer object

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
  void 			Constr();
#ifdef CSS_DEBUG_REGISTER
  virtual void		Register(); 	// for debugging purposes
#else
  virtual void		Register()	{ };  // when not debugging
#endif

  cssEl()			{ name = ""; Constr(); }
  cssEl(const char* nm) { name = nm; Constr();}
  cssEl(const cssEl& cp)	{ Constr(); Copy(cp); }
  cssEl(const cssEl& cp, const char* nm) { Constr(); Copy(cp); name = nm; }

  virtual ~cssEl();

  void   		Copy(const cssEl& cp);		// for copying to existing structs

  virtual cssEl* 	Clone() 			{ return new cssEl(*this); }
  virtual cssEl* 	AnonClone() 			{ return new cssEl(*this, ""); }
  virtual cssEl*	MakeToken_stub(int, cssEl**) 	{ return &cssMisc::Void; }

  virtual cssEl::RunStat 	MakeToken(cssProg* prg);
  virtual cssEl::RunStat 	MakeTempToken(cssProg* prg); //

  void NopErr(const char* opr="") const
  { cssMisc::Error(prog, "Operation:", opr, "not defined for type:", GetTypeName()); }

  // use this for conversions that don't work (safer)
  void CvtErr(const char* opr="") const
  { cssMisc::Error(prog, "Conversion:", opr, "not defined for type:", GetTypeName()); }

  // convert from types
  virtual String GetStr() const 		{ return _nilString; }
  virtual Variant GetVar() const 		{ return _nilVariant; }
  virtual operator Real() const	 		{ return 0; }
  virtual operator float() const 		{ return (float)(Real)*this; }
  virtual operator Int() const	 		{ return 0; }
  virtual operator unsigned int() const 	{ return (unsigned int)(Int)*this; }
  virtual operator char() const	 		{ return (char)(Int)*this; }
  virtual operator signed char() const 		{ return (signed char)(Int)*this; }
  virtual operator unsigned char() const 	{ return (unsigned char)(Int)*this; }
  virtual operator short() const    		{ return (short)(Int)*this; }
  virtual operator unsigned short() const	{ return (unsigned short)(Int)*this; }
  virtual operator long() const	 		{ return (long)(Int)*this; }
  virtual operator unsigned long() const	{ return (unsigned long)(Int)*this; }
  virtual operator int64_t() const	 	{ return 0LL; }
  virtual operator uint64_t() const	 	{ return 0ULL; }
#ifndef NO_BUILTIN_BOOL
  virtual operator bool() const			{ return (Int)*this; }
#endif
  virtual operator const char*() const 		{ return (const char*)GetStr(); }
  virtual operator String() const		{ return GetStr(); }
  virtual operator char*() const		{ return (char*)(const char*)GetStr(); }
          operator Variant() const		{ return GetVar(); }
  virtual operator unsigned char*() const	{ return (unsigned char*)(const char*)*this; }
#ifndef NO_SIGNED
  virtual operator signed char*() const		{ return (signed char*)(const char*)*this; }
#endif
#if NEED_STREAMOFF_CSS_CONV
  virtual operator streamoff() const 		{ return (streamoff)(Int)*this; }
#endif

  virtual operator void*() const		{ CvtErr("(void*)"); return NULL; }
  virtual operator void**() const		{ CvtErr("(void**)"); return NULL; }

  virtual void* GetVoidPtrOfType(TypeDef* td) const 	{ CvtErr(td->name); return NULL; }
  virtual void* GetVoidPtrOfType(const char* td) const 	{ CvtErr(td); return NULL; }
  // these are type-safe ways to convert a cssEl into a ptr to object of given type

  virtual operator int*() const		{ CvtErr("(int*)"); return NULL; }
  virtual operator short*() const	{ CvtErr("(short*)"); return NULL; }
  virtual operator long*() const	{ CvtErr("(long*)"); return NULL; }
  virtual operator double*() const	{ CvtErr("(double*)"); return NULL; }
  virtual operator float*() const	{ CvtErr("(float*)"); return NULL; }
  virtual operator String*() const	{ CvtErr("(String*)"); return NULL; }
  virtual operator Variant*() const	{ CvtErr("(Variant*)"); return NULL; }
#ifndef NO_BUILTIN_BOOL
  virtual operator bool*() const	{ CvtErr("(bool*)"); return NULL; }
#endif

  virtual operator int**() const	{ CvtErr("(int**)"); return NULL; }
  virtual operator short**() const	{ CvtErr("(short**)"); return NULL; }
  virtual operator long**() const	{ CvtErr("(long**)"); return NULL; }
  virtual operator double**() const	{ CvtErr("(double**)"); return NULL; }
  virtual operator float**() const	{ CvtErr("(float**)"); return NULL; }
  virtual operator String**() const	{ CvtErr("(String**)"); return NULL; }
  virtual operator Variant**() const	{ CvtErr("(Variant**)"); return NULL; }
#ifndef NO_BUILTIN_BOOL
  virtual operator bool**() const	{ CvtErr("(bool**)"); return NULL; }
#endif

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
#ifdef CSS_SUPPORT_TYPEA
  virtual operator TAPtr() const;
  virtual operator TAPtr*() const 		{ CvtErr("(TAPtr*)"); return NULL; }
  virtual operator TypeDef*() const;
  virtual operator MemberDef*() const;
  virtual operator MethodDef*() const;
  virtual void operator=(TAPtr*) 		{ CvtErr("(TAPtr)"); }
#endif

  // assign from types
  virtual void operator=(Real)	 		{ CvtErr("(Real)"); }
  virtual void operator=(Int)			{ CvtErr("(Int)"); }
  virtual void operator=(int64_t cp)		{ operator=((Int)cp); }
  virtual void operator=(uint64_t cp)		{ operator=((Int)cp); }
  virtual void operator=(const String&)	 	{ CvtErr("(String)"); }
  virtual void operator=(const Variant& val); 
    // usually not overridden, just dispatches according to type

  virtual void operator=(void*)	 		{ CvtErr("(void*)"); }
  virtual void operator=(void**)		{ CvtErr("(void**)"); }

  virtual void AssignFromType(TypeDef* td, void*)	{ CvtErr(td->name); }
  virtual void AssignFromType(const char* td, void*)	{ CvtErr(td); }
  // type-safe way to assign a void ptr of given type to a cssEl

  // operators
  virtual void operator=(const cssEl&) 	{ NopErr("="); }
  virtual void CastFm(const cssEl& cp)	{ operator=(cp); } // default cast is a copy
  virtual void InitAssign(const cssEl& cp) { operator=(cp); } // default initial assign is cpy

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
  virtual cssEl* operator[](int) const { NopErr("[]"); return &cssMisc::Void; }
  virtual int	 GetMemberNo(const char*) const { NopErr(".,->"); return -1; }
  virtual cssEl* GetMember(const char* nm) const  { return GetMember(GetMemberNo(nm));  }
  virtual cssEl* GetMember(int) const  { NopErr(".,->"); return &cssMisc::Void; }
  virtual int	 GetMemberFunNo(const char*) const { NopErr(".,->()"); return -1; }
  virtual cssEl* GetMemberFun(const char* nm) const { return GetMemberFun(GetMemberFunNo(nm)); }
  virtual cssEl* GetMemberFun(int) const { NopErr(".,->()"); return &cssMisc::Void; }
  virtual cssEl* GetScoped(const char*) const { NopErr("::"); return &cssMisc::Void; }
  virtual cssEl* NewOpr();
  virtual void 	 DelOpr()	{ NopErr("delete"); } // delete operator

  virtual bool operator< (cssEl&) { NopErr("<"); return 0; }
  virtual bool operator> (cssEl&) { NopErr(">"); return 0; }
  virtual bool operator! () 	  { NopErr("!"); return 0; }
  virtual bool operator<=(cssEl&) { NopErr("<="); return 0; }
  virtual bool operator>=(cssEl&) { NopErr(">="); return 0; }
  virtual bool operator==(cssEl&) { NopErr("=="); return 0; }
  virtual bool operator!=(cssEl&) { NopErr("!="); return 0; }
  virtual bool operator&&(cssEl&) { NopErr("&&"); return 0; }
  virtual bool operator||(cssEl&) { NopErr("||"); return 0; }

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
  int	 GetMemberNo_impl(const TypeDef& typ, const char*) const;
  int	 GetMemberFunNo_impl(const TypeDef& typ, const char*) const;
  cssEl* GetMember_impl(const TypeDef& typ, void* base, int memb) const;
  cssEl* GetMember_impl(MemberDef* md, void* mbr) const;
  cssEl* GetMemberFun_impl(const TypeDef& typ, void* base, int memb) const;
  cssEl* GetMemberFun_impl(void* base, MethodDef* md) const;
  cssEl* GetScoped_impl(const TypeDef& typ, void* base, const char*) const;
  cssEl* GetVariantEl_impl(const Variant& val, int idx) const; // helper for operator[]
  virtual cssEl* GetFromTA_impl(TypeDef* td, void* itm, const char* nm, 
    MemberDef* md = NULL) const; // helper for getting members and array elements
};

inline void cssEl::Ref(cssEl* it) { ++(it->refn); }
#ifndef DEBUG
inline void cssEl::unRef(cssEl* it) { --(it->refn); }
inline void cssEl::unRefDone(cssEl* it) { if (--(it->refn) <= 0) delete it;}
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
  static String& fancy_list(String& fh, const String& itm, int no, int prln,
			    int tabs, int indent = 0);

  void 		Constr();
  cssSpace()				{ alloc_size = 2;  Constr(); }
  cssSpace(const char* nm)		{ alloc_size = 2;  name = nm;  Constr(); }
  cssSpace(int no, const char* nm)	{ alloc_size = no;  name = nm;  Constr(); }
  cssSpace(const cssSpace& cp)		{ alloc_size = 2; Constr();  Copy(cp); }
  virtual ~cssSpace()			{ Reset();  free(els); }

  void Copy(const cssSpace& cp);
  void CopyUniqNameNew(const cssSpace& cp); // copy only unique items, keep new one
  void CopyUniqNameOld(const cssSpace& cp); // copy only unique items, keep old one

  cssElPtr&	FindName(const char* nm); // lookup by name
  cssElPtr&	Find(Int nm);		// lookup by number value
  cssElPtr&	Find(Real nm);		// lookup by number value
  cssElPtr&	Find(const String& nm); // lookup by string value
  int		GetIndex(cssEl* it);	// find it, return index
  int		IndexOfName(const String& nm) const; // return index of name, -1 if not found

  void 		List(ostream& fh = cout, int indent = 0) const;	// (elaborate print format)
  void 		List(pager_ostream& fh, int indent = 0) const;	// (elaborate print format)
  void 		NameList(ostream& fh = cout, int indent = 0) const;   // just the names
  void 		NameList(pager_ostream& fh, int indent = 0) const;   // just the names
  void		ValList(ostream& fh = cout, int indent = 0) const;    // just the values (printf format)
  void		TypeNameList(ostream& fh = cout, int indent = 0) const; // "fancy" type/name output
  void		TypeNameValList(ostream& fh = cout, int indent = 0) const; // "fancy" type/name/val output
  String	PrintStr(int indent = 0) const;
  String	PrintFStr(int indent = 0) const;

  void		Alloc(int sz);		// allocate space on the list..
  void 		Reset();		// clear list
  void		Sort();			// sort list

  // for stack-like operations
  cssElPtr&	Push(cssEl* it);
  cssElPtr&	PushUniqNameNew(cssEl* it);
  cssElPtr&	PushUniqNameOld(cssEl* it);
  cssEl*	Pop();			// this pop unRef's -- you must do Done
  void		DelPop(); 		// pop and delete item
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
  cssElCFun(int ac, cssEl* (*fp)(int, cssEl* args[]));
  cssElCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const char* nm);
  cssElCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const char* nm, int pt, const char* hstr=NULL);
  cssElCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const char* nm, cssEl* rtype, const char* hstr=NULL);
  cssElCFun(const cssElCFun& cp);
  cssElCFun(const cssElCFun& cp, const char* nm);
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
  cssElInCFun(int ac, cssEl* (*fp)(int, cssEl* args[]));
  cssElInCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const char* nm);
  cssElInCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const char* nm, int pt);
  cssElInCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const char* nm, cssEl* rtype);
  cssElInCFun(const cssElInCFun& cp);
  cssElInCFun(const cssElInCFun& cp, const char* nm);

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
  cssMbrCFun();
  cssMbrCFun(int ac, void* th, cssEl* (*fp)(void*, int, cssEl**));
  cssMbrCFun(int ac, void* th, cssEl* (*fp)(void*, int, cssEl**), const char* nm);
  cssMbrCFun(const cssMbrCFun& cp);
  cssMbrCFun(const cssMbrCFun& cp, const char* nm);
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
    ELSE			// only run if stack value evaluates to 0 (else)
  };

  enum LoopType {
    NOT_LOOP,			// not a loop block
    WHILE,
    DO,
    FOR,
    SWITCH			// not a loop technically, but processes break
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
  cssCodeBlock();
  cssCodeBlock(const char* nm);
  cssCodeBlock(const cssCodeBlock& cp);
  cssCodeBlock(const cssCodeBlock& cp, const char* nm);
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

  virtual void	Define(cssProg* prg, bool decl = false, const char* nm = NULL);
  // initialize the function (decl = true if in declaration, not definition)

  // constructors
  void		Constr();
  void		Copy(const cssScriptFun& cp);
  cssScriptFun();
  cssScriptFun(const char* nm);
  cssScriptFun(const cssScriptFun& cp);
  cssScriptFun(const cssScriptFun& cp, const char* nm);
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

  void		Define(cssProg* prg, bool decl = false, const char* nm = NULL);
  // initialize the function

  void		SetDesc(const char* des); // get options from desc
  bool		HasOption(const char* opt) { return opts.contains(opt); }
  String	OptionAfter(const char* opt);

  void		Constr();
  void		Copy(const cssMbrScriptFun& cp);
  cssMbrScriptFun(const char* nm, cssClassType* cls);
  cssMbrScriptFun(const cssMbrScriptFun& cp);
  cssMbrScriptFun(const cssMbrScriptFun& cp, const char* nm);
  ~cssMbrScriptFun();

  cssCloneOnly(cssMbrScriptFun);
  cssEl*	MakeToken_stub(int na, cssEl* arg[]);
  // return retv_type token, else cssInt
};

#define cssCPtr_CloneFuns(x, init) \
  cssEl* 	Clone()		{ return new x (*this); }	      \
  cssEl* 	AnonClone() 	{ return new x (*this, ""); }	      \
  cssEl*	MakeToken_stub(int, cssEl* arg[])		      \
    { return new x ( init, ptr_cnt, (const char*)*(arg[1])); }	      \


class CSS_API cssCPtr : public cssEl {
  // base class for ptrs to C objects
protected:
  cssEl*	class_parent;	// if this pointer was derived from a class structure
public:
  void* 	ptr;
  int		ptr_cnt;	// number of ptrs (1= *, 2= **, etc.)
  bool		read_only;	// true if this object comes from read-only member

  int		GetParse() const	{ return CSS_PTR; }
  uint		GetSize() const		{ return 0; } // use for ptrs
  cssTypes 	GetType() const		{ return T_C_Ptr; }
  const char*	GetTypeName() const 	{ return "(C_Ptr)"; }

  String 	PrintStr() const
  { return String(GetTypeName())+" "+name + " --> " + String((long)ptr); }
  String	PrintFStr() const { return String((long)ptr); }

  // constructors
  void		Constr();
  cssCPtr();
  cssCPtr(void* it, int pc);
  cssCPtr(void* it, int pc, const char* nm);
  cssCPtr(void* it, int pc, const char* nm, cssEl* cp, bool ro);
  cssCPtr(const cssCPtr& cp);
  cssCPtr(const cssCPtr& cp, const char* nm);
  ~cssCPtr();

  cssCPtr_CloneFuns(cssCPtr, (void*)NULL);

  virtual void	SetClassParent(cssEl* cp);

  // converters
  virtual void*	GetVoidPtr(int cnt = 1) const;	// goes down till ptr_cnt == cnt
  virtual void* GetNonNullVoidPtr(int cnt = 1) const; // generates error if null

  String	GetStr() const	{ return String((long)ptr);}
  Variant	GetVar() const { return Variant(ptr); }
  operator	Real() const	{ CvtErr("(Real)"); return 0.0; }
  operator 	Int() const	{ return (Int)(long)(ptr); }
  operator 	void*()	const	{ return GetVoidPtr(1); }
  operator 	void**() const	{ return (void**)GetVoidPtr(2); }

  void operator=(Real)	 	{ CvtErr("(Real)"); }
  void operator=(Int)		{ CvtErr("(Int)"); }
  void operator=(int64_t cp)	{ CvtErr("(Int64)"); }
  void operator=(uint64_t cp)	{ CvtErr("(Int64)"); }
  void operator=(const String&)	{ CvtErr("(String)"); }
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }

  // operators
  virtual bool	ROCheck();	// do read_only check, true if ok to modify, else err
  virtual void 	PtrAssignPtr(cssCPtr* s); 	// use when assigning ptrs to ptrs
  void operator=(const cssEl& s);

  void	UpdateAfterEdit();

  cssEl* operator*();
  cssEl* operator*(cssEl&)	{ NopErr("*"); return &cssMisc::Void; }

  virtual bool 	SamePtrLevel(cssCPtr* s); // if this and s have diff cnt, emit warning

  bool operator< (cssEl& s) { return ((Int)(long)(ptr) < (Int)s); }
  bool operator> (cssEl& s) { return ((Int)(long)(ptr) > (Int)s); }
  bool operator! () 	    { return (ptr == 0); }
  bool operator<=(cssEl& s) { return ((Int)(long)(ptr) <= (Int)s); }
  bool operator>=(cssEl& s) { return ((Int)(long)(ptr) >= (Int)s); }
  bool operator==(cssEl& s);	// these two check for sameptrlevel
  bool operator!=(cssEl& s);
  bool operator&&(cssEl& s) { return ((Int)(long)(ptr) && (Int)s); }
  bool operator||(cssEl& s) { return ((Int)(long)(ptr) || (Int)s); }
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

  cssDef(int ac);
  cssDef(int ac, const char* nm);
  cssDef(const cssDef& cp);
  cssDef(const cssDef& cp, const char* nm);

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
  virtual void		ListImpl(pager_ostream& fh, int indent = 0) const; // machine impl

  virtual cssEl::RunStat 	Do();
  virtual void 		SetLine(css_progdx it) 	{ line = it; }
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

  String	PrintStr() const;
  void 		ListSrc(pager_ostream& fh, int = 0) const;
  void		ListImpl(pager_ostream& fh, int = 0) const;
  cssEl::RunStat 	Do();
  void 		SetLine(css_progdx it) 		{ jumpto = it; }
  css_progdx	GetJump()			{ return jumpto; }
  bool		IsJump()			{ return true; }

  void		Copy(const cssIJump& cp);
  cssIJump(const cssProg* prg, css_progdx jmp);
  cssIJump(const cssProg* prg, css_progdx jmp, int lno, int clno);
  cssIJump(const cssIJump& cp);

  virtual cssInst* Clone() { return new cssIJump(*this); }
};

class CSS_API cssListEl {
public:
  css_progdx    stpc;		// starting pc for this line
  int		ln;		// line no in source code
  String	src;		// source code for line, specifically for 
  String	full_src;	// full source code 

  void		Copy(const cssListEl& cp)
  { stpc = cp.stpc; ln = cp.ln; src = cp.src; }

  cssListEl()			{ stpc = 0;   ln = 0; }
  cssListEl(css_progdx pc, int l, const String& cd)
  { stpc = pc;  ln = l;  src = cd;  }
  cssListEl(const cssListEl& cp)	{ Copy(cp); }

  cssListEl* 	Clone()	{ return new cssListEl(*this); }
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
    State_WasBurped	= 0x0002,  	// last line was burped, (delete src if necc)
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
  css_progdx	parse_st_size;		// starting size for parsing
  int		parse_st_src_size;	// source starting size for parsing

  cssFrame** 	frame;			// anything dynamic goes here
  int		fr_size;		// how deep is your frame?

  cssSpace	literals;		// literal constants used during parsing
  cssSpace	statics;		// static variables
  cssSpace	saved_stack;		// saved stack state

  cssListEl**	source;			// keep the source code here
  int 		src_size;		// number of lines of source code
  int 		line;			// current source[] index number (parsing)
  int 		col;			// current source[] column number (parsing)
  int		st_line;		// where started parsing...
  int		st_col;
  int		tok_line;		// where the current token started
  int		tok_col;
  int		state;			// prog-specific state

  int_Array	breaks;			// breakpoints
  css_progdx	lastif;			// last if statment index (for else)

  void 		Constr();
  void		Copy(const cssProg& cp);
  cssProg();
  cssProg(const char* nm);
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
  void		AllocSrc(int sz);

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

  cssProgSpace*	SetTop(cssProgSpace* pspc)
  { cssProgSpace* rval = top; top = pspc; return rval; }
  // set the top pointer, returning current one, which should be saved and used to pop
  void		PopTop(cssProgSpace* pspc)  	{ top = pspc; }
  // pop the top pointer, set to value returned by SetTop upon exit of scope

  // source, debugging
  String	GetSrc() const;
  String	GetSrcLC(int ln, int cl=0) const;
  int		CurSrcLn(css_progdx pcval); 	// extrnl srcln
  int		CurSrcLn()			{ return CurSrcLn(PC()); }
  String	GetCurSrcLn()			{ return GetSrcLC(CurSrcLn()); }
  int		CurSrcLC(css_progdx pcval); 	// internal ln cnt
  int		CurSrcLC()			{ return CurSrcLC(PC()); }
  String	GetCurSrcLC()			{ return GetSrcLC(CurSrcLC()); }
  int		FindSrcLn(int ln);		// find the particular one
  int		ClosestSrcLn(int ln);		// find closest to ln
  int		HasSrcLn(int st, int ed);	// find one in the given range
  int           CurSrcCharsLeft();              // number of chars remaining in cur src ln

  void		ListSrc(pager_ostream& fh, int indent = 0, int stln = -1); // source code
  void 		ListImpl(pager_ostream& fh, int indent = 0, int stinst = -1); // machine impl
  void		ListSpace(pager_ostream& fh, int frdx = -1, int indent = 0);

  // coding
  int 		AddCode(cssInst* it);
  cssElPtr&	AddAuto(cssEl* it);
  cssElPtr&	AddLiteral(cssEl* it);
  int 		AddSrc(const char* line);
  void		MarkParseStart()
  { parse_st_size = size; parse_st_src_size = src_size; }
  void		ZapFrom(int zp_size, int zp_src_size); // zap from
  void          ZapFromSrc(int zp_src_size); // zap only source, not inst
  void		ZapLastParse()		{ ZapFrom(parse_st_size, parse_st_src_size); }
  int 		Getc();
  void 		unGetc()		{ col--; }

  int 		Code(cssEl* it);
  int 		Code(cssElPtr &it);
  int 		Code(const char* nm);
  int 		Code(css_progdx it);
  int 		Code(cssIJump* it);
  int		ReplaceCode(int idx, cssEl* it);
  void		UnCode()		{ if(size > 0) delete insts[--size]; }
  void		ResetLasts() 		{ lastif = -1; }
  void		BurpSrc(); 		// source was read in advance, burp it
  int		Undo(int srcln);

  cssElPtr&	FindAutoName(const char* nm);	// lookup by name
  cssElPtr&	FindLiteral(Int it)	{ return literals.Find(it); }
  cssElPtr&	FindLiteral(Real it)	{ return literals.Find(it); }
  cssElPtr&	FindLiteral(const String& it) { return literals.Find(it); }

  // execution
  void 		SetPC(css_progdx npc);
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
  bool		IsBreak(css_progdx pcval);
  bool		IsBreak()		{ return IsBreak(PC()); }
  void		ShowBreaks(ostream& fh = cout);
  bool		unSetBreak(int srcln);
protected:
  int 		ReadLn(istream& fh);	// read the line in from filein
};

class CSS_API cssProgStack {
  // contains an index of what frame a given program is in
public:
  cssProg*	prog;
  int		fr_no;
};


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

  cssSpace	prog_vars;		// external Program vars, only used by pdp Programs, act like hard_vars
  cssSpace	hard_vars;		// space-specific extern vars
  cssSpace	hard_funs;		// space-specific extern funs
  cssSpace	statics;		// global variables (in space)
  cssSpace	types;			// types defined in space

  int		step_mode;		// step mode: if > 0, next Cont will run this # of lines
  cssEl::RunStat run_stat; 		// flag to tell if running: set to Stopping to stop, or BreakPoint
  int 		debug;			// debug level: 1 = src trace, 2 = machine code trace, 3 = + stack trace
  cssProg*	last_bp_prog;		// last breakpoint prog
  css_progdx	last_bp_pc;		// last breakpoint pc

  istream*	src_fin;		// source input stream
  int 		src_ln;			// present source line no (parsing)
  int		st_src_ln;		// starting source line no
  int		list_ln;		// present source line no (listing)

  bool		parsing_command; 	// true if we are presently parsing a command
  bool		external_stop;		// to stop execution externally, set this

  cssCmdShell*	cmd_shell;		// controlling command shell

  void Constr();
  cssProgSpace();
  cssProgSpace(const char* nm);
  virtual ~cssProgSpace();

  bool		AmCmdProg();		// am I a cmd shell cmd_prog?
  bool		HaveCmdShell();		// check if cmd_shell is set, issue warning if not

  cssProgStack* ProgStack()		{ return progs[size-1]; }
  cssProgStack* ProgStack(int prdx)	{ return progs[prdx]; }
  cssProg*	Prog()			{ return ProgStack()->prog; }
  cssProg*	Prog(int prdx)		{ return ProgStack(prdx)->prog; }
  int		Prog_Fr()		{ return ProgStack()->fr_no; }
  int		Prog_Fr(int prdx)	{ return ProgStack(prdx)->fr_no; }
  cssProg*	PrvProg()		{ if(size <= 1) return NULL; return ProgStack(size-2)->prog; }

  // internal coding, programs
  void		SetName(const char* nm); 		// updates all names
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

  // internal coding, variables
  cssElPtr&	AddAuto(cssEl* it) 	{ return Prog()->AddAuto(it); }
  cssElPtr&	AddLiteral(cssEl* it) 	{ return Prog()->AddLiteral(it); }
  cssElPtr&	AddLiteral(String& str);
  cssElPtr&	AddLiteral(int itm);
  cssElPtr&	AddLiteral(Real itm);
  cssElPtr&	AddPtrType(cssEl* base_type); // add new pointer type of base_type
  cssElPtr&	AddRefType(cssEl* base_type); // add new reference type of base_type
  cssElPtr&	AddVar(cssEl* it); 	// for variables, not literals, etc.
  cssElPtr&	AddStaticVar(cssEl* it);// for variables declared static
  cssElPtr&	AddStatic(cssEl* it);	// for other static objects (not variables)
  bool		ReplaceVar(cssEl* old, cssEl* nw);
  bool		RemoveVar(cssEl* it);	// this is somewhat dangerous..
  bool		DelVar(cssEl* it)	{ return ReplaceVar(it, &cssMisc::Void); }
  cssElPtr&	FindName(const char* nm); // lookup object by name (in autos, static, hards)
  cssSpace*	GetParseSpace(int idx);		// get parse spaces in order by index, NULL if over
  cssElPtr&	ParseName(const char* nm);	// parse name in all spaces (in order)
  cssElPtr&	FindTypeName(const char* nm); // find name based on type

  // compiling
  static  int	GetFile(fstream& fh, const char* fname); // get the file

  int		CompileLn(istream& fh = cin, bool* err = NULL);	// parse next line of stream, set optional err if error
  bool 		Compile(istream& fh = cin);	// parse a stream and produce a program, 'true' if successful
  bool 		Compile(const char* fname);	// parse a file and produce a program, 'true' if successful
  bool 		CompileCode(const String& code);// parse a string and produce a program, 'true' if successful
  void		Include(const char* fname);	// include a file
  void		CompileRunClear(const char* fname); // compile a file, run, then clearall
  void 		reCompile();			// parse same file and produce a program
  void		Undo(int st);
  void		Undo()			{ Undo(src_ln-2); }
  void		ResetParseFlags();

  // compile control actions
  void		ClearCompileCtrl()	{ compile_ctrl = CC_None; }
  void		SetPop()		{ compile_ctrl = CC_Pop; }
  void		SetPush(cssProg* it)	{ compile_ctrl = CC_Push; cc_push_this = it; }
  void		SetInclude(const char* it) { compile_ctrl = CC_Include; cc_include_this = it; }
  bool		DoCompileCtrl(); 	// do compile control action based on flag

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

  // display, status
  int		ListDebug()
  { int rval=debug; if(debug >= 2) rval=2; return rval; }
  void		SetDebug(int dblev);
  void 		ListSrc(int stln = -1);	// source code
  void 		ListImpl(int stln = -1); // machine code implementation
  void		List(int stln = -1);
  void		ListSpace();
  void		Status();
  void		Trace(int level=0);
  void		Help();

  // breakpoints
  bool 		SetBreak(int srcln);
  void		ShowBreaks();
  bool		unSetBreak(int srcln);

protected:
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
  enum ConsoleType { // type of console to use
    CT_Qt_Console,		// Qt-based console
    CT_NoGui_Rl,		// -nogui readline based console
    CT_QandD_Console,		// quick-and-dirty console (compatible with qt, but uses stdin/out
  };

  String 	name;
  String	prompt;
  String	act_prompt;		// the actual prompt
  ConsoleType	console_type;		// what kind of console are we running?

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
  cssCmdShell(const char* nm);
  virtual ~cssCmdShell();

  // manage the src_prog_stack
  void		AllocSrcProg(int sz);
  void		PushSrcProg(cssProgSpace* ps); 	// push onto stack, set src_prog = ps
  cssProgSpace* PopSrcProg(cssProgSpace* ps = NULL); // pop current src prog, if ps is non-NULL, then only if src_prog == ps
  void		PopAllSrcProg(); // pop off all of the src progs

  void		StartupShellInit(istream& fhi = cin, ostream& fho = cout, ConsoleType cons_typ = CT_NoGui_Rl);
  // do all the initialization stuff for a shell, but don't actually start a specific shell
  void		Shell_QandD_Console(const char* prmpt);
  // deprecated: configure a quick-and-dirty shell 
  void		Shell_Qt_Console(const char* prmpt);
  // configure qt gui-based shell that links with QcssConsole
  void		Shell_NoGui_Rl(const char* prmpt);
  // run a nogui readline-based shell

  void		SetPrompt(const char* prmpt);
  void		UpdatePrompt();
  //  void 		Source(const char* fname);	// run a file as if in a shell

  void		Exit();		// exit from the shell

public slots:
  void		AcceptNewLine(QString ln, bool eof); 
  // called when a new line of text becomes available -- all outer shells/consoles call this interface
protected:
  int		stack_alloc_size;	// allocated size of src_prog stack
};

#endif // machine_h
