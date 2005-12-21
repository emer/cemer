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



// ta_data.h -- implementation header for base data system

#ifndef TA_DATA_H
#define TA_DATA_H

#include "tamisc_stdef.h"

#include "ta_matrix.h"
#include "ta_base.h"

#include "ta_data_defs.h"


// forwards this file
class DataChannel;
class SourceChannel;//
class SinkChannel;//
class DataConnector; 
class DataConnector_List; //
class SequenceMaster; 

class DataConnector: public taNBase { // #NO_TOKENS represents a connection between a single source channel and sink channel
friend class SinkChannel;
//friend class ISourceChannel;
#ifndef __MAKETA__
typedef taNBase inherited;
#endif
public:  
  static bool		CanConnect(SourceChannel* src_ch, SinkChannel* snk_ch);
  static DataConnector*	StatConnect(SourceChannel* src_ch, SinkChannel* snk_ch);
  
  SourceChannel* 	source_channel() {return m_source_channel;} 
  SinkChannel* 		sink_channel() {return m_sink_channel;}

  
  override TAPtr 	SetOwner(TAPtr); // #IGNORE
  TA_BASEFUNS(DataConnector); //
  
public: // hidden members
  SinkChannel* 		m_sink_channel;

private:
  void			Initialize();
  void			Destroy();
protected:
  SourceChannel* 	m_source_channel; // #HIDDEN #NO_SAVE set on SetOwner
  virtual void		SinkChannelDisconnecting(SinkChannel* snk_ch);
};


class DataConnector_List: public taList<DataConnector> { // #NO_TOKENS list of connectors, owned by the SourceChannel
#ifndef __MAKETA__
typedef taList<DataConnector> inherited;
#endif
public:  
  TA_BASEFUNS(DataConnector_List);
private:
  void			Initialize();
  void			Destroy(); //
};


/* DataChannel

  A DataChannel is specialized into SourceChannel and SinkChannel.
  We provide generic implementations here that should generally serve most
  purposes, but you can override the implementation if you want.

*/

class DataChannel: public taNBase { // #VIRT_BASE #NO_INSTANCE ##NO_TOKENS a source or sink of data
#ifndef __MAKETA__
typedef taNBase inherited;
#endif
public:
  bool			active; // #DEF_true set on (default) to enable data to flow through this channel
  int_Array		geom; // #SAVE #HIDDEN
  DataTransferMode	txfer_modes_allowed; // #READ_ONLY #SHOW
  DataTransferMode	txfer_mode; // current txfer mode
  
  virtual TypeDef*	data_type() = 0; // type of data, ex TA_int, TA_float, etc.
  virtual int		dims() {return geom.size;}
     // number of dimensions of data; N=0 for sink is "any"
  virtual int		GetGeom(int dim) {return geom.SafeEl(dim);}// geom for dimension
  void			SetGeom(int d0); // set 1-d geom
  void			SetGeom2(int d0, int d1); // set 2-d geom
  void			SetGeom3(int d0, int d1, int d2); // set 3-d geom
  void			SetGeom4(int d0, int d1, int d2, int d3); // set 4-d geom
  virtual void		SetGeomN(const int_Array& value); // set any geom

  virtual void		ClearCachedData(); // clears any cached data from a previous cycle
  
  void			InitLinks();
  void			CutLinks();
  void			Copy_(const DataChannel& cp);
  COPY_FUNS(DataChannel, taNBase);
  TA_ABSTRACT_BASEFUNS(DataChannel); //
  
protected:
  taMatrixPtr_impl	m_cached_data; // most recent data
  int64_t		m_cached_cycle; // cycle counter that goes with data
  
  virtual bool		SetCachedData(taMatrix_impl* data); // validates, sets
  virtual bool		ValidateData(taMatrix_impl* data); // validates, based on expected geom and type
private:
  void			Initialize();
  void			Destroy();
};


class SourceChannel: public DataChannel { // #VIRT_BASE #NO_INSTANCE a source of data
INHERITED(DataChannel)
friend class IDataSource;
friend class DataConnector;
friend class SourceChannel_List;
public:
  DataConnector_List	connectors;
  
  IDataSource*		data_source() {return m_data_source;} // must be set by owner
  
