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

#ifndef iPanelOfText_h
#define iPanelOfText_h 1

// parent includes:
#include <iPanel>

// member includes:

// declare all other types mentioned but not required to include:
class QTextEdit; //


taTypeDef_Of(iPanelOfText);

class TA_API iPanelOfText: public iPanel {
  // a panel frame for displaying text; used, ex. by Scripts and Programs
  Q_OBJECT
INHERITED(iPanel)
public:
  QTextEdit*            txtText; // the text of the script

  virtual bool          readOnly();
  virtual void          setReadOnly(bool value);
  virtual void          setText(const String& value);


  String       panel_type() const CPP11_OVERRIDE;

  virtual int          EditAction(int ea);
  virtual int          GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)

  QWidget*     firstTabFocusWidget() CPP11_OVERRIDE;

  iPanelOfText(taiSigLink* dl_);
  ~iPanelOfText();

public: // ISigLinkClient interface
  void*        This() CPP11_OVERRIDE {return (void*)this;}
  TypeDef*     GetTypeDef() const CPP11_OVERRIDE {return &TA_iPanelOfText;}
protected:
  void         SigEmit_impl(int sls, void* op1, void* op2) CPP11_OVERRIDE; //
//  int                EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL) CPP11_OVERRIDE;

protected slots:
  void                  textText_copyAvailable (bool yes);

};

#endif // iPanelOfText_h
