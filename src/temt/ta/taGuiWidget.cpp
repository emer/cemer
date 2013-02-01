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

#include "taGuiWidget.h"
#include <taGuiWidgetHelper>
#include <taiWidgetIncrField>
#include <taiWidgetField>
#include <taiWidgetBitBox>
#include <taiWidgetComboBox>
#include <taiToggle>
#include <taGuiDialog>
#include <taiTokenPtrButton>

#include <iMainWindowViewer>
#include <iDataTableEditor>
#include <DataTable>
#include <DynEnum>

#include <taMisc>
#include <taiMisc>

#include <QUrl>
#include <QDesktopServices>



void taGuiWidget::Initialize() {
  m_helper = new taGuiWidgetHelper(this);
  tai_data = NULL;
}

void taGuiWidget::Destroy() {
  delete m_helper;
  if(tai_data)
    delete tai_data;
}

void taGuiWidget::Connect_UrlAction(QObject* src_obj, const char* src_signal) {
  static const char* slot_nm = SLOT(UrlAction());
  QObject::connect(src_obj, src_signal, m_helper, slot_nm);
}

void taGuiWidget::UrlAction() {
  if(action_url.empty()) return;
  if(action_url.startsWith("ta:")) {
    if(taiMisc::main_window)
      taiMisc::main_window->taUrlHandler(QUrl(action_url));
    // skip over middleman -- was not triggering in C++ dialogs for some reason..
  }
  else {
    QDesktopServices::openUrl(QUrl(action_url));
  }
}

void taGuiWidget::FixUrl(const String& url_tag, const String& path) {
  if(action_url.startsWith(url_tag)) {
    action_url = "ta:" + path + "." + action_url.after(url_tag);
  }
}

void taGuiWidget::GetImage() {
  if(widget_type == "DataTable") {
    iDataTableEditor* edt = (iDataTableEditor*)widget.data();
    edt->Refresh();
  }
  if(!tai_data || data.isNull()) return;
  if(widget_type == "IntField") {
    ((taiWidgetIncrField*)tai_data)->GetImage((String)*((int*)data.toPtr()));
  }
  if(widget_type == "DoubleField") {
    ((taiWidgetField*)tai_data)->GetImage((String)*((double*)data.toPtr()));
  }
  if(widget_type == "FloatField") {
    ((taiWidgetField*)tai_data)->GetImage((String)*((float*)data.toPtr()));
  }
  if(widget_type == "StringField") {
    ((taiWidgetField*)tai_data)->GetImage(*((String*)data.toPtr()));
  }
  if(widget_type == "BoolCheckbox") {
    ((taiToggle*)tai_data)->GetImage(*((bool*)data.toPtr()));
  }
  if(widget_type == "ObjectPtr") {
    String typnm = taGuiDialog::GetAttribute("type=", attributes);
    TypeDef* td;
    if(typnm.nonempty()) td = TypeDef::FindGlobalTypeName(typnm);
    if(!td) td = &TA_taOBase;
    ((taiTokenPtrButton*)tai_data)->GetImage(((taBaseRef*)data.toPtr())->ptr(), td);
  }
  if(widget_type == "HardEnum_Enum") {
    ((taiWidgetComboBox*)tai_data)->GetImage(*((int*)data.toPtr()));
  }
  if(widget_type == "HardEnum_Bits") {
    ((taiWidgetBitBox*)tai_data)->GetImage(*((int*)data.toPtr()));
  }
  if(widget_type == "DynEnum_Enum") {
    ((taiWidgetComboBox*)tai_data)->GetImage(((DynEnum*)data.toPtr())->value);
  }
  if(widget_type == "DynEnum_Bits") {
    ((taiWidgetBitBox*)tai_data)->GetImage(((DynEnum*)data.toPtr())->value);
  }
}

void taGuiWidget::GetValue() {
  if(!tai_data || data.isNull()) return;
  if(widget_type == "IntField") {
    *((int*)data.toPtr()) = (int)((taiWidgetIncrField*)tai_data)->GetValue();
  }
  if(widget_type == "DoubleField") {
    *((double*)data.toPtr()) = (double)((taiWidgetField*)tai_data)->GetValue();
  }
  if(widget_type == "FloatField") {
    *((float*)data.toPtr()) = (float)((taiWidgetField*)tai_data)->GetValue();
  }
  if(widget_type == "StringField") {
    *((String*)data.toPtr()) = ((taiWidgetField*)tai_data)->GetValue();
  }
  if(widget_type == "BoolCheckbox") {
    *((bool*)data.toPtr()) = ((taiToggle*)tai_data)->GetValue();
  }
  if(widget_type == "ObjectPtr") {
    *((taBaseRef*)data.toPtr()) = ((taiTokenPtrButton*)tai_data)->GetValue();
  }
  if(widget_type == "HardEnum_Enum") {
    ((taiWidgetComboBox*)tai_data)->GetValue(*((int*)data.toPtr()));
  }
  if(widget_type == "HardEnum_Bits") {
    ((taiWidgetBitBox*)tai_data)->GetValue(*((int*)data.toPtr()));
  }
  if(widget_type == "DynEnum_Enum") {
    ((taiWidgetComboBox*)tai_data)->GetValue(((DynEnum*)data.toPtr())->value);
  }
  if(widget_type == "DynEnum_Bits") {
    ((taiWidgetBitBox*)tai_data)->GetValue(((DynEnum*)data.toPtr())->value);
  }
//   if(widget_type == "DataTable") {
//     iDataTableEditor* edt = (iDataTableEditor*)widget;
//     edt->Refresh();
//   }
}
