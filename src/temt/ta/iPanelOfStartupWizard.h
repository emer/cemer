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

#ifndef iPanelOfStartupWizard_h
#define iPanelOfStartupWizard_h 1

// parent includes:
#include <iPanel>

// member includes:

// declare all other types mentioned but not required to include:
class iSplitter; //
class taiWidgetProjTemplateElChooser; //
class ProjTemplateEl; //
class iDialogItemChooser; //
class taiWidgetStringArrayChooser; //

taTypeDef_Of(iPanelOfStartupWizard);

class TA_API iPanelOfStartupWizard : public iPanel {
  // a panel for displaying the startup wizard in root view
  Q_OBJECT
INHERITED(iPanel)
public:
  iSplitter*   sw_split;        // main startup widget splitter 
  taiWidgetProjTemplateElChooser* new_proj_chs;  // new project chooser
  iDialogItemChooser*           new_proj_chs_dlg; // dialog for chooser
  ProjTemplateEl*               new_proj_tmplt; // chosen template
  taiWidgetStringArrayChooser*  rec_proj_chs; // recent project chooser
  iDialogItemChooser*           rec_proj_chs_dlg; // dialog for chooser
  String*                       rec_proj_nm;     // chosen project

  String       panel_type() const override {return "StartupWizard";}

  iPanelOfStartupWizard(taiSigLink* dl_);
  ~iPanelOfStartupWizard();

public slots:
  void         NewProjSelected();
  // a new project choice has been selected
  void         RecProjSelected();
  // a recent project choice has been selected
  void         OpenProject();
  // open a project file chooser..

public: // ISigLinkClient interface
  void*        This() override {return (void*)this;}
  TypeDef*     GetTypeDef() const override {return &TA_iPanelOfStartupWizard;}
  void         SigLinkDestroying(taSigLink* dl) override;
  bool         ignoreSigEmit() const override;

protected:
  void         SigEmit_impl(int sls, void* op1, void* op2) override;
  void         UpdatePanel_impl() override;

};

#endif // iPanelOfStartupWizard_h
