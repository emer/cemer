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

#include "taiDataDelegate.h"
#include <taiData>
#include <taiEditorWidgetsOfClass>
#include <taiField>
#include <taiMember>

#include <QHBoxLayout>
#include <QTableView>
#include <QLineEdit>
#include <QPainter>
#include <QTextEdit>
#include <QKeyEvent>


taiDataDelegate::taiDataDelegate(taiEditorWidgetsOfClass* edh_)
{
  edh = edh_;
  m_dat_row = -1;
  connect(this, SIGNAL(closeEditor(QWidget*,
    QAbstractItemDelegate::EndEditHint)),
    this, SLOT(this_closeEditor(QWidget*,
    QAbstractItemDelegate::EndEditHint)) );
}

QWidget* taiDataDelegate::createEditor(QWidget* parent,
    const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  MemberDef* md = NULL;
  taBase* base = NULL;
  if (IndexToMembBase(index, md, base)) {
    if (md->im == NULL) goto exit; // shouldn't happen
    // we create a wrap widget for many of these guys, mostly so that smaller
    // guys like Combo don't try to be stretched the whole way
    bool wrap = true;           // NOTE: wrap is always true!!
    QWidget* rep_par = (wrap) ?  new QWidget(parent) : parent;

    dat = md->im->GetDataRep(edh, NULL, rep_par);
    m_dat_row = index.row();
    dat->SetBase(base);
    dat->SetMemberDef(md);
    rep = dat->GetRep(); // note: rep may get replaced by rep_par
    // color stuff
    // by default, the table color shines through widget, which is weird
    // so we will shut that off if it isn't merely a container widget
    String cn(rep->metaObject()->className());
//TODO: FIGURE THIS OUT!!! wasn't any of these:
    if (cn != "QWidget") {
      //rep->setAutoFillBackground(false);
      //rep->setAttribute(Qt::WA_NoSystemBackground);
      //rep->setAttribute(Qt::WA_OpaquePaintEvent);
    }
    if (wrap) {
      hbl = new QHBoxLayout(rep_par);
      hbl->setMargin(0);
      hbl->setSpacing(0);
      hbl->addWidget(rep);
      // some controls do better without stretch
      if (!(dynamic_cast<taiField*>((taiData*)dat)))
        hbl->addStretch();
      rep = rep_par;
    }
    connect(rep, SIGNAL(destroyed(QObject*)),
      dat, SLOT(deleteLater()) );

    EditorCreated(parent, rep, option, index);
    return rep;
  }
exit:
  return inherited::createEditor(parent, option, index);
}

void taiDataDelegate::EditorCreated(QWidget* parent, QWidget* editor,
    const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  // resize the column to accommodate the controls
  // (this seems the only place that works -- DataDelegate::sizeHint()
  //  gets called *before* createEditor() so we don't know its
  //  size yet at that point; therefore, we hackily do it here)
  //  sh = editor->sizeHint();
  sh = editor->minimumSizeHint(); // use min b/c sometimes it gets too big..
  QTableView* tv = NULL;
  QWidget* tv_candidate = parent;
  while (tv_candidate) {
    tv = qobject_cast<QTableView*>(tv_candidate);
    if (tv) break;
    tv_candidate = tv_candidate->parentWidget();
  }
  if (tv) {
    if (tv->columnWidth(1) < sh.width())
      tv->setColumnWidth(1, sh.width());
  }
}

