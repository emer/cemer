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

#ifndef TopLevelViewer_h
#define TopLevelViewer_h 1

// parent includes:
#include <taViewer>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(TopLevelViewer);

class TA_API TopLevelViewer : public taViewer {
  // #NO_TOKENS #VIRT_BASE stuff that is common to anything that can be a top-level window
INHERITED(taViewer)
public:
    // can be provided to put msg up on closing
  override bool         deleteOnWinClose() const;
  bool                  openOnLoad() const {return false;}
    // 'true' if the viewer should be opened after loading (note: still must check if topLevel)
    // TODO: define impl somehow
  virtual bool          isRoot() const {return false;} // only true for main proj window
  virtual bool          isTopLevel() const {return true;} // to differentiate, when it could be either

  virtual void          ViewWindow();
    // #MENU #MENU_CONTEXT #MENU_ON_Object either de-iconfiy if exists or create a new window if doesn't
  virtual void          Iconify();              // #MENU iconify the window (saves iconified state)
  virtual void          DeIconify();            // deiconify the window (saves deiconified state)
//  virtual void                ScriptWinState()                { winState().ScriptWinState(cout); }
    // #NO_SCRIPT generate script code to position the window
  virtual void          SetWinName();           // #IGNORE set the window name

  override void         WindowClosing(CancelOp& cancel_op);

  void  InitLinks();
  void  CutLinks();
  TA_DATAVIEWFUNS(TopLevelViewer, taViewer) //
protected:
  String                win_name;

  override void         GetWinState_impl();
  override void         SetWinState_impl();
  virtual void          MakeWinName_impl() {} // set win_name, impl in subs

private:
  void  Copy_(const TopLevelViewer& cp);
  void  Initialize();
  void  Destroy() {CutLinks();}
};

#endif // TopLevelViewer_h
