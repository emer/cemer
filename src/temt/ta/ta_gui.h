// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

// ta_project.h

#ifndef TAGUI_H
#define TAGUI_H

#include "ta_qtdata_def.h"
#include "ta_qtdialog.h"
#include "ta_program.h"

class taGuiWidgetHelper;	// #IGNORE
class taGuiActionHelper;	// #IGNORE
class taGuiDialog;

class TA_API taGuiWidget : public taNBase {
  // ##CAT_Gui manages a qt widget for the taGui system
INHERITED(taNBase)
public:
#ifndef __MAKETA__
  QPointer<QWidget>	widget;	// the widget
  taiData*		tai_data;// gui data object for widget
#endif
  String		widget_type; // what type of widget is this?  types defined by creation function
  String		attributes; // attributes that were set on this widget
  String		action_url; // url to emit as an action if this is a button or other action item
  Variant		data;	// associated data -- can also use UserData interface to add more data

  virtual void		UrlAction();	// execute the action_url
  virtual void 		Connect_UrlAction(QObject* src_obj, const char* src_signal);
  // connects source object signal to the UrlAction via helper
  virtual void		FixUrl(const String& url_tag, const String& path);
  // replace starting tag in url with given path

  virtual void		GetImage();
  // get gui image, for tai_data controls
  virtual void		GetValue();
  // get value from tai_data controls

  TA_SIMPLE_BASEFUNS(taGuiWidget);
protected:
  taGuiWidgetHelper*	m_helper;

//   override void		UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy();
};

class TA_API taGuiWidgetHelper : public QObject {
 // #IGNORE this is a helper QObject that handles signals/slots
Q_OBJECT
friend class taGuiWidget;
public:

public slots:
  void		UrlAction() { widget->UrlAction(); }

protected:
  taGuiWidget* 	widget;

  taGuiWidgetHelper(taGuiWidget* wid) { widget = wid; }
};

class TA_API taGuiWidget_List : public taList<taGuiWidget> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Gui list of ta gui widgets
INHERITED(taList<taGuiWidget>)
public:
  virtual void	GetImage();	// get image for all items in the list
  virtual void	GetValue();	// get value for all items in the list

  virtual void		FixAllUrl(const String& url_tag, const String& path);
  // replace starting tag in url with given path

  TA_SIMPLE_BASEFUNS(taGuiWidget_List);
private:
  void	Initialize();
  void	Destroy() { Reset(); CutLinks(); }
};

class TA_API taGuiLayout : public taNBase {
  // ##CAT_Gui manages a qt layout for the taGui system
INHERITED(taNBase) // declares 'inherited' keyword for safer base-class references
public:
#ifndef __MAKETA__
  QPointer<QBoxLayout>	layout;	// the layout
#endif
  
  TA_SIMPLE_BASEFUNS(taGuiLayout);
// protected:
//   override void		UpdateAfterEdit_impl();
private:
  void	Initialize()	{ };
  void	Destroy() 	{ };
};

class TA_API taGuiLayout_List : public taList<taGuiLayout> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Gui list of ta gui layouts
INHERITED(taList<taGuiLayout>)
public:
  TA_SIMPLE_BASEFUNS(taGuiLayout_List);
protected:
  
private:
  void	Initialize();
  void	Destroy() { Reset(); CutLinks(); }
};


class TA_API taGuiAction : public taNBase {
  // ##CAT_Gui manages a qt action for the taGui system
INHERITED(taNBase) 
public:
#ifndef __MAKETA__
  QPointer<QAction>	action;	// the action
#endif
  String		attributes; // attributes that were set on this widget
  String		action_url; // url to emit as an action if this is a button or other action item
  String		toolbar; // toolbar that contains action
  String		menu; // menu that contains action

  virtual void		UrlAction();	// execute the action_url
  virtual void 		Connect_UrlAction(QObject* src_obj, const char* src_signal);
  // connects source object signal to the UrlAction via helper
  virtual void		FixUrl(const String& url_tag, const String& path);
  // replace starting tag in url with given path