bool taiDataDelegate::eventFilter(QObject *object, QEvent *event)
{
  QWidget *editor = qobject_cast<QWidget*>(object);
  if (!editor)
      return false;
  if (event->type() == QEvent::KeyPress) {
    switch (static_cast<QKeyEvent *>(event)->key()) {
/*  case Qt::Key_Tab:
      emit commitData(editor);
      emit closeEditor(editor, QAbstractItemDelegate::EditNextItem);
      return true;
    case Qt::Key_Backtab:
      emit commitData(editor);
      emit closeEditor(editor, QAbstractItemDelegate::EditPreviousItem);
      return true;*/
    case Qt::Key_Enter:
    case Qt::Key_Return:
      if (qobject_cast<QTextEdit*>(editor))
          return false; // don't filter enter key events for QTextEdit
      // We want the editor to be able to process the key press
      // before committing the data (e.g. so it can do
      // validation/fixup of the input).
      if (QLineEdit* e = qobject_cast<QLineEdit*>(editor))
        if (!e->hasAcceptableInput())
          return false;
      QMetaObject::invokeMethod(this, "_q_commitDataAndCloseEditor",
        Qt::QueuedConnection, Q_ARG(QWidget*, editor));
      return false;
    case Qt::Key_Escape:
      // don't commit data
      emit closeEditor(editor, QAbstractItemDelegate::RevertModelCache);
      edh->Unchanged();
      break;
    default:
      return false;
    }
    if (editor->parentWidget())
        editor->parentWidget()->setFocus();
    return true;
  }
/*  else if (event->type() == QEvent::FocusOut) {
    if (!editor->isActiveWindow() || (QApplication::focusWidget() != editor)) {
      QWidget *w = QApplication::focusWidget();
      while (w) { // don't worry about focus changes internally in the editor
        if (w == editor)
            return false;
        w = w->parentWidget();
      }
      // The window may lose focus during an drag operation.
      // i.e when dragging involves the taskbar on Windows.
      if (QDragManager::self() && QDragManager::self()->object != 0)
        return false;
      // Opening a modal dialog will start a new eventloop
      // that will process the deleteLater event.
      if (QApplication::activeModalWidget()
        && !QApplication::activeModalWidget()->isAncestorOf(editor)
        && qobject_cast<QDialog*>(QApplication::activeModalWidget()))
        return false;
      emit commitData(editor);
      emit closeEditor(editor, NoHint);
    }
  } */
  else if (event->type() == QEvent::ShortcutOverride) {
    if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Escape) {
      event->accept();
      return true;
    }
  }
  return false;
}

void taiDataDelegate::GetImage() const {
  if (!dat) return;
  edh->Updating(true);
  if (dat->mbr) { // has member (typical case)
    dat->mbr->im->GetImage(dat, dat->Base());
  } else { // no mbr, typically an inline taBase, esp for userdata
    dat->GetImage_(dat->Base());
  }
  edh->Updating(false);
  edh->Unchanged();
}

void taiDataDelegate::GetValue() const {
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
  base->UpdateAfterEdit(); // call UAE on item bases because won't happen elsewise!
  // update text of the cell, otherwise it usually ends up stale!
  edh->GetImage_Item(m_dat_row);
  edh->Unchanged();
}

void taiDataDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
  // skip, and just paint background if an editor is in place
  if (dat && (m_dat_row == index.row())) {
    drawBackground(painter, option, index);
  } else { // normal, which also means interpret rich text!
    // this stuff all from qitemdelegate.cpp
    QStyleOptionViewItemV4 opt = setOptions(index, option); // or V2 for Qt4.2
    const QStyleOptionViewItemV2 *v2 = qstyleoption_cast<const QStyleOptionViewItemV2 *>
      (&option);
    opt.features = v2 ? v2->features
      : QStyleOptionViewItemV2::ViewItemFeatures(QStyleOptionViewItemV2::None);
    const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>
      (&option);
    opt.locale = v3 ? v3->locale : QLocale();
    opt.widget = v3 ? v3->widget : 0;

    // draw rich text:
    painter->save();

    drawBackground(painter, option, index);

    QRect disp_rect = option.rect;

    // this does the rich text interp
    QTextDocument doc;
    doc.setHtml( index.data().toString() );
    painter->setClipRect(disp_rect);
    painter->translate(disp_rect.x(), disp_rect.y()+2); // add a little border
    doc.drawContents(painter);
    painter->restore();

    drawFocus(painter, opt, disp_rect);
  }
}

void taiDataDelegate::rep_destroyed(QObject* rep) {
}

QSize taiDataDelegate::sizeHint(const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
  QSize rval = inherited::sizeHint(option, index);
  if (dat) {
    rval = rval.expandedTo(sh);
  }
  return rval;
}

void taiDataDelegate::setEditorData(QWidget* editor,
    const QModelIndex& index) const
{
  if (!dat) return;
  GetImage();
}

void taiDataDelegate::setModelData(QWidget* editor,
  QAbstractItemModel* model, const QModelIndex& index) const
{
  if (!dat) return;
  GetValue();
//note: -- testing has indicated that this call only happens once
// so we invalidate it now, so the cell paints properly
  dat = NULL;
  m_dat_row = -1;
}

void taiDataDelegate::this_closeEditor(QWidget* /*editor*/,
    QAbstractItemDelegate::EndEditHint /*hint*/)
{
  // get rid of dat info, so we paint
  dat = NULL;
  m_dat_row = -1;
}
