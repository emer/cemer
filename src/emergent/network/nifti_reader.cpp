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

#include "nifti_reader.h"
#include "ta_geometry.h"
#include "ta_type.h"
#include "nifti1_io.h"
#include <QRegExp>
#include <QSet>
#include <QString>
#include <QStringList>


class NiftiReaderPrivate
{
public:
  NiftiReaderPrivate() : img(0) {}

  nifti_image* img;
};

class TalairachAtlasPrivate
{
public:
  TalairachAtlasPrivate() : labels() {}
  QSet<int> getMatchingLabelIdxs(const QString &regexp) const;

  QStringList labels;
};

QSet<int> TalairachAtlasPrivate::getMatchingLabelIdxs(const QString &regexpStr) const
{
  // The set of label indices to return.
  QSet<int> matchingLabelIdxs;

  // Turn the string into a regexp object.
  const QRegExp regexp(regexpStr);

  // Check each label for a match.
  for (int idx = 0; idx < labels.size(); ++idx) {
    if (labels[idx].contains(regexp)) {
      matchingLabelIdxs << idx;
    }
  }

  return matchingLabelIdxs;
}


/////////////////////////////////////////////////////////////////////

NiftiReader::NiftiReader(const QString& file)
:m_d(0)
{
  m_d = new NiftiReaderPrivate();
  m_d->img = nifti_image_read(file.toStdString().c_str(), 1);
  if (!isValid())
    taMisc::Warning("Failed to load NIfTI file: ", file.toLatin1() );
}

NiftiReader::~NiftiReader()
{
  if ( isValid() )
    nifti_image_free(m_d->img);
  delete m_d;
}

bool NiftiReader::isValid() const
{
  return (m_d->img == 0) ? false : true;
}

TDCoord NiftiReader::xyzDimensions() const
{
  TDCoord dims;
  dims.SetXYZ(m_d->img->nx, m_d->img->ny, m_d->img->nz);
  return dims;
}

int NiftiReader::bytesPerVoxel() const
{
  return m_d->img->nbyper;
}

void NiftiReader::sliceXY(int index, unsigned short* data) const
{
  TDCoord size(xyzDimensions());
  int nbytes(bytesPerVoxel());
  int start = size.x * size.y * (index-1);
  memcpy((void*)(data), (void*)((unsigned short*)m_d->img->data+(start)), size.x * size.y * nbytes);
}

void NiftiReader::slice(AnatomicalPlane p, int index, unsigned short* data) const
{
  int start(0);
  int nbytes(bytesPerVoxel());
  TDCoord size(xyzDimensions());

  if (p == AXIAL){
    sliceXY(index, data);
  }
  else if (p == CORONAL){
    for (int zz=0; zz<size.z; zz++){
      start = ((index-1) * size.x) + (size.x * size.y * zz);
     memcpy((void*)(data+(size.x*zz)),(void*)((unsigned short*)m_d->img->data+(start)), size.x * nbytes);
    }
  }
  else { //(p == SAGITTAL)
    int zidx(0),didx(0);
    for (int zz=0; zz<size.z; zz++){
      for (int yy=0; yy<size.y; yy++) {
        didx = (index-1) + (yy*size.x) + (size.x*size.y*zz);
        memcpy((void*)(data+(zidx)),(void*)((unsigned short*)m_d->img->data+(didx)), nbytes);
        zidx++;
      }
    }
  }
}

void NiftiReader::sliceAsTexture(AnatomicalPlane p, int index, unsigned char* data) const
{
  // TODO: this is a crap implementation and should be moved to T3BrainNode!
  TDCoord size(xyzDimensions());
  int planeSize(0);
  if (p == AXIAL)
    planeSize = size.x*size.y;
  else if (p == CORONAL)
    planeSize = size.x*size.z;
  else
    planeSize = size.y*size.z;

  unsigned short* s = new unsigned short[planeSize];
  slice(p, index, s);

  // scale the data by the monitor calibrations
  // and set paired transpaency/alpha bytes
  int pmax = static_cast<int>(m_d->img->cal_max);
  int pmin = static_cast<int>(m_d->img->cal_min);
  int prange = pmax-pmin;
  int j(0);
  int nbytes(bytesPerVoxel());
  unsigned int pixel(0); unsigned char pixel8(0);
  for (int i=0; i<planeSize; i++ )
  {
    if (s[i] > pmax) {
      pixel = pmax;
    }
    else if (s[i] < pmin) {
      pixel = pmin;
    }
    else {
      pixel = s[i];
    }
    pixel8 = static_cast<unsigned char>(((pixel-pmin) * 255) / prange);
    j = i * nbytes;
    data[j] = pixel8;
    data[j+1] = (pixel8 == 0) ? 0 : 255;
  }
  delete [] s;
}

int NiftiReader::numExtensions() const
{
  return m_d->img->num_ext;
}

const void * NiftiReader::rawData() const
{
  return m_d->img->data;
}

FloatTDCoord NiftiReader::XyzToIjk(const FloatTDCoord &xyzCoord) const
{
  FloatTDCoord ijkCoord(0,0,0);

  if (isValid()) {
    // Get the transform object from the nifti file -- this is loaded from
    // srow_x[], srow_y[], and srow_z[].
    const mat44 &xf = NiftiReader::m_d->img->sto_xyz;

    // Inverse transform each xyz-based coordinate into an ijk-based coordinate.
    // This is the inverse of the transform method applied in the IjkToXyz()
    // method, which is based on the NIfTI-1 header file.

    // Perform the inverse: translation first, inverse matrix next
    float x = xyzCoord.x - xf.m[0][3];
    float y = xyzCoord.y - xf.m[1][3];
    float z = xyzCoord.z - xf.m[2][3];
    float i = x * xf.m[0][0] + y * xf.m[1][0] + z * xf.m[2][0];
    float j = x * xf.m[0][1] + y * xf.m[1][1] + z * xf.m[2][1];
    float k = x * xf.m[0][2] + y * xf.m[1][2] + z * xf.m[2][2];

    ijkCoord.SetXYZ(i, j, k);
  }
  return ijkCoord;
}

