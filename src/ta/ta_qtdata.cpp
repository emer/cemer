/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// tai_data.cc

#include "ta_filer.h"
#include "ta_qtdata.h"
#include "ta_qt.h"
#include "ta_qtdialog.h"
#include "ta_qttype_def.h"
//#include "ta_qttype.h"
//#include "css_qt.h"
#include "ta_qtclipdata.h" // for clip-aware controls
#include "css_basic_types.h"
#include "css_qtdialog.h"
#include "ta_css.h"
#include "ta_TA_type.h"

#include "icolor.h"
#include "icheckbox.h"
#include "icombobox.h"
#include "iflowlayout.h"
#include "ilineedit.h"
#include "ispinbox.h"

#include <qapplication.h>
#include <qcolor.h> // needed for qbitmap
#include <qcombobox.h>
//#include <qcursor.h>
//#include <qbitmap.h>
#include <qfont.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qmenubar.h>
#include <qmenudata.h>
#include <qobjectlist.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qtooltip.h>
#include <qwidgetstack.h>

/*
#ifndef CYGWIN
#include <IV-X11/xwindow.h>	// this is for window dumps
#include <IV-X11/xdisplay.h>	// this is for window dumps
#endif
*/

//#include <ostream.h>

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>

// pointer alignment for putting up windows based on the mouse
#define POINTER_ALIGN_X		.5
#define POINTER_ALIGN_Y		.5

class cssiArgDialog;

void taiAction::connect(QObject* sender, const char* signal) const {
  if ((receiver != NULL) && (!member.empty()))
    QObject::connect(sender, signal, receiver, member.chars());
}

taiAction& taiAction::operator=(const taiAction& rhs) {
  if (&rhs == this) return *this;
  receiver = rhs.receiver;
  member = rhs.member;
  return *this;
}


/* NN: ??
//////////////////////////
// 	IconGlyph 	//
//////////////////////////

IconGlyph::IconGlyph(QWidget* g,ivManagedWindow* w,void* o)
: ivMonoGlyph(g) {
  window = w;
  obj = o;
  last_draw_allocation.x_allotment().span(-1); // bogus initial
  first_draw = true;
  ScriptIconify = NULL;
  SetIconify = NULL;
}

void IconGlyph::SetWindow(ivManagedWindow* w){
  window = w;
  if(taiM->icon_bitmap != NULL)  window->icon_bitmap(taiM->icon_bitmap);
}


// for some reason, when you load an object that is iconified),
// undraw gets called twice, and then draw gets called once.
// first_draw ignores this first drawing.

// CYGWIN: these are not supported because they wreak havoc with
// everything -- the user needs to use the Iconify and DeIconify calls
// explicitly instead of relying on the window manager

void IconGlyph::draw(ivCanvas* c, const ivAllocation& a) const {
  ivMonoGlyph::draw(c,a);
#ifndef CYGWIN
  if(window == NULL) return;
  taiMisc::SetWinCursor(window);

  if(taMisc::is_loading) return;
  if(first_draw == true) {
    ((IconGlyph*)this)->first_draw = false;
    return;
  }
  if(SetIconify == NULL) return;
  if((*SetIconify)(obj,-1) != false) {
    (*SetIconify)(obj,0);
    if(!(last_draw_allocation.equals(a,0.01))) {
      if(ScriptIconify != NULL) (*ScriptIconify)(obj,0);
      ((IconGlyph *) this)->last_draw_allocation = a;
    }
  }
#endif
}

void IconGlyph::undraw(){
  ivMonoGlyph::undraw();
#ifndef CYGWIN
  if(taMisc::is_loading) return;
  if(SetIconify == NULL) return;
  if((*SetIconify)(obj,-1) != true) {
    (*SetIconify)(obj,1);
    ivAllocation bogus_allocation;
    if(!(last_draw_allocation.equals(bogus_allocation,0.01))) {
      if(ScriptIconify != NULL) (*ScriptIconify)(obj,1);
      last_draw_allocation = bogus_allocation;
    }
  }
#endif
} */ // OBS NN


//////////////////////////
//    taiDataList	//
//////////////////////////

void taiDataList::El_Done_(void* it) {
  delete (taiData*)it;
}

//////////////////////////////////////////////////////////
// 	taiData: glyphs to represent kinds of data	//
//////////////////////////////////////////////////////////

// NOTE: this is for ta_TA.cc only
taiData::taiData()
:QObject()
{
  typ = NULL;
  host = NULL;
  museHiBG = false;
  mhiBG = false;
  m_rep = NULL;
  mparent = NULL;
  mflags = 0;
}

taiData::taiData(TypeDef* typ_, taiDataHost* host_, taiData* parent_, QWidget* gui_widget, int flags_)
:QObject(NULL)
{
  typ = typ_;
  host = host_;
  museHiBG = false;
  mhiBG = false;
  m_rep = NULL;
  mparent = NULL; // must be valid before calling setParent!
  mflags = flags_;
  setParent(parent_);
}

taiData::~taiData() {
  setParent(NULL);
  host = NULL;
  m_rep = NULL;
}

void taiData::DataChanged(taiData* chld) {
  // ignore completely if not yet constructed
  if (!isConstructed()) return;

  // don't do anything ourselves, but notify host and our parent..
  // if we have a parent, delegate notification to it, otherwise inform the host
  if (mparent != NULL)
    mparent->DataChanged(this);
  else if (host != NULL)
    host->Changed();

  DataChanged_impl(chld);
}

int taiData::defSize() {
  if (mparent != NULL)
    return mparent->defSize();
  else if (host != NULL)
    return host->ctrl_size;
  else return taiM->ctrl_size;
}

void taiData::emit_settingHiBG(bool value) {
  emit settingHiBG(value);
}

void taiData::emit_UpdateUi() {
  emit UpdateUi();
}

bool taiData::eventFilter(QObject* watched, QEvent* ev) {
  //note: we don't delete events, just look for focusin on our rep
  bool rval = inherited::eventFilter(watched, ev);
  if (ev->type() == QEvent::FocusIn) {
    SetThisAsHandler(true);
  }
  return rval;
}

bool taiData::isConstructed() {
  // this implementation delegates to parents, under assumption that
  // children are constructed if parents are constructed
  if (mparent != NULL)
    return mparent->isConstructed();
  else if (host != NULL)
    return (host->isConstructed());
  // if no parent or host, we assume constructed -- if needed, we would have to
  // create a variable to track this, and set it after the constructor ran (which would
  // require changes to ALL the types!)
  else return true;
}

bool taiData::readOnly() {
  if (mparent != NULL)
    return (mparent->readOnly() || (mflags & flgReadOnly));
  else
    return (mflags & flgReadOnly);
}

void taiData::setHiBG(bool value) {
  if ((!museHiBG) || (mhiBG == value)) return;
  setHiBG_impl(value);
}

void taiData::setHiBG_impl(bool value) {
  mhiBG = value;
  emit_settingHiBG(value);
}

void taiData::setParent(taiData* value) {
  if (mparent == value) return;
  if (mparent != NULL)
    mparent->ChildRemove(this);
  mparent = value;
  if (mparent != NULL)
    mparent->ChildAdd(this);

}

void taiData::SetRep(QWidget* val) {
  if (m_rep == val) return;

  // if not already destroying (either rep or this), disconnect all the signals from old rep to this object
  if (m_rep != NULL) {
    disconnect(m_rep, 0, this, 0); // all slots/signals between m_rep and this
  }
  m_rep = val;
  if (m_rep) {
    m_rep->installEventFilter(this);
  }
}

void taiData::SetThisAsHandler(bool set_it) {
  if (host) host->SetItemAsHandler(this, set_it);
}

void taiData::repChanged() {
  DataChanged(NULL);
}

void taiData::repDestroyed(QObject* obj) {
  if (m_rep == obj) m_rep = NULL;
}


//////////////////////////////////
//	taiCompData		//
//////////////////////////////////

taiCompData::taiCompData(TypeDef* typ_, taiDataHost* host_, taiData* parent_, QWidget* gui_parent_, int flags_)
      :taiData(typ_, host_, parent_, gui_parent_, flags_)
{
  lay = NULL; // usually created in InitLayout;
  last_spc = -1;
  mwidgets = new QObjectList();
}

taiCompData::~taiCompData() {
  mwidgets->clear();
  delete mwidgets;
  taiData* dat;
  while (data_el.size > 0) {
    dat = data_el.Pop();
    dat->mparent = NULL; // don't call setParent, otherwise, calls our ChildRemove
  }
}

void taiCompData::ChildAdd(taiData* child) {
  data_el.Add(child);
}

void taiCompData::ChildRemove(taiData* child) {
  //NOTE: this will get called while being removed/deleted from an own list!
  //2004-08-31 workaround, change taListptr to remove items from list before deleting, thus item not found
  data_el.Remove(child);
}

void taiCompData::InitLayout() { //virtual/overridable
  lay = new QHBoxLayout(GetRep());
  last_spc = taiM->hsep_c; // give it a bit of room
}

void taiCompData::AddChildWidget(QWidget* child_widget, int space_after) { // non-virtual
  mwidgets->append(child_widget);
  AddChildWidget_impl(child_widget);
  last_spc = space_after;
}

void taiCompData::EndLayout() { //virtual/overridable
  lay->addStretch();
}

void taiCompData::AddChildWidget_impl(QWidget* child_widget) { //virtual/overridable
  if (last_spc != -1)
    lay->addSpacing(last_spc);
  lay->add(child_widget);
}

QWidget* taiCompData::widgets(int index) {
  return (QWidget*)mwidgets->at(index);
}

int taiCompData::widgetCount() {
  return mwidgets->count();
}


/* NN:
class ScrollFieldEditor : public ivFieldEditor {
public:
  taiData* data;
  int	drawn;
  ScrollFieldEditor(taiData* d,const char* strval,ivWidgetKit* w, ivStyle* st);
  void undraw();
  void allocate(ivCanvas* c, const ivAllocation& a,ivExtension& ext);
  void keystroke(const ivEvent& e);
  ivInputHandler* focus_in();
};

ScrollFieldEditor::ScrollFieldEditor(taiData* d,const char* strval,ivWidgetKit* w, ivStyle* st)
: ivFieldEditor(strval,w,st,NULL){
  data = d;
  drawn = false;
}

void ScrollFieldEditor::allocate(ivCanvas* c, const ivAllocation& a,
				 ivExtension& ext) {
  drawn = true;
  ivFieldEditor::allocate(c,a,ext);
}

void ScrollFieldEditor::undraw(){
  drawn = false;
  ivFieldEditor::undraw();
}

ivInputHandler* ScrollFieldEditor::focus_in() {
  ivInputHandler* result = ivFieldEditor::focus_in();
  if(data != NULL) {
  //NOTE: following was from when parent was dialog...
    if(!drawn && (data->dialog != NULL) && (data->dialog->state == taiDialog::ACTIVE)) {
      data->dialog->Scroll();
    }
  }
  //  cerr << "focused" << endl << flush;
  return result;
}

void ScrollFieldEditor::keystroke(const ivEvent& e) {
  if((data == NULL) || (data->dialog == NULL)) {
    ivFieldEditor::keystroke(e);
    return;
  }                 dialog
  if(data->dialog->dialog->special_key(e))
    return;
  String curval = text()->string();
  ivFieldEditor::keystroke(e);
  if((data != NULL) && (curval != text()->string())) {
    data->DataChanged();
  }
} */ // NN

//////////////////////////////////
//	taiField		//
//////////////////////////////////


