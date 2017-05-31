// Copyright 2017, Regents of the University of Colorado,
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

#ifndef NiftiReader_h
#define NiftiReader_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:
class QString;
class taVector3f;
class taVector3i;
#include "nifti1_io.h"                  /*** NIFTI-1 header specification ***/

class NiftiReader {
public:
  enum AnatomicalPlane 
  { 
    AXIAL     = 0x0000, 
    SAGITTAL  = 0x0001, 
    CORONAL   = 0x0002    
  };

  explicit NiftiReader(const QString& file);
  virtual ~NiftiReader();

  bool    IsValid() const;
  taVector3i XyzDimensions() const;
  int     BytesPerVoxel() const;
  float   CalMax() const;
  float   CalMin() const;
  float   CalRange() const;
  void    SliceXY(int index, unsigned short* data) const;
  void    Slice(AnatomicalPlane p, int index, unsigned short* data) const;
  int     NumExtensions() const;
  int     NumVolumes() const;
  taVector3f XyzToIjk(const taVector3f &xyz_coord) const;
  taVector3f IjkToXyz(const taVector3f &ijk_coord) const;
  const void *RawData() const;

protected:
  nifti_image* m_img;

private:
  NiftiReader();
  NiftiReader(const NiftiReader&);
  NiftiReader operator=(const NiftiReader&);

};

#endif // NiftiReader_h
