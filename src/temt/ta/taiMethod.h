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

#ifndef taiMethod_h
#define taiMethod_h 1

// parent includes:
#include <taiType>

// member includes:
#include <MethodDef>

// declare all other types mentioned but not required to include:
class taiWidgetMethod; // #IGNORE

// the default method handles requests for Menu or Button instances
// the client needs to check the MethodDef directives to decide if
// the type is supposed to have buttons and/or menus in a given context

taTypeDef_Of(taiMethod);

class TA_API taiMethod : public taiType {
INHERITED(taiType)
public:
  MethodDef*    meth;

  taiMethod*            LowerBidder() { return static_cast<taiMethod*>(next_lower_bidder); }

  int           BidForType(TypeDef*)                    { return 0; }
  // none of the method specific ones should apply to types
  virtual int   BidForMethod(MethodDef*, TypeDef*)      { return 0; }
  // bid for (appropriateness) for given type of method (default is not at all approp.)

  taiWidgetMethod*        GetGenericMethodRep(void* base, taiWidget* par); // this is just for CallFun
  taiWidgetMethod*        GetButtonMethodRep(void* base, IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_);
  taiWidgetMethod*        GetMenuMethodRep(void* base, IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_); // covers MENU and MENU_BUTTON types
  USING(inherited::GetImage)
  override void         GetImage(taiWidget*, const void*) { }
  override void         GetValue(taiWidget*, void*)       { }

  taiMethod(MethodDef* mb, TypeDef* td) : taiType(td)   { meth = mb; }
  taiMethod()                                           { meth = NULL; }
  ~taiMethod()                                          { };

  virtual void          AddMethod(MethodDef* md);       // add an iv to a member

  virtual taiMethod*    MethInst(MethodDef* md, TypeDef* td) const
        { return new taiMethod(md,td);}
  TypeDef*      GetTypeDef() const {return &TA_taiMethod;}
protected:
  virtual taiWidgetMethod* GetButtonMethodRep_impl(void* base, IWidgetHost* host_,
    taiWidget* par, QWidget* gui_parent_, int flags_) {return NULL;}
  virtual taiWidgetMethod* GetMenuMethodRep_impl(void* base, IWidgetHost* host_,
    taiWidget* par, QWidget* gui_parent_, int flags_) {return NULL;}
};

#define TAI_METHOD_SUBCLASS(x, y)        \
    INHERITED(y)                         \
  public:                                \
    x(MethodDef* md, TypeDef* td)        \
      : y(md, td)                        \
    {                                    \
      Initialize();                      \
    }                                    \
    x()                                  \
    {                                    \
      Initialize();                      \
    }                                    \
    ~x()                                 \
    {                                    \
      Destroy();                         \
    }                                    \
    override TypeDef* GetTypeDef() const \
    {                                    \
      return &TA_##x;                    \
    }                                    \
    override x* MethInst(MethodDef* md, TypeDef* td) const \
    {                                    \
      return new x(md, td);              \
    }

#endif // taiMethod_h
