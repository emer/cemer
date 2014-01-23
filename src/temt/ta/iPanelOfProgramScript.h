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

#ifndef iPanelOfProgramScript_h
#define iPanelOfProgramScript_h 1

// parent includes:
#include <iPanel>

// member includes:

// declare all other types mentioned but not required to include:
#ifndef __MAKETA__
class iNumberedTextView;
#endif
class Program; 

taTypeDef_Of(iPanelOfProgramScript);

class TA_API iPanelOfProgramScript: public iPanel {
  // a panel frame for viewing script code of program
  Q_OBJECT
INHERITED(iPanel)
public:
#ifndef __MAKETA__
  iNumberedTextView*     vs; // the view script widget
#endif

  Program*              prog() {return (m_link) ? (Program*)(link()->data()) : NULL;}
  String       panel_type() const override {return "css Script";}

  bool         HasChanged() override; // 'true' if user has unsaved changes
  void                  FillList();

  QWidget*     firstTabFocusWidget() override;

  iPanelOfProgramScript(taiSigLink* dl_);
  ~iPanelOfProgramScript();

public slots:
  // these all connect from corresponding signals on the iNumberBar or iNumberedTextView
  void     lineFlagsUpdated(int lineno, int flags);
  void     viewSource(int lineno);
  void     mouseHover(const QPoint &pos, int lineno, const QString& word);

public: // ISigLinkClient interface
  void*        This() override {return (void*)this;}
  TypeDef*     GetTypeDef() const override {return &TA_iPanelOfProgramScript;}
  bool         ignoreSigEmit() const override;

protected:
  void         SigEmit_impl(int sls, void* op1, void* op2) override; //
  void         OnWindowBind_impl(iPanelViewer* itv) override;
  void         UpdatePanel_impl() override;
  void         ResolveChanges_impl(CancelOp& cancel_op) override;
  void         showEvent(QShowEvent* ev) override;
};

#endif // iPanelOfProgramScript_h
