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
//#include "array_qt.h"
#include "t3viewer.h"
#include "pdp_TA_type.h"


// forwards this file
class pdpDataViewer;

class PDP_API ProcessDialog : public taiEditDataHost {
  // #IGNORE
  Q_OBJECT
public:
  ProcessDialog(void* base, TypeDef* tp, bool readonly = false)
    : taiEditDataHost(base, tp, readonly) { };
  ProcessDialog()				{ };
  ~ProcessDialog();

  virtual bool	CtrlPanel()	{ return false; }

public slots:
  void		Ok();		// override - stop the process when these are hit..
  void		Cancel(); // override
};

class PDP_API Process_RunDlg : public ProcessDialog {
  // #IGNORE
public:
  Process_RunDlg(void* base, TypeDef* tp, bool readonly = false)
    : ProcessDialog(base, tp, readonly) {};
  Process_RunDlg() {};
  ~Process_RunDlg();

  bool	CtrlPanel()	{ return true; }

  bool ShowMember(MemberDef* md);
};

class PDP_API taiProcess : public taiEdit {
  // adds control buttons to the edit window
public:
  taiProcess*	run_ie;

  int		BidForEdit(TypeDef* td);
  override int 	Edit(void* base, bool readonly = false, const iColor* bgclr = NULL); // unusual case, so we override

  void Initialize();
  void Destroy();
  TAQT_EDIT_INSTANCE(taiProcess, taiEdit);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};


class PDP_API taiProcessRunBox : public taiProcess {
  // just has the control buttons
public:
  int		BidForEdit(TypeDef*) 	{ return 0; }
  TAQT_EDIT_INSTANCE(taiProcessRunBox, taiProcess);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};

class PDP_API taiSpecMember : public taiMember {
  // special for the spec type member (adds the unique box)
public:
  int		BidForMember(MemberDef* md, TypeDef* td);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  void		CmpOrigVal(taiData* dat, void* base, bool& first_diff);

  virtual bool	NoCheckBox(taiDataHost* host_); // deterimine if check box should be used

  TAQT_MEMBER_INSTANCE(taiSpecMember, taiMember);
protected:
  override taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  override void		GetImage_impl(taiData* dat, void* base);
};


//////////////////////////
//   ipdpDataViewer	//
//////////////////////////

class PDP_API ipdpDataViewer: public iT3DataViewer { //#IGNORE
friend class pdpDataViewer;
public:

  pdpDataViewer*	viewer() {return (pdpDataViewer*)m_viewer;}

  ipdpDataViewer(void* root_, TypeDef* typ_, pdpDataViewer* viewer_,
      QWidget* parent = NULL);
  ~ipdpDataViewer();

protected:
//  override void 	SetActionsEnabled_impl();
};

//////////////////////////
//   pdpDataViewer	//
//////////////////////////

class PDP_API pdpDataViewer: public T3DataViewer { // master viewer for network, i/o, etc.
#ifndef __MAKETA__
  typedef pdpDataViewer inherited;
#endif
public:
  static pdpDataViewer*	New(Project* proj); // populates all elements (all layers, prjns, i/o, etc.)

  Project*		proj() {return (Project*)m_data;} // aka data
  ipdpDataViewer*	net_win() {return (ipdpDataViewer*)m_window;}

  TA_BASEFUNS(pdpDataViewer)
protected:
  override void		Constr_Window_impl();  // #IGNORE implement this to set the m_window instance
private:
  void		Initialize();
  void		Destroy();
};

#endif // pdp_qt_h
