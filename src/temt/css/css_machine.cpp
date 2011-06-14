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


#include "css_machine.h"

#include "ta_platform.h"
#include "css_basic_types.h"
#include "css_c_ptr_types.h"
#include "css_console.h"
#ifdef HAVE_QT_CONSOLE
# include "css_qtconsole.h"
#endif
#ifndef CSS_NUMBER
# include "css_parse.h"
#endif


#include "css_ta.h"
#include "ta_base.h"		// for debugging alloc_list
#include "ta_matrix.h"
#include "ta_project.h"
#include "ta_program.h"

#ifdef TA_GUI
# include "css_qt.h"
# include "ta_qt.h" // for iApplication
# ifdef TA_USE_INVENTOR
# endif
#endif

#include <QEvent>
#include <QCoreApplication>
#include <QFileInfo>

#include <algorithm> // std::sort
#include <sstream>

int yyparse(void);
void yyerror(const char* s);

pager_ostream::pager_ostream() {
  fout = &cout;
  fin = &cin;
  n_lines = 40;
  cur_line = 0;
  quitting = false;
  no_page = false;
}

pager_ostream::pager_ostream(ostream* fo, istream* fi, int n_ln) {
  fout = fo;
  fin = fi;
  n_lines = n_ln;
  cur_line = 0;
  quitting = false;
  no_page = false;
}

void pager_ostream::start() {
  cur_line = 0;
  quitting = false;
}

pager_ostream& pager_ostream::operator<<(const char* str) {
  *this << (String)str;
  return *this;
}

pager_ostream& pager_ostream::operator<<(const String& str) {
  if(quitting) return *this;
  if(no_page) {
    *fout << str;
    fout->flush();
    return *this;
  }
  int n_nl = str.freq('\n');
  if(cur_line + n_nl < n_lines) {
    cur_line += n_nl;
    *fout << str;
    fout->flush();
    return *this;
  }

  int n_out = n_lines - cur_line;

  String cur_str = str;
  for(int i=0;i<n_out;i++) {
    String cur_ln = cur_str.through('\n');
    cur_str = cur_str.after('\n');
    *fout << cur_ln;
  }
  fout->flush();

  *fout << "---Press Return for More, q=quit, c=continue without paging ---";
  String resp;
  *fin >> resp;
  if(resp == "q" || resp == "Q") {
    quitting = true;
    return *this;
  }
  if(resp == "c" || resp == "C") {
    no_page = true;
    if(!cur_str.empty()) {
      *this << cur_str;
    }
    return *this;
  }
  cur_line = 0;

  if(!cur_str.empty()) {
    *this << cur_str;
  }

  return *this;
}


//////////////////////////
// 	cssMisc    	//
//////////////////////////

// jmp_buf 	cssMisc::begin;

cssSpace	cssMisc::Internal("Internal");	// for use in internal (not searched)
cssSpace	cssMisc::Parse("Parse");		// just for parsing (searched)
cssSpace	cssMisc::PreProcessor("PreProcessor");// pre-processor functions (cpp)
cssSpace	cssMisc::TypesSpace("Types");
cssSpace	cssMisc::TypesSpace_ptrs("Types_ptrs");
cssSpace	cssMisc::TypesSpace_refs("Types_refs");
cssSpace 	cssMisc::Externs("External Variables");
cssSpace	cssMisc::HardFuns("Hard-Coded Functions");
cssSpace	cssMisc::HardVars("Hard-Coded Variables");
cssSpace	cssMisc::Commands("Commands");
cssSpace	cssMisc::Functions("Functions");
cssSpace 	cssMisc::Constants("Constants"); 	// global constants
cssSpace	cssMisc::Enums("Enums");
cssSpace 	cssMisc::Settings("System Settings");
cssSpace	cssMisc::Defines("Defines");
cssSpace	cssMisc::VoidSpace("Void Space");

cssProgSpace* 	cssMisc::ConstExprTop = NULL;
cssProg* 	cssMisc::ConstExpr = NULL;
cssProg* 	cssMisc::CDtorProg = NULL;
cssProg* 	cssMisc::CallFunProg = NULL;
cssElPtr	cssMisc::cur_type;
cssClassType*	cssMisc::cur_class = NULL;
cssEl*		cssMisc::cur_scope = NULL;
cssMbrScriptFun* cssMisc::cur_method = NULL;
cssEnumType*	cssMisc::cur_enum = NULL;
int		cssMisc::anon_type_cnt = 0;
cssSpace	cssMisc::default_args("Default Args");
bool            cssMisc::parsing_membdefn = false;
bool            cssMisc::parsing_args = false;

cssProgSpace* 	cssMisc::Top = NULL;
cssProgSpace*	cssMisc::cur_top = NULL;
cssProgSpace*	cssMisc::code_cur_top = NULL;
cssCmdShell*	cssMisc::TopShell = NULL;

cssArray*	cssMisc::s_argv;
cssInt*		cssMisc::s_argc;
String		cssMisc::prompt = "css> ";
String		cssMisc::startup_file;
String		cssMisc::startup_code;
int		cssMisc::init_debug = -1;
int		cssMisc::init_bpoint = -1;
bool		cssMisc::init_interactive = true;
int		cssMisc::refcnt_trace = 0; // user wants refcnt tracing (-rct from arg)
bool		cssMisc::obey_read_only = true;
bool		cssMisc::call_update_after_edit = false;

cssEl 		cssMisc::Void("Void"); 	
cssElPtr	cssMisc::VoidElPtr;		// in theory, points to voidptr
cssPtr		cssMisc::VoidPtr;		// in theory, points to voidptr
cssArray	cssMisc::VoidArray;		// just needed for maketoken
cssArrayType    cssMisc::VoidArrayType;         // just needed for maketoken

cssString	cssMisc::VoidString;
cssVariant	cssMisc::VoidVariant;
cssEnumType	cssMisc::VoidEnumType("VoidEnum");
cssClassType	cssMisc::VoidClassType("VoidClass");

cssRef		cssMisc::VoidRef;  // for maketoken

QTime   	cssMisc::proc_events_timer;

void cssMisc::CodeConstExpr() {
  code_cur_top = ConstExprTop;
  ConstExprTop->Reset();
}

void cssMisc::CodeTop() {
  code_cur_top = NULL;
}

String cssMisc::GetSourceLoc(cssProg* prog) {
  cssProgSpace* top;
  if(prog)
    top = prog->top;
  else
    top = cssMisc::cur_top;

  if(top->state & (cssProg::State_Run)) {
    return top->CurFullRunSrc();
  }
  else {			// parsing
    return top->CurFullTokSrc();
  }
}

void cssMisc::OutputSourceLoc(cssProg* prog) {
  cssProgSpace* top;
  if(prog)
    top = prog->top;
  else
    top = cssMisc::cur_top;

  ostream* fh = &cerr;
  if(top->cmd_shell)
    fh = top->cmd_shell->ferr;

  if(taMisc::dmem_proc == 0) {
    *(fh) << GetSourceLoc(prog);
    taMisc::FlushConsole();
    fh->flush();
  }
}

String cssMisc::last_err_msg;

void cssMisc::Error(cssProg* prog, const char* a, const char* b, const char* c,
		    const char* d, const char* e, const char* f,
		    const char* g, const char* h, const char* i)
{
  cssProgSpace* top = prog ? prog->top : cssMisc::cur_top;
  cssMisc::last_err_msg = taMisc::SuperCat(a,b,c,d,e,f,g,h,i);
  if (prog) {
    cssMisc::last_err_msg += "\n" + GetSourceLoc(prog);
  }

  if(taMisc::dmem_proc == 0) {
    ostream* fh = &cerr;
    if(top->cmd_shell)
      fh = top->cmd_shell->ferr;

    *(fh) << cssMisc::last_err_msg << endl;
    taMisc::FlushConsole();
  }
  top->run_stat = cssEl::ExecError;
  top->exec_err_msg = cssMisc::last_err_msg;
}

String cssMisc::last_warn_msg;

void cssMisc::Warning(cssProg* prog, const char* a, const char* b, const char* c,
		      const char* d, const char* e, const char* f,
		      const char* g, const char* h, const char* i)
{
  cssProgSpace* top = prog ? prog->top : cssMisc::cur_top;
  cssMisc::last_warn_msg = taMisc::SuperCat(a,b,c,d,e,f,g,h,i);
  if (prog) {
    cssMisc::last_warn_msg += "\n" + GetSourceLoc(prog);
  }

  if(taMisc::dmem_proc == 0) {
    ostream* fh = &cerr;
    if(top->cmd_shell)
      fh = top->cmd_shell->ferr;

    *(fh) << cssMisc::last_warn_msg << endl;
    taMisc::FlushConsole();
  }
}

String cssMisc::Indent(int indent_level, int indent_spc) {
  if (indent_level == 0) return _nilString;
  else return String(indent_level * indent_spc, 0, ' ');
}

String cssMisc::IndentLines(const String& lines, int indent_level) {
  if (lines.empty()) return _nilString;
  if (indent_level == 0) return lines;
  // slightly complicated because we don't want to add spaces after trailing \n
  STRING_BUF(rval, (int)(lines.length() * 1.2f));
  rval += Indent(indent_level);
  bool final_n = lines.matches('\n', -1);
  if (final_n) rval += lines(0, lines.length() - 1);
  else         rval += lines;
  rval.gsub("\n", "\n" + Indent(indent_level));
  if (final_n) rval += '\n';
  return rval;
}

inline bool btwn(char c, char l, char u) 
  {return ((c >= l) && (c <= u));}
inline bool is_alphalike(char c) 
  {return isalpha(btwn(c, 'a', 'z') || btwn(c, 'A', 'Z') || (c == '_'));}
inline bool is_num(char c) 
  {return btwn(c, '0', '9');}
  
bool cssMisc::IsNameValid(const String& nm) {
  if (nm.empty()) return false;
  char c = nm.elem(0);
  if (!is_alphalike(c)) return false;
  int len = nm.length();
  for (int i = 1; i < len; ++i) {
    char c = nm.elem(i);
    if (!(is_alphalike(c) || is_num(c))) return false;
  }
  return true;
}

cssElPtr cssMisc::ParseName(const String& nm) {
  cssElPtr s;
  if((s = cssMisc::TypesSpace.FindName(nm)))
    return s;
  if((s = cssMisc::Externs.FindName(nm)))
    return s;
  if((s = cssMisc::HardFuns.FindName(nm)))
    return s;
  if((s = cssMisc::HardVars.FindName(nm)))
    return s;
  if((s = cssMisc::Commands.FindName(nm)))
    return s;
  if((s = cssMisc::Functions.FindName(nm)))
    return s;
  if((s = cssMisc::Constants.FindName(nm)))
    return s;
  if((s = cssMisc::Enums.FindName(nm)))
    return s;
  if((s = cssMisc::Settings.FindName(nm)))
    return s;
  return cssMisc::VoidElPtr;
}

#if (!defined(TA_OS_WIN))
// no longer trapping -- doesn't work!  now trapping proactively in div operation!
// void cssMisc::fpecatch(int) {
// //   signal(SIGFPE, (SIGNAL_PROC_FUN_TYPE) cssMisc::fpecatch);
//   cssProgSpace* top = cssMisc::cur_top;
// //   if(top->state & (cssProg::State_Run)) {
//     cssMisc::Warning(NULL, "Floating point exception");
//     top->run_stat = cssEl::ExecError;
// //   }
// }

void cssMisc::intrcatch(int) {
  signal(SIGINT, (SIGNAL_PROC_FUN_TYPE) cssMisc::intrcatch);
  cssMisc::Warning(NULL, "User Interrupt");
  cssProgSpace* top = cssMisc::cur_top;
  String stop_msg;
  if(top) {
    stop_msg = top->name;
    top->Stop();
  }
  Program::SetStopReq(Program::SR_USER_INTR, stop_msg);
}
#endif


//////////////////////////
// 	cssElPtr    	//
//////////////////////////

cssEl* cssElPtr::El() const {
  if(!ptr) return &cssMisc::Void;
  switch(ptr_type) {
  case DIRECT:
    return (cssEl*)ptr;
  case CLASS_MEMBER: {
    if(dx < 0)	return &cssMisc::Void;
    cssClassInst* cur_th = cssMisc::cur_top->Prog()->CurThis();
    if(!cur_th)
      return ((cssClassType*)ptr)->GetMemberFmNo((int)dx);
    else
      return cur_th->GetMemberFmNo((int)dx);
  }
  case NVIRT_METHOD: {
    if(dx < 0)	return &cssMisc::Void;
    return ((cssClassType*)ptr)->GetMethodFmNo((int)dx);
  }
  case VIRT_METHOD: {
    if(dx < 0)	return &cssMisc::Void; // try for virtual first
    cssClassInst* cur_th = cssMisc::cur_top->Prog()->CurThis();
    if(!cur_th)
      return ((cssClassType*)ptr)->GetMethodFmNo((int)dx);
    else
      return cur_th->GetMethodFmNo((int)dx);
  }
  case PROG_AUTO:
    if(dx < 0)	return &cssMisc::Void;
//     if(!((cssProg*)ptr)->Autos()) return &cssMisc::Void; // shouldn't happen
    return ((cssProg*)ptr)->Autos()->El(dx);
  case SPACE:
    if(dx < 0)	return &cssMisc::Void;
    return ((cssSpace*)ptr)->El(dx);		// use the safe version..
  case NULL_PTR:
  default:
    return &cssMisc::Void;
  }
}

String cssElPtr::PrintStr() const {
  String rval;
  switch(ptr_type) {
  case DIRECT:
    rval = "Direct";
    break;
  case CLASS_MEMBER:
    rval = "Cls Mbr " + ((cssClassType*)ptr)->type_name;
    break;
  case NVIRT_METHOD:
    rval = "NV Mth " + ((cssClassType*)ptr)->type_name;
    break;
  case VIRT_METHOD:
    rval = "V Mth " + ((cssClassType*)ptr)->type_name;
    break;
  case PROG_AUTO:
    rval = "Prg Auto " + ((cssProg*)ptr)->name;
    break;
  case SPACE:
    rval = "Spc " + ((cssSpace*)ptr)->name;
    break;
  case NULL_PTR:
  default:
    rval = "Void Ptr";
  }
  return rval;
}

void cssElPtr::Print(ostream& fh) const {
  fh << PrintStr() << "\t";
}

void cssElPtr::operator+=(int indx) {
  if((ptr_type != SPACE) || IsNull()) {
    cssMisc::Warning(NULL, "Cannot modify a NULL or non-array pointer value");
    return;
  }
  dx += indx;
  if(dx >= ((cssSpace*)ptr)->size) {
    dx = -1;
    cssMisc::Warning(NULL, "Array bounds exceeded");
  }
}
void cssElPtr::operator-=(int indx) {
  if((ptr_type != SPACE) || IsNull()) {
    cssMisc::Warning(NULL, "Cannot modify a NULL or non-array pointer value");
    return;
  }
  dx -= indx;
  if(dx < 0) {
    dx = -1;
    cssMisc::Warning(NULL, "Array bounds exceeded");
  }
}

//////////////////////////
// 	cssEl    	//
//////////////////////////

//String cssEl::no_string;

#ifdef DEBUG
void print_cssEl(cssEl* it, bool addr_only = false) {
  if (it) {
    String tmp;
    tmp = QString::number((intptr_t)it, 16); // only platform-independent way 
    cerr << "cssEl{" << tmp << "} ";
    if (!addr_only) {
      tmp = it->GetName();
      if (!tmp.empty()) cerr << tmp << " ";
      cerr << it->GetTypeName(); //note: is already in parens
    }
    cerr << " refn=" << it->refn;
  } else {
    cerr << "cssEl{NULL}";
  }
}

void cssEl::Ref(cssEl* it) {
  ++(it->refn);
  if (cssMisc::refcnt_trace > 0) {
    print_cssEl(it);
    cerr << "::Ref()" << endl;
  }
}

void cssEl::Done(cssEl* it) {
  if(!it) return;
  if (it->refn < 0) {
    cerr << "**WARNING** ";
    print_cssEl(it, true);
    cerr << "::Done(): it->refn < 0  -- **MAY BE MULTI-DELETED**\n";
  } 
  else if (cssMisc::refcnt_trace > 0) {
    print_cssEl(it);
    cerr << "::Done()\n";
  }
  //note: legacy compatibility for <= 0 in case some use cases do unRefs ???
  if (it->refn <= 0) 
    delete it; 
}

void cssEl::unRef(cssEl* it) {
  if(!it) return;
  --(it->refn);
  if (it->refn < 0) {
    cerr << "**WARNING** ";
    print_cssEl(it, true);
    cerr << "::unRef(): it->refn < 0\n";
  }
  else if (cssMisc::refcnt_trace > 0) {
    print_cssEl(it);
    cerr << "::unRef()" << endl;;
  }
}

void cssEl::unRefDone(cssEl* it) {
  if(!it) return;
  --(it->refn);
  if (it->refn < 0) {
    cerr << "**WARNING** ";
    print_cssEl(it, true);
    cerr << "::unRefDone: it->refn <= 0 -- **WILL NOT BE MULTI-DELETED**\n";
    return;
  }
  else if (cssMisc::refcnt_trace > 0) {
    print_cssEl(it);
    cerr << "::unRefDone()" << endl;
  }

  if (it->refn <= 0) 
    delete it; 
}

#endif //DEBUG

cssEl::~cssEl() {
  if(addr)
    delete addr;
  addr = NULL;
}

#ifdef CSS_DEBUG_REGISTER
int	cssEl_alloc_count = 0;
cssEl*  cssEl_alloc_last = NULL;
bool	cssEl_alloc_debug = 0;
int_Array cssEl_alloc_list;

void cssEl::Register() {
  if(!cssEl_alloc_debug)
    return;
  if(this != cssEl_alloc_last)
    cerr << "\n" << cssEl_alloc_count << " New: " << (int)this << "\t\t";
  cerr << "Type: " << GetTypeName() << "\t";
  cssEl_alloc_last = this;
}
#endif

void cssEl::Constr() {
  addr = NULL; refn = 0; prog = NULL;
#ifdef CSS_DEBUG_REGISTER
  if(cssEl_alloc_debug) {
    cssEl_alloc_count++;
    cssEl_alloc_list.Add((int)this);
    Register();
  }
#endif
}

void cssEl::Copy(const cssEl& cp) {
  prog = cp.prog;
}

cssEl* cssEl::GetTypeObject() const {
  String tp_nm = GetTypeName();
  tp_nm = tp_nm.after('(');
  tp_nm = tp_nm.before(')');
  cssElPtr s;
  if((prog) && (prog->top)) {
    if((s = prog->top->types.FindName((char*)tp_nm)) != 0)
      return s.El();
  }
  if(cssMisc::cur_top) {
    if((s = cssMisc::cur_top->types.FindName((char*)tp_nm)) != 0)
      return s.El();
  }
  if((s = cssMisc::TypesSpace.FindName((char*)tp_nm)) != 0)
    return s.El();
  return &cssMisc::Void;
}

cssElPtr cssEl::GetAddr() const {
  if(addr)
    return *addr;
  cssElPtr rval;
  rval.SetDirect((cssEl*)this);
  return rval;
}

void cssEl::SetAddr(const cssElPtr& cp) {
  if(!addr)
    addr = new cssElPtr();
  *addr = cp;
}

cssEl::RunStat cssEl::Do(cssProg* prg) {
  prog = prg;
  prog->Stack()->Push(this);
  return cssEl::Running;
}

cssEl::RunStat cssEl::FunDone(cssProg*) {
  cssMisc::Warning(NULL, "Internal error: Function or code block ended without finding proper start of block!");
  return cssEl::Running;
}

cssEl* cssEl::MakePtrType(int ptrs) {
  if((GetType() == cssEl::T_C_Ptr) || (GetType() == cssEl::T_TA)) {
    bool zero_ptr = false;
    cssCPtr* ths = (cssCPtr*)this->GetNonRefObj();
    if(ths->ptr_cnt == 0) {
      zero_ptr = true;
      ths->ptr_cnt++;		// prevent clone from complaining about making tokens
    }
    if(ptrs == 1) {
      cssCPtr* ptr = (cssCPtr*)Clone();
      ptr->name += "_ptr";
      if(zero_ptr)
	ths->ptr_cnt = 0;
      else
	ptr->ptr_cnt += 1;
      ptr->flags = cssCPtr::NO_PTR_FLAGS; // turn off the OWN_OBJ flag if it was set!
      return ptr;
    }
    else {			// 2 is max
      cssCPtr* ptr = (cssCPtr*)Clone();
      ptr->name += "_ptr_ptr";
      if(zero_ptr) {
	ptr->ptr_cnt++;
	ths->ptr_cnt = 0;
      }
      else
	ptr->ptr_cnt += 2;
      ptr->flags = cssCPtr::NO_PTR_FLAGS; // turn off the OWN_OBJ flag if it was set!
      return ptr;
    }
  }
  else {			// 1 is max for reg css guys
    cssPtr* ptr = new cssPtr(this, name + "_ptr");
    return ptr;
  }
}

cssEl* cssEl::MakeRefType() {
  return new cssRef(this, name + "_ref");
}

cssEl::RunStat cssEl::MakeToken(cssProg* prg) {
  static cssElFun arg_holder;	// this holds the args for make token..
  prog = prg;

  if(GetType() == T_Array || GetType() == T_ArrayType)
    arg_holder.argc = 3;
  else
    arg_holder.argc = 1;

  arg_holder.prog = prg;
  cssEl* args[cssElFun::ArgMax + 1];
  int act_argc;
  arg_holder.BindArgs(args, act_argc);

  cssEl* tmp;
  if((tmp = MakeToken_stub(act_argc, args)) != 0) {
    if(prg == cssMisc::ConstExpr)
      prg->Stack()->Push(new cssRef(prg->Autos(0)->Push(tmp)));
    else if(tmp_str == "extern")
      prg->Stack()->Push(new cssRef(cssMisc::Externs.PushUniqNameOld(tmp)));
    else if(tmp_str == "static")
      prg->Stack()->Push(new cssRef(prg->top->AddStaticVar(tmp)));
    else if(tmp_str == "literal")
      prg->Stack()->Push(new cssRef(prg->AddLiteral(tmp)));
    else if(tmp_str == "const")
      prg->Stack()->Push(new cssRef(cssMisc::Constants.PushUniqNameOld(tmp)));
    else
      prg->Stack()->Push(new cssRef(prg->top->AddVar(tmp)));
  }
  arg_holder.DoneArgs(args, act_argc);
  return cssEl::Running;
}

