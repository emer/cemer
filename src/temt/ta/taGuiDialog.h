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

#ifndef taGuiDialog_h
#define taGuiDialog_h 1

// parent includes:
#include <taNBase>

// smartptr, ref includes
#include <taSmartRefT>

// member includes:
#include <taGuiWidget_List>
#include <taGuiLayout_List>
#include <taGuiAction_List>

// declare all other types mentioned but not required to include:
class DynEnum; // 
class ProgVar; // 
class DataTable; // 
class taGuiWidget; // 
class taGuiLayout; // 
class taGuiAction; // 
class taGuiEditor; //
class QAction; //


TypeDef_Of(taGuiDialog);

class TA_API taGuiDialog : public taNBase {
  // ##CAT_Gui a gui dialog object -- makes a gui dialog that can be constructed with simple method calls to display a variety of gui elements (widgets) -- useful for building custom interface elements such as wizards
INHERITED(taNBase) // declares 'inherited' keyword for safer base-class references
public:
  taGuiEditor*        data_host; // #IGNORE the dialog window
  String                prompt;    // main text at top of dialog window
  String                win_title; // window title
  int                   width;     // min width of dialog
  int                   height;    // min height of dialog
  taGuiWidget_List      widgets; // #HIDDEN #NO_SAVE the widgets
  taGuiLayout_List      layouts; // #HIDDEN #NO_SAVE the layouts
  taGuiAction_List      actions; // #HIDDEN #NO_SAVE the actions

  virtual void  Reset();
  // #CAT_Dialog reset contents to start over
  virtual int   PostDialog(bool modal = false);
  // #CAT_Dialog post the dialog -- if modal, then this call does not return until the dialog is cancled -- otherwise it returns immediately and the dialog persists
  virtual void  Cancel();
  // #CAT_Dialog Cancel the dialog -- close without applying changes
  virtual void  Ok();
  // #CAT_Dialog OK the dialog -- close with applying changes
  virtual void  Apply();
  // #CAT_Dialog Apply any outstanding changes in the dialog
  virtual void  Revert();
  // #CAT_Dialog Revert any outstanding changes in the dialog, and update to reflect current contents
  virtual void  SetSize(int wd, int ht) { width = wd; height = ht; }
  // #CAT_Dialog Set the minimum size of the dialog (also just do with width, height members)

  virtual bool  AddWidget(const String& nm, const String& parent = _nilString,
                          const String& layout = _nilString,
                          const String& attributes = _nilString);
  // #CAT_Widget add a plain widget -- attributes are: tooltip=, min/max_width/height= ,font=, font_size=, bold=, italic=
  virtual bool  AddFrame(const String& nm, const String& parent = _nilString,
                         const String& layout = _nilString,
                         const String& attributes = _nilString);
  // #CAT_Widget add a frame widget -- attributes are: shape=[NoFrame,Box,Panel,WinPanel,HLine,VLine,StyledPanel] shadow=[Plain,Raised,Sunken] line_width= mid_line_width=
  virtual bool  AddLabel(const String& nm, const String& parent = _nilString,
                         const String& layout = _nilString,
                         const String& attributes = _nilString);
  // #CAT_Widget add a label widget -- attributes are: label=, wrap=on
  virtual bool  AddPushButton(const String& nm, const String& parent = _nilString,
                              const String& layout = _nilString,
                              const String& url = _nilString,
                              const String& attributes = _nilString);
  // #CAT_Widget add a pushbutton widget -- attributes are: label=
  virtual bool  AddToolButton(const String& nm, const String& parent = _nilString,
                              const String& layout = _nilString,
                              const String& url = _nilString,
                              const String& attributes = _nilString);
  // #CAT_Widget add a toolbutton widget -- attributes are: label=
  virtual bool  AddToolBar(const String& nm, const String& parent = _nilString,
                           const String& layout = _nilString,
                           const String& attributes = _nilString);
  // #CAT_Widget add a toolbar widget -- can then add actions to it -- attributes are:
  virtual bool  AddSeparator(const String& toolbar, const String& menu = _nilString);
  // #CAT_Widget add a separator to the toolbar or menu if specified
  virtual bool  AddMenu(const String& nm, const String& toolbar,
                        const String& menu = _nilString,
                        const String& attributes = _nilString);
  // #CAT_Widget add a menu to a toolbar or an existing menu on that toolbar -- can then add actions to this menu -- attributes are: label=
  virtual bool  AddAction(const String& nm, const String& toolbar,
                          const String& menu = _nilString,
                          const String& url = _nilString,
                          const String& attributes = _nilString);
  // #CAT_Widget add an action to a toolbar or menu (if menu is set) -- attributes are: label= tooltip= checkable= shortcut=

