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

#include "taiData.h"
#include <IDataHost>
#include <iLabel>
#include <taiDataDeck>
#include <taiDataLink>
#include <taiMisc>

// NOTE: this is for ta_TA.cpp only
taiData::taiData()
  : QObject()
  , typ(0)
  , mbr(0)
  , host(0)
  , orig_val()
  , mhighlight(false)
  , m_visible(true)
  , m_rep(0)
  , m_label()
  , mparent(0)
  , mflags(0)
  , m_base()
{
}

taiData::taiData(TypeDef* typ_, IDataHost* host_, taiData* parent_, QWidget*, int flags_)
  : QObject()
  , typ(typ_)
  , mbr(0)
  , host(host_)
  , orig_val()
  , mhighlight(false)
  , m_visible(true)
  , m_rep(0)
  , m_label()
  , mparent(0) // must be initialized before calling setParent!
  , mflags(flags_)
  , m_base()
{
  setParent(parent_);
}

taiData::~taiData() {
  Destroy();
}

void taiData::Destroy() {
  setParent(NULL);
  host = NULL;
  m_rep = NULL;
}

void taiData::applyNow() {
  // we send this up via parents, to let them trap first, ex taiToggle
  if (mparent)
    mparent->applyNow();
  else if (host) {
    //note: we need to use the Async because things like seledit rebuild
    // during the call, clobbering the ctrl while its sig/slot stuff still ongoing
    host->Apply_Async();
  }
}

taBase* taiData::Base() const {
  //note: not typically overridden
  if (m_base) return m_base;
  else if (mparent) return mparent->ChildBase();
  else if (host) return host->Base();
  else return NULL;
}

void taiData::SetBase(taBase* base_) const {
  m_base = base_;
}

void taiData::DataChanged(taiData* chld) {
  // ignore completely if not yet constructed
  if (!isConstructed()) return;

  // don't do anything ourselves, but notify host and our parent..
  // if we have a parent, delegate notification to it, otherwise inform the host
  // we might end up committing suicide here so guard..
  QPointer<taiData> ths = this;
  if (mparent != NULL)
    mparent->DataChanged(this);
  else if (host)
    host->Changed();

  if(!ths) return;              // above could have done it

  DataChanged_impl(chld);
  if (!chld)
    emit DataChangedNotify(this);
}

int taiData::defSize() const {
  if (mparent != NULL)
    return mparent->defSize();
  else return taiM->ctrl_size;
}

void taiData::Delete() {
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

void taiData::emit_UpdateUi() {
  emit UpdateUi();
}

void taiData::emit_settingHighlight(bool setting) {
  emit settingHighlight(setting);
}

bool taiData::eventFilter(QObject* watched, QEvent* ev) {
  //note: we don't delete events, just look for focusin on our rep
  bool rval = inherited::eventFilter(watched, ev);
  if (ev->type() == QEvent::FocusIn) {
    SetThisAsHandler(true);
  }
  return rval;
}

bool taiData::isConstructed() {
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

iLabel* taiData::MakeLabel(const String& text, QWidget* gui_parent, int font_spec) const {
  iLabel* rval = MakeLabel(gui_parent, font_spec);
  rval->setText(text);
  return rval;
}

iLabel* taiData::MakeLabel(QWidget* gui_parent, int font_spec) const {
  // fs may have size+attribs, but normal case is just def attribs+ defsize
  if (font_spec == 0) font_spec = defSize();
  iLabel* rval = new iLabel(gui_parent);
  rval->setMaximumHeight(taiM->max_control_height(defSize()));
  rval->setFont(taiM->nameFont(font_spec));
  return rval;
}

QWidget* taiData::MakeLayoutWidget(QWidget* gui_parent) const {
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

bool taiData::readOnly() const {
  if (mflags & flgReadOnly) return true;
  return (mparent && mparent->readOnly());
}

void taiData::setHighlight(bool value) {
  if (mhighlight == value) return;
  mhighlight = value;
  // if we are in a DataDeck, then it is the deck that needs to emit signal
  taiDataDeck* deck = dynamic_cast<taiDataDeck*>(mparent);
  if (deck)
    deck->emit_settingHighlight(value);
  else
    emit_settingHighlight(value);
}
void taiData::setParent(taiData* value) {
  if (mparent == value) return;
  if (mparent != NULL)
    mparent->ChildRemove(this);
  mparent = value;
  if (mparent != NULL)
    mparent->ChildAdd(this);

}

void taiData::SetRep(QWidget* val) {
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

void taiData::SetThisAsHandler(bool set_it) {
  if (host) host->SetItemAsHandler(this, set_it);
}

bool taiData::setVisible(bool value) {
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

void taiData::repChanged() {
  DataChanged(NULL);
}
