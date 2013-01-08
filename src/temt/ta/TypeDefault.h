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

#ifndef TypeDefault_h
#define TypeDefault_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <taBase_List>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class taBase; // 


class TA_API TypeDefault : public taNBase {
  // ##EXT_def #INSTANCE #NO_TOKENS #NO_UPDATE_AFTER ##CAT_Project Contains a default object of a type
INHERITED(taNBase)
public:
  TypeDef*	old_type;	// #HIDDEN #NO_SAVE has previous type (if default_type changes)
  TypeDef*	default_type;	// #TYPE_taBase type of the default
  taBase*		token;		// #DEFAULT_EDIT #NO_SAVE token which has default values
  taBase_List 	active_membs;	// MemberName / MemberValue pairs
  unsigned long active[MAX_DFT_LONGS]; // #HIDDEN #NO_SAVE bits representing a memberdefs activity in a default

  virtual void	SetActive(int memb_no, bool onoff); // set active bit
  virtual void	SetActive(char* memb_nm, bool onoff); // set active flag for member
  virtual bool	GetActive(int memb_no);              // check active bit
  virtual bool 	GetActive(char* memb_nm);	     // check active flag for member
  virtual void	UpdateToNameValue();		   // set the name value data from token
  virtual void	UpdateFromNameValue();		   // set the token form the name values
  virtual void	SetTypeDefaults()		{ taNBase::SetTypeDefaults(); }
  virtual void	SetTypeDefaults(taBase* tok);	   // set defaults for a given token

  void	Dump_Load_pre();
  int	Dump_Load_Value(istream& strm, taBase* par=NULL);

  void	InitLinks();
  TA_BASEFUNS(TypeDefault);
protected:
  void	UpdateAfterEdit_impl();
private:
  void	Copy_(const TypeDefault& cp);
  void  Initialize();
  void	Destroy();
};

#endif // TypeDefault_h