// temporary token on the stack.
cssEl::RunStat cssEl::MakeTempToken(cssProg* prg) {
  cssElFun arg_holder;	// this holds the args for make token..
  prog = prg;

  if(GetType() == T_Array)
    arg_holder.argc = 3;
  else if (GetType() == T_ArrayType)
    arg_holder.argc = VarArg;
  else {
    prg->Stack()->Push(new cssString("")); // make blank name...
    arg_holder.argc = 1;
  }

  arg_holder.prog = prg;
  cssEl* args[cssElFun::ArgMax + 1];
  int act_argc;
  arg_holder.BindArgs(args, act_argc); // could be var arg
  cssEl* tmp;
  if((tmp = MakeToken_stub(act_argc, args)) != 0) {
    tmp->prog = prg;
    prg->Stack()->Push(tmp);
  }
  arg_holder.DoneArgs(args, act_argc);
  return cssEl::Running;
}

cssEl* cssEl::NewOpr() {	// just make an object..
  MakeTempToken(cssMisc::cur_top->Prog());
  cssEl* itm = cssMisc::cur_top->Prog()->Stack()->Pop();
  cssElPtr ptr(itm);
  return new cssPtr(ptr);	// make it a pointer to the item..
}

void cssEl::Save(ostream& strm) {
  strm << GetStr();
}

void cssEl::Load(istream& strm) {
  int c = taMisc::read_till_rb_or_semi(strm);

  if(c != ';') {
    taMisc::Error("*** Missing ';' in dump file for:", name);
    return;
  }
  *this = taMisc::LexBuf;	// set via string assgn
}

void cssEl::operator=(const Variant& val) {
  switch (val.type()) {
  case Variant::T_Invalid: 
    CvtErr("(_nilVariant)"); break;
  case Variant::T_Bool: // note: because a cssBool derives from cssInt
  case Variant::T_Char: // note: because a cssChar derives from cssInt
  case Variant::T_Int:
    operator=(val.toInt()); break;
  case Variant::T_UInt:
  case Variant::T_Int64:
  case Variant::T_UInt64:
    operator=(val.toInt64()); break;
  case Variant::T_Double:
    operator=(val.toDouble()); break;
  case Variant::T_String: 
    operator=(val.toString()); break;
  case Variant::T_Ptr: 
    operator=(val.toPtr()); break;
  case Variant::T_Base: 
  case Variant::T_Matrix:
    operator=(val.toBase()); break;
  default: return ;
  }
}

cssEl* cssEl::GetElFromTA(TypeDef* td, void* itm, const String& nm, MemberDef* md,
			  cssEl* class_parent) {
  TypeDef* nptd = td->GetNonPtrType(); // always create one of these

  if(!nptd)
    return &cssMisc::Void;

  bool ro = false;
  if(md) {
    if(md->HasOption("READ_ONLY"))
      ro = true;
  }
  
  int new_ptr = td->ptr;
  if(new_ptr == 1) new_ptr++;	// bump up to a **
  else if(new_ptr > 1) {
    cssMisc::Error(NULL, "GetElFromTA -- cannot process ** (pointer-pointer) C members!");
    return &cssMisc::Void;
  }

  if(nptd->InheritsNonAtomicClass()) {
    if(nptd->DerivesFrom(&TA_ios) || nptd->DerivesFrom(&TA_istream)
       || nptd->DerivesFrom(&TA_ostream) || nptd->DerivesFrom(&TA_iostream))
      return new cssIOS(itm, new_ptr, nptd, nm, class_parent, ro);
    else if(nptd->DerivesFrom(TA_taBase))
      return new cssTA_Base(itm, new_ptr, nptd, nm, class_parent, ro);
    else
      return new cssTA(itm, new_ptr, nptd, nm, class_parent, ro);
  }
  else {
    if(nptd->InheritsFormal(TA_class)) {
      if(nptd == &TA_taString)
	return new cssCPtr_String(itm, new_ptr, nm, class_parent, ro);
      else if(nptd == &TA_Variant)
	return new cssCPtr_Variant(itm, new_ptr, nm, class_parent, ro);
      else if(nptd == &TA_TypeDef)
	return new cssTypeDef(itm, new_ptr, nptd, nm, class_parent, ro);
      else if(nptd == &TA_MemberDef)
	return new cssMemberDef(itm, new_ptr, nptd, nm, class_parent, ro);
      else if(nptd == &TA_MethodDef)
	return new cssMethodDef(itm, new_ptr, nptd, nm, class_parent, ro);
      else if(nptd->DerivesFrom(TA_taSmartRef))
	return new cssSmartRef(itm, new_ptr, nptd, nm, class_parent, ro);
      else
	return new cssTA(itm, new_ptr, nptd, nm, class_parent, ro);
    }
    else {
      if(nptd->DerivesFrom(TA_bool))
	return new cssCPtr_bool(itm, new_ptr, nm, class_parent, ro);
      else if(nptd->DerivesFormal(TA_enum))
	return new cssCPtr_enum(itm, new_ptr, nm, class_parent, ro);
      else if ((nptd->DerivesFrom(TA_int) || nptd->DerivesFrom(TA_unsigned_int)))
	return new cssCPtr_int(itm, new_ptr, nm, class_parent, ro);
      else if(nptd->DerivesFrom(TA_short) || (nptd->DerivesFrom(TA_unsigned_short)))
	return new cssCPtr_short(itm, new_ptr, nm, class_parent, ro);
      else if (nptd->DerivesFrom(TA_long) || nptd->DerivesFrom(TA_unsigned_long))
	return new cssCPtr_long(itm, new_ptr, nm, class_parent, ro);
      else if (nptd->DerivesFrom(TA_char) || nptd->DerivesFrom(TA_unsigned_char)
	       || nptd->DerivesFrom(TA_signed_char))
	return new cssCPtr_char(itm, new_ptr, nm, class_parent, ro);
      else if(nptd->DerivesFrom(TA_int64_t) || nptd->DerivesFrom(TA_uint64_t))
	return new cssCPtr_long_long(itm, new_ptr, nm, class_parent, ro);
      else if(nptd->DerivesFrom(TA_float))
	return new cssCPtr_float(itm, new_ptr, nm, class_parent, ro);
      else if(nptd->DerivesFrom(TA_double))
	return new cssCPtr_double(itm, new_ptr, nm, class_parent, ro);
      else {
	cssMisc::Error(NULL, "GetElFromTA -- atomic class not managed in case:", nptd->name);
	return &cssMisc::Void;
      }
    }
  }
}

cssEl* cssEl::GetVariantEl_impl(const Variant& val, Variant idx) const {
  switch (val.type()) {
  case Variant::T_String: {
    //TODO: maybe this should be Char???
    String nw_val = val.toString().elem(idx.toInt());
    return new cssString(nw_val);
    } break;
  case Variant::T_Matrix: {
    if (val.isNull()) {
      NopErr("[] on Variant Matrix that is null"); 
      break;
    }
    taMatrix* mat = val.toMatrix();
    Variant var(mat->SafeElAsVar_Flat(idx.toInt()));
    return new cssVariant(var);
    }
  default:
    // todo: pass on to tabase
    NopErr("[] on Variant that is not a String or Matrix"); 
  }
  return &cssMisc::Void;
}

int cssEl::GetMemberNo_impl(TypeDef* typ, const String& memb) const {
  if(!typ) return -1;
  int mdx = typ->members.FindNameIdx(memb);	// just 1st order search
  return mdx;
}

cssEl* cssEl::GetMemberFmNo_impl(TypeDef* typ, void* base, int memb) const {
  if(!typ) {
    cssMisc::Error(prog, "Type information is NULL in:", name);
    return &cssMisc::Void;
  }
  MemberDef* md = typ->members.SafeEl(memb);
  if(!md) {
    cssMisc::Error(prog, "Member not found:", String(memb), "in class of type: ", typ->name);
    return &cssMisc::Void;
  }
  return GetMemberEl_impl(typ, base, md);
}

cssEl* cssEl::GetMemberFmName_impl(TypeDef* typ, void* base, const String& memb) const {
  if(!typ) {
    cssMisc::Error(prog, "Type information is NULL in:", name);
    return &cssMisc::Void;
  }
  MemberDef* md = typ->members.FindName(memb);	// just 1st order search
//   void* mbr = NULL;
//   MemberDef* md = typ->members.FindNameAddrR(memb, base, mbr);	// skips paths!
  if(!md) {
    cssMisc::Error(prog, "MembeR not found:", String(memb), "in class of type: ", typ->name);
    return &cssMisc::Void;
  }
  return GetMemberEl_impl(typ, base, md); // for just 1st order search
//   return GetElFromTA(md->type, mbr, md->name, md, (cssEl*)this);
}

cssEl* cssEl::GetMemberEl_impl(TypeDef* typ, void* base, MemberDef* md) const {
  if(!base) {
    cssMisc::Error(prog, "GetMember: NULL pointer in: ", name);
    return &cssMisc::Void;
  }
  void* mbr = md->GetOff(base);
  return GetElFromTA(md->type, mbr, md->name, md, (cssEl*)this);
}

////////////////

Int cssEl::GetMethodNo_impl(TypeDef* typ, const String& meth) const {
  if(!typ) return -1;
  int mdx = typ->methods.FindNameIdx(meth);
  return mdx;
}

cssEl* cssEl::GetMethodFmNo_impl(TypeDef* typ, void* base, int meth) const {
  if(!typ) {
    cssMisc::Error(prog, "GetMethod: Type information is NULL in:", name);
    return &cssMisc::Void;
  }
  MethodDef* md = typ->methods.SafeEl(meth);
  if(!md) {
    cssMisc::Error(prog, "Member function not found:", String(meth), "in class of type: ", typ->name);
    return &cssMisc::Void;
  }
  return GetMethodEl_impl(typ, base, md);
}

cssEl* cssEl::GetMethodFmName_impl(TypeDef* typ, void* base, const String& meth) const {
  if(!typ) {
    cssMisc::Error(prog, "GetMethod: Type information is NULL in:", name);
    return &cssMisc::Void;
  }
  MethodDef* md = typ->methods.FindName(meth);
  if(!md) {
    cssMisc::Error(prog, "Member function not found:", String(meth), "in class of type: ", typ->name);
    return &cssMisc::Void;
  }
  return GetMethodEl_impl(typ, base, md);
}

cssEl* cssEl::GetMethodEl_impl(TypeDef* typ, void* base, MethodDef* md) const {
  if(md->stubp) {
    if(md->fun_argd >= 0)
      return new cssMbrCFun(VarArg, base, md->stubp, md->name);
    else
      return new cssMbrCFun(md->fun_argc+1, base, md->stubp, md->name); // add 1 for the 'this'
  }
  else {
    cssMisc::Error(prog, "Function pointer not callable:", md->name, "of type:", md->type->name,
	      "in class of type: ", typ->name);
    return &cssMisc::Void;
  }
}

cssEl* cssEl::GetScoped_impl(TypeDef* typ, void* base, const String& memb) const {
  if(!typ) {
    cssMisc::Error(prog, "GetScoped: Type information is NULL in:", name);
    return &cssMisc::Void;
  }
  EnumDef* ed = typ->FindEnum(memb);
  if(ed) {
    return new cssInt(ed->enum_no, memb); // this is not an Enum because it is just a value
    // and there is no object to point to that contains the enum value
  }
  TypeDef* td = typ->sub_types.FindName(memb);
  if(td) {
    if(td->DerivesFormal(TA_enum))
      return new cssCPtr_enum(NULL, 1, td->name, td);
    return new cssTA(NULL, 1, td);
  }

  MethodDef* meth = typ->methods.FindName(memb);
  if(meth) {
    return GetMethodEl_impl(typ, base, meth);
  }

  MemberDef* md = typ->members.FindName(memb);
  if(md) {
    return GetMemberEl_impl(typ, base, md);
  }
  cssMisc::Error(prog, "Scoped element not found:", memb, "in class of type:", typ->name);
  return &cssMisc::Void;
}

//////////////////////////////////
//  Basic Function Functions 	//
//////////////////////////////////

cssElFun::cssElFun() {
  argc = 0;
  dostat = cssEl::Waiting;
  def_start = 0;
  retv_type = &cssMisc::Void;
}

cssElFun::~cssElFun() {
  cssEl::DelRefPointer(&retv_type);
}

void cssElFun::Copy(const cssElFun& cp) {
  cssEl::Copy(cp);
  arg_defs.Copy(cp.arg_defs);
  def_start = cp.def_start;
  argc = cp.argc;
  dostat = cp.dostat;
  SetRetvType(cp.retv_type);
}

///////////////////////////////////////////////////////////////////////////
// 		no args

void cssElFun::BindArgs(cssEl** args, int& act_argc) {
  args[0] = this;		// first argument is always selfptr
  act_argc = 0;
  if(argc == NoArg) return;

  cssSpace* stack = prog->Stack();
  int stack_start;		// where to start getting things off of the stack

  if(argc == 0) {
    if(stack->Peek() == &cssMisc::Void)	// get rid of arg stop..
      stack->Pop();
    return;
  }

  if(argc == VarArg) {
    ///////////////////////////////////////////////////////////////////////////
    // 		variable numbers of arguments
    for(stack_start = stack->size-1; stack_start >= 0; stack_start--) {
      if(stack->FastEl(stack_start) == &cssMisc::Void)
	break;
    }
    act_argc = ((int)stack->size - stack_start) - 1;
    if(act_argc >= ArgMax) {
      cssMisc::Error(prog, "Arg count greater than max (32) in:", (const char*)name,
		     String((int)act_argc));
      act_argc = -1;
      return;
    }
    for(int i=act_argc; i>0; i--)
      args[i] = stack->Pop();

    if(stack->Peek() == &cssMisc::Void)	// get rid of arg stop..
      stack->Pop();
  }
  else if(arg_defs.size == 0) {
    ///////////////////////////////////////////////////////////////////////////
    // 		no default args -- must have correct number!

    if(stack->size < argc) {
      cssMisc::Error(prog, "Incomplete argument list for:", (const char*)name,
		     "should have at least:", String((int)argc), "got:",
		     String((int)stack->size));
      act_argc = -1;
      return;
    }

    act_argc = argc;
    for(int i=act_argc; i>0; i--)
      args[i] = stack->Pop();

    if(stack->Peek() == &cssMisc::Void)	// get rid of arg stop..
      stack->Pop();
  }
  else {
    ///////////////////////////////////////////////////////////////////////////
    // 		has default args -- need to search for how many args we got
    int max_stack = stack->size - argc;
    for(stack_start = stack->size-1; stack_start >= max_stack; stack_start--) {
      if(stack->FastEl(stack_start) == &cssMisc::Void)
	break;
    }
    act_argc = ((int)stack->size - stack_start) - 1;

    int diff = argc - act_argc;
    if(diff > arg_defs.size) {	// more difference than we have defaults..
      cssMisc::Error(prog, "Incomplete argument list for:", (const char*)name,
		     "should have at least:", String((int)(argc-arg_defs.size)), "got:",
		     String((int)act_argc));
      act_argc = -1;		// err msg indication
      return;
    }

    for(int i=act_argc; i>0; i--)
      args[i] = stack->Pop();

    if(stack->Peek() == &cssMisc::Void)	// get rid of arg stop..
      stack->Pop();

    if(diff != 0) {
      int df_st = arg_defs.size - diff;
      for(int i=df_st; i < arg_defs.size; i++)	// fill in using default args..
	args[def_start + i] = arg_defs.FastEl(i);
    }
    act_argc = argc;		// we always get the right number...
  }
}

void cssElFun::DoneArgs(cssEl** args, int& act_argc) {
  int i;
  for(i=1; i <= act_argc; i++) {
    cssEl::Done(args[i]);
    //    cssEl::unRefDone(args[i]);
  }
  act_argc = 0;
}

void cssElFun::GetArgDefs() {
  arg_vals.SetSize(argc+1);
  if(cssMisc::default_args.size == 0)
    return;
  if(arg_defs.size > 0) {
    arg_defs.Reset();
  }
  arg_defs.Copy(cssMisc::default_args);	// get default args
  def_start = 1 + argc - (int)arg_defs.size; // where defaults start..
  cssMisc::default_args.Reset(); // get rid of them..
  // get initial values from defaults
  int i;
  for(i=0; i<arg_defs.size; i++)
    arg_vals[def_start + i] = arg_defs.FastEl(i)->GetStr();
}



//////////////////////////////////
// 	C code Functions 	//
//////////////////////////////////

void cssElCFun::Constr() {
  parse = CSS_FUN;
}

void cssElCFun::Copy(const cssElCFun& cp) {
  cssElFun::Copy(cp);
  funp = cp.funp;
  parse = cp.parse;
}

cssElCFun::cssElCFun() {
  Constr(); funp = NULL;
}

cssElCFun::cssElCFun(int ac, cssEl* (*fp)(int, cssEl* args[])) {
  Constr(); argc = ac;  funp = fp;
}
cssElCFun::cssElCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const String& nm) {
  Constr(); name = nm;  argc = ac;  funp = fp;
}
cssElCFun::cssElCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const String& nm, int pt, const String& hstr) {
  Constr(); name = nm;  argc = ac;  funp = fp;  parse = pt;
  if(hstr.nonempty()) help_str = hstr;
}
cssElCFun::cssElCFun(int ac, cssEl* (*fp)(int, cssEl* args[]),
			 const String& nm, cssEl* rtype, const String& hstr) {
  Constr(); name = nm;  argc = ac;  funp = fp;  parse = CSS_FUN;
  SetRetvType(rtype);
  if(hstr.nonempty()) help_str = hstr;
}
cssElCFun::cssElCFun(const cssElCFun& cp) {
  Constr(); Copy(cp); name = cp.name;
}
cssElCFun::cssElCFun(const cssElCFun& cp, const String& nm) {
  Constr(); Copy(cp); name = nm;
}
cssElCFun::~cssElCFun() {
}
cssEl* cssElCFun::MakeToken_stub(int na, cssEl* arg[]) {
  if(retv_type != &cssMisc::Void) return retv_type->MakeToken_stub(na, arg);
  return new cssInt(0);		// default retv is int, not void
}

cssEl::RunStat cssElCFun::Do(cssProg* prg) {
  prog = prg;
  dostat = cssEl::Running;
  int act_argc;
  cssEl* args[cssElFun::ArgMax + 1];
  BindArgs(args, act_argc);
  if(act_argc < 0) return cssEl::ExecError;

  cssEl* tmp = (*funp)(act_argc, args);
  prog = prg;                   // restore if recursive
  if(!prog->top->external_stop && (tmp) && (tmp != &cssMisc::Void)) {
    tmp->prog = prog;
    prog->Stack()->Push(tmp);
  }
  DoneArgs(args, act_argc);

  return dostat;
}

//////////////////////////////////////////
//	internal fixed arg C functions	//
//////////////////////////////////////////

cssElInCFun::cssElInCFun()
  : cssElCFun()
{
}

cssElInCFun::cssElInCFun(int ac, cssEl* (*fp)(int, cssEl* args[]))
  : cssElCFun(ac, fp)
{
}
cssElInCFun::cssElInCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const String& nm)
  : cssElCFun(ac, fp, nm)
{
}
cssElInCFun::cssElInCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const String& nm, int pt)
  : cssElCFun(ac, fp, nm, pt)
{
}
cssElInCFun::cssElInCFun(int ac, cssEl* (*fp)(int, cssEl* args[]),
			 const String& nm, cssEl* rtype)
  : cssElCFun(ac, fp, nm, rtype)
{
}
cssElInCFun::cssElInCFun(const cssElInCFun& cp)
  : cssElCFun(cp)
{
}
cssElInCFun::cssElInCFun(const cssElInCFun& cp, const String& nm)
  : cssElCFun(cp, nm)
{
}

void cssElInCFun::BindArgs(cssEl** args, int& act_argc) {
  args[0] = this;		// first argument is always selfptr
  act_argc = 0;
  if(argc == NoArg || argc == 0)
    return;

  cssSpace* stack = prog->Stack();
  if(stack->size < argc) {
    cssMisc::Error(prog, "Incomplete argument list for:", (const char*)name,
		   "should have:", String((int)argc), "got:",
		   String((int)stack->size));
    act_argc = -1;
    return;
  }

  act_argc = argc;
  for(int i=act_argc; i>0; i--)
    args[i] = stack->Pop();
  // no void check
}

cssEl::RunStat cssElInCFun::Do(cssProg* prg) {
  prog = prg;
  dostat = cssEl::Running;
  //note: argc can be -2 sentinel
  int targc = (argc >= 0) ? argc : 0;
#ifdef _MSC_VER
  cssEl* args[ArgMax + 1];	// only need fixed amount
#else
  cssEl* args[targc + 1];	// only need fixed amount
#endif
  int act_argc;
  BindArgs(args, act_argc);

  cssEl* tmp = (*funp)(act_argc, args);
  prog = prg;                   // restore if recursive
  if(!prog->top->external_stop && (tmp) && (tmp != &cssMisc::Void)) {
    tmp->prog = prog;
    prog->Stack()->Push(tmp);
  }
  DoneArgs(args, act_argc);
  return dostat;
}



//////////////////////////////////////////
//	C code member functions		//
//////////////////////////////////////////

void cssMbrCFun::Constr() {
}

void cssMbrCFun::Copy(const cssMbrCFun& cp) {
  cssElFun::Copy(cp);
  funp = cp.funp; ths = cp.ths;
}
cssMbrCFun::cssMbrCFun() {
  Constr(); funp = NULL; argc = 0;
}
cssMbrCFun::cssMbrCFun(int ac, void* th, cssEl* (*fp)(void*, int, cssEl**)) {
  Constr(); argc = ac;  ths = th; funp = fp;
}
cssMbrCFun::cssMbrCFun(int ac, void* th, cssEl* (*fp)(void*, int, cssEl**), const String& nm)
{
  Constr(); name = nm;  argc = ac;  ths = th; funp = fp;
}
cssMbrCFun::cssMbrCFun(const cssMbrCFun& cp) {
  Constr(); Copy(cp); name = cp.name;
}
cssMbrCFun::cssMbrCFun(const cssMbrCFun& cp, const String& nm) {
  Constr(); Copy(cp); name = nm;
}
cssMbrCFun::~cssMbrCFun() {
  ths = NULL;
}


