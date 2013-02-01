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

#include "iBaseClipWidgetAction.h"
#include <taiSigLink>
#include <taiObjectMimeFactory>
#include <taiClipData>


iBaseClipWidgetAction::iBaseClipWidgetAction(taBase* inst_, QObject* parent)
  : inherited(parent)
{
  Init(inst_);
}

iBaseClipWidgetAction::iBaseClipWidgetAction(const QIcon & icon_, taBase* inst_,
    QObject* parent)
  : inherited(parent)
{
  Init(inst_);
  setIcon(icon_);
}

iBaseClipWidgetAction::iBaseClipWidgetAction(const String& tooltip_, const QIcon & icon_,
    taBase* inst_, QObject* parent)
  : inherited(parent)
{
  Init(inst_, tooltip_);
  setIcon(icon_);
}

iBaseClipWidgetAction::iBaseClipWidgetAction(const String& text_,
    taBase* inst_, QObject* parent)
  : inherited(parent)
{
  Init(inst_);
  setText(text_);
}

iBaseClipWidgetAction::iBaseClipWidgetAction(const String& tooltip_, const String& text_,
    taBase* inst_, QObject* parent)
  : inherited(parent)
{
  Init(inst_, tooltip_);
  setText(text_);
}


void iBaseClipWidgetAction::Init(taBase* inst_, String tooltip_) {
  m_inst = inst_;
  if (tooltip_.empty() && inst_) {
    tooltip_ = inst_->GetToolTip(taBase::key_type_desc);
  }
  if (tooltip_.nonempty()) {
    setToolTip(tooltip_);
  }
  if (inst_) {
    String statustip = inst_->statusTip(); // no key
    if (statustip.nonempty())
      setStatusTip(statustip);
  }
}

QMimeData* iBaseClipWidgetAction::mimeData() const {
  taiClipData* rval = NULL;
  if (m_inst) {
    taiSigLink* link = (taiSigLink*)m_inst->GetSigLink();
    if (link) {
      // get readonly clip data -- we don't know if dragging or not, so we always say we are
      taBase* obj = link->taData();
      if (obj) {
        taiObjectMimeFactory* mf = taiObjectMimeFactory::instance();
        rval = new taiClipData(
          (taiClipData::EA_SRC_COPY | taiClipData::EA_SRC_DRAG | taiClipData::EA_SRC_READONLY));
        mf->AddSingleObject(rval, obj);
      }
    }
  }
  return rval;
}

QStringList iBaseClipWidgetAction::mimeTypes() const {
 //NOTE: for dnd to work, we just permit our own special mime type!!!
  QStringList rval;
  rval.append(taiObjectMimeFactory::tacss_objectdesc);
  return rval;
}


