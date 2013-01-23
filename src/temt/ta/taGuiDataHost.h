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

#ifndef taGuiDataHost_h
#define taGuiDataHost_h 1

// parent includes:
#include <taiDataHostBase>
#include <IDataHost>

// member includes:

// declare all other types mentioned but not required to include:
class taGuiDialog;


TypeDef_Of(taGuiDataHost);

class TA_API taGuiDataHost : public taiDataHostBase, virtual public IDataHost
{ // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(taiDataHostBase)
  Q_OBJECT
friend class iHostDialog;
public:
  taGuiDialog*  gui_owner;

  taGuiDataHost(taGuiDialog* own, bool read_only_ = false,
                bool modal_ = false, QObject* parent = 0);
  virtual ~taGuiDataHost();

  override void Constr_Body();
  override void GetImage(bool force);
  override void Ok_impl();

public: // IDataLinkClient i/f -- note: only registered though for taiEDH and later
//   void               DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);

public: // ITypedObject i/f (common to IDLC and IDH)
  override void*        This() {return this;}
  override TypeDef*     GetTypeDef() const {return &TA_taGuiDataHost;}

public: // IDataHost i/f
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

#endif // taGuiDataHost_h
