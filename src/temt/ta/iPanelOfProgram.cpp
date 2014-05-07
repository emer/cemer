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

#include "iPanelOfProgram.h"
#include <Program>
#include <ProgCode>

#include <iProgramEditor>
#include <iTreeView>
#include <iTreeViewItem>
#include <iMainWindowViewer>
#include <ProgramToolBar>
#include <taSigLinkItr>
#include <iPanelSet>

#include <taMisc>

#include <QApplication>
#include <QKeyEvent>


iPanelOfProgram::iPanelOfProgram(taiSigLink* dl_)
:inherited(dl_)
{
  Program* prog_ = prog();
  if (prog_) {
    taiSigLink* dl = (taiSigLink*)prog_->GetSigLink();
    if (dl) {
      dl->CreateTreeDataNode(NULL, pe->items, NULL, dl->GetName());
    }
  }
  pe->items->setDefaultExpandLevels(12); // shouldn't generally be more than this
  pe->items->setDecorateEnabled(true); //TODO: maybe make this an app option
  connect(pe->items, SIGNAL(CustomExpandFilter(iTreeViewItem*, int, bool&)),
    this, SLOT(items_CustomExpandFilter(iTreeViewItem*, int, bool&)) );
}

void iPanelOfProgram::items_CustomExpandFilter(iTreeViewItem* item,
  int level, bool& expand)
{
  if (level < 1) return; // always expand root level
  // by default, expand code guys throughout, plus top-level args, vars and objs
  taiSigLink* dl = item->link();
  TypeDef* typ = dl->GetDataTypeDef();
  if((level <= 1) && (typ->InheritsFrom(&TA_ProgVar_List) ||
                      typ->InheritsFrom(&TA_ProgType_List))) return; // only top guys: args, vars
  if(typ->DerivesFrom(&TA_ProgEl_List) || typ->DerivesFrom(&TA_ProgObjList)
     || typ->DerivesFrom(&TA_Function_List))
    return;                     // expand
  if(typ->InheritsFrom(&TA_ProgEl)) {
    String mbr = typ->OptionAfter("DEF_CHILD_");
    if(!mbr.empty()) {
      MemberDef* md = typ->members.FindName(mbr);
      if(md) {
        if(md->type->InheritsFrom(&TA_ProgEl_List)) return; // expand
      }
      expand = false;           // don't expand any non-progel def childs
    }
    else {
      return;                   // ok to expand
    }
  }
  // otherwise, nada
  expand = false;
}

void iPanelOfProgram::OnWindowBind_impl(iPanelViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  // make sure the Program toolbar is created
  MainWindowViewer* mvw = itv->viewerWindow()->viewer();
  mvw->FindToolBarByType(&TA_ProgramToolBar,"Program");
}


///////////////////////////////////////////////////////////////////////
//      Program specific browser guys!

iPanelOfProgram* Program::FindMyProgramPanel() {
  if(!taMisc::gui_active) return NULL;

  BrowserSelectMe();        // select my program

  taSigLink* link = sig_link();
  if(!link) return NULL;
  taSigLinkItr itr;
  iPanelOfProgram* el;
  FOR_DLC_EL_OF_TYPE(iPanelOfProgram, el, link, itr) {
    if (el->prog() == this) {
      iPanelSet* dps = el->data_panel_set();
      if(dps) {
        dps->setCurrentPanelId(1); // this is the editor
      }
      return el;
    }
  }
  return NULL;
}

bool Program::BrowserSelectMe_ProgItem(taOBase* itm) {
  if(!taMisc::gui_active) return false;

  BrowserSelectMe();        // select my program

  taiSigLink* link = (taiSigLink*)itm->GetSigLink();
  if(!link) return false;

  iPanelOfProgram* mwv = FindMyProgramPanel();
  if(!mwv || !mwv->pe) return itm->taBase::BrowserSelectMe();

  iTreeView* itv = mwv->pe->items;
  iTreeViewItem* iti = itv->AssertItem(link);
  if(iti) {
    itv->setFocus();
    itv->clearExtSelection();
    // clear the selection first: makes sure that the select of actual item is 
    // novel and triggers whatever we want it to trigger!
    itv->setCurrentItem(NULL, 0, QItemSelectionModel::Clear);
    itv->scrollTo(iti);
    itv->setCurrentItem(iti, 0, QItemSelectionModel::ClearAndSelect);
    // make sure our operations are finished
    taiMiscCore::ProcessEvents();
    // edit ProgCode but not other ProgEls, and tab into all other items
    if(itm->InheritsFrom(&TA_ProgEl)) {
      ProgEl* pel = (ProgEl*)itm;
      if(pel->edit_move_after > 0) {
        QCoreApplication::postEvent(itv, new QKeyEvent(QEvent::KeyPress, Qt::Key_Down,
                                                       Qt::NoModifier));
        QCoreApplication::postEvent(itv, new QKeyEvent(QEvent::KeyPress, Qt::Key_Space,
                                                       Qt::ControlModifier));
        pel->edit_move_after = 0;
      }
      else if(pel->edit_move_after < 0) {
        QCoreApplication::postEvent(itv, new QKeyEvent(QEvent::KeyPress, Qt::Key_Up,
                                                       Qt::NoModifier));
        QCoreApplication::postEvent(itv, new QKeyEvent(QEvent::KeyPress, Qt::Key_Space,
                                                       Qt::ControlModifier));
        pel->edit_move_after = 0;
      }
      else {
        if(pel->InheritsFrom(&TA_ProgCode) && mwv->pe->miniEditVisible()) {
          // auto edit prog code
          QCoreApplication::postEvent(itv, new QKeyEvent(QEvent::KeyPress, Qt::Key_Space,
                                                         Qt::ControlModifier));
        }
      }
    }
    // else {
    //   // auto edit in editor non prog-els -- though this might be dangerous
    //   QCoreApplication::postEvent(itv, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab,
    //                                                  Qt::NoModifier));
    // }
  }
  return (bool)iti;
}

bool Program::BrowserExpandAll_ProgItem(taOBase* itm) {
  if(!taMisc::gui_active) return false;
  taiSigLink* link = (taiSigLink*)itm->GetSigLink();
  if(!link) return false;

  iPanelOfProgram* mwv = FindMyProgramPanel();
  if(!mwv || !mwv->pe) return itm->taBase::BrowserExpandAll();

  iTreeView* itv = mwv->pe->items;
  iTreeViewItem* iti = itv->AssertItem(link);
  if(iti) {
    itv->ExpandAllUnder(iti);
  }
  // make sure our operations are finished
  taiMiscCore::ProcessEvents();
  return (bool)iti;
}

bool Program::BrowserCollapseAll_ProgItem(taOBase* itm) {
  if(!taMisc::gui_active) return false;
  taiSigLink* link = (taiSigLink*)itm->GetSigLink();
  if(!link) return false;

  iPanelOfProgram* mwv = FindMyProgramPanel();
  if(!mwv || !mwv->pe) return itm->taBase::BrowserCollapseAll();

  iTreeView* itv = mwv->pe->items;
  iTreeViewItem* iti = itv->AssertItem(link);
  if(iti) {
    itv->CollapseAllUnder(iti);
  }
  // make sure our operations are finished
  taiMiscCore::ProcessEvents();
  return (bool)iti;
}
