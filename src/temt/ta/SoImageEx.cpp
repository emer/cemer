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

#include "SoImageEx.h"

// special Coin 3.0 code to set the read image callback so we can nuke simage for good!!!

#include <Inventor/SbBasic.h>

#if COIN_MAJOR_VERSION >= 3
#include <Inventor/SbImage.h>

// this code is essentially verbatim from Quarter
// http://svn.coin3d.org/repos/Quarter/trunk/src/Quarter/ImageReader.cpp

CoinImageReaderCB::CoinImageReaderCB(void)
{
  SbImage::addReadImageCB(CoinImageReaderCB::readImageCB, this);
}

CoinImageReaderCB::~CoinImageReaderCB(void)
{
  SbImage::removeReadImageCB(CoinImageReaderCB::readImageCB, this);
}

SbBool
CoinImageReaderCB::readImage(const SbString & filename, SbImage & sbimage) const
{
  QImage image;
  if (image.load(filename.getString())) {
    int c;
    int w = image.width();
    int h = image.height();

    // Keep in 8-bits mode if that was what we read
    if (image.depth() != 8 || !image.isGrayscale()) {
      // FIXME: consider if we should detect allGrayscale() and alpha (c = 2)
      image = image.convertToFormat(image.hasAlphaChannel() ?
                                    QImage::Format_ARGB32 : QImage::Format_RGB32);
    }

    { // QtCoinCompatibility::QImageToSbImage(const QImage & image, SbImage & sbimage)
      // Keep in 8-bits mode if that was what we read
      if (image.depth() == 8 && image.isGrayscale()) {
	c = 1;
      }
      else {
	// FIXME: consider if we should detect allGrayscale() and alpha (c = 2)
	c = image.hasAlphaChannel() ? 4 : 3;
      }

      SbVec2s size((short) w, (short) h);
      sbimage.setValue(size, c, NULL);
      unsigned char * buffer = sbimage.getValue(size, c);

      if (c == 1) {
	for (int i = 0; i < h; i++) {
	  memcpy(buffer + i*w, image.scanLine(h-(i+1)), w);
	}
      }
      else { // (c == 3 || c == 4)
	int max_idx = c * w * (h-1);
	QRgb * bits = (QRgb*) image.bits();
	for (int y = 0; y < h; y++) {
	  int idx = c*w*(h-(y+1));
	  if(idx > max_idx || idx < 0) {
	    continue;
	  }
	  unsigned char * line = &buffer[idx];
	  for (int x = 0; x < w; x++) {
	    *line = qRed(*bits);  line++;
	    *line = qGreen(*bits); line++;
	    *line = qBlue(*bits); line++;
	    if (c == 4) {
	      *line = qAlpha(*bits); line++;
	    }
	    bits++;
	  }
	}
      }
    }
    return TRUE;
  }
  return FALSE;
}

SbBool
CoinImageReaderCB::readImageCB(const SbString & filename, SbImage * image, void * closure)
{
  return ((CoinImageReaderCB*)closure)->readImage(filename, *image);
}


static CoinImageReaderCB coin_img_reader_cb; // always have one statically -- does this work??

#else // COIN_MAJOR_VERSION >= 3

// no-ops

CoinImageReaderCB::CoinImageReaderCB(void)
{
}

CoinImageReaderCB::~CoinImageReaderCB(void)
{
}

SbBool
CoinImageReaderCB::readImage(const SbString & filename, SbImage & sbimage) const
{
  return FALSE;
}

SbBool
CoinImageReaderCB::readImageCB(const SbString & filename, SbImage * image, void * closure)
{
  return FALSE;
}

#endif // COIN_MAJOR_VERSION >= 3


//////////////////////////
//   SoImageEx		//
//////////////////////////

SO_NODE_SOURCE(SoImageEx);

// copy 32 bit xRGB data to an intermediate packed buffer
// then to the Texture
void xRGB_to_Texture(const QImage& img, SoTexture2* sotx) {
  const int nc = 3;
  int src_cnt = img.width() * img.height();
  unsigned char* buf = (unsigned char*)malloc(src_cnt * nc);
  int dst_i = 0;
  //NOTE: we have to invert the data for Coin's bottom=0 addressing
  for (int y = img.height() - 1; y >= 0; --y)
  for (int x = 0; x < img.width(); ++x) {
    QRgb rgb = img.pixel(x, y);
    buf[dst_i++] = (unsigned char)qRed(rgb);
    buf[dst_i++] = (unsigned char)qGreen(rgb);
    buf[dst_i++] = (unsigned char)qBlue(rgb);
  }
  sotx->image.setValue(SbVec2s(img.width(), img.height()),
    nc, buf, SoSFImage::COPY);
  free(buf);
}

