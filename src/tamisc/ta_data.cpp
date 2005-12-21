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
//  DataConnector      //
/////////////////////////

bool DataConnector::CanConnect(SourceChannel* src_ch, SinkChannel* snk_ch) {
  // sink must be unconnected
  // must have commensurable geometries
//TEMP
return true;
}

DataConnector* DataConnector::StatConnect(SourceChannel* src_ch, SinkChannel* snk_ch) {
  //NOTE: assumes validation already done
  DataConnector* rval = new DataConnector();
  src_ch->connectors.Add(src_ch);
  rval->m_source_channel = src_ch;
  snk_ch->m_connector = rval;
  rval->m_sink_channel = snk_ch;
  return rval;
}

void DataConnector::Initialize() {
  m_source_channel = NULL;
  m_sink_channel = NULL;
}

void DataConnector::Destroy() {
  if (m_source_channel != NULL) {
    m_source_channel->ConnectorDisconnecting(this);
    m_source_channel = NULL;
  }
  if (m_sink_channel != NULL) {
    m_sink_channel->ConnectorDisconnecting(this);
    m_sink_channel = NULL;
  }
}

TAPtr DataConnector::SetOwner(TAPtr own) {
  TAPtr rval = inherited::SetOwner(own);
  m_source_channel = GET_MY_OWNER(SourceChannel); // NULL if not owned as such
  return rval;
}

void DataConnector::SinkChannelDisconnecting(SinkChannel* snk_ch) {
  if (m_sink_channel != snk_ch) return;
  m_sink_channel = NULL;
  //TODO: maybe should delete ourself???
}


/////////////////////////
//  DataConnector_List //
/////////////////////////

void DataConnector_List::Initialize() {
  SetBaseType(&TA_DataConnector);
}

void DataConnector_List::Destroy() {
}

/////////////////////////
//  DataChannel        //
/////////////////////////

void DataChannel::Initialize() {
  active = true;
  txfer_modes_allowed = DTM_BOTH;
  txfer_mode = DTM_PUSH;
  m_cached_cycle = 0;
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
  txfer_modes_allowed = cp.txfer_modes_allowed;
  txfer_mode = cp.txfer_mode;
  m_cached_cycle = 0;
}

void DataChannel::ClearCachedData() {
  m_cached_data = NULL; // derefs
  m_cached_cycle = -1;
}

bool DataChannel::SetCachedData(taMatrix_impl* data) {
  bool rval = ValidateData(data);
  if (rval) {
    m_cached_data = data;
    m_cached_cycle = SequenceMaster::instance().cycle();
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

bool DataChannel::ValidateData(taMatrix_impl* data) {
  // check not null
  if (data == NULL) {
    taMisc::Warning(this->GetPath(), "data is NULL");
    return false;
  }
  
  // check has correct type
  TypeDef* td = data->data_type();
  if ((td == NULL) || (!td->InheritsFrom(td))) {
    taMisc::Warning(this->GetPath(), "data is wrong data type");
    return false;
  }
  
  // check has correct geom
  if (data->dims() != dims()) {
    taMisc::Warning(this->GetPath(), "data has wrong number of dimensions");
    return false;
  }
  for (int i = 0; i < dims(); ++i) {
    if (data->GetGeom(i) != this->GetGeom(i)) {
      taMisc::Warning(this->GetPath(), "data has wrong geometry");
      return false;
    }
  }
  
  return true;
}

/////////////////////////
//  SourceChannel      //
/////////////////////////

void SourceChannel::Initialize() {
  m_data_source = NULL;
}

void SourceChannel::Destroy() {
  m_data_source = NULL;
}

void SourceChannel::InitLinks() {
  inherited::InitLinks();
  taBase::Own(connectors, this);
}

void SourceChannel::CutLinks() {
  connectors.CutLinks();
  inherited::CutLinks();
}

void SourceChannel::ConnectorDisconnecting(DataConnector* dc) {
  connectors.Remove(dc);
}

bool SourceChannel::DoProduceData() {
  bool rval = false;
  if (m_data_source != NULL) {
    taMatrix_impl* data = NULL;
    m_data_source->DoProduceData(this, data, rval);
    if (rval) {
      SetCachedData(data);
    }
  }
  return rval;
}
 
taMatrix_impl* SourceChannel::DoGetData() {
  if (SequenceMaster::instance().UpdateClient(m_cached_cycle)) {
    // data is stale
    DoProduceData();
  }
  return m_cached_data;
} 

TAPtr SourceChannel::SetOwner(taBase* own) {
  TAPtr rval = inherited::SetOwner(own);
  TAPtr ods = own->GetOwner();
  while (ods != NULL) {
    if (ods->InheritsFrom(TA_IDataSource)) break;
    
    ods = ods->GetOwner();
  }
  if (ods == NULL) {
    m_data_source = NULL;
  } else {
    m_data_source = dynamic_cast<IDataSource*>(ods);
  }
  return rval;
}


/////////////////////////
//  SinkChannel        //
/////////////////////////

void SinkChannel::Initialize() {
  m_data_sink = NULL;
  m_connector = NULL;
}

void SinkChannel::Destroy() {
  if (m_connector != NULL) {
    m_connector->SinkChannelDisconnecting(this);
    m_connector = NULL;
  }
  m_data_sink = NULL;
}

void SinkChannel::ConnectorDisconnecting(DataConnector* dc) {
  m_connector = NULL;
}

bool SinkChannel::DoConsumeData() {
  bool rval = false;
  if (m_data_sink != NULL) {
    m_data_sink->DoConsumeData(this, rval);
  }
  return rval;
}
 
bool SinkChannel::DoAcceptData(taMatrix_impl* data) {
  return SetCachedData(data);
} 

TAPtr SinkChannel::SetOwner(taBase* own) {
  TAPtr rval = inherited::SetOwner(own);
  TAPtr ods = own->GetOwner();
  while (ods != NULL) {
    if (ods->InheritsFrom(TA_IDataSink)) break;
    
    ods = ods->GetOwner();
  }
  if (ods == NULL) {
    m_data_sink = NULL;
  } else {
    m_data_sink = dynamic_cast<IDataSink*>(ods);
  }
  return rval;
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