cssEl* cssMbrCFun::MakeToken_stub(int, cssEl* arg[]) {
  return new cssTA((void*)NULL, 1, &TA_int, arg[1]->GetStr());
}

cssEl::RunStat cssMbrCFun::Do(cssProg* prg) {
  prog = prg;
  dostat = cssEl::Running;
  int act_argc;
  cssEl* args[cssElFun::ArgMax + 1];
  BindArgs(args, act_argc);
  if(act_argc < 0) return cssEl::ExecError;
  if(!ths) {
    cssMisc::Error(prog, "Null 'this' object for member function call:", name);    
    return cssEl::ExecError;
  }
  cssEl* tmp = (*funp)(ths, act_argc, args);
  prog = prg;                   // restore if recursive
  tmp->prog = prog;
  if(!prog->top->external_stop && (tmp)) {
    prog->Stack()->Push(tmp);
  }
  DoneArgs(args, act_argc);
  return dostat;
}

//////////////////////////////////////////////////
// 	cssCodeBlock: Block of css code		//
//////////////////////////////////////////////////

void cssCodeBlock::Constr() {
  code = new cssProg(name);
  action = JUST_CODE;
  loop_back = 0;
  loop_type = NOT_LOOP;
  argc = 0;
  cssProg::Ref(code);
  code->owner_blk = this;
  owner_prog = NULL;
}

void cssCodeBlock::Copy(const cssCodeBlock& cp) {
  cssElFun::Copy(cp);
  if(code)
    cssProg::unRefDone(code);
  // note that this means that you could lose derived classes..
  code = cp.code;
  cssProg::Ref(code);
  code->owner_blk = this;
  owner_prog = cp.owner_prog;
  action = cp.action;
  loop_back = cp.loop_back;
  loop_type = cp.loop_type;
}

cssCodeBlock::cssCodeBlock() {
  Constr();
}

cssCodeBlock::cssCodeBlock(const String& nm, cssProg* ownr_prog) {
  Constr();
  name = nm;
  code->name = name;
  owner_prog = ownr_prog;
  if(owner_prog->master_prog)
    code->master_prog = owner_prog->master_prog; // hand-me-down
  else
    code->master_prog = owner_prog; // this must be our master too!
}
cssCodeBlock::cssCodeBlock(const cssCodeBlock& cp) {
  code = NULL;
  //  Constr();
  Copy(cp);
  name = cp.name;
}
cssCodeBlock::cssCodeBlock(const cssCodeBlock& cp, const String& nm) {
  //  Constr();
  code = NULL;
  Copy(cp);
  name = nm;
  code->name = name;
}
cssCodeBlock::~cssCodeBlock() {
  if(code)
    cssProg::unRefDone(code);
}

bool cssCodeBlock::CleanDoubleBlock() {
  if(code->size != 1) return false;
  cssEl* el = code->insts[0]->inst.El();
  if(el->GetType() != T_CodeBlock) return false;
  cssCodeBlock* sub_guy = (cssCodeBlock*)el;
  if(sub_guy->action != JUST_CODE) return false;

  cssProg* old_code = code;
  code = sub_guy->code;		// get his
  cssProg::Ref(code);
  code->owner_blk = this;	// reparent

  cssProg::unRefDone(old_code);	// get rid of mine
  return true;
}

cssEl::RunStat cssCodeBlock::Do(cssProg* prg) {
  prog = prg;
  if(action == IF_TRUE) {
    cssEl* cond = prg->Stack()->Peek(); // don't consume that stack guy; pop at end (others may need to refer)
    if(!(bool)*cond) return cssEl::Running;	// do not run!
  }
  else if(action == ELSE) {
    cssEl* cond = prg->Stack()->Peek(); // don't consume that stack guy; pop at end (others may need to refer)
    if(((bool)*cond)) return cssEl::Running;	// do not run!
  }

  code->AddFrame();		// need to add the new frame first
  code->SetTop(prg->top);	// propagate top to fun
  prg->top->AddProg(code);	// push new state (not Shove, needed to add frame before)

  code->SetCurThis(prg->CurThis());	// carry this pointer down for blocks..
  // don't do this: we need to be able to tell where the fun is or not..
//   code->owner_fun = prg->owner_fun; // carry pointer down..

  return cssEl::NewProgShoved;	// new program shoved onto stack
}

cssEl::RunStat cssCodeBlock::FunDone(cssProg* prg) {
  prog = prg;			// restore if recursive things happened
  cssEl* rval = NULL;
  if(action == PUSH_RVAL) {
    cssEl* tmp = prg->top->Prog()->Stack()->Peek();
    rval = tmp->AnonClone();
  }
  prg->top->EndRunPop();
  prg->top->Pull();		// pop up to next level on stack

  if(rval)
    prg->top->Prog()->Stack()->Push(rval);

  if(loop_back > 0) {
    prg->top->Prog()->Frame()->pc -= loop_back;
    prg->top->EndRunPop();	// if looping back, always pop last off stack
  }

  if(prg->top->run_stat == cssEl::Stopping)
    return cssEl::Running;	// stopped naturally

  return prg->top->run_stat;	// some other kind of stopping
}

cssEl* cssCodeBlock::MakeToken_stub(int na, cssEl* arg[]) {
  return new cssInt(0);		// default retv is int, not void
}

String cssCodeBlock::PrintStr() const {
  String str = name;
  if(action == PUSH_RVAL) str += "(push_rval)";	
  else if(action == IF_TRUE) str += "(if true)";	
  else if(action == ELSE) str += "(else)";
  if(loop_back > 0)
    str += ", loop_back: " + String(loop_back);
  return str;
}

//////////////////////////////////////////////////
// cssScriptFun: Script-defined functions	//
//////////////////////////////////////////////////

void cssScriptFun::Constr() {
  argv = new cssElPtr[ArgMax + 1];
  fun = new cssProg(name);
  cssProg::Ref(fun);
  fun->owner_fun = this;
}

void cssScriptFun::Copy(const cssScriptFun& cp) {
  cssElFun::Copy(cp);
  if(fun)
    cssProg::unRefDone(fun);
  // note that this means that you could lose derived classes..
  fun = cp.fun;
  fun->owner_fun = this;
  cssProg::Ref(fun);
  int i;
  for(i=0; i<= ArgMax; i++)
    argv[i] = cp.argv[i];
}

cssScriptFun::cssScriptFun() {
  Constr();
  argc = 0;
}

cssScriptFun::cssScriptFun(const String& nm) {
  Constr();
  argc = 0;
  name = nm;
  fun->name = name;
}
cssScriptFun::cssScriptFun(const cssScriptFun& cp) {
  argv = new cssElPtr[ArgMax + 1];
  fun = NULL;
  //  Constr();
  Copy(cp);
  name = cp.name;
}
cssScriptFun::cssScriptFun(const cssScriptFun& cp, const String& nm) {
  argv = new cssElPtr[ArgMax + 1];
  fun = NULL;
  //  Constr();
  Copy(cp);
  name = nm;
  fun->name = name;
}
cssScriptFun::~cssScriptFun() {
  delete [] argv;
  if(fun)
    cssProg::unRefDone(fun);
}

void cssScriptFun::Define(cssProg* prg, bool, const String& nm) {
  if(nm.nonempty())
    name = nm;
  cssProgSpace* old_top = fun->SetTop(prg->top); // propagate top
  fun->name = name;

  cssRef* tmp = (cssRef*)prg->Stack()->Pop();  	// a ptr
  argv[0] = fun->AddAuto(tmp->ptr.El()); 	// retval
  cssEl::Done(tmp);

  cssSpace* stack = prg->Stack();
  int stack_start;		// where to start getting things off of the stack
  for(stack_start = stack->size-1; stack_start >= 0; stack_start--) {
    if(stack->FastEl(stack_start) == &cssMisc::Void)
      break;
  }
  argc = ((int)stack->size - stack_start) -1;
  int i;
  for(i=argc; i>0; i--) {
    tmp = (cssRef*)stack->Pop();
    argv[i] = fun->AddAuto(tmp->ptr.El());
    cssEl::Done(tmp);
  }
  if(stack->Peek() == &cssMisc::Void)	// get rid of arg stop..
    stack->Pop();
  GetArgDefs();
  fun->PopTop(old_top);
}

cssEl::RunStat cssScriptFun::Do(cssProg* prg) {
  prog = prg;
  fun->AddFrame();		// need to add the new frame first
  fun->Frame()->AllocArgs();	// explicitly allocate the args
  cssEl** args = fun->Args();	// 
  int& act_argc = fun->ActArgc();
  BindArgs(args, act_argc);	// get arguments from previous space
  if(act_argc < 0)
    return cssEl::ExecError;

  fun->SetTop(prg->top);	// propagate top to fun
  prg->top->AddProg(fun);	// push new state (not Shove, needed to add frame before)

  for(int i=1; i <= act_argc; i++) { // copy into args for current space
    (argv[i].El())->InitAssign(*args[i]);
  }
  return cssEl::NewProgShoved;	// new program shoved onto stack
}

cssEl::RunStat cssScriptFun::FunDone(cssProg* prg) {
  prog = prg;			// restore if recursive things happened
  prg->top->EndRunPop();

  cssEl** args = fun->Args();
  int& act_argc = fun->ActArgc();

  cssEl* tmp = (argv[0].El())->AnonClone(); // create clone of retval
  tmp->prog = prg;
  prg->top->PopProg();	// note -- cannot run Pull (DelFrame + Pop) cuz need frame for args!
  if(!prog->top->external_stop && (tmp)) {
    prg->Stack()->Push(tmp);
  }
  DoneArgs(args, act_argc);
  fun->DelFrame(); // now it is safe to delete the frame, after done args!
  return cssEl::Running;	// returning from a running program
}

cssEl* cssScriptFun::MakeToken_stub(int na, cssEl* arg[]) {
  return (argv[0].El())->MakeToken_stub(na, arg);
}

String cssScriptFun::PrintStr() const {
  String str = String(argv[0].El()->GetTypeName()) + " " + name + "(";
  int i;
  for(i=1; i<=argc; i++) {
    str += String(argv[i].El()->GetTypeName()) + " " + argv[i].El()->name;
    if(i >= def_start) {
      str += " = ";
      str += arg_defs.El((i-def_start))->PrintFStr();
    }
    if(i < argc)
      str += ", ";
  }
  str += ")";
  return str;
}


//////////////////////////////////////////////////////////////////
// 	cssMbrScriptFun: Script-defined member functions	//
//////////////////////////////////////////////////////////////////

void cssMbrScriptFun::Constr() {
  type_def = NULL;
  is_tor = false;
  is_virtual = false;
}

void cssMbrScriptFun::Copy(const cssMbrScriptFun& cp) {
  cssScriptFun::Copy(cp);
  type_def = cp.type_def;
  if(type_def)
    cssEl::Ref(type_def);
  desc = cp.desc;
  opts = cp.opts;
  is_tor = cp.is_tor;
  is_virtual = cp.is_virtual;
}

cssMbrScriptFun::cssMbrScriptFun() 
  : cssScriptFun() {
  Constr();
}

cssMbrScriptFun::cssMbrScriptFun(const String& nm, cssClassType* cls)
: cssScriptFun(nm) {
  Constr();
  type_def = cls;
  if(type_def)
    cssEl::Ref(type_def);
}
cssMbrScriptFun::cssMbrScriptFun(const cssMbrScriptFun& cp) {
  Copy(cp);
  name = cp.name;
}
cssMbrScriptFun::cssMbrScriptFun(const cssMbrScriptFun& cp, const String& nm) {
  Copy(cp);
  name = nm;
}

cssMbrScriptFun::~cssMbrScriptFun() {
  if(type_def)
    cssEl::unRefDone(type_def);
  type_def = NULL;
}

// on the relationship between the same function in different classes:
// when the function is the same: (inherited from parent)
// 	- fun is a pointer to the same (shared) cssProg
// 	- cssMbrScriptFun is different, but its argv's are initialized to point
//		into the same fun auto's

void cssMbrScriptFun::Define(cssProg* prg, bool decl, const String& nm) {
  if(nm.nonempty())
    name = nm;
  // code was previously defined, and needs to be overwritten
  if((fun->size > 0) || (decl && (fun->Autos(0)->size > 0))) {
    cssProg* old_fun = fun;
    fun = new cssProg(old_fun->name);
    cssProg::Ref(fun);
    fun->owner_fun = this;
    cssProg::unRefDone(old_fun);
  }
  cssProgSpace* old_top = fun->SetTop(prg->top); // propagate top
  fun->name = type_def->name + "::" + name;

  // all the args are on the stack, with the return value at the end (not start)
  cssSpace* stack = prg->Stack();
  cssRef* retval_ptr = (cssRef*)stack->Pop(); // get this for later..

  int stack_start;		// where to start getting things off of the stack
  for(stack_start = stack->size-1; stack_start >= 0; stack_start--) {
    if(stack->FastEl(stack_start) == &cssMisc::Void)
      break;
  }
  int actual_argc = (int)stack->size - stack_start;	// add 1

  if(decl) {
    if((argc > 0) && (actual_argc != argc)) {
      cssMisc::Warning(NULL, "Warning: redefining function arg count to:", String(actual_argc),
		       "from:", String(argc));
    }
  }

  if(fun->Autos(0)->size > 0) {	// already defined
    if(actual_argc != argc) {
      cssMisc::Warning(NULL, "Function definition arg count (", String(actual_argc),
		       ") does not match declaration:", String(argc));
    }
    // get the original ones..
    cssElPtr elp;
    elp.SetProgAuto(fun);
    int i;
    for(i=0; i<=argc; i++) {
      elp.dx = i;		// index into auto's
      argv[argc-i] = elp;	// set argv pointer..
    }
    // then check with current ones..
    if(actual_argc == argc) {
      for(i=argc; i>1; i--) {		// don't write over 1st 2 (0 and 1)
	cssRef* tmp = (cssRef*)stack->Pop();
	cssEl* carv = argv[i].El();
	if(carv->GetType() != tmp->ptr.El()->GetType()) {
	  cssMisc::Warning(NULL, "Type mismatch for argument:", carv->GetName(),
			   "should be of type", carv->GetTypeName());
	}
	carv->name = tmp->ptr.El()->name; // use defined name, not declared one!
	cssEl::Done(tmp);
      }
      cssEl* carv = argv[0].El();
      if(carv->GetType() != retval_ptr->ptr.El()->GetType()) {
	cssMisc::Warning(NULL, "Type mismatch for return value, should be of type",
			 carv->GetTypeName());
      }
    }
    stack->Reset();
  }
  else {
    argc = actual_argc;
    int i;
    for(i=argc; i>1; i--) {		// don't write over 1st 2 (0 and 1)
      cssRef* tmp = (cssRef*)stack->Pop();
      argv[i] = fun->AddAuto(tmp->ptr.El());
      cssEl::Done(tmp);
    }
    if(stack->Peek() == &cssMisc::Void)	// get rid of arg stop..
      stack->Pop();

    // install a pointer to hold the "this" pointer
    cssPtr* ths_ptr = new cssPtr();
    ths_ptr->name = "this";	// its actually called this..
    argv[1] = fun->AddAuto(ths_ptr); // first auto must be ptr to this..

    argv[0] = fun->AddAuto(retval_ptr->ptr.El()); 	// retval
  }

  cssEl::Done(retval_ptr);
  GetArgDefs();
  fun->PopTop(old_top);
}

cssEl::RunStat cssMbrScriptFun::Do(cssProg* prg) {
  prog = prg;
  fun->AddFrame();		// need to add the new frame first
  fun->Frame()->AllocArgs();	// explicitly allocate the args
  cssEl** args = fun->Args();	// 
  int& act_argc = fun->ActArgc();
  BindArgs(args, act_argc);   // get arguments from previous space
  if(act_argc < 0)
    return cssEl::ExecError;

  fun->SetTop(prg->top);	// propagate top to fun
  prg->top->AddProg(fun);	// push new state (not Shove, needed to add frame before)

  fun->SetCurThis(prg->CurThis()); // first use previous..

  // first argument is the this pointer..
  cssPtr* ths_ptr = (cssPtr*)(argv[1].El());
  ths_ptr->SetPtr(args[1]->GetAddr()); // get address of this pointer
  cssClassInst* cur_ths = (cssClassInst*)ths_ptr->ptr.El();
  if(cur_ths->GetType() != T_Class) {
    cssMisc::Error(prog, "'this' arg:", cur_ths->name, "of type:",
		   cur_ths->GetTypeName(),"is not a class object");
    return cssEl::ExecError;
  }
  ths_ptr->SetElType(cur_ths->type_def);

  int i;
  for(i=2; i <= act_argc; i++) {	// copy into args for current space
    (argv[i].El())->InitAssign(*args[i]);
  }

  // only set new cur_this after copying the args..
  fun->SetCurThis(cur_ths);

  cssMisc::cur_class = type_def;	// set the current class while in here..
  cssMisc::cur_method = this;

  return cssEl::NewProgShoved;	// new program shoved onto stack
}

cssEl::RunStat cssMbrScriptFun::FunDone(cssProg* prg) {
  prog = prg;			// restore if recursive things happened
  prg->top->EndRunPop();

  cssEl** args = fun->Args();
  int& act_argc = fun->ActArgc();
  cssPtr* ths_ptr = (cssPtr*)(argv[1].El());

  ths_ptr->DelOpr();		// get rid of pointer to this object

  cssEl* tmp = (argv[0].El())->AnonClone(); // create clone of retval
  tmp->prog = prg;
  // prg->top->PopTop(old_top);		       // restore previous top todo: not doing!
  prg->top->PopProg();	// note -- cannot run Pull (DelFrame + Pop) cuz need frame for args!
  if(!prog->top->external_stop && (tmp)) {
    prg->Stack()->Push(tmp);
  }
  DoneArgs(args, act_argc);
  fun->DelFrame(); // now it is safe to delete the frame, after done args!
  return cssEl::Running;		// returning from a running program
}

cssEl* cssMbrScriptFun::MakeToken_stub(int na, cssEl* arg[]) {
  return (argv[0].El())->MakeToken_stub(na, arg);
}

String cssMbrScriptFun::PrintStr() const {
  String str;
  if(is_virtual)
    str = "virtual ";
  str += String(argv[0].El()->GetTypeName()) + " " + name + "(";
  int i;
  for(i=2; i<=argc; i++) {
    str += String(argv[i].El()->GetTypeName()) + " " + argv[i].El()->name;
    if(i >= def_start) {
      str += " = ";
      str += arg_defs.El((i-def_start))->PrintFStr();
    }
    if(i < argc)
      str += ", ";
  }
  str += ")";
  return str;
}

void cssMbrScriptFun::SetDesc(const String& des) {
  desc = "";
  String tmp = des;
  tmp.gsub("\"", "'");		// don't let any quotes get through
  tmp.gsub("\n", " ");		// replace whitespace..
  tmp.gsub("\t", " ");		// replace whitespace..
  String ud;
  while(tmp.contains('#')) {
    desc += tmp.before('#');
    tmp = tmp.after('#');
    if(tmp.contains(' '))
      ud = tmp.before(' ');
    else
      ud = tmp;
    tmp = tmp.after(' ');
    opts += ud + " ";
  }
  desc += tmp;
}

String cssMbrScriptFun::OptionAfter(const String& opt) {
  if(!opts.contains(opt))
    return "";
  String rval = opts.after(opt);
  rval = rval.before(' ');
  return rval;
}

//////////////////////////////////////////
//    cssCPtr: Internal Pointer Type	//
//////////////////////////////////////////

void cssCPtr::Constr() {
  class_parent = NULL; ptr = NULL;
  ptr_cnt = 1;
  flags = NO_PTR_FLAGS;
}

void cssCPtr::Copy(const cssCPtr& cp) {
  cssEl::Copy(cp);
  ptr_cnt = cp.ptr_cnt;
  flags = cp.flags;
  if(!(flags & OWN_OBJ))
    ptr = cp.ptr;
  if(cp.class_parent) SetClassParent(cp.class_parent);
}

void cssCPtr::CopyType(const cssCPtr& cp) {
  ptr_cnt = cp.ptr_cnt;
  flags = cp.flags;
  if(cp.class_parent) SetClassParent(cp.class_parent);
}

cssCPtr::cssCPtr() {
  Constr();
}
cssCPtr::cssCPtr(void* it, int pc, const String& nm, cssEl* cp, bool ro) {
  Constr();
  if(nm.nonempty()) name = nm;
  ptr = it;
  ptr_cnt = pc;
  if(cp) SetClassParent(cp);
  if(ro) flags = (PtrFlags)(flags | READ_ONLY);
}
cssCPtr::cssCPtr(const cssCPtr& cp) {
  Constr(); Copy(cp); name = cp.name;
}
cssCPtr::cssCPtr(const cssCPtr& cp, const String& nm) {
  Constr(); Copy(cp);  name = nm;
}
cssCPtr::~cssCPtr()		   	   	{
  if(class_parent) cssEl::unRefDone(class_parent);
}

void* cssCPtr::GetVoidPtr(int cnt) const {
  if(cnt == ptr_cnt) return ptr;
  if((cnt <= 1) && (ptr_cnt <= 1)) return ptr;
  if((ptr_cnt == 2) && (cnt == 1)) {
    if(ptr) return *((void**)ptr);
    return NULL;
  }
  cssMisc::Error(prog, "GetVoidPtr: Cannot get pointer of count:", String(cnt),
		 "from pointer of count:", String(ptr_cnt));
  return NULL;
}

void* cssCPtr::GetNonNullVoidPtr(const char* opr, int cnt) const {
  void* rval = GetVoidPtr(cnt);
  if(!rval) {
    cssMisc::Error(prog, "Null c-pointer in operation:", opr);
    return NULL;
  }
  return rval;
}

String cssCPtr::PrintStr() const {
  String rval = String(GetTypeName())+" "+ name+" --> ";
  if(GetVoidPtr())
    rval += GetStr();
  else
    rval += "NULL";
  return rval;
}

bool cssCPtr::ROCheck() {
  if(!cssMisc::obey_read_only) return true;
  if(flags & READ_ONLY) {
    cssMisc::Error(prog, "Cannot modify pointer:", name, "of type:",GetTypeName(),
		   " -- it points to a read-only object");
    return false;
  }
  return true;
}

