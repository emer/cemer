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

#include "SelectEditDelegate.h"
#include <SelectEdit>
#include <taiField>
#include <taiMember>
#include <taiEditTableWidget>

#include <taiEditorWidgetsOfSelectEditFast>
#include <taiDataDelegate>

#include <QTableWidget>
#include <QHBoxLayout>



QTableWidgetItem* /*SelectEditDelegate::*/ItemFromIndex(QTableWidget* tw,
  const QModelIndex& index)
{
  QTableWidgetItem* rval = tw->item(index.row(), index.column());
  return rval;
}

SelectEditDelegate::SelectEditDelegate(SelectEdit* sele_,
  taiEditorWidgetsOfSelectEditFast* sedh_)
:inherited(sedh_)
{
  sele = sele_;
  sedh = sedh_;
}

QWidget* SelectEditDelegate::createEditor(QWidget* parent,
    const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QWidget* rval = inherited::createEditor(parent, option, index);

  if(emi && emi->is_numeric) {
    MemberDef* psmd = TA_EditMbrItem.members.FindName("param_search");
    if(psmd) {
      ps_dat = psmd->im->GetDataRep(edh, NULL, rep);
      ps_dat->SetBase(emi);
      ps_dat->SetMemberDef(psmd);
      ps_rep = ps_dat->GetRep(); // note: rep may get replaced by rep_par

      hbl->addWidget(ps_rep);
      // some controls do better without stretch
      if (!(dynamic_cast<taiField*>((taiData*)ps_dat)))
        hbl->addStretch();
      connect(ps_rep, SIGNAL(destroyed(QObject*)),
              ps_dat, SLOT(deleteLater()) );
    }
  }

  QSize sz(rval->size());
  if (sz.width() > sedh->tw->columnWidth(1)) {
    sedh->tw->setColumnWidth(1,sz.width());
  }
  return rval;
}

void SelectEditDelegate::GetImage() const {
  if (!dat) return;
  edh->Updating(true);
  if (dat->mbr) { // has member (typical case)
    dat->mbr->im->GetImage(dat, dat->Base());
  } else { // no mbr, typically an inline taBase, esp for userdata
    dat->GetImage_(dat->Base());
  }
  if(ps_dat) {
    if (ps_dat->mbr) { // has member (typical case)
      ps_dat->mbr->im->GetImage(ps_dat, ps_dat->Base());
    } else { // no mbr, typically an inline taBase, esp for userdata
      ps_dat->GetImage_(ps_dat->Base());
    }
  }
  edh->Updating(false);
  edh->Unchanged();
}

void SelectEditDelegate::GetValue() const {
  if (!dat) return;
  taBase* base = dat->Base(); // cache
  if (dat->mbr) { // has member (typical case)
    bool first_diff = true;
    dat->mbr->im->GetMbrValue(dat, base, first_diff);
    if (!first_diff)
      taiMember::EndScript(base);
  } else { // no mbr, typically an inline taBase, esp for userdata
    dat->GetValue_(base);
  }
  if(ps_dat) {
    taBase* ps_base = ps_dat->Base(); // cache
    if (ps_dat->mbr) { // has member (typical case)
      bool first_diff = true;
      ps_dat->mbr->im->GetMbrValue(ps_dat, ps_base, first_diff);
      if (!first_diff)
        taiMember::EndScript(ps_base);
    } else { // no mbr, typically an inline taBase, esp for userdata
      ps_dat->GetValue_(ps_base);
    }
  }
  base->UpdateAfterEdit(); // call UAE on item bases because won't happen elsewise!
  // update text of the cell, otherwise it usually ends up stale!
  edh->GetImage_Item(m_dat_row);
  edh->Unchanged();
}

bool SelectEditDelegate::IndexToMembBase(const QModelIndex& index,
    MemberDef*& mbr, taBase*& base) const
{
  QTableWidgetItem* twi = sedh->tw->item(index.row(), index.column());
  if (twi) {
    emi = dynamic_cast<EditMbrItem*>((taBase*)(twi->data(Qt::UserRole).value<ta_intptr_t>()));
    if (emi) {
      mbr = emi->mbr;
      base = emi->base;
      return true;
    }
  }
  return false;
}
