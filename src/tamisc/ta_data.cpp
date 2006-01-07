// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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



#include "ta_data.h"

/*
  TODO
  * proper copy semantics for all objects (could be tough...)
  
*/

/////////////////////////
//  DataChannel        //
/////////////////////////

void DataChannel::Initialize() {
  active = true;
  matrix_type = &TA_float_Matrix; // the most common is the default; replace if different
}

void DataChannel::Destroy() {
  m_cached_data = NULL; // release if any
  CutLinks();
}

void DataChannel::InitLinks() {
  inherited::InitLinks();
  taBase::Own(geom, this);
}

void DataChannel::CutLinks() {
  geom.CutLinks();
  inherited::CutLinks();
}

void DataChannel::Copy_(const DataChannel& cp) {
  active = cp.active;
  geom = cp.geom;
  matrix_type = cp.matrix_type;
  m_cached_data = NULL; // invalidate cache
}

void DataChannel::ClearCachedData() {
  m_cached_data = NULL; // derefs
}

bool DataChannel::SetCachedData(taMatrix_impl* data) {
  bool rval = ValidateData(data);
  if (rval) {
    m_cached_data = data;
  }
  return rval;
}

void DataChannel::SetGeom(int d0)  {
  //note: validation done in SetGeom
  int_Array gs(1);
  gs[0] = d0;
  SetGeomN(gs);
}

void DataChannel::SetGeom2(int d0, int d1) {
  //note: validation done in SetGeom
  int_Array gs(2);
  gs[0] = d0;
  gs[1] = d1;
  SetGeomN(gs);
}

void DataChannel::SetGeom3(int d0, int d1, int d2) {
  //note: validation done in SetGeom
  int_Array gs(3);
  gs[0] = d0;
  gs[1] = d1;
  gs[2] = d2;
  SetGeomN(gs);
}

void DataChannel::SetGeom4(int d0, int d1, int d2, int d3) {
  //note: validation done in SetGeom
  int_Array gs(4);
  gs[0] = d0;
  gs[1] = d1;
  gs[2] = d2;
  gs[3] = d3;
  SetGeomN(gs);
}


void DataChannel::SetGeomN(const int_Array& value) {
  if (value.size < 1) {
    taMisc::Error("DataChannel::SetGeom", "dims must be >= 1");
    return;
  }
  //TODO: validate
  geom = value;
}

int DataChannel::size() {
  if (geom.size == 0) return 0;
  int rval = geom[0];
  for (int i = 1; i < geom.size; ++i)
    rval *= geom[i];
  return rval;
}

bool DataChannel::ValidateData(taMatrix_impl* data) {
  // check not null
  if (data == NULL) {
    taMisc::Warning(this->GetPath(), "data is NULL");
    return false;
  }
  
  // check has correct type
  TypeDef* td = data->GetTypeDef();
  if ((td == NULL) || (!td->InheritsFrom(matrix_type))) {
    taMisc::Warning(this->GetPath(), "data is wrong data type");
    return false;
  }
  
  // check has correct geom
  if (data->dims() != dims()) {
    taMisc::Warning(this->GetPath(), "data has wrong number of dimensions");
    return false;
  }
  for (int i = 0; i < dims(); ++i) {
    int dim = this->GetGeom(i);
    if ((dim != 0) && (data->GetGeom(i) != dim)) {
      taMisc::Warning(this->GetPath(), "data has wrong geometry");
      return false;
    }
  }
  return true;
}


/////////////////////////
//  DataChannel_Group  //
/////////////////////////

void DataChannel_Group::CutLinks() {
  ClearCachedData();
  inherited::CutLinks();
}

void DataChannel_Group::ClearCachedData() {
  DataChannel* chan;
  taLeafItr itr;
  FOR_ITR_EL(DataChannel, chan, this->, itr) {
    chan->ClearCachedData();
  }
}


/////////////////////////
//  SequenceMaster     //
/////////////////////////

// Note: this singleton uses the safe singleton pattern

SequenceMaster& SequenceMaster::instance() {
  static SequenceMaster	s_instance; // actually initialized first time through here
  return s_instance;
}
  
SequenceMaster::SequenceMaster()
:m_cycle(1)
{
}

SequenceMaster::~SequenceMaster()
{
}

void SequenceMaster::Next() {
  ++m_cycle;
}

bool SequenceMaster::UpdateClient(int64_t& client_cycle, int64_t* diff) {
  // updates a client's own cycle count, returning 'true' if it was out of date, and optionally the (+ve) number of cycles by which it was stale
  if (m_cycle <= client_cycle) return false;
  if (diff != NULL) *diff = m_cycle - client_cycle;
  client_cycle = m_cycle;
  return true;
}


/////////////////////////
//  DirectoryCatalog   //
/////////////////////////

bool DirectoryCatalog::OpenData() {
 //TODO
}

void DirectoryCatalog::CloseData() {
  //TODO
}
