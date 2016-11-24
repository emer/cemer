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

#ifndef taiWidgetTokenChooser_h
#define taiWidgetTokenChooser_h 1

// parent includes:
#include <taiWidgetItemChooser>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiWidgetTokenChooser : public taiWidgetItemChooser {
  // for tokens of taBase objects
  INHERITED(taiWidgetItemChooser)
public:
  inline taBase* token() const {return (taBase*)m_sel;}
  int          columnCount(int view) const override;
  const String headerText(int index, int view) const override;
  int          viewCount() const override {return 1;}
  const String viewText(int index) const override;
  bool         hasNoItems() override;
  bool         hasOnlyOneItem() override;
  virtual bool countTokensToN(int& cnt, TypeDef* td, int n, void*& last_itm);
  // recursively count valid (in scope etc) tokens of type until reaching n, at which point a true is returned -- if n is not reached, return false -- used for above two routines
  void         GetImage(void* cur_sel, TypeDef* targ_typ) override;
  virtual void GetImageScoped(taBase* ths, TypeDef* targ_typ, taBase* scope = NULL,
                              TypeDef* scope_type = NULL);
  // get image, using the new type and scope params supplied
  virtual taBase* GetValue() {return token();}

  void         BuildChooser(iDialogItemChooser* ic, int view = 0) override;
  void         btnHelp_clicked() override;

  void         EditPanel() override;
  void         EditDialog() override;

  taiWidgetTokenChooser(TypeDef* typ_, IWidgetHost* host,
                    taiWidget* par, QWidget* gui_parent_, int flags_ = 0,
                    const String& flt_start_txt = "");
protected:
  taSmartRef            scope_ref;      // reference object for scoping, default is none
  TypeDef*              scope_typ;      // type of scope to use (NULL = default)

  const String          itemTag() const override {return "Token: ";}
  const String          labelNameNonNull() const override;

  int                   BuildChooser_0(iDialogItemChooser* ic, TypeDef* top_typ,
    QTreeWidgetItem* top_item); // we use this recursively
  virtual bool          ShowToken(taBase* obj) const;
};

#endif // taiWidgetTokenChooser_h
