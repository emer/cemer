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

#ifndef taiListElsButtonBase_h
#define taiListElsButtonBase_h 1

// parent includes:
#include <taiItemPtrBase>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiListElsButtonBase : public taiItemPtrBase {
// for items in a list
INHERITED(taiItemPtrBase)
public:
  inline taBase*        item() const {return (taBase*)m_sel;}
  int                   columnCount(int view) const; // override
  const String          headerText(int index, int view) const; // override
  int                   viewCount() const {return 1;} // override
  const String          viewText(int index) const; // override

  taBase*               GetValue() {return item();}

  taiListElsButtonBase(TypeDef* typ_, IDataHost* host,
                       taiData* par, QWidget* gui_parent_, int flags_ = 0,
                       const String& flt_start_txt = "");
protected:
  const String          labelNameNonNull() const;

  virtual int           BuildChooser_0(taiItemChooser* ic, taList_impl* top_lst,
                                       QTreeWidgetItem* top_item);
  // we use this recursively, and also in gpi guy
};

#endif // taiListElsButtonBase_h
