// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



// pdp_qt.h : Qt and Inventor stuff for pdp

#ifndef pdp_qt_h
#define pdp_qt_h 1

#include "pdpbase.h"
#include "spec.h"
#include "ta_qttype.h"
#include "ta_qtdata.h"
#include "ta_qtdialog.h"
#include "ta_qtbrowse.h"
//#include "array_qt.h"
#include "t3viewer.h"
#include "pdp_TA_type.h"



class PDP_API taiSpecMember : public taiMember {
  // special for the spec type member (adds the unique box)
public:
  int		BidForMember(MemberDef* md, TypeDef* td);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  void		CmpOrigVal(taiData* dat, const void* base, bool& first_diff); // replaces

  virtual bool	NoCheckBox(IDataHost* host_) const; // deterimine if check box should be used

  TAQT_MEMBER_INSTANCE(taiSpecMember, taiMember);
protected:
  bool		no_check_box; // complicated test, result cached for safety
  override taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  override void		GetImage_impl(taiData* dat, const void* base);
};


class PDP_API ConsoleDockViewer: public DockViewer {
  // #NO_TOKENS encapsulates the console into a dock viewer window
INHERITED(DockViewer)
public:

  TA_DATAVIEWFUNS(ConsoleDockViewer, DockViewer)
protected:
  override IDataViewWidget* ConstrWidget_impl(QWidget* gui_parent); // #IGNORE note: we just use base window, and put the console into it
  override void		MakeWinName_impl(); // set win_name, impl in subs
private:
  void			Initialize();
  void			Destroy() {}
};



#endif // pdp_qt_h
