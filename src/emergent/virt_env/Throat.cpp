// Copyright 2016, Regents of the University of Colorado,
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

/***************************************************************************
 *  Copyright 1991, 1992, 1993, 1994, 1995, 1996, 2001, 2002               *
 *    David R. Hill, Leonard Manzara, Craig Schock                         *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
// 2014-09
// This file was copied from Gnuspeech and modified by Marcelo Y. Matuda.

#include "Throat.h"

Throat::Throat(float sampleRate, float throatCutoff, float throatGain)
  : throatGain_(throatGain)
  , throatY_(0.0)
{
  // Initializes the throat lowpass filter coefficients
  // according to the throatCutoff value, and also the
  // throatGain, according to the throatVol value.

  ta0_ = (throatCutoff * 2.0) / sampleRate;
  tb1_ = 1.0 - ta0_;
}

Throat::~Throat()
{
}

void
Throat::reset()
{
  throatY_ = 0.0;
}

/******************************************************************************
 *
 *  function:  throat
 *
 *  purpose:   Simulates the radiation of sound through the walls
 *             of the throat. Note that this form of the filter
 *             uses addition instead of subtraction for the
 *             second term, since tb1 has reversed sign.
 *
 ******************************************************************************/
float
Throat::process(float input)
{
  float output = (ta0_ * input) + (tb1_ * throatY_);
  throatY_ = output;
  return output * throatGain_;
}
