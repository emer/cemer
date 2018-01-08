// Co2017ght 1995-2013, Regents of the University of Colorado,
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

#include "taiWidgetCompletionChooser.h"
#include <taList_impl>
#include <iDialogItemChooser>
#include <Completions>
#include <ProgExprBase>
#include <MemberDef>
#include <MethodDef>
#include <EnumDef>
#include <taMisc>

#include <iTreeWidgetItem>
#include <iTreeWidget>

taTypeDef_Of(Function);
taTypeDef_Of(Program);

taiWidgetCompletionChooser::taiWidgetCompletionChooser(TypeDef* typ_, IWidgetHost* host,
                                                         taiWidget* par, QWidget* gui_parent_, int flags_,
                                                         const String& flt_start_txt, int button_width)
:inherited(typ_, host, par, gui_parent_, flags_, flt_start_txt, button_width)
{
}

void taiWidgetCompletionChooser::SetCompletions(Completions* completions_info) {
  completions = completions_info;
}

void taiWidgetCompletionChooser::BuildChooser(iDialogItemChooser* item_chooser, int view) {
  //assume only called if needed
  
  if (!completions) {
    taMisc::Error("taiWidgetCompletionChooser::BuildChooser: completions object pointer not set");
    return;
  }
  dialog_item_chooser = item_chooser;
  Populate(item_chooser, completions, NULL);
}

int taiWidgetCompletionChooser::Populate(iDialogItemChooser* item_chooser, Completions* the_completions, QTreeWidgetItem* top_item)
{
  completions = the_completions;
  
  int item_count = 0;
  String display_string;
  
  // TA Objects
  for (int i = 0; i < the_completions->object_completions.size; ++i) {
    taBase* tab = the_completions->object_completions.FastEl(i);
    if (!tab)  continue;

    display_string = tab->GetName();
    if (tab->InheritsFrom(&TA_Function) || tab->InheritsFrom(&TA_Program)) {
      display_string = display_string + "()";
    }
    QTreeWidgetItem* item = item_chooser->AddItem(display_string, top_item, tab, "", 1, BASE_ITEM);

    item->setText(1, tab->GetColText(taBase::key_type));
    item->setText(2, tab->GetColText(taBase::key_desc));
    item->setText(3, tab->GetOwner()->GetColText(taBase::key_name));
    item->setText(4, tab->GetOwner()->GetColText(taBase::key_type));
    ++item_count;
  }
  
  // Members
  for (int i = 0; i < the_completions->member_completions.size; ++i) {
    MemberDef* md = the_completions->member_completions.FastEl(i);
    if (!md)  continue;
    
    display_string = md->name;
    QTreeWidgetItem* item = item_chooser->AddItem(display_string  + " (member)", top_item, md, md->desc, 1, TYPE_ITEM);
    ++item_count;
  }
  
  // Methods
  for (int i = 0; i < the_completions->method_completions.size; ++i) {
    MethodDef* md = the_completions->method_completions.FastEl(i);
    if (!md)  continue;
    
    display_string = md->name;
    display_string = display_string + "()";  // always for methods
    QTreeWidgetItem* item = item_chooser->AddItem(display_string, top_item, md, md->desc, 1, TYPE_ITEM);
   ++item_count;
  }
  
  // Enums
  for (int i = 0; i < the_completions->enum_completions.size; ++i) {
    EnumDef* ed = the_completions->enum_completions.FastEl(i);
    if (!ed)  continue;
    
    display_string = ed->name;
    QTreeWidgetItem* item = item_chooser->AddItem(display_string, top_item, ed, "", 1, TYPE_ITEM);
    ++item_count;
  }

  // Statics
  for (int i = 0; i < the_completions->static_completions.size; ++i) {
    TypeDef* td = the_completions->static_completions.FastEl(i);
    if (!td)  continue;
    
    display_string = td->name + "::";
    QTreeWidgetItem* item = item_chooser->AddItem(display_string, top_item, td, "", 1, TYPE_ITEM);
    item->setText(2, td->desc);
    ++item_count;
  }

  // Scopes
  for (int i = 0; i < the_completions->misc_completions.size; ++i) {
    TypeDef* td = the_completions->misc_completions.FastEl(i);
    if (!td)  continue;
    
    display_string = td->name + "::";
    QTreeWidgetItem* item = item_chooser->AddItem(display_string, top_item, td, "", 1, TYPE_ITEM);
    item->setText(2, td->desc);
    ++item_count;
  }

  return item_count;
}

int taiWidgetCompletionChooser::columnCount(int view) const {
  switch (view) {
    case 0:
    {
      if (completions->object_completions.size > 0) {
        return 5; // 3 plus owner_name and owner_type
      }
      else {
        return 3;
      }
      break;
    }
    default: return 0; // not supposed to happen
  }
}

const String taiWidgetCompletionChooser::headerText(int index, int view) const {
  // *** ignore view argument ***
  int member_method_count = completions->member_completions.size + completions->method_completions.size;
  
  if (member_method_count > 0) {
    switch (index) {
      case 0: return "Name";
      case 1: return "Description";
      default:
        return _nilString;
    }
  }
  else {
    switch (index) {
      case 0: return "Name";
      case 1: return "Type";
      case 2: return "Description";
      case 3: return "Owner Name";
      case 4: return "Owner Type";
      default:
        return _nilString;
    }
  }
}

const String taiWidgetCompletionChooser::labelNameNonNull() const {
//  return item()->GetDisplayName();
  return "";
}

const String taiWidgetCompletionChooser::viewText(int index) const {
  switch (index) {
    case 0: return "Flat List"; 
    default: return _nilString;
  }
}

void taiWidgetCompletionChooser::GetImage(taList_impl* base_lst, taBase* it) {
  list = base_lst;
  inherited::GetImage((void*)it, base_lst->el_base);
}

String taiWidgetCompletionChooser::GetSelectionText() {
  String selection_text;
    
  if (sel_obj_type == STRING_ITEM) {
    selection_text = (taString*)m_sel;
  }
  else if (sel_obj_type == BASE_ITEM) {
    taBase* tab = (taBase*)m_sel;
    if (tab) {
      selection_text = tab->GetName();
      if (tab->InheritsFrom(&TA_Function) || tab->InheritsFrom(&TA_Program)) {
        selection_text = selection_text + "()";
      }
    }
  }
  else if (sel_obj_type == TYPE_ITEM) {
    MethodDef* mth = dynamic_cast<MethodDef*>(GetItemType());
    if (mth) {
      selection_text = mth->name + "()";
    }
    
    MemberDef* mbr = dynamic_cast<MemberDef*>(GetItemType());
    if (mbr) {
      selection_text = mbr->name;
    }
    
    EnumDef* enm = dynamic_cast<EnumDef*>(GetItemType());
    if (enm) {
      selection_text = enm->name;
    }
  }
  return selection_text;
}
