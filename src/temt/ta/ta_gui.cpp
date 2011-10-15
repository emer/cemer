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

#include "ta_gui.h"

#include "ta_project.h"
#include "ta_datatable_qtso.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QFrame>
#include <QMenu>
#include <QToolbar>
#include <QDesktopServices>

/////////////////////////////////////////////////////
//  taGui
/////////////////////////////////////////////////////

void taGuiWidget::Initialize() {
  m_helper = new taGuiWidgetHelper(this);
  tai_data = NULL;
}

void taGuiWidget::Destroy() {
  delete m_helper;
  if(tai_data)
    delete tai_data;
}

void taGuiWidget::Connect_UrlAction(QObject* src_obj, const char* src_signal) {
  static const char* slot_nm = SLOT(UrlAction());
  QObject::connect(src_obj, src_signal, m_helper, slot_nm);
}

void taGuiWidget::UrlAction() {
  if(action_url.empty()) return;
  QDesktopServices::openUrl(QUrl(action_url));
}

void taGuiWidget::GetImage() {
  if(!tai_data || data.isNull()) return;
  if(widget_type == "IntField") {
    ((taiIncrField*)tai_data)->GetImage((String)*((int*)data.toPtr()));
  }
  if(widget_type == "DoubleField") {
    ((taiField*)tai_data)->GetImage((String)*((double*)data.toPtr()));
  }
  if(widget_type == "FloatField") {
    ((taiField*)tai_data)->GetImage((String)*((float*)data.toPtr()));
  }
  if(widget_type == "StringField") {
    ((taiField*)tai_data)->GetImage(*((String*)data.toPtr()));
  }
  if(widget_type == "BoolCheckbox") {
    ((taiToggle*)tai_data)->GetImage(*((bool*)data.toPtr()));
  }
  if(widget_type == "ObjectPtr") {
    String typnm = taGuiDialog::GetAttribute("type=", attributes);
    TypeDef* td;
    if(typnm.nonempty()) td = taMisc::types.FindName(typnm);
    if(!td) td = &TA_taOBase;
    ((taiTokenPtrButton*)tai_data)->GetImage(((taBaseRef*)data.toPtr())->ptr(), td);
  }
}

void taGuiWidget::GetValue() {
  if(!tai_data || data.isNull()) return;
  if(widget_type == "IntField") {
    *((int*)data.toPtr()) = (int)((taiIncrField*)tai_data)->GetValue();
  }
  if(widget_type == "DoubleField") {
    *((double*)data.toPtr()) = (double)((taiField*)tai_data)->GetValue();
  }
  if(widget_type == "FloatField") {
    *((float*)data.toPtr()) = (float)((taiField*)tai_data)->GetValue();
  }
  if(widget_type == "StringField") {
    *((String*)data.toPtr()) = ((taiField*)tai_data)->GetValue();
  }
  if(widget_type == "BoolCheckbox") {
    *((bool*)data.toPtr()) = ((taiToggle*)tai_data)->GetValue();
  }
  if(widget_type == "ObjectPtr") {
    *((taBaseRef*)data.toPtr()) = ((taiTokenPtrButton*)tai_data)->GetValue();
  }
}

void taGuiWidget_List::Initialize() {
  SetBaseType(&TA_taGuiWidget);
}

void taGuiWidget_List::GetImage() {
  for(int i=0; i<size; i++) {
    taGuiWidget* w = FastEl(i);
    w->GetImage();
  }
}

void taGuiWidget_List::GetValue() {
  for(int i=0; i<size; i++) {
    taGuiWidget* w = FastEl(i);
    w->GetValue();
  }
}

void taGuiLayout_List::Initialize() {
  SetBaseType(&TA_taGuiLayout);
}

////////////////////////////////////////////
//	 Actions

void taGuiAction::Initialize() {
  m_helper = new taGuiActionHelper(this);
}

void taGuiAction::Destroy() {
  delete m_helper;
}

void taGuiAction::Connect_UrlAction(QObject* src_obj, const char* src_signal) {
  static const char* slot_nm = SLOT(UrlAction());
  QObject::connect(src_obj, src_signal, m_helper, slot_nm);
}

