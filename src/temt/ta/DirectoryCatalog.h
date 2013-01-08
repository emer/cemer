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

#ifndef DirectoryCatalog_h
#define DirectoryCatalog_h 1

// parent includes:
#include <DataBlock_Idx>

// member includes:

// declare all other types mentioned but not required to include:


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

#endif // DirectoryCatalog_h
