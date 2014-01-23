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

#ifndef taGuiWidget_h
#define taGuiWidget_h 1

// parent includes:
#include <taNBase>

// member includes:
#ifndef __MAKETA__
#include <QPointer>
#include <QWidget>
#endif

// declare all other types mentioned but not required to include:
class QObject; // 
class taGuiWidget_QObj; // #IGNORE
class taiWidget; //


taTypeDef_Of(taGuiWidget);

class TA_API taGuiWidget : public taNBase {
  // ##CAT_Gui manages a qt widget for the taGui system
INHERITED(taNBase)
public:
#ifndef __MAKETA__
  QPointer<QWidget>     widget; // the widget
  taiWidget*              tai_data;// gui data object for widget
#endif
  String                widget_type; // what type of widget is this?  types defined by creation function
  String                attributes; // attributes that were set on this widget
  String                action_url; // url to emit as an action if this is a button or other action item
  Variant               data;   // associated data -- can also use UserData interface to add more data

  virtual void          UrlAction();    // execute the action_url
  virtual void          Connect_UrlAction(QObject* src_obj, const char* src_signal);
  // #IGNORE connects source object signal to the UrlAction via helper
  virtual void          FixUrl(const String& url_tag, const String& path);
  // replace starting tag in url with given path

  virtual void          GetImage();
  // get gui image, for tai_data controls
  virtual void          GetValue();
  // get value from tai_data controls

  TA_SIMPLE_BASEFUNS(taGuiWidget);
protected:
  taGuiWidget_QObj*    m_helper;

//   void              UpdateAfterEdit_impl() CPP11_OVERRIDE;
private:
  void  Initialize();
  void  Destroy();
};

#endif // taGuiWidget_h