void taGuiAction::UrlAction() {
  if(action_url.empty()) return;
  QDesktopServices::openUrl(QUrl(action_url));
}

void taGuiAction_List::Initialize() {
  SetBaseType(&TA_taGuiAction);
}


/////////////////////////////////////////////////////
//  taGuiDataHost
/////////////////////////////////////////////////////

taGuiDataHost::taGuiDataHost(taGuiDialog* own, bool read_only_, bool modal_, QObject* parent)
  : taiDataHostBase(&TA_taGuiDialog, read_only_, modal_, parent) {
  gui_owner = own;
}

taGuiDataHost::~taGuiDataHost() {
}

void taGuiDataHost::Constr_Body() {
  if(gui_owner->widgets.size > 0) {
    taGuiWidget* fw = gui_owner->widgets.FastEl(0);
    fw->widget->setParent(mwidget);
    vblDialog->addWidget(fw->widget);
  }
}

void taGuiDataHost::GetImage(bool force) {
  gui_owner->GetImage();
}

void taGuiDataHost::GetValue() {
  gui_owner->GetValue();
}

void taGuiDataHost::Ok_impl() {
  GetValue();
}

/////////////////////////////////////////////////////
//  taGuiDialog
/////////////////////////////////////////////////////

void taGuiDialog::Initialize() {
  data_host = NULL;
  width = 300;
  height = 200;
}

void taGuiDialog::Destroy() {
  if(data_host)
    delete data_host;
}

void taGuiDialog::Reset() {
  widgets.Reset();
  layouts.Reset();
  actions.Reset();
  if(data_host)
    delete data_host;
  data_host = new taGuiDataHost(this, false, false, NULL); // not modal by default
}

int taGuiDialog::PostDialog(bool modal) {
  data_host->Constr(prompt, win_title);
  int rval = data_host->Edit(modal, width, height);
  return rval;
}

void taGuiDialog::Cancel() {
  data_host->Cancel();
}
void taGuiDialog::Ok() {
  data_host->Ok();
}
void taGuiDialog::Apply() {
  data_host->Apply();
}
void taGuiDialog::Revert() {
  data_host->Revert();
}

void taGuiDialog::GetImage() {
  widgets.GetImage();
}

void taGuiDialog::GetValue() {
  widgets.GetValue();
  Program* prog = GET_MY_OWNER(Program);
  if(prog) {
    prog->UpdateAfterEdit();	// trigger update of gui
  }
}

taGuiWidget* taGuiDialog::FindWidget(const String& nm, bool err_msg) {
  taGuiWidget* rval = widgets.FindName(nm);
  TestError(!rval && err_msg, "FindWidget", "widget named", nm, "not found!");
  return rval;
}

taGuiLayout* taGuiDialog::FindLayout(const String& nm, bool err_msg) {
  taGuiLayout* rval = layouts.FindName(nm);
  TestError(!rval && err_msg, "FindLayout", "layout named", nm, "not found!");
  return rval;
}

taGuiAction* taGuiDialog::FindAction(const String& nm, bool err_msg) {
  taGuiAction* rval = actions.FindName(nm);
  TestError(!rval && err_msg, "FindAction", "action named", nm, "not found!");
  return rval;
}

String taGuiDialog::GetAttribute(const String& key, const String& attributes) {
  if(!attributes.contains(key)) return "";
  String rval = attributes.after(key);
  if(!rval.contains(';'))
    taMisc::Warning("taGuiDialog::GetAttribute", "attribute is not ; delimited!", key, rval);
  if(rval.contains(';')) rval = rval.before(';');
  return trim(rval);
}

