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

#ifndef ToolBar_h
#define ToolBar_h 1

// parent includes:
#include <taViewer>

// member includes:

// declare all other types mentioned but not required to include:
class iToolBar; // #IGNORE 


taTypeDef_Of(ToolBar);

class TA_API ToolBar: public taViewer {// ##NO_TOKENS proxy for Toolbars
friend class iToolBar;
//nn? friend class MainWindowViewer;
INHERITED(taViewer)
public:
  float                 lft;    // #HIDDEN when undocked, fractional position on screen
  float                 top;    // #HIDDEN when undocked, fractional position on screen
  Orientation           o; // whether hor or vert

  inline iToolBar*      widget() {return (iToolBar*)inherited::widget();} // #IGNORE lex override

  TA_DATAVIEWFUNS(ToolBar, taViewer)

protected:
  void          Constr_impl(QWidget* gui_parent) override;
#ifdef TA_GUI
  IViewerWidget* ConstrWidget_impl(QWidget* gui_parent) override; // in qt file
#endif
  void         WidgetDeleting_impl() override;
  void         GetWinState_impl() override;
  void         SetWinState_impl() override;
private:
  void  Copy_(const ToolBar& cp);
  void  Initialize();
  void  Destroy() {}
};

#endif // ToolBar_h
