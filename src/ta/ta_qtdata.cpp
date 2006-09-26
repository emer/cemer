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


// ta_qtdata.cpp

#include "ta_qtdata.h"

#include "ta_filer.h"
#include "ta_matrix.h"
#include "ta_script.h"
#include "ta_project.h"
#include "ta_qt.h"
#include "ta_qtdialog.h"
#include "ta_qttype_def.h"
#include "ta_qtclipdata.h" // for clip-aware controls
#include "css_basic_types.h"
#include "css_qtdialog.h"
#include "ta_css.h"
#include "ta_TA_type.h"

#include "icolor.h"
#include "icheckbox.h"
#include "icombobox.h"
#include "idimedit.h"
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
#include <Q3ListBox>
#include <Q3ListBoxText>
#include <QMenu>
#include <QMenuBar>
#include <qpushbutton.h>
#include <QStackedWidget>
#include <qstring.h>
#include <QTextEdit>
#include <QTimer>
#include <QToolButton>
#include <qtooltip.h>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <Q3WidgetStack>

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>

// pointer alignment for putting up windows based on the mouse
#define POINTER_ALIGN_X		.5
#define POINTER_ALIGN_Y		.5

using namespace Qt;

class cssiArgDialog;


//////////////////////////
//    taiDataList	//
//////////////////////////

void taiDataList::El_Done_(void* it) {
  delete (taiData*)it;
}

//////////////////////////////////////////////////////////
// 	taiData: glyphs to represent kinds of data	//
//////////////////////////////////////////////////////////

// NOTE: this is for ta_TA.cpp only
taiData::taiData()
:QObject()
{
  typ = NULL;
  host = NULL;
  mhighlight = false;
  m_rep = NULL;
  mparent = NULL;
  mflags = 0;
}

taiData::taiData(TypeDef* typ_, IDataHost* host_, taiData* parent_, QWidget* gui_widget, int flags_)
:QObject()
{
  typ = typ_;
  host = host_;
  mhighlight = false;
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
/*  else if (host != NULL)
    return host->ctrl_size; */
  else return taiM->ctrl_size;
}

