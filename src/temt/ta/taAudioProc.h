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

#ifndef taAudioProc_h
#define taAudioProc_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(taAudioProc);

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

#endif // taAudioProc_h
