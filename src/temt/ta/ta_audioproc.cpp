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

#include "ta_audioproc.h"

///////////////////////////////////////////////////////////////////////////
//  taAudioProc
///////////////////////////////////////////////////////////////////////////

void taAudioProc::Initialize() {
}

void taAudioProc::Destroy() {
}

#ifdef TA_OS_MAC 

#include <ApplicationServices/ApplicationServices.h>
//#include <SpeechSynthesis.h>

bool taAudioProc::SpeakText(const String& text) {
  // todo: should use the newer interface: SpeakCFString
  CFStringRef cfstr = CFStringCreateWithCString(NULL, text.chars(), kCFStringEncodingMacRoman);
  Str255 pstr;
  
  CFStringGetPascalString(cfstr, pstr, 255, kCFStringEncodingMacRoman);
  
  SpeakString(pstr);
  return true;
}

#elif defined(TA_OS_WIN)

//TODO: call Windows speech api, if it exists
bool taAudioProc::SpeakText(const String& text) {
  taMisc::Warning("Sorry, SpeakText not supported on this platform. Should be saying:",
		  text);
  return false;
}

#else // must be linux/unix

bool taAudioProc::SpeakText(const String& text) {
  // NOTE: uses festival command line interface
  String cmd = "echo \"" + text + "\" | festival --tts";
  int rval = system(cmd);
  return (rval == 0);
}

#endif 
