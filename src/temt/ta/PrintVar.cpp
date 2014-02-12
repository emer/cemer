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

#include "PrintVar.h"
#include <Program>
#include <ProgVar>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(PrintVar);


void PrintVar::Initialize() {
}

void PrintVar::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  Program* my_prog = program();
  if(!my_prog) return;
  if(!debug_level) {
    debug_level = my_prog->vars.FindName("debug_level");
  }
}

void PrintVar::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  //  CheckError(!print_var, quiet, rval, "print_var is NULL");
}

void PrintVar::GenCssBody_impl(Program* prog) {
  if(message.empty() && !print_var && !print_var2 && !print_var3 && !print_var4 && !print_var5 && !print_var6)
    return;

  String rval = "cout ";
  if(message.nonempty())
    rval += "<< \"" + message + "\"";
  if((bool)print_var)
    rval += "<< \"  " + print_var->name + " = \" << " + print_var->name;
  if((bool)print_var2)
    rval += "<< \"  " + print_var2->name + " = \" << " + print_var2->name;
  if((bool)print_var3)
    rval += "<< \"  " + print_var3->name + " = \" << " + print_var3->name;
  if((bool)print_var4)
    rval += "<< \"  " + print_var4->name + " = \" << " + print_var4->name;
  if((bool)print_var5)
    rval += "<< \"  " + print_var5->name + " = \" << " + print_var5->name;
  if((bool)print_var6)
    rval += "<< \"  " + print_var6->name + " = \" << " + print_var6->name;
  rval += " << endl;";

  if(my_mask && debug_level) {
    prog->AddLine(this, "if(" + my_mask->name + " & " + debug_level->name + ") {");
    prog->IncIndent();
    prog->AddLine(this, rval, ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
    prog->DecIndent();
    prog->AddLine(this, "}");
  }
  else {
    prog->AddLine(this, rval, ProgLine::MAIN_LINE);
    prog->AddVerboseLine(this);
  }
}

String PrintVar::GetDisplayName() const {
  String rval = "Print:";
  if(message.nonempty())
    rval += " " + message;
  if(print_var)
    rval += " " + print_var->name;
  if((bool)print_var2)
    rval += " " + print_var2->name;
  if((bool)print_var3)
    rval += " " + print_var3->name;
  if((bool)print_var4)
    rval += " " + print_var4->name;
  if((bool)print_var5)
    rval += " " + print_var5->name;
  if((bool)print_var6)
    rval += " " + print_var6->name;

  if(debug_level && my_mask) {
    rval += " (dbg mask: " + my_mask->name + ")";
  }
  return rval;
}

bool PrintVar::CanCvtFmCode(const String& code, ProgEl* scope_el) const {
  if(!(code.startsWith("print ") || code.startsWith("print: ") ||
       code.startsWith("cerr << ") || code.startsWith("cout << ")))
    return false;
  String exprstr;
  if(code.startsWith("print ")) exprstr = trim(code.after("print "));
  else if(code.startsWith("print: ")) exprstr = trim(code.after("print: "));
  else if(code.startsWith("cerr << ")) exprstr = trim(code.after("cerr << "));
  else if(code.startsWith("cout << ")) exprstr = trim(code.after("cout << "));
  if(exprstr.contains('"')) {
    if(exprstr.freq('"') != 2) return false; // only one message
    exprstr = trim(exprstr.after('"',-1));
    if(exprstr.contains(',')) exprstr = trim(exprstr.after(','));
    if(exprstr.contains("<<")) exprstr = trim(exprstr.after("<<"));
  }
  String varnm = exprstr;
  if(exprstr.contains(',')) varnm = trim(exprstr.before(','));
  if(exprstr.contains("<<")) varnm = trim(exprstr.before("<<"));
  if(exprstr.contains(' ')) varnm = trim(exprstr.before(' '));
  if(varnm.nonempty()) return true; // cannot look it up -- have to go on nonempty status
  return false;
}

bool PrintVar::CvtFmCode(const String& code) {
  String exprstr;
  if(code.startsWith("print ")) exprstr = trim(code.after("print "));
  else if(code.startsWith("Print ")) exprstr = trim(code.after("Print "));
  else if(code.startsWith("print: ")) exprstr = trim(code.after("print: "));
  else if(code.startsWith("Print: ")) exprstr = trim(code.after("Print: "));
  else if(code.startsWith("cerr << ")) exprstr = trim(code.after("cerr << "));
  else if(code.startsWith("cout << ")) exprstr = trim(code.after("cout << "));
  String msg;
  String vars;
  if(exprstr.contains('"')) {
    msg = trim(exprstr.before('"',-1));
    msg = msg.after('"');
    message = msg;
    exprstr = trim(exprstr.after('"',-1));
    if (exprstr.startsWith(',')) exprstr = trim(exprstr.after(','));
    if (exprstr.startsWith("<<")) exprstr = trim(exprstr.after("<<"));
  }
  if(exprstr.empty()) return true; // ok, we'll take it -- just a meassage
  String varnms = exprstr;
  int idx = 0;
  ProgVarRef* refs[6] = {&print_var, &print_var2, &print_var3, &print_var4, &print_var5,
                         &print_var6};
  do {
    String varnm = varnms;
    if (varnms.contains(',')) {
      varnm = trim(varnms.before(','));
      varnms = trim(varnms.after(','));
    }
    else if (varnms.contains("<<")) {
      varnm = trim(varnms.before("<<"));
      varnms = trim(varnms.after("<<"));
    }
    else if (varnms.contains(' ')) {
      varnm = trim(varnms.before(' '));
      varnms = trim(varnms.after(' '));
    }
    if(varnm == varnms)
      varnms = "";              // done
    ProgVar* pv = FindVarNameInScope(varnm, false);
    if(pv) {
      *(refs[idx]) = pv;
    }
    idx++;
  } while(varnms.nonempty());
  return true;
}
