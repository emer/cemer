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

#ifndef TRM_WAVETABLE_GLOTTAL_SOURCE_H_
#define TRM_WAVETABLE_GLOTTAL_SOURCE_H_

#include <memory>
#include <vector>


class FIRFilter;

class WavetableGlottalSource {
public:
  enum Type { /*  WAVEFORM TYPES  */
    TYPE_PULSE,
    TYPE_SINE
  };

  WavetableGlottalSource(
                         Type type, float sampleRate,
                         float tp = 0.0, float tnMin = 0.0, float tnMax = 0.0);
  ~WavetableGlottalSource();

  void reset();
  float getSample(float frequency);
  void updateWavetable(float amplitude);
private:
  // WavetableGlottalSource(const WavetableGlottalSource&) = delete;
  // WavetableGlottalSource& operator=(const WavetableGlottalSource&) = delete;

  void incrementTablePosition(float frequency);

  static float mod0(float value);

  int    tableDiv1_;
  int    tableDiv2_;
  float tnLength_;
  float tnDelta_;
  float basicIncrement_;
  float currentPosition_;
  std::vector<float> wavetable_;
  std::unique_ptr<FIRFilter> firFilter_;
};

#endif /* TRM_WAVETABLE_GLOTTAL_SOURCE_H_ */
