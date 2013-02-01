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
TypeDef_Of(taGuiDialog);

TypeDef_Of(taGuiEditor);

class TA_API taGuiEditor : public taiEditor, virtual public IWidgetHost
{ // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(taiEditor)
  Q_OBJECT
friend class iHostDialog;
public:
  taGuiDialog*  gui_owner;

  taGuiEditor(taGuiDialog* own, bool read_only_ = false,
                bool modal_ = false, QObject* parent = 0);
  virtual ~taGuiEditor();

  override void Constr_Body();
  override void GetImage(bool force);
  override void Ok_impl();

public: // ISigLinkClient i/f -- note: only registered though for taiEDH and later
//   void               SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2);

public: // ITypedObject i/f (common to IDLC and IDH)
  override void*        This() {return this;}
  override TypeDef*     GetTypeDef() const {return &TA_taGuiEditor;}

public: // IWidgetHost i/f
  override const iColor  colorOfCurRow() const { return bgColor(); }
  override TypeItem::ShowMembs  show() const;
  override bool         HasChanged() {return modified;}
  override bool         isConstructed() {int s = state & STATE_MASK;
    return ((s >= CONSTRUCTED) && (s < ZOMBIE));}
  override bool         isModal() {return modal;}
  override bool         isReadOnly() {return read_only;} //
  override void*        Root() const {return gui_owner;} // root of the object
  override taBase*      Base() const;
  override TypeDef*     GetRootTypeDef() const {return &TA_taGuiDialog;} // TypeDef on the root, for casting
  override void         GetImage()      { GetImage(true); }
  override void         GetValue();
public slots:
  void          Changed() {inherited::Changed();}
  void          Apply_Async() {inherited::Apply_Async(); }
};

#endif // taGuiEditor_h
