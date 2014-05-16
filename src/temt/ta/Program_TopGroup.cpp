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

#include "Program_TopGroup.h"

TA_BASEFUNS_CTORS_DEFN(Program_TopGroup);

void Program_TopGroup::Initialize() {
  gp.SetBaseType(&TA_Program_Group);
  gp.el_typ = &TA_Program_Group;
  break_points.name = "break_points";
}

void Program_TopGroup::InitLinks() {
  inherited::InitLinks();
  taBase::Own(break_points, this);
  gp.SetBaseType(&TA_Program_Group); // don't make us, make regular groups..
  gp.el_typ = &TA_Program_Group;
  break_points.name = "break_points";
}

void Program_TopGroup::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  gp.SetBaseType(&TA_Program_Group); // don't make us, make regular groups..
  gp.el_typ = &TA_Program_Group;
}  