void ARGB_to_Texture(const QImage& img, SoTexture2* sotx) {
  const int nc = 4;
  int src_cnt = img.width() * img.height();
  unsigned char* buf = (unsigned char*)malloc(src_cnt * nc);
  int dst_i = 0;
  //NOTE: we have to invert the data for Coin's bottom=0 addressing
  for (int y = img.height() - 1; y >= 0; --y)
  for (int x = 0; x < img.width(); ++x) {
    QRgb rgb = img.pixel(x, y);
    buf[dst_i++] = (unsigned char)qRed(rgb);
    buf[dst_i++] = (unsigned char)qGreen(rgb);
    buf[dst_i++] = (unsigned char)qBlue(rgb);
    buf[dst_i++] = (unsigned char)qAlpha(rgb);
  }
  sotx->image.setValue(SbVec2s(img.width(), img.height()),
    nc, buf, SoSFImage::COPY);
  free(buf);
}

bool SoImageEx::SetTextureImage(SoTexture2* sotx, const QImage& qimg) {
  // TODO: would be *great* to search paths for fname
  // assume it is a RGB or ARGB -- if latter, we'll need to rotate to Coin's RGBA
  QImage::Format format = qimg.format();
  // note: loading should never return the normalized alpha format...
  if (format == QImage::Format_ARGB32) {
    ARGB_to_Texture(qimg, sotx);
    return true; 
  } 
  QImage qimg2(qimg); // may not need to be changed
  if (format != QImage::Format_RGB32) {
    qimg2 = qimg.convertToFormat(QImage::Format_RGB32);
  }
  xRGB_to_Texture(qimg2, sotx);
  return true;
}

bool SoImageEx::SetTextureFile(SoTexture2* sotx, const String& fname) {
  // don't use simage under any condition
  return SetTextureFile_impl(sotx, fname, false);
}

bool SoImageEx::SetTextureFile_impl(SoTexture2* sotx, const String& fname,
  bool use_simage) 
{
  if (use_simage) {		// generally not used
    sotx->filename = (const char*)fname;
    // check if anything loaded
    SbVec2s size; 
    int nc = 0;
    sotx->image.getValue(size, nc);
    return (nc != 0); // would be zero on failure
  } 
  // else Qt 
  QImage qqimg;
  if (!qqimg.load(fname)) return false;
  return SetTextureImage(sotx, qqimg);
}

void SoImageEx::initClass()
{
  SO_NODE_INIT_CLASS(SoImageEx, SoSeparator, "SoSeparator");
}

SoImageEx::SoImageEx() {
  SO_NODE_CONSTRUCTOR(SoImageEx);
  texture = new SoTexture2;
  this->addChild(texture);
  // we set shape to white, so texture map modulates it properly
  SoBaseColor* bc = new SoBaseColor;
  bc->rgb.setValue(0xFF, 0xFF, 0xFF);
  this->addChild(bc);
  shape = new SoRect; //note: w,h = 1.0 so texture map maps exactly
  this->addChild(shape);
}

SoImageEx::~SoImageEx() {
  texture = NULL;
  shape = NULL;
}

void SoImageEx::adjustScale() {
  // just always keep it a square with the x a tiny bit larger than the y -- this seems to
  // prevent spurious rotations and keeps it displayed the same way every time.  geez.
  shape->setDimensions(1.0f, 0.9999f);
  // set proper type
  if (img.dims() == 2) {
    texture->model = SoTexture2::REPLACE;
  } else {
    texture->model = SoTexture2::DECAL;
  }
}

void SoImageEx::setImage(const QImage& src) {
  if (src.isGrayscale())
    setImage2(src);
  else 
    setImage3(src);
  adjustScale();
}

void SoImageEx::setImage2(const QImage& src) {
  d.x = src.width();
  d.y = src.height();
  img.SetGeom(2, d.x, d.y);
  //NOTE: we have to invert the data for Coin's bottom=0 addressing
  for (int y = d.y - 1; y >= 0; --y) {
    for (int x = 0; x < d.x; ++x) {
      img.FastEl(x,y) = (byte)(qGray(src.pixel(x, y)));
    }
  }
  texture->image.setValue(SbVec2s(d.y, d.x), 1, (const unsigned char*)img.data(),
			  SoSFImage::NO_COPY);
}

void SoImageEx::setImage3(const QImage& src) {
  d.x = src.width();
  d.y = src.height();
  //NOTE: img geom is not same as input: rgb is in innermost for us
  img.SetGeom(3, 3, d.x, d.y);
  int idx = 0;
  QRgb rgb;
  //NOTE: we have to invert the data for Coin's bottom=0 addressing
  for (int y = d.y - 1; y >= 0; --y) {
    for (int x = 0; x < d.x; ++x) {
      rgb = src.pixel(x, y);
      img.FastEl_Flat(idx++) = (byte)(qRed(rgb));
      img.FastEl_Flat(idx++) = (byte)(qGreen(rgb));
      img.FastEl_Flat(idx++) = (byte)(qBlue(rgb));
    }
  }
  texture->image.setValue(SbVec2s(d.x, d.y), 3, (const unsigned char*)img.data(),
			  SoSFImage::NO_COPY);
}