taGuiWidget* taGuiDialog::AddWidget_impl(QWidget* widg, const String& nm, const String& typ,
					 const String& layout, const String& attributes,
					 Variant data, const String& url, taiData* taidata) {
  if(!data_host) Reset();		// make sure constructed!
  if(!widg) return NULL;
  if(layout.nonempty()) {
    taGuiLayout* lay = FindLayout(layout, true);
    if(!lay) {
      delete widg;
      return NULL;
    }
    lay->layout->addWidget(widg);
  }
  taGuiWidget* wid = (taGuiWidget*)widgets.New(1);
  wid->SetName(nm);
  wid->widget_type = typ;
  wid->attributes = attributes;
  wid->data = data;
  if(taidata)
    wid->tai_data = taidata;
  wid->action_url = url;
  Program* prog = GET_MY_OWNER(Program);
  if(prog) {
    if(wid->action_url.startsWith("this.")) {
      wid->action_url = "ta:" + prog->GetPath() + "." + wid->action_url.after("this.");
    }
  }
  wid->widget = widg;

  widg->setFont(taiM->nameFont(data_host->ctrl_size)); // start with defaults
  // general attributes
  { String att = GetAttribute("tooltip=", attributes);
    if(att.nonempty()) wid->widget->setToolTip(att); }
  { String att = GetAttribute("min_width=", attributes);
    if(att.nonempty()) wid->widget->setMinimumWidth((int)att); }
  { String att = GetAttribute("max_width=", attributes);
    if(att.nonempty()) wid->widget->setMaximumWidth((int)att); }
  { String att = GetAttribute("min_height=", attributes);
    if(att.nonempty()) wid->widget->setMinimumHeight((int)att); }
  { String att = GetAttribute("max_height=", attributes);
    if(att.nonempty()) wid->widget->setMaximumHeight((int)att); }
  { String att = GetAttribute("font=", attributes);
    if(att.nonempty()) wid->widget->setFont(QFont(att, taMisc::font_size)); }
  { String att = GetAttribute("font_size=", attributes);
    if(att.nonempty()) {
      QFont font = wid->widget->font(); font.setPointSize((int)att); 
      wid->widget->setFont(font); } }
  { String att = GetAttribute("bold=", attributes);
    if(att.nonempty()) {
      QFont font = wid->widget->font(); font.setBold(att.toBool()); 
      wid->widget->setFont(font); } }
  { String att = GetAttribute("italic=", attributes);
    if(att.nonempty()) {
      QFont font = wid->widget->font(); font.setItalic(att.toBool()); 
      wid->widget->setFont(font); } }
  return wid;
}

taGuiAction* taGuiDialog::AddAction_impl(QAction* act, const String& nm,
					 const String& toolbar, const String& menu,
					 const String& attributes, const String& url) {
  if(!data_host) Reset();		// make sure constructed!
  if(!act) return NULL;

  taGuiAction* tact = (taGuiAction*)actions.New(1);
  tact->SetName(nm);
  tact->attributes = attributes;
  tact->action_url = url;
  tact->toolbar = toolbar;
  tact->menu = menu;
  Program* prog = GET_MY_OWNER(Program);
  if(prog) {
    if(tact->action_url.startsWith("this.")) {
      tact->action_url = "ta:" + prog->GetPath() + "." + tact->action_url.after("this.");
    }
  }
  tact->action = act;

  if(url.nonempty())
    tact->Connect_UrlAction(act, SIGNAL(triggered(bool)));

  act->setFont(taiM->nameFont(data_host->ctrl_size)); // start with defaults

  { String att = GetAttribute("font=", attributes);
    if(att.nonempty()) act->setFont(QFont(att, taMisc::font_size)); }
  { String att = GetAttribute("font_size=", attributes);
    if(att.nonempty()) {
      QFont font = act->font(); font.setPointSize((int)att); 
      act->setFont(font); } }
  { String att = GetAttribute("bold=", attributes);
    if(att.nonempty()) {
      QFont font = act->font(); font.setBold(att.toBool()); 
      act->setFont(font); } }
  { String att = GetAttribute("italic=", attributes);
    if(att.nonempty()) {
      QFont font = act->font(); font.setItalic(att.toBool()); 
      act->setFont(font); } }
  { String att = GetAttribute("tooltip=", attributes);
    if(att.nonempty()) act->setToolTip(att); }
  { String att = GetAttribute("checkable=", attributes);
    if(att.nonempty()) act->setCheckable(att.toBool()); }
  { String att = GetAttribute("shortcut=", attributes);
    if(att.nonempty()) act->setShortcut(QKeySequence(att.chars())); }

  return tact;
}

