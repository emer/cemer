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

#ifndef taiWidgetBitBox_h
#define taiWidgetBitBox_h 1

// parent includes:
#include <taiWidget>
#include <iCheckBox>

// member includes:

// declare all other types mentioned but not required to include:
class QHBoxLayout; // 


class TA_API iBitCheckBox: public iCheckBox { // #IGNORE specialized checkbox for the taiWidgetBitBox class
  Q_OBJECT
  INHERITED(iCheckBox)
public:
  int           val;
  bool          auto_apply;
  iBitCheckBox(bool auto_apply, int val_, String label, QWidget* parent);

public slots:
  void          this_clicked(bool on);

#ifndef __MAKETA__
signals:
  void          clickedEx(iBitCheckBox* sender, bool on);
#endif
};

class TA_API taiWidgetBitBox : public taiWidget { // supports enums that are bit fields
  Q_OBJECT
public:
  int                   no_show; // any bits that should be temporarily hidden
  int                   no_edit; // any bits that should be set readonly

  inline QWidget*       rep() const { return (QWidget*)m_rep; }
  bool                  fillHor() {return true;} // override

  taiWidgetBitBox(TypeDef* typ_, IWidgetHost* host, taiWidget* par, QWidget* gui_parent_, int flags_ = 0);
  taiWidgetBitBox(bool is_enum, TypeDef* typ_, IWidgetHost* host, taiWidget* par,
    QWidget* gui_parent_, int flags_ = 0); // treats typ as enum, and fills values

  virtual void  SetEnumType(TypeDef* enum_typ, bool force = false); // sets a new enum type
  virtual void  AddBoolItem(bool auto_apply, String name, int val,
     const String& desc = _nilString, bool bit_ro = false); // add an item to the list
  virtual void  Clear();                                    // remove all existing items

  void          GetImage(int val);  // set to this value, according to bit fields
  void          GetValue(int& val) const;

  void*                 m_par_obj_base;
  // if GetImage/Value is called by a taiMember, it will set this to point to parent object's base
  QHBoxLayout*  lay; //#IGNORE

public slots:
  void          bitCheck_clicked(iBitCheckBox* sender, bool on); // #IGNORE

#ifndef __MAKETA__
signals:
  void          itemChanged(int itm); // for use by non-IWidgetHost users, forwards chkbox signal
#endif

protected:
  int           m_val; //#IGNORE
  override void         GetImageVar_impl(const Variant& val)  {GetImage(val.toInt());}
  override void         GetValueVar_impl(Variant& val) const
    {int i; GetValue(i); val = i;}
private:
  void          Initialize(QWidget* gui_parent_);
};

#endif // taiWidgetBitBox_h
