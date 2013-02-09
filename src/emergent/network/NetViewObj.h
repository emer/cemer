// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef NetViewObj_h
#define NetViewObj_h 1

// parent includes:
#include "network_def.h"
#include <taNBase>

// smartptr, ref:
#include <taSmartRefT>

// member includes:
#include <taVector3f>
#include <taAxisAngle>
#include <taColor>

// declare all other types mentioned but not required to include:

eTypeDef_Of(NetViewObj);

class E_API NetViewObj : public taNBase {
  // ##CAT_Network ##EXT_nvobj network view object (3d object or text) -- is displayed in network view
INHERITED(taNBase)
public:
  enum ObjType {                // what type of object to create
    TEXT,                       // text label
    OBJECT,                     // 3d object loaded from an open inventor format 3d object file
  };

  String        desc;           // #EDIT_DIALOG description of this object: what does it do, how should it be used, etc
  taVector3f    pos;            // 3d position of object (can be moved within network view)
  taAxisAngle   rot;            // 3d rotation of body, specifying an axis and a rot along that axis in radians: 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854)
  taVector3f    scale;          // 3d scaling of object along each dimension (applied prior to rotation)
  ObjType       obj_type;       // type of object to display
  String        obj_fname;      // #CONDSHOW_ON_obj_type:OBJECT #FILE_DIALOG_LOAD #EXT_iv,wrl #FILETYPE_OpenInventor file name of Open Inventor file that contains the 3d geometry of the object
  String        text;           // #CONDSHOW_ON_obj_type:TEXT text to display for text type of object
  float         font_size;      // #CONDSHOW_ON_obj_type:TEXT font size to display text in, in normalized units (the entire network is 1x1x1, so this should typically be a smaller fraction like .05)
  bool          set_color;      // if true, we directly set our own color (otherwise it is whatever the object defaults to)
  taColor       color;          // #CONDSHOW_ON_set_color default color if not otherwise defined (a=alpha used for transparency)

  override String       GetDesc() const { return desc; }

  TA_SIMPLE_BASEFUNS(NetViewObj);
protected:
  override void         UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy()       { CutLinks(); }
};

SmartRef_Of(NetViewObj); // NetViewObjRef

#endif // NetViewObj_h