  TA_SIMPLE_BASEFUNS(taGuiAction);
protected:
  taGuiActionHelper*	m_helper;

//   override void		UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy();
};

class TA_API taGuiActionHelper : public QObject {
 // #IGNORE this is a helper QObject that handles signals/slots
Q_OBJECT
friend class taGuiAction;
public:

public slots:
  void		UrlAction() { action->UrlAction(); }

protected:
  taGuiAction* 	action;

  taGuiActionHelper(taGuiAction* wid) { action = wid; }
};

class TA_API taGuiAction_List : public taList<taGuiAction> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Gui list of ta gui actions
INHERITED(taList<taGuiAction>)
public:
  virtual void		FixAllUrl(const String& url_tag, const String& path);
  // replace starting tag in url with given path

  TA_SIMPLE_BASEFUNS(taGuiAction_List);
private:
  void	Initialize();
  void	Destroy() { Reset(); CutLinks(); }
};



class taGuiDataHost;

class TA_API taGuiDialog : public taNBase {
  // ##CAT_Gui a gui dialog object -- makes a gui dialog that can be constructed with simple method calls to display a variety of gui elements (widgets) -- useful for building custom interface elements such as wizards
INHERITED(taNBase) // declares 'inherited' keyword for safer base-class references
public:
  taGuiDataHost*	data_host; // #IGNORE the dialog window
  String		prompt;	   // main text at top of dialog window
  String		win_title; // window title
  int			width;	   // min width of dialog
  int			height;	   // min height of dialog
  taGuiWidget_List	widgets; // #HIDDEN #NO_SAVE the widgets
  taGuiLayout_List	layouts; // #HIDDEN #NO_SAVE the layouts
  taGuiAction_List	actions; // #HIDDEN #NO_SAVE the actions

  virtual void	Reset();
  // #CAT_Dialog reset contents to start over
  virtual int	PostDialog(bool modal = false);
  // #CAT_Dialog post the dialog -- if modal, then this call does not return until the dialog is cancled -- otherwise it returns immediately and the dialog persists
  virtual void	Cancel();
  // #CAT_Dialog Cancel the dialog -- close without applying changes
  virtual void	Ok();
  // #CAT_Dialog OK the dialog -- close with applying changes
  virtual void	Apply();
  // #CAT_Dialog Apply any outstanding changes in the dialog
  virtual void	Revert();
  // #CAT_Dialog Revert any outstanding changes in the dialog, and update to reflect current contents
  virtual void	SetSize(int wd, int ht) { width = wd; height = ht; }
  // #CAT_Dialog Set the minimum size of the dialog (also just do with width, height members)

  virtual bool	AddWidget(const String& nm, const String& parent = _nilString,
			  const String& layout = _nilString,
			  const String& attributes = _nilString);
  // #CAT_Widget add a plain widget -- attributes are: tooltip=, min/max_width/height= ,font=, font_size=, bold=, italic=
  virtual bool	AddFrame(const String& nm, const String& parent = _nilString,
			 const String& layout = _nilString,
			 const String& attributes = _nilString);
  // #CAT_Widget add a frame widget -- attributes are: shape=[NoFrame,Box,Panel,WinPanel,HLine,VLine,StyledPanel] shadow=[Plain,Raised,Sunken] line_width= mid_line_width=
  virtual bool	AddLabel(const String& nm, const String& parent = _nilString,
			 const String& layout = _nilString,
			 const String& attributes = _nilString);
  // #CAT_Widget add a label widget -- attributes are:
  virtual bool	AddPushButton(const String& nm, const String& parent = _nilString,
			      const String& layout = _nilString,
			      const String& url = _nilString,
			      const String& attributes = _nilString);
  // #CAT_Widget add a pushbutton widget -- attributes are: label=
  virtual bool	AddToolButton(const String& nm, const String& parent = _nilString,
			      const String& layout = _nilString,
			      const String& url = _nilString,
			      const String& attributes = _nilString);
  // #CAT_Widget add a toolbutton widget -- attributes are: label=
  virtual bool	AddToolBar(const String& nm, const String& parent = _nilString,
			   const String& layout = _nilString,
			   const String& attributes = _nilString);
  // #CAT_Widget add a toolbar widget -- can then add actions to it -- attributes are: 
  virtual bool	AddSeparator(const String& toolbar, const String& menu = _nilString);
  // #CAT_Widget add a separator to the toolbar or menu if specified
  virtual bool	AddMenu(const String& nm, const String& toolbar,
			const String& menu = _nilString,
			const String& attributes = _nilString);
  // #CAT_Widget add a menu to a toolbar or an existing menu on that toolbar -- can then add actions to this menu -- attributes are: label=
  virtual bool	AddAction(const String& nm, const String& toolbar,
			  const String& menu = _nilString,
			  const String& url = _nilString,
			  const String& attributes = _nilString);
  // #CAT_Widget add an action to a toolbar or menu (if menu is set) -- attributes are: label= tooltip= checkable= shortcut=

