// Copyright 2013-2018, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef NetCounterBase_h
#define NetCounterBase_h 1

// parent includes:
#include <NetBaseProgEl>

// member includes:
#include <ProgVar>

// declare all other types mentioned but not required to include:

eTypeDef_Of(NetCounterBase);

class E_API NetCounterBase: public NetBaseProgEl { 
  // #VIRT_BASE #NO_INSTANCE base for network counter manip prog els
INHERITED(NetBaseProgEl)
public:
  ProgVarRef	network_var;	// #ITEM_FILTER_NetProgVarFilter variable that points to the network 
  TypeDef*	network_type;	// #HIDDEN #NO_SAVE just to anchor the memberdef*
  ProgVarRef 	local_ctr_var;	// #ITEM_FILTER_StdProgVarFilter local version of the counter variable, maintained by the program -- must have same name as the counter!  automatically created if not set
  MemberDef*	counter;	// #TYPE_ON_network_type #DEFCAT_Counter counter variable on network to operate on
  bool		update_after;	// #DEF_false call UpdateAfterEdit on network after updating counter value -- should generally be OFF except in special / debugging situations -- will drive automatic updating of displays, and slow down processing overall
  
  String 	GetTypeDecoKey() const override { return "ProgVar"; }

  PROGEL_SIMPLE_BASEFUNS(NetCounterBase);
protected:
  void	UpdateAfterEdit_impl() override;
  void	CheckThisConfig_impl(bool quiet, bool& rval) override;
  virtual void	GetLocalCtrVar(); // if counter is not empty and local_ctr_var == NULL, then get a local ctr var for it

private:
  void	Initialize();
  void	Destroy();
};

#endif // NetCounterBase_h
