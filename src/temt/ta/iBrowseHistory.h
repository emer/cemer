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

#ifndef iBrowseHistory_h
#define iBrowseHistory_h 1

// parent includes:
#ifndef __MAKETA__
#include <QObject>
#endif
#include <IMultiDataLinkClient>

// member includes:
#include <taPtrList>
#include <taiDataLink>
#include <ContextFlag>

// declare all other types mentioned but not required to include:
class iTreeViewItem; //
class ISelectableHost; //


typedef taPtrList<taiDataLink> taiDataLink_PList; // list of taiDataLink

class TA_API iBrowseHistory: public QObject, public IMultiDataLinkClient {
  // #NO_INSTANCE #NO_CSS retains browsing history
INHERITED(QObject)
  Q_OBJECT
public:
  int                   max_items; // #DEF_20 number of history items
  int                   cur_item; // -1 when empty; max_items when off the end

  taiDataLink_PList     items;
  void                  reset(); // esp used to put all signals etc. in correct place

  void                  addItem(taiDataLink* link);

  iBrowseHistory(QObject* parent = NULL);
  ~iBrowseHistory();

public: // ITypedObject interface
  override void*        This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_iBrowseHistory;}
public: // IDataLinkClient interface
  override void         DataDataChanged(taDataLink*, int, void*, void*) {}
  override void         DataLinkDestroying(taDataLink* dl);

#ifndef __MAKETA__
public slots:
  void                  ItemSelected(iTreeViewItem* tvi);
  void                  SelectableHostNotifying(ISelectableHost* src_host, int op);
  void                  back();
  void                  forward();

signals:
  void                  back_enabled(bool);
  void                  forward_enabled(bool);
  void                  select_item(taiDataLink* dl);
#endif

protected:
  ContextFlag           navigating; // so we ignore the callback

  void                  doEnabling();
  void                  itemAdding(taiDataLink* link); // link if no refs
  void                  itemRemoved(taiDataLink* link); // unlink if no more refs
};

#endif // iBrowseHistory_h