bool taGuiDialog::AddWidget(const String& nm, const String& parent, const String& layout,
			    const String& attributes) {
  QWidget* widg = NULL;
  if(parent.nonempty()) {
    taGuiWidget* par = FindWidget(parent, true);
    if(!par) return false;
    widg = new QWidget(par->widget);
  }
  else
    widg = new QWidget();
  return (bool)AddWidget_impl(widg, nm, "Widget", layout, attributes);
}

bool taGuiDialog::AddFrame(const String& nm, const String& parent, const String& layout,
			   const String& attributes) {
  QFrame* widg = NULL;
  if(parent.nonempty()) {
    taGuiWidget* par = FindWidget(parent, true);
    if(!par) return false;
    widg = new QFrame(par->widget);
  }
  else
    widg = new QFrame();
  { String att = GetAttribute("shape=", attributes);
    if(att == "NoFrame") widg->setFrameShape(QFrame::NoFrame);
    if(att == "Box") widg->setFrameShape(QFrame::Box);
    if(att == "Panel") widg->setFrameShape(QFrame::Panel);
    if(att == "WinPanel") widg->setFrameShape(QFrame::WinPanel);
    if(att == "HLine") widg->setFrameShape(QFrame::HLine);
    if(att == "VLine") widg->setFrameShape(QFrame::VLine);
    if(att == "StyledPanel") widg->setFrameShape(QFrame::StyledPanel);
  }
  { String att = GetAttribute("shadow=", attributes);
    if(att == "Plain") widg->setFrameShadow(QFrame::Plain);
    if(att == "Raised") widg->setFrameShadow(QFrame::Raised);
    if(att == "Sunken") widg->setFrameShadow(QFrame::Sunken);
  }
  { String att = GetAttribute("line_width=", attributes);
    if(att.nonempty()) widg->setLineWidth((int)att); }
  { String att = GetAttribute("mid_line_width=", attributes);
    if(att.nonempty()) widg->setMidLineWidth((int)att); }
  return (bool)AddWidget_impl(widg, nm, "Frame", layout, attributes);
}

bool taGuiDialog::AddLabel(const String& nm, const String& parent, const String& layout,
			   const String& attributes) {
  QLabel* widg = NULL;
  if(parent.nonempty()) {
    taGuiWidget* par = FindWidget(parent, true);
    if(!par) return false;
    widg = new QLabel(par->widget);
  }
  else
    widg = new QLabel();
  String label = GetAttribute("label=", attributes);
  if(label.empty()) label = nm;	// backup
  widg->setText(label);
  return (bool)AddWidget_impl(widg, nm, "Label", layout, attributes);
}

bool taGuiDialog::AddPushButton(const String& nm, const String& parent, const String& layout,
				const String& url, const String& attributes) {
  QPushButton* widg = NULL;
  if(parent.nonempty()) {
    taGuiWidget* par = FindWidget(parent, true);
    if(!par) return false;
    widg = new QPushButton(par->widget);
  }
  else
    widg = new QPushButton();
  String att = GetAttribute("label=", attributes);
  if(att.nonempty())
    widg->setText(att);
  else
    widg->setText(nm);
  taGuiWidget* w = AddWidget_impl(widg, nm, "PushButton", layout, attributes, _nilVariant, url);
  w->Connect_UrlAction(widg, SIGNAL(released()));
  return (bool)w;
}

bool taGuiDialog::AddToolButton(const String& nm, const String& parent, const String& layout,
				const String& url, const String& attributes) {
  QToolButton* widg = NULL;
  if(parent.nonempty()) {
    taGuiWidget* par = FindWidget(parent, true);
    if(!par) return false;
    widg = new QToolButton(par->widget);
  }
  else
    widg = new QToolButton();
  String att = GetAttribute("label=", attributes);
  if(att.nonempty())
    widg->setText(att);
  else
    widg->setText(nm);
  taGuiWidget* w = AddWidget_impl(widg, nm, "ToolButton", layout, attributes, _nilVariant, url);
  w->Connect_UrlAction(widg, SIGNAL(released()));
  return (bool)w;
}

bool taGuiDialog::AddToolBar(const String& nm, const String& parent, const String& layout,
			     const String& attributes) {
  QToolBar* widg = NULL;
  if(parent.nonempty()) {
    taGuiWidget* par = FindWidget(parent, true);
    if(!par) return false;
    widg = new QToolBar(par->widget);
  }
  else
    widg = new QToolBar();
  taGuiWidget* w = AddWidget_impl(widg, nm, "ToolBar", layout, attributes, _nilVariant);
  return (bool)w;
}

