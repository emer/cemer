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

#include "taiWidget.h"
#include <IWidgetHost>
#include <iLabel>
#include <taiWidgetDeck>
#include <taiSigLink>
#include <taiMisc>

#include <taMisc>


// NOTE: this is for maketa only
taiWidget::taiWidget()
  : QObject()
  , typ(0)
  , mbr(0)
  , host(0)
  , orig_val()
  , m_highlight(false)
  , m_lighten(false)
  , m_visible(true)
  , m_rep(0)
  , m_label()
  , mparent(0)
  , mflags(0)
  , m_base()
{
}

taiWidget::taiWidget(TypeDef* typ_, IWidgetHost* host_, taiWidget* parent_, QWidget*, int flags_)
  : QObject()
  , typ(typ_)
  , mbr(0)
  , host(host_)
  , orig_val()
  , m_highlight(false)
  , m_lighten(false)
  , m_visible(true)
  , m_rep(0)
  , m_label()
  , mparent(0) // must be initialized before calling setParent!
  , mflags(flags_)
  , m_base()
{
  setParent(parent_);
}

taiWidget::~taiWidget() {
  Destroy();
}

void taiWidget::Destroy() {
  setParent(NULL);
  host = NULL;
  m_rep = NULL;
}

void taiWidget::applyNow() {
  // we send this up via parents, to let them trap first, ex taiWidgetToggle
  if (mparent)
    mparent->applyNow();
  else if (host) {
    //note: we need to use the Async because things like seledit rebuild
    // during the call, clobbering the ctrl while its sig/slot stuff still ongoing
    if(host->isModal()) {
      host->Changed();          // just tell it that things have changed
    }
    else {                      // do full apply if non-modal
      host->Apply_Async();
    }
  }
}

taBase* taiWidget::Base() const {
  //note: not typically overridden
  if (m_base) return m_base;
  else if (mparent) return mparent->ChildBase();
  else if (host) return host->Base();
  else return NULL;
}

void taiWidget::SetBase(taBase* base_) const {
  m_base = base_;
}

void taiWidget::SigEmit(taiWidget* chld) {
  // ignore completely if not yet constructed
  if (!isConstructed()) return;

  // don't do anything ourselves, but notify host and our parent..
  // if we have a parent, delegate notification to it, otherwise inform the host
  // we might end up committing suicide here so guard..
  QPointer<taiWidget> ths = this;
  if (mparent != NULL)
    mparent->SigEmit(this);
  else if (host)
    host->Changed();

  if(!ths) return;              // above could have done it

  SigEmit_impl(chld);
  if (!chld)
    emit SigEmitNotify(this);
}

int taiWidget::defSize() const {
  if (mparent != NULL)
    return mparent->defSize();
  else return taiM->ctrl_size;
}

void taiWidget::Delete() {
//NOTE: to extend this method, delete your own additional Qt gui thingies, then call us
//DO NOT CALL INHERITED THEN DO MORE STUFF YOURSELF
  if (m_rep) {
    QWidget* tmp = m_rep;
    m_rep = NULL;
    delete tmp;
  }
  if (mparent)
    setParent(NULL); // parent deletes us
  else
    delete this;
  //NO MORE CODE, WE ARE DELETED!!!
}

void taiWidget::emit_UpdateUi() {
  emit UpdateUi();
}

void taiWidget::emit_settingHighlight(bool setting) {
  emit settingHighlight(setting);
}

void taiWidget::emit_settingLighten(bool setting) {
  emit settingLighten(setting);
}

bool taiWidget::eventFilter(QObject* watched, QEvent* ev) {
  //note: we don't delete events, just look for focusin on our rep
  bool rval = inherited::eventFilter(watched, ev);
  if (ev->type() == QEvent::FocusIn) {
    SetThisAsHandler(true);
  }
  return rval;
}

