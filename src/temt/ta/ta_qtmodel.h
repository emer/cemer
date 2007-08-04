//
// This file is part of TA/CSS
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


// ta_qtmodel.h: Qt models for type and token browsing

#ifndef TA_QTMODEL_H
#define TA_QTMODEL_H

#include "ta_group.h"
#include "ta_variant.h"

#ifndef __MAKETA__
# include <QAbstractItemModel>
#endif

// forwards
class TypeDefItemModel; 


class TA_API TypeDefItemModel: public QAbstractItemModel { // #NO_INSTANCE #NO_CSS class that implements the Qt Model interface for TypeDefs
INHERITED(QAbstractItemModel)
public:
  
  TypeDefItemModel(QObject* parent = NULL); 
  ~TypeDefItemModel(); //
  
#ifndef __MAKETA__
public: // required implementations
  QModelIndex 		index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
  QModelIndex 		parent(const QModelIndex& child) const;
  int 			rowCount(const QModelIndex& parent = QModelIndex()) const; 
  int 			columnCount(const QModelIndex& parent = QModelIndex()) const; 
  QVariant 		data(const QModelIndex& index, int role = Qt::DisplayRole) const; 
//bool 			setData(const QModelIndex& index, const QVariant& value, int role);
  bool 			hasChildren(const QModelIndex& index) const;
  Qt::ItemFlags 	flags(const QModelIndex& index) const; 
  QVariant 		headerData(int section, Qt::Orientation orientation, 
    int role = Qt::DisplayRole) const; // override
  void 			sort(int column, Qt::SortOrder order);

#endif // !__MAKETA__
};

#endif

