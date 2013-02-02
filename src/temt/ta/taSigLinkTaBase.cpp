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

#include "taSigLinkTaBase.h"
#include <taBase>
#include <TypeDef>
#include <MethodDef>
#include <taiWidgetMethod>
#include <taiMethod>
#include <taiWidgetActions_List>
#include <iTreeViewItem>
#include <iTreeView>
#include <taiTreeNodeTaBase>
#include <iDialogSearch>
#include <taMisc>
#include <taList_impl>
#include <taGroup_impl>
#include <taArray_impl>

taSigLinkTaBase::taSigLinkTaBase(taBase* data_, taSigLink* &link_ref_)
  : inherited((void*)data_, link_ref_)
{
}

void taSigLinkTaBase::ChildQueryEditActions_impl(const MemberDef* par_md, taiSigLink* child,
  taiMimeSource* ms,  int& allowed, int& forbidden)
{
  if (child) {
    TypeDef* td = child->GetDataTypeDef();
    if (td && td->IsActualTaBase()) {
      data()->ChildQueryEditActions(par_md, (taBase*)child->data(), ms, allowed, forbidden);
      return;
    }
  } else
    data()->ChildQueryEditActions(par_md, NULL, ms, allowed, forbidden);
}

int taSigLinkTaBase::ChildEditAction_impl(const MemberDef* par_md, taiSigLink* child,
  taiMimeSource* ms, int ea)
{
  int rval = 0;
  if (child) {
    TypeDef* td = child->GetDataTypeDef();
    if (td && td->IsActualTaBase()) {
      int rval = data()->ChildEditAction(par_md, (taBase*)child->data(), ms, ea);
      if (rval == 0)
        child->EditAction_impl(ms, ea);
      return rval;
    }
  } else
    rval = data()->ChildEditAction(par_md, NULL, ms, ea);
  return rval;
}

taiTreeNode* taSigLinkTaBase::CreateTreeDataNode_impl(MemberDef* md, taiTreeNode* nodePar,
  iTreeView* tvPar, taiTreeNode* after, const String& node_name, int dn_flags)
{
  taiTreeNode* rval = NULL;
  if (nodePar)
    rval = new taiTreeNodeTaBase(this, md, nodePar, after, node_name, dn_flags);
  else
    rval = new taiTreeNodeTaBase(this, md, tvPar, after, node_name, dn_flags);
  return rval;
}

String taSigLinkTaBase::GetColText(const KeyString& key, int itm_idx) const {
  return data()->GetColText(key, itm_idx);
}

const QVariant taSigLinkTaBase::GetColData(const KeyString& key, int role) const {
  return data()->GetColData(key, role);
}

String taSigLinkTaBase::GetTypeDecoKey() const {
  return data()->GetTypeDecoKey();
}

String taSigLinkTaBase::GetStateDecoKey() const {
  return data()->GetStateDecoKey();
}

bool taSigLinkTaBase::isEnabled() const {
  return data()->GetEnabled();
}

void taSigLinkTaBase::QueryEditActions_impl(taiMimeSource* ms, int& allowed, int& forbidden) {
  data()->QueryEditActions(ms, allowed, forbidden);
}

int taSigLinkTaBase::EditAction_impl(taiMimeSource* ms, int ea) {
  return data()->EditAction(ms, ea);
}

taiSigLink* taSigLinkTaBase::ownLink() const {
  if (!m_data) return NULL;
  taBase* own = ((taBase*)m_data)->GetOwner();
  return (own) ? (taiSigLink*)own->GetSigLink() : NULL;
}

/*
void taSigLinkTaBase::fileClose() {
  data()->Close();
}

void taSigLinkTaBase::fileOpen() {
  data()->CallFun("Load");
}

void taSigLinkTaBase::fileSave() {
  data()->CallFun("Save");
}

void taSigLinkTaBase::fileSaveAs() {
  data()->CallFun("SaveAs");
}*/

