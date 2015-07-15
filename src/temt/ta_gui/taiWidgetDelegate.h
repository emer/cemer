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

#ifndef taiWidgetDelegate_h
#define taiWidgetDelegate_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QItemDelegate>
#endif

// member includes:
#ifndef __MAKETA__
#include <QPointer>
#endif


// declare all other types mentioned but not required to include:
class taiEditorOfClass; //
class taBase; //
class taiWidget; //
class QAbstractItemModel; //
class QWidget; //
class QModelIndex; //
class QHBoxLayout; // 


class TA_API taiWidgetDelegate: public QItemDelegate {
INHERITED(QItemDelegate)
Q_OBJECT
public:
  taiEditorOfClass*      edh;

  virtual void          GetImage() const; // callable from edh any time
  virtual void          GetValue() const; // callable from edh
  virtual bool          IndexToMembBase(const QModelIndex& index,
    MemberDef*& mbr, taBase*& base) const = 0;

  taiWidgetDelegate(taiEditorOfClass* edh_);

public: // overrides
  QWidget*     createEditor(QWidget* parent,
    const QStyleOptionViewItem& option, const QModelIndex& index) const override;
  void         paint(QPainter* painter, const QStyleOptionViewItem& option,
    const QModelIndex& index) const override;
  void         setEditorData(QWidget* editor,
    const QModelIndex& index) const override;
  void         setModelData(QWidget* editor, QAbstractItemModel* model,
    const QModelIndex& index ) const override;
  QSize        sizeHint(const QStyleOptionViewItem& option,
    const QModelIndex& index) const override;
protected:
  mutable QPointer<taiWidget> dat; // most recently created
  mutable QPointer<QWidget> rep; // most recently created
  mutable int           m_dat_row; // row corresponding to dat, -1 if none
  mutable QSize         sh; // current size hint -- we return greatest
  mutable QPointer<QHBoxLayout> hbl;

  bool         eventFilter(QObject* object, QEvent* event) override; // replace
  virtual void          EditorCreated(QWidget* parent, QWidget* editor,
    const QStyleOptionViewItem& option, const QModelIndex& index) const;

protected slots:
  virtual void          rep_destroyed(QObject* rep); // when dat.rep destroys
  virtual void          this_closeEditor(QWidget* editor,
    QAbstractItemDelegate::EndEditHint hint = NoHint);
};

#endif // taiWidgetDelegate_h
