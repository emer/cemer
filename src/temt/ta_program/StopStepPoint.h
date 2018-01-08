// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef StopStepPoint_h
#define StopStepPoint_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgExpr>

// declare all other types mentioned but not required to include:


taTypeDef_Of(StopStepPoint);

class TA_API StopStepPoint: public ProgEl { 
  // ##PROGEL_COMPLETION this is a point in the program where the Stop button will stop execution, and the Step button will act for single stepping (e.g., place inside of a loop) -- otherwise this only happens at the end of programs
INHERITED(ProgEl)
public:
  ProgExpr	cond; 		// #BROWSER_EDIT_LOOKUP optional condition for stop / step checking -- only stop / step when this expression is true -- allows e.g., longer time scales and specific conditions (e.g., errors) to be used for stopping

  String	GetDisplayName() const override;
  String 	GetTypeDecoKey() const override { return "ProgCtrl"; }
  String	GetToolbarName() const override { return "stop_step"; }

  PROGEL_SIMPLE_COPY(StopStepPoint);
  void InitLinks() override;
  SIMPLE_CUTLINKS(StopStepPoint);
  TA_BASEFUNS(StopStepPoint);
protected:
  void		CheckThisConfig_impl(bool quiet, bool& rval) override;
  void		PreGenMe_impl(int item_id) override;
  // register the target as a subprog of this one
  bool		GenCssBody_impl(Program* prog) override;

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif // StopStepPoint_h
