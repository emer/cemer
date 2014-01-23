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

#ifndef taGuiEditor_h
#define taGuiEditor_h 1

// parent includes:
#include <taiEditor>
#include <IWidgetHost>

// member includes:

// declare all other types mentioned but not required to include:
class taGuiDialog;
taTypeDef_Of(taGuiDialog);

taTypeDef_Of(taGuiEditor);

class TA_API taGuiEditor : public taiEditor, virtual public IWidgetHost
{ // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(taiEditor)
  Q_OBJECT
friend class iDialogEditor;
public:
  taGuiDialog*  gui_owner;

  taGuiEditor(taGuiDialog* own, bool read_only_ = false,
                bool modal_ = false, QObject* parent = 0);
  virtual ~taGuiEditor();

  void Constr_Body() CPP11_OVERRIDE;
  void GetImage(bool force) CPP11_OVERRIDE;
  void Ok_impl() CPP11_OVERRIDE;

public: // ISigLinkClient i/f -- note: only registered though for taiEDH and later
//   void               SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2);

public: // ITypedObject i/f (common to IDLC and IDH)
  void*        This() CPP11_OVERRIDE {return this;}
  TypeDef*     GetTypeDef() const CPP11_OVERRIDE {return &TA_taGuiEditor;}

public: // IWidgetHost i/f
  const iColor  colorOfCurRow() const CPP11_OVERRIDE { return bgColor(); }
  TypeItem::ShowMembs  show() const CPP11_OVERRIDE;
  bool         HasChanged() CPP11_OVERRIDE {return modified;}
  bool         isConstructed() CPP11_OVERRIDE {int s = state & STATE_MASK;
    return ((s >= CONSTRUCTED) && (s < ZOMBIE));}
  bool         isModal() CPP11_OVERRIDE {return modal;}
  bool         isReadOnly() CPP11_OVERRIDE {return read_only;} //
  void*        Root() const CPP11_OVERRIDE {return gui_owner;} // root of the object
  taBase*      Base() const CPP11_OVERRIDE;
  TypeDef*     GetRootTypeDef() const CPP11_OVERRIDE {return &TA_taGuiDialog;} // TypeDef on the root, for casting
  void         GetImage()      CPP11_OVERRIDE { GetImage(true); }
  void         GetValue() CPP11_OVERRIDE;
public slots:
  void          Changed() {inherited::Changed();}
  void          Apply_Async() {inherited::Apply_Async(); }
};

#endif // taGuiEditor_h
