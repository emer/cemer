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

#ifndef VETexture_h
#define VETexture_h 1

// parent includes:
#include <taNBase>

// smartptr, ref includes
#include <taSmartRefT>

// member includes:
#include <taVector2f>
#include <taColor>

// declare all other types mentioned but not required to include:
class SoTexture2; // #IGNORE
class SoTexture2Transform; // #IGNORE


TypeDef_Of(VETexture);

class TA_API VETexture : public taNBase {
  // #STEM_BASE #NO_UPDATE_AFTER ##CAT_VirtEnv texture mapping of an image onto a 3d object -- defined as a shared resource in the VEWorld that individual objects can point to
INHERITED(taNBase)
public:
  enum Mode {
    MODULATE,                   // texture color is multiplied by the underlying object color, result is shaded by lighting
    DECAL,                      // image overwrites polygon color entirely, result is NOT shaded by lighting (does not work for grayscale texture images! see REPLACE)
    BLEND,                      // for monochrome textures, the intensity is used to blend between the shaded color of the object and the color in blend_color
    REPLACE,                    // image overwrites object color entirely (same as DECAL but works for greyscale images)
  };

  enum Wrap {
    REPEAT,                     // repeat texture to fill object size
    CLAMP,                      // clamp (repeats border pixels to end of object dimension)
  };

  String        desc;           // #EDIT_DIALOG description of this object: what does it do, how should it be used, etc
  String        fname;          // #FILE_DIALOG_LOAD #FILETYPE_Image full file name (including path if not in same dir as project) for the texture image file (formats supported: JPEG, PNG, GIF, TIFF, RGB, PIC, TGA)
  Mode          mode;           // how to apply texture to object
  Wrap          wrap_horiz;     // how to wrap in the horizontal (largest) dimension
  Wrap          wrap_vert;      // how to wrap in the vertical (smallest) dimension
  taVector2f    offset; // [0,0] offset of image from 0,0 (x=horizontal dim, y=vertical dim, shape dependent)
  taVector2f    scale;          // [1,1] scaling of image (x=horizontal dim, y=vertical dim, shape dependent)
  float         rot;            // [0] rotation in the plane, in degrees
  taVector2f    center; // [0,0] center point for scale and rotation
  taColor       blend_color;    // #CONDSHOW_ON_mode:BLEND color to use in blend case

  int           idx;
  // #READ_ONLY #HIDDEN #NO_COPY #CAT_Structure index of this unit within containing unit group

  virtual void          SetTexture(SoTexture2* sotx);
  // configure the texture based on values
  virtual bool          UpdateTexture();
  // #BUTTON if environment is already initialized and viewed, this will update the texture actually shown in the view

  virtual bool          NeedsTransform(); // determines if SoTexture2Transform is needed
  virtual void          SetTransform(SoTexture2Transform* sotx);
  // configure the transform based on values

  override String       GetDesc() const { return desc; }
  override int  GetIndex() const { return idx; }
  override void SetIndex(int i) { idx = i; }

  void  Initialize();
  void  Destroy()       { };
  TA_SIMPLE_BASEFUNS(VETexture);
// protected:
//   void       UpdateAfterEdit_impl();
};

SmartRef_Of(VETexture); // VETextureRef

#endif // VETexture_h
