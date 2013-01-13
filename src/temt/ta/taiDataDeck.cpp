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

#include "taiDataDeck.h"
#include <IDataHost>
#include <iColor>


#include <QStackedLayout>

#include "taiqtso_def.h"

taiDataDeck::taiDataDeck(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags)
  : taiCompData(typ_, host_, par, gui_parent_, flags)
{
  lay_type = LT_Stacked;
  SetRep(MakeLayoutWidget(gui_parent_));
  if (host != NULL) {
    SET_PALETTE_BACKGROUND_COLOR(rep(), host->colorOfCurRow());
  }
}

void taiDataDeck::GetImage(int i) {
  layStacked()->setCurrentIndex(i);
}

