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

#ifndef iPanelOfMatrix_h
#define iPanelOfMatrix_h 1

// parent includes:
#include <iPanel>

// member includes:

// declare all other types mentioned but not required to include:
class iMatrixEditor; // #IGNORE
class iTableView; // #IGNORE
class taMatrix; //

taTypeDef_Of(iPanelOfMatrix);

class TA_API iPanelOfMatrix: public iPanel {
  Q_OBJECT
#ifndef __MAKETA__
typedef iPanel inherited;
#endif
public:
  iMatrixEditor*        me;

  taMatrix*             mat() {return (m_link) ? (taMatrix*)(link()->data()) : NULL;}
  String       panel_type() const CPP11_OVERRIDE; // this string is on the subpanel button for this panel

  QWidget*     firstTabFocusWidget() CPP11_OVERRIDE;

  iPanelOfMatrix(taiSigLink* dl_);
  ~iPanelOfMatrix();

protected:
  void         UpdatePanel_impl() CPP11_OVERRIDE;

protected slots:
  void                  tv_hasFocus(iTableView* sender);

public: // ISigLinkClient interface
  void*        This() CPP11_OVERRIDE {return (void*)this;}
  TypeDef*     GetTypeDef() const CPP11_OVERRIDE {return &TA_iPanelOfMatrix;}
protected:
  void         Render_impl() CPP11_OVERRIDE;
  void         SigEmit_impl(int sls, void* op1, void* op2) CPP11_OVERRIDE; //
//  int                EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL) CPP11_OVERRIDE;


};

#endif // iPanelOfMatrix_h