bool cssCPtr::PtrAssignPtrPtr(void* new_ptr_val) {
  if(!ptr) {
    cssMisc::Error(prog,  "Failed to assign C pointer-pointer of type:", GetTypeName(),
		   "our ptr is NULL");
    return false;
  }
  *((void**)ptr) = new_ptr_val;
  UpdateClassParent();
  return true;
}

void cssCPtr::PtrAssignNull() {
  if(ptr_cnt == 1) {
    ptr = NULL;		// I now point to that guy
  }
  else if(ptr_cnt == 2) {
    // I'm a ptr-ptr and this sets me to point to another guy
    PtrAssignPtrPtr(NULL);
    UpdateClassParent();
  }
}

bool cssCPtr::PtrAssignNullInt(const cssEl& s) {
  if(s.GetType() != T_Int) return false;
  int sval = (Int)s;
  if(sval == 0) {
    PtrAssignNull();
    return true;
  }
  return false;
}

bool cssCPtr::AssignCheckSource(const cssEl& s) {
  if((s.GetType() != T_C_Ptr) || (s.GetPtrType() != GetPtrType())) {
    cssMisc::Error(prog,  "Failed to assign C pointer of type:", GetTypeName(),
		   "source object is not an appropriate type:", s.GetTypeName());
    return false;
  }
  return true;
}

void cssCPtr::PtrAssignPtr(const cssEl& s) {
  if(PtrAssignNullInt(s)) return;
  if(!AssignCheckSource(s)) return;
  cssCPtr& sp = (cssCPtr&)s;
  if(ptr_cnt == sp.ptr_cnt) {
    ptr = sp.ptr;
    SetClassParent(sp.class_parent);
  }
  else if((ptr_cnt == 1) && (sp.ptr_cnt == 0)) {
    ptr = sp.ptr;		// I now point to that guy
    SetClassParent(sp.class_parent);
  }
  else if((ptr_cnt == 2) && (sp.ptr_cnt == 1)) {
    // I'm a ptr-ptr and this sets me to point to another guy
    PtrAssignPtrPtr(sp.ptr);
  }
}

void cssCPtr::operator=(const cssEl& s) {
  if(!ROCheck()) return;
  if(ptr_cnt > 0) {
    PtrAssignPtr(s);
    return;
  }
  cssMisc::Error(prog, "Failed to copy C object of type:", GetTypeName(),
		 "no copy semantics available");
}

bool cssCPtr::SamePtrLevel(cssCPtr* s) {
  if(ptr_cnt == s->ptr_cnt) return true;
  cssMisc::Warning(prog, "Warning: comparison between different ptr types");
  return false;
}

void cssCPtr::UpdateAfterEdit() {
  UpdateClassParent();
}

bool cssCPtr::operator==(cssEl& s) {
  if(s.GetType() == T_C_Ptr) {
    cssCPtr* pt = (cssCPtr*)s.GetNonRefObj();
    if(SamePtrLevel(pt))
      return ptr == pt->ptr;
    else
      return false;
  }
  return ((Int)(long)(ptr) == (Int)s);
}

bool cssCPtr::operator!=(cssEl& s) {
  if(s.GetType() == T_C_Ptr) {
    cssCPtr* pt = (cssCPtr*)s.GetNonRefObj();
    if(SamePtrLevel(pt))
      return ptr != pt->ptr;
    else
      return false;
  }
  return ((Int)(long)(ptr) != (Int)s);
}


cssEl* cssCPtr::operator*() {
  if(ptr_cnt == 0) {
    cssMisc::Error(prog, "Attempt to de-pointer a non-pointer object (ptr_cnt = 0)",
		   name);
    return this;		// magic ptr derefs to itself
  }

  cssCPtr* rval = (cssCPtr*)AnonClone();
  if(rval->ptr_cnt > 1)
    rval->ptr = *((void**)ptr);	// go one down, only if it is a real pointer (1 & 0 are both same)
  rval->ptr_cnt--;
  return rval;
}

void cssCPtr::SetClassParent(cssEl* cp) {
  if(class_parent)
    cssEl::unRefDone(class_parent);
  class_parent = cp;
  if(cp)
    cssEl::Ref(class_parent);
}

void cssCPtr::UpdateClassParent() {
  if(!cssMisc::call_update_after_edit || !class_parent) return;
  class_parent->UpdateAfterEdit();
}


//////////////////////////////////////////////////
// 	cssLex, Def: pre-processor stuff	//
//////////////////////////////////////////////////

// todo: move static lex functions to some other class..

String cssLex::Buf;

int cssLex::readword(cssProg* prog, int c) {
  Buf = "";

  Buf += (char)c;
  while (((c=prog->top->Getc()) != EOF) && (isalnum(c) || (c == '_')))
  { Buf += (char)c; }
  if(c == EOF)
    return EOF;
  prog->top->unGetc();
  return c;
}

void cssDef::Constr() {
}

cssDef::cssDef() {
  Constr();
  argc = 0;
}

cssDef::cssDef(int ac) {
  Constr(); argc = ac;
}
cssDef::cssDef(int ac, const String& nm) {
  Constr(); name = nm;  argc = ac;
}
cssDef::cssDef(const cssDef& cp) {
  Constr(); Copy(cp); val = cp.val; which_arg = cp.which_arg;
}
cssDef::cssDef(const cssDef& cp, const String& nm) {
  Constr(); Copy(cp); val = cp.val; which_arg = cp.which_arg; name = nm;
}

void cssDef::Skip_To_Endif(cssProg* prog) {
  int cur_prog_size = prog->size;
  int incount = 0;

  while(1) {
    int c = prog->top->Getc();
    if(c == '#') {
      cssLex::readword(prog, prog->top->Getc());
      if(cssLex::Buf == "endif") {
	if(incount == 0)
	  break;
	else
	  incount--;
      }
      if((cssLex::Buf == "else") && (incount == 0))
	break;
      if((cssLex::Buf == "ifdef") || (cssLex::Buf == "ifndef"))
	incount++;
    }
    if(c == EOF)
      break;
  }
  prog->top->unGetc();
  prog->ZapFrom(cur_prog_size);
}

cssEl::RunStat cssDef::Do(cssProg*) {
// nothing, yet
  return cssEl::Running;
}


//////////////////////////////////////////
// 	cssSpace: the final frontier	//
//////////////////////////////////////////

// copy actually clones the elements of a space, so that the new space has fresh data

void cssSpace::Alloc(int sz) {
  if(alloc_size >= sz)	return;	// no need to increase..
  sz = MAX(16,sz);		// once allocating, use a minimum of 16
  alloc_size += TA_ALLOC_OVERHEAD; // increment to full power of 2
  while((alloc_size-TA_ALLOC_OVERHEAD) <= sz) alloc_size <<= 1;
  alloc_size -= TA_ALLOC_OVERHEAD;
  els = (cssEl**)realloc(els, alloc_size * sizeof(cssEl*));
}

void cssSpace::Constr() {
  els = (cssEl**)malloc(alloc_size * sizeof(cssEl*));
  size = 0;
  el_retv.SetSpace(this);
}

void cssSpace::Copy(const cssSpace &cp) {
  int i;
  for(i=0; i < cp.size; i++)
    Push(cp.els[i]->Clone());
}

void cssSpace::Copy_NoNames(const cssSpace &cp) {
  int i;
  for(i=0; i < cp.size; i++)
    Push(cp.els[i]->AnonClone());
}

void cssSpace::Copy_Blanks(const cssSpace &cp) {
  int i;
  for(i=0; i < cp.size; i++)
    Push(cp.els[i]->BlankClone());
}

void cssSpace::CopyUniqNameNew(const cssSpace &cp) {
  int i;
  for(i=0; i < cp.size; i++)
    PushUniqNameNew(cp.els[i]->Clone());
}

void cssSpace::CopyUniqNameOld(const cssSpace &cp) {
  int i;
  for(i=0; i < cp.size; i++)
    PushUniqNameOld(cp.els[i]->Clone());
}

void cssSpace::Reset() {
  int i;
  for (i=size - 1; i >= 0; --i)
    cssEl::unRefDone(els[i]);
  size = 0;
}
cssElPtr& cssSpace::Push(cssEl* it) {
  if(size+1 >= alloc_size)
    Alloc(size+1);
  els[size++] = it;
  cssEl::Ref(it);
  el_retv.dx = size-1;
  return el_retv;
}
cssElPtr& cssSpace::PushUniqNameNew(cssEl* it) {
  cssElPtr p = FindName((char*)it->name);
  if(!p) {
    return Push(it);
  }
  cssEl::unRefDone(p.El());
  els[p.dx] = it;
  cssEl::Ref(it);
  el_retv.dx = p.dx;
  return el_retv;
}

cssElPtr& cssSpace::PushUniqNameOld(cssEl* it) {
  cssElPtr p = FindName((char*)it->name);
  if(!p) {
    return Push(it);
  }
  cssEl::Done(it);
  return el_retv;
}

bool cssSpace::Replace(cssEl* old, cssEl* nw) {
  int idx = GetIndex(old);
  if(idx < 0)
    return false;
  cssEl::unRefDone(old);
  els[idx] = nw;
  cssEl::Ref(nw);
  return true;
}

bool cssSpace::Remove(cssEl* it) { // this is very dangerous, as the ptrs are idx based
  int i, rval = false;
  for(i=0; i < size; i++) {
    if(els[i] == it) {
      cssEl::unRefDone(els[i]);
      size--;
      rval = true;
      break;
    }
  }
  for(; i < size; i++)		// compact, if necc
    els[i] = els[i+1];
  return rval;
}

cssElPtr& cssSpace::FindName(const String& nm) {	// lookup by name
  for(el_retv.dx=0; el_retv.dx<size; el_retv.dx++) {
    if(els[el_retv.dx]->name == nm)
      return el_retv;
  }
  return cssMisc::VoidElPtr;
}

int cssSpace::IndexOfName(const String& nm) const {
  for (int i = 0; i < size; ++i) {
    if (els[i]->name == nm) 
      return i;
  }
  return -1;
}

cssElPtr& cssSpace::Find(Int nm) {	// lookup by number value
  for(el_retv.dx=0; el_retv.dx<size; el_retv.dx++) {
    if((els[el_retv.dx]->GetType() == cssEl::T_Int) && (els[el_retv.dx]->name == "") &&
       ((Int)*(els[el_retv.dx]) == nm))
      return el_retv;
  }
  return cssMisc::VoidElPtr;
}
cssElPtr& cssSpace::Find(Real nm) {	// lookup by number value
  for(el_retv.dx=0; el_retv.dx<size; el_retv.dx++) {
    if((els[el_retv.dx]->GetType() == cssEl::T_Real) && (els[el_retv.dx]->name == "") &&
       ((Real)*(els[el_retv.dx]) == nm))
      return el_retv;
  }
  return cssMisc::VoidElPtr;
}
cssElPtr& cssSpace::Find(const String& nm) {	// lookup by stirng value
  for(el_retv.dx=0; el_retv.dx<size; el_retv.dx++) {
    if((els[el_retv.dx]->GetType() == cssEl::T_String) && (els[el_retv.dx]->name == "") &&
       (els[el_retv.dx]->GetStr() == nm))
      return el_retv;
  }
  return cssMisc::VoidElPtr;
}

int cssSpace::GetIndex(cssEl* it) {
  int i;
  for(i=0; i < size; i++) {
    if(els[i] == it)
      return i;
  }
  return -1;
}

ostream& cssSpace::fancy_list(ostream& fh, const String& itm, int no, int prln, int tabs, int indent) {
  fh << itm << " ";
  if((no+1) % prln == 0) {
    fh << "\n";
    taMisc::FlushConsole();
    fh << cssMisc::Indent(indent);
    return fh;
  }
  int len = itm.length() + 1;
  int spc_ln = tabs * 8 - len;
  spc_ln = MAX(1, spc_ln);
  fh << String(spc_ln , 0, ' ');
  return fh;
}
pager_ostream& cssSpace::fancy_list(pager_ostream& fh, const String& itm, int no, int prln, int tabs, int indent) {
  fh << itm << " ";
  if((no+1) % prln == 0) {
    fh << "\n";
    taMisc::FlushConsole();
    fh << cssMisc::Indent(indent);
    return fh;
  }
  int len = itm.length() + 1;
  int spc_ln = tabs * 8 - len;
  spc_ln = MAX(1, spc_ln);
  fh << String(spc_ln , 0, ' ');
  return fh;
}
String& cssSpace::fancy_list(String& fh, const String& itm, int no, int prln, int tabs, int indent) {
  fh += itm + " ";
  if((no+1) % prln == 0) {
    fh += "\n";
    fh += cssMisc::Indent(indent);
    return fh;
  }
  int len = itm.length() + 1;
  int spc_ln = tabs * 8 - len;
  spc_ln = MAX(1, spc_ln);
  fh += String(spc_ln , 0, ' ');
  return fh;
}

void cssSpace::List(ostream& fh, int indent, int per_line) const {
  fh << cssMisc::Indent(indent) << "Elements of Space: " << name << " (" << size << ")\n";
  fh << PrintStr(indent, per_line) << "\n";
  fh.flush();
}
void cssSpace::List(pager_ostream& fh, int indent, int per_line) const {
  fh << cssMisc::Indent(indent) << "Elements of Space: " << name << " (" << size << ")\n";
  fh << PrintStr(indent, per_line) << "\n";
}
void cssSpace::ValList(ostream& fh, int indent, int per_line) const {
  fh << cssMisc::Indent(indent) << "Element Values of Space: " << name << " (" << size << ")\n";
  fh << PrintFStr(indent, per_line) << "\n";
  fh.flush();
}
void cssSpace::NameList(pager_ostream& fh, int indent, int per_line) const {
  fh << "Element Names of Space: " << name << " (" << size << ")\n";
  int tabs = 0;
  int prln = 1;
  if(per_line < 1) {
    int names_width = 0;
    for(int i=0; i<size; i++) {
      names_width = MAX(names_width, (int)els[i]->name.length());
    }
    tabs = (names_width / 8) + 1;
    prln = taMisc::display_width / (tabs * 8);
    if(prln <= 0) prln = 1;
  }
//   String fl = cssMisc::Indent(indent);
  fh << cssMisc::Indent(indent);
  for(int i=0; i<size; i++) {
    cssSpace::fancy_list(fh, els[i]->name, i, prln, tabs, indent);
  }
//   fh << fl;
  fh << "\n";
}
void cssSpace::NameList(ostream& fh, int indent, int per_line) const {
  pager_ostream pgos;
  pgos.fout = &fh; pgos.no_page = true;
  NameList(pgos, indent, per_line);
}

void cssSpace::TypeNameList(ostream& fh, int indent) const {
  fh << cssMisc::Indent(indent);
  int i;
  for(i=0; i<size; i++) {
    cssEl* mbr = els[i];
    String tmp = mbr->GetTypeName();
    if(tmp.contains(')')) {
      tmp = tmp.before(')');
      tmp = tmp.after('(');
    }
    tmp = String("  ") + tmp;
    fh << tmp;
    if(tmp.length() >= 24)
      fh << " ";
    else if(tmp.length() >= 16)
      fh << "\t";
    else if(tmp.length() >= 8)
      fh << "\t\t";
    else
      fh << "\t\t\t";
    fh << mbr->name;
    if (mbr->GetType() == cssEl::T_Array) {
      cssArray* ar = (cssArray*) mbr->GetNonRefObj();
      fh << '[' << ar->items->size << ']';
    }
    else if (mbr->GetType() == cssEl::T_ArrayType) {
      cssArrayType* ar = (cssArrayType*) mbr->GetNonRefObj();
      fh << '[' << ar->size << ']';
    }
    fh << "\n";
    taMisc::FlushConsole();
    fh << cssMisc::Indent(indent);
  }
}

void cssSpace::TypeNameValList(ostream& fh, int indent) const {
  fh << cssMisc::Indent(indent);
  int i;
  cssEl* mbr;
  String tmp;
  for(i=0; i<size; i++) {
    mbr = els[i];
    tmp = mbr->GetTypeName();
    if(tmp.contains(')')) {
      tmp = tmp.before(')');
      tmp = tmp.after('(');
    }
    tmp = String("  ") + tmp;
    fh << tmp;
    if(tmp.length() >= 24)
      fh << " ";
    else if(tmp.length() >= 16)
      fh << "\t";
    else if(tmp.length() >= 8)
      fh << "\t\t";
    else
      fh << "\t\t\t";
    fh << mbr->name << " = " << mbr->PrintFStr() << "\n";
    taMisc::FlushConsole();
    fh << cssMisc::Indent(indent);
  }
}

String cssSpace::PrintStr(int indent, int per_line) const {
  int tabs = 0;
  int prln = 1;
  if(per_line < 1) {
    int vars_width = 0;
    for(int i=0; i<size; i++) {
      String tmp = els[i]->PrintStr();
      vars_width = MAX(vars_width, (int)tmp.length());
    }
    tabs = (vars_width / 8) + 1;
    prln = taMisc::display_width / (tabs * 8) - indent;
    if(prln <= 0) prln = 1;
  }
  String rval = cssMisc::Indent(indent);
  for(int i=0; i<size; i++) {
    String tmp = els[i]->PrintStr();
    cssSpace::fancy_list(rval, tmp, i, prln, tabs, indent);
  }
  return rval;
}

String cssSpace::PrintFStr(int indent, int per_line) const {
  int tabs = 0;
  int prln = 1;
  if(per_line < 1) {
    int vals_width = 0;
    for(int i=0; i<size; i++) {
      String tmp = els[i]->PrintFStr();
      vals_width = MAX(vals_width, (int)tmp.length());
    }
    tabs = (vals_width / 8) + 1;
    prln = taMisc::display_width / (tabs * 8);
    if(prln <= 0) prln = 1;
  }
  String rval = cssMisc::Indent(indent);
  for(int i=0; i<size; i++) {
    String tmp = els[i]->PrintFStr();
    cssSpace::fancy_list(rval, tmp, i, prln, tabs, indent);
  }
  return rval;
}

namespace { // anonymous
  // Functor to sort cssEl pointers
  struct SortPtrCssEl
    : public std::binary_function<const cssEl *, const cssEl *, bool>
  {
    bool operator()(const cssEl *el1, const cssEl *el2) const 
    {
      return el1->name < el2->name;
    }
  };
}

void cssSpace::Sort() {
  const bool debug = false; // print out all list elements after sorting
  const bool warn = true; // warn about duplicates after sorting
  
  // No need for std::stable_sort since els should be unique.
  std::sort(els, els + size, SortPtrCssEl());
  
  if (warn || debug) {
    if (debug) {
      cssMisc::Warning(0, "\ncssSpace", name, "has been sorted:");
    }

    // Check for duplicates (shouldn't be any)
    const String *prev = 0;
    for (int idx = 0; idx < size; ++idx) {
      String *curr = &els[idx]->name;
      if (prev && *curr == *prev) {
        cssMisc::Warning(0, *curr, "is in cssSpace", name, "more than once");
      }
      else if (debug) {
        cssMisc::Warning(0, *curr);
      }
      prev = curr;
    }
  }
}


//////////////////////////////////
// 	cssInst: Instructions	//
//////////////////////////////////

// class CSS_API cssListEl {
// public:
//   css_progdx    stpc;		// starting pc for this line
//   int		ln;		// line no in source code
//   String	src;		// source code for line, specifically for this code element
//   String	full_src;	// full source code 

//   void		Copy(const cssListEl& cp)
//   { stpc = cp.stpc; ln = cp.ln; src = cp.src; full_src = cp.full_src; }

//   cssListEl()			{ stpc = 0;   ln = 0; }
//   cssListEl(css_progdx pc, int l, const String& cd)
//   { stpc = pc;  ln = l;  src = cd;  }
//   cssListEl(const cssListEl& cp)	{ Copy(cp); }

//   cssListEl* 	Clone()	{ return new cssListEl(*this); }
// };


String cssInst::PrintStr() const {
  return prog->top->GetSrcLn(line);
}

void cssInst::ListSrc(pager_ostream& fh, int indent) const {
  fh << PrintStr();
  taMisc::FlushConsole();
}

void cssInst::ListMachine(pager_ostream& fh, int indent) const {
  fh << cssMisc::Indent(indent) << taMisc::LeadingZeros(idx,5) << "  "
     << inst.PrintStr() << "   " << inst.El()->PrintStr() << "\n";
  taMisc::FlushConsole();
}

void cssInst::Constr() {
  prog = NULL;
  idx = 0;
  line = 0;  col = 0;
}

void cssInst::Copy(const cssInst& cp) {
  line = cp.line;
  col = cp.col;
  inst = cp.inst;
}

cssInst::cssInst() {
  Constr();
}
cssInst::cssInst(const cssProg* prg, const cssElPtr& it) {
  Constr();
  prog = (cssProg*)prg;
  SetInst(it);
  idx = -1;
}
cssInst::cssInst(const cssProg* prg, const cssElPtr& it, int lno, int clno) {
  Constr();
  prog = (cssProg*)prg;
  line = lno;	col = clno;
  SetInst(it);
  idx = -1;
}

cssInst::cssInst(const cssInst& cp) {
  Constr();
  Copy(cp);
}

cssInst::~cssInst() {
  // no referencing for auto's and class objs
  if((inst.ptr_type == cssElPtr::PROG_AUTO) ||
     (inst.ptr_type == cssElPtr::CLASS_MEMBER) ||
     (inst.ptr_type == cssElPtr::NVIRT_METHOD) ||
     (inst.ptr_type == cssElPtr::VIRT_METHOD))
  {
    inst.Reset();
  }
  else
    cssEl::DelRefElPtr(inst);
}