bool taGuiDialog::AddSeparator(const String& toolbar, const String& menu) {
  if(TestError(toolbar.empty(), "AddSeparator", "a toolbar is required"))
    return false;
  taGuiWidget* par = FindWidget(toolbar, true);
  if(!par) return false;
  if(TestError(!(par->widget_type == "ToolBar"),
	       "AddAction", "destination toolbar to add to was not a toolbar, is:",
	       par->widget_type, "named:", par->name))
    return false;
  if(menu.nonempty()) {
    taGuiAction* mnu = FindAction(menu, false);
    if(mnu) {
      QMenu* mm = mnu->action->menu();
      if(TestError(!mm, "AddAction", "menu named:", menu, "is not actually a menu!"))
	return false;
      mm->addSeparator();
    }
    else {
      taGuiWidget* wmnu = FindWidget(menu, false);
      if(wmnu) {
	QMenu* mm = ((QToolButton*)wmnu->widget.data())->menu();
	if(TestError(!mm, "AddAction", "menu named:", menu, "is not actually a menu!"))
	  return false;
	mm->addSeparator();
      }
    }
  }
  else {
    QToolBar* tbar = (QToolBar*)par->widget.data();
    tbar->addSeparator();
  }
  return true;
}
  
bool taGuiDialog::AddMenu(const String& nm, const String& toolbar, const String& menu, 
			  const String& attributes) {
  if(TestError(toolbar.empty(), "AddAction", "a toolbar is required"))
    return false;
  taGuiWidget* par = FindWidget(toolbar, true);
  if(!par) return false;
  if(TestError(!(par->widget_type == "ToolBar"),
	       "AddAction", "destination toolbar to add to was not a toolbar, is:",
	       par->widget_type, "named:", par->name))
    return false;
  String label = GetAttribute("label=", attributes);
  if(label.empty()) label = nm;	// backup
  QAction* mnu = NULL; 
  if(menu.nonempty()) {
    taGuiAction* act = FindAction(menu, true);
    if(!act) return false;
    QMenu* mm = act->action->menu();
    if(TestError(!mm, "AddMenu", "menu named:", menu, "is not actually a menu!"))
      return false;
    mnu = mm->addAction(label);
    mnu->setMenu(new QMenu(label));
    AddAction_impl(mnu, nm, attributes);
  }
  else {
    QToolBar* tbar = (QToolBar*)par->widget.data();
    QToolButton* tb = new QToolButton(tbar);
    tbar->addWidget(tb);
    tb->setPopupMode(QToolButton::InstantPopup);
    tb->setMenu(new QMenu(label));
    tb->setText(label);
    AddWidget_impl(tb, nm, "Menu", "", attributes);
  }
  return true;
}

bool taGuiDialog::AddAction(const String& nm, const String& toolbar, const String& menu,
			    const String& url, const String& attributes) {
  if(TestError(toolbar.empty(), "AddAction", "a toolbaris required"))
    return false;
  taGuiWidget* par = FindWidget(toolbar, true);
  if(!par) return false;
  if(TestError(!(par->widget_type == "ToolBar"),
	       "AddAction", "destination toolbar to add to was not a toolbar, is:",
	       par->widget_type, "named:", par->name))
    return false;
  String label = GetAttribute("label=", attributes);
  if(label.empty()) label = nm;	// backup
  QAction* act = NULL;
  if(menu.nonempty()) {
    taGuiAction* mnu = FindAction(menu, false);
    if(mnu) {
      QMenu* mm = mnu->action->menu();
      if(TestError(!mm, "AddAction", "menu named:", menu, "is not actually a menu!"))
	return false;
      act = mm->addAction(label);
    }
    else {
      taGuiWidget* wmnu = FindWidget(menu, false);
      if(wmnu) {
	QMenu* mm = ((QToolButton*)wmnu->widget.data())->menu();
	if(TestError(!mm, "AddAction", "menu named:", menu, "is not actually a menu!"))
	  return false;
	act = mm->addAction(label);
      }
    }
  }
  else {
    QToolBar* tbar = (QToolBar*)par->widget.data();
    act = tbar->addAction(label);
  }
  return (bool)AddAction_impl(act, nm, toolbar, menu, attributes, url);
}

