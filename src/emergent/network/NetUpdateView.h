// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef NetUpdateView_h
#define NetUpdateView_h 1

// parent includes:
#include <NetBaseProgEl>

// member includes:
#include <ProgVar>

// declare all other types mentioned but not required to include:

eTypeDef_Of(NetUpdateView);

class E_API NetUpdateView: public NetBaseProgEl { 
  // update the network view, conditional on an update_net_view variable that is created by this progam element
INHERITED(NetBaseProgEl)
public:
  ProgVarRef	network_var;	// #ITEM_FILTER_NetProgVarFilter variable that points to the network
  ProgVarRef	update_var;	// #ITEM_FILTER_StdProgVarFilter variable that controls whether we update the display or not
  
  String	GetDisplayName() const CPP11_OVERRIDE;
  String 	GetTypeDecoKey() const CPP11_OVERRIDE { return "Function"; }
  String	GetToolbarName() const CPP11_OVERRIDE { return "net updt view"; }

  PROGEL_SIMPLE_BASEFUNS(NetUpdateView);
protected:
  void	UpdateAfterEdit_impl() CPP11_OVERRIDE;
  void	CheckThisConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  virtual void	GetUpdateVar(); // get the update_var variable

  void		GenCssBody_impl(Program* prog) CPP11_OVERRIDE;

private:
  void	Initialize();
  void	Destroy();
};

#endif // NetUpdateView_h