taiField::taiField(TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
 : taiData(typ_, host_, par, gui_parent_, flags_)
{
  SetRep(new iLineEdit(gui_parent_));
  rep()->setFixedHeight(taiM->text_height(defSize()));
  if (readOnly()) {
    rep()->setReadOnly(true);
  } else {
    QObject::connect(m_rep, SIGNAL(textChanged(const QString&) ),
          this, SLOT(repChanged() ) );
  }
  QObject::connect(this, SIGNAL(settingHiBG(bool)),
    m_rep, SLOT(setHilight(bool) ) );
  // cliphandling connections
  QObject::connect(m_rep, SIGNAL(selectionChanged()),
    this, SLOT(selectionChanged() ) );
}


void taiField::GetImage(const String& val) {
  rep()->setText(val);
}

String taiField::GetValue() const {
  return rep()->text();
}

void taiField::selectionChanged() {
  emit_UpdateUi();
}

void taiField::this_GetEditActionsEnabled(int& ea) {
  if (!readOnly())
    ea |= taiClipData::EA_PASTE;
  if (rep()->hasSelectedText()) {
    ea |= (taiClipData::EA_COPY);
    if (!readOnly())
      ea |= (taiClipData::EA_CUT |  taiClipData::EA_DELETE);
  }
}

void taiField::this_EditAction(int ea) {
  if (ea & taiClipData::EA_CUT) {
    rep()->cut();
  } else if (ea & taiClipData::EA_COPY) {
    rep()->copy();
  } else if (ea & taiClipData::EA_PASTE) {
    rep()->paste();
  } else if (ea & taiClipData::EA_DELETE) {
    rep()->del(); //note: assumes we already qualified with hasSelectedText, otherwise it is a BS
  }
}

void taiField::this_SetActionsEnabled() {
  //TODO: UNDO/REDO
}


//////////////////////////////////
//	taiIncrField		//
//////////////////////////////////

taiIncrField::taiIncrField(TypeDef* typ_, taiDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_)
: taiData(typ_, host_, par, gui_parent_, flags_)
{
  SetRep( new iSpinBox(gui_parent_) );
  iSpinBox* rep = this->rep();
  rep->setFixedHeight(taiM->text_height(defSize()));

  rep->setMaxValue(0x7FFFFFFF);
  if (HasFlag(flgPosOnly))
    rep->setMinValue(0);
  else
    rep->setMinValue(-0x80000000); // TODO: use proper minint from limits.h
  if (readOnly()) {
    rep->setReadOnly(true);
  } else {
    QObject::connect(m_rep, SIGNAL(valueChanged(int) ),
          this, SLOT(repChanged() ) );
  }
  QObject::connect(this, SIGNAL(settingHiBG(bool)),
    m_rep, SLOT(setHilight(bool) ) );
  // cliphandling connections
  QObject::connect(m_rep, SIGNAL(selectionChanged()),
    this, SLOT(selectionChanged() ) );

  // prevent from expanding too much horizontally -- may want to permit this if using the built in label
//NN  rep()->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred)); //def is Minimum,Fixed

}

void taiIncrField::GetImage(const int val) {
  rep()->setValue(val);
}

int taiIncrField::GetValue() const {
  return rep()->value();
}

void taiIncrField::selectionChanged() {
  emit_UpdateUi();
}


void taiIncrField::this_GetEditActionsEnabled(int& ea) {
  if (!readOnly())
    ea |= taiClipData::EA_PASTE;
  if (rep()->hasSelectedText()) {
    ea |= (taiClipData::EA_COPY);
    if (!readOnly())
      ea |= (taiClipData::EA_CUT |  taiClipData::EA_DELETE);
  }
}

void taiIncrField::this_EditAction(int ea) {
  if (ea & taiClipData::EA_CUT) {
    rep()->cut();
  } else if (ea & taiClipData::EA_COPY) {
    rep()->copy();
  } else if (ea & taiClipData::EA_PASTE) {
    rep()->paste();
  } else if (ea & taiClipData::EA_DELETE) {
    rep()->del(); //note: assumes we already qualified with hasSelectedText, otherwise it is a BS
  }
}

void taiIncrField::this_SetActionsEnabled() {
  //TODO: UNDO/REDO
}




//////////////////////////////////
//	taiToggle		//
//////////////////////////////////

taiToggle::taiToggle(TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) :
       taiData(typ_, host_, par, gui_parent_, flags_)
{
  SetRep( new iCheckBox(gui_parent_) );
  // prevent from expanding horizontally -- may want to permit this if using the built in label
  rep()->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed)); //def is Minimum,Preferred
  rep()->setFixedHeight(taiM->label_height(defSize()));

  //connect changed signal to our slot
  QObject::connect(m_rep, SIGNAL(toggled(bool) ),
        this, SLOT(repChanged() ) );
  if (readOnly()) {
    rep()->setReadOnly(true);
  } else {
    QObject::connect(m_rep, SIGNAL(toggled(bool) ),
          this, SLOT(repChanged() ) );
  }
  QObject::connect(this, SIGNAL(settingHiBG(bool)),
    m_rep, SLOT(setHilight(bool) ) );
}

void taiToggle::GetImage(bool val) {
  rep()->setChecked(val);
}

bool taiToggle::GetValue() const {
  return rep()->isChecked();
}

/* OBS
//////////////////////////////////
//	taiLabel		//
//////////////////////////////////

taiLabel::taiLabel(TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent_) :
	taiData(typ_, host_, par, gui_parent_)
{
  SetRep( new QLabel(gui_parent_) );
}

void taiLabel::GetImage(const char* val) {
  rep()->setText(val);
}
*/


//////////////////////////////////
//	taiPlusToggle		//
//////////////////////////////////

