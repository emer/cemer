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

#ifndef SoImageEx_h
#define SoImageEx_h 1

// parent includes:
#include "ta_def.h"
#include <Inventor/nodes/SoSeparator.h>

// member includes:
#include <byte_Matrix>
#include <iPoint>

// declare all other types mentioned but not required to include:
class SbImage; //
class SoTexture2; //
class QImage; //
class taMatrix; //
class SoRect; //


#ifndef __MAKETA__
class CoinImageReaderCB {
  // ##IGNORE callback class for SbImage image reader callback function that eliminates any dependency on the simage library -- only avail in Coin >= 3.0 -- create this object statically somewhere after project initialization and it will install itself -- this is done in ta_project.cpp
public:
  CoinImageReaderCB(void);
  ~CoinImageReaderCB(void);

  SbBool readImage(const SbString & filename, SbImage & image) const;

private:
  static SbBool readImageCB(const SbString & filename, SbImage * image, void * closure);
};
#endif

class TA_API SoImageEx: public SoSeparator { 
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS taImage-compatible image viewer -- width will always be 1; height will then be h/w ratio
#ifndef __MAKETA__
typedef SoSeparator inherited;

  SO_NODE_HEADER(SoImageEx);
#endif // def __MAKETA__
public:
  static void		initClass();
  
  static bool		SetTextureFile(SoTexture2* sotx, const String& fname);
    // set a texture from a file, using simage if available, else Qt
  static bool		SetTextureImage(SoTexture2* sotx, const QImage& img);
    // set a texture from a QImage (note: y is flipped for Coin's base 0)
  
  SoTexture2*		texture;
  
  void		setImage(const QImage& src);
  void		setImage(const taMatrix& src, bool top_zero = false);
    // gray: X,Y; rgb: X,Y,[rgb] -- tz false is normal convention for pdp
  
  SoImageEx();
protected:
  static bool 		SetTextureFile_impl(SoTexture2* sotx, const String& fname,
    bool use_simage); 

  const char*  	getFileFormatName() const {return "Separator"; } 

  SoRect*		shape;
  byte_Matrix		img;
  iVec2i		d; // cached for clarity
  
  void		adjustScale(); // called after setting image to adjust aspect
  void		setImage2(const QImage& src);
  void		setImage3(const QImage& src);
  void		setImage2(const taMatrix& src, bool top_zero);
  void		setImage3(const taMatrix& src, bool top_zero);
  ~SoImageEx();
};

#endif // SoImageEx_h
