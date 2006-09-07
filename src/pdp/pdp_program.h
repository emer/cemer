// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

// pdp_program.h -- pdp extensions to program

#ifndef PDP_PROGRAM_H
#define PDP_PROGRAM_H

#include "program.h"

#include "pdp_def.h"
#include "pdp_TA_type.h"

class PDP_API NetCounterInit: public ProgEl { 
  // initialize a network counter: program keeps a local version of the counter, and updates both this and the network's copy
INHERITED(ProgEl)
public:
  ProgVarRef	network_var;	// #SCOPE_Program_Group variable that points to the network (typically a global_var)
  ProgVarRef 	local_ctr_var;	// #SCOPE_Program_Group local version of the counter variable, maintained by the program -- must have same name as the counter!
  String	counter; 	// name of counter variable on network object (todo: should be a MemberDef*, based on network_var type)
  
  override bool		CheckConfig(bool quiet=false);
  override String	GetDisplayName() const;

  void	UpdateAfterEdit();
  TA_SIMPLE_BASEFUNS(NetCounterInit);

protected:
  virtual void	GetLocalCtrVar(); // if counter is not empty and local_ctr_var == NULL, then get a local ctr var for it

  override const String	GenCssBody_impl(int indent_level); // generate the Css body code for this object

private:
  void	Initialize();
  void	Destroy();
};

class PDP_API NetCounterIncr: public ProgEl { 
  // initialize a network counter: program keeps a local version of the counter, and updates both this and the network's copy
INHERITED(ProgEl)
public:
  ProgVarRef	network_var;	// #SCOPE_Program_Group variable that points to the network (typically a global_var)
  ProgVarRef 	local_ctr_var;	// #SCOPE_Program_Group local version of the counter variable, maintained by the program -- must have same name as the counter!
  String	counter; 	// name of counter variable on network object (todo: should be a MemberDef*, based on network_var type)
  
  override bool		CheckConfig(bool quiet=false);
  override String	GetDisplayName() const;

  void	UpdateAfterEdit();
  TA_SIMPLE_BASEFUNS(NetCounterIncr);

protected:
  virtual void	GetLocalCtrVar(); // if counter is not empty and local_ctr_var == NULL, then get a local ctr var for it

  override const String	GenCssBody_impl(int indent_level); // generate the Css body code for this object

private:
  void	Initialize();
  void	Destroy();
};

#endif