taiPlusToggle::taiPlusToggle(TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiCompData(typ_, host_, par, gui_parent_, flags_)
{
  SetRep(new QFrame(gui_parent_));
  but_rep = NULL;
  data = NULL;
}

taiPlusToggle::~taiPlusToggle() {
//  rep = NULL;
  if (data != NULL)
    delete data;
  data = NULL;
}

void taiPlusToggle::InitLayout() {
  taiCompData::InitLayout();
  but_rep = new QCheckBox(rep());
  AddChildWidget(but_rep, taiM->hsep_c);
  connect(but_rep, SIGNAL(toggled(bool)),
      this, SLOT(Toggle_Callback()) );
}

int taiPlusToggle::GetValue() {
  return but_rep->isChecked();
}
void taiPlusToggle::GetImage(bool chk) {
  but_rep->setChecked(chk);
}
void taiPlusToggle::Toggle_Callback() {
  if (host != NULL)
    host->Changed();
  // DataChanged(); //note: was already remarked out prior to qt port
}

void taiPlusToggle::DataChanged_impl(taiData* chld) {
  but_rep->setChecked(true);
}


//////////////////////////
//     taiComboBox	//
//////////////////////////

taiComboBox::taiComboBox(TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
:taiData(typ_, host_, par, gui_parent_, flags_)
{
  Initialize(gui_parent_);
}

taiComboBox::taiComboBox(bool is_enum, TypeDef* typ_, taiDataHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_)
:taiData(typ_, host_, par, gui_parent_, flags_)
{
  Initialize(gui_parent_);
  if (is_enum && typ) {
    for (int i = 0; i < typ->enum_vals.size; ++i) {
      AddItem(typ->enum_vals.FastEl(i)->GetLabel());
    }
  }
}

void taiComboBox::Initialize(QWidget* gui_parent_) {
  SetRep(new iComboBox(gui_parent_));
  rep()->setFixedHeight(taiM->combo_height(defSize()));
  // prevent from expanding horizontally -- may want to permit this if using the built in label
//dnw  rep()->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed)); //def is Minimum,Preferred

  //connect changed signal to our slot
  QObject::connect(m_rep, SIGNAL(activated(int) ),
        this, SLOT(repChanged() ) );
  // also to aux signal (used by non-taiDataHost clients)
  QObject::connect(m_rep, SIGNAL(activated(int) ),
    this, SIGNAL(itemChanged(int)) );
  QObject::connect(this, SIGNAL(settingHiBG(bool)),
    m_rep, SLOT(setHilight(bool) ) );
}

void taiComboBox::AddItem(const String& val) {
     // add an item to the list
  rep()->insertItem(val);
}

void taiComboBox::Clear() {
   //clears all items
  for (int i = rep()->count() - 1; i >= 0; --i) {
    rep()->removeItem(i);
  }
}

void taiComboBox::GetImage(int itm) {
    // set to this item number
  rep()->setCurrentItem(itm);
}

void taiComboBox::GetValue(int& itm) const {
  itm = rep()->currentItem();
}
/* NN
void taiComboBox::GetImage(const String& val) {
  // set to this string item
  rep()->setCurrentText(val);
} */


//////////////////////////
//     taiBitBox	//
//////////////////////////

iBitCheckBox::iBitCheckBox(int val_, String label, QWidget * parent)
:iCheckBox(label.chars(), parent)
{
  val = val_;
  connect(this, SIGNAL(toggled(bool)), this, SLOT(this_toggled(bool)) );
}

void iBitCheckBox::this_toggled(bool on)
{
  emit toggledEx(this, on);
}


taiBitBox::taiBitBox(TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
:taiData(typ_, host_, par, gui_parent_, flags_)
{
  Initialize(gui_parent_);
}

taiBitBox::taiBitBox(bool is_enum, TypeDef* typ_, taiDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_)
:taiData(typ_, host_, par, gui_parent_, flags_)
{
  Initialize(gui_parent_);
  if (is_enum && typ) {
    for (int i = 0; i < typ->enum_vals.size; ++i) {
      EnumDef* ed = typ->enum_vals.FastEl(i);
      AddBoolItem(ed->GetLabel(), ed->enum_no);
    }
  }
}


void taiBitBox::Initialize(QWidget* gui_parent_) {
  SetRep(new QFrame(gui_parent_));
  rep()->setFixedHeight(taiM->label_height(defSize()));
  lay = new QHBoxLayout(m_rep);

  // prevent from expanding horizontally -- may want to permit this if using the built in label
//dnw  rep()->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed)); //def is Minimum,Preferred

}

void taiBitBox::bitCheck_toggled(iBitCheckBox* sender, bool on) {
  if (on) m_val |= sender->val;
  else    m_val &= ~(sender->val);
  if (host != NULL)
    host->Changed();
}

void taiBitBox::AddBoolItem(String name, int val) {
  iBitCheckBox* bcb = new iBitCheckBox(val, name, m_rep);
  lay->addWidget(bcb);
  if (readOnly()) {
    bcb->setReadOnly(true);
  } else {
    QObject::connect(bcb, SIGNAL(toggledEx(iBitCheckBox*, bool)),
      this, SLOT(bitCheck_toggled(iBitCheckBox*, bool) ) );
  }
}

void taiBitBox::GetImage(int val) {
  QObjectListIterator it(*(m_rep->children()));
  QObject *obj;
  while ((obj = it.current()) != NULL ) {
    if (obj->inherits("iBitCheckBox")) {
      iBitCheckBox* bcb = (iBitCheckBox*)obj;
      bcb->setChecked((val & bcb->val)); //note: prob raises signal -- ok
    }
    ++it;
  }
  m_val = val;
}

void taiBitBox::GetValue(int& val) const {
  val = m_val;
}




//////////////////////////////////
// 	taiPolyData		//
//////////////////////////////////

taiPolyData::taiPolyData(TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags)
: taiCompData(typ_, host_, par, gui_parent_, flags)
{
  if (host_ && (host_->GetTypeDef()->InheritsFrom(TA_taiEditDataHost))) {
    show = ((taiEditDataHost*)host_)->show;
  } else {
    show = taMisc::show_gui;
  }
  Constr(gui_parent_);
}

taiPolyData::~taiPolyData() {
  data_el.Reset();
}

bool taiPolyData::ShowMember(MemberDef* md) {
  if (md->HasOption("HIDDEN_INLINE"))
    return false;
  else
    return md->ShowMember((taMisc::ShowMembs)show);
}

void taiPolyData::Constr(QWidget* gui_parent_) {
  SetRep(new QFrame(gui_parent_));
  rep()->setMaximumHeight(taiM->max_control_height(defSize()));
  if (host != NULL) {
    rep()->setPaletteBackgroundColor(*(host->colorOfRow(host->cur_row)));
  }
  InitLayout();
//  QHBoxLayout* hbl = new QHBoxLayout(m_rep);
  for (int i = 0; i < typ->members.size; ++i) {
    MemberDef* md = typ->members.FastEl(i);
    if (!ShowMember(md))
      continue;

    String desc = md->desc;

    // add caption
    String nm = md->GetLabel();
    QLabel* lbl = new QLabel(nm, rep());

    AddChildWidget(lbl, taiM->hsep_c);

    // add gui representation of data
    taiData* mb_dat = md->im->GetDataRep(host, this, m_rep); //adds to list
    QWidget* ctrl = mb_dat->GetRep();
    lbl->setBuddy(ctrl);
    AddChildWidget(ctrl, taiM->hspc_c);

    // add description text tooltips
    if (!desc.empty()) {
      QToolTip::add(lbl, desc);
      QToolTip::add(ctrl, desc);
    }
  }
  EndLayout();
}

void taiPolyData::GetImage(void* base) {
//NN??  cur_base = base;
  int cnt = 0;
  for (int i = 0; i < typ->members.size; ++i) {
    MemberDef* md = typ->members.FastEl(i);
    if (!ShowMember(md))
      continue;
    taiData* mb_dat = data_el.FastEl(cnt++);
    md->im->GetImage(mb_dat, base);
  }
}

void taiPolyData::GetValue(void* base) {
  ostream* rec_scrpt = taMisc::record_script; // don't record script stuff now
  taMisc::record_script = NULL;
  bool first_diff = true;
  int cnt = 0;
  for (int i = 0; i < typ->members.size; ++i) {
    MemberDef* md = typ->members.FastEl(i);
    if (!ShowMember(md))
      continue;
    taiData* mb_dat = data_el.FastEl(cnt++);
    md->im->GetMbrValue(mb_dat, base, first_diff);
  }
  if (typ->InheritsFrom(TA_taBase)) {
    TAPtr rbase = (TAPtr)base;
    rbase->UpdateAfterEdit();	// hook to update the contents after an edit..
  }
  taMisc::record_script = rec_scrpt;
}

//////////////////////////////////
// 	taiDataDeck		//
//////////////////////////////////

taiDataDeck::taiDataDeck(TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags)
: taiCompData(typ_, host_, par, gui_parent_, flags) {
  cur_deck = 0;
  SetRep(new QWidgetStack(gui_parent_));
  rep()->setMaximumHeight(taiM->max_control_height(defSize()));
  if (host != NULL) {
    rep()->setPaletteBackgroundColor(*(host->colorOfRow(host->cur_row)));
  }
}

void taiDataDeck::GetImage(int i) {
  // note: QWidgetStack doesn't seem to completely hide underneath widgets...
  cur_deck = i;
  rep()->raiseWidget(i);
  for (int j = 0; j < widgetCount(); ++j) {
    QWidget* w = widgets(j);
    if (i == j) w->show();
    else        w->hide();
  }
}

void taiDataDeck::AddChildWidget_impl(QWidget* child_widget) {
  (rep()->addWidget(child_widget, cur_deck));
}

//////////////////////////
// 	taiMenuEl	//
//////////////////////////

taiMenuEl::taiMenuEl(taiMenu* owner_, QMenuItem* item_, int radio_grp_, int sel_type_, const String& label_,
      void* usr_data_, CallbackType ct_, const QObject *receiver, const char* member)
: QObject()
{
  owner = owner_;
  rep = item_;
  label = label_;
  usr_data = usr_data_;
  radio_grp = radio_grp_;
  sel_type = sel_type_;
  connectCount = 0;
  if (ct_ != none)
    connect(ct_, receiver, member); // default action -- more could be added
}

taiMenuEl::~taiMenuEl() {
  owner = NULL;
  rep = NULL;
}

bool taiMenuEl::canSelect() {
  return ((sel_type & taiMenu::radio) && (radio_grp = -1) && !isSubMenu());
}

int taiMenuEl::id() {
  return (rep) ? rep->id() : -1;
}

bool taiMenuEl::isChecked() {
   // returns 'true' if a radio or toggle item, and checked, false otherwise
  if (isSubMenu())
    return false;
  else return rep->isChecked();
}

void taiMenuEl::setChecked(bool value) {
   // ignored if not a radio or toggle item
  if (isSubMenu() || !(sel_type & (taiMenu::radio | taiMenu::toggle)))
    return;
  else owner->menu()->setItemChecked(id(), value);
}

void taiMenuEl::connect(CallbackType ct_, const QObject *receiver, const char* member) {
  // connect callback to given
  if ((ct_ == none) || (receiver == NULL) || (member == NULL)) return;

  switch (ct_) {
  case none:
    return;
  case action:
    QObject::connect(this, SIGNAL(Action()), receiver, member);
    break;
  case men_act:
    QObject::connect(this, SIGNAL(MenuAction(taiMenuEl*)), receiver, member);
    break;
  case int_act:
    QObject::connect(this, SIGNAL(IntParamAction(int)), receiver, member);
    break;
  case ptr_act:
    QObject::connect(this, SIGNAL(PtrParamAction(void*)), receiver, member);
    break;
  }
  ++connectCount;
}

void taiMenuEl::connect(const taiAction* actn) {
   // connect Action to given callback (ignored if NULL)
  if (actn == NULL) return;
  connect(action, actn->receiver, actn->member);
}

void taiMenuEl::connect(const taiMenuAction* actn) {
    // connect MenuAction to given callback (ignored if NULL)
  if (actn == NULL) return;
  connect(men_act, actn->receiver, actn->member);
}

void taiMenuEl::Select_impl(bool selecting) {
   // called by Select() and by taiMenu::GetImage -- doesn't trigger events
  if (sel_type & taiMenu::toggle) {
    owner->menu()->setItemChecked(id(), selecting);
  } else if (sel_type & taiMenu::radio) {
    if (!selecting) {
       setChecked(false);
    } else if (radio_grp != -1) {

      // if non-global radio group item, then set the item in the group
      for (int i = 0; i < owner->items.size; ++i) {
        taiMenuEl* mel = owner->items.FastEl(i);
        if (mel->radio_grp != this->radio_grp) continue;
        setChecked((mel == this));
      }
    } else {
      setChecked(true);
   }
  }
}

void taiMenuEl::Select() {
  if (sel_type & taiMenu::toggle)
    Select_impl(!isChecked());
  else Select_impl(true);

  emitAction();		// don't set the cur_sel if executing
  emitMenuAction();	// but do it for selecting
  emitIntParamAction();
  emitIntParamAction();

  // if a radio item in global group, update global selection
  if ((sel_type & taiMenu::radio) && (radio_grp == -1))
    owner->setCurSel(this);

  if (sel_type & taiMenu::update) {
//    owner->Update();
    owner->DataChanged();		// something was selected..
  }
}

void taiMenuEl::emitAction() {
  emit Action();
}

void taiMenuEl::emitMenuAction() {
  emit MenuAction(this);
}

void taiMenuEl::emitIntParamAction() {
  emit IntParamAction((int)usr_data);
}

void taiMenuEl::emitPtrParamAction() {
  emit PtrParamAction(usr_data);
}



//////////////////////////
// 	taiSubMenuEl	//
//////////////////////////

taiSubMenuEl::taiSubMenuEl(taiMenu* owner_, QMenuItem* item_, const String& label_, void* usr_data_,
  QPopupMenu* sub_menu_, taiMenu* sub_menu_data_, const taiMenuAction* default_child_action_)
:taiMenuEl(owner_, item_, -1, taiMenu::st_none, label_, usr_data_)
{
  sub_menu = sub_menu_;
  sub_menu_data = sub_menu_data_;
  if (default_child_action_)
    default_child_action = *default_child_action_;
}

taiSubMenuEl::~taiSubMenuEl() {
  delete sub_menu_data;
  sub_menu_data = NULL;
}

//////////////////////////
// 	taiMenu	//
//////////////////////////

taiMenu::taiMenu(int rt, int st, int ft, TypeDef* typ_, taiDataHost* host_, taiData* par,
	QWidget* gui_parent_, int flags_, taiMenu* par_menu_)
: taiData(typ_, host_, par, gui_parent_, flags_)
{
  init(rt, st, ft, gui_parent_, par_menu_);
}

/*nbg taiMenu::taiMenu(int rt, int st, int ft, QWidget* gui_parent_)
: taiData(NULL, NULL, NULL, gui_parent_, 0)
{
  init(rt, st, ft, gui_parent_, NULL);
} */

taiMenu::taiMenu(QWidget* gui_parent_, int rt, int st, int ft, QMenuData* exist_menu)
: taiData(NULL, NULL, NULL, gui_parent_, 0)
{
  init(rt, st, ft, gui_parent_, NULL, exist_menu);
}


void taiMenu::init(int rt, int st, int ft, QWidget* gui_parent_, taiMenu* par_menu_, QMenuData* exist_menu)
{
  mrep_bar = NULL;
  mrep_popup = NULL;
  rep_type = (RepType)rt;
  sel_type = (SelType)st;
  font_spec = ft;
  gui_parent = gui_parent_;
  button = NULL;
  par_menu = par_menu_;
  cur_radio_grp = -1; // global group
  par_menu_el = NULL;
  //TODO: would be safer if we used Qt's type system to absolutely confirm that correct type was passed...
  if (rep_type == menubar) {
    mrep_bar = (exist_menu) ? (QMenuBar*)exist_menu : new QMenuBar(gui_parent_);
    mrep_bar->setFont(taiM->menuFont(font_spec));
    mrep_bar->setFixedHeight(taiM->button_height(font_spec)); // button height is ok to control bar height
    SetRep(mrep_bar);
  } else { // constr popup
    mrep_popup = (exist_menu) ? (QPopupMenu*)exist_menu : new QPopupMenu(gui_parent_);
    mrep_popup->setFont(taiM->menuFont(font_spec));
    if (sel_type & (radio | toggle))  {
      //TODO: probably need to always set this true, since we could have checkable items even if creating menu with normal seltype
      mrep_popup->setCheckable(true); //note: always enabled on Windows
    }
    if (rep_type == buttonmenu) {
      button = new QPushButton(gui_parent_);
      button->setPopup(mrep_popup);
      button->setFont(taiM->menuFont(font_spec)); //note: we use menu font -- TODO: might need to use a button font
      button->setFixedHeight(taiM->button_height(font_spec));
      SetRep(button);
    } else {
      SetRep(mrep_popup);
    }
  }
  cur_sel = NULL;
;
}
/*
taiMenu::taiMenu(const char* lbl, int rt, int st, int ft, TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent_)
	: taiData(typ_, host_, par, gui_parent_)
{
  rep_type = (RepType)rt;
  sel_type = (SelType)st;
  font_spec = (FonType)ft;
  Constr();
  SetMLabel(lbl);
} */

taiMenu::~taiMenu() {
  items.Reset(); //note: DON'T call ResetMenu, because it clears gui objects -- they should clear on their own
  mrep_bar = NULL;
  mrep_popup = NULL;
}


QMenuData* taiMenu::menu() {
  if (rep_type == menubar)
    return mrep_bar;
  else
    return mrep_popup;
}

void taiMenu::ResetMenu() {
  items.Reset();
  menu()->clear();
  cur_sel = NULL;
}

void taiMenu::AddSep(bool new_radio_grp) {
  //NOTE: should not add separators to menu bars
  if (rep_type == menubar) return;

  //don't double add seps or add at beginning (this check simplifies many callers, so they don't need to know
  //  whether a previous operation added items and/or seps, or not)
  if (menu()->count() == 0)  return;
  if (menu()->count() > 0) {
    // if last item is a separator, it will have an id of -count, and thus will be found (see below)
    QMenuItem* itm = menu()->findItem(menu()->idAt(menu()->count() - 1));
    if (itm->isSeparator()) return;
  }

  menu()->insertSeparator();
  //NOTE: in Qt, we don't create taiMenuEl's for separators, unlike in IV version
//  items.Add(new taiMenuEl(this,new_men,cur_item, "-", NULL,(ivAction*)NULL));
  if (new_radio_grp)
    ++cur_radio_grp; // note: ok for there to be unused radio group numbers
}

taiMenuEl* taiMenu::AddItem(const char* val, void* usr, SelType st, const taiAction* actn) {
  if (actn == NULL)
    return AddItem(val, usr, st, taiMenuEl::none);
  else
    return AddItem(val, usr, st, taiMenuEl::action, actn->receiver, actn->member);
}

taiMenuEl* taiMenu::AddItem(const char* val, void* usr, SelType st, const taiMenuAction* actn) {
  if (actn == NULL)
    return AddItem(val, usr, st, taiMenuEl::none);
  else
    return AddItem(val, usr, st, taiMenuEl::men_act, actn->receiver, actn->member);
}

taiMenuEl* taiMenu::AddItem(const char* val, void* usr_data, SelType st, taiMenuEl::CallbackType ct_,
  const QObject *receiver, const char* member, const QKeySequence* accel)
{ // 'member' is the result of the SLOT() macro
  if (st == use_default)
    st = sel_type;

  taiMenuEl* rval;
  // do not add items of same name -- return it instead of adding it
  for (int i = 0; i < items.size; ++i) {
    rval = items.FastEl(i);
    if (rval->label == val) {
      return rval;
    }
  }
  int rgrp;
  if (st & taiMenu::radio)
    rgrp = cur_radio_grp;
  else rgrp = -1;
  QMenuItem* new_men = NewItem(val, st, NULL, accel);
  rval = new taiMenuEl(this, new_men, rgrp, st, val, usr_data, ct_, receiver, member);
  rval->label = val;
  items.Add(rval);
  menu()->connectItem(rval->id(), rval, SLOT(Select()));
  // connect any parent submenu handler
  if (par_menu_el) {
    if (par_menu_el->default_child_action.receiver) {
      menu()->connectItem(rval->id(), par_menu_el->default_child_action.receiver, par_menu_el->default_child_action.member);
    }
  }
  return rval;
}

taiMenuEl* taiMenu::AddItem_FromAction(iAction* act)
{
  QPopupMenu* pm = rep_popup();
  if (!pm) {
    taMisc::Error("taiMenu::AddItem_FromAction: can only be called for QPopupMenu taiMenu objects");
    return NULL;
  }
  taiMenu::SelType st = act->isToggleAction() ? taiMenu::toggle : taiMenu::normal;

  taiMenuEl* rval;
  int rgrp = -1; // radiogroup n/a
  // use the built in api to add the menu, then retrieve it
  act->addTo(pm);
  QMenuItem* new_men = pm->findItem(pm->idAt(pm->count() - 1));

  rval = new taiMenuEl(this, new_men, rgrp, st, act->menuText(), NULL,
      taiMenuEl::int_act, act, SIGNAL(activated(int)));
  rval->label = act->menuText();
  items.Add(rval);
  menu()->connectItem(rval->id(), rval, SLOT(Select()));
  // connect any parent submenu handler
  if (par_menu_el) {
    if (par_menu_el->default_child_action.receiver) {
      menu()->connectItem(rval->id(), par_menu_el->default_child_action.receiver, par_menu_el->default_child_action.member);
    }
  }
  return rval;
}

taiSubMenuEl* taiMenu::AddSubItem(const char* val, void* usr, QPopupMenu* child, taiMenu* sub_menu_data,
    const taiMenuAction* default_child_action)
{ // 'member' is the result of the SLOT() macro
  //NOTE: we do not do any duplicate checks, because we must unconditionally add the new subitem
  QMenuItem* new_men = NewItem(val, submenu, child);
  taiSubMenuEl* rval = new taiSubMenuEl(this, new_men, val, usr, child, sub_menu_data, default_child_action);
  items.Add(rval);
  rval->label = val;
  return rval;
}

taiMenu* taiMenu::AddSubMenu(const char* val, void* usr, SelType st,
	const taiMenuAction* default_child_action, TypeDef* typ_)
{
  String subname = String(val);
  if (st == use_default) {
    // we use the value of the most recent submenu, otherwise ourself
    if (items.size > 0)
      st = (SelType)items.FastEl(items.size - 1)->sel_type;
    else
      st = this->sel_type;
  }
  taiMenu* rval = new taiMenu(taiMenu::popupmenu, st, font_spec, typ_, host, this, gui_parent, mflags, this);
//  int cur_item = items.size;
  taiSubMenuEl* sme = AddSubItem(subname, usr, rval->rep_popup(), rval, default_child_action);
  sme->sel_type = st;
  rval->par_menu_el = sme;
  rval->mlabel = val;
  return rval;
}

taiMenu* taiMenu::AddSubMenu(const char* val, const taiMenuAction* default_child_action) {
  return AddSubMenu(val, NULL, use_default, default_child_action);
}

taiMenu* taiMenu::AddSubMenu(const char* val, const taiMenuAction& default_child_action) {
  return AddSubMenu(val, NULL, use_default, &default_child_action);
}

taiMenuEl* taiMenu::curSel() const {
  if (par_menu != NULL)
    return par_menu->curSel();
  else  return cur_sel;
}

void taiMenu::DeleteItem(uint idx) {
  if (idx >= (uint)items.size) return;
  taiMenuEl* mel = items[idx];
  if (mel->rep) {
    int id = mel->rep->id();
    if (mrep_bar) {
      mrep_bar->removeItem(id);
    } else if (mrep_popup) {
      mrep_popup->removeItem(id);
    }
  }
  items.Remove(idx); // deletes
}

void taiMenu::emitLabelChanged(const char* val) {
  emit labelChanged(val);
}

int taiMenu::exec(const iPoint& pos, int indexAtPoint) {
  if (mrep_popup == NULL) return -1; // shouldn't be called if not a popup; behaves as if user selected nothing
  return mrep_popup->exec((QPoint)pos, indexAtPoint);
}

taiMenu* taiMenu::FindSubMenu(const char* nm) {
  for (int i = 0; i < items.size; ++i) {
    taiMenuEl* itm = items.FastEl(i);
    if (!itm->isSubMenu()) continue;
    taiSubMenuEl* sme = (taiSubMenuEl*)itm;
    if (sme->label == nm)
      return sme->sub_menu_data;
  }
  return NULL;
}

void taiMenu::GetImageByIndex(int itm) {
  if (itm >= items.size) return;
  taiMenuEl* mel = items.FastEl(itm);
  setCurSel(mel);
//  Update();
}

bool taiMenu::GetImage_impl(void* usr) {
   // set to this usr item, returns false if not found
  // first, look at our items...
  for (int i = 0; i < items.size; ++i) {
    taiMenuEl* itm = items.FastEl(i);
    if (!itm->canSelect()) continue;
    if (itm->usr_data == usr) {
      if ((usr == NULL) && (itm->label != "NULL"))
	continue;
      if (!itm->canSelect()) continue;
      setCurSel(itm);
      return true;
    }
  }
  // ...otherwise, recursively descend to submenus
  for (int i = 0; i < items.size; ++i) {
    taiMenuEl* itm = items.FastEl(i);
    if (!itm->isSubMenu()) continue;
    taiSubMenuEl* sme = (taiSubMenuEl*)itm;
    taiMenu* sub_menu = sme->sub_menu_data;
    if (sub_menu->GetImage_impl(usr))
      return true;
  }
  return false;
}

bool taiMenu::GetImage(void* usr) {
  // first try to find item by iterating through all eligible items and subitems
  if (GetImage_impl(usr))
      return true;

  // otherwise get first eligible item, if any, on this menu only, with data and without any menu callbacks, as default if nothing else works
  for (int i = 0; i < items.size; ++i) {
    taiMenuEl* itm = items.FastEl(i);
    if (!itm->canSelect()) continue;
    if ( (itm->usr_data != NULL) && (!itm->hasCallbacks())) {
      setCurSel(itm);
      return true;
    }
  }
  return false;
}

int taiMenu::insertItem(const char* val, const QObject *receiver, const char* member, const QKeySequence* accel) {
  taiMenuEl* mel = AddItem(val, NULL, use_default, taiMenuEl::none, receiver, member, accel);
  return mel->id();
}

QMenuItem* taiMenu::NewItem(const char* val, SelType st, QPopupMenu* child, const QKeySequence* accel) {
  QMenuItem* new_men;
  int itemId;
  if (st == submenu) {
    itemId = menu()->insertItem(val, child);
  } else {
    if (st == use_default)
      st = sel_type;
    //note: you can't pass NULL/NULL to the signal params, so if you want to set things like Accel, you have to do it after
    itemId = menu()->insertItem(val);
    if (accel) {
      menu()->setAccel(*accel, itemId);
    }
  }
  new_men = menu()->findItem(itemId);
/* TODO: Font control
  if((font_spec == big) || (font_spec == big_italic)) {
    lbl = new ivLabel(val, taiM->big_menu_font, taiM->font_foreground);
  }
  else {
    lbl = new ivLabel(val, taiM->small_menu_font, taiM->font_foreground);
  }
*/
  return new_men;
}

void taiMenu::setCurSel(taiMenuEl* value) {
  //curSel can only be a global radio type, or null
  if ( (value != NULL) && !value->canSelect() ) return;
  if (par_menu != NULL) {
    par_menu->setCurSel(value);
  } else {
    // controlling root needs to unselect existing element
    if (cur_sel == value) return;
    if (cur_sel != NULL) {
      cur_sel->Select_impl(false);
    }
    cur_sel = value;
    if (cur_sel != NULL) {
      cur_sel->Select_impl(true);
      setLabel(cur_sel->label);
    } else { //NOTE: special case of going from legal radio item to no item -- set label to NULL
      setLabel("NULL");
    }
  }
}

void taiMenu::setLabel(const String& val) {
  if (par_menu != NULL)
    par_menu->setLabel(val);
  else {
    if (mlabel == val) return;
    mlabel = val;
    if (button != NULL) {
      button->setText(val);
    }
    emitLabelChanged(mlabel);
  }
}

/*
void taiMenu::Update() {
  if (par_menu != NULL)
    par_menu->Update();
  else {
    if ((cur_sel != NULL) && ((cur_sel->sel_type & radio) && (cur_sel->radio_grp != -1)))
      setLabel(cur_sel->label);
//    else if (cur_sel == NULL)
//      setLabel("NULL");
  }
}*/
/* OBS
void taiMenu::Update() {
  if ((cur_sel != NULL) && ((sel_type == radio_update) || (sel_type == normal_update)))
    SetMLabel(cur_sel->label);
  else if(cur_sel == NULL)
    SetMLabel("NULL");
  return;
}
*/


//////////////////////////
// 	iAction 	//
//////////////////////////

void iAction::AddTo(taiMenu* menu) {
  menu->AddItem_FromAction(this);
}

void iAction::addedTo(int index, QPopupMenu * menu ) {
  QAction::addedTo(index, menu);
  if (menu)
    menu->setItemParameter(menu->idAt(index), param);
}

void iAction::init(int param_) {
  param = param_;
  connect(this, SIGNAL(activated()), this, SLOT(this_activated()) );
}

void iAction::this_activated() {
  emit activated(param);
}


//////////////////////////////////
// 	Action_List 		//
//////////////////////////////////

String iAction_List::El_GetName_(void* el) const {
  iAction* ia = (iAction*)el;
  return ia->name();
}


//////////////////////////////////
// 	taiEditButton	 	//
//////////////////////////////////


taiEditButton::taiEditButton(void* base, taiEdit *taie, TypeDef* typ_,
	taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiData(typ_, host_, par, gui_parent_, flags_)
{
  mgui_parent = gui_parent_;
  cur_base = base;
  if (!typ->InheritsFrom(TA_taBase) || typ->HasOption("EDIT_ONLY"))
    SetFlag(flgEditOnly, true);
  imenu = NULL;
  ie = taie;	// note: if null, it uses type's ie
  SetRep(new QPushButton(gui_parent_));
  rep()->setFixedHeight(taiM->button_height(defSize()));
  // if true edit-only button, we create a regular pushbutton and connect it to Edit action
  // otherwise, the button becomes a PopupMenu button, and we create a menu
  if (HasFlag(flgEditOnly)) {
    connect(m_rep, SIGNAL(clicked()),
        this, SLOT(Edit()) );
  } else {
    // note: gui_parent_ of menu is *not* the button, nor is it transferred to button when we set its popup
    imenu = new taiMenu(taiMenu::popupmenu, taiMenu::normal_update, taiMisc::fonSmall, typ_, host_, this, gui_parent_);
    rep()->setPopup(imenu->rep_popup());
//    connect(imenu, SIGNAL(labelChanged(const char*)),
//        this, SLOT(setLabel(const char*)) );
  }
  SetLabel();
}

taiEditButton::~taiEditButton(){
  if (ie != NULL) {
    delete ie;
    ie = NULL;
  }
  delete imenu;
  imenu = NULL;
  meth_el.Reset();
}

void taiEditButton::SetLabel() {
  String lbl = typ->name;
  if (HasFlag(flgEditOnly)) {
    lbl += ": Edit...";
  } else {
    lbl += ": Actions";
  }
  setRepLabel(lbl);
}

void taiEditButton::GetMethMenus() {
  if (meth_el.size > 0)		// only do this once..
    return;
  if (HasFlag(flgEditOnly)) return;

  String men_nm;
  String lst_men_nm;
  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    if ((md->im == NULL) || (md->name == "Close"))
      continue;
    // don't do following unless
    if ((md->name == "CopyFrom") || (md->name == "CopyTo") || (md->name == "DuplicateMe")
       || (md->name == "ChangeMyType") || (md->name == "Help"))
      continue;
    if ((HasFlag(flgReadOnly)) && !(md->HasOption("EDIT_READ_ONLY"))) continue;
    String cur_nm = md->OptionAfter("MENU_ON_");
    if (cur_nm != "")
      men_nm = cur_nm;
    // has to be on one of these two menus..
    if ((men_nm != "Object") && (men_nm != "Edit"))
      continue;
    if ((men_nm != lst_men_nm) && (lst_men_nm != ""))
      imenu->AddSep();
    lst_men_nm = men_nm;
    taiMethodData* mth_rep = md->im->GetMethodRep(cur_base, host, this, mgui_parent);
    if (mth_rep == NULL)
      continue;
    meth_el.Add(mth_rep);
    if ((ie != NULL) && (md->name == "Edit"))
      imenu->AddItem("Edit", NULL, taiMenu::use_default,
          taiMenuEl::action, this, SLOT(Edit()));
    else
      mth_rep->AddToMenu(imenu);
  }
}

void taiEditButton::GetImage(void* base) {
  cur_base = base;
  SetLabel();
  GetMethMenus();
}

void taiEditButton::Edit() {
  if (cur_base == NULL)
    return;
  const iColor* bgclr = NULL;
  if (typ->InheritsFrom(TA_taBase)) {
    bgclr = ((TAPtr)cur_base)->GetEditColorInherit();
  }
  if ((bgclr == NULL) && (host != NULL)) bgclr = host->bg_color;
/*obs  bool modal = false;
  if (host != NULL)
    modal = host->modal; */
  // note that the target of a pointer is not necessarily readonly just because
  // the pointer itself was readonly... (so we don't propagate ro to target)
  if (ie == NULL) {
    typ->ie->Edit(cur_base, false, bgclr);
  }
  else {
    ie->typ = typ;
    ie->Edit(cur_base, false, bgclr);
  }
  GetImage(cur_base);
}

void taiEditButton::setRepLabel(const char* label) {
    rep()->setText(label);
}


//////////////////////////////////////////
// 		taiObjChooser		//
//////////////////////////////////////////

class ocListBoxItem: public QListBoxText {
public:
  const void* data;
  ocListBoxItem(const QString& text_, const void* data_);
};

ocListBoxItem::ocListBoxItem(const QString& text_, const void* data_)
:QListBoxText(text_)
{
  data = data_;
}

taiObjChooser* taiObjChooser::createInstance(TAPtr parob, const char* captn, bool selonly, QWidget* par_window_) {
  if (par_window_ == NULL)
    par_window_ = qApp->mainWidget();
  return new taiObjChooser(parob, captn, selonly, par_window_);
}

taiObjChooser* taiObjChooser::createInstance(TypeDef* tpdf, const char* captn, TAPtr scope_ref_, QWidget* par_window_) {
  if (par_window_ == NULL)
    par_window_ = qApp->mainWidget();
  return new taiObjChooser(tpdf, captn, scope_ref_, par_window_);
}

void taiObjChooser::init(const char* captn, bool selonly, QWidget* par_window_) {
  lst_par_obj = NULL;
  reg_par_obj = NULL;
  typ_par_obj = NULL;
  scope_ref = NULL;
  caption = captn;
  select_only = selonly;
  msel_obj = NULL;
  setCaption(caption);
  setFont(taiM->dialogFont(taiMisc::fonSmall));
  resize(taiM->dialogSize(taiMisc::hdlg_s));
}

taiObjChooser::taiObjChooser(TAPtr parob, const char* captn, bool selonly, QWidget* par_window_)
{
  init(captn, selonly, par_window_);

  if(parob->InheritsFrom(&TA_taList_impl))
    lst_par_obj = (TABLPtr)parob;
  else
    reg_par_obj = parob;

  Build();
}

taiObjChooser::taiObjChooser(TypeDef* td, const char* captn, TAPtr scope_ref_, QWidget* par_window_)
: QDialog(par_window_, NULL, true)
{
  init(captn, true, par_window_); //select_only = true always true for typedef!

  if(!td->InheritsFrom(TA_taBase)) {
    taMisc::Error("*** warning, will not be able to select non-taBase tokens in chooser");
  }
  typ_par_obj = td;

  Build();
}

void taiObjChooser::setSel_obj(const TAPtr value) {
  if (msel_obj == value) return;
  msel_obj = value;
  for (uint i = 0; i < browser->count(); ++i) {
    ocListBoxItem* lbi = (ocListBoxItem*)browser->item(i);
    if (lbi->data == value) {
      browser->setCurrentItem(i);
      return;
    }
  }
  browser->setCurrentItem(-1);
}

bool taiObjChooser::Choose() {
  return (exec() == QDialog::Accepted);
}

void taiObjChooser::GetPathStr() {
  if(lst_par_obj != NULL)
    path_str = lst_par_obj->GetPath();
  else if(reg_par_obj != NULL)
    path_str = reg_par_obj->GetPath();
  else if(typ_par_obj != NULL)
    path_str = typ_par_obj->name;
}

void taiObjChooser::ReRead() {
  GetPathStr();
  editor->setText((const char*)path_str);
  Clear();
  Load();
}

void taiObjChooser::Build() {
  layOuter = new QGridLayout(this, 3, 1, taiM->vsep_c, taiM->vspc_c); // rows, cols, margin, space
  browser = new QListBox(this);
  layOuter->addWidget(browser, 1, 0);
  layOuter->setRowStretch(1, 1); // list is item to expand in host
  layOuter->setRowSpacing(1, 100); // don't shrink to nothing

  layButtons = new QHBoxLayout();
  layButtons->addStretch();
  btnOk = new QPushButton("&Ok", this);
  btnOk->setDefault(true);
  layButtons->addWidget(btnOk);
  layButtons->addSpacing(taiM->vsep_c);
  btnCancel = new QPushButton("&Cancel", this);
  layButtons->addWidget(btnCancel);
  layOuter->addLayout(layButtons, 2, 0);

  //note: create editor last so it is last on tab order
  GetPathStr();
  editor = new QLineEdit(path_str, this);
  layOuter->addWidget(editor, 0, 0);


  connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()) );
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()) );
  connect(browser, SIGNAL(doubleClicked(QListBoxItem*)),
      this, SLOT(browser_doubleClicked(QListBoxItem*)));
  connect(browser, SIGNAL(selectionChanged(QListBoxItem*)),
      this, SLOT(browser_selectionChanged(QListBoxItem*)));
  connect(editor, SIGNAL(returnPressed()),
      this, SLOT(AcceptEditor()) );

  Load();
}

