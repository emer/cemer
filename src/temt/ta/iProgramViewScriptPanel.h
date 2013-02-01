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

#ifndef iProgramViewScriptPanel_h
#define iProgramViewScriptPanel_h 1

// parent includes:
#include <iDataPanelFrame>

// member includes:

// declare all other types mentioned but not required to include:
#ifndef __MAKETA__
class NumberedTextView;
#endif
class Program; 

TypeDef_Of(iProgramViewScriptPanel);

class TA_API iProgramViewScriptPanel: public iDataPanelFrame {
  // a panel frame for viewing script code of program
  Q_OBJECT
INHERITED(iDataPanelFrame)
public:
#ifndef __MAKETA__
  NumberedTextView*     vs; // the view script widget
#endif

  Program*              prog() {return (m_link) ? (Program*)(link()->data()) : NULL;}
  override String       panel_type() const {return "css Script";}

  override bool         HasChanged(); // 'true' if user has unsaved changes
  void                  FillList();

  override QWidget*     firstTabFocusWidget();

  iProgramViewScriptPanel(taiSigLink* dl_);
  ~iProgramViewScriptPanel();

public slots:
  // these all connect from corresponding signals on the NumberBar or NumberedTextView
  void     lineFlagsUpdated(int lineno, int flags);
  void     viewSource(int lineno);
  void     mouseHover(const QPoint &pos, int lineno, const QString& word);

public: // ISigLinkClient interface
  override void*        This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_iProgramViewScriptPanel;}
  override bool         ignoreSigEmit() const;

protected:
  override void         SigEmit_impl(int dcr, void* op1, void* op2); //
  override void         OnWindowBind_impl(iTabViewer* itv);
  override void         UpdatePanel_impl();
  override void         ResolveChanges_impl(CancelOp& cancel_op);
  override void         showEvent(QShowEvent* ev);
};

#endif // iProgramViewScriptPanel_h