bool taGuiDialog::AddIntField(int* int_var, const String& nm, const String& parent,
			      const String& layout, const String& attributes) {
  if(!int_var) return false;
  if(TestError(parent.empty(), "AddIntField", "a parent widget is required"))
    return false;
  taGuiWidget* par = FindWidget(parent, true);
  if(!par) return false;
  taiIncrField* taidata = new taiIncrField(&TA_int, data_host, NULL, par->widget);
  taGuiWidget* w = AddWidget_impl(taidata->GetRep(), nm, "IntField", layout, attributes, 
				  Variant((void*)int_var), _nilString, taidata);
  return (bool)w;
}

bool taGuiDialog::AddDoubleField(double* dvar, const String& nm, const String& parent,
			      const String& layout, const String& attributes) {
  if(!dvar) return false;
  if(TestError(parent.empty(), "AddDoubleField", "a parent widget is required"))
    return false;
  taGuiWidget* par = FindWidget(parent, true);
  if(!par) return false;
  taiField* taidata = new taiField(&TA_int, data_host, NULL, par->widget);
  taGuiWidget* w = AddWidget_impl(taidata->GetRep(), nm, "DoubleField", layout, attributes, 
				  Variant((void*)dvar), _nilString, taidata);
  return (bool)w;
}

bool taGuiDialog::AddFloatField(float* dvar, const String& nm, const String& parent,
			      const String& layout, const String& attributes) {
  if(!dvar) return false;
  if(TestError(parent.empty(), "AddFloatField", "a parent widget is required"))
    return false;
  taGuiWidget* par = FindWidget(parent, true);
  if(!par) return false;
  taiField* taidata = new taiField(&TA_int, data_host, NULL, par->widget);
  taGuiWidget* w = AddWidget_impl(taidata->GetRep(), nm, "FloatField", layout, attributes, 
				  Variant((void*)dvar), _nilString, taidata);
  return (bool)w;
}

bool taGuiDialog::AddStringField(String* dvar, const String& nm, const String& parent,
			      const String& layout, const String& attributes) {
  if(!dvar) return false;
  if(TestError(parent.empty(), "AddStringField", "a parent widget is required"))
    return false;
  taGuiWidget* par = FindWidget(parent, true);
  if(!par) return false;
  taiField* taidata = new taiField(&TA_int, data_host, NULL, par->widget);
  taGuiWidget* w = AddWidget_impl(taidata->GetRep(), nm, "StringField", layout, attributes, 
				  Variant((void*)dvar), _nilString, taidata);
  return (bool)w;
}

bool taGuiDialog::AddBoolCheckbox(bool* dvar, const String& nm, const String& parent,
			      const String& layout, const String& attributes) {
  if(!dvar) return false;
  if(TestError(parent.empty(), "AddBoolCheckbox", "a parent widget is required"))
    return false;
  taGuiWidget* par = FindWidget(parent, true);
  if(!par) return false;
  taiToggle* taidata = new taiToggle(&TA_bool, data_host, NULL, par->widget);
  taGuiWidget* w = AddWidget_impl(taidata->GetRep(), nm, "BoolCheckbox", layout, attributes, 
				  Variant((void*)dvar), _nilString, taidata);
  return (bool)w;
}

bool taGuiDialog::AddObjectPtr(taBaseRef* obj, TypeDef* td, const String& nm, const String& parent,
				       const String& layout, const String& attributes) {
  if(!obj) return false;
  if(TestError(parent.empty(), "AddObjectPtr", "a parent widget is required"))
    return false;
  taGuiWidget* par = FindWidget(parent, true);
  if(!par) return false;
  String atts = attributes;
  if(td)
    atts = String("type=") + td->name + "; " + atts;
  taiTokenPtrButton* taidata = new taiTokenPtrButton(td, data_host, NULL, par->widget);
  taGuiWidget* w = AddWidget_impl(taidata->GetRep(), nm, "ObjectPtr",
				  layout, atts,  Variant((void*)obj),
				  _nilString, taidata);
  return (bool)w;
}

