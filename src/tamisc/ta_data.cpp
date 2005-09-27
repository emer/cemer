/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

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

//NOTE: following only called from static New's -- DO NOT CALL FROM CONSTRUCTOR
void DataChannel::InitInstance(const String& name, 
    const int_FixedArray& geom, int txfer_modes_allowed, int txfer_mode)
{
  this->name = name;
  this->setGeom(geom);
  this->m_txfer_modes_allowed = txfer_modes_allowed;
  this->m_txfer_mode = txfer_mode;
}


void DataChannel::Initialize() {
  m_index = -1;
  m_data_type = NULL;
  m_txfer_mode = DTM_PUSH;
  m_txfer_modes_allowed = DTM_PUSH;
  m_cached_cycle = 0;
}

void DataChannel::Destroy() {
  m_cached_data = NULL; // release if any
}

void DataChannel::InitLinks() {
  inherited::InitLinks();
}

void DataChannel::CutLinks() {
  inherited::CutLinks();
}

void DataChannel::setGeom(int dims, int geom0, int geom1, int geom2, int geom3) {
  //note: validation done in setGeom
  int_FixedArray gs(dims);
  if (dims > 0) gs[0] = geom0;
  if (dims > 1) gs[1] = geom1;
  if (dims > 2) gs[2] = geom2;
  if (dims > 3) gs[3] = geom3;
  setGeom(gs);
}

void DataChannel::setGeom(const int_FixedArray& value) {
  if (value.size < 1) {
    taMisc::Error("DataChannel::setGeom", "dims must be >= 1");
    return;
  }
  m_geom = value;
}

void DataChannel::setTxfer_mode(int val) {
  if ((val & txfer_modes_allowed()) != 0) {
    setTxfer_mode_(val);
  } // TODO: else raise exception
}

void DataChannel::setTxfer_mode_(int val) {
  if (val == m_txfer_mode) return;
  m_txfer_mode = val;
  //TODO: notify???
}


/////////////////////////
//  SourceChannel      //
/////////////////////////

SourceChannel* SourceChannel::New(IDataSource* data_source, const String& name, 
    const int_FixedArray& geom, int txfer_modes_allowed, int txfer_mode)
{
  SourceChannel* rval = new SourceChannel();
  rval->m_data_source = data_source;
  rval->InitInstance(name, geom, 
    txfer_modes_allowed, txfer_mode);
  return rval;
}

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

taMatrix_impl* SourceChannel::GetData_() {
  if (SequenceMaster::instance().UpdateClient(m_cached_cycle)) {
    // data is stale
    if (m_data_source != NULL) {
      m_cached_data = m_data_source->GetData(this);
    }
  }
  return m_cached_data;
} 

/////////////////////////
//  SourceChannel_List //
/////////////////////////

void SourceChannel_List::Initialize() {
  SetBaseType(&TA_SourceChannel);
  data_source = NULL;
}

void SourceChannel_List::Destroy() {
  data_source = NULL;
}

void* SourceChannel_List::El_Own_(void* it) {
  void* rval = inherited::El_Own_(it); 
  SourceChannel* src_ch = static_cast<SourceChannel*>(it);
  src_ch->m_data_source = data_source;
  return rval; 
}
void SourceChannel_List::El_disOwn_(void* it) {
  SourceChannel* src_ch = static_cast<SourceChannel*>(it);
  src_ch->m_data_source = NULL;
  return inherited::El_disOwn_(it); 
}

/////////////////////////
//  SinkChannel        //
/////////////////////////

SinkChannel* SinkChannel::New(IDataSink* data_sink, const String& name, 
    const int_FixedArray& geom, int txfer_modes_allowed, int txfer_mode)
{
  SinkChannel* rval = new SinkChannel();
  rval->m_data_sink = data_sink;
  rval->InitInstance(name, geom, 
    txfer_modes_allowed, txfer_mode);
  return rval;
}

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

bool SinkChannel::ConsumeData_() {
  bool rval = false;
  if (m_data_sink != NULL) {
    m_data_sink->DoConsumeData(this, rval);
  }
  return rval;
}
 
void SinkChannel::AcceptData_(taMatrix_impl* data) {
  bool handled = false;
  if (m_data_sink != NULL) {
    m_data_sink->DoAcceptData(this, data, handled);
  }
} 

/////////////////////////
//  SinkChannel_List   //
/////////////////////////

void SinkChannel_List::Initialize() {
  SetBaseType(&TA_SinkChannel);
  data_sink = NULL;
}

void SinkChannel_List::Destroy() {
  data_sink = NULL;
}

void* SinkChannel_List::El_Own_(void* it) {
  void* rval = inherited::El_Own_(it); 
  SinkChannel* src_ch = static_cast<SinkChannel*>(it);
  src_ch->m_data_sink = data_sink;
  return rval; 
}
void SinkChannel_List::El_disOwn_(void* it) {
  SinkChannel* src_ch = static_cast<SinkChannel*>(it);
  src_ch->m_data_sink = NULL;
  return inherited::El_disOwn_(it); 
}


/*obs
/////////////////////////
//  DataSource_impl    //
/////////////////////////

void DataSource_impl::Initialize() {
  data_source = NULL;
}

void DataSource_impl::Destroy() {
}

void DataSource_impl::InitLinks() {
  inherited::InitLinks();
  taBase::Own(m_source_channels, this);
}

void DataSource_impl::CutLinks() {
  m_source_channels.CutLinks();
  inherited::CutLinks();
} */


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
