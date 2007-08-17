// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
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

// emergent_qt.h : Qt and Inventor stuff for pdp

#ifndef emergent_qt_h
#define emergent_qt_h 1

#include "emergent_base.h"
#include "spec.h"
#include "ta_qttype.h"
#include "ta_qtdata.h"
#include "ta_qtdialog.h"
#include "ta_qtviewer.h"
#include "t3viewer.h"
#include "emergent_TA_type.h"



class EMERGENT_API taiSpecMember : public taiMember {
  // special for the spec type member (adds the unique box)
public:
  int		BidForMember(MemberDef* md, TypeDef* td);

  void		CmpOrigVal(taiData* dat, const void* base, bool& first_diff); // replaces

  virtual bool	NoCheckBox(IDataHost* host_) const; // deterimine if check box should be used

  TAQT_MEMBER_INSTANCE(taiSpecMember, taiMember);
protected:
  override taiData*	GetArbitrateDataRep(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  override void		GetArbitrateImage(taiData* dat, const void* base);
  override void		GetArbitrateMbrValue(taiData* dat, void* base, bool& first_diff);
};

#endif // emergent_qt_h