void taiObjChooser::Clear() {
  browser->clear();
  items.Reset();
  editor->clear();
}

void taiObjChooser::Load() {
  if(lst_par_obj != NULL) {
    if(!select_only) {
      AddItem("..");
      AddObjects(lst_par_obj);
    }
    int i;
    for (i=0; i<lst_par_obj->size; i++) {
      TAPtr ob = (TAPtr)lst_par_obj->FastEl_(i);
      String lbl = ob->GetName();
      if(lbl.length() == 0)
	lbl = String("[") + String(i) + "]";
      AddItem((const char*)lbl, ob);
    }
  }
  else if(reg_par_obj != NULL) {
    if(!select_only) {
      AddItem("..");
    }
    AddObjects(reg_par_obj);
  }
  else if(typ_par_obj != NULL) {
    AddTokens(typ_par_obj);
  }
}

void taiObjChooser::AddObjects(TAPtr obj) {
  TypeDef* td = obj->GetTypeDef();
  for (int i = 0; i < td->members.size; ++i) {
    MemberDef* md = td->members.FastEl(i);
    if(!md->type->InheritsFrom(&TA_taBase)) continue;
    TAPtr mb = (TAPtr)md->GetOff((void*)obj);
    if(mb->GetOwner() == NULL) continue; // not going to be a good selection item
    AddItem((const char*)md->name, md);
  }
}

