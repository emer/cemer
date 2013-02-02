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

#ifndef FrameViewer_h
#define FrameViewer_h 1

// parent includes:
#include <taViewer>

// member includes:

// declare all other types mentioned but not required to include:
class iFrameViewer; // 
class MainWindowViewer; // 


TypeDef_Of(FrameViewer);

class TA_API FrameViewer : public taViewer {
  // #NO_TOKENS #VIRT_BASE for views that can be in the splitter of a MainWindowViewer
INHERITED(taViewer)
public:

  inline iFrameViewer* widget() {return (iFrameViewer*)inherited::widget();}
  // #IGNORE lex override

  inline MainWindowViewer* mainWindowViewer() {return parent();}

//  void        InitLinks();
//  void        CutLinks(); //
//  void        Copy_(const FrameViewer& cp);
  TA_DATAVIEWFUNS(FrameViewer, taViewer) //
protected:
  override void         GetWinState_impl(); // set gui state; only called if mapped
  override void         SetWinState_impl(); // fetch gui state; only called if mapped

private:
  NOCOPY(FrameViewer)
  void  Initialize();
  void  Destroy() {CutLinks();}
};

#endif // FrameViewer_h
