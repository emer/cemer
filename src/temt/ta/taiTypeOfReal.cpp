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

#include "taiTypeOfReal.h"
#include <taiWidgetField>
#include <iLineEdit>
#include <MemberDef>
#include <BuiltinTypeDefs>

#include <QDoubleValidator>


int taiTypeOfReal::BidForType(TypeDef* td){
  if (td->DerivesFrom(&TA_float) || td->DerivesFrom(&TA_double))
    return (taiType::BidForType(td) +1);
  return 0;
}

taiWidget* taiTypeOfReal::GetDataRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef* mbr) {
  taiWidgetField* rval = new taiWidgetField(typ, host_, par, gui_parent_, flags_);
  // now, decorate with a validator, and init
  QDoubleValidator* dv = new QDoubleValidator(rval->rep());
  // set std notation, otherwise default is scientific
#if (QT_VERSION >= 0x040300)
  dv->setNotation(QDoubleValidator::ScientificNotation);
#endif
  //note: validator doesn't actually enforce mins and max's...
  if (mbr) {
    String val = mbr->OptionAfter("MIN_");
    // programmer must make sure value is valid!!!!!
    if (val.nonempty()) {
      double fval = val.toDouble();
      dv->setBottom(fval);
    }
    val = mbr->OptionAfter("MAX_");
    // programmer must make sure value is valid!!!!!
    if (val.nonempty()) {
      double fval = val.toDouble();
      dv->setTop(fval);
    }
  }
  rval->rep()->setValidator(dv);
  return rval;
}

void taiTypeOfReal::GetValue_impl(taiWidget* dat, void* base) {
  taiWidgetField* rval = dynamic_cast<taiWidgetField*>(dat);
  if (!rval) return;
  String strval(rval->GetValue());
  double dval = strval.toDouble();
  // enforce range
  if (dat->mbr) {
    String mval = dat->mbr->OptionAfter("MIN_");
    // programmer must make sure value is valid!!!!!
    if (mval.nonempty()) {
      double dminval = mval.toDouble();
      if (dval < dminval)
        dval = dminval;
    }
    mval = dat->mbr->OptionAfter("MAX_");
    // programmer must make sure value is valid!!!!!
    if (mval.nonempty()) {
      double dmaxval = mval.toDouble();
      if (dval > dmaxval)
        dval = dmaxval;
    }
  }
  typ->SetValVar(dval, base);
}

