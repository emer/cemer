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


// forwards this file
class pdpDataViewer;
class iPdpMainWindowViewer;
class PdpMainWindowViewer;

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


class PDP_API iPdpMainWindowViewer: public iMainWindowViewer { // viewer window for the pdp application
friend class PdpMainWindowViewer;
    Q_OBJECT
INHERITED(iMainWindowViewer)
public:
  ProjectBase*		curProject(); // the Project at root of what user has selected; Null if n/a
  bool		showFileObjectOps() {return true;} // override
  
  ~iPdpMainWindowViewer(); //

public: // slot overrrides
  void 		fileNew(); //override
  void 		fileOpen(); //override
  void 		fileSave(); //override
  void 		fileSaveAs(); //override
  void 		fileSaveAll(); //override
  void 		fileClose(); //override
  
protected:
  ProjectBase*		cur_project; // cached
  iPdpMainWindowViewer(PdpMainWindowViewer* browser_, QWidget* parent = 0);
  void 			Constr_Menu_impl(); // override
  void		NewBrowser(ProjectBase* proj); // temp, until rework finished, makes browser
};


class TA_API PdpMainWindowViewer : public MainWindowViewer {
  // #NO_TOKENS the base type for browsers in pdp (used by MainWindowViewer::NewBrowser)
INHERITED(MainWindowViewer)
friend class iPdpMainWindowViewer;
public:

  override iPdpMainWindowViewer* window() {return (iPdpMainWindowViewer*)m_widget;}
    // note: polymorphic return-value override
  
  TA_DATAVIEWFUNS(PdpMainWindowViewer, MainWindowViewer)
protected:
  override QWidget*	ConstrWidget_impl(QWidget* gui_parent); // #IGNORE
private:
  void			Initialize() {}
  void			Destroy() {}
};



#endif // pdp_qt_h