void taiObjChooser::AddTokens(TypeDef* td) {
  int i;
  for(i=0; i<td->tokens.size; i++) {
    void* tmp = td->tokens.FastEl(i);
    String adrnm = String((long)tmp);
    if(td->InheritsFrom(TA_taBase)) {
      TAPtr btmp = (TAPtr)tmp;
      if((scope_ref != NULL) && !btmp->SameScope(scope_ref))
	continue;
      if(!btmp->GetName().empty()) {
	AddItem(btmp->GetName(), tmp);
	items[items.size - 1] = adrnm;	// always store the actual address in the string!
      }
      else {
	AddItem(adrnm, tmp);
      }
    }
    else {
      AddItem(adrnm, tmp);
    }
  }

  for(i=0; i<td->children.size; i++) {
    TypeDef* chld = td->children[i];
    if(chld->ptr != 0)
      continue;
    if((chld->tokens.size == 0) && (chld->tokens.sub_tokens == 0))
      continue;
    if((chld->tokens.size > 0) || (chld->children.size > 0))
      AddTokens(chld);
  }
}

void taiObjChooser::AddItem(const char* itm, const void* data_) {
  items.Add(itm);
  browser->insertItem(new ocListBoxItem(QString(itm), data_));
  if (msel_obj == data_)
    browser->setCurrentItem(browser->count() - 1);
}

void taiObjChooser::UpdateFmSelStr() {
  msel_obj = NULL;
  if(lst_par_obj != NULL) {
    if(msel_str == "root")
      msel_obj = tabMisc::root;
    else if(msel_str == "..")
      msel_obj = lst_par_obj->GetOwner();
    else
      msel_obj = lst_par_obj->FindFromPath(msel_str);
  }
  else if(reg_par_obj != NULL) {
    if(msel_str == "root")
      msel_obj = tabMisc::root;
    else if(msel_str == "..")
      msel_obj = reg_par_obj->GetOwner();
    else
      msel_obj = reg_par_obj->FindFromPath(msel_str);
  }
  else if(typ_par_obj != NULL) {
    if(!typ_par_obj->InheritsFrom(TA_taBase))
      return;
    long adr = (long)msel_str;
    msel_obj = (TAPtr)adr;
    if((msel_obj != NULL) && !msel_obj->GetName().empty())
      msel_str = msel_obj->GetName();
  }
  if(msel_obj == NULL) {
    taMisc::Error("Could not find object:", msel_str, "in path:", path_str,"try again");
    return;
  }
}

void taiObjChooser::accept() {
  int i = browser->currentItem();
  if (i == -1) {
    if (!select_only)
      AcceptEditor_impl(NULL);	// null is clue to not fork to descend!
    else
      AcceptEditor_impl(editor);
    return;
  }
  msel_str = items.FastEl(i);

  UpdateFmSelStr();		// get the new selection based on that!
  if (msel_obj != NULL)
    QDialog::accept();
}

void taiObjChooser::browser_doubleClicked(QListBoxItem* itm) {
  if (select_only)
    accept();
  else
    DescendBrowser();
}

void taiObjChooser::browser_selectionChanged(QListBoxItem* itm) {
  //TODO: verify this is what we want to do... this is assumed behavior of IV FileBrowser
  GetPathStr();
  if (itm == NULL)
    msel_obj = NULL;
  else
    msel_obj = (TAPtr)(((ocListBoxItem*)itm)->data);

  String nw_txt;
  if (msel_obj == NULL)
    nw_txt = path_str;
  else
    nw_txt = msel_obj->GetPath();

  editor->setText(nw_txt);
}

void taiObjChooser::DescendBrowser() {
  int i = browser->currentItem();
  if (i == -1) {
    msel_str = editor->text();
    if (select_only) {
      msel_str = msel_str.after(path_str);
    }
    else {
      reg_par_obj = tabMisc::root;
      lst_par_obj = NULL;
    }
  }
  else {
    msel_str = items.FastEl(i);
  }

  UpdateFmSelStr();		// get the new selection based on that!
  if (msel_obj == NULL) return;

  String nw_txt = msel_obj->GetPath();
  editor->setText((const char*)nw_txt);

  if(msel_obj->InheritsFrom(&TA_taList_impl)) {
    lst_par_obj = (TABLPtr)msel_obj;
    reg_par_obj = NULL;
  }
  else {
    reg_par_obj = msel_obj;
    lst_par_obj = NULL;
  }
  ReRead();
}

void taiObjChooser::reject() {
  msel_obj = NULL;
  QDialog::reject();
}

void taiObjChooser::AcceptEditor() {
  AcceptEditor_impl(editor);
}

