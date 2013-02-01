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

#include "UserDataDelegate.h"
#include <UserDataItemBase>
#include <taiEditorWidgetsOfUserData>
#include <taiMember>
#include <taiField>
#include <taiPolyData>

#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>



UserDataDelegate::UserDataDelegate(UserDataItem_List* udil_,
  taiEditorWidgetsOfUserData* uddh_)
:inherited(uddh_)
{
  udil = udil_;
  uddh = uddh_;
}

QWidget* UserDataDelegate::createEditor(QWidget* parent,
    const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  MemberDef* md = NULL;
  taBase* base = NULL;
  if (IndexToMembBase(index, md, base)) {
    if (md) {
      if (md->im == NULL) goto exit; // shouldn't happen
      // we create a wrap widget for many of these guys, mostly so that smaller
      // guys like Combo don't try to be stretched the whole way
      bool wrap = true;
      QWidget* rep_par = (wrap) ?  new QWidget(parent) : parent;

      dat = md->im->GetDataRep(edh, NULL, rep_par);
      dat->SetMemberDef(md);
      rep = dat->GetRep(); // note: rep may get replaced by rep_par
      if (wrap) {
        QHBoxLayout* hbl = new QHBoxLayout(rep_par);
        hbl->setMargin(0);
        hbl->setSpacing(0);
        hbl->addWidget(rep);
        // some controls do better without stretch
        if (!(dynamic_cast<taiField*>((taiData*)dat)))
          hbl->addStretch();
        rep = rep_par;
      }
    } else { // an inline taBase -- always wrap so we can stretch for longer guys
      QWidget* rep_par = new QWidget(parent);
      QHBoxLayout* hbl = new QHBoxLayout(rep_par);
      hbl->setMargin(0);
      hbl->setSpacing(0);
      dat = taiPolyData::New(true, base->GetTypeDef(), edh, NULL,
        rep_par);
      rep = dat->GetRep();
      hbl->addWidget(rep);
      rep = rep_par;
    }
    dat->SetBase(base);
    m_dat_row = index.row();

    connect(rep, SIGNAL(destroyed(QObject*)),
      dat, SLOT(deleteLater()) );
    EditorCreated(parent, rep, option, index);
    return rep;
  }
exit:
  return inherited::createEditor(parent, option, index);

}

bool UserDataDelegate::IndexToMembBase(const QModelIndex& index,
    MemberDef*& mbr, taBase*& base) const
{
  QTableWidgetItem* twi = uddh->tw->item(index.row(), index.column());
  if (twi) {
    UserDataItemBase* item = dynamic_cast<UserDataItemBase*>(
      (taBase*)(twi->data(Qt::UserRole).value<ta_intptr_t>()));
    base = item; // the item itself is the base
    if (item) {
      if (item->isSimple()) {
        mbr = item->FindMember("value"); // better be found!
      } else { // complex
      //nothing
      }
      return true;
    }
  }
  return false;
}

