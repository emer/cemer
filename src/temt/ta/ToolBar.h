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
#include <DataViewer>

// member includes:

// declare all other types mentioned but not required to include:
class iToolBar; //


class TA_API ToolBar: public DataViewer {// ##NO_TOKENS proxy for Toolbars
friend class iToolBar;
//nn? friend class MainWindowViewer;
INHERITED(DataViewer)
public:
  float                 lft;    // #HIDDEN when undocked, fractional position on screen
  float                 top;    // #HIDDEN when undocked, fractional position on screen
  Orientation           o; // whether hor or vert

  inline iToolBar*      widget() {return (iToolBar*)inherited::widget();} // #IGNORE lex override

  TA_DATAVIEWFUNS(ToolBar, DataViewer)

protected:
  override void          Constr_impl(QWidget* gui_parent);
#ifdef TA_GUI
  override IDataViewWidget* ConstrWidget_impl(QWidget* gui_parent); // in qt file
#endif
  override void         WidgetDeleting_impl();
  override void         GetWinState_impl();
  override void         SetWinState_impl();
private:
  void  Copy_(const ToolBar& cp);
  void  Initialize();
  void  Destroy() {}
};

#endif // ToolBar_h
