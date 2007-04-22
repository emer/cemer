// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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



#ifndef dynenum_h
#define dynenum_h

#include "ta_base.h"
#include "ta_TA_type.h"

class TA_API DynEnumItem : public taNBase {
  // #NO_TOKENS #NO_UPDATE_AFTER ##INLINE ##CAT_Program dynamic enumerated type value (name and numerical int value)
INHERITED(taNBase)
public:
  int		value;		// numerical (integer) value of this enum
  String	desc;		// description of item

  inline void 	Initialize() 			{ value = 0; }
  inline void 	Destroy()			{ };
  inline void 	Copy_(const DynEnumItem& cp)	{ value = cp.value; desc = cp.desc; }
  TA_BASEFUNS(DynEnumItem);
};

class TA_API DynEnum : public taList<DynEnumItem> {
  // #NO_TOKENS #NO_UPDATE_AFTER #CHILDREN_INLINE ##CAT_Program dynamic enumerated type: list of dyn enum items
INHERITED(taList<DynEnumItem>)
public:
  int	value_idx;		// index of current value in list of enums (-1 = not set)

  virtual int 	NumVal() const;
  // current numerical (integer) value of enum (-1 = no value set)
  virtual const String NameVal() const;
  // current name (string) value of enum ("" = no value set)

  virtual int	FindNumIdx(int val) const; // find index of given numerical value
  virtual int	FindNameIdx(const String& nm) const
  { int rval=0; FindName(nm, rval); return rval;}
  // find index of given name value

  virtual bool	SetNumVal(int val);
  // set current enum value by numerical value; false (and error msg) if not found
  virtual bool	SetNameVal(const String& nm);
  // set current enum value by name; false (and error msg) if not found

  virtual ostream& OutputType(ostream& fh, int indent = 0) const;
  // output type information

  virtual void	OrderItems();
  // ensure that the item values are sequentially increasing

  override void DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  // #IGNORE invalidate current value_idx when any of the type data changes
  // sort by numerical value
  override int	El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(((DynEnumItem*)a)->value > ((DynEnumItem*)b)->value) rval=1;
    else if(((DynEnumItem*)a)->value == ((DynEnumItem*)b)->value) rval=0; return rval; }

  void	Initialize();
  void	Destroy() { Reset(); }
  void	Copy_(const DynEnum& cp);
  TA_BASEFUNS(DynEnum);
};

#endif // dynenum_h
