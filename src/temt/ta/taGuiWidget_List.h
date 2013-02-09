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

#ifndef taGuiWidget_List_h
#define taGuiWidget_List_h 1

// parent includes:
#include <taGuiWidget>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taGuiWidget_List);

class TA_API taGuiWidget_List : public taList<taGuiWidget> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Gui list of ta gui widgets
INHERITED(taList<taGuiWidget>)
public:
  virtual void  GetImage();     // get image for all items in the list
  virtual void  GetValue();     // get value for all items in the list

  virtual void          FixAllUrl(const String& url_tag, const String& path);
  // replace starting tag in url with given path

  TA_SIMPLE_BASEFUNS(taGuiWidget_List);
private:
  void  Initialize();
  void  Destroy() { Reset(); CutLinks(); }
};

#endif // taGuiWidget_List_h
