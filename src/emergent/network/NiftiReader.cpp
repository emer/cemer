// Copyright 2013, Regents of the University of Colorado,
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

#include "NiftiReader.h"
#include <taVector3f>
#include <taVector3i>

#include <taMisc>

#include <QList>
#include <QString>
#include <QStringList>

#include <QRegExp>
#include <QFile>
#include <QXmlStreamReader>
#include <QFileInfo>
#include <QDir>
#include <QSet>
#include <QString>
#include <QColor>
#include <QStringList>

/////////////////////////////////////////////////////////////////////

NiftiReader::NiftiReader(const QString& file)
:m_img(0)
{
  m_img = nifti_image_read(file.toStdString().c_str(), 1);
  if (!IsValid())
    taMisc::Warning("Failed to load NIfTI file: ", file.toLatin1() );
}

NiftiReader::~NiftiReader()
{
  if ( IsValid() )
    nifti_image_free(m_img);
}

bool NiftiReader::IsValid() const
{
  return (m_img == 0) ? false : true;
}

taVector3i NiftiReader::XyzDimensions() const
{
  taVector3i dims;
  dims.SetXYZ(m_img->nx, m_img->ny, m_img->nz);
  return dims;
}

int NiftiReader::BytesPerVoxel() const
{
  return m_img->nbyper;
}

float NiftiReader::CalMax() const
{
  return m_img->cal_max;
}

float NiftiReader::CalMin() const
{
  return m_img->cal_min;
}

float NiftiReader::CalRange() const
{
  return (m_img->cal_max - m_img->cal_min);
}

void NiftiReader::SliceXY(int index, unsigned short* data) const
{
  taVector3i size(XyzDimensions());
  int nbytes(BytesPerVoxel());
  int start = size.x * size.y * (index-1);
  memcpy((void*)(data), (void*)((unsigned short*)m_img->data+(start)), size.x * size.y * nbytes);
}

void NiftiReader::Slice(AnatomicalPlane p, int index, unsigned short* data) const
{
  int start(0);
  int nbytes(BytesPerVoxel());
  taVector3i size(XyzDimensions());

  if (p == AXIAL){
    SliceXY(index, data);
  }
  else if (p == CORONAL){
    for (int zz=0; zz<size.z; zz++){
      start = ((index-1) * size.x) + (size.x * size.y * zz);
     memcpy((void*)(data+(size.x*zz)),(void*)((unsigned short*)m_img->data+(start)), size.x * nbytes);
    }
  }
  else { //(p == SAGITTAL)
    int zidx(0),didx(0);
    for (int zz=0; zz<size.z; zz++){
      for (int yy=0; yy<size.y; yy++) {
        didx = (index-1) + (yy*size.x) + (size.x*size.y*zz);
        memcpy((void*)(data+(zidx)),(void*)((unsigned short*)m_img->data+(didx)), nbytes);
        zidx++;
      }
    }
  }
}

int NiftiReader::NumExtensions() const
{
  return m_img->num_ext;
}

int NiftiReader::NumVolumes() const
{
  // returns the number of volumes (fourth dimension) in image
  if (m_img->ndim > 3){
    return m_img->nt;
  }
  else {
    return 0;
  }
}

const void * NiftiReader::RawData() const
{
  return m_img->data;
}

taVector3f NiftiReader::XyzToIjk(const taVector3f &xyz_coord) const
{
  taVector3f ijk_coord(0,0,0);

  if (IsValid()) {
    // Get the transform object from the nifti file -- this is loaded from
    // srow_x[], srow_y[], and srow_z[].
    const mat44 &xf = m_img->sto_xyz;

    // Inverse transform each xyz-based coordinate into an ijk-based coordinate.
    // This is the inverse of the transform method applied in the IjkToXyz()
    // method, which is based on the NIfTI-1 header file.

    // Perform the inverse: translation first, inverse matrix next
    float x = xyz_coord.x - xf.m[0][3];
    float y = xyz_coord.y - xf.m[1][3];
    float z = xyz_coord.z - xf.m[2][3];
    float i = x * xf.m[0][0] + y * xf.m[1][0] + z * xf.m[2][0];
    float j = x * xf.m[0][1] + y * xf.m[1][1] + z * xf.m[2][1];
    float k = x * xf.m[0][2] + y * xf.m[1][2] + z * xf.m[2][2];

    ijk_coord.SetXYZ(i, j, k);
  }
  return ijk_coord;
}

taVector3f NiftiReader::IjkToXyz(const taVector3f &ijk_coord) const
{
  taVector3f xyz_coord(0,0,0);

  if (IsValid()) {
    // Get the transform object from the nifti file -- this is loaded from
    // srow_x[], srow_y[], and srow_z[].
    const mat44 &xf = m_img->sto_xyz;

    // Transform each ijk-based coordinate to an xyz-based coordinate.
    // METHOD 3 (used when sform_code > 0):
    // -----------------------------------
    // The (x,y,z) coordinates are given by a general affine transformation
    // of the (i,j,k) indexes:
    //   x = srow_x[0] * i + srow_x[1] * j + srow_x[2] * k + srow_x[3]
    //   y = srow_y[0] * i + srow_y[1] * j + srow_y[2] * k + srow_y[3]
    //   z = srow_z[0] * i + srow_z[1] * j + srow_z[2] * k + srow_z[3]
    float i = ijk_coord.x;
    float j = ijk_coord.y;
    float k = ijk_coord.z;
    float x = xf.m[0][0] * i + xf.m[0][1] * j + xf.m[0][2] * k + xf.m[0][3];
    float y = xf.m[1][0] * i + xf.m[1][1] * j + xf.m[1][2] * k + xf.m[1][3];
    float z = xf.m[2][0] * i + xf.m[2][1] * j + xf.m[2][2] * k + xf.m[2][3];

    xyz_coord.SetXYZ(x, y, z);
  }
  return xyz_coord;
}

