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

#ifndef taiAction_h
#define taiAction_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QAction>
#endif

// member includes:
#include <Variant>

// declare all other types mentioned but not required to include:
class QString; // #IGNORE
class QKeySequence; // #IGNORE
class taiMenuAction; //

class TA_API taiAction: public QAction {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS holds menu and/or toolbar item data -- can be the root item of a submenu
  Q_OBJECT
  friend class taiActions;
  friend class taiAction_List;
public:
  enum CallbackType {
    none,               // callback parameters ignored
    action,             // corresponds to IV "action" type of callback -- slot for signal is parameterless
    men_act,            // corresponds to IV "men_act" (menu action) type of callback -- slot for signal takes taiAction* as a sender
    int_act,            // corresponds to Qt menu/action signals -- slot for signal takes an int parameter
    ptr_act,            // slot for signal takes a void* parameter
    var_act             // slot for signal takes a "const Variant&" parameter
  };

  int                   sel_type;
  String                label() {return String(text());}
  Variant               usr_data; // default is Invalid, can also be int or void*

  bool                  canSelect(); // true if item can be taiMenu::curSel value
  virtual bool          isSubMenu()     { return false; }
  bool                  isGrouped(); // indicates if item in a group, ie is a radio group action

  taiAction(int sel_type_, const String& label_); // used by taiMenu etc.
  taiAction(const QString& label_, const QKeySequence& accel, const char* name); // used by viewer/browser
  taiAction(const QString& label_, QObject* receiver, const char* member, const QKeySequence& accel);
    // used by viewer/browser
  taiAction(const Variant& usr_data_, const QString& label_, const QKeySequence& accel, const char* name = NULL); // used by viewer/browser
  virtual ~taiAction();

  void connect(CallbackType ct_, const QObject *receiver, const char* member); // connect callback to given
  void connect(const taiMenuAction* mact); // convenience function

  virtual QMenu*        SubMenu()       { return NULL; }

#ifndef __MAKETA__
signals:
  void Action();                        // for a direct call-back
  void MenuAction(taiAction* sender);   // for a menu-action call-back (returns item selected)
  void IntParamAction(int param);       // for Qt-style parameter ((int)usr_data)
  void PtrParamAction(void* ptr);       // for ptr parameter ((void*)usr_data)
  void VarParamAction(const Variant& var);      // for variant parameter ((void*)usr_data)
#endif // ndef __MAKETA__

protected:
  int                   nref; // ref'ed when put onto a taiAction_List, deleted when ref=0
  int                   m_changing; // for supressing recursive signals
  void                  init(int sel_type_);
  void                  emitActions();

protected slots:
  virtual void          this_triggered_toggled(bool checked); // annoying, but self connect, so we can re-raise

private:
  taiAction(const taiAction&); // no copying
  void operator=(const taiAction&);
};

#endif // taiAction_h
