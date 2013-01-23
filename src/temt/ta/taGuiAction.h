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

#ifndef taGuiAction_h
#define taGuiAction_h 1

// parent includes:
#include <taNBase>

// member includes:
#ifndef __MAKETA__
#include <QPointer>
#include <QAction>
#endif

// declare all other types mentioned but not required to include:
class QObject; // 
class taGuiActionHelper; // #IGNORE

TypeDef_Of(taGuiAction);

class TA_API taGuiAction : public taNBase {
  // ##CAT_Gui manages a qt action for the taGui system
INHERITED(taNBase)
public:
#ifndef __MAKETA__
  QPointer<QAction>     action; // the action
#endif
  String                attributes; // attributes that were set on this widget
  String                action_url; // url to emit as an action if this is a button or other action item
  String                toolbar; // toolbar that contains action
  String                menu; // menu that contains action

  virtual void          UrlAction();    // execute the action_url
  virtual void          Connect_UrlAction(QObject* src_obj, const char* src_signal);
  // connects source object signal to the UrlAction via helper
  virtual void          FixUrl(const String& url_tag, const String& path);
  // replace starting tag in url with given path

  TA_SIMPLE_BASEFUNS(taGuiAction);
protected:
  taGuiActionHelper*    m_helper;

//   override void              UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy();
};

#endif // taGuiAction_h
