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

#ifndef ISelectableHost_QObj_h
#define ISelectableHost_QObj_h 1

// parent includes:
#include "ta_def.h"

// member includes:
#include <ISelectableHost>

// declare all other types mentioned but not required to include:
class iAction; // 


class TA_API ISelectableHost_QObj: public QObject {
 // #IGNORE this is a helper QObject that handles signals/slots
  Q_OBJECT
INHERITED(QObject)
friend class ISelectableHost;
friend class ISelectable;
public:

public slots:
  void          EditActionsEnabled(int& ea) {host->EditActionsEnabled(ea);}
    // callback for when we are ClipHandler
  void          EditAction(int ea) {host->EditAction(ea);} //  callback for when we are ClipHandler
  void          ctxtMenu_destroyed() {host->ctxtMenu_destroyed();}
   // attached to ctxt menu so we can clean up the mimesource
  void          EditAction(iAction*); //  callback for context menu

#ifndef __MAKETA__
signals:
  void          NotifySignal(ISelectableHost* src, int op);
#endif

protected:
  ISelectableHost*      host;

  override void         customEvent(QEvent* ev); // dispatch
  void                  Emit_NotifySignal(ISelectableHost::NotifyOp op);

  ISelectableHost_QObj(ISelectableHost* host_) {host = host_;}

protected slots:
  void          DynAction(int i) {host->DoDynAction(i);}
  void          DropEditAction(int ea) {host->DropEditAction(ea);}  // for drops
  void          ItemRemoving(ISelectable* item) {host->RemoveSelectedItem(item, false);}
private:
  ISelectableHost_QObj& operator=(const ISelectableHost_QObj&); // not allowed
};

#endif // ISelectableHost_QObj_h