/* Context Menus are filled in the following order (with indicated separators)
------
Dynamic actions
------
Standard Edit menu items (Cut, Copy, etc.)
------
#MENU_CONTEXT directive methods (tend to be base-ish methods that often appear)
------
#BUTTON directive methods (key operations for the item)
#MENU_BUTTON directive methods (each button gets a submenu)
------
Normal submenus (ex. Object Edit, etc.)
------
Browser/gui framework items
------

*/
void taSigLinkTaBase::FillContextMenu_impl(taiWidgetActions* menu) {
  inherited::FillContextMenu_impl(menu);

  TypeDef* typ = GetDataTypeDef();
  if (typ == NULL)  return;

  // add all the #MENU_CONTEXT items first, so they always appear before the nested submenus
  int cnt = 0;
  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    if (!(md->HasOption("MENU_CONTEXT"))) continue;
    if (!md->ShowMethod()) continue;
    if (md->im == NULL)  continue;
    taiWidgetMethod* mth_rep = md->im->GetMenuMethodRep(data(), NULL, NULL, NULL);
    if (mth_rep == NULL)  continue;
    if (cnt == 0) menu->AddSep();
    mth_rep->AddToMenu(menu);
    ++cnt;
  }

  // add the BUTTON and MENU_BUTTON items
  cnt = 0;
  taiWidgetActions_List ta_menus;
  String men_nm = "Misc";  //see note in taiEditorOfClass::SetCurMenuButton
  String on_nm;
  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    // we always have to extract the MENU_ON because it has side-effects
    on_nm = md->OptionAfter("MENU_ON_");
    if (on_nm.nonempty()) men_nm = on_nm;
    //check skip conditions
    if (md->HasOption("NO_MENU_CONTEXT") || md->HasOption("MENU_CONTEXT") ||
        !(md->HasOption("BUTTON") || md->HasOption("MENU_BUTTON")) )
        continue;
    if (!md->ShowMethod()) continue;
    if (md->im == NULL)  continue;
    //note: we request the Menu guy because we put it in menu even if BUTTON
    taiWidgetMethod* mth_rep = md->im->GetMenuMethodRep(data(), NULL, NULL, NULL);
    if (mth_rep == NULL)  continue;
    if (cnt == 0) menu->AddSep();

    //note: both are allowed, but we give priority to BUTTON
    if (md->HasOption("BUTTON")) {
      mth_rep->AddToMenu(menu);
    }
    else { // has to be "MENU_BUTTON"
      // create the submenus when needed, and locate -- default is last created one
      taiWidgetActions *cur_menu = ta_menus.FindName(men_nm);
      if (!cur_menu) {
        cur_menu = menu->AddSubMenu(men_nm);
        ta_menus.Add(cur_menu);
      }
      mth_rep->AddToMenu(cur_menu);
    }
    ++cnt;
  } // end BUTTON and MENU_BUTTON loop

  // now, finally, add the normal submenu items, ex. Object, Edit, etc.
  ta_menus.Reset();
  cnt = 0;
  men_nm = "Actions"; // default until/unless explicit
  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    // we always have to extract the MENU_ON because it has side-effects
    on_nm = md->OptionAfter("MENU_ON_");
    if (on_nm.nonempty()) men_nm = on_nm;
    // skip the ones we already put up
    if ((!md->HasOption("MENU")) || (md->HasOption("NO_MENU_CONTEXT") ||
      md->HasOption("MENU_CONTEXT") ||
      md->HasOption("BUTTON") || md->HasOption("MENU_BUTTON")) )
        continue;
    // standard test
    if (!md->ShowMethod()) continue;
    if (md->im == NULL)  continue;
    taiWidgetMethod* mth_rep = md->im->GetMenuMethodRep(data(), NULL, NULL, NULL);
    if (mth_rep == NULL)  continue;
    if (cnt == 0) menu->AddSep();

    // create the submenus when needed, and locate -- default is last created one
    taiWidgetActions *cur_menu = ta_menus.FindName(men_nm);
    if (!cur_menu) {
      cur_menu = menu->AddSubMenu(men_nm);
      ta_menus.Add(cur_menu);
    }
    mth_rep->AddToMenu(cur_menu);
    ++cnt;
  } // end normal submenu items

}

TypeDef* taSigLinkTaBase::GetDataTypeDef() const {
  return data()->GetTypeDef();
}

bool taSigLinkTaBase::GetIcon(int bmf, int& flags_supported, QIcon& ic) {
  const QPixmap* pm = data()->GetDataNodeBitmap(bmf, flags_supported);
  if (pm) {
    QIcon tmp(*pm);
    ic = tmp;
    return true;
  } else return false;
}

String taSigLinkTaBase::GetName() const {
  return data()->GetName();
}

String taSigLinkTaBase::GetDisplayName() const {
  MemberDef* md = GetDataMemberDef();
  if (md) return md->name;
  else    return data()->GetDisplayName();
}

bool taSigLinkTaBase::HasChildItems() {
  // we only search up until we can say yes...
  MemberSpace* ms = &GetDataTypeDef()->members;
  for (int i = 0; i < ms->size; ++ i) {
    MemberDef* md = ms->FastEl(i);
    if (ShowMember(md, TypeItem::SC_TREE)) return true;
  }
  return false;
}

int taSigLinkTaBase::checkConfigFlags() const {
  return (data()->baseFlags() & taBase::INVALID_MASK);
}

