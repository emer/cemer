// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#ifndef TA_AUDIOPROC_H
#define TA_AUDIOPROC_H

#include "ta_base.h"

class TA_API taAudioProc : public taNBase {
  // #STEM_BASE ##CAT_Audio basic audio processing operations
INHERITED(taNBase)
public:	

  static bool	SpeakText(const String& text);
  // #CAT_SpeechOut #BUTTON generate an speech synthesis output of the given text string -- returns false if not successful

  TA_SIMPLE_BASEFUNS(taAudioProc);
private:
  void 	Initialize();
  void  Destroy();
};
TA_SMART_PTRS(taAudioProc)

#endif


