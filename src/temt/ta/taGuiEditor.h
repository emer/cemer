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

  void Constr_Body() override;
  void GetImage(bool force) override;
  void Ok_impl() override;

public: // ISigLinkClient i/f -- note: only registered though for taiEDH and later
//   void               SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2);

public: // ITypedObject i/f (common to IDLC and IDH)
  void*        This() override {return this;}
  TypeDef*     GetTypeDef() const override {return &TA_taGuiEditor;}

public: // IWidgetHost i/f
  const iColor  colorOfCurRow() const override { return bgColor(); }
  TypeItem::ShowMembs  show() const override;
  bool         HasChanged() override {return modified;}
  bool         isConstructed() override {int s = state & STATE_MASK;
    return ((s >= CONSTRUCTED) && (s < ZOMBIE));}
  bool         isModal() override {return modal;}
  bool         isReadOnly() override {return read_only;} //
  void*        Root() const override {return gui_owner;} // root of the object
  taBase*      Base() const override;
  TypeDef*     GetRootTypeDef() const override {return &TA_taGuiDialog;} // TypeDef on the root, for casting
  void         GetImage()      override { GetImage(true); }
  void         GetValue() override;
public slots:
  void          Changed() {inherited::Changed();}
  void          Apply_Async() {inherited::Apply_Async(); }
};

#endif // taGuiEditor_h
