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

#ifndef iTopLevelWindow_List_h
#define iTopLevelWindow_List_h 1

// parent includes:
#include <IViewerWidget>
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:
class iMainWindowViewer; // 
class iDockViewer; // 


class TA_API iTopLevelWindow_List: public taPtrList<IViewerWidget> {
public:
  iMainWindowViewer*    SafeElAsMainWindow(int i);
    // returns item cast as requested, or NULL if not of that type
  iMainWindowViewer*    FindMainWindowById(int id);
    // find the main window with indicated uniqueId NULL if not of that type
  iDockViewer*          SafeElAsDockWindow(int i);
    // returns item cast as requested, or NULL if not of that type
  iMainWindowViewer*    Peek_MainWindow(); // finds the topmost main window, NULL if none
  iMainWindowViewer*    Peek_ProjBrowser(); // finds the topmost main window that is a project browser, NULL if none
  iDockViewer*          Peek_DockWindow(); // finds the topmost undocked dock win, NULL if none
  void                  GotFocus_MainWindow(iMainWindowViewer* imw); // puts to top of list
  void                  GotFocus_DockWindow(iDockViewer* idv); // puts to top of list
};

#endif // iTopLevelWindow_List_h