static bool IsHit(const String_PArray& targs, const String_PArray& kicks, String& p,
                  bool ci) {
  if(ci) {
    for (int i = 0; i < kicks.size; ++i) {
      if (p.contains_ci(kicks[i])) return false;
    }
  }
  else {
    for (int i = 0; i < kicks.size; ++i) {
      if (p.contains(kicks[i])) return false;
    }
  }

  String targ;
  String px = p;
  // px: highlighted version; we progressively put in placeholders for the
  // font start/end tags, which will split up partial overlapping strings
  // so we don't generate invalid html (some composite searches won't get full
  // highlighting)
  bool rval = false;
  for (int i = 0; i < targs.size; ++i) {
    targ = targs[i];
    int pos;
    if(ci)
      pos = p.index_ci(targ);
    else
      pos = p.index(targ);
    if (pos >= 0) {
      rval = true;
      if(ci)
        pos = px.index_ci(targ);
      else
        pos = px.index(targ);
      if (pos >= 0) { // can still highlight in already highlighted version
        px = px.left(pos) + "~~~~" +
          px.at(pos, targ.length()) + "```" +
          px.from(pos + targ.length());
      }
    }
  }
  if (rval) {
    px.gsub("~~~~", "<font color=red>");
    px.gsub("```", "</font>");
    p = px;
  }
  return rval;
}

static void AddHit(int item_type, const String& probedx, String& hits) {
  if (hits.nonempty()) hits += "<br>";
  switch (item_type) {
  case iDialogSearch::SO_OBJ_NAME: hits += "name:"; break;
  case iDialogSearch::SO_OBJ_TYPE: hits += "type:"; break;
  case iDialogSearch::SO_OBJ_DESC: hits += "desc:"; break;
  case iDialogSearch::SO_MEMB_NAME: hits += "memb name:"; break;
  case iDialogSearch::SO_MEMB_VAL: hits += "memb val:"; break;
  default: break; // huh?
  }
  hits += probedx;
}