  virtual bool	AddIntField(int* iptr, const String& nm,
			    const String& parent, const String& layout = _nilString,
			    const String& attributes = _nilString);
  // #CAT_XpertWidget add an integer value edit field -- attributes are:
  virtual bool	AddDoubleField(double* dptr, const String& nm,
			    const String& parent, const String& layout = _nilString,
			    const String& attributes = _nilString);
  // #CAT_XpertWidget add a double-precision floating point value edit field -- attributes are:
  virtual bool	AddFloatField(float* fptr, const String& nm,
			    const String& parent, const String& layout = _nilString,
			    const String& attributes = _nilString);
  // #CAT_XpertWidget add a floating point value edit field -- attributes are:
  virtual bool	AddStringField(String* sptr, const String& nm,
			    const String& parent, const String& layout = _nilString,
			    const String& attributes = _nilString);
  // #CAT_XpertWidget add a string value edit field -- attributes are:
  virtual bool	AddBoolCheckbox(bool* bptr, const String& nm,
			    const String& parent, const String& layout = _nilString,
			    const String& attributes = _nilString);
  // #CAT_XpertWidget add a bool checkbox -- attributes are:
  virtual bool	AddObjectPtr(taBaseRef* obj, TypeDef* td, const String& nm,
			    const String& parent, const String& layout = _nilString,
			    const String& attributes = _nilString);
  // #CAT_XpertWidget add an object (taBase) pointer button -- attributes are:
  virtual bool	AddHardEnum(int* iptr, TypeDef* enum_td, const String& nm,
			    const String& parent, const String& layout = _nilString,
			    const String& attributes = _nilString);
  // #CAT_XpertWidget add a hard enum combo-box chooser -- attributes are:
  virtual bool	AddDynEnum(DynEnum* deptr, const String& nm,
			    const String& parent, const String& layout = _nilString,
			    const String& attributes = _nilString);
  // #CAT_XpertWidget add a dynamic enum item -- attributes are:
  virtual bool	AddProgVar(ProgVar& pvar, const String& nm,
			   const String& parent, const String& layout = _nilString,
			   const String& attributes = _nilString);
  // #CAT_Widget add a program variable edit field -- IMPORTANT: must use this.vars.progvarname in program code, not the direct name of the program variable (which is not actually the full progvar) -- attributes are:
  virtual bool	AddDataTable(DataTable* dt, const String& nm,
			   const String& parent, const String& layout = _nilString,
			   const String& attributes = _nilString);
  // #CAT_Widget add a DataTable editor for given data table -- attributes are:

