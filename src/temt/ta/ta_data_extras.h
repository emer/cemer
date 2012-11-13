// Copyright, 1995-2007, Regents of the University of Colorado,
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



// ta_data_extras.h -- sundry useful DataBlock implementations

#ifndef TA_DATA_EXTRAS_H
#define TA_DATA_EXTRAS_H

#include "ta_data.h"
#include "ta_geometry.h"
#include "ta_TA_type_WRAPPER.h"

class TA_API DirectoryCatalog: public DataBlock_Idx {
  // #INSTANCE a DirectoryCatalog provides a list of files in a directory
INHERITED(DataBlock_Idx)
public:
  enum CatalogOptions { // #BITS the items to catalog -- usually one or the other
    CO_Files            = 0x01, // #LABEL_Files files
    CO_Dirs             = 0x02, // #LABEL_Dirs directories
  };

  CatalogOptions        options; // what items to catalog -- usually one or the other
  String                filters; // ; separated list of * or ? filters, ex. "*.txt;*.doc"
  String                directory;  // path of directory where the files are

  TA_BASEFUNS(DirectoryCatalog)

protected:
  QDir&                 dir();

public: // DataBlock i/f
  override DBOptions    dbOptions() const
    {return (DBOptions)(DB_INDEXABLE | DB_SEQUENCABLE | DB_SOURCE);}
  override int          ItemCount() const;

public: // DataSource i/f
  override int          SourceChannelCount() const {return 1;}
  override const String SourceChannelName(int chan) const;

protected:
  override const Variant GetData_impl(int chan);
  override void         ReadOpen_impl(bool& ok);
  override void         ReadClose_impl();

private:
  QDir*                 m_dir; // autocreated, only access via dir()
  void Copy_(const DirectoryCatalog& cp);
  void                  Initialize();
  void                  Destroy();
};



class TA_API ImageReader: public DataBlock {
  // #INSTANCE provides ability to read image files, sequential only
INHERITED(DataBlock)
public:
  enum Channels {
    chan_img            = 0 // the entire image matrix, [h, w, comps]
  };

  enum ColorMode { // the color mode to use for reading
    CM_GRAYSCALE,       // read images in b&w only, 1 channel: bw (HxW byte array)
    CM_RGB              // RGB mode, 3 channels: r, g, b (H x W x r=0,g=1,b=2 byte array)
  };

  enum ImageFormat { // the type of image encoding
    IF_AUTO,            // try to determine automatically, ex. from filename, or file header
    IF_JPEG             //
  };

  ColorMode             color_mode; // color mode to use for reading
  ImageFormat           img_format; // image format to use
  String                fname; // #NO_SAVE file we are to read

  ImageFormat           cur_img_format; // #READ_ONLY #SHOW #NO_SAVE image format of current image
  taVector2i            img_size; // #READ_ONLY #SHOW #NO_SAVE size of current image


  void  InitLinks();
  void  CutLinks();
  void  Copy_(const ImageReader& cp);
  TA_BASEFUNS(ImageReader)

protected:
  taMatrix*             m_mat; // we dynamically allocate each time, dims: [w h ch] 0=r,1=g,2=b
  rgb_Matrix*           m_rgb_mat; // we do this by just slicing the m_mat and fudging the dims

  bool                  ReadImage_Jpeg();
  void                  SetMat(taMatrix* new_mat);

public: // DataBlock i/f
  override DBOptions    dbOptions() const
    {return (DBOptions)(DB_SEQUENCABLE | DB_SOURCE);}

  virtual bool          ReadNext() {return ReadItem_impl();}

public: // DataSource i/f
  override int          sourceChannelCount() const; // depends on mode
  override const String sourceChannelName(int chan) const; // bw || r, g, b

  override bool         ReadItem_impl(); // actually does the read
  override void         ReadOpen_impl(bool& ok);
  override void         ReadClose_impl();
protected:
  override taMatrix*    GetDataMatrix_impl(int chan);

private:
  void                  Initialize();
  void                  Destroy();
};



#endif