void taSigLinkTaBase::SearchStat(taBase* tab, iDialogSearch* sd, int level) {
  if (sd->stop()) return; // user hit stop
  const String_PArray& targs = sd->targets();
  const String_PArray& kicks = sd->kickers();
  String probed;
  String hits; // cumulative hits guy
  int n = 0; // hits counter, to know to call Add
  bool ci = true;               // case independent
  if(sd->options() & iDialogSearch::SO_MATCH_CASE)
    ci = false;

  // NAME
  int item_type = iDialogSearch::SO_OBJ_NAME;
  if (sd->options() & item_type) {
    probed = tab->GetName();
    if (IsHit(targs, kicks, probed, ci))
      {++n; AddHit(item_type, probed, hits);}
  }

  // TYPE
  item_type = iDialogSearch::SO_OBJ_TYPE;
  if (sd->options() & item_type) {
    probed = tab->GetTypeDef()->name;
    if (IsHit(targs, kicks, probed, ci)) {
      ++n; AddHit(item_type, probed, hits);
    }
    else {
      if(sd->options() & iDialogSearch::SO_TYPE_DESC) {
        probed = tab->GetTypeDef()->desc;
        if (IsHit(targs, kicks, probed, ci)) {
          ++n; AddHit(item_type, probed, hits);
        }
      }
    }
  }

  // DESC
  item_type = iDialogSearch::SO_OBJ_DESC;
  if (sd->options() & item_type) {
    probed = tab->GetColText(taBase::key_desc);
    if (IsHit(targs, kicks, probed, ci)) {
      ++n; AddHit(item_type, probed, hits);
    }
    else {
      probed = tab->GetDisplayName(); // include display name
      if (IsHit(targs, kicks, probed, ci)) {
        ++n; AddHit(item_type, probed, hits);
      }
    }
  }

  TypeDef* td = tab->GetTypeDef();
  // MEMB NAME (note: NO_SEARCH not applicable to name search)
  item_type = iDialogSearch::SO_MEMB_NAME;
  if (sd->options() & item_type) {
    for(int m=0;m<td->members.size;m++) {
      MemberDef* md = td->members[m];
      if (!(sd->options() & iDialogSearch::SO_ALL_MEMBS) && !md->ShowMember()) continue;
      probed = md->name;
      if (IsHit(targs, kicks, probed, ci)) {
        ++n; AddHit(item_type, probed, hits);
      }
      else {
        if(sd->options() & iDialogSearch::SO_TYPE_DESC) {
          probed = md->desc;
          if (IsHit(targs, kicks, probed, ci)) {
            ++n; AddHit(item_type, probed, hits);
          }
        }
      }
    }
  }


  // MEMB VALUE

  // first pass: "value" members:
  // non-ptrs, non-owned taBase values, owned no-browse taBase values
  item_type = iDialogSearch::SO_MEMB_VAL;
  if (sd->options() & item_type) {
    for(int m=0;m<td->members.size;m++) {
      MemberDef* md = td->members[m];
      if (!(sd->options() & iDialogSearch::SO_ALL_MEMBS) && !md->ShowMember()) continue;
      if (md->is_static) continue;
      if (md->HasOption("NO_SEARCH")) continue;
      if (md->type->IsNotPtr()) {
        // a list or greater is never a "value"
        if (md->type->InheritsFrom(TA_taList_impl)) continue;
        if (md->type->IsActualTaBase()) {
          taBase* obj = (taBase*)md->GetOff(tab);
          taBase* own = obj->GetOwner(); //note: embedded obj must be owned by par
          // non-owned values can't be browsed, and must be handled inline, below
          if (own) {
            // if owned, could browsable child -- we do that as recursive
            if (md->ShowMember(TypeItem::USE_SHOW_GUI_DEF,
                               TypeItem::SC_TREE)) continue;
          }
        }
        // have to force getting an inline value, since default is often the path
                probed = md->type->GetValStr(md->GetOff(tab), tab, md, (TypeDef::StrContext)0, true); // force_inline
                if (IsHit(targs, kicks, probed, ci))
          {++n; AddHit(item_type, probed, hits);}
      }
      else if(md->type->IsPointer()) {
        // if a pointer, treat it as a value and go for it!
                  probed = md->type->GetValStr(md->GetOff(tab), tab, md, (TypeDef::StrContext)0, true); // force_inline
                  if (IsHit(targs, kicks, probed, ci))
          {++n; AddHit(item_type, probed, hits);}
      }
    }
  }
  // end of this guy, so if he hit, then output
  if (n > 0) {
    String headline = tab->GetColText(taBase::key_disp_name) +
        " (" + tab->GetTypeDef()->name + ")";
    String href = "ta:" + tab->GetPath();
    String path_long = tab->GetPathNames();
    String desc = tab->GetColText(taBase::key_desc);
    sd->AddItem(headline, href, desc, hits, path_long, level, n);
  }


  String def_child = td->OptionAfter("DEF_CHILD_");
  // browsable taBase members
  // second pass: recurse
  for(int m=0; m<td->members.size;m++) {
    if (sd->stop()) return; // user hit stop
    MemberDef* md = td->members[m];
    if (!(sd->options() & iDialogSearch::SO_ALL_MEMBS) && !md->ShowMember()) {
      // def children are excluded from show, but should not be from search!!
      if (md->name != def_child)
        continue;
    }
    if (md->is_static) continue;
    if (md->HasOption("NO_SEARCH")) continue;

    taBase* chld = NULL;
    // we are only handling owned browsable taBase guys here
    if (md->type->IsPtrPtr() || !md->type->IsTaBase() )
      continue;
    // if guy is not a list or greater, must be browsable taBase
    if (!md->type->InheritsFrom(TA_taList_impl)) {
      if (!(sd->options() & iDialogSearch::SO_ALL_MEMBS) &&            
          !md->ShowMember(TypeItem::USE_SHOW_GUI_DEF,
          TypeItem::SC_TREE)) continue;
    }

    if (md->type->IsNotPtr()) {
        chld = static_cast<taBase*>(md->GetOff(tab));
    }
    else { // must be == 1
      taBase** pchld = static_cast<taBase**>(md->GetOff(tab));
      if (!pchld || !(chld = *pchld)) continue;
      if (chld->GetOwner() != tab) continue;
    }
    // note: chld will have a value by here
    taSigLinkTaBase::SearchStat(chld, sd, level+1);
  }

  // only for Lists:
  if (td->InheritsFrom(&TA_taList_impl)) {
    taList_impl* tal = static_cast<taList_impl*>(tab);
    for(int i=0; i<tal->size; i++) {
      if (sd->stop()) return; // user hit stop
      taBase* itm = (taBase*)tal->FastEl_(i);
      if(!itm) continue;
       // for guys we own (not links; prevents loops)
      if (itm->GetOwner() != tab) continue;
      taSigLinkTaBase::SearchStat(itm, sd, level+1);
    }

    // only for Groups:
    if (td->InheritsFrom(&TA_taGroup_impl)) {
      taGroup_impl* tag = static_cast<taGroup_impl*>(tab);
      for(int i=0; i < tag->gp.size; i++) {
        if (sd->stop()) return; // user hit stop
        taGroup_impl* gp = tag->gp.FastEl(i);
        if(!gp) continue;
        taSigLinkTaBase::SearchStat(gp, sd, level+1);
      }
    }
  }
}


void taSigLinkTaBase::Search(iDialogSearch* dlg) {
  taSigLinkTaBase::SearchStat(data(), dlg);
}

bool taSigLinkTaBase::ShowMember(MemberDef* md, TypeItem::ShowContext show_context) const {
  TypeDef* td = md->type;
  if (td == NULL) return false; // shouldn't happen...
  // should just be able to completely delegate to the memberdef...

  return md->ShowMember(TypeItem::USE_SHOW_GUI_DEF, show_context);
}

