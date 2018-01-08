// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#ifndef taiWidgetItemChooser_h
#define taiWidgetItemChooser_h 1

// parent includes:
#include <taiWidget>

// member includes:

// declare all other types mentioned but not required to include:
class taiWidgetItemChooser; //
class iDialogItemChooser; //
class String_Array; //
class QAbstractButton; //
class QToolButton; //
class QTreeWidgetItem; //

typedef bool (*item_filter_fun)(void*, void*); // optional filter, spec'ed in ITEM_FILTER_xxx
typedef bool (*cust_chooser_fun)(taBase*, taiWidgetItemChooser*); // optional custom config, spec'ed in CUST_CHOOSER_xxx

taTypeDef_Of(taiWidgetItemChooser);

class TA_API taiWidgetItemChooser : public taiWidget {
  // common base for MemberDefs, MethodDefs, TypeDefs, Enums, and tokens, that use the ItemChooser
  Q_OBJECT
  INHERITED(taiWidget)
public:
  enum ItemObjType {  // used to switch on object type when objects have no common parent - pass type to QTreeWidgetItem constructor
    UNSPECIFIED_ITEM = 1000,
    STRING_ITEM,
    BASE_ITEM,
    TYPE_ITEM
  };
  
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
  ItemObjType           sel_obj_type; // used to switch on object type when objects have no common parent

  inline void*          GetAltSel() const { return alt_sel; }  // return the smart alternate selection if host provides one
  virtual bool          isValid() const {return (targ_typ);} // if all required params have been set
  virtual int           catCount() const;
  // number of categories, where supported; 0=nocat, 1+=cats
  virtual const String  catText(int index) const;
  // number of different kinds of views, ex flat vs.
  virtual int           viewCount() const {return 1;}
  // number of different kinds of views, ex flat vs. tree
  virtual const String  viewText(int index) const = 0;
  // number of different kinds of views, ex flat vs.
  virtual int           setInitialSel(void* cur_sel);
  // set the initial selection value based on available options, and return the number of tokens available in the list of options as either 0, 1 or 2, where 2 = 2 or more (precise number not given if > 2), and this INCLUDES the NULL option if available -- this is called in UpdateImage with the cur_sel as the arg, passed in from the current selection -- if there are no items or one item, the default selection can be overridden from cur_sel -- the flgPreferNull flag specifies the nature of this choice

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
  virtual int           GetDefaultView() { return 0; }

  virtual void          BuildCategories(); // for types that support categories
  virtual void          BuildChooser(iDialogItemChooser* ic, int view = 0);
  // builds the tree
  virtual void          SetTitleText(String title);

  ~taiWidgetItemChooser();

public slots:
  bool                  OpenChooser(); // make and then open chooser dialog
  virtual void          EditPanel() {} // used by tokens to edit -- opens panel on guy (in new tab)
  virtual void          EditDialog() {} // used by tokens to edit -- opens edit dialog
  virtual void          btnHelp_clicked() {}

protected:
  void*                 m_sel; // current value
  void*                 alt_sel; // the chooser can provide this so that some sensible item is selected rather than NULL when the chooser list appears
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
  String                title_text;

  virtual const String  itemTag() const {return _nilString;} // for "N: label" on button, is "N: "
  virtual const String  labelNameNonNull() const = 0; // name part of label, when obj non-null

  virtual void          BuildCategories_impl() {} // for types that support categories
  virtual void          UpdateImage(void* cur_sel);

  taiWidgetItemChooser(TypeDef* typ_, IWidgetHost* host,
                 taiWidget* par, QWidget* gui_parent_, int flags_ = 0,
                 const String& flt_start_txt = "", int button_width = -1); // typ_
};

#endif // taiWidgetItemChooser_h