void taiObjChooser::AcceptEditor_impl(QLineEdit* e) {
  if(!select_only && (e != NULL)) {
    DescendBrowser();
    return;
  }
  if(e == NULL) e = editor;
  msel_str = e->text();
  if (select_only) {
    msel_str = msel_str.after(path_str);
    UpdateFmSelStr();		// get the new selection based on that!
    if (msel_obj != NULL)
      QDialog::accept();
  } else {
    reg_par_obj = tabMisc::root;
    lst_par_obj = NULL;
    UpdateFmSelStr();
    if(msel_obj != NULL)
      QDialog::accept();
  }
}


//////////////////////////////////////////
// 		taiFileButton		//
//////////////////////////////////////////

taiFileButton::taiFileButton(void* base, TypeDef* typ_, taiDataHost* host_, taiData* par,
	QWidget* gui_parent_, bool rd_only, bool wrt_only)
: taiData(typ_, host_, par, gui_parent_)
{
  if (base != NULL) {
    gf = *((taFiler**) base);
    if (gf != NULL)
      taRefN::Ref(gf);
  }
  else
    gf = NULL;
  read_only = rd_only;
  write_only = wrt_only;
  filemenu = new taiMenu(taiMenu::buttonmenu, taiMenu::normal, taiMisc::fonSmall, typ, host_, this, gui_parent_);
  filemenu->setLabel("------No File-----");
}

taiFileButton::~taiFileButton() {
  delete filemenu;
  if (gf != NULL)
    taRefN::unRefDone(gf);
}

QWidget* taiFileButton::GetRep() {
  return (filemenu == NULL) ? NULL : filemenu->GetRep();
}

void* taiFileButton::GetValue(){
  return (void*) gf;
}

void taiFileButton::GetImage(void* base) {
  if (gf != NULL)
    taRefN::unRefDone(gf);
  gf = *((taFiler**) base);
  if (gf != NULL)
    taRefN::Ref(gf);
  GetImage();
}

void taiFileButton::GetImage() {
  if (filemenu->items.size == 0) {
    if (!write_only)
      filemenu->AddItem("Open", NULL, taiMenu::use_default,
	taiMenuEl::action, this, SLOT(Open()) );
    if(!read_only && ((gf == NULL) || !gf->select_only)) {
      filemenu->AddItem("Save", NULL, taiMenu::use_default,
	taiMenuEl::action, this, SLOT(Save()) );
      filemenu->AddItem("SaveAs", NULL, taiMenu::use_default,
	taiMenuEl::action, this, SLOT(SaveAs()) );
      filemenu->AddItem("Append", NULL, taiMenu::use_default,
	taiMenuEl::action, this, SLOT(Append()) );
    }
    filemenu->AddItem("Close", NULL, taiMenu::use_default,
	taiMenuEl::action, this, SLOT(Close()) );
    filemenu->AddItem("Edit", NULL, taiMenu::use_default,
	taiMenuEl::action, this, SLOT(Edit()) );
  }

  if ((gf == NULL) || (!gf->select_only && !gf->open_file) || (gf->fname == ""))
    filemenu->setLabel("------No File-----");
  else
    filemenu->setLabel(gf->fname);
}

void taiFileButton::GetGetFile() {
  if (gf ==  NULL) {
    gf = taFiler_CreateInstance(".","",false);
    taRefN::Ref(gf);
  }
}

void taiFileButton::Open() {
  GetGetFile();
  if ((gf->Open() != NULL) || (gf->select_only)) {
    GetImage();
  }
}

void taiFileButton::Append() {
  GetGetFile();
  if(gf->Append() != NULL) {
    GetImage();
  }
}

void taiFileButton::Save() {
  GetGetFile();
  if (gf->Save() != NULL) {
    GetImage();
  }
}
void taiFileButton::SaveAs() {
  GetGetFile();
  if (gf->SaveAs() != NULL) {
    GetImage();
  }
}
void taiFileButton::Close() {
  GetGetFile();
  gf->Close();
  if(gf->select_only)
    gf->fname = "";		// reset file name on close
  GetImage();
}

void taiFileButton::Edit() {
  GetGetFile();
  char* edtr_c = getenv("EDITOR");
  String edtr = "emacs";
  if (edtr_c != NULL)
    edtr = edtr_c;
  edtr += String(" ") + gf->fname + " &";
  system(edtr);
}


taiElBase::taiElBase(taiMenu* menu_, TypeDef* tp, taiDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_)
:taiData(tp, host_, par, gui_parent_, flags_)
{
  cur_obj = NULL;
  ta_menu = menu_;
  ownflag = false;
}

taiElBase::~taiElBase() {
  if (ownflag)
    delete ta_menu;
  ta_menu = NULL;
}

void taiElBase::setCur_obj(TAPtr value, bool do_chng) {
  if (cur_obj == value) return;
  cur_obj = value;
  if (value == NULL)
    ta_menu->setLabel("NULL");
  else
    ta_menu->setLabel(value->GetName());
  if (do_chng)
    DataChanged(NULL);
}

//////////////////////////
// 	taiToken	//
//////////////////////////

taiToken::taiToken(int rt, int ft, TypeDef* typ_, taiDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_)
: taiElBase(NULL, typ_, host_, par, gui_parent_, flags_)
{
  ta_menu = new taiMenu(rt, taiMenu::radio_update, ft, typ_, host_, this, gui_parent_);
  ownflag = true;
  scope_ref = NULL;
}

/*taiToken::taiToken(taiMenu* existing_menu, TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent_,
	bool nul_not, bool edt_not)
: taiData(typ_, host_, par, gui_parent_)
{
  ta_menu = existing_menu;
  ownflag = false;
  mscope_ref = NULL;
  null_not = nul_not;
  edit_not = edt_not;
  over_max = false;
  chs_obj = NULL;
}*/

void taiToken::GetImage(TAPtr ths, TAPtr scp_obj) {
  scope_ref = scp_obj;
  ta_menu->GetImage(ths);
  setCur_obj(ths, false);
}

TAPtr taiToken::GetValue() {
  return cur_obj;
}

void taiToken::ItemChosen(taiMenuEl* menu_el) {
  setCur_obj((TAPtr)(menu_el->usr_data));
}

void taiToken::Edit() {
  void* cur_base = GetValue();
  if(cur_base == NULL) return;

  taiEdit* gc;
  const iColor* bgclr = NULL;
  if (typ->InheritsFrom(TA_taBase)) {
    bgclr = ((TAPtr)cur_base)->GetEditColorInherit();
    gc = (taiEdit*) ((taBase*)cur_base)->GetTypeDef()->ie;
  }
  else {
    gc = (taiEdit*)typ->ie;
  }
  if ((bgclr == NULL) && (host != NULL)) bgclr = host->bg_color;
/*obs  bool wait = false;
  if (host != NULL) wait = host->modal; */

  gc->Edit(cur_base, false, bgclr);
}

void taiToken::Chooser() {
  QWidget* par_window = (host == NULL) ? NULL : host->widget();
  taiObjChooser* chs = taiObjChooser::createInstance(typ, "Tokens of Given Type", scope_ref, par_window);
  chs->setSel_obj(cur_obj); // set initial selection
  bool rval = chs->Choose();
  if (rval) {
    setCur_obj((TAPtr)chs->sel_obj()); //TODO: ***DANGEROUS CAST*** -- could possibly be non-taBase type!!!
 /*TODO: can we even do this??? is there ever actions for radio button items???   if ((ta_menu->cur_sel != NULL) && (ta_menu->cur_sel->label == "<Over max, select...>") &&
       (ta_menu->cur_sel->men_act != NULL)) {
      ta_menu->cur_sel->usr_data = (void*)chs_obj;
      ta_menu->cur_sel->men_act->Select(ta_menu->cur_sel); // call it!
    }
    else
      ta_menu->setLabel(chs->sel_str());*/
  }
  delete chs;
}

void taiToken::GetMenu(const taiMenuAction* actn) {
  if (ownflag) {
    if (HasFlag(flgEditOk))
      ta_menu->AddItem("Edit...", NULL, taiMenu::normal, taiMenuEl::action, this, SLOT(Edit()) );
    ta_menu->AddSep();
    if (HasFlag(flgNullOk)) {
      taiMenuEl* mel = ta_menu->AddItem("NULL", NULL, taiMenu::radio, actn);
      mel->connect(taiMenuEl::men_act, this, SLOT(ItemChosen(taiMenuEl*)));
    }
  }
  GetMenu_impl(ta_menu, typ, actn);
}
void taiToken::UpdateMenu(const taiMenuAction* actn) {
  ta_menu->ResetMenu();
  GetMenu(actn);
}

void taiToken::GetMenu_impl(taiMenu* menu, TypeDef* td, const taiMenuAction* actn) {
  if (!td->InheritsFrom(TA_taBase)) return; // sanity check, so we don't crash...

  if (!td->tokens.keep) {
    menu->AddItem("<Sorry, not keeping tokens>", (void*)NULL, taiMenu::normal);
  } else {
    bool too_many = false;
    if (scope_ref != NULL) {
      int cnt = taBase::NTokensInScope(td, scope_ref);
      if (cnt > taMisc::max_menu)
        too_many = true;
    }
    else if (td->tokens.size > taMisc::max_menu) {
      too_many = true;
    }
    if (too_many) {
      taiMenuEl* mnel = menu->AddItem
        ("<Over max, select...>", (void*)NULL, taiMenu::normal,
        taiMenuEl::action, this, SLOT(Chooser()) );
      if (actn != NULL) {		// also set callback action!
        mnel->connect(actn);
      }
    } else {
      taiMenuAction ma(this, SLOT(ItemChosen(taiMenuEl*)));
      String	nm;
      for (int i = 0; i < td->tokens.size; ++i) {
        TAPtr btmp = (TAPtr)td->tokens.FastEl(i);
        if ((scope_ref != NULL) && !btmp->SameScope(scope_ref))
          continue;
        if (!btmp->GetName().empty())
          nm = btmp->GetName();
        taiMenuEl* mel = menu->AddItem((char*)nm, btmp, taiMenu::radio, actn); //connect caller's callback
        mel->connect(&ma); // connect our own callback
      }
    } // too_many
  } // !td.tokens.keep

  if (td->children.size == 0)
    return;

  if (td->tokens.size > 0)
    menu->AddSep();

  for (int i = 0; i < td->children.size; ++i) {
    TypeDef* chld = td->children[i];
    if (chld->ptr != 0)
      continue;
    if ((chld->tokens.size == 0) && (chld->tokens.sub_tokens == 0))
      continue;
    if (chld->tokens.size != 0) {
      taiMenu* submenu = menu->AddSubMenu(chld->name, (void*)chld);
//huh?? why??      menu->AddSep();
      GetMenu_impl(submenu, chld, actn);
    } else {
      GetMenu_impl(menu, chld, actn);	// if no tokens, don't add a submenu..
    }
  }
}
/* taiToken is by definition only for taBase descendants -- therefore, we nuke all TA_taBase stuff...
void taiToken::GetMenu_impl(taiMenu* menu, TypeDef* td, const taiMenuAction* actn) {
  String	nm;
  bool too_many = false;
  if (td->InheritsFrom(TA_taBase) && (scope_ref != NULL)) {
    int cnt = taBase::NTokensInScope(td, scope_ref);
    if (cnt > taMisc::max_menu)
      too_many = true;
  }
  else if (td->tokens.size > taMisc::max_menu) {
    too_many = true;
  }
  if (too_many) {
    taiMenuEl* mnel = menu->AddItem
      ("<Over max, select...>", (void*)NULL, taiMenu::normal,
       taiMenuEl::action, this, SLOT(Chooser()) );
    over_max = true;
    if (actn != NULL) {		// also set callback action!
      mnel->connect(actn);
    }
  } else {
    if (!td->tokens.keep) {
      menu->AddItem("<Sorry, not keeping tokens>", (void*)NULL, taiMenu::normal);
    } else {
      for (int i = 0; i < td->tokens.size; ++i) {
	void* tmp = td->tokens.FastEl(i);
	nm = String((long)tmp);
	if (td->InheritsFrom(TA_taBase)) {
	  TAPtr btmp = (TAPtr)tmp;
	  if ((scope_ref != NULL) && !btmp->SameScope(scope_ref))
	    continue;
	  if (!btmp->GetName().empty())
	    nm = btmp->GetName();
	}
	menu->AddItem((char*)nm, tmp, taiMenu::radio_update, actn);
      }
    } // !td->tokens.keep
  } // too_many

  if (td->children.size == 0)
    return;

  if (td->tokens.size > 0)
    menu->AddSep();

  for (int i = 0; i < td->children.size; ++i) {
    TypeDef* chld = td->children[i];
    if (chld->ptr != 0)
      continue;
    if ((chld->tokens.size == 0) && (chld->tokens.sub_tokens == 0))
      continue;
    if (chld->tokens.size != 0) {
      taiMenu* submenu = menu->AddSubMenu(chld->name, (void*)chld);
//huh?? why??      menu->AddSep();
      GetMenu_impl(submenu, chld, actn);
    } else {
      GetMenu_impl(menu, chld, actn);	// if no tokens, don't add a submenu..
    }
  }
}*/


