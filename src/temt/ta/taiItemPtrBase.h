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

#ifndef taiItemPtrBase_h
#define taiItemPtrBase_h 1

// parent includes:
#include <taiData>

// member includes:

// declare all other types mentioned but not required to include:
class taiItemPtrBase;
class taiItemChooser;
class String_Array;

typedef bool (*item_filter_fun)(void*, void*); // optional filter, spec'ed in ITEM_FILTER_xxx
typedef bool (*cust_chooser_fun)(taBase*, taiItemPtrBase*); // optional custom config, spec'ed in CUST_CHOOSER_xxx

class TA_API taiItemPtrBase : public taiData {
  // common base for MemberDefs, MethodDefs, TypeDefs, Enums, and tokens, that use the ItemChooser
  Q_OBJECT
  INHERITED(taiData)
public:
  item_filter_fun       item_filter; // #IGNORE optional filter, in ITEM_FILTER_xxx
  cust_chooser_fun      cust_chooser; // #IGNORE customization call, in CUST_CHOOSER_xxx
  String                filter_start_txt; // if nonempty, item name must start with this text to be included

  virtual const String  labelText(); // "tag: name" for button
  virtual const String  titleText(); // title of overall chooser;
  virtual int           columnCount(int view) const = 0;
    // number of header columns in the view
  virtual const String  headerText(int index, int view) const = 0;
  inline const String   nullText() { return null_text; }
  inline QAbstractButton* rep() {return m_but;}
  inline void*          sel() const {return m_sel;}
  virtual bool          isValid() const {return (targ_typ);} // if all required params have been set
  virtual int           catCount() const;
  // number of categories, where supported; 0=nocat, 1+=cats
  virtual const String  catText(int index) const;
  // number of different kinds of views, ex flat vs.
  virtual int           viewCount() const {return 1;}
  // number of different kinds of views, ex flat vs. tree
  virtual const String  viewText(int index) const = 0;
  // number of different kinds of views, ex flat vs.
  virtual bool          hasNoItems() { return false; }
  // if it is possible to quickly determine that there are no items on the list, return true -- makes the chooser read-only with "no items available to choose" text label -- if nullOK, then this must always be false.
  virtual bool          hasOnlyOneItem() { return false; }
  // if it is possible to quickly determine that there is one and only one item in the list, return true AND set the selection to that one item -- if true, it makes the chooser read-only --  first check should be if nullOK and there are no items -- then there is one item! -- set to null -- if not, then multiple items and should return false

  void                  setNullText(const String& nt) { null_text = " " + nt; }
  // set text to display instead of NULL for a null item
  void                  setNewObj1(taBase* parent, const String& nt)
  { new1_par = parent; new1_text = nt; }
  // set new object option
  void                  setNewObj2(taBase* parent, const String& nt)
  { new2_par = parent; new2_text = nt; }
  // set new object option

  bool                  ShowItemFilter(void* base, void* item, const String& itnm) const;
  // apply optional item_filter and filter_start_txt, else true

  virtual void          GetImage(void* cur_sel, TypeDef* targ_typ);

  virtual void          BuildCategories(); // for types that support categories
  virtual void          BuildChooser(taiItemChooser* ic, int view = 0);
  // builds the tree

  ~taiItemPtrBase();

public slots:
  bool                  OpenChooser(); // make and then open chooser dialog
  virtual void          EditPanel() {} // used by tokens to edit -- opens panel on guy (in new tab)
  virtual void          EditDialog() {} // used by tokens to edit -- opens edit dialog
  virtual void          btnHelp_clicked() {}

protected:
  void*                 m_sel; // current value
  QAbstractButton*      m_but;
  QToolButton*          btnEdit; // only for tokens
  QToolButton*          btnHelp; // typically for non-tokens, ex Type, Method, etc.
  TypeDef*              targ_typ;
  String_Array*         cats; // categories -- only created if needed
  String                null_text;
  taBase*               new1_par; // where to create new object
  String                new1_text;
  taBase*               new2_par; // where to create new object
  String                new2_text;

  virtual const String  itemTag() const {return _nilString;} // for "N: label" on button, is "N: "
  virtual const String  labelNameNonNull() const = 0; // name part of label, when obj non-null

  virtual void          BuildCategories_impl() {} // for types that support categories
  virtual void          UpdateImage(void* cur_sel);

  taiItemPtrBase(TypeDef* typ_, IDataHost* host,
                 taiData* par, QWidget* gui_parent_, int flags_ = 0,
                 const String& flt_start_txt = ""); // typ_
};

#endif // taiItemPtrBase_h
