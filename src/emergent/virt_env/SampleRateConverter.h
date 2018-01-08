// Copyright 2016-2017, Regents of the University of Colorado,
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

#ifndef TRM_SAMPLE_RATE_CONVERTER_H_
#define TRM_SAMPLE_RATE_CONVERTER_H_

#include <vector>

class SampleRateConverter {
public:
  SampleRateConverter(int sampleRate, float outputRate, std::vector<float>& outputData);
  ~SampleRateConverter();

  void reset();
  void dataFill(float data);
  void dataEmpty();
  void flushBuffer();

  float maximumSampleValue() const { return maximumSampleValue_; }
  long numberSamples() const { return numberSamples_; }
private:
  // SampleRateConverter(const SampleRateConverter&) = delete;
  // SampleRateConverter& operator=(const SampleRateConverter&) = delete;

  void initializeConversion(int sampleRate, float outputRate);
  void initializeBuffer();
  void initializeFilter();

  static float Izero(float x);
  static void srIncrement(int *pointer, int modulus);
  static void srDecrement(int *pointer, int modulus);

  float sampleRateRatio_;
  int fillPtr_;
  int emptyPtr_;
  int padSize_;
  int fillSize_;
  unsigned int timeRegisterIncrement_;
  unsigned int filterIncrement_;
  unsigned int phaseIncrement_;
  unsigned int timeRegister_;
  int fillCounter_;

  float maximumSampleValue_;
  long numberSamples_;

  std::vector<float> h_;
  std::vector<float> deltaH_;
  std::vector<float> buffer_;
  std::vector<float>& outputData_;
};

#endif /* TRM_SAMPLE_RATE_CONVERTER_H_ */
