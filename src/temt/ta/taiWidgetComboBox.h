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

#ifndef taiWidgetComboBox_h
#define taiWidgetComboBox_h 1

// parent includes:
#include <taiWidget>

// member includes:

// declare all other types mentioned but not required to include:
class iComboBox; //


class TA_API taiWidgetComboBox : public taiWidget {
  Q_OBJECT
public:
  inline iComboBox*     rep() const { return (iComboBox*)(QWidget*)m_rep; }

  taiWidgetComboBox(TypeDef* typ_, IWidgetHost* host, taiWidget* par, QWidget* gui_parent_, int flags_ = 0);
  taiWidgetComboBox(bool is_enum, TypeDef* typ_, IWidgetHost* host, taiWidget* par,
    QWidget* gui_parent_, int flags_ = 0); // treats typ as enum, and fills values

  void          AddItem(const String& val, const QVariant& userData = QVariant()); // add an item to the list
  void          Clear(); //clears all items (only needed if refilling list)
  void          RemoveItemByData(const QVariant& userData); // remove item with indicated data
  void          RemoveItemByText(const String& val); // remove item with indicated text

  void          GetImage(int itm);  // set to this item number
  void          GetValue(int& itm) const;

  void          SetEnumType(TypeDef* enum_typ, bool force = false); // sets a new enum type
  void          GetEnumImage(int enum_val);  // set to item number corresponding to this enum value, enum mode only
  void          GetEnumValue(int& enum_val) const; // set to enum value corresponding to item number, enum mode only

#ifndef __MAKETA__
signals:
  void          itemChanged(int itm); // for use by non-IWidgetHost users, forwards chkbox signal
#endif

protected:
  override void         GetImageVar_impl(const Variant& val)
    {if (m_is_enum) GetEnumImage(val.toInt()); else GetImage(val.toInt());}
  override void         GetValueVar_impl(Variant& val) const
    {int i; if (m_is_enum) GetEnumValue(i); else GetValue(i); val = i;}

private:
  bool          m_is_enum;
  void          Initialize(QWidget* gui_parent_, bool is_enum = false);
};

#endif // taiWidgetComboBox_h