void cssInst::SetInst(const cssElPtr& it) {
  if ((it.ptr_type == cssElPtr::SPACE) &&
     ((it.ptr == (void*)&(prog->top->statics)) ||
      (it.ptr == (void*)&(prog->top->hard_vars)) ||
      (it.ptr == (void*)&(prog->top->prog_vars)) ||
      (it.ptr == (void*)&(prog->top->enums)) ||
      (it.ptr == (void*)&(prog->top->hard_funs))))
  {
    cssScriptFun* cur_fun = prog->top->GetCurrentFun();
    if((cur_fun) && ((cur_fun->GetType() == cssEl::T_MbrScriptFun) ||
			     (cssMisc::Externs.GetIndex(cur_fun) >= 0)))
    {
      bool do_msg = true;
      if(cur_fun->GetType() == cssEl::T_MbrScriptFun) {
	cssMbrScriptFun* scrf = (cssMbrScriptFun*)cur_fun;
	if(!scrf->type_def->multi_space) do_msg = false; // if not actually in multiple spaces, don't complain!
      }
      if (do_msg) {
	if (it.ptr == (void*)&(prog->top->prog_vars))
	  cssMisc::Warning(prog, "Warning: Referring to Program variable in a class method",
			   "or extern function, which is non-portable if script is used multiple times");
	else if((it.ptr == (void*)&(prog->top->hard_vars)) ||
	   (it.ptr == (void*)&(prog->top->hard_funs)))
	  cssMisc::Warning(prog, "Warning: Referring to hard-coded variable or function in a class method",
			   "or extern function, which is non-portable if script is used multiple times");
	else
	  cssMisc::Warning(prog, "Warning: Referring to a non-extern (static, local) object in a class method",
			   "or extern function, which is non-portable if script is used multiple times");
      }
      cssElPtr nw_ptr;
      nw_ptr.SetDirect(it.El());
      cssEl::SetRefElPtr(inst, nw_ptr);
      return;
    }
//     else {			// otherwise just set direct for efficiency!!
//       cssElPtr nw_ptr;
//       nw_ptr.SetDirect(it.El());
//       cssEl::SetRefElPtr(inst, nw_ptr);
//       return;
//     }
  } else if ((it.ptr_type == cssElPtr::PROG_AUTO) ||
	  (it.ptr_type == cssElPtr::CLASS_MEMBER) ||
	  (it.ptr_type == cssElPtr::NVIRT_METHOD) ||
	  (it.ptr_type == cssElPtr::VIRT_METHOD))
  {
    if(inst.El() != &cssMisc::Void)  cssEl::unRefDone(inst.El());
    inst = it;			// no referencing for auto's and class objs
    return;
  }
  cssEl::SetRefElPtr(inst, it);
}

cssEl::RunStat cssInst::Do() {	// any "do" must be surrounded by a break catcher
  cssInst* ths = this;
  if(ths->prog->top->run_stat != cssEl::ExecError) {
    return (ths->inst.El())->Do(ths->prog);
  }
  return cssEl::ExecError;
}

//////////////////////////////
// IJump

String cssIJump::PrintStr() const {
  return cssInst::PrintStr() + " // Jump-> " + String(jumpto);
}

void cssIJump::ListMachine(pager_ostream& fh, int indent) const {
  fh << cssMisc::Indent(indent) << taMisc::LeadingZeros(idx,5) << "  Jump-> "
     << taMisc::LeadingZeros(jumpto,5) << "\n";
  taMisc::FlushConsole();
}

cssEl::RunStat cssIJump::Do() {
  if(jumpto < 0)
    return cssEl::Stopping;			// the "STOP" condition
  prog->SetPC(jumpto);
  return cssEl::Running;
}

void cssIJump::Copy(const cssIJump& cp) {
  cssInst::Copy(cp);
  jumpto = cp.jumpto;
}

cssIJump::cssIJump(const cssProg* prg, css_progdx jmp) {
  Constr();
  prog = (cssProg*)prg;
  jumpto = jmp;
  idx = -1;
}

cssIJump::cssIJump(const cssProg* prg, css_progdx jmp, int lno, int clno) {
  Constr();
  prog = (cssProg*)prg;
  jumpto = jmp;
  line = lno;	col = clno;
  idx = -1;
}

cssIJump::cssIJump(const cssIJump& cp) {
  Constr();
  Copy(cp);
}


//////////////////////////////////
// 	cssProg: Programs	//
//////////////////////////////////

void cssProg::Constr() {
  alloc_size = 2;
  fr_alloc_size = 2;
  src_alloc_size = 2;
  el_retv.SetProgAuto(this);
  refn = 0;
  owner_fun = NULL;
  owner_blk = NULL;
  top = cssMisc::Top;
  master_prog = NULL;

  insts = (cssInst**)malloc(alloc_size * sizeof(cssInst*));
  size = 0;

  frame = (cssFrame**)malloc(fr_alloc_size * sizeof(cssFrame*));
  fr_size = 0;

  literals.name = "Literals";
  statics.name = "Statics";
  saved_stack.name = "Saved Stack";

  first_src_ln = -1;
  last_src_ln = -1;

  state = 0;
  lastif = -1;
  lastelseif = false;
  AddFrame();			// always start off with one

  Frame(0)->autos = new cssSpace("autos"); // always have autos in base frame;
  Frame(0)->autos->el_retv.SetProgAuto(this);
}

void cssProg::Copy(const cssProg& cp) {
  literals.Copy(cp.literals);
  statics.Copy(cp.statics);
  Autos(0)->Copy(*(cp.Autos(0)));

  AllocInst(cp.size+1);
  for(int i=0; i<cp.size; i++) {
    cssInst* nwi = cp.insts[i]->Clone();
    insts[i] = nwi;
    nwi->prog = this;
    nwi->idx = i;
    if(nwi->inst.ptr == (void*)&(cp.literals))
      nwi->inst.ptr = (void*)&literals;
    if(nwi->inst.ptr == (void*)&(cp.statics))
      nwi->inst.ptr = (void*)&statics;
    if(nwi->inst.ptr == (void*)&cp)
      nwi->inst.ptr = (void*)this;
  }
  size = cp.size;
  first_src_ln = cp.first_src_ln;
  last_src_ln = cp.last_src_ln;

  state = cp.state;
  lastif = cp.lastif;
  lastelseif = cp.lastelseif;
}

cssProg::cssProg() {
  Constr();
}
cssProg::cssProg(const String& nm) {
  Constr();
  name = nm;
}
cssProg::cssProg(const cssProg& cp) {
  Constr();
  Copy(cp);
}

cssProg::~cssProg() {
  Reset();
  DelFrame();			// get rid of 1st frame
  free(insts);
  free(frame);
}

//////////////////////////////////////////
//	cssProg: Internal Functions 	//
//////////////////////////////////////////

void cssProg::AllocInst(int sz) {
  if(alloc_size >= sz)	return;	// no need to increase..
  sz = MAX(16,sz);		// once allocating, use a minimum of 16
  alloc_size += TA_ALLOC_OVERHEAD; // increment to full power of 2
  while((alloc_size-TA_ALLOC_OVERHEAD) <= sz) alloc_size <<= 1;
  alloc_size -= TA_ALLOC_OVERHEAD;
  insts = (cssInst**)realloc(insts, alloc_size * sizeof(cssInst*));
}

void cssProg::AllocFrame(int sz) {
  if(fr_alloc_size >= sz)	return;	// no need to increase..
  sz = MAX(16,sz);		// once allocating, use a minimum of 16
  fr_alloc_size += TA_ALLOC_OVERHEAD; // increment to full power of 2
  while((fr_alloc_size-TA_ALLOC_OVERHEAD) <= sz) fr_alloc_size <<= 1;
  fr_alloc_size -= TA_ALLOC_OVERHEAD;
  frame = (cssFrame**)realloc(frame, fr_alloc_size * sizeof(cssFrame*));
}

int cssProg::AddFrame() {
  if(fr_size+1 >= fr_alloc_size)
    AllocFrame(fr_size+1);
  cssFrame* nwfr = new cssFrame(this);
  frame[fr_size++] = nwfr;
  if(fr_size > 1) {
    if(Autos(0)->size > 0) {
      nwfr->autos = new cssSpace(Autos(0)->size, _nilString);
      nwfr->autos->el_retv.SetProgAuto(this);
      nwfr->autos->Copy_Blanks(*(Autos(0))); // just need the blank vars here..
    }
  }
//   if(!master_prog) {		// only if we are the master do we get a stack
    nwfr->stack = new cssSpace();
//   }
  return fr_size-1;
}

int cssProg::DelFrame() {
  if(fr_size <= 0)
    return 0;
  delete frame[fr_size-1];
  fr_size--;
  return fr_size+1;
}

void cssProg::Reset() {
  ResetLasts();
  saved_stack.Reset();
  while(fr_size > 1)	DelFrame();
  ResetCode();
  Autos(0)->Reset();
  literals.Reset();
  statics.Reset();
}
void cssProg::ResetCode() {
  int i;
  Restart();
  for(i=0; i<size; i++)
    delete insts[i];
  size = 0;
}

cssScriptFun* cssProg::GetCurrentFun() {
  cssProg* cp = this;
  while(cp) {
    if(cp->owner_fun) return cp->owner_fun;
    if(!cp->owner_blk) return NULL;
    cp = cp->owner_blk->owner_prog;
  }
  return NULL;
}

//////////////////////////////////////////
//	cssProg: Source, Debugging	//
//////////////////////////////////////////

int cssProg::GetSrcLn(css_progdx pcval) const {
  if(pcval > size-1)
    return 0;
  return insts[pcval]->line;
}

int cssProg::FindSrcLn(int ln) const {
  if(ln < first_src_ln || ln > last_src_ln) return -1;
  for(int i = 0; i< size; i++) {
    if(insts[i]->line == ln)
      return i;
  }
  return -1;
}

void cssProg::ListSrc() const {
  if(!top->HaveCmdShell()) return;
  pager_ostream& fh = top->cmd_shell->pgout;
  fh.start();
  for(int ln = first_src_ln; ln <= last_src_ln; ln++) {
    fh << top->GetSrcLn(ln);
    taMisc::FlushConsole();
    if(top->ListDebug() >= 2) {
      ListMachine(fh, 1, ln);
    }
  }
}


void cssProg::ListMachine(pager_ostream& fh, int indent, int ln) const {
  int stpc = FindSrcLn(ln);
  if(stpc < 0) return;
  for(int i=stpc; i < size; i++) {
    if(insts[i]->line == ln) 
      insts[i]->ListMachine(fh, indent);
    cssEl* el = insts[i]->inst.El();
    if(el->GetType() == cssEl::T_CodeBlock) {
      el->GetSubProg()->ListMachine(fh, indent + 1, ln);
    }
  }
}

void cssProg::ListLocals(pager_ostream& fh, int frdx, int indent) {
  if(frdx < 0)
    frdx = fr_size-1;

  String nm = name;
  cssScriptFun* cur_fun = GetCurrentFun();
  if(cur_fun) {
    nm = cur_fun->PrintStr();
    if(top->debug >= 1)
      nm += " (" + name + ")";
  }

  fh << cssMisc::Indent(indent) << "Local Variables For Program: "
     << nm << " (frame = " << frdx  << ")\n";
  int curpc = PC(frdx);
  cssInst* inst = Inst(curpc);
  if(inst) {
    fh << cssMisc::Indent(indent + 1) << inst->PrintStr() << "\n";
  }
  if(Autos(frdx))
     Autos(frdx)->List(fh, indent+1, 1);
  statics.List(fh, indent+1, 1);
  Stack(frdx)->List(fh, indent+1, 1);
  if(top->debug >= 1)
    literals.List(fh, indent+1, 1);
}

//////////////////////////////////////////
//	cssProg: Coding			//
//////////////////////////////////////////

int cssProg::AddCode(cssInst* it) {
  if(size+1 >= alloc_size)
    AllocInst(size+1);
  insts[size++] = it;
  return size-1;
}

cssElPtr& cssProg::AddAuto(cssEl* it) {
  el_retv = Autos(0)->FindName(it->name);
  if(it->name.empty() || (el_retv == 0))
    el_retv = Autos(0)->Push(it);
  else {
    cssMisc::Warning(this, "Warning: attempt to redefine variable in same scope:",it->name);
    cssEl::Done(it);
  }
  el_retv.SetProgAuto(this);
  return el_retv;
}

cssElPtr& cssProg::AddLiteral(cssEl* it) {
  el_retv = literals.Push(it);
  return el_retv;
}

int cssProg::Code(cssEl* it) {
  if((cssMisc::code_cur_top) && (cssMisc::code_cur_top->Prog() != this))
    return cssMisc::code_cur_top->Prog()->Code(it);
  cssElPtr elp;
  elp.SetDirect(it);
  cssInst* tmp = new cssInst(this, elp, top->src_ln, top->src_col);
  tmp->idx = AddCode(tmp);
  return tmp->idx;
}
int cssProg::Code(cssElPtr& it) {
  if((cssMisc::code_cur_top) && (cssMisc::code_cur_top->Prog() != this))
    return cssMisc::code_cur_top->Prog()->Code(it);
  cssInst* tmp = new cssInst(this, it, top->src_ln, top->src_col);
  tmp->idx = AddCode(tmp);
  return tmp->idx;
}
int cssProg::Code(const String& nm) {
  if((cssMisc::code_cur_top) && (cssMisc::code_cur_top->Prog() != this))
    return cssMisc::code_cur_top->Prog()->Code(nm);
  cssElPtr tmp;
  if((tmp = top->FindName(nm)) == 0)
    cssMisc::Error(this, "Function/Variable Not Found:", nm);
  return Code(tmp);
}
int cssProg::Code(css_progdx it) {
  if((cssMisc::code_cur_top) && (cssMisc::code_cur_top->Prog() != this))
    return cssMisc::code_cur_top->Prog()->Code(it);
  cssIJump* tmp;
  tmp = new cssIJump(this, it, top->src_ln, top->src_col);
  tmp->idx = AddCode(tmp);
  return tmp->idx;
}
int cssProg::Code(cssIJump* it) {
  it->idx = AddCode(it);
  return it->idx;
}
int cssProg::ReplaceCode(int idx, cssEl* it) {
  if(idx >= size) return -1;
  cssInst* old_code = insts[idx];
  cssElPtr elp;  elp.SetDirect(it);
  cssInst* tmp = new cssInst(this, elp);
  tmp->line = old_code->line;
  tmp->col = old_code->col;
  insts[idx] = tmp;
  tmp->idx = idx;
  delete old_code;		// get rid of prev one
  return tmp->idx;
}

void cssProg::ZapFrom(int zp_size) {
  int i;
  for(i=zp_size; i<size; i++)
    delete insts[i];
  size = zp_size;
}

int cssProg::Undo(int srcln) {
  // todo: not currently supported
  cssMisc::Warning(this, "Undo not currently supported, sorry!");
  return -1;
  
//   int srcdx, i, rval = -1;
//   int diff;
//   css_progdx bppc;

//   if((srcdx = FindSrcLn(srcln)) < 0) {
//     for(int i=0; i < size; i++) {
//       cssEl* tmp = insts[i]->inst.El();
//       if(tmp->GetType() == cssEl::T_CodeBlock) {
// 	if((rval = tmp->GetSubProg()->Undo(srcln)) >= 0)
// 	  return rval;
//       }
//     }
//     return -1;
//   }
//   bppc = source[srcdx]->stpc;
//   for(i = bppc; i < size; i++) {
//     if(source[insts[i]->line]->ln > srcln) {
//       break;
//     }
//   }
//   diff = i - bppc;		// amount to delete
//   for(i=bppc; i < size-diff; i++) {
//     delete insts[i];
//     insts[i] = insts[i+diff];
//   }
//   size -= diff;
//   src_size--;
//   for(i=srcdx; i < src_size; i++) {
//     delete source[i];
//     source[i] = source[i+1];
//   }
//   return bppc;
}

cssElPtr& cssProg::FindAutoName(const String& nm) {	// lookup by name
  if((el_retv = Autos(0)->FindName(nm)) != 0)
    return el_retv;
  if((el_retv = statics.FindName(nm)) != 0)
    return el_retv;
  return cssMisc::VoidElPtr;
}

int cssProg::OptimizeCode() {
  int nopt = 0;
  for(int i=0; i < size; i++) {
    cssEl* el = insts[i]->inst.El();
    if(el->GetType() == cssEl::T_CodeBlock) {
      cssCodeBlock* cb = (cssCodeBlock*)el;
      if(cb->CleanDoubleBlock()) nopt++;
      if(cb->code)
	nopt += cb->code->OptimizeCode();
    }
  }
  return nopt;
}


//////////////////////////////////////////
//	cssProg: Execution	 	//
//////////////////////////////////////////

cssProg* cssProg::SetSrcPC(int srcln) {
  int srcdx;

  if((srcdx = FindSrcLn(srcln)) < 0) {
    cssProg* rval;
    for(int i=0; i < size; i++) {
      cssEl* tmp = insts[i]->inst.El();
      if(tmp->GetType() == cssEl::T_CodeBlock) {
	if((rval = tmp->GetSubProg()->SetSrcPC(srcln)))
	  return rval;
      }
    }
    return NULL;
  }
  SetPC(srcdx);
  return this;
}

void cssProg::RunDebugInfo(cssInst* nxt) {
  static int last_src_ln = -1;
  if((top->debug == 0) || (!top->cmd_shell)) return;

  pager_ostream& fh = top->cmd_shell->pgout;
  if(top->debug <= 1) {
    if(nxt->line != last_src_ln) {
      nxt->ListSrc(fh);
      last_src_ln = nxt->line;
    }
  }
  else {
    nxt->ListSrc(fh);
    nxt->ListMachine(fh, top->size-1); // indent
    if(top->debug >= 3) {
      Stack()->List(fh, top->size); // indent
    }
  }
  taMisc::FlushConsole();
}

bool cssProg::IsBreak(css_progdx pcval) {
  int idx = breaks.FindEl(pcval);
  if(idx >= 0) {
    if((top->last_bp_prog == this) && (top->last_bp_pc == pcval)) {
      top->last_bp_prog = NULL;
      top->last_bp_pc = -1;
      return false;		// don't break again!
    }
    if(top->cmd_shell) {
      pager_ostream& fh = top->cmd_shell->pgout;
      fh << "\nStopped on breakpoint: " << idx << " pc: " << pcval << " in prog: "
	 << name << " of: " << top->name << "\n";
      cssInst* nxt = insts[Frame()->pc];
      nxt->ListSrc(fh);
    }
    return true;
  }
  return false;
}

bool cssProg::CheckWatch() {
  for(int i=0; i<top->watchpoints.size; i++) {
    cssWatchPoint* wp = top->watchpoints[i];
    wp->GetAsCurVal();
    if(wp->prv_val != wp->cur_val) {
      if(top->cmd_shell) {
	pager_ostream& fh = top->cmd_shell->pgout;
	fh << "\nStopped on watchpoint: " << i << " " << wp->GetStr() << " in prog: "
	   << name << " of: " << top->name << "\n";
	cssInst* nxt = insts[Frame()->pc-1];
	nxt->ListSrc(fh);
      }
      wp->GetAsPrvVal();
      return true;
    }
  }
  return false;
}

// run and set status, returning value
cssEl* cssProg::Cont() {
  top->run_stat = cssEl::Running;

  if(top->step_mode == 0) {
    while((PC() < size) && (top->run_stat == cssEl::Running)) {
      if((breaks.size > 0) && IsBreak(PC())) {
	top->last_bp_prog = this;
	top->last_bp_pc = PC();
	top->run_stat = cssEl::BreakPoint;
	Program::SetStopReq(Program::SR_BREAKPOINT, top->name + " prog: " + name + " pc: " +
			    String(top->last_bp_pc));
	// stop programs when this guy was stopped
      }
      else if(top->external_stop && !(state & State_NoBreak)) {
	top->run_stat = cssEl::BreakPoint;
      }
      else {
	cssInst* nxt = insts[Frame()->pc++];
#ifdef DEBUG
	if(top->debug > 0) RunDebugInfo(nxt);
#endif
	cssEl::RunStat rval = nxt->Do();
	if((top->watchpoints.size > 0) && CheckWatch()) {
	  top->run_stat = cssEl::BreakPoint;
	  Program::SetStopReq(Program::SR_BREAKPOINT, top->name + " prog: " + name + " pc: " +
			      String(PC()));
	}
	else if(top->run_stat == cssEl::ExecError) // do could have triggered an exec error
	  Program::SetStopReq(Program::SR_ERROR, top->exec_err_msg);
	else
	  top->run_stat = rval;
      }
    }
  }
  else {
    int stc, lim;
    if(top->debug >= 2) {
      stc = 0;
      lim = top->step_mode;
    }
    else {
      if(PC() < size)
	stc = insts[PC()]->line;
      else
	stc = -1;
      lim = stc + top->step_mode;
    }
    while((stc < lim) && (PC() < size) && (top->run_stat == cssEl::Running)) {
      if((breaks.size > 0) && IsBreak(PC())) {
	top->last_bp_prog = this;
	top->last_bp_pc = PC();
	top->run_stat = cssEl::BreakPoint;
	Program::SetStopReq(Program::SR_BREAKPOINT, top->name + " prog: " + name + " pc: " +
			    String(top->last_bp_pc));
      }
      else if(top->external_stop && !(state & State_NoBreak)) {
	top->run_stat = cssEl::BreakPoint;
      }
      else {
	cssInst* nxt = insts[Frame()->pc++];
#ifdef DEBUG
	if(top->debug > 0) RunDebugInfo(nxt);
#endif
	cssEl::RunStat rval = nxt->Do();
	if((top->watchpoints.size > 0) && CheckWatch()) {
	  top->run_stat = cssEl::BreakPoint;
	  Program::SetStopReq(Program::SR_BREAKPOINT, top->name + " prog: " + name + " pc: " +
			      String(PC()));
	}
	else if(top->run_stat == cssEl::ExecError) 
	  Program::SetStopReq(Program::SR_ERROR, top->exec_err_msg);
	else
	  top->run_stat = rval;
	if(top->debug < 2)
	  stc = nxt->line;
	else
	  stc++;
      }
    }
    top->step_mode = 0;		// always temporary
  }

  if(cssMisc::proc_events_timer.elapsed() > taMisc::css_gui_event_interval) {
    taiM->RunPending();
    cssMisc::proc_events_timer.restart();
  }

  if(top->run_stat == cssEl::Running)
    top->run_stat = cssEl::Stopping;
  return Stack()->Peek();
}

cssEl* cssProg::ContSrc(int srcln) {
  cssProg* cp = SetSrcPC(srcln);
  if(!cp)
    return &cssMisc::Void;

  return cp->Cont();
}

void cssProg::Restart() {
  SetPC(0);
  Stack()->Reset();
  ResetLasts();
}

