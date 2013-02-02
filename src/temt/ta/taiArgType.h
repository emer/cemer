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

#ifndef taiArgType_h
#define taiArgType_h 1

// parent includes:
#include <taiType>

// member includes:
#include <taString>
#include <MethodDef>

// declare all other types mentioned but not required to include:
class taBase; //


TypeDef_Of(taiArgType);

class TA_API taiArgType : public taiType {
  // unlike taiTypes, these are created and destroyed each time
  // thus, they cache their values
public:
  MethodDef*    meth;           // method that has the args
  int           arg_idx;        // index of argument in list of args
  TypeDef*      arg_typ;        // which arg typedef this one is
  bool          err_flag;       // true if one of the args was improperly set

  taiType*      use_it;         // alternate iv type to use
  void*         arg_base;       // base value is computed for typedef
  cssEl*        arg_val;        // argument value (as a css element)
  taBase*       obj_inst;       // instance of taBase object for ptr=0 args

  int           BidForType(TypeDef*)                    { return 0; }
  // none of the argtype specific ones should apply to types
  virtual int   BidForArgType(int, TypeDef*, MethodDef*, TypeDef*)      { return 1; }
  // bid for (appropriateness) for given type of method and argument type

  taiWidget*      GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);

  virtual cssEl* GetElFromArg(const char* arg_nm, void* base);
  // this is responsible for setting arg_base and arg_val (base is parent base)

  // base passed here is of the parent object(!)
  void          GetImage_impl(taiWidget* dat, const void* base);
  void          GetValue_impl(taiWidget* dat, void* base);

  virtual bool  GetHasOption(const String& opt, MethodDef* md = NULL, int aidx = -1);
  // check the meth->HasOption, also taking into account a possible restriction to a particular argument index, specified as opt_n_xxx where n = arg idx
  virtual String GetOptionAfter(const String& opt, MethodDef* md = NULL, int aidx = -1);
  // get the meth->OptionAfter(opt), also taking into account a possible restriction to a particular argument index, specified as opt_n_xxx where n = arg idx

  taiArgType(int aidx, TypeDef* argt, MethodDef* mb, TypeDef* td);
  taiArgType();
  ~taiArgType();

  virtual taiArgType*  ArgTypeInst(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) const
  { return new taiArgType(aidx, argt,md,td);}
  TypeDef*      GetTypeDef() const {return &TA_taiArgType;}
private:
  void          Initialize() {}
  void          Destroy() {}
};

#define TAI_ARGTYPE_SUBCLASS(x, y)       \
    INHERITED(y)                         \
  public:                                \
    x(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) \
      : y(aidx, argt, md, td)            \
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
    override x* ArgTypeInst(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) const \
    {                                    \
      return new x(aidx, argt, md, td);  \
    }


#endif // taiArgType_h
