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

#ifndef ValIdx_h
#define ValIdx_h 1

// parent includes:
#include <taBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(ValIdx);

class TA_API ValIdx : public taBase {
  // #STEM_BASE ##NO_TOKENS #NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math a float value and an index: very useful for sorting!
  INHERITED(taBase)
public:
  float         val;            // value
  int           idx;            // index

  inline void   SetValIdx(float v, int i)       { val = v; idx = i; }
  inline void   GetValIdx(float& v, int& i)     { v = val; i = idx; }

  inline void   Initialize()                    { val = 0.0; idx = 0; }
  inline void   Destroy()                       { };
  void  Copy(const ValIdx& cp)  { val = cp.val; idx = cp.idx; }
  inline bool Copy(const taBase* cp) {return taBase::Copy(cp);}

  ValIdx()                              { Initialize(); }
  ValIdx(const ValIdx& cp)              { Copy(cp); }
  ValIdx(float v, int i)                { SetValIdx(v, i); }
  ValIdx(const String& str)             { val = (float)str; }
  ~ValIdx()                             { };
  taBase* Clone() const                 { return new ValIdx(*this); }
  void  UnSafeCopy(const taBase* cp) {
    if(cp->InheritsFrom(&TA_ValIdx)) Copy(*((ValIdx*)cp));
    if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this);
  }
  void  CastCopyTo(taBase* cp) const    { ValIdx& rf = *((ValIdx*)cp); rf.Copy(*this); }
  taBase* MakeToken() const             { return (taBase*)(new ValIdx); }
  taBase* MakeTokenAry(int no) const    { return (taBase*)(new ValIdx[no]); }
  TypeDef* GetTypeDef() const           { return &TA_ValIdx; }
  static TypeDef* StatTypeDef(int)      { return &TA_ValIdx; }

  inline operator String () const { return String(val); }

  inline void operator=(const ValIdx& cp) { Copy(cp); }
  inline void operator=(float cp)               { val = cp; idx = -1; }

  inline void operator += (const ValIdx& td)    { val += td.val; }
  inline void operator -= (const ValIdx& td)    { val -= td.val; }
  inline void operator *= (const ValIdx& td)    { val *= td.val; }
  inline void operator /= (const ValIdx& td)    { val /= td.val; }

  inline void operator += (float td)    { val += td; }
  inline void operator -= (float td)    { val -= td; }
  inline void operator *= (float td)    { val *= td; }
  inline void operator /= (float td)    { val /= td; }

  inline ValIdx operator + (const ValIdx& td) const {
    ValIdx rv; rv.val = val + td.val; rv.idx = idx; return rv;
  }
  inline ValIdx operator - (const ValIdx& td) const {
    ValIdx rv; rv.val = val - td.val; rv.idx = idx; return rv;
  }
  inline ValIdx operator * (const ValIdx& td) const {
    ValIdx rv; rv.val = val * td.val; rv.idx = idx; return rv;
  }
  inline ValIdx operator / (const ValIdx& td) const {
    ValIdx rv; rv.val = val / td.val; rv.idx = idx; return rv;
  }

  inline ValIdx operator + (float td) const {
    ValIdx rv; rv.val = val + td; rv.idx = idx; return rv;
  }
  inline ValIdx operator - (float td) const {
    ValIdx rv; rv.val = val - td; rv.idx = idx; return rv;
  }
  inline ValIdx operator * (float td) const {
    ValIdx rv; rv.val = val * td; rv.idx = idx; return rv;
  }
  inline ValIdx operator / (float td) const {
    ValIdx rv; rv.val = val / td; rv.idx = idx; return rv;
  }

  inline ValIdx operator - () const {
    ValIdx rv; rv.val = -val; rv.idx = idx; return rv;
  }

  inline bool operator <  (const ValIdx& td) const { return (val <  td.val); }
  inline bool operator >  (const ValIdx& td) const { return (val >  td.val); }
  inline bool operator <= (const ValIdx& td) const { return (val <= td.val); }
  inline bool operator >= (const ValIdx& td) const { return (val >= td.val); }
  inline bool operator == (const ValIdx& td) const { return (val == td.val); }
  inline bool operator != (const ValIdx& td) const { return (val != td.val); }

  inline bool operator <  (float td) const { return (val <  td); }
  inline bool operator >  (float td) const { return (val >  td); }
  inline bool operator <= (float td) const { return (val <= td); }
  inline bool operator >= (float td) const { return (val >= td); }
  inline bool operator == (float td) const { return (val == td); }
  inline bool operator != (float td) const { return (val != td); }

  inline String GetStr() const { return String(val) + ":" + String(idx); }
};

inline ValIdx operator + (float td, const ValIdx& v) {
  ValIdx rv; rv.val = td + v.val; rv.idx = v.idx; return rv;
}
inline ValIdx operator - (float td, const ValIdx& v) {
  ValIdx rv; rv.val = td - v.val; rv.idx = v.idx; return rv;
}
inline ValIdx operator * (float td, const ValIdx& v) {
  ValIdx rv; rv.val = td * v.val; rv.idx = v.idx; return rv;
}
inline ValIdx operator / (float td, const ValIdx& v) {
  ValIdx rv; rv.val = td / v.val; rv.idx = v.idx; return rv;
}

#endif // ValIdx_h