bool taiWidget::isConstructed() {
  // this implementation delegates to parents, under assumption that
  // children are constructed if parents are constructed
  if (mparent != NULL)
    return mparent->isConstructed();
  else if (host != NULL)
    return (host->isConstructed());
  // if no parent or host, we assume constructed -- if needed, we would have to
  // create a variable to track this, and set it after the constructor ran (which would
  // require changes to ALL the types!)
  else return true;
}

iLabel* taiWidget::MakeLabel(const String& text, QWidget* gui_parent, int font_spec) const {
  iLabel* rval = MakeLabel(gui_parent, font_spec);
  rval->setText(text);
  return rval;
}

iLabel* taiWidget::MakeLabel(QWidget* gui_parent, int font_spec) const {
  // fs may have size+attribs, but normal case is just def attribs+ defsize
  if (font_spec == 0) font_spec = defSize();
  iLabel* rval = new iLabel(gui_parent);
  rval->setMaximumHeight(taiM->max_control_height(defSize()));
  rval->setFont(taiM->nameFont(font_spec));
  return rval;
}

QWidget* taiWidget::MakeLayoutWidget(QWidget* gui_parent) const {
  QWidget* wid = new QWidget(gui_parent);
#if defined(TA_OS_MAC) && (QT_VERSION >= 0x040300)
//  wid->setAttribute(Qt::WA_LayoutUsesWidgetRect, true);
  wid->setAttribute(Qt::WA_LayoutOnEntireRect, true);
#endif
  wid->setMaximumHeight(taiM->max_control_height(defSize()));
  //following needed to fix the squished layout issue on Mac:
  wid->setMinimumHeight(taiM->max_control_height(defSize()));
  return wid;
}

bool taiWidget::readOnly() const {
  if (mflags & flgReadOnly) return true;
  return (mparent && mparent->readOnly());
}

void taiWidget::setHighlight(bool value) {
  if (m_highlight == value) return;
  m_highlight = value;
  // if we are in a DataDeck, then it is the deck that needs to emit signal
  taiWidgetDeck* deck = dynamic_cast<taiWidgetDeck*>(mparent);
  if (deck)
    deck->emit_settingHighlight(value);
  else
    emit_settingHighlight(value);
}

void taiWidget::setLighten(bool value) {
  if (m_lighten == value) return;
  m_lighten = value;
  // if we are in a DataDeck, then it is the deck that needs to emit signal
  taiWidgetDeck* deck = dynamic_cast<taiWidgetDeck*>(mparent);
  if (deck)
    deck->emit_settingLighten(value);
  else
    emit_settingLighten(value);
}

void taiWidget::setParent(taiWidget* value) {
  if (mparent == value) return;
  if (mparent != NULL)
    mparent->ChildRemove(this);
  mparent = value;
  if (mparent != NULL)
    mparent->ChildAdd(this);

}

void taiWidget::SetRep(QWidget* val) {
  if (m_rep == val) return;

  // if not already destroying (either rep or this)
  if (m_rep != NULL) {
    m_rep->removeEventFilter(this);
  }
  m_rep = val;
  if (m_rep) {
#if defined(TA_OS_MAC) && (QT_VERSION >= 0x040300)
    m_rep->setAttribute(Qt::WA_LayoutUsesWidgetRect, true);
#endif
    m_rep->installEventFilter(this);
  }
}

void taiWidget::SetThisAsHandler(bool set_it) {
  if (host) host->SetItemAsHandler(this, set_it);
}

bool taiWidget::setVisible(bool value) {
  if (m_visible == value) return false;
  QWidget* wid = GetRep();
  QLabel* lbl = label();
  if (value) {
    if (lbl) {
      lbl->setVisible(true);
    }
    if (wid) {
      wid->setVisible(true);
    }
  } else {
    if (wid) {
      wid->setVisible(false);
    }
    if (lbl) {
      lbl->setVisible(false);
    }
  }
  m_visible = value;
  return true;
}

void taiWidget::repChanged() {
  SigEmit(NULL);
}
