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



#include "pdpview_qt.h"




//////////////////////////////////////////////////
//	PDPView:  view for pdp system		//
//////////////////////////////////////////////////

void PDPView::AddNotify(TAPtr ud) {
/*TODO:fixup  if((ud != NULL) && ud->InheritsFrom(TA_SchedProcess)) {
    ((SchedProcess*)ud)->displays.LinkUnique(this);
  }*/
}

void PDPView::RemoveNotify(TAPtr ud) {
/*TODO:fixup   if((ud != NULL) && ud->InheritsFrom(TA_SchedProcess)) {
    ((SchedProcess*)ud)->displays.Remove(this);
  } */
}

