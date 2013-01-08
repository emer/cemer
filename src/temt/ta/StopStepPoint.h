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

#ifndef StopStepPoint_h
#define StopStepPoint_h 1

// parent includes:
#include <ProgEl>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API StopStepPoint: public ProgEl { 
  // this is a point in the program where the Stop button will stop execution, and the Step button will act for single stepping (e.g., place inside of a loop) -- otherwise this only happens at the end of programs
INHERITED(ProgEl)
public:
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgCtrl"; }
  override String	GetToolbarName() const { return "stop/step"; }

  PROGEL_SIMPLE_COPY(StopStepPoint);
  void InitLinks();
  SIMPLE_CUTLINKS(StopStepPoint);
  TA_BASEFUNS(StopStepPoint);
protected:
  override void		PreGenMe_impl(int item_id);
  // register the target as a subprog of this one
  override void		GenCssBody_impl(Program* prog);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif // StopStepPoint_h