void taiData::Delete() {
//NOTE: to extend this method, delete your own additional Qt gui thingies, then call us
//DO NOT CALL INHERITED THEN DO MORE STUFF YOURSELF
  if (m_rep) {
    delete m_rep;
    m_rep = NULL;
  }
  if (mparent)
    setParent(NULL); // parent deletes us
  else
    delete this;
  //NO MORE CODE, WE ARE DELETED!!!
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

void taiData::setHighlight(bool value) {
  if (mhighlight == value) return;
  mhighlight == value;
  emit settingHighlight(value);
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

taiCompData::taiCompData(TypeDef* typ_, IDataHost* host_, taiData* parent_, QWidget* gui_parent_, int flags_)
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
  lay->setMargin(0); // in Qt4 it adds style-dependent defaults
  last_spc = taiM->hsep_c; // give it a bit of room
}

void taiCompData::AddChildMember(MemberDef* md) {
  String desc = md->desc;

  // add caption
  String nm = md->GetLabel();
  QLabel* lbl = new QLabel(nm, GetRep());

  AddChildWidget(lbl, taiM->hsep_c);

  // add gui representation of data
  taiData* mb_dat = md->im->GetDataRep(host, this, m_rep); //adds to list
  QWidget* ctrl = mb_dat->GetRep();
  lbl->setBuddy(ctrl);
  AddChildWidget(ctrl, taiM->hspc_c);

  // add description text tooltips
  if (!desc.empty()) {
    lbl->setToolTip(desc);
    ctrl->setToolTip(desc);
  }
}

void taiCompData::EndLayout() { //virtual/overridable
  if (last_spc != -1)
    lay->addStretch();
}

void taiCompData::AddChildWidget(QWidget* child_widget, int space_after,
  int stretch) 
{ 
  if (space_after == -1) space_after = taiM->hspc_c;
  mwidgets->append(child_widget);
  AddChildWidget_impl(child_widget, last_spc, stretch);
  last_spc = space_after;
}

void taiCompData::AddChildWidget_impl(QWidget* child_widget, int spacing,
  int stretch) 
{ 
  if (spacing != -1)
    lay->addSpacing(last_spc);
  lay->addWidget(child_widget, stretch, (Qt::AlignLeft | Qt::AlignVCenter));
  child_widget->show();
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


taiField::taiField(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
 : taiData(typ_, host_, par, gui_parent_, flags_)
{
  if (flags_ && flgEditDialog) {
    QWidget* act_par = new QWidget(gui_parent_);
    QHBoxLayout* lay = new QHBoxLayout(act_par);
    lay->setMargin(0);
    lay->setSpacing(1);
    leText = new iLineEdit(act_par);
    lay->addWidget(leText, 1);
    btnEdit = new QToolButton(act_par);
    btnEdit->setText("...");
    btnEdit->setFixedHeight(taiM->text_height(defSize()));
    lay->addWidget(btnEdit);
    SetRep(act_par);
    connect(btnEdit, SIGNAL(clicked(bool)),
      this, SLOT(btnEdit_clicked(bool)) );
  } else {
    SetRep(leText = new iLineEdit(gui_parent_));
    btnEdit = NULL;
  }
  rep()->setFixedHeight(taiM->text_height(defSize()));
  if (readOnly()) {
    rep()->setReadOnly(true);
  } else {
    QObject::connect(rep(), SIGNAL(textChanged(const QString&) ),
          this, SLOT(repChanged() ) );
  }
  // cliphandling connections
  QObject::connect(rep(), SIGNAL(selectionChanged()),
    this, SLOT(selectionChanged() ) );
}

void taiField::btnEdit_clicked(bool) {
  iTextEditDialog* dlg = new iTextEditDialog(); // no parent needed for modals
  if (readOnly())
    dlg->txtText->setReadOnly(true);
  dlg->txtText->setPlainText(rep()->text());
  if (!readOnly() && (dlg->exec() == QDialog::Accepted)) {
    rep()->setText(dlg->txtText->toPlainText());
  }
  dlg->deleteLater();
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

taiIncrField::taiIncrField(TypeDef* typ_, IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_)
: taiData(typ_, host_, par, gui_parent_, flags_)
{
  SetRep( new iSpinBox(gui_parent_) );
  iSpinBox* rep = this->rep();
  rep->setFixedHeight(taiM->text_height(defSize()));

  //note: the taiType will set the max/min
  if (readOnly()) {
    rep->setReadOnly(true);
  } else {
    QObject::connect(m_rep, SIGNAL(valueChanged(int) ),
          this, SLOT(repChanged() ) );
  }
  // cliphandling connections
  QObject::connect(m_rep, SIGNAL(selectionChanged()),
    this, SLOT(selectionChanged() ) );
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

void taiIncrField::setMinimum(const Variant& min) {
  rep()->setMinimum(min.toInt()); 
}

void taiIncrField::setMaximum(const Variant& max) {
  rep()->setMaximum(max.toInt());
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

taiToggle::taiToggle(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) :
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

taiLabel::taiLabel(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_) :
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

taiPlusToggle::taiPlusToggle(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiCompData(typ_, host_, par, gui_parent_, flags_)
{
  SetRep(new QFrame(gui_parent_));
  but_rep = NULL;
  data = NULL;
}

taiPlusToggle::~taiPlusToggle() {
//  rep = NULL;
  data = NULL; //note: will be owned/parented elsewise, so it must delete that way
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

taiComboBox::taiComboBox(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
:taiData(typ_, host_, par, gui_parent_, flags_)
{
  Initialize(gui_parent_);
}

taiComboBox::taiComboBox(bool is_enum, TypeDef* typ_, IDataHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_)
:taiData(typ_, host_, par, gui_parent_, flags_)
{
  Initialize(gui_parent_, is_enum);
  if (is_enum && typ) {
    SetEnumType(typ, true);
  }
}

void taiComboBox::Initialize(QWidget* gui_parent_, bool is_enum_) {
  m_is_enum = is_enum_;
  SetRep(new iComboBox(gui_parent_));
  rep()->setFixedHeight(taiM->combo_height(defSize()));

  //connect changed signal to our slot
  QObject::connect(m_rep, SIGNAL(activated(int) ),
        this, SLOT(repChanged() ) );
  // also to aux signal (used by non-IDataHost clients)
  QObject::connect(m_rep, SIGNAL(activated(int) ),
    this, SIGNAL(itemChanged(int)) );
}

void taiComboBox::AddItem(const String& val, const QVariant& usrData) {
  rep()->addItem(val, usrData);
}

void taiComboBox::Clear() {
  rep()->clear();
}

void taiComboBox::GetImage(int itm) {
    // set to this item number
  rep()->setCurrentIndex(itm);
}

void taiComboBox::GetValue(int& itm) const {
  itm = rep()->currentIndex();
}

void taiComboBox::GetEnumImage(int enum_val) {
  int i = rep()->findData(QVariant(enum_val));
  rep()->setCurrentIndex(i);
}

void taiComboBox::GetEnumValue(int& enum_val) const {
  int i = rep()->currentIndex();
  if (i >= 0) enum_val = rep()->itemData(i).toInt();
  else enum_val = 0; // perhaps the safest invalid choice...
}

void taiComboBox::RemoveItemByData(const QVariant& userData) {
  int i;
  while ((i = rep()->findData(userData)) != -1) {
    rep()->removeItem(i);
  }
}

void taiComboBox::RemoveItemByText(const String& val) {
  int i;
  while ((i = rep()->findText(val)) != -1) {
    rep()->removeItem(i);
  }
}

void taiComboBox::SetEnumType(TypeDef* enum_typ, bool force) {
  if ((typ != enum_typ) || force) {
    Clear();
    typ = enum_typ;
    for (int i = 0; i < typ->enum_vals.size; ++i) {
      EnumDef* ed = typ->enum_vals.FastEl(i);
      AddItem(ed->GetLabel(), QVariant(ed->enum_no));
    }
  }
}


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


taiBitBox::taiBitBox(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
:taiData(typ_, host_, par, gui_parent_, flags_)
{
  Initialize(gui_parent_);
}

taiBitBox::taiBitBox(bool is_enum, TypeDef* typ_, IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_)
:taiData(typ_, host_, par, gui_parent_, flags_)
{
  Initialize(gui_parent_);
  if (is_enum && typ) {
    for (int i = 0; i < typ->enum_vals.size; ++i) {
      if (i > 0)
        lay->addSpacing(taiM->hspc_c);
      EnumDef* ed = typ->enum_vals.FastEl(i);
      if (ed->HasOption("NO_BIT")) continue;
      
      AddBoolItem(ed->GetLabel(), ed->enum_no);
    }
    lay->addStretch();
  }
}


void taiBitBox::Initialize(QWidget* gui_parent_) {
  SetRep(new QWidget(gui_parent_));
  rep()->setFixedHeight(taiM->label_height(defSize()));
  lay = new QHBoxLayout(m_rep);
  lay->setMargin(0); // in Qt4 it adds style-dependent defaults
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
  QObject *obj;
  foreach (obj, m_rep->children() ) {
    if (obj->inherits("iBitCheckBox")) {
      iBitCheckBox* bcb = (iBitCheckBox*)obj;
      bcb->setChecked((val & bcb->val)); //note: prob raises signal -- ok
    }
  }
  m_val = val;
}

void taiBitBox::GetValue(int& val) const {
  val = m_val;
}


//////////////////////////////////
// 	taiDimEdit		//
//////////////////////////////////

taiDimEdit::taiDimEdit(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
:taiData(typ_, host_, par, gui_parent_, flags_)
{
  Initialize(gui_parent_);
}

void taiDimEdit::Initialize(QWidget* gui_parent_) {
  SetRep(new iDimEdit(gui_parent_));
  if (readOnly()) {
    rep()->setReadOnly(true);
  } else {
    connect(m_rep, SIGNAL(changed(iDimEdit*)), this, SLOT(repChanged()) );
  }
}

void taiDimEdit::GetImage(const MatrixGeom* arr) {
  iDimEdit* de = rep(); // cache
  de->setDims(arr->size);
  for (int i = 0; i < arr->size; ++i) {
    de->setDim(i, arr->FastEl(i));
  }
}

void taiDimEdit::GetValue(MatrixGeom* arr) const {
  iDimEdit* de = rep(); // cache
  arr->EnforceSize(de->dims());
  for (int i = 0; i < de->dims(); ++i) {
    arr->Set(i, de->dim(i));
  }
}


//////////////////////////////////
// 	taiPolyData		//
//////////////////////////////////

taiPolyData* taiPolyData::New(bool add_members, TypeDef* typ_, IDataHost* host_,
  taiData* par, QWidget* gui_parent_, int flags)
{
  taiPolyData*  rval = new taiPolyData(typ_, host_, par, gui_parent_, flags);
  rval->Constr(gui_parent_);
  if (add_members) 
    rval->AddTypeMembers();
  return rval;
}

taiPolyData::taiPolyData(TypeDef* typ_, IDataHost* host_, taiData* par, 
  QWidget* gui_parent_, int flags)
: inherited(typ_, host_, par, gui_parent_, flags)
{
  if (host_ && (host_->GetTypeDef()->InheritsFrom(TA_taiEditDataHost))) {
    show = (dynamic_cast<taiEditDataHost*>(host_))->show;
  } else {
    show = taMisc::show_gui;
  }
}

taiPolyData::~taiPolyData() {
  data_el.Reset();
}

bool taiPolyData::ShowMember(MemberDef* md) const {
  if (md->HasOption("HIDDEN_INLINE") ||
    (md->type->HasOption("HIDDEN_INLINE") && !md->HasOption("SHOW_INLINE")) 
    )
    return false;
  else
    return md->ShowMember((taMisc::ShowMembs)show);
}

void taiPolyData::AddChildMember(MemberDef* md) {
  memb_el.Add(md);
  inherited::AddChildMember(md);
}

void taiPolyData::AddTypeMembers() {
  InitLayout();
  for (int i = 0; i < typ->members.size; ++i) {
    MemberDef* md = typ->members.FastEl(i);
    if (!ShowMember(md))
      continue;
    AddChildMember(md);
  }
  EndLayout();
}

void taiPolyData::Constr(QWidget* gui_parent_) {
  SetRep(new QWidget(gui_parent_));
  rep()->setMaximumHeight(taiM->max_control_height(defSize()));
  if (host != NULL) {
    SET_PALETTE_BACKGROUND_COLOR(rep(),*(host->colorOfCurRow()));
  }
}

void taiPolyData::ChildRemove(taiData* child) {
  int i = data_el.Find(child);
  if (i > 0)
    memb_el.Remove(i);
  inherited::ChildRemove(child);
}

void taiPolyData::GetImage_impl(const void* base) {
//NN??  cur_base = base;
  for (int i = 0; i < memb_el.size; ++i) {
    MemberDef* md = memb_el.FastEl(i);
    taiData* mb_dat = data_el.FastEl(i);
    md->im->GetImage(mb_dat, base);
  }
}

void taiPolyData::GetValue_impl(void* base) const {
  ostream* rec_scrpt = taMisc::record_script; // don't record script stuff now
  taMisc::record_script = NULL;
  bool first_diff = true;
  int cnt = 0;
  for (int i = 0; i < memb_el.size; ++i) {
    MemberDef* md = memb_el.FastEl(i);
    taiData* mb_dat = data_el.FastEl(i);
    md->im->GetMbrValue(mb_dat, base, first_diff);
  }
  if (typ->InheritsFrom(TA_taBase) && !HasFlag(flgNoUAE)) {
    TAPtr rbase = (TAPtr)base;
    rbase->UpdateAfterEdit();	// hook to update the contents after an edit..
  }
  taMisc::record_script = rec_scrpt;
}


//////////////////////////////////
// 	taiDataDeck		//
//////////////////////////////////

taiDataDeck::taiDataDeck(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags)
: taiCompData(typ_, host_, par, gui_parent_, flags) {
  cur_deck = 0;
  SetRep(new Q3WidgetStack(gui_parent_));
  rep()->setMaximumHeight(taiM->max_control_height(defSize()));
  if (host != NULL) {
    SET_PALETTE_BACKGROUND_COLOR(rep(),*(host->colorOfCurRow()));
  }
}

void taiDataDeck::GetImage(int i) {
  // note: Q3WidgetStack doesn't seem to completely hide underneath widgets...
  cur_deck = i;
  rep()->raiseWidget(i);
  for (int j = 0; j < widgetCount(); ++j) {
    QWidget* w = widgets(j);
    if (i == j) w->show();
    else        w->hide();
  }
}

void taiDataDeck::AddChildWidget_impl(QWidget* child_widget, int spacing,
  int /*stretch*/)
{//note: stretch not needed/used
  (rep()->addWidget(child_widget, cur_deck));
}


//////////////////////////////////
// 	taiVariantBase		//
//////////////////////////////////

taiVariantBase::taiVariantBase(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags)
: taiCompData(typ_, host_, par, gui_parent_, flags)
{
  //note: call Constr in your own class' constructor
}

taiVariantBase::~taiVariantBase() {
  data_el.Reset();
}

void taiVariantBase::Constr(QWidget* gui_parent_) { 
  cmbVarType = NULL;
  fldVal = NULL;
  togVal = NULL;
  m_updating = 0;
  
  QWidget* rep_ = new QWidget(gui_parent_);
  SetRep(rep_);
  rep_->setMaximumHeight(taiM->max_control_height(defSize()));
  if (host != NULL) {
    SET_PALETTE_BACKGROUND_COLOR(rep_,*(host->colorOfCurRow()));
  }
  InitLayout();
  Constr_impl(gui_parent_, (mflags & flgReadOnly));
  EndLayout();
}

void taiVariantBase::Constr_impl(QWidget* gui_parent_, bool read_only_) { 
  // type stuff
  QWidget* rep_ =  GetRep();
  QLabel* lbl = new QLabel("var type",rep_);
  AddChildWidget(lbl, taiM->hsep_c);
  
  TypeDef* typ_var_enum = TA_Variant.sub_types.FindName("VarType");
  cmbVarType = new taiComboBox(true, typ_var_enum, host, this, rep_);
  //remove unused variant enum types according to flags
  if (mflags & (flgNoInvalid | flgIntOnly)) {
    cmbVarType->RemoveItemByData(QVariant(Variant::T_Invalid));
  }
  if (mflags & (flgNoAtomics | flgIntOnly)) {
    for (int vt = Variant::T_Atomic_Min; vt <= Variant::T_Atomic_Max; ++vt) { 
      if (!((vt == Variant::T_Int) && (mflags & flgIntOnly)))
        cmbVarType->RemoveItemByData(QVariant(vt));
    }
  }
  if (mflags & (flgNoPtr | flgIntOnly)) {
    cmbVarType->RemoveItemByData(QVariant(Variant::T_Ptr));
  }
  if (mflags & (flgNoBase | flgIntOnly)) {
    cmbVarType->RemoveItemByData(QVariant(Variant::T_Matrix));
    cmbVarType->RemoveItemByData(QVariant(Variant::T_Base));
  }
  AddChildWidget(cmbVarType->rep(), taiM->hsep_c);
  lbl->setBuddy(cmbVarType->rep());
  if (read_only_) {
    cmbVarType->rep()->setEnabled(false);
  } else {
    connect(cmbVarType, SIGNAL(itemChanged(int)), this, SLOT(cmbVarType_itemChanged(int)));
  }
  
  lbl = new QLabel("var value", rep_);
  AddChildWidget(lbl, taiM->hsep_c);
  stack = new QStackedWidget(rep_);
  AddChildWidget(stack); // fill rest of space
  lbl->setBuddy(stack);
  
  // created in order of StackControls
  lbl = new QLabel("(no value for type Invalid)");
  stack->addWidget(lbl);
  togVal = new taiToggle(typ, host, this, NULL);
  stack->addWidget(togVal->rep());
  incVal = new taiIncrField(typ, host, this, NULL, mflags & flgEditDialog);
  incVal->setMinimum(INT_MIN); //note: must be int
  incVal->setMaximum(INT_MAX); //note: must be int
  stack->addWidget(incVal->rep());
  fldVal = new taiField(typ, host, this, NULL, mflags & flgEditDialog);
  stack->addWidget(fldVal->rep());
  lbl = new QLabel("(Ptr cannot be set)");
  stack->addWidget(lbl);
  
  tabVal = new taiTokenPtrButton(&TA_taBase, host, this, NULL);
  stack->addWidget(tabVal->GetRep());
}
    

/*
bool taiVariantBase::ShowMember(MemberDef* md) {
  if (md->HasOption("HIDDEN_INLINE"))
    return false;
  else
    return md->ShowMember((taMisc::ShowMembs)show);
} */

void taiVariantBase::cmbVarType_itemChanged(int itm) {
  if (m_updating != 0) return;
  ++m_updating;
  int vt; //Variant::VarType
  // set combo box to right type
  cmbVarType->GetEnumValue(vt);
  switch (vt) {
  case Variant::T_Invalid: 
    stack->setCurrentIndex(scInvalid);
    break;
  
  case Variant::T_Bool:
    stack->setCurrentIndex(scBool);
    break;
  
  case Variant::T_Int:
    stack->setCurrentIndex(scInt);
    break;
  
  case Variant::T_UInt:
  case Variant::T_Int64:
  case Variant::T_UInt64:
  case Variant::T_Double:
  case Variant::T_Char:
  case Variant::T_String: 
    stack->setCurrentIndex(scField);
    break;
  
  case Variant::T_Ptr: 
    stack->setCurrentIndex(scPtr);
    break;
  case Variant::T_Base: 
    stack->setCurrentIndex(scBase);
    tabVal->GetImage(NULL, &TA_taBase); // obj, no scope
    break;
  case Variant::T_Matrix:
    stack->setCurrentIndex(scBase);
    tabVal->GetImage(NULL, &TA_taMatrix); // obj, no scope
    break;
  default: return ;
  }
  --m_updating;
}

void taiVariantBase::GetImage_Variant(const Variant& var) {
  ++m_updating;
  // set combo box to right type
  cmbVarType->GetEnumImage(var.type());
  
  switch (var.type()) {
  case Variant::T_Invalid: 
    stack->setCurrentIndex(scInvalid);
    break;
  
  case Variant::T_Bool:
    stack->setCurrentIndex(scBool);
    togVal->GetImage(var.toBool());
    break;
  
  case Variant::T_Int:
    stack->setCurrentIndex(scInt);
    incVal->GetImage(var.toInt());
    break;
  
  case Variant::T_UInt:
  case Variant::T_Int64:
  case Variant::T_UInt64:
  case Variant::T_Double:
  case Variant::T_Char:
  case Variant::T_String: 
    stack->setCurrentIndex(scField);
    fldVal->GetImage(var.toString());
    break;
  
  case Variant::T_Ptr: 
    stack->setCurrentIndex(scPtr);
    //TODO: need to set something! 
    break;
  case Variant::T_Base: 
    stack->setCurrentIndex(scBase);
    tabVal->GetImage(var.toBase(), &TA_taBase); // obj, no scope
    break;
  case Variant::T_Matrix:
    stack->setCurrentIndex(scBase);
    tabVal->GetImage(var.toMatrix(), &TA_taMatrix); // obj, no scope
    break;
  default: return ;
  }
  --m_updating;
}

void taiVariantBase::GetValue_Variant(Variant& var) const {
  ++m_updating;
  int vt; //Variant::VarType
  // set combo box to right type
  cmbVarType->GetEnumValue(vt);
  var.setType((Variant::VarType)vt);
  
  //note: the correct widget should be visible...
  
  switch (var.type()) {
  case Variant::T_Invalid: 
    var = _nilVariant;
    break;
  
  case Variant::T_Bool:
    var.setBool(togVal->GetValue());
    break;
  
  case Variant::T_Int:
    var = incVal->GetValue();
    break;
  
  case Variant::T_UInt:
  case Variant::T_Int64:
  case Variant::T_UInt64:
  case Variant::T_Double:
  case Variant::T_Char:
  case Variant::T_String: 
    var.updateFromString(fldVal->GetValue());
    break;
  
  case Variant::T_Ptr: 
    //TODO: need to set something! maybe should be NULL
    break;
  case Variant::T_Base: 
    var.setBase(tabVal->GetValue());
    break;
  case Variant::T_Matrix:
    var.setBase(tabVal->GetValue());
    break;
  default: break;
  }
  --m_updating;
  return;
}


//////////////////////////////////
// 	taiVariant		//
//////////////////////////////////

taiVariant::taiVariant(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags)
:inherited(&TA_Variant, host_, par, gui_parent_, flags)
{
  Constr(gui_parent_);
}

taiVariant::~taiVariant() {
}




//////////////////////////
// 	taiAction	//
//////////////////////////

taiAction::taiAction(int sel_type_, const String& label_)
: QAction(label_, NULL)
{
  init(sel_type_);
}

taiAction::taiAction(const QString& label_, const QKeySequence& accel, const char* name_)
: QAction(label_, NULL)
{
  init(taiMenu::use_default);
  setShortcut(accel);
  setName(name_);
}

taiAction::taiAction(const QString& label_, QObject* receiver, const char* member, const QKeySequence& accel)
: QAction(label_, NULL)
{
  init(taiMenu::use_default);
  setShortcut(accel);
  connect(action, receiver, member);
}


taiAction::taiAction(const Variant& usr_data_, const QString& label_, const QKeySequence& accel,
  const char* name_) 
: QAction(label_, NULL)
{
  init(taiMenu::use_default);
  usr_data = usr_data_;
  setShortcut(accel);
  setName(name_);
}

taiAction::~taiAction() {
}

void taiAction::init(int sel_type_)
{
  sel_type = sel_type_;
  //note: we do this here, but also at AddAction time in case we create default here, 
  // and it only gets its true sel_type when added
  if (sel_type & (taiActions::radio | taiActions::toggle)) {
    setCheckable(true);
  }
  nref = 0;
  m_changing = 0;
  QObject::connect(this, SIGNAL(triggered(bool)), this, SLOT(this_triggered_toggled(bool)) );
//note: we don't want the toggled signal, because this causes us to signal twice in most cases
// the only thing 'triggered' doesn't signal is programmatic changes, which is ok
//  QObject::connect(this, SIGNAL(toggled(bool)), this, SLOT(this_triggered_toggled(bool)) );
}

void taiAction::AddTo(taiActions* targ) {
  targ->AddAction(this);
}

bool taiAction::canSelect() {
  // an item can be the curSel if it is a global radio item
  return ((sel_type & taiMenu::radio) && (!isGrouped()) && !isSubMenu());
}

bool taiAction::isGrouped() {
  return (actionGroup() != NULL);
}

/*nn bool taiAction::isChecked() {
   // returns 'true' if a radio or toggle item, and checked, false otherwise
  if (isSubMenu())
    return false;
  else return rep->isChecked();
} 

void taiAction::setChecked(bool value) {
   // ignored if not a radio or toggle item
  if (isSubMenu() || !(sel_type & (taiMenu::radio | taiMenu::toggle)))
    return;
  else owner->menu()->setItemChecked(id(), value);
} */

void taiAction::connect(CallbackType ct_, const QObject *receiver, const char* member) {
  // connect callback to given
  if ((ct_ == none) || (receiver == NULL) || (member == NULL)) return;

  switch (ct_) {
  case none:
    return;
  case action:
    QObject::connect(this, SIGNAL(Action()), receiver, member);
    break;
  case men_act:
    QObject::connect(this, SIGNAL(MenuAction(taiAction*)), receiver, member);
    break;
  case int_act:
    QObject::connect(this, SIGNAL(IntParamAction(int)), receiver, member);
    break;
  case ptr_act:
    QObject::connect(this, SIGNAL(PtrParamAction(void*)), receiver, member);
    break;
  case var_act:
    QObject::connect(this, SIGNAL(VarParamAction(const Variant&)), receiver, member);
    break;
  }
}

void taiAction::connect(const taiMenuAction* mact) {
  if (mact == NULL) return;
  connect(men_act, mact->receiver, mact->member);
}

void taiAction::emitActions() {
  emit Action();
  emit MenuAction(this);
  emit IntParamAction(usr_data.toInt());
  emit PtrParamAction(usr_data.toPtr());
  emit VarParamAction(usr_data);
}

void taiAction::this_triggered_toggled(bool checked) {
  if (m_changing > 0) return;
  ++m_changing;
  emitActions(); // will also cause curSel update, and datachanged
  --m_changing;	
}

/*obs 
void taiAction::Select() {
  if (sel_type & taiMenu::toggle)
    Select_impl(!isChecked());
  else Select_impl(true);

  emitActions();	// don't set the cur_sel if executing
  
  // if a radio item in global group, update global selection
  if ((sel_type & taiMenu::radio) && (radio_grp == -1))
    owner->setCurSel(this);

  if (sel_type & taiMenu::update) {
//    owner->Update();
    owner->DataChanged();		// something was selected..
  } 
}*/

/*obs
void taiAction::Select_impl(bool selecting) {
// TODO: verify
  if (sel_type & taiMenu::toggle) {
    setChecked(selecting);
  } else if (sel_type & taiMenu::radio) {
  
    if (!selecting) {
       setChecked(false);
    } else if (radio_grp != -1) {
 
      // if non-global radio group item, then set the item in the group
      for (int i = 0; i < owner->items.size; ++i) {
        taiAction* mel = owner->items.FastEl(i);
        if (mel->radio_grp != this->radio_grp) continue;
        setChecked((mel == this));
      } 
    } else {
      setChecked(true);
    } 
  } */
  

/*Qt3   // called by Select() and by taiMenu::GetImage -- doesn't trigger events
  if (sel_type & taiMenu::toggle) {
    owner->menu()->setItemChecked(id(), selecting);
  } else if (sel_type & taiMenu::radio) {
    if (!selecting) {
       setChecked(false);
    } else if (radio_grp != -1) {

      // if non-global radio group item, then set the item in the group
      for (int i = 0; i < owner->items.size; ++i) {
        taiAction* mel = owner->items.FastEl(i);
        if (mel->radio_grp != this->radio_grp) continue;
        setChecked((mel == this));
      }
    } else {
      setChecked(true);
   }
  } 
}*/


//////////////////////////
// 	taiSubMenuEl	//
//////////////////////////

taiSubMenuEl::taiSubMenuEl(const String& label_, taiMenu* sub_menu_data_)
:taiAction(taiMenu::st_none, label_)
{
  sub_menu_data = sub_menu_data_;
  setMenu(sub_menu_data->menu());
}

taiSubMenuEl::~taiSubMenuEl() {
  if (sub_menu_data != NULL) {
    delete sub_menu_data;
    sub_menu_data = NULL;
  }
}



//////////////////////////
//  taiAction_List	//
//////////////////////////

void taiAction_List::El_Done_(void* it_)	{ 
  taiAction* it = (taiAction*)it_;
  if (it->nref == 0)
    delete it; //NB: don't deleteLater, because taiData->parent will be invalid by then
}

taiAction* taiAction_List::PeekNonSep() {
  taiAction* rval;
  for (int i = size - 1; i >= 0; --i) {
    rval = FastEl(i);
    if (!rval->isSeparator())
      return rval;
  }
  return NULL;
}


//////////////////////////
//  taiActions	//
//////////////////////////

taiActions* taiActions::New(RepType rt, int sel_type_, int font_spec_, TypeDef* typ_, IDataHost* host,
      taiData* par, QWidget* gui_parent_, int flags_, taiActions* par_menu_)
{
  taiActions* rval = NULL; //note: switch handles all actual cases, NULL=compiler food
  switch (rt) {
  case popupmenu: 
    rval = new taiMenu(sel_type_, font_spec_, typ_, host, par, gui_parent_, flags_, par_menu_);
  case buttonmenu: 
    rval = new taiButtonMenu(sel_type_, font_spec_, typ_, host, par, gui_parent_, flags_, par_menu_);
  }
  return rval;
}

taiActions::taiActions(int sel_type_, int ft, TypeDef* typ_, 
  IDataHost* host_, taiData* par_, QWidget* gui_parent_, int flags_, taiActions* par_menu_,
      bool has_menu, QMenu* exist_menu)
: taiData(typ_, host_, par_, gui_parent_, flags_)
{
  sel_type = (SelType)sel_type_;
  font_spec = ft;
  gui_parent = gui_parent_;
  cur_grp = NULL;
  cur_sel = NULL;
  par_menu = par_menu_;
  par_menu_el = NULL;
  if (has_menu) {
    m_menu = (exist_menu) ? exist_menu : new QMenu(gui_parent);
    m_menu->setFont(taiM->menuFont(font_spec));
    connect(m_menu, SIGNAL(destroyed(QObject*)), 
      this, SLOT(repDestroyed(QObject*)) );
  } else {
    m_menu = NULL;
  } 
}

taiActions::~taiActions() {
  Reset(); 
  if (m_menu) {
    m_menu->setParent(NULL); // avoid potential issues 
    m_menu->deleteLater();
    m_menu = NULL;
  }
}

void taiActions::ActionAdded(taiAction* it) {
  actionsRep()->addAction(it);
}

void taiActions::ActionRemoving(taiAction* it) {
  actionsRep()->removeAction(it);
}

QWidget* taiActions::actionsRep() {
  if (m_menu != NULL) return m_menu;
  else                return GetRep();
}

void taiActions::AddAction(taiAction* act) {
  // if it is a radio item
  if (act->sel_type & (radio | toggle)) {
    act->setCheckable(true);
  }
  // if we have an explicit radio group going, add it
  // this is not done much, only when we explicitly create groups, like for the view menu
  if ((act->sel_type & radio) && (cur_grp != NULL)) {
    cur_grp->addAction(act);
  }
  // font compliance
  act->setFont(taiM->menuFont(font_spec));
  items.Add(act);
  ActionAdded(act);
  connect(act, SIGNAL(MenuAction(taiAction*)), this, SLOT(child_triggered_toggled(taiAction*)) );
}

taiAction* taiActions::AddItem(const String& val, SelType st, 
  taiAction::CallbackType ct_, const QObject *receiver, const char* member,
  const Variant& usr)
{ // 'member' is the result of the SLOT() macro
  if (st == use_default)
    st = sel_type;

  taiAction* rval;
  // do not add items of same name -- return it instead of adding it
  for (int i = 0; i < items.size; ++i) {
    rval = items.FastEl(i);
    if (rval->text() == val) {
      return rval;
    }
  }
  rval = new taiAction(st, val);
  rval->usr_data = usr;
  AddAction(rval);
  rval->connect(ct_, receiver, member);
  
/*TODO: modify for Qt4  
  // connect any parent submenu handler
  if (par_menu_el) {
    if (par_menu_el->default_child_action.receiver) {
      menu()->connectItem(rval->id(), par_menu_el->default_child_action.receiver, par_menu_el->default_child_action.member);
    }
  } */
  return rval;
}

taiAction* taiActions::AddItem(const String& val, SelType st, const taiMenuAction* mact, 
  const Variant& usr)
{
  if (mact != NULL)
    return AddItem(val, st, taiAction::men_act, mact->receiver, mact->member, usr); 
  else
    return AddItem(val, st, taiAction::none, NULL, NULL, usr);
}

taiAction* taiActions::AddItem(const String& val, const Variant& usr) {
    return AddItem(val, sel_type, taiAction::none, NULL, NULL, usr);
}

void taiActions::AddSep(bool new_radio_grp) {
  if (new_radio_grp) NewRadioGroup();
  //don't double add seps or add at beginning (this check simplifies many callers, so they don't need to know
  //  whether a previous operation added items and/or seps, or not)
  QWidget* ar = actionsRep();
  QAction* it = NULL;
  if (ar->actions().count() > 0) 
    it = ar->actions().last();
//  taiAction* it = items.Peek();
  if ((it == NULL) || it->isSeparator()) return;

  it = new QAction(ar); //TODO: make sure making the rep as parent for hidden seps won't ever accumulate
  it->setSeparator(true);
  actionsRep()->addAction(it);
}

taiMenu* taiActions::AddSubMenu(const String& val, TypeDef* typ_)
{
  /*NOTE: Qt4 -- the below was way too obscure -- we should just set the st to this one
  SelType st;
  // we use the value of the most recent submenu, otherwise ourself
  taiAction* it = items.PeekNonSep();
  if (it != NULL)
    st = (SelType)it->sel_type;
  else
    st = this->sel_type; */

  // do not add items of same label -- return it instead of adding it
  for (int i = 0; i < items.size; ++i) {
    taiAction* act = items.FastEl(i);
    if (act->isSubMenu() && (act->text() == val)) {
      return ((taiSubMenuEl*)act)->sub_menu_data;
    }
  }
  
  taiMenu* rval = new taiMenu(sel_type, font_spec, typ_, host, this, gui_parent, mflags, this);
  taiSubMenuEl* sme = new taiSubMenuEl(val, rval);
  rval->par_menu_el = sme;
  AddAction(sme);
  return rval;
}

void taiActions::child_triggered_toggled(taiAction* act) {
  // if a radio item in global group, these are the cases:
  // * user manually unchecked it -- not an allowed operation, so  just recheck it
  // * user selected a new item -- just request the change
  if (act->canSelect()) {
    if (!act->isChecked()) {
      taiAction* cur = curSel();
      if (act == cur) {
        act->setChecked(true);
        return; // skip data change, because nothing actually changed
      }
    } else { 
      // just request the change on select
      setCurSel(act); // gets ignored while already setting, ex. when unsetting the other item
    }
  }

  if (act->sel_type & taiActions::update) {
    DataChanged();		// something was selected..
  } 
}

taiAction* taiActions::curSel() const {
  if (par_menu != NULL)
    return par_menu->curSel();
  else  return cur_sel;
}

void taiActions::DeleteItem(uint idx) {
  if (idx >= (uint)items.size) return;
  taiAction* act = items[idx];
  ActionRemoving(act);
  items.Remove(idx); // deletes if ref==0
}

void taiActions::emitLabelChanged(const String& val) {
  emit labelChanged(val.chars());
}

taiMenu* taiActions::FindSubMenu(const char* nm) {
  for (int i = 0; i < items.size; ++i) {
    taiAction* itm = items.FastEl(i);
    if (!itm->isSubMenu()) continue;
    taiSubMenuEl* sme = (taiSubMenuEl*)itm;
    if (sme->text() == nm)
      return sme->sub_menu_data;
  }
  return NULL;
}

 bool taiActions::GetImageByData(const Variant& usr) {
  // first try to find item by iterating through all eligible items and subitems
  if (GetImage_impl(usr))
      return true; 
/*TODO Qt4 NOTE: this is fairly horribly obscure!!!
  // otherwise get first eligible item, if any, on this menu only, with data and without any menu callbacks, as default if nothing else works
  for (int i = 0; i < items.size; ++i) {
    taiAction* itm = items.FastEl(i);
    if (!itm->canSelect()) continue;
    if ( (!itm->usr_data.isInvalid()) && (!itm->hasCallbacks())) {
      setCurSel(itm);
      return true;
    }
  } */
  return false;
}

bool taiActions::GetImage_impl(const Variant& usr) {
   // set to this usr item, returns false if not found
  // first, look at our items...
  for (int i = 0; i < items.size; ++i) {
    taiAction* itm = items.FastEl(i);
    if (!itm->canSelect()) continue;
    if (itm->usr_data == usr) {
//TODO Qt4: make sure this case is automatically handled now
//      if (usr.isPtrType() && (usr.toPtr() == NULL) && (itm->text() != String::con_NULL))
//	continue;
      setCurSel(itm);
      return true;
    }
  }
  // ...otherwise, recursively descend to submenus
  for (int i = 0; i < items.size; ++i) {
    taiAction* itm = items.FastEl(i);
    if (!itm->isSubMenu()) continue;
    taiSubMenuEl* sme = (taiSubMenuEl*)itm;
    taiMenu* sub_menu = sme->sub_menu_data;
    if (sub_menu->GetImage_impl(usr))
      return true;
  }
  return false;
}

void taiActions::GetImageByIndex(int itm) {
  if ((itm < 0) || (itm >= items.size)) return;
  taiAction* mel = items.FastEl(itm);
  setCurSel(mel);
//  Update();
}

void taiActions::NewRadioGroup() {
  cur_grp = new QActionGroup(gui_parent);
}

void taiActions::Reset() {
  for (int i = count() - 1; i >= 0; --i) {
    DeleteItem(i);
  }
  // also remove the phantom seps
  cur_sel = NULL;
  QWidget* ar = actionsRep();
  if (ar != NULL)
    ar->actions().clear();
}

void taiActions::setCurSel(taiAction* value) {
  //curSel can only be a global radio type, or null
  if ( (value != NULL) && !value->canSelect() ) return;
  if (par_menu != NULL) {
    par_menu->setCurSel(value);
  } else {
    // controlling root needs to unselect existing element
    if (cur_sel == value) return;
    if (cur_sel != NULL) {
      // need to get it out of curSel so unchecking is allowed by taiAction item's handler
      taiAction* tmp = cur_sel;
      cur_sel = NULL;
      tmp->setChecked(false);
    }
    cur_sel = value; // need to set it to new, so checking action causes an ignore
    if (cur_sel != NULL) {
      cur_sel->setChecked(true);
      setLabel(cur_sel->text());
    } else { //NOTE: special case of going from legal radio item to no item -- set label to NULL
      setLabel(String::con_NULL);
    }
  }
}

String taiActions::label() const {
  if (par_menu != NULL)
    return par_menu->label();
  else {
    return mlabel;
  }
}

void taiActions::repDestroyed(QObject* obj) {
  if (obj == m_menu)
    m_menu = NULL;
  else inherited::repDestroyed(obj);
}

void taiActions::setLabel(const String& val) {
  if (par_menu != NULL)
    par_menu->setLabel(val);
  else {
    if (mlabel == val) return;
    mlabel = val;
    // we support a limited number of reps...
    QMenu* menu_ = qobject_cast<QMenu*>(GetRep());
    if (menu_ != NULL) {
      menu_->menuAction()->setText(val);
    } else {
      QPushButton* pb_ = qobject_cast<QPushButton*>(GetRep());
      if (pb_ != NULL) {
        pb_->setText(val);
      }
    }
    emitLabelChanged(mlabel);
  }
}

void taiActions::SetRep(QWidget* val) {
  if (val) 
    connect(val, SIGNAL(destroyed(QObject*)),
      this, SLOT(repDestroyed(QObject*)) );
  else if (m_rep)
    disconnect(m_rep, SIGNAL(destroyed(QObject*)),
      this, SLOT(repDestroyed(QObject*)) );
  inherited::SetRep(val);
}

//////////////////////////
// 	taiMenu	//
//////////////////////////

taiMenu::taiMenu(int st, int ft, TypeDef* typ_, IDataHost* host_, taiData* par,
	QWidget* gui_parent_, int flags_, taiActions* par_menu_)
: taiActions(st, ft, typ_, host_, par, gui_parent_, flags_, par_menu_, true, NULL)
{
  init();
}

/*nbg taiMenu::taiMenu(int rt, int st, int ft, QWidget* gui_parent_)
: taiData(NULL, NULL, NULL, gui_parent_, 0)
{
  init(rt, st, ft, gui_parent_, NULL);
} */

taiMenu::taiMenu(QWidget* gui_parent_, int st, int ft, QMenu* exist_menu)
: taiActions(st, ft, NULL, NULL, NULL, gui_parent_, 0, NULL, true, exist_menu)
{
  init();
}


void taiMenu::init()
{
  SetRep(menu());
}

taiMenu::~taiMenu() {
}

/*obs taiAction* taiMenu::AddItem_FromAction(iAction* act)
{
  Q3PopupMenu* pm = rep_popup();
  if (!pm) {
    taMisc::Error("taiMenu::AddItem_FromAction: can only be called for Q3PopupMenu taiMenu objects");
    return NULL;
  }
  taiMenu::SelType st = act->isToggleAction() ? taiMenu::toggle : taiMenu::normal;

  taiAction* rval;
  int rgrp = -1; // radiogroup n/a
  // use the built in api to add the menu, then retrieve it
  act->addTo(pm);
  int new_id = pm->idAt(pm->count() - 1);
  QMenuItem* new_men = pm->findItem(new_id);
  pm->setItemParameter(new_id, act->param);

  rval = new taiAction(this, new_men, rgrp, st, act->menuText(), NULL,
      taiAction::int_act, act, SIGNAL(activated(int)));
  rval->label = act->menuText();
  items.Add(rval);
//TODO: Qt4  // connect any parent submenu handler
  if (par_menu_el) {
    if (par_menu_el->default_child_action.receiver) {
      menu()->connectItem(rval->id(), par_menu_el->default_child_action.receiver, par_menu_el->default_child_action.member);
    }
  }
  return rval;
} */

void taiMenu::exec(const iPoint& pos) {
  menu()->exec((QPoint)pos);
}

taiAction* taiMenu::insertItem(const char* val, const QObject *receiver, const char* member, const QKeySequence* accel) {
  taiAction* mel = AddItem(val, use_default, taiAction::none, receiver, member);
  if (accel != NULL) mel->setShortcut(*accel);
  return mel;
}

/*QMenu* taiMenu::NewSubItem(const char* val, QMenu* child, const QKeySequence* accel) {
  QMenu* new_men;
  int itemId;
  itemId = menu()->insertItem(val, child);
  new_men = menu()->findItem(itemId);
// TODO: Font control
  if((font_spec == big) || (font_spec == big_italic)) {
    lbl = new ivLabel(val, taiM->big_menu_font, taiM->font_foreground);
  }
  else {
    lbl = new ivLabel(val, taiM->small_menu_font, taiM->font_foreground);
  }

  return new_men;
} */

/* QAction* taiMenu::NewMenuItem(const char* val, SelType st, QMenu* child, const QKeySequence* accel) {
  QAction* new_men;
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
// TODO: Font control
  if((font_spec == big) || (font_spec == big_italic)) {
    lbl = new ivLabel(val, taiM->big_menu_font, taiM->font_foreground);
  }
  else {
    lbl = new ivLabel(val, taiM->small_menu_font, taiM->font_foreground);
  }

  return new_men;
} */

/*
void taiMenu::Update() {
  if (par_menu != NULL)
    par_menu->Update();
  else {
    if ((cur_sel != NULL) && ((cur_sel->sel_type & radio) && (cur_sel->radio_grp != -1)))
      setLabel(cur_sel->label);
//    else if (cur_sel == NULL)
//      setLabel(String::con_NULL);
  }
}*/
/* OBS
void taiMenu::Update() {
  if ((cur_sel != NULL) && ((sel_type == radio_update) || (sel_type == normal_update)))
    SetMLabel(cur_sel->label);
  else if(cur_sel == NULL)
    SetMLabel(String::con_NULL);
  return;
}
*/

//////////////////////////
//  taiButtonMenu 	//
//////////////////////////

taiButtonMenu::taiButtonMenu(int st, int ft, TypeDef* typ_, IDataHost* host_, taiData* par,
	QWidget* gui_parent_, int flags_, taiActions* par_menu_)
: taiActions(st, ft, typ_, host_, par, gui_parent_, flags_, par_menu_, true, NULL)
{
  init();
}

void taiButtonMenu::init()
{
  QPushButton* button = new QPushButton(gui_parent);
  //note: for taiEditButton, we don't add the menu to ourself if it is in EditOnly mode
  //  because that seems to interfere with normal pushbutton ability
  if (!HasFlag(flgEditOnly)) {
    button->setMenu(menu());
  }
  button->setFont(taiM->menuFont(font_spec)); //note: we use menu font -- TODO: might need to use a button font
  button->setFixedHeight(taiM->button_height(defSize()));
  SetRep(button);
}

void taiButtonMenu::Delete() {
  if (!HasFlag(flgEditOnly)) {
    if (m_menu) {
      delete m_menu;
      m_menu = NULL;
    }
  }

  inherited::Delete();
  //WE ARE DELETE HERE
}


//////////////////////////
//  taiMenuBar	 	//
//////////////////////////

taiMenuBar::taiMenuBar(int ft, TypeDef* typ_, IDataHost* host_,
    taiData* par_, QWidget* gui_parent_, int flags_)
: taiActions(normal, ft, typ_, host_, par_, gui_parent_, flags_)
{
  init(NULL);
}

taiMenuBar::taiMenuBar(QWidget* gui_parent_, int ft, QMenuBar* exist_menu)
: taiActions(normal, ft, NULL, NULL, NULL, gui_parent_, 0)
{
  init(exist_menu);
}

taiMenuBar::~taiMenuBar() {
}

void taiMenuBar::init(QMenuBar* exist_menu)
{
  //TODO: would be safer if we used Qt's type system to absolutely confirm that correct type was passed...
  QMenuBar* mrep_bar = (exist_menu) ? exist_menu : new QMenuBar(gui_parent);
  mrep_bar->setFont(taiM->menuFont(font_spec));
  //NOTE: do *not* try to change the height of menubars -- if too small, some platforms
  // turn the menubars into pop-aside menubuttons
  SetRep(mrep_bar);
//  cur_sel = NULL;
}

/* QMenu* taiMenuBar::NewSubItem(const char* val, QMenu* child, const QKeySequence* accel) {
  QMenu* new_men;
  int itemId;
  itemId = menu()->insertItem(val, child);
  new_men = menu()->findItem(itemId);
// TODO: Font control
  if((font_spec == big) || (font_spec == big_italic)) {
    lbl = new ivLabel(val, taiM->big_menu_font, taiM->font_foreground);
  }
  else {
    lbl = new ivLabel(val, taiM->small_menu_font, taiM->font_foreground);
  }

  return new_men;
}*/


//////////////////////////////////
// 	taiToolBar	 	//
//////////////////////////////////

taiToolBar::taiToolBar(QWidget* gui_parent_, int ft, QToolBar* exist_bar) 
: taiActions(normal, ft, NULL, NULL, NULL, gui_parent_, 0)
{
  init(exist_bar);
}
  
void taiToolBar::init(QToolBar* exist_bar) {
  if (exist_bar == NULL) {
    exist_bar = new QToolBar(gui_parent);
  }
  SetRep(exist_bar);
}


//////////////////////////////////
// 	taiEditButton	 	//
//////////////////////////////////


taiEditButton* taiEditButton::New(void* base, taiEdit *taie, TypeDef* typ_,
  IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
{
  if (typ_ && !typ_->InheritsFrom(TA_taBase) || typ_->HasOption("EDIT_ONLY"))
    flags_ |= flgEditOnly;
  taiEditButton* rval = new taiEditButton(base, taie, typ_,
    host_, par, gui_parent_, flags_);
  return rval;
}

taiEditButton::taiEditButton(void* base, taiEdit *taie, TypeDef* typ_,
	IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiButtonMenu(taiMenu::normal_update, taiMisc::fonSmall, typ_, host_, par, gui_parent_, flags_)
{
  cur_base = base;
  ie = taie;	// note: if null, it uses type's ie
  // if true edit-only button, we just wire the action right to the button, and don't make any menu items
  // otherwise,  we create a menu
  if (HasFlag(flgEditOnly)) {
    connect(m_rep, SIGNAL(clicked()),
        this, SLOT(Edit()) );
  }
  SetLabel();
}

taiEditButton::~taiEditButton(){
  if (ie != NULL) {
    delete ie;
    ie = NULL;
  }
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
      AddSep();
    lst_men_nm = men_nm;
    taiMethodData* mth_rep = md->im->GetMethodRep(cur_base, host, this, gui_parent);
    if (mth_rep == NULL)
      continue;
    meth_el.Add(mth_rep);
    if ((ie != NULL) && (md->name == "Edit"))
      AddItem("Edit", taiMenu::use_default,
          taiAction::action, this, SLOT(Edit()));
    else
      mth_rep->AddToMenu(this);
  }
}

void taiEditButton::GetImage_impl(const void* base) {
  cur_base = (void*)base; //ok to deconstify
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
//nn  if ((bgclr == NULL) && (host != NULL)) bgclr = host->bg_color;
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
  GetImage_impl(cur_base);
}

void taiEditButton::setRepLabel(const char* label) {
    rep()->setText(label);
}


//////////////////////////////////////////
// 		taiObjChooser		//
//////////////////////////////////////////

class ocListBoxItem: public Q3ListBoxText {
public:
  const void* data;
  ocListBoxItem(const QString& text_, const void* data_);
};

ocListBoxItem::ocListBoxItem(const QString& text_, const void* data_)
:Q3ListBoxText(text_)
{
  data = data_;
}

taiObjChooser* taiObjChooser::createInstance(TAPtr parob, const char* captn, bool selonly, QWidget* par_window_) {
  if (par_window_ == NULL)
    par_window_ = taiMisc::main_window;
  return new taiObjChooser(parob, captn, selonly, par_window_);
}

taiObjChooser* taiObjChooser::createInstance(TypeDef* tpdf, const char* captn, TAPtr scope_ref_, QWidget* par_window_) {
  if (par_window_ == NULL)
    par_window_ = taiMisc::main_window;
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
: QDialog(par_window_)
{
  setModal(true);
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
//Qt3  layOuter = new QGridLayout(this, 3, 1, taiM->vsep_c, taiM->vspc_c); // rows, cols, margin, space
  layOuter = new QGridLayout(this);
  layOuter->setMargin(taiM->vsep_c);
  layOuter->setSpacing(taiM->vspc_c); 
  browser = new Q3ListBox(this);
  layOuter->addWidget(browser, 1, 0);
  layOuter->setRowStretch(1, 1); // list is item to expand in host
  layOuter->setRowMinimumHeight(1, 100); // don't shrink to nothing

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
  connect(browser, SIGNAL(doubleClicked(Q3ListBoxItem*)),
      this, SLOT(browser_doubleClicked(Q3ListBoxItem*)));
  connect(browser, SIGNAL(selectionChanged(Q3ListBoxItem*)),
      this, SLOT(browser_selectionChanged(Q3ListBoxItem*)));
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

void taiObjChooser::browser_doubleClicked(Q3ListBoxItem* itm) {
  if (select_only)
    accept();
  else
    DescendBrowser();
}

void taiObjChooser::browser_selectionChanged(Q3ListBoxItem* itm) {
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


//////////////////////////////////
//   taiItemChooser		//
//////////////////////////////////

int taiItemChooser::filt_delay = 500; 

taiItemChooser* taiItemChooser::New(const String& caption_, taiItemPtrBase* client_, 
  int ft, QWidget* par_window_) 
{
/*no, let qt choose  if (par_window_ == NULL)
    par_window_ = taiMisc::main_window;*/
  taiItemChooser* rval = new taiItemChooser(caption_, par_window_);
  rval->setFont(taiM->dialogFont(ft));
  rval->Constr(client_);
  return rval;
}

taiItemChooser::taiItemChooser(const String& caption_, QWidget* par_window_)
:inherited(par_window_)
{
  init(caption_);
}

void taiItemChooser::init(const String& caption_) {
  m_changing = 0;
  caption = caption_;
  m_selObj = NULL;
  m_selItem = NULL;
  m_client = NULL;
  m_view = -1; // until set to valid value
  setModal(true);
  setCaption(caption);
//  setFont(taiM->dialogFont(taiMisc::fonSmall));
  resize(taiM->dialogSize(taiMisc::hdlg_m));
}

void taiItemChooser::accept() {
  QTreeWidgetItem* itm = items->currentItem();
  if (itm) {
    m_selObj = (void*)QVARIANT_TO_INTPTR(itm->data(0,ObjDataRole));
  }
  m_client = NULL;
  inherited::accept();
}

QTreeWidgetItem* taiItemChooser::AddItem(const QString& itm_txt, QTreeWidgetItem* parent,
  const void* data_)
{
  QTreeWidgetItem* rval;
  if (parent)
    rval = new QTreeWidgetItem(parent);
  else
    rval = new QTreeWidgetItem(items);
  // set standard item text
  rval->setText(0, itm_txt);
  // set the object, which is an extended attribute
  //note: use the ta version because Qt uses longs on some plats
  if (data_)
    rval->setData(0, ObjDataRole, QVariant((ta_intptr_t)data_));
  return rval;
}

bool taiItemChooser::Choose(taiItemPtrBase* client_) {
//NOTE: current semantics is simple: always rebuild each show
// more complex caching would require flags etc. to track whether the
// inputs to the item changed -- cpus are so fast now, this may not be worth it
  m_client = client_;
  if (view() < 0) {
    setView(0); // triggers build, and sets sel
  } else {
    Refresh(); // input data could have changed since last view
    setSelObj(m_client->sel());
  }
  return (exec() == QDialog::Accepted);
}

void taiItemChooser::Clear() {
  timFilter->stop();
  items->clear();
  m_selObj = NULL;
  m_selItem = NULL;
}

void taiItemChooser::ClearFilter() {
  ++m_changing;
  taMisc::Busy();
  last_filter.clear();
  QTreeWidgetItemIterator it(items, QTreeWidgetItemIterator::Hidden);
  QTreeWidgetItem* item;
  while ((item = *it)) { 
    items->setItemHidden(item, false);
    ++it;
  }
  taMisc::DoneBusy();
  --m_changing;
} 

void taiItemChooser::cmbView_currentIndexChanged(int index) {
  if (m_changing > 0) return;
  setView(index);
}

void taiItemChooser::Constr(taiItemPtrBase* client_) {
  m_client = client_; // revoked at end
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(taiM->vsep_c);
  layOuter->setSpacing(taiM->vspc_c); 
  // we only put up a view selector if more than 1 view supported
  if (client_ && client_->viewCount() > 1) {
    cmbView = new QComboBox(this);
    for (int i = 0; i < client_->viewCount(); ++i) {
      cmbView->addItem(client_->viewText(i));
    }
    layOuter->addWidget(cmbView);
    connect(cmbView, SIGNAL(currentIndexChanged(int)),
      this, SLOT(cmbView_currentIndexChanged(int)) );
  } else cmbView = NULL;
  items = new QTreeWidget(this);
  items->setSortingEnabled(true);
  layOuter->addWidget(items, 1); // list is item to expand in host

  QHBoxLayout* lay = new QHBoxLayout();
  lay->addStretch();
  btnOk = new QPushButton("&Ok", this);
  btnOk->setDefault(true);
  lay->addWidget(btnOk);
  lay->addSpacing(taiM->vsep_c);
  btnCancel = new QPushButton("&Cancel", this);
  lay->addWidget(btnCancel);
  layOuter->addLayout(lay);

  lay = new QHBoxLayout();
  lay->addSpacing(taiM->hspc_c); 
  QLabel* lbl = new QLabel("search", this);
  lbl->setToolTip("Enter text that must appear in an item to keep it visible");
  lay->addWidget(lbl);
  lay->addSpacing(taiM->vsep_c);
  filter = new QLineEdit(this);
  filter->setToolTip(lbl->toolTip());
  lay->addWidget(filter, 1);
  lay->addSpacing(taiM->hspc_c); 
  layOuter->addLayout(lay);
  
  timFilter = new QTimer(this);
  timFilter->setSingleShot(true);
  timFilter->setInterval(filt_delay);
  
  connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()) );
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()) );
  connect(items, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
      this, SLOT(items_itemDoubleClicked(QTreeWidgetItem*, int)) );
  connect(filter, SIGNAL(textChanged(const QString&)),
    this, SLOT(filter_textChanged(const QString&)) );
  connect(timFilter, SIGNAL(timeout()), this, SLOT(timFilter_timeout()) );

  m_client = NULL;
}

void taiItemChooser::items_itemDoubleClicked(QTreeWidgetItem* itm, int col) {
  accept();
}

void taiItemChooser::filter_textChanged(const QString& /*text*/) {
  // following either starts timer, or restarts it
  timFilter->start();
}

void taiItemChooser::Refresh() {
  ++m_changing;
  // delete items
  Clear();
  if (m_client) { // note: should be valid
    // redo header
    int cols = m_client->columnCount(m_view); // cache
    items->setColumnCount(cols);
    QTreeWidgetItem* hi = items->headerItem();
    for (int i = 0; i < cols; ++i) {
      hi->setData(i, Qt::DisplayRole, m_client->headerText(i, m_view));
    }
    // call client to fill items
    m_client->BuildChooser(this, m_view);
    // make columns nice sizes (not last)
    for (int i = 0; i < (cols - 1); ++i) {
      items->resizeColumnToContents(i);
    }
    setSelObj(m_client->sel());
  }
  --m_changing;
}

void taiItemChooser::reject() {
//TODO: maybe shouldn't nuke sel, caller should check true/false
  m_selObj = NULL;
  m_client = NULL;
  inherited::reject();
}

bool taiItemChooser::SetCurrentItemByData(void* value) {
  // note: assumes at most 1 selectable item for NULL
  QTreeWidgetItemIterator it(items, QTreeWidgetItemIterator::Selectable);
  QTreeWidgetItem* item;
  while ((item = *it)) { 
    void* data = (void*)QVARIANT_TO_INTPTR(item->data(0, ObjDataRole));
    if (value == data) {
      items->setCurrentItem(item);
      m_selItem = item; // cache for showEvent
      items->scrollToItem(item); //note: this only works when we are visible
      return true;
    }
    ++it;
  }
  // not found
  items->setCurrentItem(NULL);
  m_selItem = NULL;
  return false;
}

void taiItemChooser::SetFilter(const QString& filt) {
  ++m_changing;
  last_filter = filt;
  taMisc::Busy();
  QTreeWidgetItemIterator it(items, QTreeWidgetItemIterator::All);
  QTreeWidgetItem* item;
  QString s;
  int cols = items->columnCount(); // cache
  while ((item = *it)) { 
    // TODO (maybe): don't hide NULL item
    bool hide = true;
    for (int i = 0; i < cols; ++i) {
      s = item->text(i);
      if (s.contains(filt, Qt::CaseInsensitive)) {
        hide = false;
        break;
      }  
    }
    items->setItemHidden(item, hide);
    ++it;
  }
  taMisc::DoneBusy();
  --m_changing;
}

void taiItemChooser::setSelObj(void* value) {
  if (m_selObj == value) return;
  m_selObj = value;
  SetCurrentItemByData(value);
}

void taiItemChooser::setView(int value, bool force) {
  if ((m_view == value) && !force) return;
  m_view = value; //so its valid for any subcalls, etc.
  Refresh();
  QString text = filter->text();
  if (!text.isEmpty()) SetFilter(text);
}

void taiItemChooser::showEvent(QShowEvent* event) {
  inherited::showEvent(event);
  QTimer::singleShot(150, this, SLOT(show_timeout()) );
}

void taiItemChooser::show_timeout() {
  if (m_selItem)
    items->scrollToItem(m_selItem);
}

void taiItemChooser::timFilter_timeout() {
  // if nothing has changed in text, do nothing
  QString text = filter->text();
  if (last_filter == text) return;
  // if we are already filtering, then don't reenter, but just try again
  if (m_changing > 0) {
    timFilter->start();
    return;
  }
  if (text.isEmpty()) ClearFilter();
  else SetFilter(text);
}


//////////////////////////////////
//   taiItemPtrBase		//
//////////////////////////////////

taiItemPtrBase::taiItemPtrBase(TypeDef* typ_,
  IDataHost* host_, taiData* par, 
  QWidget* gui_parent_, int flags_)
: taiData(typ_, host_, par, gui_parent_, flags_)
{
  targ_typ = NULL; // gets set later
  m_sel = NULL;
  QPushButton* rep_ = new QPushButton(gui_parent_);
  rep_->setFixedHeight(taiM->button_height(defSize()));
  SetRep(rep_);
  connect(rep_, SIGNAL(clicked()), this, SLOT(OpenChooser()) );
}

taiItemPtrBase::~taiItemPtrBase() {
}

void taiItemPtrBase::GetImage(void* cur_sel, TypeDef* targ_typ_) {
  targ_typ = targ_typ_;
  UpdateImage(cur_sel);
}

const String taiItemPtrBase::labelText() {
  String nm;
  if (m_sel) nm = labelNameNonNull();
  else       nm = String::con_NULL;
  return itemTag() + nm + "...";
}

void taiItemPtrBase::OpenChooser() {
//TODO: cache etc., but for now, just create/build
  taiItemChooser* ic = taiItemChooser::New("Choose " + itemTag(), this);
  if (ic->Choose(this)) {
    if (m_sel != ic->selObj()) {
      UpdateImage(ic->selObj());
      DataChanged();
    }
  }
delete ic;
}

void taiItemPtrBase::UpdateImage(void* cur_sel) {
  // note: don't optimize this if same msel, since we use it to set label
  m_sel = cur_sel;
  rep()->setText(labelText());
}


//////////////////////////////////
//   taiMemberDefButton		//
//////////////////////////////////

taiMemberDefButton::taiMemberDefButton(TypeDef* typ_, IDataHost* host,
    taiData* par, QWidget* gui_parent_, int flags_)
:inherited(typ_, host, par, gui_parent_, flags_)
{
}

void taiMemberDefButton::BuildChooser(taiItemChooser* ic, int view) {
  //assume only called if needed
  
  if (!targ_typ) {
    taMisc::Error("taiMemberDefButton::BuildChooser: targ_type needed");
    return;
  }
  switch (view) {
  case 0: 
    BuildChooser_0(ic); 
    ic->items->sortItems(0, Qt::Ascending);
    break; 
  default: break; // shouldn't happen
  }
}

void taiMemberDefButton::BuildChooser_0(taiItemChooser* ic) {
  MemberSpace* mbs = &targ_typ->members;
  for (int i = 0; i < mbs->size; ++i) {
    MemberDef* mbr = mbs->FastEl(i);
    QTreeWidgetItem* item = ic->AddItem(mbr->name, NULL, (void*)mbr);
    item->setData(1, Qt::DisplayRole, mbr->desc);
  }
}


int taiMemberDefButton::columnCount(int view) const {
  switch (view) {
  case 0: return 2;
  default: return 0; // not supposed to happen
  }
}

const String taiMemberDefButton::headerText(int index, int view) const {
  switch (view) {
  case 0: switch (index) {
    case 0: return "Member"; 
    case 1: return "Description"; 
    } break; 
  default: break; // compiler food
  }
  return _nilString; // shouldn't happen
}

const String taiMemberDefButton::labelNameNonNull() const {
  return md()->name;
}

const String taiMemberDefButton::viewText(int index) const {
  switch (index) {
  case 0: return "Flat List"; 
  default: return _nilString;
  }
}


//////////////////////////////////
//   taiMethodDefButton		//
//////////////////////////////////

taiMethodDefButton::taiMethodDefButton(TypeDef* typ_, IDataHost* host,
    taiData* par, QWidget* gui_parent_, int flags_)
:inherited(typ_, host, par, gui_parent_, flags_)
{
}

void taiMethodDefButton::BuildChooser(taiItemChooser* ic, int view) {
  //assume only called if needed
  
  if (!targ_typ) {
    taMisc::Error("taiMethodDefButton::BuildChooser: targ_type needed");
    return;
  }
  switch (view) {
  case 0: 
    BuildChooser_0(ic); 
    ic->items->sortItems(0, Qt::Ascending);
    break; 
  case 1: 
    ic->items->sortItems(1, Qt::Ascending); // so items aren't sorted by 0
    BuildChooser_1(ic, targ_typ, NULL); 
    ic->items->sortItems(1, Qt::Ascending); 
    break; 
  default: break; // shouldn't happen
  }
}

void taiMethodDefButton::BuildChooser_0(taiItemChooser* ic) {
  MethodSpace* mbs = &targ_typ->methods;
  for (int i = 0; i < mbs->size; ++i) {
    MethodDef* mth = mbs->FastEl(i);
    QTreeWidgetItem* item = ic->AddItem(mth->name, NULL, (void*)mth);
    item->setData(0, Qt::ToolTipRole, mth->prototype());
    item->setData(1, Qt::DisplayRole, mth->desc);
  }
}

int taiMethodDefButton::BuildChooser_1(taiItemChooser* ic, TypeDef* top_typ, 
  QTreeWidgetItem* top_item) 
{
  int rval = 0;
  MethodSpace* mbs = &top_typ->methods;
  QString typ_nm = top_typ->name; // let Qt share the rep
  // do methods at this level -- basically, anything living here, or not a virt override
  for (int i = 0; i < mbs->size; ++i) {
    MethodDef* mth = mbs->FastEl(i);
    if ((mth->owner != mbs) || mth->is_override) continue;
    ++rval;
    QTreeWidgetItem* item = ic->AddItem(typ_nm, top_item, (void*)mth);
    QVariant proto = mth->prototype(); // share
    item->setData(0, Qt::ToolTipRole, proto);
    item->setData(1, Qt::DisplayRole, mth->name);
    item->setData(1, Qt::ToolTipRole, proto);
    item->setData(2, Qt::DisplayRole, mth->desc);
  }
  // do parent type(s) as non-selectable folders
  for (int i = 0; i < top_typ->parents.size; ++i) {
    TypeDef* par_typ = top_typ->parents.FastEl(i);
    
    QTreeWidgetItem* item = ic->AddItem(par_typ->name, top_item, (void*)NULL);
    item->setFlags(Qt::ItemIsEnabled); // but not selectable
    //NOTE: no other column data, to keep the display clean
    // render the methods for this item -- if none, we'll just delete it
    int num = BuildChooser_1(ic, par_typ, item);
    //TODO: delete this item if num==0
    rval += num; // the result needs to include deeply nested methods
  }
  return rval;
}

int taiMethodDefButton::columnCount(int view) const {
  switch (view) {
  case 0: return 2;
  case 1: return 3;
  default: return 0; // not supposed to happen
  }
}

const String taiMethodDefButton::headerText(int index, int view) const {
  switch (view) {
  case 0: switch (index) {
    case 0: return "Method"; 
    case 1: return "Description"; 
    } break; 
  case 1: switch (index) {
    case 0: return "Class"; 
    case 1: return "Method"; 
    case 2: return "Description"; 
    } break; 
  default: break; // compiler food
  }
  return _nilString; // shouldn't happen
}

const String taiMethodDefButton::labelNameNonNull() const {
  return md()->name;
}

const String taiMethodDefButton::viewText(int index) const {
  switch (index) {
  case 0: return "Flat List"; 
  case 1: return "Grouped By Class"; 
  default: return _nilString;
  }
}



//////////////////////////////////
//   taiTypeDefButton		//
//////////////////////////////////

taiTypeDefButton::taiTypeDefButton(TypeDef* typ_, IDataHost* host,
    taiData* par, QWidget* gui_parent_, int flags_)
:inherited(typ_, host, par, gui_parent_, flags_)
{
}

taiTypeDefButton::TypeCat taiTypeDefButton::AddType_Class(TypeDef* typ_) {
  if ((typ_->ptr > 0) || (typ_->HasOption("HIDDEN"))) return TC_NoAdd;
  if (!typ_->InheritsFormal(TA_class)) // only type classes please..
    return TC_NoAdd;
  // no nested typedefs TODO: find a better way to identify nested typedefs
  if (typ_->name == "inherited") return TC_NoAdd;
  
  // we don't add templates, but we do add their children
  if (typ_->InheritsFormal(TA_templ_inst))
    return TC_NoAddCheckChildren;
  


  // don't clutter list with these..
  if((typ_->members.size==0) && (typ_->methods.size==0) && !(typ_ == &TA_taBase))
    return TC_NoAdd;		
  return TC_Add;
}

void taiTypeDefButton::BuildChooser(taiItemChooser* ic, int view) {
  //assume only called if needed
  
  if (!targ_typ) {
    taMisc::Error("taiTypeDefButton::BuildChooser: targ_type needed");
    return;
  }
  switch (view) {
  case 0: 
    if (HasFlag(flgNullOk)) {
      QTreeWidgetItem* item = ic->AddItem(" NULL", NULL, (void*)NULL); 
      item->setData(1, Qt::DisplayRole, " "); //note: no desc
    }
    BuildChooser_0(ic, targ_typ, NULL); 
    ic->items->sortItems(0, Qt::Ascending);
    break; 
  default: break; // shouldn't happen
  }
}

int taiTypeDefButton::BuildChooser_0(taiItemChooser* ic, TypeDef* top_typ, 
  QTreeWidgetItem* top_item)
{
  int rval = 0;
  QTreeWidgetItem* item = NULL; // used for intermediate items
  TypeCat tc = AddType_Class(top_typ);
  switch (tc) {
  case TC_NoAdd : return rval;
  case TC_NoAddCheckChildren: break;
  //TODO: maybe we should create a node for these, since they are typically templates
  // but then again, usually there will be a _impl just prior that will get a node
  case TC_Add: {
    TypeDef* par = top_typ->parents.SafeEl(0); // NULL for root types, ex. taBase
    String par_name;
    if (par) par_name = par->name;
    item = ic->AddItem(top_typ->name, top_item, (void*)top_typ);
    item->setData(1, Qt::DisplayRole, par_name);
    item->setData(2, Qt::DisplayRole, top_typ->desc);
    ++rval;
    } break;
  }
  for (int i = 0; i < top_typ->children.size; ++i) {
    TypeDef* chld = top_typ->children.FastEl(i);
    rval += BuildChooser_0(ic, chld, top_item); // note, we just build a list
  }
  //TODO: if a NoAddCheckChildren didn't have items, delete it
  // do initial sort
  return rval;
}

int taiTypeDefButton::columnCount(int view) const {
  switch (view) {
  case 0: return 3;
  default: return 0; // not supposed to happen
  }
}

int taiTypeDefButton::CountChildren(TypeDef* td) {
  int rval = 0;
  TypeDef* chld;
  for (int i = 0; i < td->children.size; ++i) {
    chld = td->children[i];
    if (chld->ptr != 0)
      continue;
    ++rval;
  }
  return rval;
}

const String taiTypeDefButton::headerText(int index, int view) const {
  switch (view) {
  case 0: switch (index) {
    case 0: return "Type"; 
    case 1: return "Par Type"; 
    case 2: return "Description"; 
    } break; 
  default: break; // compiler food
  }
  return _nilString; // shouldn't happen
}

const String taiTypeDefButton::labelNameNonNull() const {
  return td()->name;
}

const String taiTypeDefButton::viewText(int index) const {
  switch (index) {
  case 0: return "Flat List"; 
//case 1: return "By Class Hierarchy"; 
  default: return _nilString;
  }
}


//////////////////////////////////
//   taiEnumTypeDefButton		//
//////////////////////////////////

taiEnumTypeDefButton::taiEnumTypeDefButton(TypeDef* typ_, IDataHost* host,
    taiData* par, QWidget* gui_parent_, int flags_)
:inherited(typ_, host, par, gui_parent_, flags_)
{
}

bool taiEnumTypeDefButton::AddType_Enum(TypeDef* typ_, TypeDef* par_typ) {
//TODO: doesn't seem to work for eliminating ex taBase::Orientation
//from showing up in template types
  //note: we already determined typ_ is an enum
  if (typ_->HasOption("HIDDEN")) return false;
  // because enums are inherited, only show in the type itself
  // we will show all base and inherited from top, but then only
  // base for all inherited from top
  
  // easiest to determine is when this is directly in parent
  TypeDef* ot = typ_->GetOwnerType(); //cache
  if (ot == par_typ) return true;
  // ok, so not in parent, only other case we allow is when it
  // is inherited in the top type, then we only show in that type
  if ((par_typ == targ_typ) && (ot == targ_typ)) return true;
  return false;
}

void taiEnumTypeDefButton::BuildChooser(taiItemChooser* ic, int view) {
  //assume only called if needed
  
  if (!targ_typ) {
    taMisc::Error("taiEnumTypeDefButton::BuildChooser: targ_type needed");
    return;
  }
  switch (view) {
  case 0: 
    if (HasFlag(flgNullOk)) {
      // note: ' ' makes it sort to the top
      QTreeWidgetItem* item = ic->AddItem(" ", NULL, (void*)NULL); //note: no desc
      item->setData(1, Qt::DisplayRole, " NULL");
    }
    BuildChooser_0(ic, targ_typ, NULL); 
    ic->items->sortItems(0, Qt::Ascending);
    break; 
  default: break; // shouldn't happen
  }
}

int taiEnumTypeDefButton::BuildChooser_0(taiItemChooser* ic, TypeDef* top_typ, 
  QTreeWidgetItem* top_item)
{
  int rval = 0;
  
  // add Enums of this type
  for (int i = 0; i < top_typ->sub_types.size; ++i) {
    TypeDef* chld = top_typ->sub_types.FastEl(i);
    if (!chld->is_enum()) continue;
    if (AddType_Enum(chld, top_typ)) {
      QTreeWidgetItem* item = ic->AddItem(top_typ->name, top_item, (void*)chld);
      item->setData(1, Qt::DisplayRole, chld->name);
      item->setData(2, Qt::DisplayRole, chld->desc);
      ++rval;
    }
  }
  // add entries for the subclasses (but only full class types)
  for (int i = 0; i < top_typ->children.size; ++i) {
    TypeDef* chld = top_typ->children.FastEl(i);
    if ((chld->ptr != 0) || !chld->is_class())
      continue;
    //note: we are recursive, but aren't making a tree  
    int num = BuildChooser_0(ic, chld, top_item);
    rval += num;
  }
  return rval;
}

/*
int taiEnumTypeDefButton::BuildChooser_1(taiItemChooser* ic, TypeDef* top_typ, 
  QTreeWidgetItem* top_item)
{
  int rval = 0;
  // for top level, we need to add NULL choice
  if (!top_item) {
    if (HasFlag(flgNullOk)) {
      ic->AddItem(String::con_NULL, top_item, (void*)NULL); //note: no desc
      ++rval;
    }
  }
  
  // add Enums of this type
  for (int i = 0; i < top_typ->sub_types.size; ++i) {
    TypeDef* chld = top_typ->sub_types.FastEl(i);
    if (AddType_Enum(chld)) {
      QTreeWidgetItem* item = ic->AddItem(top_typ->name + "::" + chld->name, top_item, (void*)chld);
      item->setData(1, Qt::DisplayRole, chld->desc);
      ++rval;
    }
  }
  // add entries for the subclasses
  for (int i = 0; i < top_typ->children.size; ++i) {
    TypeDef* chld = top_typ->children.FastEl(i);
    if (chld->ptr != 0)
      continue;

    if ((CountChildren(chld) > 0) || (CountEnums(chld) > 0))
    {
      QTreeWidgetItem* item = ic->AddItem((char*)chld->name, top_item);
      item->setFlags(Qt::ItemIsEnabled); // but not selectable
      int num = BuildChooser_1(ic, chld, item);
      // if no items were actually generated, then we just delete this one
      if (num ==0) {
        delete item;
      } else rval += num;
      
    }
  }
  return rval;
}*/

int taiEnumTypeDefButton::columnCount(int view) const {
  switch (view) {
  case 0: return 3;
  default: return 0; // not supposed to happen
  }
}

const String taiEnumTypeDefButton::headerText(int index, int view) const {
  switch (view) {
  case 0: switch (index) {
    case 0: return "Type"; 
    case 1: return "Enum"; 
    case 2: return "Description"; 
    } break; 
  default: break; // compiler food
  }
  return _nilString; // shouldn't happen
}


//////////////////////////////////
//   taiTokenPtrButton		//
//////////////////////////////////

taiTokenPtrButton::taiTokenPtrButton(TypeDef* typ_, IDataHost* host,
    taiData* par, QWidget* gui_parent_, int flags_)
:inherited(typ_, host, par, gui_parent_, flags_)
{
}

void taiTokenPtrButton::BuildChooser(taiItemChooser* ic, int view) {
  //assume only called if needed
  
  if (!targ_typ) {
    taMisc::Error("taiTokenPtrButton::BuildChooser: targ_type needed");
    return;
  }
  switch (view) {
  case 0: 
    if (HasFlag(flgNullOk)) {
      // note: ' ' makes it sort to the top
      QTreeWidgetItem* item = ic->AddItem(" NULL", NULL, (void*)NULL); //note: no desc
      item->setData(1, Qt::DisplayRole, " ");
    }
    BuildChooser_0(ic, targ_typ, NULL); 
    ic->items->sortItems(0, Qt::Ascending); 
    break; 
  default: break; // shouldn't happen
  }
}

int taiTokenPtrButton::BuildChooser_0(taiItemChooser* ic, TypeDef* td, 
  QTreeWidgetItem* top_item) 
{
  if (!td->InheritsFrom(TA_taBase) 
     || (td->ptr > 0)
  ) return 0;
  int rval = 0;
  
  //NOTES:
  // if !tokens.keep then tokens.size==0
  
  for (int i = 0; i < td->tokens.size; ++i) {
    TAPtr btmp = (TAPtr)td->tokens.FastEl(i);
    if ((scope_ref) && !btmp->SameScope(scope_ref))
      continue;
    QTreeWidgetItem* item = ic->AddItem(btmp->GetColText(taBase::key_disp_name),
      top_item, (void*)btmp); 
    item->setData(1, Qt::DisplayRole, btmp->GetTypeDef()->name);
    TAPtr own = btmp->GetOwner();
    if (own) {
      item->setData(2, Qt::DisplayRole, own->GetColText(taBase::key_disp_name));
      item->setData(3, Qt::DisplayRole, own->GetColText(taBase::key_type));
    }
    ++rval;
  }

  for (int i = 0; i < td->children.size; ++i) {
    TypeDef* chld = td->children[i];
    rval += BuildChooser_0(ic, chld, top_item); //note: we don't create subnodes
  }
  return rval;
}

int taiTokenPtrButton::columnCount(int view) const {
  switch (view) {
  case 0: return 4;
  default: return 0; // not supposed to happen
  }
}

void taiTokenPtrButton::GetImage(TAPtr ths, TypeDef* targ_typ_, TAPtr scope_) {
  scope_ref = scope_;
  inherited::GetImage((void*)ths, targ_typ_);
}

const String taiTokenPtrButton::headerText(int index, int view) const {
  switch (view) {
  case 0: switch (index) {
    case 0: return "Name"; 
    case 1: return "Type"; 
    case 2: return "Owner Name"; 
    case 3: return "Owner Type"; 
    } break; 
  default: break; // compiler food
  }
  return _nilString; // shouldn't happen
}

const String taiTokenPtrButton::labelNameNonNull() const {
  return token()->GetDisplayName();
}

const String taiTokenPtrButton::viewText(int index) const {
  switch (index) {
  case 0: return "Flat List"; 
  default: return _nilString;
  }
}

//////////////////////////////////////////
// 		taiFileButton		//
//////////////////////////////////////////

taiFileButton::taiFileButton(TypeDef* typ_, IDataHost* host_, taiData* par,
	QWidget* gui_parent_, bool rd_only, bool wrt_only)
: taiButtonMenu(taiMenu::normal, taiMisc::fonSmall, typ_, host_, par, gui_parent_)
{
  gf = NULL;
  read_only = rd_only;
  write_only = wrt_only;
  setLabel("------No File-----");
}

taiFileButton::~taiFileButton() {
  SetFiler(NULL);
}

void taiFileButton::SetFiler(taFiler* gf_) {
  if (gf != gf_) {
    if (gf != NULL)
      taRefN::unRefDone(gf);
    gf = gf_;
    if (gf != NULL)
      taRefN::Ref(gf);
  }
}

void taiFileButton::GetImage() {
  if (items.size == 0) {
    if (!write_only)
      AddItem("Open", taiMenu::use_default,
	taiAction::action, this, SLOT(Open()) );
    if(!read_only && ((gf == NULL) || !gf->select_only)) {
      AddItem("Save", taiMenu::use_default,
	taiAction::action, this, SLOT(Save()) );
      AddItem("SaveAs", taiMenu::use_default,
	taiAction::action, this, SLOT(SaveAs()) );
      AddItem("Append", taiMenu::use_default,
	taiAction::action, this, SLOT(Append()) );
    }
    AddItem("Close", taiMenu::use_default,
	taiAction::action, this, SLOT(Close()) );
    AddItem("Edit", taiMenu::use_default,
	taiAction::action, this, SLOT(Edit()) );
  }

  if ((gf == NULL) || (!gf->select_only && !gf->open_file) || gf->fname.empty() )
    setLabel("------No File-----");
  else
    setLabel(gf->fname);
}

void taiFileButton::GetGetFile() {
  if (!gf) {
    SetFiler(taFiler::New());
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
  if(gf->Append()) {
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
  if (gf->SaveAs()) {
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
//  char* edtr_c = getenv("EDITOR");
  String edtr = taMisc::edit_cmd; //don't run gsub on the original string!
  edtr.gsub("%s", gf->fname);
  system(edtr);
}


taiElBase::taiElBase(taiActions* actions_, TypeDef* tp, IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_)
:taiData(tp, host_, par, gui_parent_, flags_)
{
  cur_obj = NULL;
  ta_actions = actions_;
  ownflag = false;
}

taiElBase::~taiElBase() {
  if (ownflag)
    delete ta_actions;
  ta_actions = NULL;
}

void taiElBase::setCur_obj(TAPtr value, bool do_chng) {
  if (cur_obj == value) return;
  cur_obj = value;
  ta_actions->GetImageByData(Variant(value));
/*TODO Qt4 -- this should happen automatically...
  if (value == NULL)
    ta_actions->setLabel(String::con_NULL);
  else
    ta_actions->setLabel(value->GetName()); */
  if (do_chng)
    DataChanged(NULL);
}

//////////////////////////
// 	taiToken	//
//////////////////////////

taiToken::taiToken(taiActions::RepType rt, int ft, TypeDef* typ_, IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_)
: taiElBase(NULL, typ_, host_, par, gui_parent_, flags_)
{
  ta_actions = taiActions::New(rt, taiMenu::radio_update, ft, typ_, host_, this, gui_parent_);
  ownflag = true;
  scope_ref = NULL;
}

void taiToken::Chooser() {
/*nn  QWidget* par_window = (host == NULL) ? NULL : host->widget(); */
  taiObjChooser* chs = taiObjChooser::createInstance(typ, "Tokens of Given Type", scope_ref, NULL);
  chs->setSel_obj(cur_obj); // set initial selection
  bool rval = chs->Choose();
  if (rval) {
    setCur_obj((TAPtr)chs->sel_obj()); //TODO: ***DANGEROUS CAST*** -- could possibly be non-taBase type!!!
 /*TODO: can we even do this??? is there ever actions for radio button items???   if ((ta_actions->cur_sel != NULL) && (ta_actions->cur_sel->label == "<Over max, select...>") &&
       (ta_actions->cur_sel->men_act != NULL)) {
      ta_actions->cur_sel->usr_data = (void*)chs_obj;
      ta_actions->cur_sel->men_act->Select(ta_actions->cur_sel); // call it!
    }
    else
      ta_actions->setLabel(chs->sel_str());*/
  }
  delete chs;
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
//nn  if ((bgclr == NULL) && (host != NULL)) bgclr = host->bg_color;
/*obs  bool wait = false;
  if (host != NULL) wait = host->modal; */

  gc->Edit(cur_base, false, bgclr);
}

void taiToken::GetImage(TAPtr ths) {
  GetUpdateMenu();
  setCur_obj(ths, false);
}

void taiToken::GetImage(TAPtr ths, TypeDef* new_typ, TAPtr new_scope) {
  scope_ref = new_scope;
  typ = new_typ;
  GetImage(ths);
}

void taiToken::GetUpdateMenu(const taiMenuAction* actn) {
  ta_actions->Reset();
  if (ownflag) {
    if (HasFlag(flgEditOk))
      ta_actions->AddItem("Edit...", taiMenu::normal, taiAction::action, this, SLOT(Edit()) );
    ta_actions->AddSep();
    if (HasFlag(flgNullOk)) {
      taiAction* mel = ta_actions->AddItem(String::con_NULL, taiMenu::radio, actn, (void*)NULL);
      mel->connect(taiAction::men_act, this, SLOT(ItemChosen(taiAction*)));
    }
  }
  GetMenu_impl(ta_actions, typ, actn);
}

TAPtr taiToken::GetValue() {
  return cur_obj;
}

void taiToken::GetMenu_impl(taiActions* menu, TypeDef* td, const taiMenuAction* actn) {
  if (!td->InheritsFrom(TA_taBase)) return; // sanity check, so we don't crash...

  if (!td->tokens.keep) {
    menu->AddItem("<Sorry, not keeping tokens>", taiMenu::normal);
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
      taiAction* mnel = menu->AddItem
        ("<Over max, select...>", taiMenu::normal,
        taiAction::action, this, SLOT(Chooser()) );
      if (actn != NULL) {		// also set callback action!
        mnel->connect(actn);
      }
    } else {
      taiMenuAction ma(this, SLOT(ItemChosen(taiAction*)));
      String	nm;
      for (int i = 0; i < td->tokens.size; ++i) {
        TAPtr btmp = (TAPtr)td->tokens.FastEl(i);
        if ((scope_ref != NULL) && !btmp->SameScope(scope_ref))
          continue;
        if (!btmp->GetName().empty())
          nm = btmp->GetName();
        taiAction* mel = menu->AddItem((char*)nm, taiMenu::radio, actn, Variant(btmp)); //connect caller's callback
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
//Qt3      taiMenu* submenu = menu->AddSubMenu(chld->name, (void*)chld);
      taiMenu* submenu = menu->AddSubMenu(chld->name);
//huh?? why??      menu->AddSep();
      GetMenu_impl(submenu, chld, actn);
    } else {
      GetMenu_impl(menu, chld, actn);	// if no tokens, don't add a submenu..
    }
  }
}

void taiToken::ItemChosen(taiAction* menu_el) {
  setCur_obj(menu_el->usr_data.toBase());
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
    taiAction* mnel = menu->AddItem
      ("<Over max, select...>", (void*)NULL, taiMenu::normal,
       taiAction::action, this, SLOT(Chooser()) );
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
/*obs
void taiToken::SetTypeScope(TypeDef* new_typ, TAPtr new_scope, bool force) {
  if ((new_typ == typ) && (new_scope == scope_ref) && !force) return;
  typ = new_typ;
  scope_ref = new_scope;
  GetUpdateMenu();
}*/


//////////////////////////////////
// 	taiSubToken		//
//////////////////////////////////

taiSubToken::taiSubToken(taiActions::RepType rt, int ft, TypeDef* typ_, IDataHost* host_, taiData* par,
	QWidget* gui_parent_, int flags_)
: taiElBase(NULL, typ_, host_, par, gui_parent_, flags_)
{
  menubase = NULL;
  ta_actions = taiActions::New(rt, taiMenu::radio_update, ft, typ_, host_, this, gui_parent_); //note: only needs taiMenu, but this doesn't hurt
  ownflag = true;
}

taiSubToken::taiSubToken(taiMenu* existing_menu, TypeDef* typ_, IDataHost* host_,
	taiData* par, QWidget* gui_parent_, int flags_)
: taiElBase(existing_menu, typ_, host_, par, gui_parent_, flags_)
{
  menubase = NULL;
}

QWidget* taiSubToken::GetRep() {
   return (ta_actions == NULL) ? NULL : ta_actions->GetRep();
}

void* taiSubToken::GetValue() {
  taiAction* cur = ta_actions->curSel();
  if (cur == NULL)
    return NULL;
  else
    return cur->usr_data.toPtr();
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

//nn  if ((bgclr == NULL) && (host != NULL)) bgclr = host->bg_color;
/*obs  bool modal = false;
  if (host != NULL) modal = host->modal; */

  gc->Edit(cur_base, false, bgclr);
}

void taiSubToken::GetImage(const void* ths, void* sel) {
  if (menubase != ths) {
    menubase = (void*)ths;
    UpdateMenu();
  }
  if (sel == NULL)
    sel = (void*)ths;
  if (!(ta_actions->GetImageByData(Variant(sel))))
    ta_actions->GetImageByData(Variant(ths));
}

void taiSubToken::UpdateMenu(taiMenuAction* actn){
  ta_actions->Reset();
  GetMenu(actn);
}

void taiSubToken::GetMenu(taiMenuAction* actn) {
  if (HasFlag(flgNullOk))
    ta_actions->AddItem(String::con_NULL, taiMenu::use_default, actn, (void*)NULL);
  if (HasFlag(flgEditOk))
    ta_actions->AddItem("Edit", taiMenu::normal,
      taiAction::action, this, SLOT(Edit()) );
  ta_actions->AddSep(); // note: never adds spurious seps

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
    ta_actions->AddItem(nm, taiMenu::use_default, actn, rbase);
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
//   taiTypeInfoBase	//
//////////////////////////////////

taiTypeInfoBase::taiTypeInfoBase(taiActions::RepType rt, int ft, 
  MemberDef* memb_md_, TypeDef* typ_, IDataHost* host_, taiData* par, 
  QWidget* gui_parent_, int flags_)
: taiData(typ_, host_, par, gui_parent_, flags_)
{
  targ_typ = NULL; // gets set later
  memb_md = memb_md_; 
  menubase = NULL;
  ta_actions = taiActions::New(rt, taiMenu::radio_update, ft, NULL, host_, this, gui_parent_);
}

taiTypeInfoBase::~taiTypeInfoBase() {
  if (ta_actions != NULL) {
    delete ta_actions;
    ta_actions = NULL;
  }
}

void taiTypeInfoBase::GetImage(const void* base, bool get_menu, void* cur_sel){
  menubase = (void*)base; // ok
//??  if (!typ)  return;
  if (get_menu)
    GetMenu();
  if ((!cur_sel) || (!(ta_actions->GetImageByData(Variant(cur_sel)))))
    ta_actions->GetImageByIndex(0);
}


QWidget* taiTypeInfoBase::GetRep() {
  if (ta_actions) return ta_actions->GetRep();
  else return NULL;
}

void taiTypeInfoBase::GetTarget() {
  targ_typ = typ; // may get overridden by comment directives
  if (!memb_md)  return;
  // a XxxDef* can have one of three options to specify the
  // target type for its XxxDef menu.
  // 1) a TYPE_xxxx in its comment directives
  // 2) a TYPE_ON_xxx in is comment directives, specifying the name
  //    of the member in the same object which is a TypeDef*
  // 3) Nothing, which defaults to the type of the object the memberdef
  //      is in.

  String mb_nm = memb_md->OptionAfter("TYPE_ON_");
  if (!mb_nm.empty()) {
//    taBase* base = (taBase*)host->cur_base; //TODO: highly unsafe cast -- should provide As_taBase() or such in taiDialog
    if (menubase != NULL) {
      void* adr; // discarded
      MemberDef* tdmd = ((taBase*)menubase)->FindMembeR(mb_nm, adr); //TODO: highly unsafe cast!!
      if (tdmd != NULL)
        targ_typ = *((TypeDef **) tdmd->GetOff(menubase));
    }
    return;
  } 
  
  mb_nm = memb_md->OptionAfter("TYPE_");
  if (!mb_nm.empty()) {
    targ_typ = taMisc::types.FindName(mb_nm);
    return;
  }
}

/*
//////////////////////////////////
// 	taiMemberDefMenu	//
//////////////////////////////////

taiMemberDefMenu::taiMemberDefMenu(taiActions::RepType rt, int ft, MemberDef* md_, 
  MemberDef* memb_md_, TypeDef* typ_, IDataHost* host_, taiData* par, 
  QWidget* gui_parent_, int flags_)
: inherited(rt, ft, memb_md_, typ_, host_, par, gui_parent_, flags_)
{
  md = md_;
}

MemberDef* taiMemberDefMenu::GetValue() {
  taiAction* cur = ta_actions->curSel();
  if (cur == NULL)
    return NULL;
  else
    return (MemberDef*)(cur->usr_data.toPtr());
}

void taiMemberDefMenu::GetMenu() {
  ta_actions->Reset();
  //always get target, because it could be dynamic
  GetTarget();

  if (targ_typ == NULL) {
    ta_actions->AddItem("!!!TypeSpace Error!!!");
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
    ta_actions->AddItem(mbd->GetLabel(), mbd);
  }
}
*/

//////////////////////////////////
// 	taiMethodDefMenu	//
//////////////////////////////////

taiMethodDefMenu::taiMethodDefMenu(taiActions::RepType rt, int ft, MethodDef* md_, 
  MemberDef* memb_md_, TypeDef* typ_, IDataHost* host_, taiData* par, 
  QWidget* gui_parent_, int flags_)
: inherited(rt, ft, memb_md_, typ_, host_, par, gui_parent_, flags_)
{
  md = md_;
  sp =  NULL;
}


MethodDef* taiMethodDefMenu::GetValue() {
  taiAction* cur = ta_actions->curSel();
  if (cur != NULL)
    return (MethodDef*)cur->usr_data.toPtr();
  return NULL;
}

void taiMethodDefMenu::UpdateMenu(const taiMenuAction* actn) {
  ta_actions->Reset();
  GetMenu(actn);
}

void taiMethodDefMenu::GetMenu(const taiMenuAction* actn) {
  ta_actions->Reset();
  //always get target, because it could be dynamic
  GetTarget();
  if (!targ_typ) {
    ta_actions->AddItem("TypeSpace Error");
    return;
  }
  MethodDef* mbd;
  MethodSpace* mbs = sp;
  if (!mbs) 
    mbs = &targ_typ->methods;
  for (int i = 0; i < mbs->size; ++i){
    mbd = mbs->FastEl(i);
    ta_actions->AddItem(mbd->GetLabel(),taiMenu::use_default, actn,mbd);
  }
}

//////////////////////////////////
// 	taiTypeHier		//
//////////////////////////////////

taiTypeHier::taiTypeHier(taiActions::RepType rt, int ft, TypeDef* typ_, IDataHost* host_,
	taiData* par, QWidget* gui_parent_, int flags_)
: taiData(typ_, host_, par, gui_parent_, flags_)
{
  ta_actions = taiActions::New(rt, taiMenu::radio_update, ft, typ, host_, this, gui_parent_);
  ownflag = true;
  enum_mode = false;
}

taiTypeHier::taiTypeHier(taiMenu* existing_menu, TypeDef* typ_,
	IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_)
: taiData(typ_, host_, par, gui_parent_, flags_)
{
  ta_actions = existing_menu;
  ownflag = false;
  enum_mode = false;
}

taiTypeHier::~taiTypeHier() {
  if(ownflag && (ta_actions != NULL)) delete ta_actions;
  ta_actions = NULL;
}

bool taiTypeHier::AddType_Class(TypeDef* typ_) {
  if (typ_->HasOption("HIDDEN")) return false;
  if (!typ_->InheritsFormal(TA_class)) // only type classes please..
    return false;
  // no templates (since a template is not itself a type)
  if (typ_->InheritsFormal(TA_templ_inst))
    return false;
  // no nested typedefs TODO: find a better way to identify nested typedefs
  if (typ_->name == "inherited") return false;


  // don't clutter list with these..
  if((typ_->members.size==0) && (typ_->methods.size==0) && !(typ_ == &TA_taBase))
    return false;		
  return true;
}

bool taiTypeHier::AddType_Enum(TypeDef* typ_) {
  if (typ_->HasOption("HIDDEN")) return false;
  if (typ_->enum_vals.size == 0) return false; // not an enum type
  return true;
}

int taiTypeHier::CountChildren(TypeDef* td) {
  int rval = 0;
  TypeDef* chld;
  for (int i = 0; i < td->children.size; ++i) {
    chld = td->children[i];
    if (chld->ptr != 0)
      continue;
    ++rval;
  }
  return rval;
}

int taiTypeHier::CountEnums(TypeDef* td) {
  int rval = 0;
  TypeDef* chld;
  for (int i = 0; i < td->sub_types.size; ++i) {
    chld = td->sub_types.FastEl(i);
    if (!AddType_Enum(chld))
      continue;
    ++rval;
  }
  return rval;
}

void taiTypeHier::GetImage(TypeDef* ths) {
  ta_actions->GetImageByData(Variant((void*)ths));
}

void taiTypeHier::GetMenu(const taiMenuAction* acn) {
  if (HasFlag(flgNullOk))
    ta_actions->AddItem(String::con_NULL, taiMenu::use_default, acn, (void*)NULL);
  if (enum_mode)
    GetMenu_Enum_impl(ta_actions, typ, acn);
  else
    GetMenu_impl(ta_actions, typ, acn);
}

QWidget* taiTypeHier::GetRep() {
  return ta_actions->GetRep();
}

void taiTypeHier::GetMenu_Enum_impl(taiActions* menu, TypeDef* typ_, const taiMenuAction* acn) {
  // add Enums of this type
  for (int i = 0; i < typ_->sub_types.size; ++i) {
    TypeDef* chld = typ_->sub_types.FastEl(i);
    if (AddType_Enum(chld)) {
      menu->AddItem(typ_->name + "::" + chld->name, taiMenu::use_default, acn, (void*)chld);
    }
  }
  menu->AddSep(false); 
  // add entries for the subclasses
  for (int i = 0; i < typ_->children.size; ++i) {
    TypeDef* chld = typ_->children.FastEl(i);
    if (chld->ptr != 0)
      continue;

    if ((CountChildren(chld) > 0) || (CountEnums(chld) > 0))
    {
      taiMenu* chld_menu = menu->AddSubMenu((char*)chld->name);
      GetMenu_Enum_impl(chld_menu, chld, acn);
    }
  }
}

void taiTypeHier::GetMenu_impl(taiActions* menu, TypeDef* typ_, const taiMenuAction* acn) {
  // it is hard to do recursive menus, so we just build optimistically, then delete empties
  if (AddType_Class(typ_)) {
    menu->AddItem((char*)typ_->name, taiMenu::use_default, acn, (void*)typ_);
    menu->AddSep(false); //don't start new radio group
  }
  for (int i = 0; i < typ_->children.size; ++i) {
    TypeDef* chld = typ_->children.FastEl(i);
    if (chld->ptr != 0)
      continue;
    if (!AddType_Class(chld)) {
      if (chld->InheritsFormal(TA_templ_inst) && (chld->children.size == 1)) {
	GetMenu_impl(menu, chld->children.FastEl(0), acn);
      }
      continue;
    }

    if (CountChildren(chld) > 0) {
      taiMenu* chld_menu = menu->AddSubMenu((char*)chld->name);
      GetMenu_impl(chld_menu, chld, acn);
    }  else
      menu->AddItem((char*)chld->name, taiMenu::use_default, acn, (void*)chld);
  }
}

TypeDef* taiTypeHier::GetValue() {
  taiAction* cur = ta_actions->curSel();
  if (cur) return (TypeDef*)cur->usr_data.toPtr(); return NULL;
}

void taiTypeHier::UpdateMenu(const taiMenuAction* acn) {
  ta_actions->Reset();
  GetMenu(acn);
}


///////////////////////////
// 	taiMethodData     //
///////////////////////////

taiMethodData::taiMethodData(void* bs, MethodDef* md, TypeDef* typ_, IDataHost* host_, taiData* par,
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

void taiMethodData::AddToMenu(taiActions* mnu) {
  if (meth->HasOption("MENU_SEP_BEFORE"))
    mnu->AddSep();
  mnu->AddItem(meth->GetLabel(), taiMenu::use_default,
	taiAction::action, this, SLOT(CallFun()) );
  if (meth->HasOption("MENU_SEP_AFTER"))
    mnu->AddSep();
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
//nn  if ((bgclr == NULL) && (host != NULL)) bgclr = host->bg_color;
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

QAbstractButton* taiMethodData::MakeButton() {
  if (buttonRep == NULL) {
    //buttonRep = new QPushButton(meth->GetLabel(), gui_parent);
    buttonRep = new QToolButton(gui_parent);
    buttonRep->setText(meth->GetLabel());
    connect(buttonRep, SIGNAL(clicked()),
      this, SLOT(CallFun()) );
    buttonRep->show();
  }
  return buttonRep;
}

void taiMethodData::ShowReturnVal(cssEl* rval) {
  if (!(meth->HasOption("USE_RVAL")  ||
       (meth->HasOption("USE_RVAL_RMB") && (arg_dlg->mouse_button == Qt::RightButton)) ))
    return;

  if ((rval->GetType() == cssEl::T_TA) || (rval->GetType() == cssEl::T_Class)) {
    if (host != NULL)
      rval->Edit(host->isModal());
    else
      rval->Edit(false);
    return;
  }
  String val = meth->name + " Return Value: ";
  val += rval->PrintStr();
  taiChoiceDialog::ChoiceDialog(NULL, val, "Return Value", true);
}

void taiMethodData::ApplyBefore() {
//nn??  if ((host == NULL) || (host->state != IDataHost::ACTIVE))
  if (host == NULL) 
    return;
  if (meth->HasOption("NO_APPLY_BEFORE") || !host->HasChanged())
    return;
  if (taMisc::auto_revert == taMisc::CONFIRM_REVERT) {
    int chs = taiChoiceDialog::ChoiceDialog
      (NULL, "Auto Apply/Revert: You have edited the dialog--apply or revert and lose changes?,Apply,Revert");
    if (chs == 0)
      host->GetValue();
  } else {
    host->GetValue();
  }
}

void taiMethodData::UpdateAfter() {
  if (meth->HasOption("NO_REVERT_AFTER"))
    return;
  // this is for stuff just called from menus, not host
  if ((host == NULL) || 
    (host->GetTypeDef()->InheritsFrom(&TA_taiDataHost) && 
    (((taiDataHost*)host->This())->state != taiDataHost::ACTIVE)) ) 
  {
    if(base == NULL) return;
    TAPtr tap = (TAPtr)base;
    if (meth->HasOption("UPDATE_MENUS"))
      taiMisc::Update(tap);
    return;
  }
  // this is inside the host itself
  if ((host->GetBaseTypeDef() != NULL) && 
    host->GetBaseTypeDef()->InheritsFrom(TA_taBase)) 
  {
    TAPtr tap = (TAPtr)host->Base();
    if (tap && meth->HasOption("UPDATE_MENUS")) {
      taiMisc::Update(tap);	// update menus and stuff
      tap->UpdateAllViews(); // tell others to update
    }
  }
/*obs  // almost always revert host..
  host->Revert();		// apply stuff dealt with already*/
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

taiMethMenu::taiMethMenu(void* bs, MethodDef* md, TypeDef* typ_, IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_)
: taiMethodData(bs, md, typ_, host_, par, gui_parent_, flags_) {
  is_menu_item = true;
}


/////////////////////////////
// 	taiMethButton    //
/////////////////////////////

taiMethButton::taiMethButton(void* bs, MethodDef* md, TypeDef* typ_, IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_)
: taiMethodData(bs, md, typ_, host_, par, gui_parent_, flags_)
{
  is_menu_item = false;
  if (gui_parent) SetRep(MakeButton()); //note: later code in win_base.cc etc. has convoluted menu
  // code that will end up spuriously invoking this, unless we prevent it.
}


//////////////////////////////
// 	taiMethToggle     //
//////////////////////////////

taiMethToggle::taiMethToggle(void* bs, MethodDef* md, TypeDef* typ_, IDataHost* host_, taiData* par,
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