void SoImageEx::setImage(const taMatrix& src, bool top_zero) {
  int dims = src.dims(); //cache
  if (dims == 2) {
    setImage2(src, top_zero);
  } else if (((dims == 3) && (src.dim(2) == 3))) { 
    setImage3(src, top_zero);
  } else { 
    taMisc::Error("SoImageEx::setImage: must be gray (dims=2) or rgb matrix");
    return;
  }
  adjustScale();
}

void SoImageEx::setImage2(const taMatrix& src, bool top_zero) {
  d.x = src.dim(0);
  d.y = src.dim(1);
//   if(src.GetDataValType() == taBase::VT_BYTE) {
//     texture->image.setValue(SbVec2s(d.x, d.y), 1, (const unsigned char*)src.data());
//   }
  if((src.GetDataValType() == taBase::VT_FLOAT) ||
	  (src.GetDataValType() == taBase::VT_DOUBLE)) {
    img.SetGeom(2, d.x, d.y);
    if (top_zero) {
      for (int y = 0; y < d.y; ++y) {
	for (int x = 0; x < d.x; ++x) {
	  img.FastEl(x,d.y - y - 1) = (byte)(src.FastElAsFloat(x, y) * 255.0f);
	}
      }
    } else {
      for (int y = 0; y < d.y; ++y) {
	for (int x = 0; x < d.x; ++x) {
	  img.FastEl(x,y) = (byte)(src.FastElAsFloat(x, y) * 255.0f);
	}
      }
    }
    texture->image.setValue(SbVec2s(d.x, d.y), 1, (const unsigned char*)img.data(),
			    SoSFImage::NO_COPY);
  }
  else {			// int type -- assume raw value
    img.SetGeom(2, d.x, d.y);
    if (top_zero) {
      for (int y = 0; y < d.y; ++y) {
	for (int x = 0; x < d.x; ++x) {
	  img.FastEl(x, d.y - 1 - y) = (byte)src.FastElAsFloat(x, y);
	}
      }
    } else {
      for (int y = 0; y < d.y; ++y) {
	for (int x = 0; x < d.x; ++x) {
	  img.FastEl(x,y) = (byte)src.FastElAsFloat(x, y);
	}
      }
    }
    texture->image.setValue(SbVec2s(d.x, d.y), 1, (const unsigned char*)img.data(),
			    SoSFImage::NO_COPY);
  }
}

void SoImageEx::setImage3(const taMatrix& src, bool top_zero) {
  d.x = src.dim(0);
  d.y = src.dim(1);
  //NOTE: img geom is not same as input: rgb is in innermost for us
  img.SetGeom(3, 3, d.x, d.y);
  if((src.GetDataValType() == taBase::VT_FLOAT) ||
     (src.GetDataValType() == taBase::VT_DOUBLE)) {
    if (top_zero) {
      for (int y = 0; y < d.y; ++y) {
	for (int x = 0; x < d.x; ++x) {
	  img.FastEl(0,x,d.y - y - 1) = (byte)(src.FastElAsFloat(x, y, 0) * 255.0f);
	  img.FastEl(1,x,d.y - y - 1) = (byte)(src.FastElAsFloat(x, y, 1) * 255.0f);
	  img.FastEl(2,x,d.y - y - 1) = (byte)(src.FastElAsFloat(x, y, 2) * 255.0f);
	}
      }
    } else {
      for (int y = 0; y < d.y; ++y) {
	for (int x = 0; x < d.x; ++x) {
	  img.FastEl(0,x,y) = (byte)(src.FastElAsFloat(x, y, 0) * 255.0f);
	  img.FastEl(1,x,y) = (byte)(src.FastElAsFloat(x, y, 1) * 255.0f);
	  img.FastEl(2,x,y) = (byte)(src.FastElAsFloat(x, y, 2) * 255.0f);
	}
      }
    }
  }
  else {			// int type -- assume raw value
    if (top_zero) {
      for (int y = 0; y < d.y; ++y) {
	for (int x = 0; x < d.x; ++x) {
	  img.FastEl(0,x,d.y - y - 1) = (byte)(src.FastElAsFloat(x, y, 0));
	  img.FastEl(1,x,d.y - y - 1) = (byte)(src.FastElAsFloat(x, y, 1));
	  img.FastEl(2,x,d.y - y - 1) = (byte)(src.FastElAsFloat(x, y, 2));
	}
      }
    } else {
      for (int y = 0; y < d.y; ++y) {
	for (int x = 0; x < d.x; ++x) {
	  img.FastEl(0,x,y) = (byte)(src.FastElAsFloat(x, y, 0));
	  img.FastEl(1,x,y) = (byte)(src.FastElAsFloat(x, y, 1));
	  img.FastEl(2,x,y) = (byte)(src.FastElAsFloat(x, y, 2));
	}
      }
    }
  }
  texture->image.setValue(SbVec2s(d.x, d.y), 3, (const unsigned char*)img.data(),
			  SoSFImage::NO_COPY);
}
