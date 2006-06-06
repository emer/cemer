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


// machine.cc
#include "ta_platform.h"
#include "css_machine.h"
#include "css_basic_types.h"
#include "css_c_ptr_types.h"
#ifndef CSS_NUMBER
# include "css_parse.h"
#endif


#include "ta_css.h"
#include "ta_base.h"		// for debugging alloc_list
#include "ta_matrix.h"


#ifdef TA_GUI
# include "css_qt.h"
# include <QApplication>
# ifdef TA_USE_INVENTOR
#   include <Inventor/Qt/SoQt.h>
# endif
#endif

#include <QEvent>
#include <QCoreApplication>

#include <sstream>

int yyparse(void);
void yyerror(char* s);


//////////////////////////
// 	cssMisc    	//
//////////////////////////

jmp_buf 	cssMisc::begin;

cssSpace	cssMisc::Internal("Internal");	// for use in internal (not searched)
cssSpace	cssMisc::Parse("Parse");		// just for parsing (searched)
cssSpace	cssMisc::PreProcessor("PreProcessor");// pre-processor functions (cpp)
cssSpace	cssMisc::TypesSpace("Types");
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
cssProgSpace*	cssMisc::delete_me = NULL;

cssArray*	cssMisc::s_argv;
cssInt*		cssMisc::s_argc;
String		cssMisc::prompt = "css";
String		cssMisc::startup_file;
String		cssMisc::startup_code;
int		cssMisc::init_debug = -1;
int		cssMisc::init_bpoint = -1;
bool		cssMisc::init_interactive = false;
cssConsole*	cssMisc::console = NULL;

cssEl 		cssMisc::Void("Void"); 	
cssElPtr	cssMisc::VoidElPtr;		// in theory, points to voidptr
cssPtr		cssMisc::VoidPtr;		// in theory, points to voidptr
cssArray	cssMisc::VoidArray;		// just needed for maketoken
cssArrayType    cssMisc::VoidArrayType;         // just needed for maketoken

String		cssMisc::VoidStringVal;
cssString	cssMisc::VoidString;
cssVariant	cssMisc::VoidVariant;
cssEnumType	cssMisc::VoidEnumType("VoidEnum");
cssClassType	cssMisc::VoidClassType("VoidClass");

cssRef		cssMisc::VoidRef;  // for maketoken

int		cssMisc::argc = 0;		// number of args passed by commandline to app
char**		cssMisc::argv = NULL;		// args passed by commandline to app
bool		cssMisc::gui = false;

void cssMisc::CodeConstExpr() {
  code_cur_top = ConstExprTop;
  ConstExprTop->Reset();
}

void cssMisc::CodeTop() {
  code_cur_top = NULL;
}

bool cssMisc::HasCmdLineSwitch(const String& sw_name, bool starts_with) {
  int dummy = 1;
  return HasCmdLineSwitch(sw_name, dummy, starts_with);
}

bool cssMisc::HasCmdLineSwitch(const String& sw_name, int& index,
  bool starts_with) 
{
  // looks for the switch value (include the '-' if applicable)
  bool rval = false;
  String tmp;
  while (!rval && (index < argc)) {
    tmp = argv[index];
    if (starts_with)
      rval = tmp.matches(sw_name); 
    else rval = (tmp == sw_name);
    ++index;
  }
  return rval;
}

bool cssMisc::CmdLineSwitchValue(const String& sw_name, int& index, 
  String& sw_value, bool starts_with) 
{
  bool rval = HasCmdLineSwitch(sw_name, index, starts_with); //note: index advanced
  if (rval) {
    if (index < argc) {
      sw_value = argv[index++];
    } else {
      sw_value = "";
    }
  }
  return rval;
}

void cssMisc::Error(cssProg* prog, const char* a, const char* b, const char* c, const char* d, const char* e, const char* f,
		  const char* g, const char* h, const char* i, const char* j, const char* k, const char* l)
{
  cssProgSpace* top;
  if(prog != NULL)
    top = prog->top;
  else
    top = cssMisc::cur_top;

  if(taMisc::dmem_proc == 0) {
    *(top->ferr) << a << " " << b << " " << c << " " << d << " " << e << " " << f << " "
		 << g << " " << h << " " << i << " " << j << " " << k << " " << l;
  }
  if(top->state & (cssProg::State_Run | cssProg::State_Cont | cssProg::State_RunLast)) {
    if(taMisc::dmem_proc == 0) {
      *(top->ferr) << "\n" << top->name << "\n>" << top->Prog()->CurSrcLn() << "\t"
		   << top->Prog()->GetSrcLC(top->Prog()->CurSrcLC()) << "\n";
      top->ferr->flush();
    }
    top->run = cssEl::ExecError;
    longjmp(cssMisc::begin, 1);
  }
  else {
    if(taMisc::dmem_proc == 0) {
      *(top->ferr) << "\n" << top->name << "\n>" << top->src_ln << "\t"
		   << top->Prog()->GetSrcLC(top->Prog()->tok_line) << "\n";
      top->ferr->flush();
    }
    top->run = cssEl::ExecError;
  }
}

String cssMisc::Indent(int indent_level) {
  if (indent_level == 0) return _nilString;
  else return String(indent_level * 2, 0, ' ');
}

inline bool btwn(char c, char l, char u) 
  {return ((c >= l) && (c <= u));}
inline bool is_alphalike(char c) 
  {return (btwn(c, 'a', 'z') || btwn(c, 'A', 'Z') || (c == '_'));}
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

void cssMisc::Warning(cssProg* prog, const char* a, const char* b, const char* c, const char* d, const char* e, const char* f,
		    const char* g, const char* h, const char* i, const char* j, const char* k, const char* l)
{
  cssProgSpace* top;
  if(prog != NULL)
    top = prog->top;
  else
    top = cssMisc::cur_top;

  if(taMisc::dmem_proc > 0) return;

  *(top->ferr) << a << " " << b << " " << c << " " << d << " " << e << " " << f << " "
	       << g << " " << h << " " << i << " " << j << " " << k << " " << l;
  if(top->state & (cssProg::State_Run | cssProg::State_Cont | cssProg::State_RunLast)) {
    *(top->ferr) << "\n" << top->name << "\n>" << top->Prog()->CurSrcLn() << "\t"
      << top->Prog()->GetSrcLC(top->Prog()->CurSrcLC()) << "\n";
  }
  else {
    *(top->ferr) << "\n" << top->name << "\n>" << top->src_ln << "\t"
      << top->Prog()->GetSrcLC(top->Prog()->tok_line) << "\n";
  }
  top->ferr->flush();
}

void cssMisc::PreInitialize(int argc_, char** argv_) {
  cssMisc::argc = argc_;
  cssMisc::argv = argv_; // cast away constness -- we still treat it as const
#ifdef TA_GUI
# ifdef TA_USE_INVENTOR
  SoQt::init(argc, argv, prompt.chars()); // creates a special Coin QApplication instance
# else
  new QApplication(argc, (char**)argv); // accessed as qApp
# endif
#else
  new QCoreApplication(argc, (char**)argv); // accessed as qApp
#endif

// create the system console
//TODO: may need to move somewhere if we are using a gui console
  if (!console) {
    console = cssConsole::Get_SysConsole();
  
#ifdef TA_USE_READLINE
    // have waitproc called back 20/s (instead of 10/s)
    rl_set_keyboard_input_timeout(50000); 
#endif //  TA_USE_READLINE
//TODO: modalize
    rl_done = false;
  }
}


#if (!defined(TA_OS_WIN))
void cssMisc::fpecatch(int) {
  signal(SIGFPE, (SIGNAL_PROC_FUN_TYPE) cssMisc::fpecatch);
  cssProgSpace* top = cssMisc::cur_top;
  if(top->state & (cssProg::State_Run | cssProg::State_Cont)) {
    cssMisc::Warning(NULL, "Floating point exception");
    top->run = cssEl::ExecError;
    longjmp(cssMisc::begin,1);
  }
}

void cssMisc::intrcatch(int) {
  signal(SIGINT, (SIGNAL_PROC_FUN_TYPE) cssMisc::intrcatch);
  cssMisc::Warning(NULL, "User Interrupt");
  cssProgSpace* top = cssMisc::cur_top;
  top->run = cssEl::ExecError;
  if(top->state & (cssProg::State_Run | cssProg::State_Cont))
    longjmp(cssMisc::begin,1);
}
#endif
//////////////////////////
// 	cssElPtr    	//
//////////////////////////

cssEl* cssElPtr::El() const {
  if(ptr == NULL) return &cssMisc::Void;
  switch(ptr_type) {
  case DIRECT:
    return (cssEl*)ptr;
  case CLASS_MEMBER: {
    if(dx < 0)	return &cssMisc::Void;
    cssClassInst* cur_th = cssMisc::cur_top->Prog()->CurThis();
    if(cur_th == NULL)
      return ((cssClassType*)ptr)->GetMember((int)dx);
    else
      return cur_th->GetMember((int)dx);
  }
  case NVIRT_METHOD: {
    if(dx < 0)	return &cssMisc::Void;
    return ((cssClassType*)ptr)->GetMemberFun((int)dx);
  }
  case VIRT_METHOD: {
    if(dx < 0)	return &cssMisc::Void; // try for virtual first
    cssClassInst* cur_th = cssMisc::cur_top->Prog()->CurThis();
    if(cur_th == NULL)
      return ((cssClassType*)ptr)->GetMemberFun((int)dx);
    else
      return cur_th->GetMemberFun((int)dx);
  }
  case PROG_AUTO:
    if(dx < 0)	return &cssMisc::Void;
    return ((cssProg*)ptr)->Autos()->El(dx);
  case SPACE:
    if(dx < 0)	return &cssMisc::Void;
    return ((cssSpace*)ptr)->El(dx);		// use the safe version..
  case NULL_PTR:
  default:
    return &cssMisc::Void;
  }
}

