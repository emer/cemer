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

#ifndef taiTokenPtrButton_h
#define taiTokenPtrButton_h 1

// parent includes:
#include <taiItemPtrBase>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taiTokenPtrButton : public taiItemPtrBase {
  // for tokens of taBase objects
  INHERITED(taiItemPtrBase)
public:
  inline taBase*        token() const {return (taBase*)m_sel;}
  override int          columnCount(int view) const;
  override const String headerText(int index, int view) const;
  override int          viewCount() const {return 1;}
  override const String viewText(int index) const;
  override bool         hasNoItems();
  override bool         hasOnlyOneItem();
  virtual bool          countTokensToN(int& cnt, TypeDef* td, int n, void*& last_itm);
  // recursively count valid (in scope etc) tokens of type until reaching n, at which point a true is returned -- if n is not reached, return false -- used for above two routines
  override void         GetImage(void* cur_sel, TypeDef* targ_typ);
  virtual void          GetImageScoped(taBase* ths, TypeDef* targ_typ, taBase* scope = NULL,
        TypeDef* scope_type = NULL);
  // get image, using the new type and scope params supplied
  virtual taBase*       GetValue() {return token();}

  void                  BuildChooser(taiItemChooser* ic, int view = 0); // override
  override void         btnHelp_clicked();

  override void         EditPanel();
  override void         EditDialog();

  taiTokenPtrButton(TypeDef* typ_, IWidgetHost* host,
                    taiData* par, QWidget* gui_parent_, int flags_ = 0,
                    const String& flt_start_txt = "");
protected:
  taSmartRef            scope_ref;      // reference object for scoping, default is none
  TypeDef*              scope_typ;      // type of scope to use (NULL = default)

  const String          itemTag() const {return "Token: ";}
  const String          labelNameNonNull() const;

  int                   BuildChooser_0(taiItemChooser* ic, TypeDef* top_typ,
    QTreeWidgetItem* top_item); // we use this recursively
  virtual bool          ShowToken(taBase* obj) const;
};

#endif // taiTokenPtrButton_h
