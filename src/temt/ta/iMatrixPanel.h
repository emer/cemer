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

#ifndef iMatrixPanel_h
#define iMatrixPanel_h 1

// parent includes:
#include <iDataPanelFrame>

// member includes:

// declare all other types mentioned but not required to include:
class iMatrixEditor; // #IGNORE
class iTableView; // #IGNORE
class taMatrix; //

TypeDef_Of(iMatrixPanel);

class TA_API iMatrixPanel: public iDataPanelFrame {
  Q_OBJECT
#ifndef __MAKETA__
typedef iDataPanelFrame inherited;
#endif
public:
  iMatrixEditor*        me;

  taMatrix*             mat() {return (m_link) ? (taMatrix*)(link()->data()) : NULL;}
  override String       panel_type() const; // this string is on the subpanel button for this panel

  override QWidget*     firstTabFocusWidget();

  iMatrixPanel(taiSigLink* dl_);
  ~iMatrixPanel();

protected:
  override void         UpdatePanel_impl();

protected slots:
  void                  tv_hasFocus(iTableView* sender);

public: // ISigLinkClient interface
  override void*        This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_iMatrixPanel;}
protected:
  override void         Render_impl();
  override void         SigEmit_impl(int dcr, void* op1, void* op2); //
//  override int                EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL);


};

#endif // iMatrixPanel_h