void cssProg::SaveStack() {
  if(saved_stack.size != 0) return;
  cssSpace* stk = Stack();
  int i;
  for(i=0; i<stk->size; i++)
    saved_stack.Push(stk->FastEl(i));
  stk->Reset();
}

void cssProg::ReloadStack() {
  cssSpace* stk = Stack();
  stk->Reset();
  int i;
  for(i=0; i<saved_stack.size; i++)
    stk->Push(saved_stack.FastEl(i));
  saved_stack.Reset();
}


//////////////////////////////////////////
//	cssProg: Breakpoints	 	//
//////////////////////////////////////////

// scans through subroutines embedded within
bool cssProg::SetBreak(int srcln) {
  int srcdx;
  if((srcdx = FindSrcLn(srcln)) < 0) {
    for(int i=0; i < size; i++) {
      cssEl* tmp = insts[i]->inst.El();
      if(tmp->GetType() == cssEl::T_CodeBlock) {
	if(tmp->GetSubProg()->SetBreak(srcln))
	  return true;
      }
    }
    return false;
  }
  breaks.Add(srcdx);
  return true;
}

void cssProg::ShowBreaks(ostream& fh) {
  if(breaks.size > 0) {
    fh << "prog: " << name << endl;
    for(int i=0; i<breaks.size; i++) {
      fh << "breakpoint: " << i << "\t" << insts[breaks[i]]->PrintStr() << endl;
    }
  }
  for(int i=0; i < size; i++) {
    cssEl* tmp = insts[i]->inst.El();
    if(tmp->GetType() == cssEl::T_CodeBlock) {
      tmp->GetSubProg()->ShowBreaks(fh);
    }
  }
}

bool cssProg::DelBreak(int srcln) {
  int srcdx;
  if((srcdx = FindSrcLn(srcln)) < 0) {
    int i;
    for(i=0; i < size; i++) {
      cssEl* tmp = insts[i]->inst.El();
      if(tmp->GetType() == cssEl::T_CodeBlock) {
	if(tmp->GetSubProg()->DelBreak(srcln))
	  return true;
      }
    }
    return false;
  }
  return breaks.RemoveEl(srcdx);
}


//////////////////////////////////////////
//	cssProg: Watchpoints	 	//
//////////////////////////////////////////

cssWatchPoint::cssWatchPoint() {
  watch = NULL;
}

cssWatchPoint::~cssWatchPoint() {
  if(watch)
    cssEl::unRefDone(watch);
  watch = NULL;
}

void cssWatchPoint::SetWatch(cssEl* wp) {
  cssEl::SetRefPointer(&watch, wp); 
  GetAsPrvVal();
}

String cssWatchPoint::GetStr() {	
  if(!watch) return "NULL";
  return watch->PrintStr() + " prv_val: " + prv_val + " cur_val: " + cur_val;
}

// scans through subroutines embedded within
bool cssProg::SetWatch(cssEl* watch) {
  return top->SetWatch(watch);
}

bool cssProg::DelWatch(cssEl* watch) {
  return top->DelWatch(watch);
}

bool cssProg::DelWatchIdx(int idx) {
  return top->DelWatchIdx(idx);
}

void cssProg::ShowWatchpoints(ostream& fh) {
  top->ShowWatchpoints();
}

//////////////////////////////////////////////////
// 	cssProgSpace: Space of Programs		//
//////////////////////////////////////////////////

void cssProgSpace::Constr() {
  old_debug = 0;
  compile_ctrl = CC_None;
  cc_push_this = NULL;

  size = 0;
  progs = (cssProgStack**)calloc(alloc_size, sizeof(cssProgStack*));
  state = 0;
  parse_depth = 0;
  step_mode = 0;
  run_stat = cssEl::Waiting;
  if(cssMisc::Top)
    debug = cssMisc::Top->debug;
  else
    debug = 0;

  src_fin = NULL;
  cur_fnm_lno = 0;

  src_ln = 0;
  src_col = 0;
  src_pos = 0;
  list_ln = 0;
  list_n = 20;

  tok_src_ln = 0;
  tok_src_col = 0;

  parsing_command = false;
  parse_path_expr = false;
  external_stop = false;

  ext_parse_fun_pre = NULL;
  ext_parse_fun_post = NULL;
  ext_parse_user_data = NULL;

  cmd_shell = NULL;

  SetName(name);
  AddProg(new cssProg("Top Level"));
  Prog()->top = this;		// this one gets us as a top for sure
  Restart();
}

cssProgSpace::cssProgSpace() {
  alloc_size = 2;
  Constr();
}

cssProgSpace::cssProgSpace(const String& nm) {
  alloc_size = 2;
  name = nm;
  Constr();
}

cssProgSpace::~cssProgSpace() {
  Reset();
  DelProg();
  free(progs);
}

bool cssProgSpace::AmCmdProg() {
  if(!cmd_shell) return false;
  if(cmd_shell->cmd_prog == this) return true;
  return false;
}

bool cssProgSpace::HaveCmdShell() {
  if(cmd_shell) return true;
  cerr << "css program: " << name << " Warning: trying to access shell-level functionality but no shell is present!" << endl;
  return false;
}

cssProgSpace* cssProgSpace::GetSrcProg() {
  if(!AmCmdProg()) return NULL;
  return cmd_shell->src_prog;
}

//////////////////////////////////////////////////
// 	cssProgSpace: Internal, Programs	//
//////////////////////////////////////////////////

void cssProgSpace::SetName(const String& nm) {
  name = nm;
  prog_vars.name = name + ".prog_vars";
  hard_vars.name = name + ".hard_vars";
  hard_funs.name = name + ".hard_funs";
  enums.name = name + ".enums";
  statics.name = name + ".statics";
  types.name = name + ".types";
}

void cssProgSpace::Reset() {
#ifdef TA_GUI
  cssiSession::CancelProgEdits(this);
#endif // TA_GUI
  parse_depth = 0;
  Restart();
  Prog()->Reset();		// reset our top-level guy
  Prog()->top = this;		// this one gets us as a top for sure
  src_list.Reset();
  src_list_fnm.Reset();
  src_list_lno.Reset();
  src_ln = 0;
  src_col = 0;
  src_pos = 0;
  list_ln = 0;
  cur_fnm = name;
  cur_fnm_lno = 0;
  tok_src_ln = 0;
  tok_src_col = 0;
  //  ResetParseFlags(); // can't call this here because it calls a Reset() of const expr!!!
  parsing_command = false;
  parse_path_expr = false;
  external_stop = false;
  run_stat = cssEl::Waiting;
}

void cssProgSpace::ClearAll() {
#ifdef TA_GUI
  cssiSession::CancelProgEdits(this);
#endif // TA_GUI
  Reset();
  statics.Reset();
  enums.Reset();
  types.Reset();
}

void cssProgSpace::AllocProg(int sz) {
  if(alloc_size >= sz)	return;	// no need to increase..
  sz = MAX(16,sz);		// once allocating, use a minimum of 16
  alloc_size += TA_ALLOC_OVERHEAD; // increment to full power of 2
  while((alloc_size-TA_ALLOC_OVERHEAD) <= sz) alloc_size <<= 1;
  alloc_size -= TA_ALLOC_OVERHEAD;
  progs = (cssProgStack**)realloc(progs, alloc_size * sizeof(cssProgStack*));
}

void cssProgSpace::AddProg(cssProg* it) {
  if(size+1 >= alloc_size)
    AllocProg(size+1);
  cssProgStack* prg_stk = new cssProgStack;
  progs[size++] = prg_stk;
  prg_stk->prog = it;
  cssProg::Ref(it);
  prg_stk->fr_no = it->fr_size-1;
  it->Restart();
}

void cssProgSpace::DelProg() {
  if(size <= 0)
    return;
  cssProg* tmp = Prog();
  delete progs[size-1];
  size--;
  cssProg::unRefDone(tmp);
}

cssProg* cssProgSpace::PopProg() {
  if(size <= 0)
    return NULL;
  cssProg* tmp = Prog();
  delete progs[size-1];
  size--;
  cssProg::unRef(tmp);
  return tmp;
}

// this is for coding, setting of top occurs here
void cssProgSpace::Push(cssProg* it) {
  AddProg(it);
  it->top = this;		// set top to be this
  parse_depth++;
  if(debug >= 2) {
    cerr << "Pushed new prog: " << it->name << " depth: " << parse_depth << endl;
  }
}

// this is for running, setting of top occurs in calling context
void cssProgSpace::Shove(cssProg* it) {
  it->AddFrame();
  AddProg(it);
}

// this is for coding, unsetting of top occurs here
cssProg* cssProgSpace::Pop() {
  if(size > 1) {
    cssProg* prv = PopProg();
    prv->top = cssMisc::Top;
    parse_depth--;
    if(debug >= 2) {
      cerr << "Popped prog: " << prv->name << " depth: " << parse_depth << endl;
    }
    return prv;
  }
  cssMisc::Warning(Prog(), "Attempt to end Top Level definition with } ");
  return 0;
}

// this is for running, resetting of top occurs in calling context
cssProg* cssProgSpace::Pull() {
  if(size > 1) {
    Prog()->DelFrame();		// remove this frame
    return PopProg();		// leave
  }
  return 0;
}

cssScriptFun* cssProgSpace::GetCurrentFun() {
  int i;
  for(i = size-1; i>=0; i--) {
    cssProg* pg = Prog(i);
    if(!pg->owner_fun)
      continue;
    return pg->owner_fun;
  }
  return NULL;
}

//////////////////////////////////////////////////
// 	cssProgSpace: Internal, Source  	//
//////////////////////////////////////////////////

int cssProgSpace::AddSrcList(const String& nw_lst) {
  src_ln++;
  cur_fnm_lno++;
  src_list.SetSize(src_ln+1);
  src_list[src_ln] = nw_lst;
  src_list_fnm.SetSize(src_ln+1);
  src_list_lno.SetSize(src_ln+1);
  src_list_fnm[src_ln] = cur_fnm;
  src_list_lno[src_ln] = cur_fnm_lno;

  if(Prog()->first_src_ln < 0) {
    Prog()->first_src_ln = src_ln;
    Prog()->last_src_ln = src_ln;
  }
  else {
    Prog()->first_src_ln = MIN(Prog()->first_src_ln, src_ln);
    Prog()->last_src_ln = MAX(Prog()->last_src_ln, src_ln);
  }
  return src_ln;
}

int cssProgSpace::Getc() {
  while(true) {
    if(src_ln == 0) {
      if(ReadLn(src_fin) == EOF)
	return EOF;
    }
    String& src = src_list[src_ln];
    if(src_col < src.length()) {
      src_pos++;
      return (int) (src[src_col++]);
    }
    if(src_ln < src_list.size-1) { // could have been ungot back up a line, now getting again
      src_ln++; continue;
    }
    if(ReadLn(src_fin) == EOF)
      return EOF;
  }
}

int cssProgSpace::unGetc() {
  src_pos--;
  src_col--;
  if(src_col < 0) {
    while(true) {
      src_ln--;
      if(src_ln < 0) {
	src_col = 0;
	src_pos = 0;
	src_ln = 0; 
	return EOF;
      }
      if(src_ln < src_list.size) {
	String& src = src_list[src_ln];
	src_col = src.length()-1;
	if(src_col <0) continue;
	return 1;
      }
    }
  }
  return 1;
}

int cssProgSpace::CurSrcCharsLeft() {
  if(src_ln == 0 || src_ln >= src_list.size) return 0;
  return src_list[src_ln].length() - src_col;
}

int cssProgSpace::ReadLn(istream* fh) {
  if(!fh) return EOF;
  int c;
  AddSrcList();			// new blank line
  String& src = src_list[src_ln];
  while (((c = fh->get()) != EOF) && (c != '\n')) {
    src += (char)c;	// add
  }
  if(c == EOF)
    return EOF;
  
  src += '\n';	// always end with a newline
  if(debug >= 3) {
    cerr << "\nsrc ===> " << src;	// get a source code trace here..
    taMisc::FlushConsole();
  }
  src_col = 0;

  return 1;
}

void cssProgSpace::StoreCurTokSrcPos() {
  tok_src_col = src_col-1;
  tok_src_ln = src_ln;
}

//////////////////////////////////////////////////
// 	cssProgSpace: Source Access		//
//////////////////////////////////////////////////

String cssProgSpace::GetSrcLnCol(int ln, int cl) const {
  String rval = "\n";
  if(ln < src_list.size) {
    rval = src_list[ln];
    if(cl > 0)
      rval = rval.from(cl);
  }
  return rval;
}

String cssProgSpace::GetSrcLn(int ln) const {
  String rval = "\n";
  if(ln >= src_list.size) return rval;
  rval = taMisc::LeadingZeros(ln, 6) + "\t" + taMisc::LeadingZeros(src_list_lno[ln], 6)
    + "\t" + src_list[ln];
  if(rval.lastchar() != '\n')
    rval += "\n";
  return rval;
}

String cssProgSpace::GetFullSrcLn(int ln) const {
  String rval = "\n";
  if(ln >= src_list.size) return rval;

  rval = src_list_fnm[ln] + " line: " + taMisc::LeadingZeros(src_list_lno[ln], 6);
  rval += "\n" + src_list[ln];
  return rval;
}

String cssProgSpace::CurParseSrc() const {
  return GetSrcLnCol(src_ln, src_col);
}

String cssProgSpace::CurTokSrc() const {
  return GetSrcLnCol(tok_src_ln, tok_src_col);
}

String cssProgSpace::CurFullTokSrc() const {
  return GetFullSrcLn(tok_src_ln);
}

String cssProgSpace::CurFullRunSrc() const {
  return GetFullSrcLn(Prog()->CurSrcLn());
}

String cssProgSpace::GetSrcListFnm(int i) const {
  String rval = src_list_fnm[i];
  if(rval.empty())
    rval = name;
  return rval;
}

void cssProgSpace::ListMachine_impl(pager_ostream& fh, int ln) const {
  for(int i=0;i<types.size;i++) {
    cssEl* el = types[i];
    if(el->GetType() == cssEl::T_ClassType) {
      cssClassType* cl = (cssClassType*)el->GetNonRefObj();
      for(int j=0;j<cl->methods->size;j++) {
	cssProg* fun = cl->methods->FastEl(j)->GetSubProg();
	if(fun) fun->ListMachine(fh, 1, ln);
      }
    }
  }

  for(int i=0;i<statics.size;i++) {
    cssEl* el = statics[i];
    if(el->HasSubProg() && (el->GetType() != cssEl::T_CodeBlock)) {
      el->GetSubProg()->ListMachine(fh, 1, ln);
    }
  }
  Prog(0)->ListMachine(fh, 1, ln);
}

void cssProgSpace::ListSrc_impl(pager_ostream& fh, int stln) const {
  int st = 1;
  if(stln >= 0)
    st = stln;
  String curf;
  int ln;
  int mx_ln = MIN(src_list.size, st + list_n);
  for(ln=st; ln<mx_ln; ln++) {
    String fnm = GetSrcListFnm(ln);
    if(curf != fnm) {
      curf = fnm;
      fh << "file: " << curf << "\n";    
    }
    fh << GetSrcLn(ln);
    taMisc::FlushConsole();
    if(ListDebug() >= 2) {
      ListMachine_impl(fh, ln);
    }
  }
  if(mx_ln == src_list.size)
    ((cssProgSpace*)this)->list_ln = 0;
  else
    ((cssProgSpace*)this)->list_ln = ln;
}

//////////////////////////////////////////////////
// 	cssProgSpace: Internal, Variables	//
//////////////////////////////////////////////////

cssElPtr& cssProgSpace::AddLiteral(String& str) {
  if((el_retv = Prog()->FindLiteral(str)) == 0)
    el_retv = Prog()->AddLiteral(new cssString((char*)str));
  if(debug == 0) {
    cssEl* pt = el_retv.El();
    el_retv.SetDirect(pt);
  }
  return el_retv;
}
cssElPtr& cssProgSpace::AddLiteral(int itm) {
  if((el_retv = Prog()->FindLiteral(itm)) == 0)
    el_retv = Prog()->AddLiteral(new cssInt(itm));
  if(debug == 0) {
    cssEl* pt = el_retv.El();
    el_retv.SetDirect(pt);
  }
  return el_retv;
}
cssElPtr& cssProgSpace::AddLiteral(Real itm) {
  if((el_retv = Prog()->FindLiteral(itm)) == 0)
    el_retv = Prog()->AddLiteral(new cssReal(itm));
  if(debug == 0) {
    cssEl* pt = el_retv.El();
    el_retv.SetDirect(pt);
  }
  return el_retv;
}
cssElPtr& cssProgSpace::AddVar(cssEl* it) {
  if(size > 1) return Prog()->AddAuto(it);
  cssElPtr& anel_retv = statics.FindName(it->name);
  if(it->name.empty() || (anel_retv == 0))
    return statics.Push(it);
  else {
    cssMisc::Warning(Prog(), "Warning: attempt to redefine variable in same scope:",it->name);
    cssEl::Done(it);
  }
  return anel_retv;
}
cssElPtr& cssProgSpace::AddStatic(cssEl* it) {
  if(size > 1) return Prog()->statics.Push(it);
  return statics.Push(it);
}

cssElPtr& cssProgSpace::AddStaticVar(cssEl* it) {
  cssSpace* spc;
  if(size > 1) spc = &(Prog()->statics);
  else spc = &statics;
  cssElPtr& anel_retv = spc->FindName(it->name);
  if(it->name.empty() || (anel_retv == 0))
    return spc->Push(it);
  else {
    cssMisc::Warning(Prog(), "Warning: attempt to redefine static variable in same scope:",it->name);
    cssEl::Done(it);
  }
  return anel_retv;
}
bool cssProgSpace::ReplaceVar(cssEl* old, cssEl* nw) {
  if(size > 1) {
    if(Prog()->Autos(0)->Replace(old, nw))  return true;
    if(Prog()->statics.Replace(old, nw))   return true;
  }
  if(statics.Replace(old, nw))	return true;
  if(cssMisc::Constants.Replace(old, nw)) return true;
  if(cssMisc::Externs.Replace(old, nw)) return true;
  return false;
}
bool cssProgSpace::RemoveVar(cssEl* old) {
  if(size > 1) {
    if(Prog()->Autos(0)->Remove(old))	return true;
    if(Prog()->statics.Remove(old))	return true;
  }
  if(statics.Remove(old))	return true;
  if(cssMisc::Constants.Remove(old)) return true;
  if(cssMisc::Externs.Remove(old)) return true;
  return false;
}

cssElPtr& cssProgSpace::FindName(const String& nm) {	// lookup by name
  int i;
  for(i=size-1; i>= 0; i--) {
    if((el_retv = Prog(i)->FindAutoName(nm)) != 0)
      return el_retv;
  }
  if ((el_retv = statics.FindName(nm)) != 0)
    return el_retv;
  if ((el_retv = prog_vars.FindName(nm)) != 0)
    return el_retv;
  if ((el_retv = hard_funs.FindName(nm)) != 0)
    return el_retv;
  if ((el_retv = hard_vars.FindName(nm)) != 0)
    return el_retv;
  return enums.FindName(nm);
}

// idx is an arbitrary number, returns NULL when no more values
cssSpace* cssProgSpace::GetParseSpace(int idx) {
  static int n_above;		// number of guys above me

  int after_class = 2;
  int dynamics = after_class + (2 * n_above) + 2; // number of "dynamic" spaces (changes based on size)
  if(cssMisc::cur_class) { // two more spaces to check..
    dynamics += 2;
    after_class += 2;
  }

//   cerr << "idx: " << idx << " dynamics: " << dynamics << " after class: " << after_class << endl;

  if(idx == 0)
    return &cssMisc::Defines; 	// #define is uber alles
  else if(idx == 1)
    return &cssMisc::Parse;	// first parsed stuff
  else if((cssMisc::cur_class) && (idx == 2))
    return cssMisc::cur_class->members;
  else if((cssMisc::cur_class) && (idx == 3))
    return cssMisc::cur_class->methods;
  else if(idx == after_class) {		// first auto block
    n_above = 0;
    cssProg* cp = Prog();
    while((cp->owner_blk) && (cp->owner_blk->owner_prog)) {
      n_above++;
      cp = cp->owner_blk->owner_prog;
    }
    if((n_above > 0) && (cp->owner_fun))
      n_above++;
    return Prog()->Autos(0);
  }
  else if(idx == after_class+1)
    return &(Prog()->statics);
  else if((idx >= after_class+2) && (idx < dynamics)) {	// 0 - size-1 for progs, 2 per
    int stat_auto = (idx - (after_class+2)) % 2;		// 0 is auto, 1 is stat
    int prog_idx = ((idx - (after_class+2)) / 2) + 1;	// index of prog (from end)
    int cur_idx = 0;
    cssProg* cp = Prog();
    while((cp->owner_blk) && (cp->owner_blk->owner_prog)) {
      cur_idx++;
      cp = cp->owner_blk->owner_prog;
      if(cur_idx == prog_idx) break;
    }
    if(stat_auto)
      return cp->Autos(0);
    else
      return &(cp->statics);
  }
  else if(idx == dynamics)
    return &statics;
  else if(idx == dynamics+1)
    return &prog_vars;
  else if(idx == dynamics+2)
    return &hard_funs;
  else if(idx == dynamics+3)
    return &hard_vars;
  else if(idx == dynamics+4)
    return &enums;
  else if(idx == dynamics+5)	// global variables
    return &cssMisc::Externs;
  else if(idx == dynamics+6)
    return &cssMisc::HardFuns;
  else if(idx == dynamics+7)
    return &cssMisc::HardVars;
  else if(idx == dynamics+8)
    return &cssMisc::Commands;
  else if(idx == dynamics+9)
    return &cssMisc::Functions;
  else if(idx == dynamics+10)
    return &cssMisc::Constants;
  else if(idx == dynamics+11)
    return &cssMisc::Enums;
  else if(idx == dynamics+12)
    return &cssMisc::Settings;
  else
    return NULL;
}