  virtual bool  AddIntField(int* iptr, const String& nm,
                            const String& parent, const String& layout = _nilString,
                            const String& attributes = _nilString);
  // #EXPERT #CAT_Widget add an integer value edit field -- attributes are:
  virtual bool  AddDoubleField(double* dptr, const String& nm,
                            const String& parent, const String& layout = _nilString,
                            const String& attributes = _nilString);
  // #EXPERT #CAT_Widget add a double-precision floating point value edit field -- attributes are:
  virtual bool  AddFloatField(float* fptr, const String& nm,
                            const String& parent, const String& layout = _nilString,
                            const String& attributes = _nilString);
  // #EXPERT #CAT_Widget add a floating point value edit field -- attributes are:
  virtual bool  AddStringField(String* sptr, const String& nm,
                            const String& parent, const String& layout = _nilString,
                            const String& attributes = _nilString);
  // #EXPERT #CAT_Widget add a string value edit field -- attributes are:
  virtual bool  AddBoolCheckbox(bool* bptr, const String& nm,
                            const String& parent, const String& layout = _nilString,
                            const String& attributes = _nilString);
  // #EXPERT #CAT_Widget add a bool checkbox -- attributes are:
  virtual bool  AddObjectPtr(taBaseRef* obj, TypeDef* td, const String& nm,
                            const String& parent, const String& layout = _nilString,
                            const String& attributes = _nilString);
  // #EXPERT #CAT_Widget #IGNORE add an object (taBase) pointer button -- attributes are:
  virtual bool  AddHardEnum(int* iptr, TypeDef* enum_td, const String& nm,
                            const String& parent, const String& layout = _nilString,
                            const String& attributes = _nilString);
  // #EXPERT #CAT_Widget add a hard enum combo-box chooser -- attributes are:
  virtual bool  AddDynEnum(DynEnum* deptr, const String& nm,
                            const String& parent, const String& layout = _nilString,
                            const String& attributes = _nilString);
  // #EXPERT #CAT_Widget add a dynamic enum item -- attributes are:
  virtual bool  AddProgVar(ProgVar& pvar, const String& nm,
                           const String& parent, const String& layout = _nilString,
                           const String& attributes = _nilString);
  // #CAT_Widget add a program variable edit field -- IMPORTANT: must use this.vars.progvarname in program code, not the direct name of the program variable (which is not actually the full progvar) -- attributes are:
  virtual bool  AddDataTable(DataTable* dt, const String& nm,
                           const String& parent, const String& layout = _nilString,
                           const String& attributes = _nilString);
  // #CAT_Widget add a DataTable editor for given data table -- attributes are:

  virtual bool  AddVBoxLayout(const String& nm, const String& parent = _nilString,
                              const String& widget = _nilString,
                              const String& attributes = _nilString);
  // #CAT_Layout add a vertical box layout to given widget (optional), and parent *layout* -- attributes are: spacing=x, margin=x
  virtual bool  AddHBoxLayout(const String& nm, const String& parent = _nilString,
                              const String& widget = _nilString,
                              const String& attributes = _nilString);
  // #CAT_Layout add a horizontal box layout to given widget (optional), and parent *layout* -- attributes are: spacing=x, margin=x
  virtual bool  AddSpace(int sp_size, const String& layout);
  // add space to given layout
  virtual bool  AddStretch(const String& layout);
  // add flexible stretch to given layout

  static String GetAttribute(const String& key, const String& attributes);
  // get attribute value for given key, format: key=value;  (semicolon terminates)

  virtual taGuiWidget*  FindWidget(const String& nm, bool err_msg = true);
  // find widget -- issue error message if not found if err_msg
  virtual taGuiLayout*  FindLayout(const String& nm, bool err_msg = true);
  // find layout -- issue error message if not found if err_msg
  virtual taGuiAction*  FindAction(const String& nm, bool err_msg = true);
  // find action -- issue error message if not found if err_msg

  virtual taGuiWidget*  AddWidget_impl(QWidget* widg, const String& nm,
                               const String& typ, const String& layout = _nilString,
                               const String& attributes = _nilString,
                               Variant data = _nilVariant, const String& url = _nilString,
                               taiData* taidata = NULL);
  // #IGNORE add a widget, implementation -- must have already parsed parent and used that for creating widget
  virtual taGuiAction*  AddAction_impl(QAction* act, const String& nm,
                                       const String& toolbar = _nilString,
                                       const String& menu = _nilString,
                                       const String& attributes = _nilString,
                                       const String& url = _nilString);
  // #IGNORE add a action, implementation -- must have already parsed parent and used that for creating action

  virtual void  GetImage();     // #IGNORE get image for all widgets that need it
  virtual void  GetValue();     // #IGNORE get value for all widgets that need it

  virtual void          FixAllUrl(const String& url_tag, const String& path);
  // replace starting tag in url with given path for all actions and widgets

  TA_SIMPLE_BASEFUNS(taGuiDialog);
// protected:
//   override void              UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy();
};

#endif // taGuiDialog_h
