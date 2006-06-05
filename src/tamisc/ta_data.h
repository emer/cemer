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


#include "ta_matrix.h"
#include "ta_base.h"
#include "ta_group.h"

#include "tamisc_def.h"
#include "tamisc_TA_type.h"


class TAMISC_API DataBlock: public taNBase { // #VIRT_BASE #NO_INSTANCE ##TOKENS base class for objects that provide and/or accept data
INHERITED(taNBase)
public:
  enum DBOptions { // #BITS datablock options -- they also have individual convenience accessors
    DB_NONE		= 0,      // #NO_BIT 
    DB_INDEXABLE	= 0x0001, // items can be access randomly, by index
    DB_SEQUENCABLE	= 0x0002, // items can be accessed sequentially (note: usually true)
    DB_SOURCE		= 0x0004, // is a data source (provides data, "read")
    DB_SINK		= 0x0008 // is a data sink (accepts data, "write")
#ifndef __MAKETA__
    ,DB_IND_SEQ_SRC_SNK = 0x000F
#endif
  };
  virtual DBOptions	dbOptions() const = 0; // options the instance type support
  
  inline bool		isIndexable() const {return (dbOptions() & DB_INDEXABLE);} 
    // 'true' if can be accessed by index
  inline bool		isSequencable() const {return (dbOptions() & DB_SEQUENCABLE);} 
    // 'true' if can be accessed sequentially
  inline bool		isSource() const {return (dbOptions() & DB_SOURCE);} 
    // 'true' if is a data source
  inline bool		isSink() const {return (dbOptions() & DB_SINK);} 
    // 'true' if is a data sink
  
  virtual int		itemCount() const {return 0;} 
    // number of items (if indexable)
  
  virtual void		ResetData() = 0; // for supported devices, clears all the data (but not the schema)
  TA_ABSTRACT_BASEFUNS(DataBlock); //
  
public: // DataSource i/f
  virtual int		sourceChannelCount() const {return 0;}
    // number of source channels
  inline bool		sourceChannelInRange(int chan) const
    {return ((chan >= 0) && (chan < sourceChannelCount()));}
  virtual const String	sourceChannelName(int chan) const
    {return _nilString;}
  virtual bool		sourceItemAvailable() const {return false;}
    // true when a valid item is available for reading
  
  virtual int		GetSourceChannelIndexByName(const String& ch_nm)
    {int scc = sourceChannelCount();  for (int i; i < scc; ++i) 
     if (ch_nm == sourceChannelName(i)) return i;   return -1;}
    // get the channel number for the name; -1 if none
  virtual void		ReadInit() = 0;
    // initializes sequential read iteration
  virtual bool		ReadNext() = 0;
    // read next item, 'true' if item available
  virtual bool		ReadItem(int idx) = 0; 
    // if indexable, goes to item idx, 'true' if item exists and was read
  
  const Variant		GetData(int chan) 
    {if (sourceItemAvailable() && sourceChannelInRange(chan)) 
       return GetData_impl(chan);
     else return _nilVariant;}
  const Variant		GetDataByChannelName(const String& ch_nm) 
    {int chan; if (sourceItemAvailable() &&
     ((chan = GetSourceChannelIndexByName(ch_nm)) >= 0)) 
       return GetData_impl(chan);
     else return _nilVariant;}
 
  taMatrix*		GetMatrixData(int chan) 
    {if (sourceItemAvailable() && sourceChannelInRange(chan))
       return GetMatrixData_impl(chan);
     else return NULL;}
    // get Matrix data; note: you should ref/unref the matrix
  taMatrix*		GetMatrixDataByChannelName(const String& ch_nm) 
    {int chan; if (sourceItemAvailable() &&
     ((chan = GetSourceChannelIndexByName(ch_nm)) >= 0)) 
       return GetMatrixData_impl(chan);
     else return NULL;}
    // get Matrix data; note: you should ref/unref the matrix
protected: // DataSource i/f
  virtual const Variant	GetData_impl(int chan) {return _nilVariant;}
    // get data at current position
  virtual taMatrix*	GetMatrixData_impl(int chan) {return NULL;}
    // get matrix data at current position
  virtual bool		ReadItem_impl() {return true;} 
    // read the item at current position
  
  
private:
  void			Initialize() {}
  void			Destroy() {}
};




