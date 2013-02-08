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

#ifndef taiTypeBase_h
#define taiTypeBase_h 1

// parent includes:
#include <taRefN>

// member includes:
#include <TypeDef>

// declare all other types mentioned but not required to include:
class iColor; //


TypeDef_Of(taiTypeBase);

class TA_API taiTypeBase: public taRefN {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS graphically represents a type
public:
  taiTypeBase(TypeDef* typ_);
  taiTypeBase();
  virtual ~taiTypeBase();

  bool HasLowerBidder() const {
    return next_lower_bidder;
  }
  virtual TypeDef* GetTypeDef() const {
    return &TA_taiTypeBase;
  }
  virtual taiTypeBase* TypeInst(TypeDef* td) const {
    return new taiTypeBase(td);
  }

private:
  void InsertThisIntoBidList_impl(taiTypeBase *&pHead);
  // Insert this object into a list of taiTypeBase objects bidding for some type.  The list is sorted in order of the objects' bids.

protected:
#ifndef __MAKETA__ // maketa doesn't like template methods
  template<typename T>
  void InsertThisIntoBidList(T *&pHead) {
    // Need this little helper function since Der** isn't-a Base**.
    taiTypeBase *pHeadBase = pHead;
    InsertThisIntoBidList_impl(pHeadBase);
    pHead = static_cast<T*>(pHeadBase);
  }
#endif

public:
  static const iColor   def_color;      // passed as a default, or explicitly, to indicate using default
  TypeDef*              typ;            // typedef of base object
  int                   bid;            // this object's bid for typ.
  taiTypeBase*          next_lower_bidder; // points to the next bidder in the linked-list
  bool                  no_setpointer;  // don't use SetPointer for taBase pointers (ie., for css or other secondary pointers)
};

// Implement common routines for taiTypeBase subclasses.
// The overridden TypeInst() function has a covariant return type.
// See also TAI_MEMBER_SUBCLASS, TAI_ARGTYPE_SUBCLASS, TAI_METHOD_SUBCLASS.
#define TAI_TYPEBASE_SUBCLASS(x, y)         \
    INHERITED(y)                            \
  public:                                   \
    x(TypeDef* td)                          \
      : y(td)                               \
    {                                       \
      Initialize();                         \
    }                                       \
    x()                                     \
    {                                       \
      Initialize();                         \
    }                                       \
    ~x()                                    \
    {                                       \
      Destroy();                            \
    }                                       \
    override TypeDef* GetTypeDef() const    \
    {                                       \
      return &TA_##x;                       \
    }                                       \
    override x* TypeInst(TypeDef* td) const \
    {                                       \
      return new x(td);                     \
    }


#endif // taiTypeBase_h