bool taGuiDialog::AddProgVar(ProgVar& pvar, const String& nm, const String& parent,
			     const String& layout, const String& attributes) {
  if(TestError(pvar.HasVarFlag(ProgVar::LOCAL_VAR),
	       "AddProgVar", "prog var is not configured properly -- be sure you used this.vars.varname to pass it as an arg"))
    return false;
  if(TestError(parent.empty(), "AddProgVar", "a parent widget is required"))
    return false;
  taGuiWidget* par = FindWidget(parent, true);
  if(!par) return false;
  bool rval = false;
  switch(pvar.var_type) {
  case ProgVar::T_Int: {
    rval = AddIntField(&(pvar.int_val), nm, parent, layout, attributes);
    break;
  }
  case ProgVar::T_Real: {
    rval = AddDoubleField(&(pvar.real_val), nm, parent, layout, attributes);
    break;
  }
  case ProgVar::T_String: {
    rval = AddStringField(&(pvar.string_val), nm, parent, layout, attributes);
    break;
  }
  case ProgVar::T_Bool: {
    rval = AddBoolCheckbox(&(pvar.bool_val), nm, parent, layout, attributes);
    break;
  }
  case ProgVar::T_Object: {
    rval = AddObjectPtr(&(pvar.object_val), pvar.object_type, nm, parent, layout, attributes);
    break;
  }
  }
  return rval;
}

bool taGuiDialog::AddDataTable(DataTable* dt, const String& nm, const String& parent,
				       const String& layout, const String& attributes) {
  if(!dt) return false;
  iDataTableEditor* widg;
  if(parent.nonempty()) {
    taGuiWidget* par = FindWidget(parent, true);
    if(!par) return false;
    widg = new iDataTableEditor(par->widget);
  }
  else
    widg = new iDataTableEditor();
  widg->setDataTable(dt);
  taGuiWidget* w = AddWidget_impl(widg, nm, "DataTable", layout, attributes, _nilVariant);
  return (bool)w;
}

bool taGuiDialog::AddVBoxLayout(const String& nm, const String& parent,
				const String& widget, const String& attributes) {
  taGuiLayout* par = NULL;
  if(parent.nonempty()) {
    par = FindLayout(parent, true);
    if(!par) return false;
  }
  taGuiWidget* widg = NULL;
  if(widget.nonempty()) {
    widg = FindWidget(widget, true);
    if(!widg) return false;
  }
  QVBoxLayout* lay;
  if(widg)
    lay = new QVBoxLayout(widg->widget);
  else
    lay = new QVBoxLayout();
  if(par)
    par->layout->addLayout(lay);
  taGuiLayout* lo = (taGuiLayout*)layouts.New(1);
  lo->SetName(nm);
  lo->layout = lay;
  // todo: attributes
  return true;
}

bool taGuiDialog::AddHBoxLayout(const String& nm, const String& parent,
				const String& widget, const String& attributes) {
  taGuiLayout* par = NULL;
  if(parent.nonempty()) {
    par = FindLayout(parent, true);
    if(!par) return false;
  }
  taGuiWidget* widg = NULL;
  if(widget.nonempty()) {
    widg = FindWidget(widget, true);
    if(!widg) return false;
  }
  QHBoxLayout* lay;
  if(widg)
    lay = new QHBoxLayout(widg->widget);
  else
    lay = new QHBoxLayout();
  if(par)
    par->layout->addLayout(lay);
  taGuiLayout* lo = (taGuiLayout*)layouts.New(1);
  lo->SetName(nm);
  lo->layout = lay;
  // todo: attributes
  return true;
}

bool taGuiDialog::AddSpace(int sp_size, const String& layout) {
  if(layout.empty()) return false;
  taGuiLayout* lay = FindLayout(layout, true);
  if(!lay) return false;
  lay->layout->addSpacing(sp_size);
  return true;
}

bool taGuiDialog::AddStretch(const String& layout) {
  if(layout.empty()) return false;
  taGuiLayout* lay = FindLayout(layout, true);
  if(!lay) return false;
  lay->layout->addStretch();
  return true;
}