class TAMISC_API DataBlock_Idx: public DataBlock { 
 // #VIRT_BASE #NO_INSTANCE partial implementation for an indexable data block
public: // DataSource i/f
  override bool		sourceItemAvailable() const
    {return ((rd_itr >= 0) && (rd_itr < itemCount()));}
  
  override void		ReadInit() {rd_itr = -1;}
    // initializes sequential read iteration
  override bool		ReadNext() 
    {if (rd_itr < -1) return false;  ++rd_itr;
     if (rd_itr >= itemCount()) {rd_itr = -2; return false;}
     return ReadItem_impl();} 
    //  advance itr; read next item, 'true' if item available
  override bool		ReadItem(int idx) 
    {if ((idx < 0) || (idx >= itemCount())) return false;
     rd_itr = idx;  return ReadItem_impl();} 
    // if indexable, goes to item idx, 'true' if item exists and was read
  
  void	Copy_(const DataBlock_Idx& cp)
     {rd_itr = -2;}
  COPY_FUNS(DataBlock_Idx, DataBlock)
  TA_ABSTRACT_BASEFUNS(DataBlock_Idx); //
  
protected:
  int			rd_itr;  // an int iterator for reading
    // -3=ReadItem error, -2=EOF, -1=BOF, >=0 is valid item
     
private:
  void			Initialize() {rd_itr = -2;}
  void			Destroy() {}
};

/*TODO
class TAMISC_API IDataSink: public virtual IDataSourceSink  { 
// #VIRT_BASE #NO_INSTANCE #NO_TOKENS represents a consumer of data
friend class SinkChannel;
public:
  virtual int		sinkChannelCount() const // number of sink channels
    {return channelCount();}
  inline bool		sinkChannelInRange(int chan) const
    {return ((chan >= 0) && (chan < sinkChannelCount()));}
  virtual const String	sinkChannelName(int idx) const
    {return channelName(chan);}
  
  virtual int		GetSinkChannelIndexByName(const String& ch_nm)
    {int scc = sinkChannelCount();  for (int i; i < scc; ++i) 
     if (ch_nm == sinkChannelName(i)) return i;   return -1;}
    // get the channel number for the name; -1 if none
  virtual void		WriteInit() {wr_itr = -1;} 
    // initializes write iteration
  virtual bool		WriteNext()
    {bool rval = WriteItem(wr_itr); if (rval) BumpItr(rd_itr); return rval;} 
    // goes to next item, creating a new one if at end; 'true' if item available
  virtual bool		WriteItem(int idx) {return false;} 
  // if indexable, goes to item idx, if 1+end, Adds a new item; 'true' if item available
  virtual void		WriteDone() {} 
    // call after writing all channels of the item, for impl-dependent commit
  
  void			SetData(const Variant& data, int chan) 
    {if (sinkChannelInRange(chan)) SetData_impl(data, wr_itr, chan);}
  void			SetDataByChannelName(const Variant& data, const String& ch_nm) 
    {SetData(data, wr_itr, GetSinkChannelIndexByName(ch_nm));}
 
  void			SetMatrixData(const taMatrix* data, int chan) 
    {if (sinkChannelInRange(chan)) SetMatrixData_impl(data, rd_itr, chan);}
    // set the data from a Matrix
  void			SetMatrixDataByChannelName(const taMatrix* data, const String& ch_nm) 
    {SetMatrixData(data, GetSinkChannelIndexByName(ch_nm));}
    // set the data from a Matrix
protected:
  int			wr_itr;  // an int iterator for writing
  virtual bool		AddItem_impl() = 0; // implements the adding of a new item
  virtual void		SetData_impl(const Variant& data, int idx, int chan) = 0;
  virtual void		SetMatrixData_impl(const taMatrix* data, int idx, int chan) = 0;
  virtual bool		ReadItem_impl() {return true;} // read the itr item, itr validated
};

*/


