// Copyright, 1995-2007, Regents of the University of Colorado,
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

// ta_viewer.h: non-gui definitions of classes mediated window viewing

#ifndef TA_VIEWER_H
#define TA_VIEWER_H

// #include "ta_base.h"
// #include "colorscale.h"

// #include "ta_TA_type_WRAPPER.h"

/* Node Bitmap flags

The icon client (ex. Browser) will query for an icon, passing the attributes such as Open, Linked, etc.
The icon supplier (ex. taBase object) returns an icon bitmap, and an indication of what attribute flags it
supports. The Browser can then add its own generic modifiers for attributes not supported by the supplier.
ex. the browser can add a generic small arrow to indicate a link
*/

// enum NodeBitmapFlags {
//   NBF_FOLDER_OPEN               = 0x01,
//   NBF_LINK_ITEM                 = 0x02
// };

// // externals (most in in ta_qtviewer.h, some in ta_qtdata.h)

// class taProject;

// #ifdef TA_GUI
//   class taiMenuBar;
//   class taiMenu_List;
//   class taiActions;
//   class taiDataLink;
//   class ISelectable;
//   class IDataViewWidget;
//   class iFrameViewer;
//   class iBrowseViewer;
//   class iTabViewer;
//   class iTabViewer;
//   class iMainWindowViewer;
//   class iDockViewer;
//   class iToolBoxDockViewer;
//   class iTabBarBase;
//   class iTabBar;
//   class iTabView;
//   class iDataPanel;
//   class iDataPanelSet;
//   class iDataPanel_PtrList;
//   class iTabView_PtrList;
//   class iToolBar;
//   class iToolBar_List; //
// #else // !TA_GUI
// // note: QObject/QWidget below are always the dummy guys
// # typedef taiMenuBar QWidget;
// # typedef taiMenu_List VoidClass;
// # typedef taiActions QObject;
// # typedef taiDataLink taDataLink;
// # typedef ISelectable VoidClass;
// # typedef IDataViewWidget VoidClass;
// # typedef iFrameViewer QWidget;
// # typedef iBrowseViewer QWidget;
// # typedef iTabViewer QWidget;
// # typedef iTabViewer QWidget;
// # typedef iMainWindowViewer QWidget;
// # typedef iDockViewer QWidget;
// # typedef iToolBoxDockViewer QWidget;
// # typedef iTabBar QWidget;
// # typedef iTabView QWidget;
// # typedef iDataPanel QWidget;
// # typedef iDataPanelSet QWidget;
// # typedef iDataPanel_PtrList VoidClass;
// # typedef iTabView_PtrList VoidClass;
// # typedef iToolBar QWidget;
// # typedef iToolBar_List  VoidClass;
// #endif // def TA_GUI

// // forwards this file

// class ToolBar_List;
// class DataViewer;
// class   ToolBar;
// class   TopLevelViewer;
// class     DockViewer;
// class     MainWindowViewer;
// class   FrameViewer;
// class     BrowseViewer;
// class       tabBrowseViewer;
// class       ClassBrowseViewer;
// class     PanelViewer;
// class     T3DataViewer;
// class ToolBoxRegistrar; //

#endif
