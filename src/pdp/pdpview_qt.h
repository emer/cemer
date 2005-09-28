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



#ifndef PDPVIEW_QT_H
#define PDPVIEW_QT_H

// pdpview_qt.h pdp viewer

#include "pdpdeclare.h"
#include "ta_qtviewer.h"
#include "pdp_TA_type.h"


//////////////////////////////////////////////////
//	PDPView:  view for pdp system		//
//////////////////////////////////////////////////

class PDPView : public DataViewer {
  // #NO_TOKENS pdp version of a win view
public:
  void		AddNotify(TAPtr ud);
  void		RemoveNotify(TAPtr ud);
  TypeDef* 	UpdaterType()		{ return &TA_SchedProcess; }

  TA_BASEFUNS(PDPView);

private:
  void	Initialize()	{ };
  void 	Destroy()	{ };
};



#endif // PDPVIEW_H