void cssElPtr::Print(ostream& fh) const {
  switch(ptr_type) {
  case DIRECT:
    fh << "Direct\t";
    break;
  case CLASS_MEMBER:
    fh << "Cls Mbr " << ((cssClassType*)ptr)->type_name << "\t";
    break;
  case NVIRT_METHOD:
    fh << "NV Mth " << ((cssClassType*)ptr)->type_name << "\t";
    break;
  case VIRT_METHOD:
    fh << "V Mth " << ((cssClassType*)ptr)->type_name << "\t";
    break;
  case PROG_AUTO:
    fh << "Prg Auto " << ((cssProg*)ptr)->name << "\t";
    break;
  case SPACE:
    fh << "Spc " << ((cssSpace*)ptr)->name << "\t";
    break;
  case NULL_PTR:
  default:
    fh << "Void Ptr\t";
  }
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

// this is needed early on
const int cssElFun::ArgMax = 64;

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

void cssEl::Done(cssEl* it) {
  if (it->refn < 0) {
    cerr << "**WARNING** ";
    print_cssEl(it, true);
    cerr << "::Done(): it->refn < 0  -- **MAY BE MULTI-DELETED**\n";
  } 
  else if (cssMisc::init_debug > 1) {
    print_cssEl(it);
    cerr << "::Done()\n";
  }
  //note: legacy compatibility for <= 0 in case some use cases do unRefs ???
  if (it->refn <= 0) 
    delete it; 
}

void cssEl::unRef(cssEl* it) {
  --(it->refn);
  if (it->refn < 0) {
    cerr << "**WARNING** ";
    print_cssEl(it, true);
    cerr << "::unRef(): it->refn < 0\n";
  }
  else if (cssMisc::init_debug > 1) {
    print_cssEl(it);
    cerr << "::unRef()\n";
  }
}

void cssEl::unRefDone(cssEl* it) {
  --(it->refn);
  if (it->refn < 0) {
    cerr << "**WARNING** ";
    print_cssEl(it, true);
    cerr << "::unRefDone: it->refn <= 0 -- **MAY BE MULTI-DELETED**\n";
  }
  else if (cssMisc::init_debug > 1) {
    print_cssEl(it);
    cerr << "::unRefDone()\n";
  }

  if (it->refn <= 0) 
    delete it; 
}

#endif //DEBUG

cssEl::~cssEl() {
  if(addr != NULL)
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
  addr = NULL; refn = 0; prog = 0;
#ifdef CSS_DEBUG_REGISTER
  if(cssEl_alloc_debug) {
    cssEl_alloc_count++;
    cssEl_alloc_list.Add((int)this);
    Register();
  }
#endif
}

cssEl* cssEl::GetTypeObject() const {
  String tp_nm = GetTypeName();
  tp_nm = tp_nm.after('(');
  tp_nm = tp_nm.before(')');
  cssElPtr s;
  if((prog != NULL) && (prog->top != NULL)) {
    if((s = prog->top->types.FindName((char*)tp_nm)) != 0)
      return s.El();
  }
  if(cssMisc::cur_top != NULL) {
    if((s = cssMisc::cur_top->types.FindName((char*)tp_nm)) != 0)
      return s.El();
  }
  if((s = cssMisc::TypesSpace.FindName((char*)tp_nm)) != 0)
    return s.El();
  return &cssMisc::Void;
}

cssElPtr cssEl::GetAddr() const {
  if(addr != NULL)
    return *addr;
  cssElPtr rval;
  rval.SetDirect((cssEl*)this);
  return rval;
}

void cssEl::SetAddr(const cssElPtr& cp) {
  if(addr == NULL)
    addr = new cssElPtr();
  *addr = cp;
}

cssEl::RunStat cssEl::Do(cssProg* prg) {
  prog = prg;
  prog->Stack()->Push(this);
  return cssEl::Running;
}

void cssEl::Copy(const cssEl& cp) {
  name = cp.name;  prog = cp.prog;
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
      prg->Stack()->Push(new cssRef(prg->Autos()->Push(tmp)));
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
  arg_holder.BindArgs(args, act_argc);
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

cssEl::operator TAPtr() const {
  if(GetType() == T_String) {
    void* rval;
    String nm = this->GetStr();
    if((tabMisc::root != NULL) && (tabMisc::root->FindMembeR(nm, rval) != 0))
      return (TAPtr)rval;
    return (TAPtr)NULL;
  }
  CvtErr("(TAPtr)"); return (TAPtr)NULL;
}

cssEl* cssEl::GetFromTA_impl(TypeDef* td, void* itm, const char* nm, MemberDef* md) const {
  TypeDef* nptd;

  nptd = td->GetNonPtrType(); // always create one of these

  if(nptd == NULL)
    return &cssMisc::Void;

  bool ro = false;
  if(md != NULL) {
    if(md->HasOption("READ_ONLY"))
      ro = true;
  }
  if(nptd->DerivesFrom(TA_bool))
    return new cssCPtr_bool(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if(nptd->DerivesFormal(TA_enum))
    return new cssCPtr_enum(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if ((nptd->DerivesFrom(TA_int) || nptd->DerivesFrom(TA_unsigned_int)))
    return new cssCPtr_int(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if(nptd->DerivesFrom(TA_short) || (nptd->DerivesFrom(TA_unsigned_short)))
    return new cssCPtr_short(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if (nptd->DerivesFrom(TA_long) || nptd->DerivesFrom(TA_unsigned_long))
    return new cssCPtr_long(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if (nptd->DerivesFrom(TA_char) || nptd->DerivesFrom(TA_unsigned_char)
    || nptd->DerivesFrom(TA_signed_char))
    return new cssCPtr_char(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if(nptd->DerivesFrom(TA_int64_t) || nptd->DerivesFrom(TA_uint64_t))
    return new cssCPtr_long_long(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if(nptd->DerivesFrom(TA_float))
    return new cssCPtr_float(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if(nptd->DerivesFrom(TA_double))
    return new cssCPtr_double(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if(nptd->DerivesFrom(TA_taString))
    return new cssCPtr_String(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if(nptd->DerivesFrom(TA_Variant))
    return new cssCPtr_Variant(itm, td->ptr+1, nm, (cssEl*)this, ro);
  else if(nptd->DerivesFrom(TA_taBase))
    return new cssTA_Base(itm, td->ptr+1, nptd, nm, (cssEl*)this, ro);

  return new cssTA(itm, td->ptr+1, nptd, nm, (cssEl*)this, ro);
}

cssEl* cssEl::GetVariantEl_impl(const Variant& val, int idx) const {
  switch (val.type()) {
  case Variant::T_String: {
    //TODO: maybe this should be Char???
    String nw_val = val.toString().elem(idx);
    return new cssString(nw_val);
    } break;
  case Variant::T_Matrix: {
    if (val.isNull()) {
      NopErr("[] on Variant Matrix that is null"); 
      break;
    }
    taMatrix* mat = val.toMatrix();
    Variant var(mat->SafeElAsVar_Flat(idx));
    return new cssVariant(var);
    }
  default:
    NopErr("[] on Variant that is not a String or Matrix"); 
  }
  return &cssMisc::Void;
}

int cssEl::GetMemberNo_impl(const TypeDef& typ, const char* memb) const {
  int md;
  typ.members.FindName(memb, md);	// just 1st order search
  return md;
}

int cssEl::GetMemberFunNo_impl(const TypeDef& typ, const char* memb) const {
  int md;
  typ.methods.FindName(memb, md);
  return md;
}

cssEl* cssEl::GetMember_impl(const TypeDef& typ, void* base, int memb) const {
  MemberDef* md = typ.members.SafeEl(memb);
  if (md == NULL) {
    cssMisc::Error(prog, "Member not found:", String(memb), "in class of type: ", typ.name);
    return &cssMisc::Void;
  }
  void* mbr = md->GetOff(base);
  return GetMember_impl(md, mbr);
}

cssEl* cssEl::GetMember_impl(MemberDef* md, void* mbr) const {
  return GetFromTA_impl(md->type, mbr, md->name, md);
}

cssEl* cssEl::GetMemberFun_impl(const TypeDef& typ, void* base, int memb) const {
  MethodDef* md = typ.methods.SafeEl(memb);
  if(md == NULL) {
    cssMisc::Error(prog, "Member function not found:", String(memb), "in class of type: ", typ.name);
    return &cssMisc::Void;
  }
  return GetMemberFun_impl(base, md);
}

cssEl* cssEl::GetMemberFun_impl(void* base, MethodDef* md) const {
  if(md->stubp != NULL) {
    if(md->fun_argd >= 0)
      return new cssMbrCFun(VarArg, base, md->stubp, md->name);
    else
      return new cssMbrCFun(md->fun_argc, base, md->stubp, md->name);
  }
  else {
    cssMisc::Error(prog, "Function pointer not callable:", md->name, "of type:", md->type->name,
	      "in class of type: ", md->GetOwnerType()->name);
    return &cssMisc::Void;
  }
}

cssEl* cssEl::GetScoped_impl(const TypeDef& typ, void* base, const char* memb) const {
  EnumDef* ed = typ.FindEnum(memb);
  if (ed != NULL) {
    return new cssInt(ed->enum_no);
  }

  MethodDef* md = typ.methods.FindName(memb);
  if(md == NULL) {
    cssMisc::Error(prog, "Scoped element not found:", memb, "in class of type: ", typ.name);
    return &cssMisc::Void;
  }

  return GetMemberFun_impl(base, md);
}



#ifdef CSS_SUPPORT_TYPEA
cssEl::operator TypeDef*() const {
  String nm = this->GetStr();
  TypeDef* td = taMisc::types.FindName(nm);
  if(td == NULL) {
    cssMisc::Error(prog, "Could not find type:", nm);
    return NULL;
  }
  return td;
}

cssEl::operator MemberDef*() const {
  String nm = this->GetStr();
  MemberDef* md;
  TA_MemberDef_ptr.SetValStr(nm, (void*)&md);
  if(md == NULL) {
    cssMisc::Error(prog, "Could not find member def:", nm);
    return NULL;
  }
  return md;
}

cssEl::operator MethodDef*() const {
  String nm = this->GetStr();
  MethodDef* md;
  TA_MethodDef_ptr.SetValStr(nm, (void*)&md);
  if(md == NULL) {
    cssMisc::Error(prog, "Could not find method def:", nm);
    return NULL;
  }
  return md;
}

#endif

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

int cssElFun::BindArgs(cssEl** args, int& act_argc) {
  args[0] = this;		// first argument is always selfptr
  act_argc = 0;

  if(argc == NoArg)
    return 0;

  cssSpace* stack = prog->Stack();
  if(prog->top->debug > 2) {
    cerr << "\nStack at time of function call to: " << name;
    stack->List(cerr);
    cerr << endl;
  }
  if(argc == 0) {
    if(stack->Peek() == &cssMisc::Void)	// get rid of arg stop..
      stack->Pop();
    return act_argc;
  }

  int stack_start;		// where to start getting things off of the stack
  if(argc == VarArg) {		// variable numbers of arguments
    for(stack_start = stack->size-1; stack_start >= 0; stack_start--) {
      if(stack->FastEl(stack_start) == &cssMisc::Void)
	break;
    }
    act_argc = ((int)stack->size - stack_start) - 1;
    int i;
    for(i=act_argc; i>0; i--)
      args[i] = stack->Pop();

    if(stack->Peek() == &cssMisc::Void)	// get rid of arg stop..
      stack->Pop();
  }
  else {
    int max_stack = stack->size - argc;
    for(stack_start = stack->size-1; stack_start >= max_stack; stack_start--) {
      if(stack->FastEl(stack_start) == &cssMisc::Void)
	break;
    }
    act_argc = ((int)stack->size - stack_start) - 1;
    int i;
    for(i=act_argc; i>0; i--)
      args[i] = stack->Pop();

    if(stack->Peek() == &cssMisc::Void)	// get rid of arg stop..
      stack->Pop();

    int diff = argc - act_argc;
    if(diff > arg_defs.size) {	// more difference than we have defaults..
      cssMisc::Error(prog, "Incomplete argument list for:", (const char*)name,
		     "should have:", String((int)(argc-arg_defs.size)), "got:",
		     String((int)act_argc));
    }
    if(diff != 0) {
      int df_st = arg_defs.size - diff;
      for(i=df_st; i < arg_defs.size; i++)	// fill in using default args..
	args[def_start + i] = arg_defs.FastEl(i);
    }
    act_argc = argc;		// we always get the right number...
  }
  return act_argc;
}

void cssElFun::DoneArgs(cssEl** args, int& act_argc) {
  int i;
  for(i=1; i <= act_argc; i++)
    cssEl::Done(args[i]);
  act_argc = 0;
}

void cssElFun::GetArgDefs() {
  arg_vals.EnforceSize(argc+1);
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
  Register();
  parse = CSS_FUN;
}

void cssElCFun::Copy(const cssElCFun& cp) {
  cssElFun::Copy(cp);
  funp = cp.funp;
  parse = cp.parse;
}

cssElCFun::cssElCFun(int ac, cssEl* (*fp)(int, cssEl* args[])) {
  Constr(); argc = ac;  funp = fp;
}
cssElCFun::cssElCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const char* nm) {
  Constr(); name = nm;  argc = ac;  funp = fp;
}
cssElCFun::cssElCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const char* nm, int pt, const char* hstr) {
  Constr(); name = nm;  argc = ac;  funp = fp;  parse = pt;
  if(hstr != NULL) help_str = hstr;
}
cssElCFun::cssElCFun(int ac, cssEl* (*fp)(int, cssEl* args[]),
			 const char* nm, cssEl* rtype, const char* hstr) {
  Constr(); name = nm;  argc = ac;  funp = fp;  parse = CSS_FUN;
  SetRetvType(rtype);
  if(hstr != NULL) help_str = hstr;
}
cssElCFun::cssElCFun(const cssElCFun& cp) {
  Constr(); Copy(cp);
}
cssElCFun::cssElCFun(const cssElCFun& cp, const char* nm) {
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
  cssEl* args[cssElFun::ArgMax + 1];
  int act_argc;
  BindArgs(args, act_argc);
  cssEl* tmp = (*funp)(act_argc, args);
  prog = prg;                   // restore if recursive
  if((tmp != NULL) && (tmp != &cssMisc::Void)) {
    tmp->prog = prog;
    prog->Stack()->Push(tmp);
  }
  DoneArgs(args, act_argc);
  return dostat;
}

//////////////////////////////////////////
//	internal fixed arg C functions	//
//////////////////////////////////////////

cssElInCFun::cssElInCFun(int ac, cssEl* (*fp)(int, cssEl* args[]))
  : cssElCFun(ac, fp)
{
}
cssElInCFun::cssElInCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const char* nm)
  : cssElCFun(ac, fp, nm)
{
}
cssElInCFun::cssElInCFun(int ac, cssEl* (*fp)(int, cssEl* args[]), const char* nm, int pt)
  : cssElCFun(ac, fp, nm, pt)
{
}
cssElInCFun::cssElInCFun(int ac, cssEl* (*fp)(int, cssEl* args[]),
			 const char* nm, cssEl* rtype)
  : cssElCFun(ac, fp, nm, rtype)
{
}
cssElInCFun::cssElInCFun(const cssElInCFun& cp)
  : cssElCFun(cp)
{
}
cssElInCFun::cssElInCFun(const cssElInCFun& cp, const char* nm)
  : cssElCFun(cp, nm)
{
}

// this assumes fixed argc, and does not assume a void pointer exists for marking args!
int cssElInCFun::BindArgs(cssEl** args, int& act_argc) {
  args[0] = this;		// first argument is always selfptr
  act_argc = 0;

  if((argc == NoArg) || (argc == 0))
    return act_argc;

  cssSpace* stack = prog->Stack();
  if(stack->size < argc) {
    cssMisc::Error(prog, "Incomplete argument list for:", (const char*)name,
		   "should have:", String((int)argc), "got:",
		   String((int)stack->size));
    return act_argc;
  }
  act_argc = argc;
  int i;
  for(i=act_argc; i>0; i--)
    args[i] = stack->Pop();

  return act_argc;
}


//////////////////////////////////////////
//	C code member functions		//
//////////////////////////////////////////

void cssMbrCFun::Constr() {
  Register();
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
cssMbrCFun::cssMbrCFun(int ac, void* th, cssEl* (*fp)(void*, int, cssEl**), const char* nm)
{
  Constr(); name = nm;  argc = ac;  ths = th; funp = fp;
}
cssMbrCFun::cssMbrCFun(const cssMbrCFun& cp) {
  Constr(); Copy(cp);
}
cssMbrCFun::cssMbrCFun(const cssMbrCFun& cp, const char* nm) {
  Constr(); Copy(cp); name = nm;
}
cssMbrCFun::~cssMbrCFun() {
  ths = NULL;
}


cssEl* cssMbrCFun::MakeToken_stub(int, cssEl* arg[]) {
  return new cssTA((void*)NULL, 1, &TA_int, (const char*)*(arg[1]));
}

cssEl::RunStat cssMbrCFun::Do(cssProg* prg) {
  prog = prg;
  dostat = cssEl::Running;
  cssEl* args[cssElFun::ArgMax + 1];
  int act_argc;
  BindArgs(args, act_argc);
  cssEl* tmp = (*funp)(ths, act_argc, args);
  prog = prg;                   // restore if recursive
  tmp->prog = prog;
  prog->Stack()->Push(tmp);
  DoneArgs(args, act_argc);
  return dostat;
}


//////////////////////////////////////////////////
// cssScriptFun: Script-defined functions	//
//////////////////////////////////////////////////

void cssScriptFun::Constr() {
  argv = new cssElPtr[ArgMax + 1];
  fun = new cssProg(name);
  cssProg::Ref(fun);
  fun->owner = this;
}

void cssScriptFun::Copy(const cssScriptFun& cp) {
  cssElFun::Copy(cp);
  if(fun != NULL)
    cssProg::unRefDone(fun);
  // note that this means that you could lose derived classes..
  fun = cp.fun;
  cssProg::Ref(fun);
  is_block = cp.is_block;
  int i;
  for(i=0; i<= ArgMax; i++)
    argv[i] = cp.argv[i];
}

cssScriptFun::cssScriptFun() {
  Constr();
  is_block = true;
  argc = 0;
}

cssScriptFun::cssScriptFun(const char* nm) {
  Constr();
  is_block = true;
  argc = 0;
  name = nm;
  fun->name = name;
}
cssScriptFun::cssScriptFun(const cssScriptFun& cp) {
  argv = new cssElPtr[ArgMax + 1];
  //  Constr();
  Copy(cp);
}
cssScriptFun::cssScriptFun(const cssScriptFun& cp, const char* nm) {
  argv = new cssElPtr[ArgMax + 1];
  //  Constr();
  Copy(cp);
  name = nm;
  fun->name = name;
}
cssScriptFun::~cssScriptFun() {
  delete [] argv;
  if(fun != NULL)
    cssProg::unRefDone(fun);
}

void cssScriptFun::List(ostream& fh) const {
  fun->List(fh);
}

void cssScriptFun::Define(cssProg* prg, bool, const char* nm) {
  if(nm != NULL)
    name = nm;
  is_block = false;		// not a block if it is defined..
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
  cssEl* args[cssElFun::ArgMax + 1];
  int act_argc;
  BindArgs(args, act_argc);   // get arguments from previous space

  cssProgSpace* old_top = fun->SetTop(prg->top); // propagate top to fun, get old
  prg->top->Shove(fun);		// push new state
  if(is_block)
    fun->SetCurThis(prg->CurThis());	// carry this pointer down for blocks..

  int i;
  for(i=1; i <= act_argc; i++) { // copy into args for current space
    (argv[i].El())->InitAssign(*args[i]);
  }

  prg->top->Cont();		// run the fun
  prg->top->EndRunPop();	// pop the results

  prog = prg;			// restore if recursive things happened

  if(is_block) {
    prg->top->Pull();		// jump down
    fun->PopTop(old_top);		// restore fun to previous top
    DoneArgs(args, act_argc);
    if(prg->top->run == cssEl::Stopping)
      return cssEl::Running;	// stopped naturally
    else
      return prg->top->run;	// some other kind of stopping
  }

  cssEl* tmp = (argv[0].El())->AnonClone(); // create clone of retval
  tmp->prog = prg;
  prg->top->Pull();		// jump down
  fun->PopTop(old_top);		// restore fun to previous top
  prg->Stack()->Push(tmp);
  DoneArgs(args, act_argc);
  return cssEl::Running;	// returning from a running program
}

cssEl* cssScriptFun::MakeToken_stub(int na, cssEl* arg[]) {
  if(!is_block) {		// only named programs return
    return (argv[0].El())->MakeToken_stub(na, arg);
  }
  return new cssInt(0);		// default retv is int, not void
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
  is_block = false;
}

void cssMbrScriptFun::Copy(const cssMbrScriptFun& cp) {
  cssScriptFun::Copy(cp);
  type_def = cp.type_def;
  if(type_def != NULL)
    cssEl::Ref(type_def);
  desc = cp.desc;
  opts = cp.opts;
  is_tor = cp.is_tor;
  is_virtual = cp.is_virtual;
}

cssMbrScriptFun::cssMbrScriptFun(const char* nm, cssClassType* cls)
: cssScriptFun(nm) {
  Constr();
  type_def = cls;
  if(type_def != NULL)
    cssEl::Ref(type_def);
  is_tor = false;
  is_virtual = false;
}
cssMbrScriptFun::cssMbrScriptFun(const cssMbrScriptFun& cp) {
  Constr();
  Copy(cp);
}
cssMbrScriptFun::cssMbrScriptFun(const cssMbrScriptFun& cp, const char* nm) {
  Constr();
  Copy(cp);
  name = nm;
}

cssMbrScriptFun::~cssMbrScriptFun() {
  if(type_def != NULL)
    cssEl::unRefDone(type_def);
  type_def = NULL;
}

// on the relationship between the same function in different classes:
// when the function is the same: (inherited from parent)
// 	- fun is a pointer to the same (shared) cssProg
// 	- cssMbrScriptFun is different, but its argv's are initialized to point
//		into the same fun auto's

void cssMbrScriptFun::Define(cssProg* prg, bool decl, const char* nm) {
  if(nm != NULL)
    name = nm;
  // code was previously defined, and needs to be overwritten
  if((fun->size > 0) || (decl && (fun->Autos()->size > 0))) {
    cssProg* old_fun = fun;
    fun = new cssProg(old_fun->name);
    cssProg::Ref(fun);
    fun->owner = this;
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

  if(fun->Autos()->size > 0) {	// already defined
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
  cssEl* args[cssElFun::ArgMax + 1];
  int act_argc;
  BindArgs(args, act_argc);   // get arguments from previous space

  cssProgSpace* old_top = fun->SetTop(prg->top); // propagate top
  prg->top->Shove(fun);	// push new state
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

  cssClassType* old_cls = cssMisc::cur_class;
  cssMbrScriptFun* old_meth = cssMisc::cur_method;

  cssMisc::cur_class = type_def;	// set the current class while in here..
  cssMisc::cur_method = this;

  prg->top->Cont();
  prg->top->EndRunPop();

  ths_ptr->DelOpr();		// get rid of pointer to this object

  cssMisc::cur_class = old_cls;	// no longer in class..
  cssMisc::cur_method = old_meth;

  prog = prg;			// restore if recursive

  cssEl* tmp = (argv[0].El())->AnonClone(); // create clone of retval
  tmp->prog = prg;
  prg->top->Pull();		       // jump down
  fun->PopTop(old_top);		       // restore previous top
  prg->Stack()->Push(tmp);
  DoneArgs(args, act_argc);
  return cssEl::Running;		// returning from a running program
}

cssEl* cssMbrScriptFun::MakeToken_stub(int na, cssEl* arg[]) {
  if(!is_block) {		// only named programs return
    return (argv[0].El())->MakeToken_stub(na, arg);
  }
  return new cssInt(0);		// default retv is int, not void
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

void cssMbrScriptFun::SetDesc(const char* des) {
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

String cssMbrScriptFun::OptionAfter(const char* opt) {
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
  read_only = false;
  Register();
}

cssCPtr::cssCPtr() {
  Constr();
}
cssCPtr::cssCPtr(void* it, int pc) {
  Constr(); ptr = it; ptr_cnt = pc;
}
cssCPtr::cssCPtr(void* it, int pc, const char* nm)	{
  Constr(); name = nm; ptr = it; ptr_cnt = pc;
}
cssCPtr::cssCPtr(void* it, int pc, const char* nm, cssEl* cp, bool ro) {
  Constr(); name = nm; ptr = it; ptr_cnt = pc;
  SetClassParent(cp);
  read_only = ro;
}
cssCPtr::cssCPtr(const cssCPtr& cp) {
  Constr(); Copy(cp);
  ptr = cp.ptr;
  ptr_cnt = cp.ptr_cnt;
  read_only = cp.read_only;
}
cssCPtr::cssCPtr(const cssCPtr& cp, const char* nm) 	{
  Constr(); Copy(cp);
  ptr = cp.ptr;
  ptr_cnt = cp.ptr_cnt;
  read_only = cp.read_only;
  name = nm;
}
cssCPtr::~cssCPtr()		   	   	{
  if(class_parent) cssEl::unRefDone(class_parent);
}

void* cssCPtr::GetVoidPtr(int cnt) const {
  void* rval = ptr;
  int r_cnt = ptr_cnt;
  while((rval != NULL) && (r_cnt > cnt)) {
    rval = *((void**)rval);
    r_cnt--;
  }
  if(r_cnt < cnt) {
    String rcs = String(r_cnt);
    String cns = String(cnt);
    cssMisc::Error(prog, "Pointer of count:", rcs, "is less indirect than the target:", cns);
    return NULL;
  }
  return rval;
}

void* cssCPtr::GetNonNullVoidPtr(int cnt) const {
  void* rval = GetVoidPtr(cnt);
  if(rval == NULL) {
    cssMisc::Error(prog, "Null c-pointer");
    return NULL;
  }
  return rval;
}

void cssCPtr::operator=(const cssEl& s) {
  if(s.GetType() == T_C_Ptr) 		PtrAssignPtr((cssCPtr*)&s);
  else {
    int sval = (Int)s;
    if(sval == 0)
      ptr = NULL;
    else
      cssMisc::Error(prog, "Assigning internal pointer to non-internal ptr value","");
  }
}

bool cssCPtr::ROCheck() {
  if(read_only) {
    cssMisc::Error(prog, "Pointer:", name, "of type:",GetTypeName(),
		   "points to a read-only object");
    return false;
  }
  return true;
}

void cssCPtr::PtrAssignPtr(cssCPtr* s) {
  String tpnm = GetTypeName();
  String s_tpnm = s->GetTypeName();
  if(ptr_cnt == s->ptr_cnt) {
    if((ptr_cnt == 1) && (class_parent != NULL) && (ptr != NULL) && (s->ptr != NULL)) {
      if ((tpnm == "(c_long_long)"))
	*this = (int64_t)*s;
      else if((tpnm == "(c_double)") || (tpnm == "(c_float)"))
	*this = (Real)*s;
      else if(tpnm == "(c_String)")
	*this = s->GetStr();
      else
	*this = (Int)*s;
      class_parent->UpdateAfterEdit();
      return;
    }
    ptr = s->ptr;
    SetClassParent(s->class_parent);
    if((prog != NULL) && (prog->top->debug) && (tpnm != s_tpnm))
	 cssMisc::Warning(prog, "Warning: assigning different ptr types");
  }
  else if(ptr_cnt == s->ptr_cnt + 1) {
    if(!ROCheck()) return;
    if(ptr != NULL)
      *((void**)ptr) = s->ptr;
    if(class_parent != NULL)	class_parent->UpdateAfterEdit();
    if((prog != NULL) && (prog->top->debug) && (tpnm != s_tpnm))
      cssMisc::Warning(prog, "Warning: assigning different ptr types");
  }
  else {
    cssMisc::Error(prog, "Pointer assignment between incompatible pointers");
  }
}

bool cssCPtr::SamePtrLevel(cssCPtr* s) {
  if(ptr_cnt == s->ptr_cnt) return true;
  cssMisc::Warning(prog, "Warning: comparison between different ptr types");
  return false;
}

void cssCPtr::UpdateAfterEdit() {
  if(class_parent != NULL)	class_parent->UpdateAfterEdit();
}

bool cssCPtr::operator==(cssEl& s) {
  if(s.GetType() == T_C_Ptr) {
    cssCPtr* pt = (cssCPtr*)&s;
    if(SamePtrLevel(pt))
      return ptr == pt->ptr;
    else
      return false;
  }
  return ((Int)(long)(ptr) == (Int)s);
}

bool cssCPtr::operator!=(cssEl& s) {
  if(s.GetType() == T_C_Ptr) {
    cssCPtr* pt = (cssCPtr*)&s;
    if(SamePtrLevel(pt))
      return ptr != pt->ptr;
    else
      return false;
  }
  return ((Int)(long)(ptr) != (Int)s);
}


cssEl* cssCPtr::operator*() {
  if(ptr_cnt <= 1)
    return this;		// magic ptr derefs to itself

  cssCPtr* rval = (cssCPtr*)AnonClone();
  rval->ptr = *((void**)ptr);	// go one down
  rval->ptr_cnt--;
  return rval;
}

void cssCPtr::SetClassParent(cssEl* cp) {
  if(class_parent != NULL)
    cssEl::unRefDone(class_parent);
  class_parent = cp;
  cssEl::Ref(class_parent);
}


//////////////////////////////////////////////////
// 	cssLex, Def: pre-processor stuff	//
//////////////////////////////////////////////////

// todo: move static lex functions to some other class..

String cssLex::Buf;

int cssLex::readword(cssProg* prog, int c) {
  Buf = "";

  Buf += (char)c;
  while (((c=prog->Getc()) != EOF) && (isalnum(c) || (c == '_')))
  { Buf += (char)c; }
  if(c == EOF)
    return EOF;
  prog->unGetc();
  return c;
}


void cssDef::Constr() {
  Register();
}

cssDef::cssDef(int ac) {
  Constr(); argc = ac;
}
cssDef::cssDef(int ac, const char* nm) {
  Constr(); name = nm;  argc = ac;
}
cssDef::cssDef(const cssDef& cp) {
  Constr(); Copy(cp); val = cp.val; which_arg = cp.which_arg;
}
cssDef::cssDef(const cssDef& cp, const char* nm) {
  Constr(); Copy(cp); val = cp.val; which_arg = cp.which_arg; name = nm;
}

void cssDef::Skip_To_Endif(cssProg* prog) {
  int cur_prog_size = prog->size;
  int cur_prog_src_size = prog->src_size;

  int incount = 0;

  while(1) {
    int c = prog->Getc();
    if(c == '#') {
      cssLex::readword(prog, prog->Getc());
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
  prog->unGetc();
  prog->ZapFrom(cur_prog_size, cur_prog_src_size);

// keep on numbering..
//  prog->top->src_ln = cur_src_ln;
//  prog->top->list_ln = cur_list_ln;
}

cssEl::RunStat cssDef::Do(cssProg*) {
// nothing, yet
  return cssEl::Running;
}


//////////////////////////////////////////
// 	cssSpace: the final frontier	//
//////////////////////////////////////////

// copy actually clones the elements of a space, so that the new space has fresh data

ostream& cssSpace::fancy_list(ostream& fh, const String& itm, int no, int prln, int tabs) {
  fh << itm << " ";
  if((no+1) % prln == 0) {
    fh << "\n";
    return fh;
  }
  int len = itm.length() + 1;
  int i;
  for(i=tabs; i>=0; i--) {
    if(len < i * 8)
      fh << "\t";
  }
  return fh;
}
String& cssSpace::fancy_list(String& fh, const String& itm, int no, int prln, int tabs) {
  fh += itm + " ";
  if((no+1) % prln == 0) {
    fh += "\n";
    return fh;
  }
  int len = itm.length() + 1;
  int i;
  for(i=tabs; i>=0; i--) {
    if(len < i * 8)
      fh += "\t";
  }
  return fh;
}

void cssSpace::Alloc(int sz) {
  if(alloc_size >= sz)	return;	// no need to increase..
  sz = MAX(16,sz);		// once allocating, use a minimum of 16
  alloc_size += TA_ALLOC_OVERHEAD; // increment to full power of 2
  while((alloc_size-TA_ALLOC_OVERHEAD) <= sz) alloc_size <<= 1;
  alloc_size -= TA_ALLOC_OVERHEAD;
  els = (cssEl**)realloc(els, alloc_size * sizeof(cssEl*));
}

void cssSpace::Constr() {
  els = (cssEl**)calloc(alloc_size, sizeof(cssEl*));
  size = 0;
  el_retv.SetSpace(this);
}

void cssSpace::Copy(const cssSpace &cp) {
  int i;
  for(i=0; i < cp.size; i++)
    Push(cp.els[i]->Clone());
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
  if(p == cssMisc::VoidElPtr) {
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
  if(p == cssMisc::VoidElPtr) {
    return Push(it);
  }
  cssEl::Done(it);
  return el_retv;
}


cssEl* cssSpace::Pop() {
  cssEl* tmp;
  if(size == 0) return &cssMisc::Void;
  tmp = els[--size];
  cssEl::unRef(tmp);
  return tmp;
}

void cssSpace::DelPop() {
  cssEl* tmp = Pop();
  if(tmp != &cssMisc::Void)
    cssEl::Done(tmp);
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

cssElPtr& cssSpace::FindName(const char* nm) {	// lookup by name
  for(el_retv.dx=0; el_retv.dx<size; el_retv.dx++) {
    if(els[el_retv.dx]->name == nm)
      return el_retv;
  }
  return cssMisc::VoidElPtr;
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

void cssSpace::List(ostream& fh) const {
  fh << "\nElements of Space: " << name << " (" << size << ")\n";
  fh << PrintStr() << "\n";
  fh.flush();
}
void cssSpace::ValList(ostream& fh) const {
  fh << "\nElement Values of Space: " << name << " (" << size << ")\n";
  fh << PrintFStr() << "\n";
  fh.flush();
}
void cssSpace::NameList(ostream& fh) const {
  int i;
  fh << "\nElement Names of Space: " << name << " (" << size << ")\n";
  int names_width = 0;
  for(i=0; i<size; i++) {
    names_width = MAX(names_width, (int)els[i]->name.length());
  }
  int tabs = (names_width / 8) + 1;
  int prln = taMisc::display_width / (tabs * 8);
  if(prln <= 0) prln = 1;
  for(i=0; i<size; i++) {
    cssSpace::fancy_list(fh, els[i]->name, i, prln, tabs);
  }
  fh << "\n";
  fh.flush();
}
void cssSpace::TypeNameList(ostream& fh) const {
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
      cssArray* ar = (cssArray*) mbr;
      fh << '[' << ar->items->size << ']';
    }
    else if (mbr->GetType() == cssEl::T_ArrayType) {
      cssArrayType* ar = (cssArrayType*) mbr;
      fh << '[' << ar->size << ']';
    }
    fh << "\n";
  }
}

void cssSpace::TypeNameValList(ostream& fh) const {
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
  }
}

String cssSpace::PrintStr() const {
  int i;
  int vars_width = 0;
  for(i=0; i<size; i++) {
    String tmp = els[i]->PrintStr();
    vars_width = MAX(vars_width, (int)tmp.length());
  }
  int tabs = (vars_width / 8) + 1;
  int prln = taMisc::display_width / (tabs * 8);
  if(prln <= 0) prln = 1;
  String rval;
  for(i=0; i<size; i++) {
    String tmp = els[i]->PrintStr();
    cssSpace::fancy_list(rval, tmp, i, prln, tabs);
  }
  return rval;
}
String cssSpace::PrintFStr() const {
  int i;
  int vals_width = 0;
  for(i=0; i<size; i++) {
    String tmp = els[i]->PrintFStr();
    vals_width = MAX(vals_width, (int)tmp.length());
  }
  int tabs = (vals_width / 8) + 1;
  int prln = taMisc::display_width / (tabs * 8);
  if(prln <= 0) prln = 1;
  String rval;
  for(i=0; i<size; i++) {
    String tmp = els[i]->PrintFStr();
    cssSpace::fancy_list(rval, tmp, i, prln, tabs);
  }
  return rval;
}

void cssSpace::Sort() {
  // lets do a heap sort since it requires no secondary storage
  // code borrowed from numerical recipies in C

  int n = size;
  int l,j,ir,i;
  cssEl* tmp;

  l = (n >> 1)+1;
  ir = n;
  for(;;) {
    if(l>1)
      tmp = els[--l -1]; 	// tmp = ra[--l]
    else {
      tmp = els[ir-1]; 		// tmp = ra[ir]
      els[ir-1] = els[0]; 	// ra[ir] = ra[1]
      if(--ir == 1) {
	els[0] = tmp; 		// ra[1]=tmp
	return;
      }
    }
    i=l;
    j=l << 1;
    while(j<= ir) {
      if(j<ir && (els[j-1]->name < els[j]->name)) j++;
      if(tmp->name  < els[j-1]->name) { 	// tmp < ra[j]
	els[i-1] = els[j-1]; 	// ra[i]=ra[j];
	j += (i=j);
      }
      else j = ir+1;
    }
    els[i-1] = tmp;		 // ra[i] = tmp;
  }
}


//////////////////////////////////
// 	cssInst: Instructions	//
//////////////////////////////////

int cssInst::Print(ostream& fh) const {
  cssListEl* src = prog->source[line];
  if((prog->top->ListDebug() >= 2)) {
    fh << src->ln << "\t" << idx << "\t";
    inst.Print(fh);
    (inst.El())->Print(fh);
    fh << "\t> " << prog->GetSrcLC(line, 0);
  }
  else
    fh << src->ln << "\t" << src->src;
  return 1;
}
int cssInst::List(ostream& fh) const {
  if(isdefn)
    (inst.El())->List(fh);
  else
    Print(fh);
  return 1;
}

void cssInst::Constr() {
  prog = NULL;
  idx = 0;
  line = 0;  col = 0;
  isdefn = false;
  previf = -1;
}

void cssInst::Copy(const cssInst& cp) {
  line = cp.line;
  col = cp.col;
  inst = cp.inst;
  isdefn = cp.isdefn;
  previf = cp.previf;
}

cssInst::cssInst() {
  Constr();
}
cssInst::cssInst(const cssProg& prg, const cssElPtr& it) {
  Constr();
  prog = (cssProg*)&prg;
  SetInst(it);
  idx = prog->AddCode(this);
}
cssInst::cssInst(const cssProg& prg, const cssElPtr& it, int lno, int clno) {
  Constr();
  prog = (cssProg*)&prg;
  line = lno;	col = clno;
  SetInst(it);
  idx = prog->AddCode(this);
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
      (it.ptr == (void*)&(prog->top->hard_funs))))
  {
    cssScriptFun* cur_fun = prog->top->GetCurrentFun();
    if((cur_fun != NULL) && ((cur_fun->GetType() == cssEl::T_MbrScriptFun) ||
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
#ifdef HAVE_VOLATILE
  volatile cssInst* ths = this;
#else
  cssInst* ths = this;
#endif
  setjmp(cssMisc::begin);
  if(ths->prog->top->run != cssEl::ExecError) {
    return (ths->inst.El())->Do(ths->prog);
  }
  return cssEl::ExecError;
}

void cssInst::SetDefn() {
  if((inst.El()->GetType() == cssEl::T_ScriptFun) ||
     (inst.El()->GetType() == cssEl::T_MbrScriptFun))
  {
    isdefn = true;
  }
  else {
    cssMisc::Warning(prog, "Attempt to flag non-script fun as isdefn");
  }
}

void cssInst::EndIf(css_progdx end) {
  if(end > 0) {
    // this fixes the problem that if a stack of else-ifs is not terminated with
    // a final else, there are two stop jumps at the end, and it needs fixed...
    if((end < prog->size) && prog->insts[end]->IsJump() &&
       (prog->insts[end]->GetJump() < 0))
      end++;
    prog->insts[idx + 3]->SetLine(end); // set my own end
  }
  else
    end = prog->insts[idx + 3]->GetJump();
  if((previf < 0) || (previf == idx))
    return;
  prog->insts[previf]->EndIf(end); // set the previous one's end
}

int cssIJump::Print(ostream& fh) const {
  cssListEl* src = prog->source[line];
  if(prog->top->ListDebug() >= 2) {
    fh << src->ln << "\t" << idx << "\t Jump->\t\t" << jumpto << "\t\t";
    fh << "> " << prog->GetSrcLC(line, 0);
  }
  else
    fh << src->ln << "\t" << prog->GetSrcLC(line, 0);
  return 1;
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

cssIJump::cssIJump(const cssProg &prg, css_progdx jmp) {
  Constr();
  prog = (cssProg*)&prg;
  jumpto = jmp;
  idx = prog->AddCode(this);
}
cssIJump::cssIJump(const cssProg &prg, css_progdx jmp, int lno, int clno) {
  Constr();
  prog = (cssProg*)&prg;
  jumpto = jmp;
  line = lno;	col = clno;
  idx = prog->AddCode(this);
}
cssIJump::cssIJump(const cssIJump& cp) {
  Constr();
  Copy(cp);
}

cssFrame::cssFrame(cssProg* prg) {
  prog = prg;
  fr_no = 0;
  pc = 0;
  stack.name = "Stack";
  autos.name = "Autos";
  autos.el_retv.SetProgAuto(prg);
  cur_this = NULL;
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
  owner = NULL;
  top = cssMisc::Top;

  insts = (cssInst**)malloc(alloc_size * sizeof(cssInst*));
  size = 0;
  parse_st_size = 0; parse_st_src_size = 0;
  run_st_size = 0; run_st_src_size = 0;

  frame = (cssFrame**)malloc(fr_alloc_size * sizeof(cssFrame*));
  fr_size = 0;

  literals.name = "Literals";
  statics.name = "Statics";
  saved_stack.name = "Saved Stack";

  source = (cssListEl**)malloc(src_alloc_size * sizeof(cssListEl*));
  src_size = 0;
  line = 0;  col = 0;
  st_line = 0; st_col = 0;
  tok_line = 0; tok_col = 0;
  state = 0;
  lastif = -1;	elseif = -1; lastdo = -1;
  AddFrame();			// always start off with one
}

void cssProg::Copy(const cssProg& cp) {
  literals.Copy(cp.literals);
  statics.Copy(cp.statics);
  Autos()->Copy(cp.frame[0]->autos);

  AllocSrc(cp.src_size+1);
  int i;
  for(i=0; i<cp.src_size; i++)
    source[i] = cp.source[i]->Clone();

  AllocInst(cp.size+1);
  for(i=0; i<cp.size; i++) {
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
  src_size = cp.src_size;
  line = cp.line;
  col = cp.col;
  st_line = cp.st_line;
  st_col = cp.st_col;
  tok_line = cp.tok_line;
  tok_col = cp.tok_col;
  state = cp.state;
  lastif = cp.lastif;
  elseif = cp.elseif;
  lastdo = cp.lastdo;
}

cssProg::cssProg() {
  Constr();
}
cssProg::cssProg(const char* nm) {
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
  free(source);
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

void cssProg::AllocSrc(int sz) {
  if(src_alloc_size >= sz)	return;	// no need to increase..
  sz = MAX(16,sz);		// once allocating, use a minimum of 16
  src_alloc_size += TA_ALLOC_OVERHEAD; // increment to full power of 2
  while((src_alloc_size-TA_ALLOC_OVERHEAD) <= sz) src_alloc_size <<= 1;
  src_alloc_size -= TA_ALLOC_OVERHEAD;
  source = (cssListEl**)realloc(source, src_alloc_size * sizeof(cssListEl*));
}

int cssProg::AddFrame() {
  if(fr_size+1 >= fr_alloc_size)
    AllocFrame(fr_size+1);
  frame[fr_size++] = new cssFrame(this);
  SetPC(0);
  if(fr_size > 1)
    Autos()->Copy(frame[0]->autos);
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
  Autos()->Reset();
  literals.Reset();
  statics.Reset();
}
void cssProg::ResetCode() {
  int i;
  Restart();
  for(i=0; i<size; i++)
    delete insts[i];
  size = 0;
  for(i=0; i<src_size; i++)
    delete source[i];
  src_size = 0;
}

//////////////////////////////////////////
//	cssProg: Source, Debugging	//
//////////////////////////////////////////

int cssProg::CurSrcLn(css_progdx pcval) {
  if(pcval > size-1)
    return 0;
  return source[insts[pcval]->line]->ln;
}

int cssProg::CurSrcLC(css_progdx pcval) {
  if(pcval > size-1)
    return 0;
  return insts[pcval]->line;
}

int cssProg::FindSrcLn(int ln) {
  int i;
  for(i = 0; i< src_size; i++) {
    if(source[i]->ln == ln)
      return i;
  }
  return -1;
}
int cssProg::ClosestSrcLn(int ln) {
  int i;
  for(i = 0; i< src_size; i++) {
    if(source[i]->ln >= ln)
      return i;
  }
  return -1;
}
// specifies a range
int cssProg::HasSrcLn(int st, int ed) {
  int i;
  for(i = 0; i< src_size; i++) {
    if((source[i]->ln >= st) && (source[i]->ln <= ed))
      return i;
  }
  return -1;
}

int cssProg::CurSrcCharsLeft() {
  if(line < src_size)
    return source[line]->src.length() - col;
  return 0;
}

// print any subroutines starting from sln
int cssProg::SubList(int sln, int eln, ostream& fh) {
  css_progdx ln = source[sln]->stpc;
  while((ln < size) && (source[insts[ln]->line]->ln < eln)) {
    if(insts[ln]->isdefn) {
      cssScriptFun* tmp = (cssScriptFun*)(insts[ln]->inst.El());
      cssProgSpace* old_top = tmp->fun->SetTop(top);
      int rval = tmp->fun->List(fh);
      tmp->fun->PopTop(old_top);
      if(rval) return 1;
      break;
    }
    ln++;
  }
  return 0;
}

int cssProg::List(ostream& fh, int st, int nlines) {
  top->list_ln = st;
  int cur_list_n = top->list_n;	// cache this..
  top->list_n = nlines;
  top->SetListStop();
  if(owner != NULL) {
    fh << owner->PrintStr() << " {\n";
  }
  else {
    fh << name << ":\n";
  }
  int retv = List(fh);
  top->list_n = cur_list_n;
  return retv;
}

int cssProg::List(ostream& fh) {
  int sln, i;

  if((sln = HasSrcLn(top->list_ln, top->lstop_ln)) < 0) {
    for(i=0; i < size; i++) {
      if(insts[i]->isdefn) {
	cssScriptFun* tmp = (cssScriptFun*)(insts[i]->inst.El());
	cssProgSpace* old_top = tmp->fun->SetTop(top);
	int rval = tmp->fun->List(fh);
	tmp->fun->PopTop(old_top);
	if(rval) return 1;
      }
    }
    return 0;
  }
  sln = ClosestSrcLn(top->list_ln);
  if((sln > 0) && (source[sln]->ln > top->list_ln)) {// print previous
    if(SubList(sln-1, source[sln]->ln, fh))
      return 1;
  }

  if(top->ListDebug() >= 2) {
    css_progdx ln = source[sln]->stpc;	// start at starting pc
    fh << "\nListing of Elements of Program: " << name << "\n";

    while((ln < size) && ((top->list_ln = Print(ln, fh)) <= top->lstop_ln)) {
      ln++;
    }
    fh << "\n";
  }
  else {
    while(sln < src_size) {
      if((source[sln]->src != "") && (source[sln]->ln != top->prev_ln))
	fh << source[sln]->ln << "\t" << source[sln]->src;
      top->list_ln = top->prev_ln = source[sln]->ln;
      if((sln >= src_size-1) || (top->list_ln > top->lstop_ln))
	break;
      if((source[sln+1]->ln - source[sln]->ln) > 1) { // must be something in there
	if(SubList(sln, source[sln+1]->ln, fh))
	  return 1;
      }
      sln++;
    }
  }
  if(top->list_ln >= top->lstop_ln)
    return 1;			// tell calling program to stop
  return 0;
}

void cssProg::ListSpace(ostream& fh, int frdx) {
  if(frdx < 0)
    frdx = fr_size-1;
  fh << "\nElements of Spaces For Program: " << name << " (frame = " << frdx  << ")\n";
  Autos(frdx)->List(fh);
  statics.List(fh);
  Stack(frdx)->List(fh);
  if(top->ListDebug() > 1)
    literals.List(fh);
}

void cssProg::SubPrint(css_progdx pcdx, ostream& fh) {
  if(top->run == cssEl::Running)
    insts[pcdx]->Print(fh);
  else
    insts[pcdx]->List(fh);
}


int cssProg::Print(css_progdx pcdx, ostream& fh) {
  int i, l_prev;
  pcdx = MIN(pcdx, size-1);
  cssListEl* src = source[insts[pcdx]->line];

  if((top->run == cssEl::Running) && (src->ln < top->list_ln))
    return top->list_ln;

  top->list_ln = src->ln;
  if(top->ListDebug() < 2) {
    if(!(insts[pcdx]->isdefn) && (top->prev_ln == top->list_ln))
      return top->list_ln;
    if(top->run != cssEl::Running) {
      for(i=top->prev_ln + 1; i < top->list_ln; i++) {
	if((l_prev = FindSrcLn(i)) >= 0)
	  fh << i << "\t" << GetSrcLC(l_prev);
      }
    }
    SubPrint(pcdx, fh);
  }
  else
    SubPrint(pcdx, fh);

  top->prev_ln = top->list_ln;
  fh.flush();
  return top->list_ln;
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
  el_retv = Autos()->FindName(it->name);
  if(it->name.empty() || (el_retv == 0))
    el_retv = Autos()->Push(it);
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

int cssProg::AddSrc(const char* ln) {
  if(src_size+1 >= src_alloc_size)
    AllocSrc(src_size+1);
  top->src_ln++;
  source[src_size++] = new cssListEl(size, top->src_ln, ln);
  return src_size-1;
}

int cssProg::Getc() {
  state &= ~State_WasBurped;
  // if possible, we return an unconsumed char already in input lines
  if ((line < src_size) && (col < (int)source[line]->src.length()))
    return (int) (source[line]->src[col++]);
    
  switch (top->GetInputMode()) {
  case cssProgSpace::IM_File:
    // if reading from file, get more input and try again
    if (ReadLn_File() == EOF)
      return EOF;
    break;
  case cssProgSpace::IM_Console:
    // if using push mode from a console, just tell caller no more text (now)
    return EOF;
  //default: NOTE: don't put a default, must handle all cases
  }
    
  // ok, presumably we got more text, so are now recursively calling ourself again
  return Getc();
}

/*obs
int cssProg::ReadLn() {
  if(top->external_exit)	// bail if external exit flag was set
    return EOF;
  if((top->InShell() || (top->state & cssProg::State_Defn)) &&
     (top->fin == top->fshell))
  {
    const char* pt;
    pt = (top->state & cssProg::State_Defn) ? "#" : ">";
    if(top->depth > 0)
      top->act_prompt = String(top->depth) + " " + top->prompt + pt + " ";
    else
      top->act_prompt = top->prompt + pt + " ";
    top->in_readline = true;
//obs    curln = rl_readline((char*)top->act_prompt);
    //NOTE: according to rl spec, we must call free() on the string returned
    char* curln_ = readline((char*)top->act_prompt);
    if (!curln_)
      return EOF;
    String curln(curln_);
    free(curln_); 
    curln_ = NULL;

    top->in_readline = false;

    if(top->external_exit) 	// bail if external exit flag was set during readline
      return EOF;

    line = AddSrc(curln);	// put onto buffer
    if(source[line]->src.length() > 0)
      add_history(curln);
  } else {
    int c;
    line = AddSrc("");			// new line

    while(((c = top->fin->get()) != EOF) && (c != '\n')) {
      source[line]->src += (char)c;	// add
    }
    if(c == EOF)
      return EOF;
  }
  source[line]->src += '\n';	// always end with a newline
  if(top->debug >= 3)
    cerr << "\nsrc ===> " << source[line]->src;	// get a source code trace here..
  col = 0;

  st_col = col;			// reset the st_lines..
  st_line = line;

  return 1;
}*/

int cssProg::ReadLn_File() {
  if(top->external_exit)	// bail if external exit flag was set
    return EOF;
    
  int c;
  line = AddSrc("");			// new line
  while (((c = top->fin->get()) != EOF) && (c != '\n')) {
    source[line]->src += (char)c;	// add
  }
//TODO: note: seems wrong to exit here with EOF, without doing rest of routine...
  if (c == EOF)
    return EOF;
  
  source[line]->src += '\n';	// always end with a newline
  if (top->debug >= 3)
    cerr << "\nsrc ===> " << source[line]->src;	// get a source code trace here..
  col = 0;

  st_col = col;			// reset the st_lines..
  st_line = line;

  return 1;
}

int cssProg::Code(cssEl* it) {
  if((cssMisc::code_cur_top != NULL) && (cssMisc::code_cur_top->Prog() != this))
    return cssMisc::code_cur_top->Prog()->Code(it);
  while(line >= src_size) AddSrc("\n");	  // in case we have no source..
  cssElPtr elp;
  elp.SetDirect(it);
  cssInst* tmp = new cssInst(*this, elp, line, col);
  return tmp->idx;
}
int cssProg::Code(cssElPtr& it) {
  if((cssMisc::code_cur_top != NULL) && (cssMisc::code_cur_top->Prog() != this))
    return cssMisc::code_cur_top->Prog()->Code(it);
  while(line >= src_size) AddSrc("\n");	  // in case we have no source..
  cssInst* tmp = new cssInst(*this, it, line, col);
  return tmp->idx;
}
int cssProg::Code(const char* nm) {
  if((cssMisc::code_cur_top != NULL) && (cssMisc::code_cur_top->Prog() != this))
    return cssMisc::code_cur_top->Prog()->Code(nm);
  cssElPtr tmp;
  if((tmp = top->FindName(nm)) == 0)
    cssMisc::Error(this, "Function/Variable Not Found:", nm);
  return Code(tmp);
}
int cssProg::Code(css_progdx it) {
  if((cssMisc::code_cur_top != NULL) && (cssMisc::code_cur_top->Prog() != this))
    return cssMisc::code_cur_top->Prog()->Code(it);
  while(line >= src_size) AddSrc("\n");	  // in case we have no source..
  cssIJump* tmp;
  tmp = new cssIJump(*this, it, line, col);
  return tmp->idx;
}

void cssProg::ZapFrom(int zp_size, int zp_src_size) {
  int i;
  for(i=zp_size; i<size; i++)
    delete insts[i];
  size = zp_size;
  for(i=zp_src_size; i<src_size; i++)
    delete source[i];
  src_size = zp_src_size;
}
void cssProg::ZapFromSrc(int zp_src_size) {
  int i;
  for(i=zp_src_size; i<src_size; i++)
    delete source[i];
  src_size = zp_src_size;
}

void cssProg::BurpSrc() {
  line = st_line;
  col = st_col;
  state |= State_WasBurped;
}

int cssProg::Undo(int srcln) {
  int srcdx, i, rval = -1;
  int diff;
  css_progdx bppc;

  if((srcdx = FindSrcLn(srcln)) < 0) {
    int i;
    cssScriptFun* tmp;

    for(i=0; i < size; i++) {
      if(insts[i]->isdefn) {
	tmp = (cssScriptFun*)(insts[i]->inst.El());
	if((rval = tmp->fun->Undo(srcln)) >= 0)
	  return rval;
      }
    }
    return -1;
  }
  bppc = source[srcdx]->stpc;
  for(i = bppc; i < size; i++) {
    if(source[insts[i]->line]->ln > srcln) {
      break;
    }
  }
  diff = i - bppc;		// amount to delete
  for(i=bppc; i < size-diff; i++) {
    delete insts[i];
    insts[i] = insts[i+diff];
  }
  size -= diff;
  src_size--;
  for(i=srcdx; i < src_size; i++) {
    delete source[i];
    source[i] = source[i+1];
  }
  return bppc;
}

cssElPtr& cssProg::FindAutoName(const char* nm) {	// lookup by name
  if((el_retv = Autos()->FindName(nm)) != 0)
    return el_retv;
  if((el_retv = statics.FindName(nm)) != 0)
    return el_retv;
  return cssMisc::VoidElPtr;
}


//////////////////////////////////////////
//	cssProg: Execution	 	//
//////////////////////////////////////////

void cssProg::SetPC(css_progdx npc) {
  Frame()->pc = MIN(npc, size);
  if((PC() < size) && (insts[PC()]->line < src_size))
    top->list_ln = source[insts[PC()]->line]->ln;
}

cssProg* cssProg::SetSrcPC(int srcln) {
  int srcdx;

  if((srcdx = FindSrcLn(srcln)) < 0) {
    int i;
    cssScriptFun* tmp;
    cssProg* rval;

    for(i=0; i < size; i++) {
      if(insts[i]->isdefn) {
	tmp = (cssScriptFun*)(insts[i]->inst.El());
	if((rval = tmp->fun->SetSrcPC(srcln)) != NULL)
	  return rval;
      }
    }
    return NULL;
  }
  SetPC(source[srcdx]->stpc);
  return this;
}

cssInst* cssProg::Next() {
  cssInst* rval = NULL;
  if(PC() >= size)
    top->run = cssEl::Stopping;
  else if(IsBreak(PC()))
    top->run = cssEl::BreakPoint;
  else
    rval = insts[Frame()->pc++];
  return rval;
}

// run and set status, returning value
cssEl* cssProg::Cont() {
  if(top->debug) top->prev_ln = -1;
  top->run = cssEl::Running;

  cssInst* nxt;
  if(PC() >= size) {
    top->run = cssEl::Stopping;
    return Stack()->Peek();
  }
  else nxt = insts[PC()];

  if(IsBreak(PC())) {			// step over bp
    Frame()->pc++;			// make the _peek a real next()
    if(top->debug) Print(nxt->idx, *(top->fout));
    top->run = nxt->Do();
  }

  if(top->step_mode) {
    int stc, lim;
    if(top->debug >= 2) {
      stc = 0;
      lim = top->step_mode;
    }
    else {
      stc = source[nxt->line]->ln;
      lim = stc + top->step_mode;
    }
    while((stc < lim) && (top->run == cssEl::Running) && (nxt = Next())) {
      Print(nxt->idx, *(top->fout));
      top->run = nxt->Do();
      if(top->debug < 2) {
	if(PC() >= size) {
	  top->run = cssEl::Stopping;
	  break;
	}
	nxt = insts[PC()];
	if(nxt == NULL)
	  stc = lim;
	else
	  stc = source[nxt->line]->ln;
      }
      else
	stc++;
    }
    return Stack()->Peek();
  }
  else {
    while((top->run == cssEl::Running) && (nxt = Next())) {
      if(top->debug) Print(nxt->idx, *(top->fout));
      top->run = nxt->Do();
    }
  }
  return Stack()->Peek();
}

cssEl* cssProg::ContSrc(int srcln) {
  cssProg* cp = SetSrcPC(srcln);
  if(cp == NULL)
    return &cssMisc::Void;

  return cp->Cont();
}

cssEl* cssProg::RunLast() {
  css_progdx oldpc = PC();
  top->old_debug = top->debug;
  if(top->debug > 0)
    top->debug = 1;			// shell debug
  else
    top->debug = 0;
  int old_state = top->state;
  top->run = cssEl::Waiting;
  top->state |= cssProg::State_RunLast;
  cssEl* rval;
  rval = top->Cont(run_st_size);
  SetPC(oldpc);			// save previous pc
  top->debug = top->old_debug;
  top->state = old_state;
  return rval;
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
int cssProg::SetBreak(int srcln) {
  int srcdx;
  if((srcdx = FindSrcLn(srcln)) < 0) {
    int i, rval;
    for(i=0; i < size; i++) {
      if(insts[i]->isdefn) {
	cssScriptFun* tmp = (cssScriptFun*)(insts[i]->inst.El());
	if((rval = tmp->fun->SetBreak(srcln)) >= 0)
	  return rval;
      }
    }
    return -1;
  }
  if(source[srcdx]->stpc == 0)
    breaks.Add(source[srcdx]->stpc + 1); // avoid the "1st instr" problem
  else
    breaks.Add(source[srcdx]->stpc);
  return breaks.Peek();
}

void cssProg::ShowBreaks(ostream& fh) {
  int i;
  cssScriptFun* tmp;

  for(i=0; i<breaks.size; i++) {
    fh << name << "\t";
    insts[breaks[i]]->Print(fh);
  }
  for(i=0; i < size; i++) {
    if(insts[i]->isdefn) {
      tmp = (cssScriptFun*)(insts[i]->inst.El());
      tmp->fun->ShowBreaks(fh);
    }
  }
}

bool cssProg::unSetBreak(int srcln) {
  int srcdx;
  if((srcdx = FindSrcLn(srcln)) < 0) {
    int i;
    for(i=0; i < size; i++) {
      if(insts[i]->isdefn) {
	cssScriptFun* tmp = (cssScriptFun*)(insts[i]->inst.El());
	if(tmp->fun->unSetBreak(srcln))
	  return true;
      }
    }
    return false;
  }
  css_progdx bppc = source[srcdx]->stpc;
  return breaks.Remove(bppc);
}


//////////////////////////////////////////////////
// 	cssProgSpace: Space of Programs		//
//////////////////////////////////////////////////

void cssProgSpace::Constr() {
  old_debug = 0;
  cont_pending = false;
  cont_here = -1;
  compile_ctrl = CC_None;
  cc_push_this = NULL;
  shell_cmds = SC_None;
  sc_undo_this = -1;
  sc_shell_this = NULL;

  old_src_ln = 0;
  init_depth = 0;
  old_state = 0;
  old_top = NULL;
  old_fh = NULL;
  old_prog = NULL;

  prompt = cssMisc::prompt;
  act_prompt = prompt;

  size = 0;
  progs = (cssProgStack**)calloc(alloc_size, sizeof(cssProgStack*));
  state = 0;
  depth = 0;
  fin = &cin;  fout = &cout;  ferr = &cerr; fshell = &cin;
  step_mode = 0;
  run = cssEl::Waiting;
  if(cssMisc::Top != NULL)
    debug = cssMisc::Top->debug;
  else
    debug = 0;
  external_exit = false;
  in_readline = false;

  src_ln = 0; st_src_ln = 0;
  list_ln = 0; list_n = 20;
  st_list_ln = 0;
  lstop_ln = 0; prev_ln = 0;
  in_shell = false;

  parsing_command = false;

  SetName(name);
  AddProg(new cssProg("Top Level"));
  Prog()->top = this;		// this one gets us as a top for sure
  Restart();
}

cssProgSpace::cssProgSpace() {
  alloc_size = 2;
  Constr();
}

cssProgSpace::cssProgSpace(const char* nm) {
  alloc_size = 2;
  name = nm;
  Constr();
}

cssProgSpace::~cssProgSpace() {
  Reset();
  DelProg();
  free(progs);
}

void cssProgSpace::AcceptNewLine(QString ln, bool eof) {
  CompileCode(ln);

  int rval = CompileLn(*fshell);

  if(external_exit || ShellCmdPending() || (rval == cssProg::YY_Exit)) {
    step_mode = 0;
    return;
  }
  // if in step mode and blank line, exit shell (and continue, presumably)
  if((step_mode > 0) && (rval == cssProg::YY_Blank))
    return;

  if(Prog()->CurSrcCharsLeft() > 0)
    return;             // more source remains to be parsed (multi statements)
  if(depth > init_depth) {
    if(cssMisc::cur_top->debug > 0)
      cerr << "<parsing more (depth)>\n";
    return;		// this will parse more if you put brackets in..
  }
  if(cssMisc::cur_top->debug > 0)
    cerr << "<running last statements..>\n";
  Prog()->RunLast();
  Prog()->ZapLastRun();
  RestoreListStart(old_src_ln);

    // if continue is pending, exit shell (and continue)
//    if(cont_pending)
//      break;
}

bool cssProgSpace::DeleteOk() {
  if(in_readline)	return false; // don't delete when in readline..
  return true;
}

void cssProgSpace::DeferredDelete() {
  ExitShell();
  cssMisc::delete_me = this;
}

void cssProgSpace::ExitShell() {
  if(in_readline) {
    cssMisc::Warning(Prog(), "Exiting shell by external command");
  }
  rl_done = true;		// finish readline..
  rl_stuff_char('\n');
  rl_stuff_char(EOF);
  rl_stuff_char('\n');
  external_exit = true;		// force exit
  SetRestart();			// set the restart flag to occur as we bail
  run = cssEl::Bailing;		// bail
}

//////////////////////////////////////////////////
// 	cssProgSpace: Internal, Programs	//
//////////////////////////////////////////////////

void cssProgSpace::SetName(const char* nm) {
  name = nm;
  prog_vars.name = name + ".prog_vars";
  hard_vars.name = name + ".hard_vars";
  hard_funs.name = name + ".hard_funs";
  statics.name = name + ".statics";
  types.name = name + ".types";
}

void cssProgSpace::Reset() {
#ifdef TA_GUI
  cssiSession::CancelProgEdits(this);
#endif // TA_GUI
  Restart();
  Prog()->Reset();		// reset our top-level guy
  Prog()->top = this;		// this one gets us as a top for sure
  src_ln = 0;   st_src_ln = 0;
  list_ln = 0; list_n = 20;
  st_list_ln = 0;
  lstop_ln = 0; prev_ln = 0;
  parsing_command = false;
  run = cssEl::Waiting;
  external_exit = false;
  ClearShellCmds();
  //TODO: might have to reset some of the new PushNewShell/PopShell vars
}

void cssProgSpace::ClearAll() {
#ifdef TA_GUI
  cssiSession::CancelProgEdits(this);
#endif // TA_GUI
  Reset();
  statics.Reset();
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
  cssProg* prv = Prog();
  AddProg(it);
  it->top = this;		// set top to be this
  src_ln--;			// isn't really a new line..
  Prog()->line = Prog()->AddSrc(prv->GetSrc());
  Prog()->col = 0;
  if(prv->state & cssProg::State_WasBurped) {
    if(prv->col == 0) {			// new line here
      prv->src_size--;			// undo last add
      delete prv->source[prv->src_size];	// get rid of the list el
      prv->line--;
    }
    prv->state &= ~cssProg::State_WasBurped;
  }
  prv->col = strlen(prv->GetSrcLC(prv->line)); // put col at end of cur line
  depth++;
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
    if(strcmp(prv->GetSrc(), "") != 0) {
      src_ln--;		// isn't really a new line..
      Prog()->line = Prog()->AddSrc(prv->GetSrc());
      Prog()->col = 0;
      prv->col = strlen(prv->GetSrcLC(prv->line));
    }
    depth--;
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
    if((pg->owner == NULL) || (pg->owner->is_block))
      continue;
    return pg->owner;
  }
  return NULL;
}

cssProgSpace::InputMode cssProgSpace::GetInputMode() const {
  if ((InShell() || (state & cssProg::State_Defn)) &&
     (fin == fshell))
    return IM_Console;
  else return IM_File;
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
cssElPtr& cssProgSpace::AddPtrType(cssEl* base_type) {
  // first try to find pre-existing on appropriate type space
  cssSpace* tp_spc = &cssMisc::TypesSpace;
  if((cssMisc::cur_class != NULL) &&
     (cssMisc::cur_class->types->GetIndex(base_type) >= 0))
    tp_spc = cssMisc::cur_class->types;

  String nm = base_type->name + "_ptr";
  int i;
  for(i=0; i<tp_spc->size; i++) {
    cssEl* tp = tp_spc->FastEl(i);
    if(tp->GetType() != cssEl::T_Ptr) continue;
    cssPtr* ptr = (cssPtr*)tp;
    if((ptr->el_type == base_type) && (ptr->name == nm)) {
      if(debug == 0)
	el_retv.SetDirect(tp);	// go direct
      else
	el_retv.SetSpace(tp_spc, i);
      return el_retv;
    }
  }
  // didn't find it..

  cssPtr* nwtp = new cssPtr(base_type, (const char*)nm);
  el_retv = tp_spc->Push(nwtp);
  if(debug == 0)
    el_retv.SetDirect(nwtp);
  return el_retv;
}

cssElPtr& cssProgSpace::AddRefType(cssEl* base_type) {
  // first try to find pre-existing on appropriate type space
  cssSpace* tp_spc = &cssMisc::TypesSpace;
  if((cssMisc::cur_class != NULL) &&
     (cssMisc::cur_class->types->GetIndex(base_type) >= 0))
    tp_spc = cssMisc::cur_class->types;

  String nm = base_type->name + "_ref";
  int i;
  for(i=0; i<tp_spc->size; i++) {
    cssEl* tp = tp_spc->FastEl(i);
    if(!tp->IsRef()) continue;
    cssRef* ptr = (cssRef*)tp;
    if((ptr->ptr.El() == base_type) && (ptr->name == nm)) {
      if(debug == 0)
	el_retv.SetDirect(tp);	// go direct
      else
	el_retv.SetSpace(tp_spc, i);
      return el_retv;
    }
  }
  // didn't find it..

  cssRef* nwtp = new cssRef(base_type, (const char*)nm);
  el_retv = tp_spc->Push(nwtp);
  if(debug == 0)
    el_retv.SetDirect(nwtp);
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
    if(Prog()->Autos()->Replace(old, nw))  return true;
    if(Prog()->statics.Replace(old, nw))   return true;
  }
  if(statics.Replace(old, nw))	return true;
  if(cssMisc::Constants.Replace(old, nw)) return true;
  if(cssMisc::Externs.Replace(old, nw)) return true;
  return false;
}
bool cssProgSpace::RemoveVar(cssEl* old) {
  if(size > 1) {
    if(Prog()->Autos()->Remove(old))	return true;
    if(Prog()->statics.Remove(old))	return true;
  }
  if(statics.Remove(old))	return true;
  if(cssMisc::Constants.Remove(old)) return true;
  if(cssMisc::Externs.Remove(old)) return true;
  return false;
}

cssElPtr& cssProgSpace::FindName(const char* nm) {	// lookup by name
  int i;
  for(i=size-1; i>= 0; i--) {
    if((el_retv = Prog(i)->FindAutoName(nm)) != 0)
      return el_retv;
  }
  if ((el_retv = statics.FindName(nm)) != 0)
    return el_retv;
  if ((el_retv = hard_funs.FindName(nm)) != 0)
    return el_retv;
  if ((el_retv = hard_vars.FindName(nm)) != 0)
    return el_retv;
  return prog_vars.FindName(nm);
}

// idx is an arbitrary number, returns NULL when no more values
cssSpace* cssProgSpace::GetParseSpace(int idx) {
  static int max_prog;		// highest prog to search back to
  int dynamics = (2 * size) + 2; // number of "dynamic" spaces (changes based on size)
  int after_class = 2;
  if(cssMisc::cur_class != NULL) { // two more spaces to check..
    dynamics += 2;
    after_class += 2;
  }

  if(idx == 0)
    return &cssMisc::Defines; 	// #define is uber alles
  else if(idx == 1)
    return &cssMisc::Parse;	// first parsed stuff
  else if((cssMisc::cur_class != NULL) && (idx == 2))
    return cssMisc::cur_class->members;
  else if((cssMisc::cur_class != NULL) && (idx == 3))
    return cssMisc::cur_class->methods;
  else if(idx == after_class) {		// first auto block, also set max_prog
    int i;
    for(i=size-1; i>0; i--) {
      max_prog = i;
      // go up through the blocks, but stop once we get to non-blocks..
      if(!((Prog(i)->owner != NULL) && (Prog(i)->owner->is_block)))
	break;
    }
    return Prog(size-1)->Autos();
  }
  else if(idx == after_class+1)
    return &(Prog(size-1)->statics);
  else if((idx >= after_class+2) && (idx < dynamics)) {	// 0 - size-1 for progs, 2 per
    int stat_auto = (idx - (after_class+2)) % 2;		// 0 is auto, 1 is stat
    int prog_idx = size - 1 - ((idx - (after_class+2)) / 2);	// index of prog (from end)
    if(prog_idx < max_prog)
      return &cssMisc::VoidSpace; 		// don't search in these spaces..
    if(stat_auto)
      return Prog(prog_idx)->Autos();
    else
      return &(Prog(prog_idx)->statics);
  }
  else if(idx == dynamics)	// local to prog space
    return &statics;
  else if(idx == dynamics+1)
    return &hard_funs;
  else if(idx == dynamics+2)
    return &hard_vars;
  else if(idx == dynamics+3)	// global variables
    return &cssMisc::Externs;
  else if(idx == dynamics+4)
    return &cssMisc::HardFuns;
  else if(idx == dynamics+5)
    return &cssMisc::HardVars;
  else if(idx == dynamics+6)
    return &cssMisc::Commands;
  else if(idx == dynamics+7)
    return &cssMisc::Functions;
  else if(idx == dynamics+8)
    return &cssMisc::Constants;
  else if(idx == dynamics+9)
    return &cssMisc::Enums;
  else if(idx == dynamics+10)
    return &cssMisc::Settings;
  else if(idx == dynamics+11)
    return &prog_vars;
  else
    return NULL;
}

cssElPtr& cssProgSpace::ParseName(const char* nm) {
  int i = 0;
  cssSpace* spc=NULL;
  while((spc = GetParseSpace(i++)) != NULL) {
    if((el_retv = spc->FindName(nm)) != 0) {
      if(spc == &(cssMisc::Commands))
	parsing_command = true;
      else if((cssMisc::cur_class != NULL) && (spc == cssMisc::cur_class->methods)
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

cssElPtr& cssProgSpace::FindTypeName(const char* nm) {
  cssElPtr& tp_ptr = types.FindName(nm);
  if(tp_ptr != 0)
    return tp_ptr;
  return cssMisc::TypesSpace.FindName(nm);
}

//////////////////////////////////////////////////
// 	cssProgSpace: Compiling			//
//////////////////////////////////////////////////

int cssProgSpace::GetFile(fstream& fh, const char* fname) {
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
  for(i=0; i<taMisc::include_paths.size; i++) {
    String trynm = taMisc::include_paths.FastEl(i) + "/" + fname;
    fh.open(trynm, ios::in);
    if(fh.good())
      return true;
    fh.close(); fh.clear();

    trynm = taMisc::include_paths.FastEl(i) + "/" + fname + ".css";
    fh.open(trynm, ios::in);
    if(fh.good())
      return true;
    fh.close(); fh.clear();
  }
  fh.close(); fh.clear();
  return false;
}

// this compiles one line of code, does not allow for run-last type shell execution
int cssProgSpace::CompileLn(istream&, bool* err) {
  Prog()->MarkParseStart();
  int old_src_ln = MarkListStart();
  if(step_mode > 0)		// this is needed to get blank lines
    parsing_command = true;

  int retval = cssProg::YY_Ok;
  do {
    ResetParseFlags();
    retval = yyparse();		// parse current line
  } while(retval == cssProg::YY_Parse);

  parsing_command = false;

  if (retval == cssProg::YY_Err) { // remove source code associated with errors
    if (err) *err = true;
    RestoreListStart(old_src_ln);
    Prog()->ZapLastParse();
  }
  else
    DoCompileCtrl();		// do any compile control things necessary

  return retval;
}

// this just does compiling, no shell-style execution
bool cssProgSpace::Compile(istream& fh) {
  bool err = false;
  istream* old_fh = fin;
  fin = &fh;
  cssProgSpace* old_top = cssMisc::SetCurTop(this);
  int old_state = state;
  state &= ~cssProg::State_Shell;
  external_exit = false;	// don't exit!

  while ((CompileLn(fh, &err) != cssProg::YY_Exit) && !external_exit);

  state = old_state;
  fin = old_fh;
  cssMisc::PopCurTop(old_top);
  return !err;
}

bool cssProgSpace::Compile(const char* fname) {
  bool rval = false;
  String fnm = fname;
  if (fnm == "-") {
    rval = Compile(*fin);
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
      taMisc::include_paths.AddUnique(dir);
    SetName(fname);
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

void cssProgSpace::Include(const char* fname) {
  String cur_name = name;
  int cur_prog_size = Prog()->size;
  int cur_prog_src_size = Prog()->src_size;
  int cur_list_ln = list_ln;
  int cur_src_ln = src_ln;

  Compile(fname);

  SetName(cur_name);
  Prog()->ZapFrom(cur_prog_size, cur_prog_src_size);
  src_ln = cur_src_ln;
  list_ln = cur_list_ln;
}

void cssProgSpace::CompileRunClear(const char* fname) {
  Compile(fname);
  Run();
  ClearAll();
}

void cssProgSpace::reCompile() {
  ClearAll();
  Compile((const char*)name);
}

void cssProgSpace::Undo(int st) {
  if(Prog(0)->Undo(st) < 0)
    yyerror("Source line not found");
}

void cssProgSpace::ResetParseFlags() {
  parsing_command = false;
  cssMisc::CodeTop();
  cssMisc::ConstExprTop->Reset();
  cssMisc::parsing_args = false;
  cssMisc::parsing_membdefn = false;
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

//////////////////////////////////////////////////
// 	cssProgSpace:    Execution 		//
//////////////////////////////////////////////////

void cssProgSpace::Restart() {
  while(size > 1) DelProg();
  list_ln = 0;  prev_ln = -1;
  depth = 0;
  Prog()->Restart();
}

cssEl* cssProgSpace::Cont() {
  // things that happen in a setjmp environment need to be volatile..
#ifdef HAVE_VOLATILE
  volatile cssEl* rval = &cssMisc::Void;
  volatile cssProgSpace* ths = this; // use this ths for anything post-setjmp
#else
  cssEl* rval = &cssMisc::Void;
  cssProgSpace* ths = this; // use this ths for anything post-setjmp
#endif

  cssProgSpace* old_top = cssMisc::SetCurTop(this);

  run = cssEl::Running;
  state |= cssProg::State_Cont;
  depth++;
  while(!ths->external_exit && !ths->ShellCmdPending()) {
    setjmp(cssMisc::begin);

    if(ths->external_exit || ths->ShellCmdPending()) {
      cssMisc::PopCurTop(old_top);
      return &cssMisc::Void;
    }

    if(ths->run != cssEl::ExecError) {
      rval = ths->Prog()->Cont();
    }

    if(ths->state & cssProg::State_RunLast) { // don't do anything with runlast...
      if(ths->run == cssEl::BreakPoint)
	cssMisc::Warning(ths->Prog(), "breakpoints don't work in immediate-mode shell");
      break;
    }

    // if last cont got a stop, stop
    if(ths->run == cssEl::Stopping)
      break;

    if((ths->run == cssEl::ExecError) || (ths->run == cssEl::BreakPoint) ||
       (ths->step_mode > 0))
    {
cerr << "cssProgSpace::Cont -- attempt to run Shell not supported!\n";
//TODO: fixup
//      ths->Shell(*(ths->fshell));
    }

    // allow for things inside the shell to have led to another exit condition
    if(ths->external_exit || ths->ShellCmdPending()) {
      cssMisc::PopCurTop(old_top);
      return &cssMisc::Void;
    }

    if(cont_pending) {
      cont_pending = false;	// reset flag
      if(ths->debug < 2)
	ths->Prog()->EndRunPop();	// get rid of last result for continue
      if(cont_here >= 0) {
	cssProg* cp = ths->Prog()->SetSrcPC(cont_here);
	if(cp == NULL) {
	  cssMisc::Warning(ths->Prog(), "source code line:",String(cont_here),
			   "not found for continue");
	  break;
	}
	if(cp != Prog())
	  Shove(cp);
      }
      continue;			// this will continue execution next time around
    }
    if(ths->step_mode > 0) {
      if(ths->debug < 2)
	ths->Prog()->EndRunPop();	// get rid of last result for continue
      continue;
    }

    break;			// otherwise, quit
  }

  ths->state &= ~cssProg::State_Cont;
  ths->depth--;
  cssMisc::PopCurTop(old_top);
  return (cssEl*)rval;
}

cssEl* cssProgSpace::Cont(css_progdx st) {
  Prog()->SetPC(st);
  return Cont();
}

cssEl* cssProgSpace::ContSrc(int srcln) {
  cssProg* cp = Prog()->SetSrcPC(srcln);
  if(cp == NULL)
    return &cssMisc::Void;
  if(cp != Prog())
    Shove(cp);
  return Cont();
}

cssEl* cssProgSpace::Run() {
  cssProgSpace* old_top = cssMisc::SetCurTop(this);
  state |= cssProg::State_Run;
  Restart();
  external_exit = false; // NOTE: added 4/11/06 to support Stop()
  cssEl* rval = Cont();
  state &= ~cssProg::State_Run;
  cssMisc::PopCurTop(old_top);
  return rval;
}

void cssProgSpace::Stop() {
//NOTE: added 4/11/06 to support stopping when a program call fails
// this implementation may have side-effects, and needs to be validated
  external_exit = true;
}

//////////////////////////////////////////////////
// 	cssProgSpace:   Shell Commands		//
//////////////////////////////////////////////////


void cssProgSpace::PushNewShell(istream& fh) {
  old_fh = fin;
  fin = &fh;
  old_top = cssMisc::SetCurTop(this);
  old_state = state;
  old_prog = Prog(); //TODO: could anything delete this prior to PopShell???
  if (old_prog) {
    old_prog->SaveStack();	// save the stack at this point
  }

  Prog()->MarkRunStart();	// runlast starts here
  old_src_ln = MarkListStart();
  init_depth = depth;

  run = cssEl::Waiting;
  state = cssProg::State_Shell;
}

void cssProgSpace::PopShell() {
  if (old_prog && (Prog() == old_prog)) {
    old_prog->ReloadStack();
    old_prog = NULL;
  }
  state = old_state;
  fin = old_fh;
  cssMisc::PopCurTop(old_top);
}


// this is the top-level control point for a shell
void cssProgSpace::CtrlShell(istream& fhi, ostream& fho, const char* prmpt) {
  // process delayed deletes upon entering new shell

  return; // not doing this now..

  if((cssMisc::delete_me != NULL) && (cssMisc::delete_me != this)) {
    delete cssMisc::delete_me;
    cssMisc::delete_me = NULL;
  }

  fshell = &fhi;
  fout = &fho;
  external_exit = false;	// don't exit yet!

  cssProgSpace* old_top = cssMisc::SetCurTop(this);

  if(cssMisc::init_debug >= 0) {
    SetDebug(cssMisc::init_debug);
  }

  if(prmpt != NULL)
    prompt = prmpt;
  else
    prompt = cssMisc::prompt;

  if(prompt.contains('/'))
    prompt = prompt.after('/', -1);
  if(prompt.contains('\\')) //windows
    prompt = prompt.after('\\', -1);
  if(prompt.contains(".css"))
    prompt = prompt.before(".css");

  while(!external_exit) {
//TODO: no longer exists    Shell(*fshell);
    if(DoShellCmd())		// we just popped out to do a command, continue
      continue;
    if(external_exit)
      break;
    if(cont_pending) {		// we did a continue when not running!
      cssMisc::Warning(NULL, "continue executed while not running, resuming shell");
      cont_pending = false;
      continue;
    }
    if(step_mode > 0)
      continue;
//obs: bad ui to ask confirmation for exit
//    char* surep;
//    if(!(surep = rl_readline("Quit, Are You Sure (y/n)? ")) || (*surep == 'y') ||
//       (*surep == 'Y'))
//      qApp->quit();
      break;
  }

  external_exit = false;
  in_readline = false;
  cssMisc::PopCurTop(old_top);
}

bool cssProgSpace::InShell() const { 
  //was, 3.2: return (state & cssProg::State_Shell);
  return in_shell; 
}

void cssProgSpace::StartupShellInit(istream& fhi, ostream& fho) {
  fshell = &fhi;
  fout = &fho;
  cssProgSpace* old_top = cssMisc::SetCurTop(this);

#ifndef __GNUG__
  fout->sync_with_stdio();
#endif
  if(cssMisc::init_debug >= 0) {
    SetDebug(cssMisc::init_debug);
  }

  // startup file
  CompileRunClear(".cssinitrc");

  SetName(cssMisc::prompt);
#if (!defined(TA_OS_WIN))
  signal(SIGFPE, (SIGNAL_PROC_FUN_TYPE) cssMisc::fpecatch);
  signal(SIGTRAP, (SIGNAL_PROC_FUN_TYPE) cssMisc::fpecatch);
  signal(SIGINT, (SIGNAL_PROC_FUN_TYPE) cssMisc::intrcatch);
#endif
  bool run_flag = false;

  if(cssMisc::startup_file != "") {
    Reset();
    Compile((const char*)cssMisc::startup_file);
    run_flag = true;
  }
  if(cssMisc::init_bpoint >= 0) {
    SetBreak(cssMisc::init_bpoint);
  }

  prompt = cssMisc::prompt;

  if(run_flag) {
    state |= cssProg::State_Shell;
    Run();
    EndRunPop();
    state &= ~cssProg::State_Shell;
  }

  // allow both startup_file and startup_code to co-exist..
  if (cssMisc::startup_code != "") {
    Reset();
    CompileCode(cssMisc::startup_code);
    state |= cssProg::State_Shell;
    Run();
    EndRunPop();
    state &= ~cssProg::State_Shell;
    run_flag = true;
  }

/*  if(!run_flag || cssMisc::init_interactive)
    CtrlShell(*fshell, *fout);

  cssMisc::PopCurTop(old_top);*/

  PushNewShell(*fshell);
  // todo: this shell is never popped!  needs to be added to quit routine
  // also, this is not the ctrl shell that is being pushed, just a plain shell
  
//TODO: stuff taken from CrtlShell that is probably important...
  prompt = cssMisc::prompt;
//end stuff taken  
  
  // connect us to the console
  if (cssMisc::console) {
    cssMisc::console->setPrompt(prompt);
    //NB: we must use queued connection, because console lives in our thread,
    // but signal may be raised in another thread
    connect(cssMisc::console, SIGNAL(NewLine(QString, bool)),
      this, SLOT(AcceptNewLine(QString, bool)), Qt::QueuedConnection);
    in_shell = true;
    cssMisc::console->Start();
  } 
#ifdef DEBUG
  //TODO: should warn that console was expected
#endif

/*TODO: More things to do:
  *now that input is async, the prompt will be wrong when it changes (since it will
    already have been output as the old value)
*/
}

void cssProgSpace::Source(const char* fname) {
cerr << "cssProgSpace::Source is currently not implemented\n";
return;
//TODO: need to fix to get Shell back again
  String fnm = fname;
  if (fnm == "-") {
//    Shell(*fin);
  } else {
    fstream fh;
    if(!GetFile(fh, fname)) {
      cssMisc::Warning(Prog(), "File Not Found:",fname);
      return;
    }
    // make sure directory is in include path
    String dir = fnm.before('/', -1);
    if(!dir.empty())
      taMisc::include_paths.AddUnique(dir);
//    Shell(fh);
    fh.close(); fh.clear();
  }
}

bool cssProgSpace::DoShellCmd() {
  switch(shell_cmds) {
  case SC_None:
    return false;
  case SC_Compile:
    ClearShellCmds();
    ClearAll();
    Compile((const char*)sc_compile_this);
    break;
  case SC_reCompile:
    ClearShellCmds();
    reCompile();
    break;
  case SC_Source:
    ClearShellCmds();
    Source(sc_compile_this);
    break;
  case SC_Defn:
    ClearShellCmds();
    state |= cssProg::State_Defn;
    state |= cssProg::State_Shell;
    Compile(*fshell);
    state &= ~cssProg::State_Defn;
    state &= ~cssProg::State_Shell;
    break;
  case SC_Shell:
    ClearShellCmds();
    if(sc_shell_this->name != "")
      sc_shell_this->CtrlShell(*fin, *fout, sc_shell_this->name);
    else
      sc_shell_this->CtrlShell(*fin, *fout);
    break;
  case SC_Run:
    ClearShellCmds();
    Run();
    EndRunPop();
    break;
  case SC_Restart:
    ClearShellCmds();
    Restart();
    break;
  case SC_Reset:
    ClearShellCmds();
    Reset();
    break;
  case SC_ClearAll:
    ClearShellCmds();
    ClearAll();
    break;
  case SC_Undo:
    ClearShellCmds();
    Undo(sc_undo_this);
    break;
  }
  return true;
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

void cssProgSpace::SetListStop() {
  if(debug >= 2)
    lstop_ln = list_ln + (list_n / 6); // each line = ~6 machine instr..
  else
    lstop_ln = list_ln + list_n;
}

void cssProgSpace::List(int st) {
  list_ln = st;
  List();
}

void cssProgSpace::List(int st, int nlines) {
  list_ln = st;
  list_n = nlines;
  List();
}

void cssProgSpace::List() {
  lstop_ln = MIN(lstop_ln, src_ln);
  list_ln = MIN(list_ln, src_ln);
  prev_ln = -1;			// reset
  SetListStop();
  *fout << "\nListing of Program: " << name << "\n";
  Prog(0)->List(*fout);
  st_list_ln = list_ln;		// reset saved list_ln
  fout->flush();
}

void cssProgSpace::ListSpace() {
  int i;
  *fout << "\nListing of Elements of: " << name << "\n";
  statics.List(*fout);
  for(i=0; i<size; i++) {
    Prog(i)->ListSpace(*fout, Prog_Fr(i));
    *fout << "\n";
  }
  fout->flush();
}

static char* rs_vals[9] = {"Waiting", "Running", "Stopping", "Returning",
			   "Breaking", "Continuing", "BreakPoint",
			   "ExecError", "Bailing"};

static char* sc_vals[11] = {"None", "Compile", "reCompile", "Source",
			   "Defn", "Shell", "Run", "Restart", "Reset",
			   "ClearAll", "Undo"};

void cssProgSpace::Status() {
  *fout << "\n\tStatus of Program: " << name << "\n";

  *fout << "curnt:\t" << Prog()->name << "\tsrc_ln:\t" << Prog()->CurSrcLn()
    << "\tpc:\t" << Prog()->PC() << "\n";
  *fout << "debug:\t" << debug << "\tstep:\t" << step_mode
    << "\tdepth:\t" << size << "\tconts:\t" << depth << "\n";
  *fout << "lines:\t" << src_ln << "\tlist:\t" << list_ln << "\n";

  int shstat = (state & cssProg::State_Shell) ? 1 : 0;
  int rstat = (state & cssProg::State_Run) ? 1 : 0;
  int cstat = (state & cssProg::State_Cont) ? 1 : 0;
  int dstat = (state & cssProg::State_Defn) ? 1 : 0;
  int rlstat = (state & cssProg::State_RunLast) ? 1 : 0;
  *fout << "State: shell:\t" << shstat << "\trun:\t" << rstat << "\tcont:\t" << cstat
        << "\tdefn:\t" << dstat << "\trunlast: " << rlstat << "\n";

  *fout << "run status:\t" << rs_vals[run] << "\n";
  *fout << "shell cmd:\t" << sc_vals[shell_cmds] << "\n";

  fout->flush();
}

void cssProgSpace::Trace(int level) {
  int i;
  *fout << "\n\tTrace of Program: " << name << "\n";

  for(i=0; i<size; i++) {
    *fout << i << ":\t" << Prog(i)->name << "\t";
    if(Prog(i)->Inst(Prog(i)->PC(Prog_Fr(i))))
      Prog(i)->Inst(Prog(i)->PC(Prog_Fr(i)))->Print(*fout);
    else
      *fout << "\n";
    if(level > 0) {
      Prog(i)->Stack(Prog_Fr(i))->List(*fout);
      if(level > 1) {
	Prog(i)->Autos(Prog_Fr(i))->List(*fout);
      }
    }
  }
  fout->flush();
}

void cssProgSpace::Help() {
  *fout << "\nC^c syntax is a subset of C++, with standard C math and stdio functions.\n\
Except: The (f)printf functions take arguments which print themselves\n\
\tprintf(\"varname:\\t\",avar,\"\\tvar2:\\t\",var2,\"\\n\"\n\
and a special String type is available for strings (ala C++)\n";

  *fout << "\nArguments interpreted by C^c are:\n\
[-f|-file] <file>\tcompile and execute given file upon startup, exit (unless -i)\n\
[-e|-exec] <code>\tcompile and execute given code upon startup, exit (unless -i)\n\
[-i|-interactive]\tif using -f or -e, go into interactive (prompt) mode after\n\
-v[<number>]     \trun with debug level set to given number (default 1)\n\
[-b|-bp] <line>  \tset initial breakpoint at given line of code (if using -f)\n\n\
Any other arguments can be accessed by user script programs by the global\n\
variables argv (an array of strings) and argc (an int)\n";

  *fout << "\nDo help <expr> to obtain more detailed help on functions, objects, etc.\n";

  *fout << "\nThe following functions and debugging & control commands are available\n";
  cssMisc::Functions.NameList(*fout);
  cssMisc::Commands.NameList(*fout);
  *fout << "\n ...and the following hard-coded functions are available\n";
  hard_funs.NameList(*fout);
  cssMisc::HardFuns.NameList(*fout);
  *fout << "\n ...and the following Program variables are available\n";
  prog_vars.NameList(*fout);
  *fout << "\n ...and the following hard-coded variables are available\n";
  hard_vars.NameList(*fout);
  cssMisc::HardVars.NameList(*fout);
  *fout << "\n";
  fout->flush();
}

//////////////////////////////////////////////////
// 	cssProgSpace:    Breakpoints 		//
//////////////////////////////////////////////////

void cssProgSpace::SetBreak(int srcln) {
  if(srcln > src_ln) {
    cssMisc::Warning(Prog(), "Breakpoint larger than max line number");
    return;
  }

  if(Prog(0)->SetBreak(srcln) < 0)
    cssMisc::Warning(Prog(), "Source line not found");
}

void cssProgSpace::ShowBreaks() {
  Prog(0)->ShowBreaks(*fout);
}

void cssProgSpace::unSetBreak(int srcln) {
  if(srcln > src_ln) {
    cssMisc::Warning(Prog(), "Breakpoint larger than max line number");
    return;
  }
  if(!Prog(0)->unSetBreak(srcln))
    cssMisc::Warning(Prog(), "Breakpoint not found");
}

