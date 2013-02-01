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

#ifndef taiCompData_h
#define taiCompData_h 1

// parent includes:
#include <taiData>

// member includes:
#include <taiDataList>

// declare all other types mentioned but not required to include:
class QHBoxLayout; // 
class iFlowLayout; //
class QStackedLayout; //

class TA_API taiCompData : public taiData {
  //  #NO_INSTANCE base class for composite data elements
  Q_OBJECT
  INHERITED(taiData)
public:
  enum LayoutType { // the type of layout being used
    LT_HBox,
    LT_Flow,
    LT_Stacked
  };

  taiCompData(TypeDef* typ_, IWidgetHost* host_, taiData* parent_, QWidget* gui_parent_, int flags = 0);
  override ~taiCompData();

  taiDataList           data_el;

  QLayout*              GetLayout() {return (QLayout*)lay;} // override
  inline LayoutType     layType() const {return lay_type;}
  QWidget*              widgets(int index);
  int                   widgetCount();

  override taBase*      ChildBase() const
   {if (m_child_base) return m_child_base; return inherited::ChildBase();}
   // child base, typically obtained from parent or host, except ex. PolyData
  virtual void          InitLayout(); // default creates a QHBoxLayout in the Rep
  void                  AddChildWidget(QWidget* child_widget, int space_after = -1,
    int stretch = 0);
    // s_a=-1 for default taiM->hspc_c
  virtual void          AddChildMember(MemberDef* md); // adds label and control for the member
  virtual void          EndLayout(); // default adds a stretch

#ifndef __MAKETA__
signals:
  void                  ChildSigEmitNotify(taiData* sender);
  // emitted whenever a child does SigEmitNotify
#endif
protected:
  QLayout*              lay; // may be ignored/unused by subclasses
  int                   last_spc;       // space after last widget, -1 = none
  LayoutType            lay_type;
  mutable taBase*       m_child_base; // typically set in PolyData GetImage
  inline QHBoxLayout*   layHBox() const
    {return (QHBoxLayout*)lay;} // only if !hasFlow
  inline iFlowLayout*   layFlow() const
    {return (iFlowLayout*)lay;} // only if hasFlow
  inline QStackedLayout* layStacked() const
    {return (QStackedLayout*)lay;} // only if hasFlow
  override void         ChildAdd(taiData* child);
  override void         ChildRemove(taiData* child);
  virtual void          AddChildWidget_impl(QWidget* child_widget, int spacing,
    int stretch);// default does an add to layout

protected slots:
  void                  ChildSigEmit(taiData* sender);
  // connected to SigEmit of all our children

private:
  QObjectList*          mwidgets; // list of child widgets
};

#endif // taiCompData_h
