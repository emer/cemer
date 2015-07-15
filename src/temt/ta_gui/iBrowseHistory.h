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
#include <IMultiSigLinkClient>

// member includes:
#include <taPtrList>
#include <taiSigLink>
#include <ContextFlag>

// declare all other types mentioned but not required to include:
class iTreeViewItem; //
class ISelectableHost; //


typedef taPtrList<taiSigLink> taiSigLink_PList; // list of taiSigLink

taTypeDef_Of(iBrowseHistory);

class TA_API iBrowseHistory: public QObject, public IMultiSigLinkClient {
  // #NO_INSTANCE #NO_CSS retains browsing history
INHERITED(QObject)
  Q_OBJECT
public:
  int                   max_items; // #DEF_20 number of history items
  int                   cur_item; // -1 when empty; max_items when off the end

  taiSigLink_PList     items;
  void                  reset(); // esp used to put all signals etc. in correct place

  void                  addItem(taiSigLink* link);

  iBrowseHistory(QObject* parent = NULL);
  ~iBrowseHistory();

public: // ITypedObject interface
  void*        This() override {return (void*)this;}
  TypeDef*     GetTypeDef() const override {return &TA_iBrowseHistory;}
public: // ISigLinkClient interface
  void         SigLinkRecv(taSigLink*, int, void*, void*) override {}
  void         SigLinkDestroying(taSigLink* dl) override;

#ifndef __MAKETA__
public slots:
  void                  ItemSelected(iTreeViewItem* tvi);
  void                  SelectableHostNotifying(ISelectableHost* src_host, int op);
  void                  back();
  void                  forward();

signals:
  void                  back_enabled(bool);
  void                  forward_enabled(bool);
  void                  select_item(taiSigLink* dl);
#endif

protected:
  ContextFlag           navigating; // so we ignore the callback

  void                  doEnabling();
  void                  itemAdding(taiSigLink* link); // link if no refs
  void                  itemRemoved(taiSigLink* link); // unlink if no more refs
};

#endif // iBrowseHistory_h
