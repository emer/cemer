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

#ifndef iMethodButtonMgr_h
#define iMethodButtonMgr_h 1

// parent includes:
#ifndef __MAKETA__
#include <QObject>
#endif
#include <ISigLinkClient>

// member includes:
#include <taString>
#ifndef __MAKETA__
#include <taiMenu_List>
#include <taiWidget_List>
#else
class taiWidget_List;
#endif

// declare all other types mentioned but not required to include:
class taiMenu_List; //
class taBase; //
class IWidgetHost; //
class taiMethodData; //
class taiWidgetActions; //
class QLayout; //
class IWidgetHost; //

TypeDef_Of(iMethodButtonMgr);

class TA_API iMethodButtonMgr: public QObject, virtual public ISigLinkClient
{ // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS an that can be conveniently used anywhere to provide the meth buttons of an edit -- note: requires an IWidgetHost and gui objects
INHERITED(QObject)
  Q_OBJECT
public:
  inline QLayout*       lay() const {return m_lay;}

  bool                  show_meth_buttons; // set when we have any guys to show


  void                  setBase(taBase* value);

  void                  Constr(taBase* base,
    IWidgetHost* host = NULL); // #IGNORE -- note: host prob not needed, can be removed
  void                  Constr(QWidget* widg, QLayout* lay, taBase* base,
    IWidgetHost* host = NULL); // #IGNORE -- note: host prob not needed, can be removed
  void                  AddMethButton(taiMethodData* mth_rep,
    const String& label = _nilString);
  void                  GetImage();
  void                  Reset();

  iMethodButtonMgr(QObject* parent = NULL);
  iMethodButtonMgr(QWidget* widg, QLayout* lay, QObject* parent = NULL);
  ~iMethodButtonMgr();

protected:
  QWidget*              widg; // the host widget
  QLayout*              m_lay; // usually an iFlowLayout or QHBoxLayout, margins/spacing set
  IWidgetHost*    host; // must have outer lifetime to us!
  taBase*               base; // the object that has the methods
  TypeDef*              typ;
  taiMenu_List          ta_menu_buttons; // menu representations (from methods -- menubuttons only)
  taiWidgetActions*           cur_menu_but; // current menu button to add to (if not otherwise
  taiWidget_List           meth_el;        // method elements

  void                  Constr_impl(taBase* base, IWidgetHost* host); // #IGNORE
  virtual void          Constr_Methods_impl(); // #IGNORE
  void                  DoAddMethButton(QWidget* but); // #IGNORE
  void                  SetCurMenuButton(MethodDef* md);

public: // ITypedObject i/f (common to IDLC and IDH)
  void*         This() {return this;} // override
  TypeDef*      GetTypeDef() const {return &TA_iMethodButtonMgr;} // override
public: // ISigLinkClient i/f -- note: only registered though for taiEDH and later
  void          SigLinkDestroying(taSigLink* dl);
  void          SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2);

private:
  void          Init();
};

#endif // iMethodButtonMgr_h