  override TAPtr	SetOwner(TAPtr own); // also sets data source
  
  taMatrix_impl*	GetData() {return DoGetData();}
    // gets latest item, producing it if necessary; only called in Pull mode
    
  void			InitLinks();
  void			CutLinks();
  TA_ABSTRACT_BASEFUNS(SourceChannel); //

public: // hidden
  IDataSource*		m_data_source; // #HIDDEN must be set by owner
  
protected:
//  override void		setTxfer_mode_(int val); // set new txfer mode
  virtual void		ConnectorDisconnecting(DataConnector* dc);
  virtual taMatrix_impl* DoGetData(); // default returns cached data if fresh, else calls produce
  virtual bool		DoProduceData(); // performs actual job of producing data into cache
  
private:
  void			Initialize();
  void			Destroy(); //
};


class SinkChannel: public DataChannel { // #VIRT_BASE #NO_INSTANCE a sink for data
#ifndef __MAKETA__
typedef DataChannel inherited;
#endif
friend class IDataSink;
friend class DataConnector;
public:
  IDataSink*		data_sink() {return m_data_sink;} // the DataSink that owns this channel
  DataConnector* 	connector() {return m_connector;} // the connector, if attached
  
  bool			AcceptData(taMatrix_impl* item) {return DoAcceptData(item);} 
    // sets current item after verifying; only used in Push mode
    
  override TAPtr	SetOwner(TAPtr own); // also sets data sink
  TA_ABSTRACT_BASEFUNS(SinkChannel); //
  
public: // hidden
  IDataSink*		m_data_sink; // #HIDDEN the DataSink that owns this channel
  DataConnector* 	m_connector; // #HIDDEN #SAVE the connector, if attached

protected:
  virtual void		ConnectorDisconnecting(DataConnector* dc);
  virtual bool		DoConsumeData(); // does work of consuming current item, default delegates to sink
  virtual bool		DoAcceptData(taMatrix_impl* item); 
    // accepts current item, calls consume if in push mode
private:
  void			Initialize();
  void			Destroy();
};


class SequenceMaster { // #NO_INSTANCE singleton class
public:
  static SequenceMaster& instance();
  
  int64_t		cycle() {return m_cycle;} // global cycle counter; note: init=1 so all data is stale
  
  void			Next(); // advance the cycle counter

  bool			UpdateClient(int64_t& client_cycle, int64_t* diff = NULL);
    // updates a client's own cycle count, returning 'true' if it was out of date, and optionally the (+ve) number of cycles by which it was stale
private:
  int64_t		m_cycle; // this puppy won't be overflowing anytime soon!
  
  SequenceMaster();
  ~SequenceMaster();
  SequenceMaster(const SequenceMaster& src); // not defined
  void operator =(const SequenceMaster& src); // not defined
};

/* DataCatalogs
   A DataCatalog provides a collection of data items. Examples would be image files
   in a file system folder, or data patterns stored in a database. 
   Usage:
     * set the path to the items (type-dependent, ex. folder)
     * call OpenData()
     * access the normal DataSource interface, data items, etc.
     * when finished, call CloseData() (note: this may do nothing for some types)
     * to reenumerate, call CloseData() followed by OpenData()
   
   A DataCatalogItemSpec 
   
   The Catalog provides the location/path, enumeration, and retrieval functions.

*/

class DataCatalog: public taNBase { // #VIRT_BASE #NO_INSTANCE a Catalog provides a collection of data items
INHERITED(taNBase)
public:

  virtual bool		OpenData() = 0; // opens the data source, returns true if successful
  virtual void		CloseData() = 0; // closes the data source
  
  TA_ABSTRACT_BASEFUNS(DataCatalog)
private:
  void			Initialize() {}
  void			Destroy() {}
};


class DirectoryCatalog: public DataCatalog {// #INSTANCE a DirectoryCatalog provides a collection of data items, described by a DataCatalogItemSpec
INHERITED(DataCatalog)
public:
  String		directory; // path to the directory where the files are
  
  override bool		OpenData(); // opens the data source, returns true if successful
  override void		CloseData(); // closes the data source
  
  TA_BASEFUNS(DirectoryCatalog)
private:
  void			Initialize() {}
  void			Destroy() {}
};


//TODO: prob move these to an _extras file

#endif