  virtual bool	AddVBoxLayout(const String& nm, const String& parent = _nilString,
			      const String& widget = _nilString,
			      const String& attributes = _nilString);
  // #CAT_Layout add a vertical box layout to given widget (optional), and parent *layout* -- attributes are: spacing=x, margin=x
  virtual bool	AddHBoxLayout(const String& nm, const String& parent = _nilString,
			      const String& widget = _nilString,
			      const String& attributes = _nilString);
  // #CAT_Layout add a horizontal box layout to given widget (optional), and parent *layout* -- attributes are: spacing=x, margin=x
  virtual bool	AddSpace(int sp_size, const String& layout);
  // add space to given layout
  virtual bool	AddStretch(const String& layout);
  // add flexible stretch to given layout

  static String	GetAttribute(const String& key, const String& attributes);
  // get attribute value for given key, format: key=value;  (semicolon terminates)

  virtual taGuiWidget*	FindWidget(const String& nm, bool err_msg = true);
  // find widget -- issue error message if not found if err_msg
  virtual taGuiLayout*	FindLayout(const String& nm, bool err_msg = true);
  // find layout -- issue error message if not found if err_msg
  virtual taGuiAction*	FindAction(const String& nm, bool err_msg = true);
  // find action -- issue error message if not found if err_msg

  virtual taGuiWidget*	AddWidget_impl(QWidget* widg, const String& nm,
			       const String& typ, const String& layout = _nilString,
			       const String& attributes = _nilString,
			       Variant data = _nilVariant, const String& url = _nilString,
			       taiData* taidata = NULL);
  // #IGNORE add a widget, implementation -- must have already parsed parent and used that for creating widget
  virtual taGuiAction*	AddAction_impl(QAction* act, const String& nm,
				       const String& toolbar = _nilString,
				       const String& menu = _nilString,
				       const String& attributes = _nilString,
				       const String& url = _nilString);
  // #IGNORE add a action, implementation -- must have already parsed parent and used that for creating action
  
  virtual void	GetImage();	// #IGNORE get image for all widgets that need it
  virtual void	GetValue();	// #IGNORE get value for all widgets that need it

  virtual void		FixAllUrl(const String& url_tag, const String& path);
  // replace starting tag in url with given path for all actions and widgets
  
  TA_SIMPLE_BASEFUNS(taGuiDialog);
// protected:
//   override void		UpdateAfterEdit_impl();
private:
  void	Initialize();
  void	Destroy();
};

class TA_API taGuiDataHost : public taiDataHostBase, virtual public IDataHost 
{ // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(taiDataHostBase)
  Q_OBJECT
friend class iHostDialog;
public:
  taGuiDialog* 	gui_owner;

  taGuiDataHost(taGuiDialog* own, bool read_only_ = false,
		bool modal_ = false, QObject* parent = 0);
  virtual ~taGuiDataHost();

  override void	Constr_Body();
  override void GetImage(bool force);
  override void	Ok_impl();
  
public: // IDataLinkClient i/f -- note: only registered though for taiEDH and later
//   void		DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);

public: // ITypedObject i/f (common to IDLC and IDH)
  override void*	This() {return this;}
  override TypeDef* 	GetTypeDef() const {return &TA_taGuiDataHost;}

public: // IDataHost i/f
  override const iColor	 colorOfCurRow() const { return bgColor(); } 
  override taMisc::ShowMembs	show() const {return taMisc::show_gui; }
  override bool  	HasChanged() {return modified;}	
  override bool		isConstructed() {int s = state & STATE_MASK;
    return ((s >= CONSTRUCTED) && (s < ZOMBIE));}
  override bool		isModal() {return modal;}
  override bool		isReadOnly() {return read_only;} //
  override void*	Root() const {return gui_owner;} // root of the object
  override taBase*	Base() const {return gui_owner;}
  override TypeDef*	GetRootTypeDef() const {return &TA_taGuiDialog;} // TypeDef on the root, for casting
  override void		GetImage()	{ GetImage(true); }
  override void		GetValue();
public slots:
  void  	Changed() {inherited::Changed();}
  void		Apply_Async() {inherited::Apply_Async(); }
};

#endif
