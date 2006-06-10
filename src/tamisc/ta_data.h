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
    DB_SINK		= 0x0008, // is a data sink (accepts data, "write")
    DB_SINK_GROWABLE	= 0x0010  // a sink that can be extended (via AddItem)
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
  inline bool		isSinkGrowable() const {return (dbOptions() & DB_SINK_GROWABLE);} 
    // 'true' if sink is growable (via AddItem)
  
  virtual int		itemCount() const {return 0;} 
    // number of items (if indexable)
  
  virtual void		ResetData() {} // for supported devices, clears all the data (but not the schema)
  TA_ABSTRACT_BASEFUNS(DataBlock); //
  
public: // Properties i/f
  virtual void		SetProperty(const String& name, const Variant& value) {}
  virtual const Variant GetProperty(const String& name) const {return _nilVariant;}
  
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
    {int scc = sourceChannelCount();  for (int i = 0; i < scc; ++i) 
     if (ch_nm == sourceChannelName(i)) return i;   return -1;}
    // get the channel number for the name; -1 if none
    
  bool			ReadOpen() {bool ok = true; ReadOpen_impl(ok); 
    return ok;}
    // opens the block for read operation -- must be called after changing params
  void			ReadClose() {ReadClose_impl();}
    // closes the block for read operation -- call when done
  bool			ReadFirst() {ReadItrInit(); return ReadNext();}
    // (re-)initializes sequential read iteration, reads first item
  virtual bool		ReadNext() {return false;}
    // if sequencable, read next item, 'true' if item available
  virtual bool		ReadItem(int idx) {return false;} 
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
  virtual void		ReadItrInit() {}
  virtual void		ReadOpen_impl(bool& ok) {} // extend
  virtual void		ReadClose_impl() {} // extend
  virtual bool		ReadItem_impl() {return true;} // replace
  
public: // DataSink i/f
  virtual int		sinkChannelCount() const {return 0;}
   // number of sink channels
  inline bool		sinkChannelInRange(int chan) const
    {return ((chan >= 0) && (chan < sinkChannelCount()));}
  virtual const String	sinkChannelName(int chan) const {return _nilString;}
  virtual bool		sinkItemAvailable() const {return false;}
    // true when a valid item is available for writing
  
  virtual int		GetSinkChannelIndexByName(const String& ch_nm)
    {int scc = sinkChannelCount();  for (int i = 0; i < scc; ++i) 
     if (ch_nm == sinkChannelName(i)) return i;   return -1;}
    // get the channel number for the name; -1 if none
  bool			WriteOpen() {bool ok = true; WriteOpen_impl(ok); 
    return ok;}
    // opens the block for write operation -- must be called after changing params
  void			WriteClose() {WriteClose_impl();}
    // closes the block for write operation -- call when done
  bool			WriteFirst() {WriteItrInit(); return WriteNext();}
    // (re-)initializes sequential write iteration, prepares first item for write
  virtual bool		WriteNext() {return false;} 
    // goes to next item, creating a new one if at end; 'true' if item available
  virtual bool		WriteItem(int idx) {return false;} 
  // if indexable, goes to item idx, if 1+end, Adds a new item; 'true' if item available
  virtual void		WriteDone() {} 
    // call after writing all channels of the item, for impl-dependent commit
  
  bool			SetData(const Variant& data, int chan) 
    {if (sinkItemAvailable() && sinkChannelInRange(chan)) 
       return SetData_impl(data, chan); else return false;}
    // set the data, returns 'true' if successful
  bool			SetDataByChannelName(const Variant& data, const String& ch_nm) 
    {int chan; if (sinkItemAvailable() &&
     ((chan = GetSinkChannelIndexByName(ch_nm)) >= 0)) 
       return SetData_impl(data, chan); else return false;}
    // set the data, returns 'true' if successful
 
  bool			SetMatrixData(const taMatrix* data, int chan) 
    {if (sinkItemAvailable() && sinkChannelInRange(chan)) 
       return SetMatrixData_impl(data, chan); else return false;}
    // set the data from a Matrix, returns 'true' if successful
  bool			SetMatrixDataByChannelName(const taMatrix* data, const String& ch_nm) 
    {int chan; if (sinkItemAvailable() &&
     ((chan = GetSinkChannelIndexByName(ch_nm)) >= 0)) 
       return SetMatrixData_impl(data, chan); else return false;}
    // set the data from a Matrix, returns 'true' if successful
protected:
  virtual bool		AddItem_impl(int n) {return false;} // adds n items
  virtual void		WriteOpen_impl(bool& ok) {} // open for writing
  virtual void		WriteClose_impl() {} // close writing
  virtual void		WriteItrInit() {}   // initializes write iteration
  
  virtual bool		SetData_impl(const Variant& data, int chan) {return false;}
  virtual bool		SetMatrixData_impl(const taMatrix* data, int chan) {return false;}
  virtual bool		WriteItem_impl() {return true;} // write the current item

  
private:
  void			Initialize() {}
  void			Destroy() {}
};




class TAMISC_API DataBlock_Idx: public DataBlock { 
 // #VIRT_BASE #NO_INSTANCE partial implementation for an indexable data block
public: 
  override bool		sourceItemAvailable() const
    {return ((rd_itr >= 0) && (rd_itr < itemCount()));}
  
  override bool		ReadNext() 
    {if (rd_itr < -1) return false;  ++rd_itr;
     if (rd_itr >= itemCount()) {rd_itr = -2; return false;}
     return ReadItem_impl();} 
    //  advance itr; read next item, 'true' if item available
  override bool		ReadItem(int idx) 
    {if ((idx < 0) || (idx >= itemCount())) return false;
     rd_itr = idx;  return ReadItem_impl();} 
    // if indexable, goes to item idx, 'true' if item exists and was read
    
  override bool		sinkItemAvailable() const
    {return ((wr_itr >= 0) && (wr_itr < itemCount()));}
  
  override bool		WriteNext() 
    {if (wr_itr < -1) return false;  ++wr_itr;
     if ((wr_itr == itemCount()) && (isSinkGrowable())) {AddItem_impl(1);}
     if (wr_itr >= itemCount()) {wr_itr = -2; return false;}
     return WriteItem_impl();} 
    //  advance itr; read next item, 'true' if item available
  override bool		WriteItem(int idx) 
    {if ((idx == itemCount()) && (isSinkGrowable())) {AddItem_impl(1);}
     if ((idx < 0) || (idx >= itemCount())) return false;
     wr_itr = idx;  return WriteItem_impl();} 
    // if indexable, goes to item idx, 'true' if item exists and was read
  
  void	Copy_(const DataBlock_Idx& cp)
     {rd_itr = -2;}
  COPY_FUNS(DataBlock_Idx, DataBlock)
  TA_ABSTRACT_BASEFUNS(DataBlock_Idx); //
  
protected:
  int			rd_itr;  // an int iterator for reading
    // -3=ReadItem error, -2=EOF, -1=BOF, >=0 is valid item
  int			wr_itr;  // an int iterator for writing
    // -3=WriteItem error, -2=EOF, -1=BOF, >=0 is valid item
     
  override void		ReadItrInit() {rd_itr = -1;}
    // initializes sequential read iteration
    
  override void		WriteItrInit() {wr_itr = -1;}
    // initializes sequential write iteration
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
/*
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
*/


#endif
