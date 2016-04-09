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

#ifndef TRM_FIR_FILTER_H_
#define TRM_FIR_FILTER_H_

#include <vector>

/******************************************************************************
 *
 *  class:    FIRFilter
 *
 *  purpose:  Is the linear phase, lowpass FIR filter.
 *
 ******************************************************************************/
class FIRFilter {
public:
  FIRFilter(float beta, float gamma, float cutoff);
  ~FIRFilter();

  void reset();
  float filter(float input, int needOutput);
private:
  FIRFilter(const FIRFilter&) = delete;
  FIRFilter& operator=(const FIRFilter&) = delete;

  static int maximallyFlat(float beta, float gamma, int* np, float* coefficient);
  static void trim(float cutoff, int* numberCoefficients, float* coefficient);
  static int increment(int pointer, int modulus);
  static int decrement(int pointer, int modulus);
  static void rationalApproximation(float number, int* order, int* numerator, int* denominator);

  std::vector<float> data_;
  std::vector<float> coef_;
  int ptr_;
  int numberTaps_;
};

#endif /* TRM_FIR_FILTER_H_ */
