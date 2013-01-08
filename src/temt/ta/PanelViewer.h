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

#ifndef PanelViewer_h
#define PanelViewer_h 1

// parent includes:
#include <FrameViewer>

// member includes:

// declare all other types mentioned but not required to include:
class iTabViewer; // 
class iTabBarBase; // 


class TA_API PanelViewer : public FrameViewer {
  // #NO_TOKENS ##DEF_NAME_ROOT_Panels ##DEF_NAME_STYLE_2 the base type for browser frames (tree of objects or classes)
INHERITED(FrameViewer)
friend class iDataPanel;
public:

  inline iTabViewer*    widget() {return (iTabViewer*)inherited::widget();}
  iTabBarBase*          tabBar();

  TA_DATAVIEWFUNS(PanelViewer, FrameViewer) //
protected:
  override IDataViewWidget* ConstrWidget_impl(QWidget* gui_parent); // #IGNORE
private:
  NOCOPY(PanelViewer)
  void                  Initialize();
  void                  Destroy() {CutLinks();}
};

#endif // PanelViewer_h
