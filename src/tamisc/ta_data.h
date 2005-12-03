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
class SourceChannel;
class SourceChannel_List;
class SinkChannel;
class SinkChannel_List;
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

class DataChannel: public taNBase { // #VIRT_BASE #NO_INSTANCE ##NO_TOKENS a connection between a source and sink -- owned by the source list
#ifndef __MAKETA__
typedef taNBase inherited;
#endif
public:
  
  virtual int		index() {return m_index;} // this channel's index, typically invariant
  virtual TypeDef*	data_type() {return m_data_type;} // type of data, ex TA_int, TA_float, etc.
  virtual int		dims() {return m_geom.size;}
     // number of dimensions of data; N=0 for sink is "any"
  virtual int		geom(int dim) {return m_geom.SafeEl(dim);}// geom for dimension; N=0 is "don't care"
  void			setGeom(int dims, int geom0, int geom1 = 0, int geom2 = 0, int geom3 = 0);
    // convenience api that calls virt version
  virtual void		setGeom(const int_FixedArray& value);
  virtual int		txfer_modes_allowed() const {return m_txfer_modes_allowed;}
    // specialized impls can provide pull
  virtual int		txfer_mode() const {return m_txfer_mode;} // current txfer mode
  void			setTxfer_mode(int val); // set new txfer mode (validates before calling _)
  
  void			InitLinks();
  void			CutLinks();
  TA_ABSTRACT_BASEFUNS(DataChannel); //
  
public: // hidden members -- do not access directly
  int_FixedArray	m_geom; // #SAVE #HIDDEN
  int			m_index; // #NO_SAVE #HIDDEN
  int			m_txfer_modes_allowed; // #SAVE #HIDDEN

protected:
  virtual void		InitInstance(const String& name, 
    const int_FixedArray& geom, int txfer_modes_allowed, int txfer_mode);
    // code shared by specialized static New's -- DO NOT CALL FROM CONSTRUCTOR
    
  TypeDef*		m_data_type;
  int			m_txfer_mode; // current txfer mode
  taMatrixPtr_impl	m_cached_data; // most recent data
  int64_t		m_cached_cycle; // cycle counter that goes with data
  virtual void		setTxfer_mode_(int val); // set new txfer mode, typically not overridden
private:
  void			Initialize();
  void			Destroy();
};


class SourceChannel: public DataChannel { // 
INHERITED(DataChannel)
friend class IDataSource;
friend class DataConnector;
friend class SourceChannel_List;
public:
  static SourceChannel*	New(IDataSource* data_source, const String& name, 
    const int_FixedArray& geom, int txfer_modes_allowed, int txfer_mode);
    // this is the normal way to create a Channel
  
  DataConnector_List	connectors;
  
  IDataSource*		data_source() {return m_data_source;} // must be set by owner
  
  taMatrix_impl*	GetData() {return GetData_();}
    // gets current item
    
  void			InitLinks();
  void			CutLinks();
  TA_BASEFUNS(SourceChannel); //

public: // hidden
  IDataSource*		m_data_source; // #HIDDEN must be set by owner
  
protected:
//  override void		setTxfer_mode_(int val); // set new txfer mode
  virtual void		ConnectorDisconnecting(DataConnector* dc);
  virtual taMatrix_impl* GetData_(); // default returns cached data if fresh, else calls source
  
private:
  void			Initialize();
  void			Destroy(); //
};

/* delete
class SourceChannelD<T>: public SourceChannel { // template for a source channel that contains its data, and caches the global cycle counter to mediate execution of makedata
INHERITED(SourceChannel)
friend class IDataSource;
public:
protected:
  T			m_data;
  int64_t		m_data_cycle; // cycle on which data was generated
  override bool MakeData_() {
  bool rval = false;
  if (m_data_source != NULL) {
    m_data_source->DoMakeData(this, rval);
  }
  return rval;
}
 
  override taMatrix_impl* GetData_() {
    if (SequenceMaster::instance().UpdateClient(m_data_cycle)) {
    }
  taMatrix_impl* rval = NULL;
  bool handled = false;
  if (m_data_source != NULL) {
    rval = m_data_source->DoGetData(this, handled);
  }
  return rval;
} 

}; */

class SourceChannel_List: public taList<SourceChannel> { // #NO_TOKENS
#ifndef __MAKETA__
typedef taList<SourceChannel> inherited;
#endif
friend class IDataSource;
friend class SourceChannel;
public:
  IDataSource*		data_source; // #HIDDEN must be set by owning IDataSource
  TA_BASEFUNS(SourceChannel_List); //
  
protected: 
  override void*	El_Own_(void* it);
  override void		El_disOwn_(void* it);
  override void		El_SetIndex_(void* item, int idx) 
    {static_cast<SourceChannel*>(item)->m_index = idx;}
private:
  void			Initialize();
  void			Destroy();
};


class SinkChannel: public DataChannel { // 
#ifndef __MAKETA__
typedef DataChannel inherited;
#endif
friend class IDataSink;
friend class DataConnector;
public:
  static SinkChannel*	New(IDataSink* data_sink, const String& name, 
    const int_FixedArray& geom, int txfer_modes_allowed, int txfer_mode);
    // this is the normal way to create a Channel
    
  IDataSink*		data_sink() {return m_data_sink;} // the DataSink that owns this channel
  DataConnector* 	connector() {return m_connector;} // the connector, if attached
  
  bool			ConsumeData() {return ConsumeData_();} 
    // consumes the current data item INTERNAL; 'true' if data actually consumed
  void			AcceptData(taMatrix_impl* item) {AcceptData_(item);} 
    // sets current item; only used in Push mode
  TA_BASEFUNS(SinkChannel); //
  
public: // hidden
  IDataSink*		m_data_sink; // #HIDDEN the DataSink that owns this channel
  DataConnector* 	m_connector; // #HIDDEN #SAVE the connector, if attached

protected:
  virtual void		ConnectorDisconnecting(DataConnector* dc);
  virtual bool		ConsumeData_(); // does actual work of making current item
  virtual void		AcceptData_(taMatrix_impl* item); 
    // gets current item; only used in Push mode
private:
  void			Initialize();
  void			Destroy();
};

class SinkChannel_List: public taList<SinkChannel> { //  #NO_TOKENS
#ifndef __MAKETA__
typedef taList<SinkChannel> inherited;
#endif
friend class IDataSink;
friend class SinkChannel;
public:
  IDataSink*		data_sink; // #HIDDEN must be set by owning IDataSource
  
  TA_BASEFUNS(SinkChannel_List);
protected: 
  override void*	El_Own_(void* it);
  override void		El_disOwn_(void* it);
  override void		El_SetIndex_(void* item, int idx) 
    {static_cast<SinkChannel*>(item)->m_index = idx;}
private:
  void			Initialize();
  void			Destroy();
};


/* delete
class DataSource_impl: public taOBase { // #NO_INSTANCE #NO_TOKENS helper object that implements most behavior of IDataSource
#ifndef __MAKETA__
typedef taOBase inherited;
#endif
public:
  IDataSource*		data_source; // #HIDDEN 
  SourceChannel_List	m_source_channels; // #HIDDEN
  
  void 			InitLinks(); 
  void 			CutLinks();
  TA_BASEFUNS(DataSource_impl);
private:
  void			Initialize();
  void			Destroy(); //
}; */

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


#endif