/* OBS
// forwards this file
class DataChannel;
class DataChannel_Group;
class ISequencable;
class IDataSource;
class IDataSink; //
//class SequenceMaster; 


// DataChannel

//  A DataChannel is specialized into SourceChannel and SinkChannel.
//  We provide generic implementations here that should generally serve most
//  purposes, but you can override the implementation if you want.



class TAMISC_API DataChannel: public taNBase { // #INSTANCE ##NO_TOKENS a source or sink of data
INHERITED(taNBase)
public:
  bool			active; // #DEF_true set on (default) to enable data to flow through this channel
  int_Array		geom; // #SAVE #HIDDEN
  TypeDef*		matrix_type; // #NO_NULL #TYPE_taMatrix_impl type of matrix, ex float_Matrix, int_Matrix, etc., note: def is float
  taMatrix*	cached_data() const {return m_cached_data;}  
  virtual int		dims() {return geom.size;}
  int			size(); // number of els in all dims, 0 if any are 0
     // number of dimensions of data; N=0 for sink is "any"
  virtual int		GetGeom(int dim) {return geom.SafeEl(dim);}// geom for dimension
  void			SetGeom(int d0); // set 1-d geom
  void			SetGeom2(int d0, int d1); // set 2-d geom
  void			SetGeom3(int d0, int d1, int d2); // set 3-d geom
  void			SetGeom4(int d0, int d1, int d2, int d3); // set 4-d geom
  virtual void		SetGeomN(const int_Array& value); // set any geom

  virtual void		ClearCachedData(); // clears any cached data
  virtual bool		SetCachedData(taMatrix* data); // validates, sets if valid, true if set
  virtual bool		ValidateData(taMatrix* data); // validates, based on expected geom and type
  
  void			InitLinks();
  void			CutLinks();
  void			Copy_(const DataChannel& cp);
  COPY_FUNS(DataChannel, taNBase);
  TA_BASEFUNS(DataChannel); //
  
protected:
  taMatrixPtr	m_cached_data; // #NO_SAVE most recent data set/get for this channel
  
private:
  void			Initialize();
  void			Destroy();
};


class TAMISC_API DataChannel_Group: public taGroup<DataChannel> { // groups of channels
INHERITED(taGroup<DataChannel>)
public:
  virtual void		ClearCachedData(); // clear cached data of all channels
  
  override void		CutLinks(); // clear cached data as early as possible, to avoid issues
  TA_BASEFUNS(DataChannel_Group); //
  
private:
  void			Initialize() {}
  void			Destroy() {CutLinks();}
};


class TAMISC_API ISequencable { // #VIRT_BASE #NO_INSTANCE #NO_TOKENS interface exposed by entities that can be sequenced, particularly DataSources;\n only one instance of this interface is allowed per data block
public:
  virtual int		num_items() {return -1;} // N<0 if items unknown, or cannot be accessed randomly
  virtual bool		is_indexable() {return false;} // 'true' if can be accessed by index
  virtual bool		is_sequential() {return false;} // 'true' if can be accessed sequentially
  
  virtual void		InitData() {} // initializes data system (ex. clears cache, sets state to 0, enumerates count, etc.)
  virtual void		ResetData() {InitData();} // if defined, clears existing data (impl should also call InitData)
  virtual bool		NextItem()  {return false;} // for seq access, goes to the next item, 'true' if there was a next item
  virtual void		GoToItem(int index) {} // for indexed access, goes to the item 
  
  virtual ~ISequencable() {};
};


class TAMISC_API IDataSourceSink { // #VIRT_BASE #NO_INSTANCE #NO_TOKENS common interface for DataSink and/or DataSource
public:
  virtual int		itemCount() const {return -1;} // N<0 if items unknown, or cannot be accessed randomly
  virtual bool		isIndexable() const {return false;} // 'true' if can be accessed by index
  virtual bool		isSequential() const {return false;} // 'true' if can be accessed sequentially
  
  virtual ~IDataSourceSink() {};
};



class TAMISC_API IDataSource: public virtual IDataSourceSink { // #VIRT_BASE #NO_INSTANCE #NO_TOKENS represents a source of data
friend class SourceChannel;
public:
  virtual int		sourceChannelCount() const = 0; // number of source channels
  virtual DataChannel* 	sourceChannel(int idx) const = 0; // get a source channel
  
  virtual void		ReadInit() {} // if sequencable, initializes read iteration
  virtual bool		ReadNext() {return false;} // if sequencable, goes to next item, 'true' if item available
  virtual bool		ReadItem(int idx) {return false;} // if indexable, goes to item N, 'true' if item exists
  
  virtual bool		GetData(taMatrixPtr& data, int chan = 0) // get a single channel's data
    {if ((chan >= 0) && (chan < sourceChannelCount())) {
       data = sourceChannel(chan)->cached_data(); return true;
     } else return false;}
  virtual bool		GetDataMulti(MatrixPtr_Array& data) 
    {for (int i = 0; i < data.size; ++i) if (!GetData(data.FastEl(i), i)) return false; return true;}
    // get all channels of data
    
  ~IDataSource() {} //
};

class TAMISC_API IDataSink: public virtual IDataSourceSink  { // #VIRT_BASE #NO_INSTANCE #NO_TOKENS represents a consumer of data
friend class SinkChannel;
public:
  virtual int		sinkChannelCount() const = 0; // number of sink channels
  virtual DataChannel* 	sinkChannel(int idx) const = 0; // get a sink channel
  
  virtual void		WriteInit() {} // if sequencable, initializes write iteration
  virtual bool		WriteNext() {return false;} // if sequencable, goes to next item, 'true' if item available
  virtual bool		WriteItem(int idx) {return false;} // if indexable, goes to item N, 'true' if item exists
  virtual void		WriteDone() {} // call after writing all channels of the item, for impl-dependent commit
  
  virtual bool		SetData(taMatrixPtr& data, int chan = 0) // set a single channel's data
    {if ((chan >= 0) && (chan < sinkChannelCount()))
       return sinkChannel(chan)->SetCachedData(data);
     else return false;}
  virtual bool		SetDataMulti(MatrixPtr_Array& data) 
    {for (int i = 0; i < data.size; ++i) if (!SetData(data.FastEl(i), i)) return false; return true;}
    // set all channels of data
    
  ~IDataSink() {} //
};
*/
/*nn??
class TAMISC_API DataBlock: public taNBase, public ISequencable {
 // #VIRT_BASE #NO_INSTANCE #NO_TOKENS generic base for sources, sinks, and filters
INHERITED(taNBase)
public:
  DataChannel_Group	channels;

  virtual void		ClearCachedData(); // clears all cached data in channels
  
  TA_ABSTRACT_BASEFUNS(DataChannel); //
  
public: // ISequencable i/f DO NOT OVERRIDE THESE AGAIN, use the _impl functions if needed
  virtual int		num_items() {return -1;} // N<0 if items unknown, or cannot be accessed randomly
  virtual bool		is_indexable() {return false;} // 'true' if can be accessed by index
  virtual bool		is_sequential() {return false;} // 'true' if can be accessed sequentially
  
  virtual void		InitData() {} // initializes data system (ex. clears cache, sets state to 0, enumerates count, etc.)
  virtual void		ResetData() {} // if defined, clears existing data (impl should also call InitData)
  virtual bool		NextItem()  {return false;} // for seq access, goes to the next item, 'true' if there was a next item
  virtual void		GoToItem(int index) {} // for indexed access, goes to the item 

protected:
  int			m_cur_item; // for pseudo-sequential access

private:
  void 		Initialize();
  void		Destroy();
}; */

/* prob NN
class TAMISC_API SequenceMaster { // #NO_INSTANCE singleton class
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
*/
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
/*TODO
class TAMISC_API DataCatalog: public taNBase { // #VIRT_BASE #NO_INSTANCE a Catalog provides a collection of data items
INHERITED(taNBase)
public:

  virtual bool		OpenData() = 0; // opens the data source, returns true if successful
  virtual void		CloseData() = 0; // closes the data source
  
  TA_ABSTRACT_BASEFUNS(DataCatalog)
private:
  void			Initialize() {}
  void			Destroy() {}
};


class TAMISC_API DirectoryCatalog: public DataCatalog {// #INSTANCE a DirectoryCatalog provides a collection of data items, described by a DataCatalogItemSpec
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

*/
//TODO: prob move these to an _extras file

#endif