FloatTDCoord NiftiReader::IjkToXyz(const FloatTDCoord &ijkCoord) const
{
  FloatTDCoord xyzCoord(0,0,0);

  if (isValid()) {
    // Get the transform object from the nifti file -- this is loaded from
    // srow_x[], srow_y[], and srow_z[].
    const mat44 &xf = NiftiReader::m_d->img->sto_xyz;

    // Transform each ijk-based coordinate to an xyz-based coordinate.
    // METHOD 3 (used when sform_code > 0):
    // -----------------------------------
    // The (x,y,z) coordinates are given by a general affine transformation
    // of the (i,j,k) indexes:
    //   x = srow_x[0] * i + srow_x[1] * j + srow_x[2] * k + srow_x[3]
    //   y = srow_y[0] * i + srow_y[1] * j + srow_y[2] * k + srow_y[3]
    //   z = srow_z[0] * i + srow_z[1] * j + srow_z[2] * k + srow_z[3]
    float i = ijkCoord.x;
    float j = ijkCoord.y;
    float k = ijkCoord.z;
    float x = xf.m[0][0] * i + xf.m[0][1] * j + xf.m[0][2] * k + xf.m[0][3];
    float y = xf.m[1][0] * i + xf.m[1][1] * j + xf.m[1][2] * k + xf.m[1][3];
    float z = xf.m[2][0] * i + xf.m[2][1] * j + xf.m[2][2] * k + xf.m[2][3];

    xyzCoord.SetXYZ(x, y, z);
  }
  return xyzCoord;
}

/////////////////////////////////////////////////////////////////////

TalairachAtlas::TalairachAtlas(const QString& filename)
  : NiftiReader(filename)
  , m_d(0)
{
  m_d = new TalairachAtlasPrivate();
  if (isValid() && (numExtensions() > 0)) {
    QString s(NiftiReader::m_d->img->ext_list[0].edata);
    m_d->labels = s.split("\n");

    // Get rid of any empty labels due to trailing newline.
    while (m_d->labels.back().isEmpty()) {
      m_d->labels.removeLast();
    }
  }
}

TalairachAtlas::~TalairachAtlas()
{
  delete m_d;
}

const QStringList& TalairachAtlas::labels() const
{
  return m_d->labels;
}

QString TalairachAtlas::label(int index) const
{
  return m_d->labels.value(index);
}

QList<TDCoord>
TalairachAtlas::GetVoxelsInArea(const QString &labelRegexp) const
{
  // The list of voxels to be returned.
  QList<TDCoord> voxels;

  if (isValid()) {
    // Get the set of all matching label indices.
    QSet<int> matchingLabelIdxs = m_d->getMatchingLabelIdxs(labelRegexp);

    // Get the dimensions of the data and a pointer to the data.
    // The data has already been byte-swapped for this platform.
    TDCoord size(xyzDimensions());
    const short *data = reinterpret_cast<const short *>(rawData());

    // i,j,k are the array indices in the x,y,z dimensions, respectively.
    for (int k = 0; k < size.z; ++k) {
      for (int j = 0; j < size.y; ++j) {
        for (int i = 0; i < size.x; ++i) {
          short labelIdx = *data++;
          bool match = matchingLabelIdxs.contains(labelIdx);
          if (match) {
            voxels << TDCoord(i, j, k);
          }
        }
      }
    }
  }

  return voxels;
}

QList<FloatTDCoord> TalairachAtlas::GetVoxelCoords(const QList<TDCoord> &voxelIdxs) const
{
  QList<FloatTDCoord> voxelCoords;

  if (isValid()) {
    // Transform each ijk-based coordinate to an xyz-based coordinate.
    foreach(const TDCoord &ijkCoord, voxelIdxs) {
      voxelCoords << IjkToXyz(ijkCoord);
    }
  }
  return voxelCoords;
}


FloatTDCoord TalairachAtlas::Tal2Mni(const FloatTDCoord &talCoord)
{
  // Adapted from the tal2icbm.m MATLAB file available at:
  // http://brainmap.org/icbm2tal/
  //
  // This function converts coordinates from Talairach space to MNI
  // space (normalized using the FSL software package) using the
  // tal2icbm transform developed and validated by Jack Lancaster
  // at the Research Imaging Center in San Antonio, Texas.
  //
  // ric.uthscsa.edu 3/14/07
  // http://www3.interscience.wiley.com/cgi-bin/abstract/114104479/ABSTRACT

  // Inverted icbm_fsl transformation matrix
  float xf[4][4] = {
    {  1.0566F, -0.0040F, 0.0028F,  1.1155F },
    {  0.0088F,  1.0505F, 0.0677F,  0.8694F },
    { -0.0068F, -0.0719F, 1.1052F, -3.6047F },
    {  0.0000F,  0.0000F, 0.0000F,  1.0000F }
  };

  int   x = talCoord.x;
  int   y = talCoord.y;
  int   z = talCoord.z;
  float i = x * xf[0][0] + y * xf[0][1] + z * xf[0][2] + 1.00 * xf[0][3];
  float j = x * xf[1][0] + y * xf[1][1] + z * xf[1][2] + 1.00 * xf[1][3];
  float k = x * xf[2][0] + y * xf[2][1] + z * xf[2][2] + 1.00 * xf[2][3];

  return FloatTDCoord(i, j, k);
}
