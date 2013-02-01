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

#ifndef taiWidgetActions_h
#define taiWidgetActions_h 1

// parent includes:
#include <taiWidget>

// member includes:
#include <iAction_List>
#include <QMenu>

// declare all other types mentioned but not required to include:
class taiWidgetMenu;
class iActionSubMenuEl;


class TA_API taiWidgetActions : public taiWidget {
  // #VIRT_BASE common subtype for menus and menubars
  Q_OBJECT
  INHERITED(taiWidget)
  friend class taiWidgetActions_List; // hack because lists return refs to strings, not values
public:
  enum RepType { // for parameterized creation of a compatible subtype
    popupmenu,  // for a taiWidgetMenu
    buttonmenu // for a taiWidgetMenuButton
  };

  enum SelType {
    st_none =           0x00,   // no action on selection -- primarily for SubItem entries
    normal =            0x01,   // selecting the item causes an action; no persistent check mark by menu item
    normal_update =     0x81,
    radio =             0x02,   // only 1 of n items on the menu can be selected; selecting one deselects any other
    radio_update =      0x82,
    toggle =            0x04,   // item can be checked or unchecked, independent of any other item
    toggle_update =     0x84,

    use_default =       0x10,   // only for taiWidgetActions::AddAction -- inherits from the taiWidgetActions

    update =            0x80    // flag added to normal selection types, causes parent item to indicate Changed when item selected
  };

  static taiWidgetActions* New(RepType rt, int sel_type_, int font_spec_, TypeDef* typ_, IWidgetHost* host,
      taiWidget* par, QWidget* gui_parent_, int flags_ = 0, taiWidgetActions* par_menu_ = NULL); // note: this is primarily for compatibility with Qt3 version code

  SelType               sel_type;
  int                   font_spec; //taiMisc::FontSpec
  iAction_List        items;
  QWidget*              gui_parent; // needed for submenus

  int                   count() const {return items.size;} // qt compatability -- note that count won't have separators
  String                label() const; //#GET_Label
  QMenu*                menu()  { return m_menu; }       // for when actions stored in a menu, otherwise NULL
  virtual void          setLabel(const String& val); // #SET_Label replaces the SetMLabel call in IV
  iAction*            curSel() const;
  void                  setCurSel(iAction* value);
  QWidget*              actionsRep(); // where actions are stored, in menu if a menu, else in Rep
  virtual void          AddSep(bool new_radio_grp = false); // add menu separator -- can also be used to create new radio groups --  won't add initial sep, or 2 separators in a row; seps don't count as taiWidgetActions
  virtual void          AddAction(iAction* act); // add the already created action
  iAction*            AddItem(
                          const String& val,
                          SelType st = use_default,
                          iAction::CallbackType ct = iAction::none,
                          const QObject *receiver = NULL,
                          const char* member = NULL,
                          const Variant& usr = _nilVariant
                        );
  iAction*            AddItem(
                          const String& val,
                          iAction::CallbackType ct,
                          const QObject *receiver,
                          const char* member,
                          const Variant& usr = _nilVariant,
                          const QKeySequence& shortcut = 0
                        ); // Note: The shortcut key sequence is global, like Ctrl-S for Save.
  iAction*            AddItemWithNumericAccel(
                          const String& val,
                          iAction::CallbackType ct,
                          const QObject *receiver,
                          const char* member,
                          const Variant& usr = _nilVariant
                        ); // Prepends &1, &2, ..., &9, &0 to the 'val' argument as a keyboard accelerator.
  iAction*            AddItem(
                          const String& val,
                          SelType st,
                          const iMenuAction* men_act,
                          const Variant& usr = _nilVariant
                        );
  iAction*            AddItem(
                          const String& val,
                          const Variant& usr
                        );

  virtual taiWidgetMenu*      AddSubMenu(const String& val, TypeDef* typ_ = NULL); // add a submenu -- this also works for toolbars, and will create a toolbar menu button

//obs  virtual iAction*       GetValue()      { return cur_sel; }
  virtual bool          GetImageByData(const Variant& usr); // for radio menus, set to item with this data; recursive
  void                  GetImageByIndex(int item);
  iAction*            FindActionByData(const Variant& usr); // find 1st action with this data, NULL if not found
  taiWidgetMenu*              FindSubMenu(const String& nm); // find specified submenu, or NULL if not found

  void                  DeleteItem(uint index); // deletes the indicated item -- deletes the gui representation as well
  virtual void          NewRadioGroup();        // start a new radio group (must also preceed first group)
  virtual void          Reset();

  iAction*            operator[](int index) const {return items.SafeEl(index);}
  taiWidgetActions(int sel_type_, int font_spec_, TypeDef* typ_, IWidgetHost* host,
      taiWidget* par, QWidget* gui_parent_, int flags_ = 0, taiWidgetActions* par_menu_ = NULL,
      bool has_menu = false, QMenu* exist_menu = NULL);
  ~taiWidgetActions();
#ifndef __MAKETA__
signals:
  void labelChanged(const char* val); //
#endif

protected:
  QActionGroup*         cur_grp; // for radio groups, current group, if any
  iAction*            cur_sel;  // selection for getting value of menu -- only used by top-level menu
  String                mlabel; // string contents of current menu label
#ifndef __MAKETA__
  QPointer<QMenu>       m_menu; // for when items add to a menu
#endif
  taiWidgetActions*   par_menu; // parent menu, if any -- many methods delegate their calls upward if there is a parent
  iActionSubMenuEl*         par_menu_el; // parent submenu element, if any
  void                  emitLabelChanged(const String& val); // #IGNORE
  USING(inherited::GetImage_impl)
  virtual bool          GetImage_impl(const Variant& usr);  // #IGNORE set to this usr item, returns false if not found -- recursive for hiermenus
  virtual void          ActionAdded(iAction* it); // add to rep, def adds to mrep, but overridden for menubutton type
  virtual void          ActionRemoving(iAction* it); // remove from rep, def removes from mrep, but overridden for menubutton type

protected slots:
  virtual void          child_triggered_toggled(iAction* act);
};


#endif // taiWidgetActions_h
