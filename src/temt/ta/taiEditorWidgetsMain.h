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

#ifndef taiEditorWidgetsMain_h
#define taiEditorWidgetsMain_h 1

// parent includes:
#include <taiEditorWidgets>
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:
#ifndef __MAKETA__
#include <QPointer>
#include <QWidget>
#else
class QWidget; //
class QPointer; //
#endif
class iFormLayout; //
class QVBoxLayout; //
class iEditGrid;   //
class QSplitter;   //
class iLabel;      //

TypeDef_Of(taiEditorWidgetsMain);

class TA_API taiEditorWidgetsMain: public taiEditorWidgets {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS specific instantiation of the gui appearance of the edit, using an iFormLayout
INHERITED(taiEditorWidgets)
  Q_OBJECT
friend class iDialogEditor;
public:
  static iLabel* MakeInitEditLabel(const String& name, QWidget* par, int ctrl_size,
    const String& desc, taiWidget* buddy = NULL,
    QObject* ctx_obj = NULL, const char* ctx_slot = NULL, int row = 0);
    // helper used by AddName, and in ProgEditor (and elsewhere, ex inlines)

  int           cur_row;        // #IGNORE marks row num of ctrl being added to grid or matrix (for groups) -- child can read this to determine its background color, by calling colorOfRow()
  int           dat_cnt; // count of actual data controls added -- used to mark flat data index of control for right click menu


  QSplitter*    splBody;        // if not null when body created, then body is put into this splitter (used for list/group hosts)
#ifndef __MAKETA__
  QPointer<QWidget> first_tab_foc;      // first tab focus widget
#endif

  QVBoxLayout*  body_vlay;      // vertical layout in body -- this is created with body
  iFormLayout*  layBody;

  override int          curRow() const {return cur_row;}
  override QWidget*     firstTabFocusWidget() { return first_tab_foc; }

  taiEditorWidgetsMain(TypeDef* typ_ = NULL, bool read_only_ = false, bool modal_ = false, QObject* parent = 0);
  ~taiEditorWidgetsMain();

public: // ITypedObject i/f (common to IDLC and IDH)
  void*         This() {return this;} // override
  TypeDef*      GetTypeDef() const {return &TA_taiEditorWidgetsMain;} // override

public: // ISigLinkClient i/f -- note: only registered though for taiEDH and later
//  void                SigLinkDestroying(taSigLink* dl);
//  void                SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2);

protected:

  virtual void  SetMultiSize(int rows, int cols) {}
  int           AddSectionLabel(int row, QWidget* wid, const String& desc);
  // add a widget, usually a label or checkbox, that will span both columns (no data)
  int           AddNameData(int row, const String& name, const String& desc,
                            QWidget* data_wid, taiWidget* data_dat = NULL,
                            MemberDef* md = NULL, bool fill_hor = false);
  // add a label item in first column, data item in second column; row<0 means "next row"; returns row
  int           AddData(int row, QWidget* data_wid, bool fill_hor = false);
  // add a data item with no label (spanning whole row); row<0 means "next row"; returns row
  void          AddMultiRowName(iEditGrid* multi_body, int row, const String& name, const String& desc); // adds a label item in first column of multi data area -- we define here for source code mgt, since AddName etc. are similar
  void          AddMultiColName(iEditGrid* multi_body, int col, const String& name, const String& desc); // adds descriptive column text to top of a multi data item
  void          AddMultiData(iEditGrid* multi_body, int row, int col, QWidget* data); // add a data item in the multi-data area -- expands if necessary
  override void Constr_Box();
  virtual void  Constr_Body_impl();
  override void Constr_Final();
  override void ClearBody_impl();

protected:
  override void InitGuiFields(bool virt = true); // NULL the gui fields -- virt used for ctor
};

class TA_API taiDialogEditor_List : public taPtrList<taiEditorWidgetsMain> {
  // #IGNORE list of DataHosts that have been dialoged
protected:
  void  El_Done_(void* it)      { delete (taiEditorWidgetsMain*)it; }

public:
  ~taiDialogEditor_List()            { Reset(); }
};

#endif // taiEditorWidgetsMain_h
