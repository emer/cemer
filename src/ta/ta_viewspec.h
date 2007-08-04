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

#ifndef TA_VIEWSPEC_H
#define TA_VIEWSPEC_H

#include "ta_base.h"
#include "ta_group.h"

#include "ta_TA_type.h"

class TA_API ViewSpec: public taDataView {
  // ##CAT_Display generic specification for a view of some item
INHERITED(taDataView)
public:
  String                name; 
  String		display_name; // name used in display IF DIFFERENT THAN name
  bool	        	visible; 	// visibility flag
  
  String GetName() const {return name;} 
  bool  SetName(const String& value);
  void	SetDefaultName() {SetDefaultName_();}
  String GetDisplayName() const;
  TA_BASEFUNS(ViewSpec) //
private:
  void	Copy_(const ViewSpec& cp);
  void Initialize();
  void Destroy() {}
};

  
class TA_API ViewSpec_Group: public taGroup<ViewSpec> {
  // ##CAT_Display group of view specs
INHERITED(taGroup<ViewSpec>)
public:
  TA_BASEFUNS(ViewSpec_Group) //
private:
  NOCOPY(ViewSpec_Group)
  void Initialize() {SetBaseType(&TA_ViewSpec);}
  void Destroy() {}
};

#endif