//////////////////////////////////
// 	taiSubToken		//
//////////////////////////////////

taiSubToken::taiSubToken(int rt, int ft, TypeDef* typ_, taiDataHost* host_, taiData* par,
	QWidget* gui_parent_, int flags_)
: taiElBase(NULL, typ_, host_, par, gui_parent_, flags_)
{
  menubase = NULL;
  ta_menu = new taiMenu(rt, taiMenu::radio_update, ft, typ_, host_, this, gui_parent_); //note: only needs taiMenu, but this doesn't hurt
  ownflag = true;
}

taiSubToken::taiSubToken(taiMenu* existing_menu, TypeDef* typ_, taiDataHost* host_,
	taiData* par, QWidget* gui_parent_, int flags_)
: taiElBase(existing_menu, typ_, host_, par, gui_parent_, flags_)
{
  menubase = NULL;
}

QWidget* taiSubToken::GetRep() {
   return (ta_menu == NULL) ? NULL : ta_menu->GetRep();
}

void* taiSubToken::GetValue() {
  taiMenuEl* cur = ta_menu->GetValue();
  if (cur == NULL)
    return NULL;
  else
    return cur->usr_data;
}

void taiSubToken::Edit() {
  void* cur_base = GetValue();
  if (cur_base == NULL) return;

  taiEdit* gc;
  const iColor* bgclr = NULL;
  if(typ->InheritsFrom(TA_taBase)) {
    bgclr = ((TAPtr)cur_base)->GetEditColorInherit();
    gc = (taiEdit*) ((taBase*)cur_base)->GetTypeDef()->ie;
  }
  else {
    gc = (taiEdit*)typ->ie;
  }

  if ((bgclr == NULL) && (host != NULL)) bgclr = host->bg_color;
/*obs  bool modal = false;
  if (host != NULL) modal = host->modal; */

  gc->Edit(cur_base, false, bgclr);
}

void taiSubToken::GetImage(void* ths, void* sel) {
  if (menubase != ths) {
    menubase = ths;
    UpdateMenu();
  }
  if (sel == NULL)
    sel = ths;
  if (!(ta_menu->GetImage(sel)))
    ta_menu->GetImage(ths);
}

void taiSubToken::UpdateMenu(taiMenuAction* actn){
  ta_menu->ResetMenu();
  GetMenu(actn);
}

void taiSubToken::GetMenu(taiMenuAction* actn) {
  if (HasFlag(flgNullOk))
    ta_menu->AddItem("NULL", NULL, taiMenu::use_default, actn);
  if (HasFlag(flgEditOk))
    ta_menu->AddItem("Edit", NULL, taiMenu::normal,
      taiMenuEl::action, this, SLOT(Edit()) );
  ta_menu->AddSep(); // note: never adds spurious seps

  GetMenuImpl(menubase, actn);
}

void taiSubToken::GetMenuImpl(void* base, taiMenuAction* actn){
  if (base== NULL) return;
  taBase* rbase = (taBase*) base;
  taBase** memb;
  TypeDef* basetd = rbase->GetTypeDef();

  // if you're the right type, put yourself on the list
  if (basetd->DerivesFrom(typ)) {
    String nm = rbase->GetName();
    ta_menu->AddItem(nm, rbase, taiMenu::use_default, actn);
  }

  // put your typed members on the list
  MemberDef* md;
  for (int i = 0; i <basetd->members.size; ++i){
    md = basetd->members.FastEl(i);
    if (md->type->DerivesFrom(typ) && !(md->HasOption("NO_SUBTYPE"))) {
      memb = (taBase **) md->GetOff((void*)rbase);
      if ((*memb != NULL) && ((void *) *memb != (void *) rbase)) {
	GetMenuImpl((void *) *memb, actn);
      }
    }
  }
}


//////////////////////////////////
// 	taiMemberDefMenu	//
//////////////////////////////////