cssElPtr& cssProgSpace::ParseName(const String& nm) {
//   cerr << "searching for: " << nm << endl;
  int i = 0;
  cssSpace* spc=NULL;
  while((spc = GetParseSpace(i++))) {
    if((spc == &(cssMisc::Commands) && !AmCmdProg())) // don't process commands unless I'm a command prog!
      continue;
    if((el_retv = spc->FindName(nm)) != 0) {
      if(spc == &(cssMisc::Commands))
	parsing_command = true;
      else if((cssMisc::cur_class) && (spc == cssMisc::cur_class->methods)
	      && (spc->FastEl(el_retv.dx)->GetType() == cssEl::T_MbrScriptFun)) {
	if(((cssMbrScriptFun*)spc->FastEl(el_retv.dx))->is_virtual)
	  el_retv.SetVirtMethod(cssMisc::cur_class); // make relative if virtual..
      }
      // todo: disabled..
      // convert space refs to direct for faster execution ("optimized")
//        if((debug == 0) && (el_retv.ptr_type == cssElPtr::SPACE) &&
//  	 (spc != &cssMisc::Constants)) {
//  	cssEl* pt = el_retv.El();
//  	el_retv.SetDirect(pt);
	//      }
      return el_retv;
    }
  }
  return cssMisc::VoidElPtr;
}

cssElPtr& cssProgSpace::FindTypeName(const String& nm) {
  cssElPtr& tp_ptr = types.FindName(nm);
  if(tp_ptr != 0)
    return tp_ptr;
  return cssMisc::TypesSpace.FindName(nm);
}

cssElPtr& cssProgSpace::GetPtrType(cssEl* base_type, int ptrs) {
  // first try to find pre-existing on appropriate type space
  cssSpace* tp_spc = &cssMisc::TypesSpace_ptrs;
  String sufx = "_ptr";
  if(ptrs >= 2) sufx += "_ptr";

  if((cssMisc::cur_class) &&
     (cssMisc::cur_class->types->GetIndex(base_type) >= 0))
    tp_spc = cssMisc::cur_class->types;

  String nm = base_type->name + sufx;
  cssElPtr& tp_ptr = tp_spc->FindName(nm);
  if(tp_ptr) 
    return tp_ptr;
  cssEl* ptr = base_type->MakePtrType(ptrs);
  el_retv = tp_spc->Push(ptr);
  if(debug == 0)
    el_retv.SetDirect(ptr);
  return el_retv;
}

cssElPtr& cssProgSpace::GetRefType(cssEl* base_type) {
  // first try to find pre-existing on appropriate type space
  cssSpace* tp_spc = &cssMisc::TypesSpace_refs;
  String sufx = "_ref";

  if((cssMisc::cur_class) &&
     (cssMisc::cur_class->types->GetIndex(base_type) >= 0))
    tp_spc = cssMisc::cur_class->types;

  String nm = base_type->name + sufx;
  cssElPtr& tp_ptr = tp_spc->FindName(nm);
  if(tp_ptr) 
    return tp_ptr;

  cssEl* ref = base_type->MakeRefType();
  el_retv = tp_spc->Push(ref);
  if(debug == 0)
    el_retv.SetDirect(ref);
  return el_retv;
}

//////////////////////////////////////////////////
// 	cssProgSpace: Compiling			//
//////////////////////////////////////////////////

int cssProgSpace::GetFile(fstream& fh, const String& fname) {
  fh.open(fname, ios::in);
  if(fh.good())
    return true;
  fh.close(); fh.clear();

  String extnm = String(fname) + ".css";
  fh.open(extnm, ios::in);
  if(fh.good())
    return true;
  fh.close(); fh.clear();

  int i;
  for(i=0; i<taMisc::css_include_paths.size; i++) {
    String trynm = taMisc::css_include_paths.FastEl(i) + "/" + fname;
    fh.open(trynm, ios::in);
    if(fh.good())
      return true;
    fh.close(); fh.clear();

    trynm = taMisc::css_include_paths.FastEl(i) + "/" + fname + ".css";
    fh.open(trynm, ios::in);
    if(fh.good())
      return true;
    fh.close(); fh.clear();
  }
  fh.close(); fh.clear();
  return false;
}

// this compiles one line of code, does not allow for run-last type shell execution
int cssProgSpace::CompileLn(istream& fh, bool* err) {
  cssProg* parse_prog = Prog();
  int parse_prog_sz = Prog()->size;

  int retval = cssProg::YY_Ok;
  do {
    ResetParseFlags();
    retval = yyparse();		// parse current line
  } while(retval == cssProg::YY_Parse);

  if (retval == cssProg::YY_Err) { // remove code associated with errors
    if (err) *err = true;
    parse_prog->ZapFrom(parse_prog_sz);
  }
  else {
    DoCompileCtrl();		// do any compile control things necessary
  }

  return retval;
}

bool cssProgSpace::Compile(istream& fh) {
  istream* old_fh = src_fin;
  src_fin = &fh;

  bool err = false;
  cssProgSpace* old_top = cssMisc::SetCurTop(this);
  int old_state = state;

  while (CompileLn(fh, &err) != cssProg::YY_Exit);

  state = old_state;
  src_fin = old_fh;		// this is key for include -- not sure why it was removed..

  OptimizeCode();

  cssMisc::PopCurTop(old_top);
  return !err;
}

bool cssProgSpace::Compile(const String& fname) {
  bool rval = false;
  String fnm = fname;
  if (fnm == "-") {
    rval = Compile(cin);
  }  else {
    fstream fh;
    if(!GetFile(fh, fname)) {
      if(!((fnm.length() > 1) && (fnm[0] == '.') && (!fnm.contains('/')))) {
	cssMisc::Warning(Prog(), "File Not Found:",fname); // don't complain about . files..
      }
      return rval;
    }
    // make sure directory is in include path
    String dir = taPlatform::getFilePath(fnm);
    if(!dir.empty())
      taMisc::css_include_paths.AddUnique(dir);
    SetName(fname);
    cur_fnm = fname;
    cur_fnm_lno = 0;
    rval = Compile(fh);
    fh.close(); fh.clear();
  }
  return rval;
}

bool cssProgSpace::CompileCode(const String& code) {
  stringstream fh;
  fh << code << endl;
  fh.seekg(0, ios::beg);
  return Compile(fh);
}

void cssProgSpace::Include(const String& fname) {
  String save_name = name;
  String save_fnm = cur_fnm;
  int save_lno = cur_fnm_lno;

  Compile(fname);

  SetName(save_name);
  cur_fnm = save_fnm;
  cur_fnm_lno = save_lno;
}

void cssProgSpace::CompileRunClear(const String& fname) {
  String old_nm = name;
  Compile(fname);
  Run();
  ClearAll();
  SetName(old_nm);
}

void cssProgSpace::reCompile() {
  ClearAll();
  Compile(name);
}

void cssProgSpace::Undo(int st) {
  if(Prog(0)->Undo(st) < 0)
    yyerror("Source line not found");
}

void cssProgSpace::ResetParseFlags() {
  parse_path_expr = false;
  parsing_command = false;
  cssMisc::CodeTop();
  cssMisc::ConstExprTop->Reset();
  cssMisc::parsing_args = false;
  cssMisc::parsing_membdefn = false;
  cssMisc::cur_scope = NULL;
}

bool cssProgSpace::DoCompileCtrl() {
  switch(compile_ctrl) {
  case CC_None:
    return false;
  case CC_Pop:
    ClearCompileCtrl();
    Pop();
    break;
  case CC_Push:
    ClearCompileCtrl();
    Push(cc_push_this);
    break;
  case CC_Include:
    ClearCompileCtrl();
    Include(cc_include_this);
    break;
  }
  return true;			// something happened
}

bool cssProgSpace::ParseElseCheck() {
  int c;
  bool got_else = false;
  while (isspace(c=Getc())); // skip space
  if(c == 'e') {
    if(Getc() == 'l') {
      if(Getc() == 's') {
	if(Getc() == 'e') {
	  got_else = true;
	  unGetc(); unGetc();
	  unGetc(); unGetc();
	}
	else {
	  unGetc(); unGetc();
	  unGetc(); unGetc();
	}
      }
      else {
	unGetc(); unGetc(); unGetc();
      }
    }
    else {
      unGetc(); unGetc();
    }
  }
  else {
    unGetc();
  }
  return got_else;
}

bool cssProgSpace::PopElseBlocks() {
  bool popped = false;
  while((Prog()->owner_blk != NULL) &&
	(Prog()->owner_blk->action == cssCodeBlock::ELSE)) {
    Pop();
    popped = true;
  }
  return popped;
}

int cssProgSpace::OptimizeCode() {
  int nopt = 0;
  for(int i=0;i<types.size;i++) {
    cssEl* el = types[i];
    if(el->GetType() == cssEl::T_ClassType) {
      cssClassType* cl = (cssClassType*)el->GetNonRefObj();
      for(int j=0;j<cl->methods->size;j++) {
	cssProg* fun = cl->methods->FastEl(j)->GetSubProg();
	if(fun) {
	  nopt += fun->OptimizeCode();
	}
      }
    }
  }

  for(int i=0;i<statics.size;i++) {
    cssEl* el = statics[i];
    if(el->HasSubProg() && (el->GetType() != cssEl::T_CodeBlock)) {
      nopt += el->GetSubProg()->OptimizeCode();
    }
  }
  nopt += Prog(0)->OptimizeCode();
//   if(nopt > 0)
//     cerr << name << " optimized: " << nopt << " times" << endl;
  return nopt;
}


//////////////////////////////////////////////////
// 	cssProgSpace:    Execution 		//
//////////////////////////////////////////////////

void cssProgSpace::Restart() {
  while(size > 1) DelProg();
  list_ln = 0;
  Prog()->Restart();
}

bool cssProgSpace::ContinueLoop() {
  while(size > 1) {
    cssProg* prg = Prog();
    if(prg->owner_fun)
      return false;
    if(!prg->owner_blk) {
      Pull();
      continue;
    }
    cssCodeBlock* blk = prg->owner_blk;
    if(blk->loop_type == cssCodeBlock::NOT_LOOP) {
      Pull();
      continue;
    }
    if(blk->loop_type == cssCodeBlock::WHILE) {
      Prog()->SetPC(Prog()->size);
      return true;
    }
    if(blk->loop_type == cssCodeBlock::DO) {
      Prog()->SetPC(0);
      return true;
    }
    if(blk->loop_type == cssCodeBlock::FOR) {
      Prog()->SetPC(Prog()->size);
      return true;
    }
  }
  return false;
}

bool cssProgSpace::BreakLoop() {
  while(size > 1) {
    cssProg* prg = Prog();
    if(prg->owner_fun)
      return false;
    if(!prg->owner_blk) {
      Pull();
      continue;
    }
    cssCodeBlock* blk = prg->owner_blk;
    if(blk->loop_type == cssCodeBlock::NOT_LOOP) {
      Pull();
      continue;
    }
    if(blk->loop_type == cssCodeBlock::WHILE) {
      Pull();			// pull out of this level
      EndRunPop();		// need to do this??
      return true;
    }
    if(blk->loop_type == cssCodeBlock::DO) {
      Pull();			// pull out of entire surrounding do-loop 
      return true;
    }
    if(blk->loop_type == cssCodeBlock::FOR) {
      Pull();
      Pull();			// pull all the way out of entire for loop
      return true;
    }
    if(blk->loop_type == cssCodeBlock::SWITCH) {
      Pull();			// pull out of entire surrounding switch block
      return true;
    }
  }
  return false;
}

bool cssProgSpace::ReturnFun() {
  while(size > 1) {
    cssProg* prg = Prog();
    if(prg->owner_fun)
      return true;
    Pull();
  }
  return false;
}

cssEl* cssProgSpace::Cont() {
  cssProgSpace* old_top = cssMisc::SetCurTop(this);

  state |= cssProg::State_Run;
  external_stop = false;

//   if(Prog()->PC() < Prog()->size) {
//     cssInst* nxt = Prog()->insts[Prog()->PC()];
//     cerr << name << " starting at: " << Prog()->name << " pc: " << Prog()->PC()
// 	 << " nxt: " << nxt->inst.El()->name << endl;
//   }

  cssEl* rval;
  do {
    run_stat = cssEl::Running;
    rval = Prog()->Cont();
    if(((run_stat == cssEl::Stopping) || (run_stat == cssEl::Returning)) && (size > 1)) {
      if(run_stat == cssEl::Returning) {
	if(!ReturnFun()) {
	  cssMisc::Error(NULL, "Error: return not within a function!");
	}
      }
      cssProg* prv_prg = Prog(size-2);
      // should be fun or code block that shoved current prog
      cssEl* fun_el = prv_prg->insts[prv_prg->PC()-1]->inst.El();
      if(!fun_el->HasSubProg()) {
	cssMisc::Warning(NULL, "Internal error: Function or code block ended without finding proper start of block!");
	Pull();
	run_stat = cssEl::BreakPoint; // todo: should have error code instead?
      }
      else {
	if((debug >= 2) && (cmd_shell)) {
	  cmd_shell->pgout << cssMisc::Indent(size-1) << "FunDone  at "
			   << taMisc::LeadingZeros(prv_prg->PC()-1,4)
			   << " el: " << fun_el->PrintStr() << "\n";
	}
	fun_el->FunDone(prv_prg);	// note passing prg from higher level
	if(prv_prg->state & cssProg::State_IsTmpProg) {
	  break;
	}
	else {
	  run_stat = cssEl::NewProgShoved;
	}
      }
    }
    if(run_stat == cssEl::Continuing) {
      if(ContinueLoop()) {
	run_stat = cssEl::NewProgShoved;
      }
      else {
	cssMisc::Warning(NULL, "Error: continue without surrounding loop!");
	run_stat = cssEl::ExecError;
	exec_err_msg = cssMisc::last_warn_msg;
      }
    }
    if(run_stat == cssEl::Breaking) {
      if(BreakLoop()) {
	run_stat = cssEl::NewProgShoved;
      }
      else {
	cssMisc::Warning(NULL, "Error: break without surrounding loop!");
	run_stat = cssEl::ExecError;
	exec_err_msg = cssMisc::last_warn_msg;
      }
    }
  } while(run_stat == cssEl::NewProgShoved);

  state &= ~cssProg::State_Run;
  cssMisc::PopCurTop(old_top);
  return rval;
}

cssEl* cssProgSpace::Cont(css_progdx st) {
  Prog()->SetPC(st);
  return Cont();
}

cssEl* cssProgSpace::ContSrc(int srcln) {
  cssProg* cp = Prog()->SetSrcPC(srcln);
  if(!cp)
    return &cssMisc::Void;
  if(cp != Prog())
    Shove(cp);
  return Cont();
}

cssEl* cssProgSpace::Run() {
  cssProgSpace* old_top = cssMisc::SetCurTop(this);
  state |= cssProg::State_Run;
  Restart();
  cssEl* rval = Cont();
  state &= ~cssProg::State_Run;
  cssMisc::PopCurTop(old_top);
  return rval;
}

void cssProgSpace::Stop() {
  external_stop = true;
}

//////////////////////////////////////////////////
// 	cssProgSpace:    Display, Status	//
//////////////////////////////////////////////////

extern int yydebug;

void cssProgSpace::SetDebug(int dblev) {
  debug = dblev;
  old_debug = dblev;
#ifdef CSS_DEBUG_REGISTER
  cssEl_alloc_debug = (dblev > 2);
  cssEl_alloc_count = 0;
  cssEl_alloc_list.Reset();
  cssEl_alloc_last = NULL;
#endif
  yydebug = (dblev > 3);
}

void cssProgSpace::ListFun(const String& fnm) {
  if(!HaveCmdShell()) return;
  pager_ostream& fh = cmd_shell->pgout;
  fh.start();

  bool got_one = false;
  String clnm;
  String funm = fnm;
  if(fnm.contains("::")) {
    clnm = fnm.before("::");
    funm = fnm.after("::");
  }

  for(int i=0;i<types.size;i++) {
    cssEl* el = types[i];
    if(el->GetType() == cssEl::T_ClassType) {
      cssClassType* cl = (cssClassType*)el->GetNonRefObj();
      if(clnm.nonempty() && cl->name != clnm) continue;
      cssElPtr ptr = cl->methods->FindName(funm); // find name of this type..
      if(ptr != 0) {
	cssMbrScriptFun* meth = (cssMbrScriptFun*)cl->methods->FastEl(ptr.dx);
	fh << "\nListing of Method: " << cl->name << "::" << meth->name << "\n";
	cssProg* fun = meth->GetSubProg();
	if(fun)
	  fun->ListSrc();
	got_one = true;
      }
    }
  }

  for(int i=0;i<statics.size;i++) {
    cssEl* el = statics[i];
    if(el->name == funm) {
      if(el->HasSubProg() && (el->GetType() != cssEl::T_CodeBlock)) {
	fh << "\nListing of Function: " << el->name << "\n";
	el->GetSubProg()->ListSrc();
	got_one = true;
      }
    }
  }
  if(!got_one)
    cssMisc::Warning(Prog(), "Function", fnm, "not found!");
}

void cssProgSpace::ListSrc(int stln) {
  if(!HaveCmdShell()) return;
  pager_ostream& fh = cmd_shell->pgout;
  fh.start();
  if(stln < 0) {
    if(size > 1)	// prog currently running; start list from there
      stln = Prog()->CurSrcLn();
    else
      stln = list_ln;
  }
  fh << "\nListing of Program: " << name << "\n";
  ListSrc_impl(fh, stln);
  fh << "\n";
}

void cssProgSpace::ListConstants() {
  if(!HaveCmdShell()) return;
  cssMisc::Constants.List(cmd_shell->pgout);
}

void cssProgSpace::ListDefines() {
  if(!HaveCmdShell()) return;
  cssMisc::Defines.NameList(cmd_shell->pgout);
}

void cssProgSpace::ListEnums() {
  if(!HaveCmdShell()) return;
  cmd_shell->pgout << "\nEnumerated types in local program space\n";
  enums.NameList(cmd_shell->pgout);
  cmd_shell->pgout << "\nEnumerated types in global name space\n";
  cssMisc::Enums.NameList(cmd_shell->pgout);
}

void cssProgSpace::ListFunctions() {
  if(!HaveCmdShell()) return;
  cmd_shell->pgout << "\nGlobal builtin functions\n";
  cssMisc::Functions.NameList(cmd_shell->pgout, 1);
  cssMisc::HardFuns.NameList(cmd_shell->pgout, 1);
  cmd_shell->pgout << "\nHard-coded functions from parent object\n";
  hard_funs.NameList(cmd_shell->pgout, 1);

  cmd_shell->pgout << "\nCss-coded functions\n";
  for(int i=0;i<statics.size;i++) {
    cssEl* el = statics[i];
    if(el->HasSubProg() && (el->GetType() != cssEl::T_CodeBlock)) {
      cmd_shell->pgout << el->PrintStr() << "\n";
    }
  }
}

void cssProgSpace::ListGlobals() {
  if(!HaveCmdShell()) return;
  pager_ostream& fh = cmd_shell->pgout;
  
  fh << "Global vars:\n";
  cssMisc::HardVars.List(fh);
  fh << "\n";
  cssMisc::Externs.List(fh);
  fh << "\n";
  hard_vars.List(fh);
  fh << "\n";
  prog_vars.List(fh);
  fh << "\n";
}

void cssProgSpace::ListLocals(int levels_back) {
  if(!HaveCmdShell()) return;
  pager_ostream& fh = cmd_shell->pgout;
  
  if(levels_back < 0) levels_back = 0;
  int lev = size - 1 - levels_back;
  if(lev < 0) lev = 0;

  fh << "Local vars for stack frame: " << levels_back << " (levels from top: " << lev << ")\n";
  Prog(lev)->ListLocals(fh, -1, 0);
  if(lev == 0)
    statics.List(fh, 0, 1);
}

void cssProgSpace::ListObjHards() {
  if(!HaveCmdShell()) return;
  pager_ostream& fh = cmd_shell->pgout;
  
  fh << "Containing object (Script or Program) vars (in reverse search order):\n";
  hard_vars.List(fh);
  fh << "\n";
  hard_funs.List(fh);
  fh << "\n";
  prog_vars.List(fh);
  fh << "\n";
}

void cssProgSpace::ListSettings() {
  if(!HaveCmdShell()) return;
  ostream& fh = *cmd_shell->fout;

  fh << "Include Paths:\n";
  taMisc::css_include_paths.List(fh);
  fh << "\n";
  for(int i=1; i<cssMisc::Settings.size; i++) {
    cssMisc::Settings.FastEl(i)->Print(fh);
    fh << "\n";
  }
}

void cssProgSpace::ListTypes() {
  if(!HaveCmdShell()) return;
  pager_ostream& fh = cmd_shell->pgout;
  fh.start();
  fh << "Global types: " << "\n";
  cssMisc::TypesSpace.NameList(fh);
  fh << "\n==========================\n";
  fh << "Types local to current top-level program space (" << name << "):" << "\n";
  cmd_shell->src_prog->types.NameList(fh);
}

static const char* rs_vals[] = {"Waiting", "Running", "Stopping", "NewProgShoved",
			   "Returning", "Breaking", "Continuing", "BreakPoint",
			   "ExecError", "Bailing"};

void cssProgSpace::Status() {
  if(!HaveCmdShell()) return;
  ostream& fh = *cmd_shell->fout;

  fh << "\n\tStatus of Program: " << name << "\n";

  fh << "curnt:\t" << Prog()->name << "\tsrc_ln:\t" << Prog()->CurSrcLn()
    << "\tpc:\t" << Prog()->PC() << "\n";
  fh << "debug:\t" << debug << "\tstep:\t" << step_mode
    << "\trun_depth:\t" << size-1 << "\tparse_depth:\t" << parse_depth << "\n";
  fh << "lines:\t" << src_ln << "\tlist:\t" << list_ln << "\n";

  int rstat = (state & cssProg::State_Run) ? 1 : 0;
  int nobreak = (Prog()->state & cssProg::State_NoBreak) ? 1 : 0;
  fh << "State: run:\t" << rstat
     << "\tnobrk: " << nobreak << endl;

  fh << "run status:\t" << rs_vals[run_stat] << "\n";
  
  fh << "external_stop:\t" << external_stop << "\n";

  cmd_shell->fout->flush();
}

