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

#ifndef taWizard_h
#define taWizard_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <taDoc>

// declare all other types mentioned but not required to include:


taTypeDef_Of(taWizard);

class TA_API taWizard : public taNBase {
  // ##BUTROWS_2 ##EDIT_WIDTH_60 ##CAT_Wizard wizard for automating construction of simulation objects
INHERITED(taNBase)
public:
  bool          auto_open;      // open this wizard upon startup
  taDoc         wiz_doc;        // #HIDDEN #NO_SAVE wizard doc object

  virtual void          RenderWizDoc();
  // #IGNORE render the wizard doc, providing the interface that the user sees to select wizard elements -- called in InitLinks -- calls header, impl, footer -- use impl to override raw text render

  override String       GetTypeDecoKey() const { return "Wizard"; }

  void  InitLinks();
  TA_BASEFUNS(taWizard);
protected:
  void  UpdateAfterEdit_impl();
  virtual void  RenderWizDoc_header(); // the doc header (very start) text
  virtual void  RenderWizDoc_footer(); // the doc footer (very end) text
  virtual void  RenderWizDoc_impl();   // main render of content
private:
  SIMPLE_COPY(taWizard);
  void  Initialize();
  void  Destroy()       { CutLinks(); }
};

#endif // taWizard_h