taiMemberDefMenu::taiMemberDefMenu(int rt, int ft, MemberDef* m, TypeDef* targ_typ_, TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiData(typ_, host_, par, gui_parent_, flags_)
{
  md = m;
  targ_typ = targ_typ_; // if NULL, will get determined in GetMenu
  menubase = NULL;
//  sp =  NULL;
  String nm;
  if (md != NULL)
    nm = md->name;
  ta_menu = new taiMenu(rt, taiMenu::radio_update, ft, NULL, host_, this, gui_parent_);
//  ta_menu->setLabel(nm);
}

taiMemberDefMenu::~taiMemberDefMenu() {
  if (ta_menu != NULL)
    delete ta_menu;
}
QWidget* taiMemberDefMenu::GetRep() {
  return ta_menu->GetRep();
}

MemberDef* taiMemberDefMenu::GetValue() {
  taiMenuEl* cur = ta_menu->GetValue();
  if (cur == NULL)
    return NULL;
  else
    return (MemberDef*)(cur->usr_data);
}

void taiMemberDefMenu::GetImage(void* base, bool get_menu, void* cur_sel){
  menubase = base;
  if (typ == NULL)  return;
  if (get_menu)
    GetMenu(base);
  if ((cur_sel == NULL) || (!(ta_menu->GetImage(cur_sel))))
    ta_menu->GetImage(ta_menu->items.SafeEl(0));
}

void taiMemberDefMenu::GetTarget() {
  if (md == NULL)  return;
  // a memberdef* can have one of three options to specify the
  // target type for its memberdefmenu.
  // 1) a TYPE_xxxx in its comment directives
  // 2) a TYPE_ON_xxx in is comment directives, specifying the name
  //    of the member in the same object which is a TypeDef*
  // 3) Nothing, which defaults to the type of the object the memberdef
  //      is in.

  String mb_nm = md->OptionAfter("TYPE_ON_");
  if (mb_nm != "") {
//    taBase* base = (taBase*)host->cur_base; //TODO: highly unsafe cast -- should provide As_taBase() or such in taiDialog
    if (menubase != NULL) {
      void* adr; // discarded
      MemberDef* tdmd = ((taBase*)menubase)->FindMembeR(mb_nm, adr); //TODO: highly unsafe cast!!
      if (tdmd != NULL)
        targ_typ = *((TypeDef **) tdmd->GetOff(menubase));
    }
  } else {
    mb_nm = md->OptionAfter("TYPE_");
    if (mb_nm != "") {
      targ_typ = taMisc::types.FindName((char *) mb_nm);
    } else {
      targ_typ = typ;
    }
  }
}

void taiMemberDefMenu::GetMenu(void* base) {
  menubase = base;
  ta_menu->ResetMenu();
  if (targ_typ == NULL) {
    GetTarget();
  }

  if (targ_typ == NULL) {
    ta_menu->AddItem("!!!TypeSpace Error!!!", NULL);
    return;
  }

  // determine list filter
  taMisc::ShowMembs show = taMisc::NO_HID_DET;

  // if the target type is member-containing type, then optionally filter members by visibility of its host
  if (targ_typ == typ) {
    if ((md != NULL)  && (md->HasOption("TYPESHOW_VISIBLE"))) {
      taiEditDataHost* dlg = taiM->FindEdit(menubase, typ); //NOTE: finds any, in any window or viewer
      if (dlg != NULL) {
        show = dlg->show;
      }
    }
  }

  MemberSpace& mbs = targ_typ->members;
  for (int i = 0; i < mbs.size; ++i) {
    MemberDef* mbd = mbs.FastEl(i);
    if (!mbd->ShowMember(show)) continue;
    ta_menu->AddItem(mbd->GetLabel(), mbd);
  }
}


//////////////////////////////////
// 	taiMethodDefMenu	//
//////////////////////////////////

taiMethodDefMenu::taiMethodDefMenu(int rt, int ft, MethodDef* m, TypeDef* typ_, taiDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_)
: taiData(typ_, host_, par, gui_parent_, flags_)
{
  md = m;
  menubase = NULL;
  sp =  NULL;
  String nm;
  if (md != NULL)
    nm = md->name;
  ta_menu = new taiMenu(rt, taiMenu::radio_update, ft, NULL, host_, this, gui_parent_);
  ta_menu->setLabel(nm);
}

taiMethodDefMenu::~taiMethodDefMenu() {
  if (ta_menu != NULL)
    delete ta_menu;
}

QWidget* taiMethodDefMenu::GetRep() {
  return ta_menu->GetRep();
}

void* taiMethodDefMenu::GetValue() {
  taiMenuEl* cur = ta_menu->GetValue();
  if (cur != NULL)
    return cur->usr_data;
  return NULL;
}

void taiMethodDefMenu::GetImage(MethodSpace* space, MethodDef* memb) {
  sp = space;
  md = memb;
  typ = NULL;
  UpdateMenu();
  ta_menu->GetImage(md);
}

void taiMethodDefMenu::GetImage(TypeDef* type, MethodDef* memb){
  md = memb;
  typ = type;
  sp = NULL;
  UpdateMenu();
  ta_menu->GetImage(md);
}

void taiMethodDefMenu::GetImage(void* ths, void* sel) {
  if (menubase != ths) {
    menubase = ths;
  }

  String mb_nm = md->OptionAfter("TYPE_ON_");
  if (mb_nm != "") {
    void* adr;
    MemberDef* tdmd = ((taBase*) menubase)->FindMembeR(mb_nm, adr);
    if(tdmd == NULL) return;
    typ = *((TypeDef **) tdmd->GetOff(ths));
  } else {
    mb_nm = md->OptionAfter("TYPE_");
    if (mb_nm != "")
      typ = taMisc::types.FindName((char *) mb_nm);
    else
      typ = ((taBase *)ths)->GetTypeDef();
  }
  sp =  NULL;
  UpdateMenu();
  if (!(ta_menu->GetImage(sel)))
    ta_menu->GetImage(ta_menu->items.SafeEl(0));
}

void taiMethodDefMenu::UpdateMenu(const taiMenuAction* actn) {
  ta_menu->ResetMenu();
  GetMenu(actn);
}

void taiMethodDefMenu::GetMenu(const taiMenuAction* actn) {
  if ((typ == NULL) && (sp == NULL)) {
    ta_menu->AddItem("TypeSpace Error", NULL);
    return;
  }
  MethodDef* mbd;
  MethodSpace* mbs = sp;
  if (mbs == NULL) mbs = &typ->methods;
  for (int i = 0; i < mbs->size; ++i){
    mbd = mbs->FastEl(i);
    ta_menu->AddItem(mbd->GetLabel(),mbd,taiMenu::use_default, actn);
  }
}

//////////////////////////////////
// 	taiTypeHier		//
//////////////////////////////////

taiTypeHier::taiTypeHier(int  rt, int ft, TypeDef* typ_, taiDataHost* host_,
	taiData* par, QWidget* gui_parent_, int flags_)
: taiData(typ_, host_, par, gui_parent_, flags_)
{
  ta_menu = new taiMenu(rt, taiMenu::radio_update, ft, typ, host_, this, gui_parent_);
  ownflag = true;
}

taiTypeHier::taiTypeHier(taiMenu* existing_menu, TypeDef* typ_,
	taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiData(typ_, host_, par, gui_parent_, flags_)
{
  ta_menu = existing_menu;
  ownflag = false;
}

taiTypeHier::~taiTypeHier() {
  if(ownflag && (ta_menu != NULL)) delete ta_menu;
  ta_menu = NULL;
}

QWidget* taiTypeHier::GetRep() {
  return ta_menu->GetRep();
}

void taiTypeHier::GetImage(TypeDef* ths) {
  ta_menu->GetImage((void*)ths);
}

void taiTypeHier::GetMenu(const taiMenuAction* acn) {
  if (HasFlag(flgNullOk))
    ta_menu->AddItem("NULL", NULL, taiMenu::use_default, acn);
  GetMenu_impl(ta_menu, typ, acn);
}

void taiTypeHier::UpdateMenu(const taiMenuAction* acn) {
  ta_menu->ResetMenu();
  GetMenu(acn);
}

int  taiTypeHier::CountChildren(TypeDef* td) {
  int rval = 0;
  TypeDef*	chld;
  for (int i = 0; i < td->children.size; ++i) {
    chld = td->children[i];
    if (chld->ptr != 0)
      continue;
    ++rval;
  }
  return rval;
}

bool taiTypeHier::AddType(TypeDef* typ_) {
  if (!typ_->InheritsFormal(TA_class)) // only type classes please..
    return false;
  // don't add any template instances that have a single further subclass
  // (use the subclass instead)
  if (typ_->InheritsFormal(TA_templ_inst)) {
    if ((typ_->children.size != 1) || (typ_->children.FastEl(0)->parents.size > 1))
      return true;
    return false;
  }
  if((typ_->members.size==0) && (typ_->methods.size==0) && !(typ_ == &TA_taBase))
    return false;		// don't clutter list with these..
  return true;
}

void taiTypeHier::GetMenu_impl(taiMenu* menu, TypeDef* typ_, const taiMenuAction* acn) {
  menu->AddItem((char*)typ_->name, (void*)typ_, taiMenu::use_default, acn);
  menu->AddSep(false); //don't start new radio group
  for (int i = 0; i < typ_->children.size; ++i) {
    TypeDef* chld = typ_->children.FastEl(i);
    if (chld->ptr != 0)
      continue;
    if (!AddType(chld)) {
      if (chld->InheritsFormal(TA_templ_inst) && (chld->children.size == 1)) {
	GetMenu_impl(menu, chld->children.FastEl(0), acn);
      }
      continue;
    }

    if (CountChildren(chld) > 0) {
      taiMenu* chld_menu = menu->AddSubMenu((char*)chld->name, (void*)chld);
      GetMenu_impl(chld_menu, chld, acn);
    }
    else
      menu->AddItem((char*)chld->name, (void*)chld, taiMenu::use_default, acn);
  }
}

TypeDef* taiTypeHier::GetValue() {
  taiMenuEl* cur = ta_menu->GetValue();
  if(cur) return (TypeDef*)cur->usr_data; return NULL;
}


///////////////////////////
// 	taiMethodData     //
///////////////////////////

taiMethodData::taiMethodData(void* bs, MethodDef* md, TypeDef* typ_, taiDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_)
: taiData(typ_, host_, par, gui_parent_, flags_) {
  base = bs;
  meth = md;
  gui_parent = gui_parent_;
  if ((base != NULL) && typ->InheritsFrom(TA_taBase)) {
    typ = ((TAPtr)base)->GetTypeDef(); // get the *actual* type def of this object!
  }
  is_menu_item = false;
  args = NULL;
  arg_dlg = NULL;
  use_argc = 0;
  buttonRep = NULL;
}

void taiMethodData::AddToMenu(taiMenu* mnu) {
  if (meth->HasOption("MENU_SEP_BEFORE"))
    mnu->AddSep();
  mnu->AddItem(meth->GetLabel(), NULL, taiMenu::use_default,
	taiMenuEl::action, this, SLOT(CallFun()) );
  if (meth->HasOption("MENU_SEP_AFTER"))
    mnu->AddSep();
}


QPushButton* taiMethodData::makeButton() {
  if (buttonRep == NULL) {
    buttonRep = new QPushButton(meth->GetLabel(), gui_parent);
    connect(buttonRep, SIGNAL(clicked()),
      this, SLOT(CallFun()) );
    buttonRep->show();
  }
  return buttonRep;
}

void taiMethodData::CallFun() {
  if ((meth->stubp == NULL) || (base == NULL))
    return;
  ApplyBefore();
  use_argc = meth->fun_argc;
  String argc_str = meth->OptionAfter("ARGC_");
  if (argc_str != "")
    use_argc = (int)argc_str;
  use_argc = MIN(use_argc, meth->arg_types.size);
  use_argc = MIN(use_argc, meth->arg_names.size);
  if ((use_argc == 0) && !meth->HasOption("CONFIRM")) {
    GenerateScript();
#ifdef DMEM_COMPILE
    // don't actually run the command when using gui in dmem mode: everything happens via the script!
    if (taMisc::dmem_nprocs == 1) {
#endif
      cssEl* rval = (*(meth->stubp))(base, 0, (cssEl**)NULL);
      UpdateAfter();
      if (rval != &cssMisc::Void)
	ShowReturnVal(rval);
#ifdef DMEM_COMPILE
    }
#endif
    return;
  }
  arg_dlg = new cssiArgDialog(meth, typ, base, use_argc, 0); //modal
  const iColor* bgclr = NULL;
  if (typ->InheritsFrom(TA_taBase)) {
    bgclr = ((TAPtr)base)->GetEditColorInherit();
  }
  if ((bgclr == NULL) && (host != NULL)) bgclr = host->bg_color;
  arg_dlg->Constr("", "", bgclr);
  int ok_can = arg_dlg->Edit(true);	// true = wait for a response
  if (ok_can && !arg_dlg->err_flag) {
    GenerateScript();
#ifdef DMEM_COMPILE
    // don't actually run the command when using gui in dmem mode: everything happens via the script!
    if (taMisc::dmem_nprocs == 1) {
#endif
      cssEl* rval = (*(meth->stubp))(base, arg_dlg->obj->members->size-1,
				     arg_dlg->obj->members->els);
      UpdateAfter();
      if (rval != &cssMisc::Void)
	ShowReturnVal(rval);
#ifdef DMEM_COMPILE
    }
#endif
  }
  delete arg_dlg;
}

void taiMethodData::ShowReturnVal(cssEl* rval) {
  if (!(meth->HasOption("USE_RVAL")  ||
       (meth->HasOption("USE_RVAL_RMB") && (arg_dlg->mouse_button == Qt::RightButton)) ))
    return;

  if ((rval->GetType() == cssEl::T_TA) || (rval->GetType() == cssEl::T_Class)) {
    if (host != NULL)
      rval->Edit(host->modal);
    else
      rval->Edit(false);
    return;
  }
  String val = meth->name + " Return Value: ";
  val += rval->PrintStr();
  taiChoiceDialog::ChoiceDialog(NULL, val, "Return Value", true);
}

void taiMethodData::ApplyBefore() {
  if ((host == NULL) || (host->state != taiDataHost::ACTIVE))
    return;
  if (meth->HasOption("NO_APPLY_BEFORE") || !host->HasChanged())
    return;
  if (taMisc::auto_revert == taMisc::CONFIRM_REVERT) {
    int chs = taiChoiceDialog::ChoiceDialog
      (NULL, "Auto Apply/Revert: You have edited the dialog--apply or revert and lose changes?,Apply,Revert");
    if (chs == 0)
      host->GetValue();
  }
  else {
    host->GetValue();
  }
}

void taiMethodData::UpdateAfter() {
  if (meth->HasOption("NO_REVERT_AFTER"))
    return;
  // this is for stuff just called from menus, not host
  if ((host == NULL) || (host->state != taiDataHost::ACTIVE)) {
    if(base == NULL) return;
    TAPtr tap = (TAPtr)base;
    if (meth->HasOption("UPDATE_MENUS"))
      taiMisc::Update(tap);
    return;
  }
  // this is inside the host itself
  if ((host->typ != NULL) && host->typ->InheritsFrom(TA_taBase)) {
    TAPtr tap = (TAPtr)host->cur_base;
    if (meth->HasOption("UPDATE_MENUS"))
      taiMisc::Update(tap);	// update menus and stuff
  }
  // almost always revert host..
  host->Revert();		// apply stuff dealt with already
}

void taiMethodData::GenerateScript() {
  if((taMisc::record_script == NULL) || !typ->InheritsFrom(TA_taBase))
    return;

#ifndef DMEM_COMPILE
  // dmem requires everything to be scripted to share commands!
  if(meth->HasOption("NO_SCRIPT")) {
    return;
  }
#endif

  TAPtr tab = (TAPtr)base;

  if ((use_argc == 0) || (arg_dlg == NULL)) {
    String rscr = tab->GetPath() + "." + meth->name + "();";
#ifdef DMEM_COMPILE
    if(taMisc::dmem_debug)
      cerr << "proc: " << taMisc::dmem_proc << " recording fun call: " << rscr << endl;
#endif
    *(taMisc::record_script) << rscr << endl;
    return;
  }

  int_Array tmp_objs;		// indicies of the temp objects
  for (int i = 0; i < arg_dlg->type_el.size; ++i) {
    taiArgType* art = (taiArgType*)arg_dlg->type_el.FastEl(i);
    if((art->arg_typ->ptr == 0) && art->arg_typ->DerivesFrom(TA_taBase))
      tmp_objs.Add(i+1);
  }
  if (tmp_objs.size > 0) {
    *taMisc::record_script << "{ "; // put in context
    for (int i = 0; i < tmp_objs.size; ++i) {
      taiArgType* art = (taiArgType*)arg_dlg->type_el.FastEl(tmp_objs.FastEl(i) - 1);
      cssEl* argv = arg_dlg->obj->members->FastEl(tmp_objs.FastEl(i));
      *taMisc::record_script << art->arg_typ->name << " tmp_" << i
			       << " = new " << art->arg_typ->name << "; \t tmp_" << i
			       << " = \"" << argv->GetStr() << "\";\n  ";
    }
  }

  String arg_str;
  for (int i = 1; i < arg_dlg->obj->members->size; ++i) {
    if (i > 1)
      arg_str += ", ";
    int idx;
    if((idx = tmp_objs.Find(i)) >= 0)
      arg_str += "tmp_" + String(idx);
    else {
      cssEl* argv = arg_dlg->obj->members->FastEl(i);
      if(argv->GetType() == cssEl::T_String) {
	arg_str += "\"";
	arg_str += argv->PrintFStr();
	arg_str += "\"";
      }
      else if(argv->GetType() == cssEl::T_TA) {
	cssTA* cssta = (cssTA*)argv;
	TypeDef* csstd = cssta->type_def;
	if(csstd->InheritsFrom(&TA_istream) || csstd->InheritsFrom(&TA_fstream)
	   || csstd->InheritsFrom(&TA_ostream))
	  arg_str += "\"" + taFiler::last_fname + "\"";
	else {
	  arg_str += argv->PrintFStr();
	}
      }
      else {
	arg_str += argv->PrintFStr();
      }
    }
  }

  String rscr = tab->GetPath() + "." + meth->name + "(" + arg_str + ");\n";
  *taMisc::record_script << rscr;
#ifdef DMEM_COMPILE
    if(taMisc::dmem_debug)
      cerr << "proc: " << taMisc::dmem_proc << " recording fun call: " << rscr << endl;
#endif
  if (tmp_objs.size > 0) {
    for (int i = 0; i < tmp_objs.size; ++i) {
      *taMisc::record_script << "  delete tmp_" << i << ";\n";
    }
    *taMisc::record_script << "}\n"; // put in context
  }
  taMisc::record_script->flush();
}


/////////////////////////////
// 	taiMethMenu    //
/////////////////////////////

taiMethMenu::taiMethMenu(void* bs, MethodDef* md, TypeDef* typ_, taiDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_)
: taiMethodData(bs, md, typ_, host_, par, gui_parent_, flags_) {
  is_menu_item = true;
}


/////////////////////////////
// 	taiMethButton    //
/////////////////////////////

taiMethButton::taiMethButton(void* bs, MethodDef* md, TypeDef* typ_, taiDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_)
: taiMethodData(bs, md, typ_, host_, par, gui_parent_, flags_)
{
  is_menu_item = false;
  if (gui_parent) SetRep(makeButton()); //note: later code in win_base.cc etc. has convoluted menu
  // code that will end up spuriously invoking this, unless we prevent it.
}


//////////////////////////////
// 	taiMethToggle     //
//////////////////////////////

taiMethToggle::taiMethToggle(void* bs, MethodDef* md, TypeDef* typ_, taiDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_)
: taiMethodData(bs, md, typ_, host_, par, gui_parent_, flags_)
{
  SetRep(new QCheckBox(md->name, gui_parent_));
  connect(rep(), SIGNAL(toggled(bool)), this, SLOT(CallFun()) );
}

void taiMethToggle::CallFun() {
  if (rep()->isChecked())
    taiMethodData::CallFun();
}



