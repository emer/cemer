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
class QStringList;
class QString;
class FloatTDCoord;
class TDCoord;
class NiftiReaderPrivate;
class TalairachAtlasPrivate;

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

  bool    isValid() const;
  TDCoord xyzDimensions() const;
  int     bytesPerVoxel() const;
  void    sliceXY(int index, unsigned short* data) const;
  void    slice(AnatomicalPlane p, int index, unsigned short* data) const;
  void    sliceAsTexture(AnatomicalPlane p, int index, unsigned char* data) const;
  int     numExtensions() const;
  FloatTDCoord XyzToIjk(const FloatTDCoord &xyzCoord) const;
  FloatTDCoord IjkToXyz(const FloatTDCoord &ijkCoord) const;

protected:
  const void *rawData() const;

  NiftiReaderPrivate* m_d;

private:
  NiftiReader();
  NiftiReader(const NiftiReader&);
  NiftiReader operator=(const NiftiReader&);

};


class TalairachAtlas : public NiftiReader {
public:
  explicit  TalairachAtlas(const QString& filename);
  virtual   ~TalairachAtlas();

  const QStringList& labels() const;
  QString label(int index) const;
  QList<TDCoord> GetVoxelsInArea(const QString &labelRegexp) const;
  QList<FloatTDCoord> GetVoxelCoords(const QList<TDCoord> &voxelIdxs) const;
  static FloatTDCoord Tal2Mni(const FloatTDCoord &talCoord);

protected:
  TalairachAtlasPrivate* m_d;

private:
  TalairachAtlas();
  TalairachAtlas(const TalairachAtlas&);
  TalairachAtlas operator=(const TalairachAtlas&);

};
#endif // niftireader_h