void cssProgSpace::BackTrace(int levels_back) {
  if(!HaveCmdShell()) return;
  pager_ostream& fh = cmd_shell->pgout;
  fh << "\nBackTrace of Program: " << name << "\n";

  if(levels_back < 0) levels_back = size - 1;
  int stop_lev = size - 1 - levels_back;
  if(stop_lev < 0) stop_lev = 0;

  for(int i=size-1; i>=stop_lev; i--) {
    int cnt = size-1 - i;
    cssProg* cp = Prog(i);
    String nm = cp->name;
    cssScriptFun* cur_fun = cp->GetCurrentFun();
    if(cur_fun) {
      nm = cur_fun->PrintStr();
      if(debug >= 1)
	nm += " (" + cp->name + ")";
    }
    fh << "#" << cnt << "  " << nm << "\n";
    int curpc = cp->PC(Prog_Fr(i));
    cssInst* inst = cp->Inst(curpc);
    if(inst) {
      fh << cssMisc::Indent(1) << inst->PrintStr() << "\n";
    }
    if(debug >= 2) {
      int fr = Prog_Fr(i);
      cp->Stack(fr)->List(fh, 2, 1);
      if(cp->Autos(fr))
	cp->Autos(fr)->List(fh, 2, 1);
    }
  }
}

void cssProgSpace::Help(cssEl* help_on) {
  if(!HaveCmdShell()) return;
  cmd_shell->pgout.start();

  if(help_on) {
    ostream& fh = *cmd_shell->fout;
    if(help_on->GetType() == cssEl::T_ElCFun) {
      cssElCFun* fun = (cssElCFun*)help_on;
      fh << "\nHelp for function: " << fun->name << "\n" << fun->name << " ";
      String str = fun->help_str;
      int wdth = 0;
      while(!str.empty()) {
	String wrd;
	if(str.contains(' ')) {
	  wrd = str.before(' ');
	  str = str.after(' ');
	}
	else {
	  wrd = str;
	  str = "";
	}
	if(wdth + (int)wrd.length() > taMisc::display_width) {
	  fh << "\n";
	  wdth = 0;
	}
	fh << wrd << " ";
	wdth += wrd.length() + 1;
      }
      fh << "\n";
    }
    else {
      help_on->TypeInfo(fh);
      fh << "\n";
    }
  }
  else {
    Help_Generic(); 
  }
}

void cssProgSpace::Help_Generic() {
  cmd_shell->pgout << "\nC^c syntax is a subset of C++, with standard C math and stdio functions.\n\
 Except: The (f)printf functions take arguments which print themselves\n\
 \tprintf(\"varname:\\t\",avar,\"\\tvar2:\\t\",var2,\"\\n\"\n\
 and a special String type is available for strings (ala C++)\n";

  cmd_shell->pgout << "\nArguments interpreted by C^c are:\n\
 [-f|-file] <file>\tcompile and execute given file upon startup, exit (unless -i)\n\
 [-e|-exec] <code>\tcompile and execute given code upon startup, exit (unless -i)\n\
 [-i|-interactive]\tif using -f or -e, go into interactive (prompt) mode after\n\
 -v[<number>]     \trun with debug level set to given number (default 1)\n\
 [-b|-bp] <line>  \tset initial breakpoint at given line of code (if using -f)\n\n\
 Any other arguments can be accessed by user script programs by the global\n\
 variables argv (an array of strings) and argc (an int)\n";

  cmd_shell->pgout << "\nDo help <expr> to obtain more detailed help on functions, objects, etc.\n";

  cmd_shell->pgout << "\nThe following debugging & control commands are available\n";
  cssMisc::Commands.NameList(cmd_shell->pgout, 1);
  Info_Generic();
}

void cssProgSpace::Info(const String& inf_type, cssEl* arg) {
  cmd_shell->pgout.start();
  String it = inf_type;
  it.downcase();
  if(it.empty()) {
    Info_Generic();
    return;
  }
  if(it.startsWith("b")) {
    ShowBreaks();
  }
  else if(it.startsWith("c")) {
    ListConstants();
  }
  else if(it.startsWith("d")) {
    ListDefines();
  }
  else if(it.startsWith("e")) {
    ListEnums();
  }
  else if(it.startsWith("fr")) {
    ListLocals(0);		// todo: what else?
  }
  else if(it.startsWith("fu")) {
    ListFunctions();
  }
  else if(it.startsWith("g")) {
    ListGlobals();
  }
  else if(it.startsWith("i")) {
    if(arg) {
      arg->InheritInfo(*cmd_shell->fout); *cmd_shell->fout << endl;
    }
  }
  else if(it.startsWith("l")) {
    if(arg)
      ListLocals((int)*arg);
    else
      ListLocals();
  }
  else if(it.startsWith("m")) {
    taMisc::MallocInfo(*cmd_shell->fout);
  }
  else if(it.startsWith("o")) {
    ListObjHards();
  }
  else if(it.startsWith("se")) {
    ListSettings();
  }
  else if(it == "status") {
    Status();
  }
  else if(it == "stack") {
    if(arg)
      BackTrace((int)*arg);
    else
      BackTrace();
  }
  else if(it.startsWith("to")) {
    if(arg) {
      arg->TokenInfo(*cmd_shell->fout); *cmd_shell->fout << endl;
    }
  }
  else if(it.startsWith("ty")) {
    if(arg) {
      arg->TypeInfo(*cmd_shell->fout); *cmd_shell->fout << endl;
    }
    else {
      ListTypes();
    }
  }
  else if(it.startsWith("v")) {
    ListGlobals();
    ListLocals(0);
  }
  if(it.startsWith("w")) {
    ShowWatchpoints();
  }
}

void cssProgSpace::Info_Generic() {
  cmd_shell->pgout << "\nThe following types of detailed info are available:\n\
  (only unique letters required to be specified)\n";

  cmd_shell->pgout << "\
  args           Argument variables for current stack frame\n\
  breakpoints    Current breakpoints\n\
  constants      List of defined constants\n\
  defines        List of #define pre-processor macros\n\
  enums          List of enums (see also type info for specific classes containing enums)\n\
  frame, [#]     All about the stack frame (# levels back)\n\
  functions      List of all available functions\n\
  globals        Global variables\n\
  inherit, <typ> Inheritance info for given type\n\
  locals, [#]    Local variables in stack frame (# levels back)\n\
  malloc         Memory allocation info\n\
  obj            Hard-coded variables and functions from owner Script or Program object\n\
  settings       List of current settings (static members of class taMisc \n\
                  -- can be set e.g., taMisc::display_width = 90;\n\
  stack, [#]     Backtrace of the stack (# levels back) (same as 'backtrace' command)\n\
  status         General status & current state of the program\n\
  tokens, <typ>  List of tokens (instances) for given type of object\n\
  types, [<typ>] List of all types (or type information for given type of object)\n\
  variables      List all variables\n\
  watchpoints    List all watchpoints\n";
}


//////////////////////////////////////////////////
// 	cssProgSpace:    Breakpoints 		//
//////////////////////////////////////////////////

bool cssProgSpace::SetBreak(int srcln) {
  if(srcln > src_ln) {
    cssMisc::Warning(Prog(), "Breakpoint larger than max line number");
    return false;
  }

  for(int i=0;i<types.size;i++) {
    cssEl* el = types[i];
    if(el->GetType() == cssEl::T_ClassType) {
      cssClassType* cl = (cssClassType*)el->GetNonRefObj();
      for(int j=0;j<cl->methods->size;j++) {
	cssProg* fun = cl->methods->FastEl(j)->GetSubProg();
	if(fun) {
	  if(fun->SetBreak(srcln)) return true;
	}
      }
    }
  }
  for(int i=0;i<statics.size;i++) {
    cssEl* el = statics[i];
    if(el->HasSubProg() && (el->GetType() != cssEl::T_CodeBlock)) {
      if(el->GetSubProg()->SetBreak(srcln)) return true;
    }
  }
  if(Prog(0)->SetBreak(srcln))
    return true;
  cssMisc::Warning(Prog(), "Source line not found");
  return false;
}

void cssProgSpace::ShowBreaks() {
  if(!HaveCmdShell()) return;

  for(int i=0;i<types.size;i++) {
    cssEl* el = types[i];
    if(el->GetType() == cssEl::T_ClassType) {
      cssClassType* cl = (cssClassType*)el->GetNonRefObj();
      for(int j=0;j<cl->methods->size;j++) {
	cssProg* fun = cl->methods->FastEl(j)->GetSubProg();
	if(fun) fun->ShowBreaks();
      }
    }
  }
  for(int i=0;i<statics.size;i++) {
    cssEl* el = statics[i];
    if(el->HasSubProg() && (el->GetType() != cssEl::T_CodeBlock)) {
      el->GetSubProg()->ShowBreaks(*cmd_shell->fout);
    }
  }
  Prog(0)->ShowBreaks(*cmd_shell->fout);
}

bool cssProgSpace::DelBreak(int srcln) {
  if(srcln > src_ln) {
    cssMisc::Warning(Prog(), "Breakpoint larger than max line number");
    return false;
  }
  for(int i=0;i<types.size;i++) {
    cssEl* el = types[i];
    if(el->GetType() == cssEl::T_ClassType) {
      cssClassType* cl = (cssClassType*)el->GetNonRefObj();
      for(int j=0;j<cl->methods->size;j++) {
	cssProg* fun = cl->methods->FastEl(j)->GetSubProg();
	if(fun) {
	  if(fun->DelBreak(srcln)) return true;
	}
      }
    }
  }
  for(int i=0;i<statics.size;i++) {
    cssEl* el = statics[i];
    if(el->HasSubProg() && (el->GetType() != cssEl::T_CodeBlock)) {
      if(el->GetSubProg()->DelBreak(srcln)) return true;
    }
  }
  if(Prog(0)->DelBreak(srcln))
    return true;
  cssMisc::Warning(Prog(), "Breakpoint not found");
  return false;
}

//////////////////////////////////////////////////
// 	cssProgSpace:    Watchpoints 		//
//////////////////////////////////////////////////

bool cssProgSpace::SetWatch(cssEl* watch) {
  if(!watch || watch == &cssMisc::Void) return false;
  cssWatchPoint* wp = new cssWatchPoint;
  wp->SetWatch(watch);
  watchpoints.Add(wp);
  return true;
}

void cssProgSpace::ShowWatchpoints() {
  if(!HaveCmdShell()) return;
  if(watchpoints.size == 0) return;
  ostream& fh = *cmd_shell->fout;
  fh << "Watchpoints for prog: " << name << endl;
  for(int i=0; i<watchpoints.size; i++) {
    cssWatchPoint* wp = watchpoints[i];
    fh << "watch point: " << i  << "\t" << wp->GetStr() << endl;
  }
}

bool cssProgSpace::DelWatch(cssEl* watch) {
  bool got = false;
  for(int i=watchpoints.size-1; i >= 0; i--) {
    cssWatchPoint* wp = watchpoints[i];
    if(wp->watch == watch) {
      watchpoints.RemoveIdx(i);
      got = true;
    }
  }
  return got;
}

bool cssProgSpace::DelWatchIdx(int idx) {
  bool got = watchpoints.RemoveIdx(idx);
  return got;
}



///////////////////////////////////
//   CmdShell
///////////////////////////////////

static cssConsole* qand_console = NULL;
#ifdef HAVE_QT_CONSOLE
QPointer<QcssConsole> qcss_console;
#endif

void cssCmdShell::Constr() {
  fin = &cin;  fout = &cout;  ferr = &cerr;
  pgout.fin = fin; pgout.fout = fout; pgout.n_lines = 40;
  pgout.no_page = true; // very dangerous, except maybe with Qt guy, and dangerous even then!

  console_type = taMisc::CT_NONE;

  external_exit = false;
//   sc_shell_this = NULL;
  
  src_prog = NULL;
  cmd_prog = new cssProgSpace("Cmd Shell Prog Space");
  cmd_prog->cmd_shell = this;	// link it up

  prompt = "css> ";

  stack_size = 0;
  stack_alloc_size = 2;
  src_prog_stack = (cssProgSpace**)calloc(stack_alloc_size, sizeof(cssProgSpace*));
}

cssCmdShell::cssCmdShell() {
  Constr();
}

cssCmdShell::cssCmdShell(const String& nm) {
  name = nm;
  Constr();
}

cssCmdShell::~cssCmdShell() {
  PopAllSrcProg();
  src_prog = NULL;
  delete cmd_prog;
  cmd_prog = NULL;
  free(src_prog_stack);
  src_prog_stack = NULL;
}

void cssCmdShell::AllocSrcProg(int sz) {
  if(stack_alloc_size >= sz)	return;	// no need to increase..
  sz = MAX(16,sz);		// once allocating, use a minimum of 16
  stack_alloc_size += TA_ALLOC_OVERHEAD; // increment to full power of 2
  while((stack_alloc_size-TA_ALLOC_OVERHEAD) <= sz) stack_alloc_size <<= 1;
  stack_alloc_size -= TA_ALLOC_OVERHEAD;
  src_prog_stack = (cssProgSpace**)realloc(src_prog_stack, stack_alloc_size * sizeof(cssProgSpace*));
}

void cssCmdShell::PushSrcProg(cssProgSpace* ps) {
  if(src_prog == ps) return;	// don't push on if already on!
  if(stack_size+1 >= stack_alloc_size)
    AllocSrcProg(stack_size+1);
  src_prog_stack[stack_size++] = ps;
  src_prog = ps;
  ps->cmd_shell = this;		// link to this shell
  SetPrompt(ps->name, true);		// name is the prompt..
}

cssProgSpace* cssCmdShell::PopSrcProg(cssProgSpace* ps) {
  if(stack_size <= 1)		// always keep the top guy
    return NULL;
  if((ps) && (src_prog != ps)) return NULL;
  cssProgSpace* tmp = src_prog;
  tmp->cmd_shell = NULL;	// unlink from this shell
  stack_size--;
  src_prog = src_prog_stack[stack_size-1];
  SetPrompt(src_prog->name, true);	// restore previous prompt
  return tmp;
}

void cssCmdShell::PopAllSrcProg() {
  while(PopSrcProg());
}

void cssCmdShell::AcceptNewLine_Qt(QString ln, bool eof) {
  AcceptNewLine(ln, eof);
}

// this is the work-horse of the shell: a new string line is sent to it by some
// outer-loop, and it is processed (compiled, etc).
void cssCmdShell::AcceptNewLine(const String& ln, bool eof) {
  cmd_prog->CompileCode(ln);
  if(cmd_prog->debug >= 2) {
    cmd_prog->ListSrc(0);
  }
  bool run_cmd = true;
  if(cmd_prog->parse_depth > 0) { // user entered a { so don't run yet
    if(cmd_prog->debug > 0)
      cerr << "<cmdshell: parsing more (depth)>\n";
    run_cmd = false;
  }
  if(cmd_prog->CurSrcCharsLeft() > 0) { // more stuff left to parse, not sure about this
    if(cmd_prog->debug > 0)
      cerr << "<cmdshell: more left to parse)>\n";
    run_cmd = false;
  }

  if(run_cmd) {
    cmd_prog->Run();
    cmd_prog->Reset();
  }
  UpdatePrompt();
}

void cssCmdShell::StartupShellInit(istream& fhi, ostream& fho,
  taMisc::ConsoleType cons_typ)
{
  console_type = cons_typ;

  fin = &fhi;
  fout = &fho;
  pgout.fin = fin; pgout.fout = fout; pgout.n_lines = 40;

#ifndef __GNUG__
  fout->sync_with_stdio();
#endif
  if(src_prog && (cssMisc::init_debug >= 0)) {
    src_prog->SetDebug(cssMisc::init_debug);
  }

  // startup file: TODO: put into app data folder
  cmd_prog->CompileRunClear(".cssinitrc");

//   SetName(cssMisc::prompt);
#if (!defined(TA_OS_WIN))
  // no longer trapping FPE -- now checking proactively in css!
//   signal(SIGFPE, (SIGNAL_PROC_FUN_TYPE) cssMisc::fpecatch);
//   signal(SIGTRAP, (SIGNAL_PROC_FUN_TYPE) cssMisc::fpecatch);
  signal(SIGINT, (SIGNAL_PROC_FUN_TYPE) cssMisc::intrcatch);
#endif
  switch (console_type) {
  case taMisc::CT_OS_SHELL:		// quick-and-dirty console (compatible with qt, but uses stdin/out
    cssMisc::TopShell->Shell_OS_Console(cssMisc::prompt);
    break;
#ifdef HAVE_QT_CONSOLE
  case taMisc::CT_GUI:
    cssMisc::TopShell->Shell_Gui_Console(cssMisc::prompt);
    break;
#endif
  case taMisc::CT_NONE:
    cssMisc::TopShell->Shell_No_Console(cssMisc::prompt);
    break;
  // all cases handled
  }
}

bool cssCmdShell::RunStartupScript() {
  bool ran = false;
  if(cssMisc::startup_file != "") {
    src_prog->Reset();
    src_prog->Compile(cssMisc::startup_file);
    if(cssMisc::init_bpoint >= 0)
      src_prog->SetBreak(cssMisc::init_bpoint);
    src_prog->Run();
    src_prog->EndRunPop();
    ran = true;
  }

  // allow both startup_file and startup_code to co-exist..
  if (cssMisc::startup_code != "") {
    src_prog->Reset();
    src_prog->CompileCode(cssMisc::startup_code);
    src_prog->Run();
    src_prog->EndRunPop();
    ran = true;
  }
  return ran;
}

void cssCmdShell::SetPrompt(const String& prmpt, bool disp_prompt) {
  if(prmpt.nonempty())
    prompt = prmpt;
  else
    prompt = cssMisc::prompt;

  if(prompt.contains('/'))
    prompt = prompt.after('/', -1);
  if(prompt.contains('\\')) //windows
    prompt = prompt.after('\\', -1);
  if(prompt.contains(".css"))
    prompt = prompt.before(".css");
  UpdatePrompt(disp_prompt);
}

//////////////////////////////////////////////////////
//  Specific console details

extern "C" {
  extern char* rl_readline(char*);
  extern void add_history(char*);
  extern int rl_done;		// readline done reading
  extern int (*rl_event_hook)(void);	// rl callback routine -- only used in NoConsole
}

void cssCmdShell::UpdatePrompt(bool disp_prompt) {
  if(!src_prog) {
    act_prompt = "no src_prog> ";
    return;
  }
  const char* pt = ">";
  //  pt = (src_prog->state & cssProg::State_Defn) ? "#" : ">";
  if(src_prog->size > 1)
    act_prompt = String(src_prog->size-1) + " " + prompt + pt + " ";
  else if(src_prog->parse_depth > 0)
    act_prompt = String(src_prog->parse_depth) + " " + prompt + pt + " ";
  else if(cmd_prog->parse_depth > 0)
    act_prompt = String(cmd_prog->parse_depth) + " " + prompt + pt + " ";
  else
    act_prompt = prompt + pt + " ";
  switch (console_type) {
  case taMisc::CT_NONE: break; // TODO: nothing???
  case taMisc::CT_OS_SHELL:
    qand_console->setPrompt(act_prompt);
    break;
#ifdef HAVE_QT_CONSOLE
  case taMisc::CT_GUI:
    qcss_console->setPrompt(act_prompt, disp_prompt);	// do not display new prompt
    break;
#endif
  }
}

void cssCmdShell::Shell_OS_Console(const String& prmpt) {
  //WARNING: PAGING IS EVIL SINCE IT CONFLICTS WITH THE THREADED INPUT HANDLERS (in GUI ONLY?)
  if(taMisc::gui_active)
    pgout.no_page = !(taMisc::console_options & taMisc::CO_USE_PAGING_GUI);
  else
    pgout.no_page = !(taMisc::console_options & taMisc::CO_USE_PAGING_NOGUI);

  if(!qand_console)
    qand_console = cssConsole::Get_SysConsole();

  console_type = taMisc::CT_OS_SHELL;
  SetPrompt(prmpt);
  external_exit = false;

  cssMisc::TopShell->PushSrcProg(cssMisc::Top);

  //NB: we must use queued connection, because console lives in our thread,
  // but signal may be raised in another thread
  connect(qand_console, SIGNAL(NewLine(QString, bool)),
	  this, SLOT(AcceptNewLine_Qt(QString, bool)), Qt::QueuedConnection);
  qand_console->Start();
}

#ifdef HAVE_QT_CONSOLE
void cssCmdShell::Shell_Gui_Console(const String& prmpt) {
  qcss_console = QcssConsole::getInstance();

  if(!qcss_console) {
    taMisc::Error("cssCmdShell::Shell_Qt -- no console found!");
    return;
  }

  console_type = taMisc::CT_GUI;
  qcss_console->setPrompt(prmpt);
  external_exit = false;
  pgout.no_page = true;		// console has its own pager!

  cssMisc::TopShell->PushSrcProg(cssMisc::Top);
  qcss_console->flushOutput();	// get it flushed to start up
}
#endif
void cssCmdShell::Shell_No_Console(const String& prmpt) {
  rl_done = false;
  pgout.no_page = true; // default
  console_type = taMisc::CT_NONE;
  SetPrompt(prmpt);
  external_exit = false;

  cssMisc::TopShell->PushSrcProg(cssMisc::Top);
}

void cssCmdShell::Shell_NoConsole_Run() {
//TODO: rejig this for batch or piped contexts:
// end = EOF, no readline, non-blocking input
  rl_event_hook = taiMiscCore::rl_callback;
  while(!external_exit && !taMisc::quitting) {
    char* curln = rl_readline((char*)act_prompt);
    if (rl_done) break;
    // NULL result is defined as EOF
    if (curln == (char*)0) {
      Exit();
    } else {
      QString str = curln;
      free(curln); // spec has rl allocating this 
      curln = NULL;
      if(str.length() > 0)
	add_history((char*)(str.toAscii().constData()));
      AcceptNewLine(str, false);
    }
  }
}

void cssCmdShell::FlushConsole() {
#ifdef HAVE_QT_CONSOLE
  if(console_type == taMisc::CT_GUI) {
    if((bool)qcss_console) {
      qcss_console->flushOutput(true); // wait for pager!
      taiM->ProcessEvents();
    }
  }
#endif
}

void cssCmdShell::Exit() {
//TODO: first get working with uncancellable, then see where we can allow cancel
  external_exit = true;
  switch (console_type) {
  case taMisc::CT_OS_SHELL:
    break;
#ifdef HAVE_QT_CONSOLE
  case taMisc::CT_GUI:
    if((bool)qcss_console)
      qcss_console->exit();
    break;
#endif
  case taMisc::CT_NONE:
    break;
  }
  taiMiscCore::Quit();
}
