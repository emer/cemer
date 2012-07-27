// Copyright, 1995-2007, Regents of the University of Colorado,
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


// niftireader.h

#ifndef NIFTIREADER_H
#define NIFTIREADER_H

#include <QList>
#include <QString>
#include <QStringList>
#include "nifti1_io.h"                  /*** NIFTI-1 header specification ***/
#include "t3node_so.h"

class QString;
class FloatTDCoord;
class TDCoord;

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
  TDCoord XyzDimensions() const;
  int     BytesPerVoxel() const;
  float   CalMax() const;
  float   CalMin() const;
  float   CalRange() const;
  void    SliceXY(int index, unsigned short* data) const;
  void    Slice(AnatomicalPlane p, int index, unsigned short* data) const;
  int     NumExtensions() const;
  int     NumVolumes() const;
  FloatTDCoord XyzToIjk(const FloatTDCoord &xyz_coord) const;
  FloatTDCoord IjkToXyz(const FloatTDCoord &ijk_coord) const;
  const void *RawData() const;

protected:
  nifti_image* m_img;

private:
  NiftiReader();
  NiftiReader(const NiftiReader&);
  NiftiReader operator=(const NiftiReader&);

};

#endif // niftireader_h
