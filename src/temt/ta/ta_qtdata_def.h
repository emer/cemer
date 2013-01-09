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


// ta_qtdata.h: Qt-based graphical data representations

#ifndef TA_QTDATA_DEF_H
#define TA_QTDATA_DEF_H

#include "ta_base.h"
#include "ta_variant.h"

#ifndef __MAKETA__
# include <QObject>
# include <QLabel>
# include <QPointer>
# include <QWidget>
#endif

//??nn in win32  #include <unistd.h>

// externals
class iMainWindowViewer; //

// Forward declarations

class taiData;
class IDataHost; // interface for top level host of data items, mostly for DataChanged
class taiDataHost;
class taiAction;
class taiMenuAction;
class taiMenu;
class taiMethodData;

class QWidget;


//////////////////////////////////////////////////////////
//      taiMenuAction: wrapper for Qt callbacks         //
//////////////////////////////////////////////////////////

class TA_API taiMenuAction { // object that holds specs for receiver for Action(taiAction*) callback
public:
  QObject*      receiver;
  String        member;

  taiMenuAction() {receiver = NULL;}
  taiMenuAction(QObject* receiver_, const char* member_) {receiver = receiver_; member = member_;}
  taiMenuAction(const taiMenuAction& src) {receiver = src.receiver; member = src.member;} //

//  void                connect(QObject* sender, const char* signal) const; // #IGNORE
  taiMenuAction&        operator=(const taiMenuAction& rhs);
};


class TA_API IDataHost: public ITypedObject { // #VIRT_BASE #NO_INSTANCE #NO_CSS
public:
  virtual const iColor  colorOfCurRow() const = 0; // #IGNORE background color of cur row
  virtual bool          isConstructed() = 0;
  virtual bool          isModal() = 0;
  virtual bool          isReadOnly() = 0;
  virtual taMisc::ShowMembs show() const = 0;
  virtual iMainWindowViewer* viewerWindow() const {return NULL;} // used to set cliphandler
  virtual bool          HasChanged() = 0; // 'true' if has changed
  virtual void*         Root() const = 0; // root of the object
  virtual taBase*       Base() const = 0; // root of the object, if a taBase
  virtual TypeDef*      GetRootTypeDef()const  = 0; // TypeDef on the root, for casting
  virtual void          GetValue() = 0; // copy gui to value
  virtual void          GetImage() = 0; // copy value to gui
  virtual void          Changed() {} // called by embedded item to indicate contents have changed
  virtual void          SetItemAsHandler(taiData* item, bool set_it = true); // called by compatible controls to set or unset the control as clipboard/focus handler (usually don't need to unset); in ta_qtviewer.cpp
  virtual void          Apply_Async() = 0; // provided for the APPLY_IMMED functionality

  IDataHost() {}
  virtual ~IDataHost() {} //
};

//////////////////////////////////////////////////////////
//      taiData: objects to wrap gui widgets for data   //
//////////////////////////////////////////////////////////

#include <taiDataList>

#endif
