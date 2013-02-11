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

#ifndef DataBlock_h
#define DataBlock_h 1

// parent includes:
#include <taFBase>

// smartptr, ref includes
#include <taSmartRefT>

// member includes:
#include <taMatrix>

// declare all other types mentioned but not required to include:
class ChannelSpec; // 


taTypeDef_Of(DataBlock);

class TA_API DataBlock: public taFBase {
  // #VIRT_BASE #STEM_BASE #NO_INSTANCE ##TOKENS ##CAT_Data abstract base class interface for objects that provide (source) and/or accept (sink) data
INHERITED(taFBase)
public:
  enum DBOptions { // #BITS datablock options -- they also have individual convenience accessors
    DB_NONE             = 0,      // #NO_BIT
    DB_INDEXABLE        = 0x0001, // items can be access randomly, by index
    DB_SEQUENCABLE      = 0x0002, // items can be accessed sequentially (note: usually true)
    DB_SOURCE           = 0x0004, // is a data source (provides data, "read")
    DB_SINK             = 0x0008, // is a data sink (accepts data, "write")
    DB_SINK_GROWABLE    = 0x0010,  // a sink that can be extended (via AddItem)
    DB_SINK_DYNAMIC     = 0x0020,  // a sink whose schema can be modified
    DB_IND_SEQ_SRC_SNK  = 0x000F,  // #IGNORE
  };
  virtual DBOptions     dbOptions() const = 0; // #CAT_Access options the instance type support

  inline bool           isIndexable() const {return (dbOptions() & DB_INDEXABLE);}
  // #EXPERT #CAT_Access true if can be accessed by index
  inline bool           isSequencable() const {return (dbOptions() & DB_SEQUENCABLE);}
  // #EXPERT #CAT_Access true if can be accessed sequentially
  inline bool           isSource() const {return (dbOptions() & DB_SOURCE);}
  // #EXPERT #CAT_Access true if is a data source
  inline bool           isSink() const {return (dbOptions() & DB_SINK);}
  // #EXPERT #CAT_Access true if is a data sink
  inline bool           isSinkGrowable() const {return (dbOptions() & DB_SINK_GROWABLE);}
  // #EXPERT #CAT_Access true if sink is growable (via AddItem)
  inline bool           isSinkDynamic() const {return (dbOptions() & DB_SINK_DYNAMIC);}
  // #EXPERT #CAT_Access true if sink schema can be modified

  virtual int           ItemCount() const {return 0;}
  // #CAT_Access number of items (if indexable)
  virtual int           ReadIndex() const {return -1;}
  // #CAT_Access index of current item to read (source), if indexible
  virtual int           WriteIndex() const {return -1;}
  // #CAT_Access index of current item to write (sink), if indexible

  virtual void          ResetData() {}
  // #CAT_Modify for supported devices, clears all the data (but not the schema)

  TA_ABSTRACT_BASEFUNS(DataBlock);

public:
  /////////////////////////////////////////////////////////
  // DataSource interface

  //////////////////////////////////
  // Reading Data: making it available

  bool                  ReadOpen() { bool ok = true; ReadOpen_impl(ok); return ok;}
  // #CAT_Source opens the block for read operation -- call prior to reading data to perform any initialization required by data source
  void                  ReadClose() { ReadClose_impl();}
  // #CAT_Source closes the block for read operation -- call when done reading data -- performs any cleanup/freeing of resources after reading

  bool                  ReadFirst() { ReadItrInit(); return ReadNext(); }
  // #CAT_Source (re-)initializes sequential read iteration, reads first item so that it is now available for GetData routines (false if no items available)
  virtual bool          ReadNext() {return false;}
  // #CAT_Source read next item of data (sequential access) so that it is now available for GetData routines -- returns true if item available

  virtual bool          ReadItem(int idx) {return false;}
  // #CAT_Source goes directly (random access) to item idx (-1 = last item available, otherwise must be in range 0 <= idx < ItemCount()) so that it is now available for GetData routines, returns true if item exists and was read

  virtual bool          ReadAvailable() const { return false; }
  // #CAT_Source true when a valid item is available for reading by GetData routines

  //////////////////////////////////
  // Source Channel Access

  virtual int           SourceChannelCount() const {return 0;}
  // #CAT_SourceChannel number of source channels available for reading
  inline bool           SourceChannelInRange(int chan) const
  { return ((chan >= 0) && (chan < SourceChannelCount()));}
  // #CAT_SourceChannel is this channel number available for reading
  virtual const String  SourceChannelName(int chan) const
  { return _nilString;}
  // #CAT_SourceChannel name of source channel from channel index
  virtual int           GetSourceChannelByName(const String& ch_nm, bool err_msg = true)
  { int scc = SourceChannelCount();
    for (int i = 0; i < scc; ++i)
      if (ch_nm == SourceChannelName(i)) return i;
    TestError(err_msg, "GetSourceChannelByName", "channel named",ch_nm,"not found");
    return -1;}
  // #CAT_SourceChannel get the source channel number for the name; -1 if none

  const Variant         GetData(int chan);
  // #CAT_SourceChannel get source data from given channel index -- must have done Read first!
  const Variant         GetDataByName(const String& ch_nm)
  { int chan; if (ReadAvailable() &&
                  ((chan = GetSourceChannelByName(ch_nm)) >= 0))
                return GetData_impl(chan);
    else return _nilVariant; }
  // #CAT_SourceChannel get source data from given channel name -- must have done Read first!
  taMatrix*             GetMatrixData(int chan)
  { if (ReadAvailable() && SourceChannelInRange(chan))
      return GetMatrixData_impl(chan);
    else return NULL;}
  // #CAT_SourceChannel get source data from Matrix channel -- must have done Read first; note: you must taBase::Ref/unRefDone the matrix
  taMatrix*             GetMatrixDataByName(const String& ch_nm)
  { int chan; if (ReadAvailable() &&
                  ((chan = GetSourceChannelByName(ch_nm)) >= 0))
                return GetMatrixData_impl(chan);
    else return NULL;}
  // #CAT_SourceChannel get source data from Matrix channel -- must have done Read first; note: you must taBase::Ref/unRefDone the matrix
  const Variant         GetMatrixCellData(int chan, int cell)
  { taMatrix* mat = GetMatrixData(chan);  if(mat) return mat->SafeElAsVar_Flat(cell);
    else return _nilVariant; }
  // #CAT_SourceChannel get source data cell from Matrix channel -- must have done Read first; cell is one element in matrix data, using a flat index regardless of dimensionality
  const Variant         GetMatrixCellDataByName(const String& ch_nm, int cell)
  { taMatrix* mat = GetMatrixDataByName(ch_nm);  if(mat) return mat->SafeElAsVar_Flat(cell);
    else return _nilVariant; }
  // #CAT_SourceChannel get source data cell from Matrix channel of given name -- must have done Read first; cell is one element in matrix data, using a flat index regardless of dimensionality

protected:
  /////////////////////////////////////////////////////////
  // DataSource impl: these all must be implemented by actual source

  virtual const Variant GetData_impl(int chan) {return _nilVariant;}
  virtual taMatrix*     GetMatrixData_impl(int chan) {return NULL;}
  virtual void          ReadItrInit() {}
  virtual void          ReadOpen_impl(bool& ok) {}
  virtual void          ReadClose_impl() {}
  virtual bool          ReadItem_impl() {return true;}

public:
  /////////////////////////////////////////////////////////
  // DataSink interface

  //////////////////////////////////
  // Writing Data: sending it out

  bool                  WriteOpen() { bool ok = true; WriteOpen_impl(ok); return ok;}
  // #CAT_Sink opens the block for write operation -- call prior to writing any data to perform any initialization required by data sink
  void                  WriteClose() { WriteClose_impl();}
  // #CAT_Sink closes the block for write operation -- call when done writing data -- performs any post-writing cleanup/closing of files, flushing of buffers, etc

  bool                  WriteFirst() { WriteItrInit(); return WriteNext(); }
  // #CAT_Sink (re-)initializes sequential write iteration, makes first item of data available for wirting with SetData routines (false if no items available)
  virtual bool          WriteNext() { return false; }
  // #CAT_Sink goes to next item of data (sequential acccess) for writing by SetData routines, creating a new one if at end; true if item available

  virtual bool          WriteItem(int idx) {return false;}
  // #CAT_Sink goes directly (random access) to item idx (in range 0 <= idx < ItemCount()) for SetData writing routines, if 1+end, adds a new item; true if item available
  virtual bool          WriteAvailable() const {return false;}
  // #CAT_Sink true when a valid item is available for writing by SetData routines

  //////////////////////////////////
  // Sink Channel Access

  virtual int           SinkChannelCount() const {return 0;}
  // #CAT_SinkChannel number of sink channels available for writing
  inline bool           SinkChannelInRange(int chan) const
  { return ((chan >= 0) && (chan < SinkChannelCount()));}
  // #CAT_SinkChannel is this channel number available for writing
  virtual const String  SinkChannelName(int chan) const {return _nilString;}
  // #CAT_SinkChannel Get name of sink channel from channel index

  virtual int           GetSinkChannelByName(const String& ch_nm, bool err_msg = true)
  { int scc = SinkChannelCount();
    for (int i = 0; i < scc; ++i)
      if (ch_nm == SinkChannelName(i)) return i;
    TestError(err_msg, "GetSourceChannelByName", "channel named",ch_nm,"not found");
    return -1;}
  // #CAT_SinkChannel get the channel number for the name; -1 if none

  virtual bool          AddSinkChannel(ChannelSpec* cs)
  { return false; }
  // #CAT_SinkChannel add a sink channel; returns true if ok, and sets the chan_num
  virtual bool          AssertSinkChannel(ChannelSpec* cs)
  // #CAT_SinkChannel make sure sink channel exists; returns true if ok, and sets the chan_num
  { return false; }
  void                  DeleteSinkChannel(int chan)
  { if (isSinkDynamic() && (chan >= 0) && (chan < SinkChannelCount()))
      DeleteSinkChannel_impl(chan);}
  // #CAT_SinkChannel delete given sink channel
  void                  DeleteSinkChannelByName(const String& ch_nm)
  { DeleteSinkChannel(GetSinkChannelByName(ch_nm)); }
  // #CAT_SinkChannel delete given sink channel indicated by name

  bool                  SetData(const Variant& data, int chan)
  { if (WriteAvailable() && SinkChannelInRange(chan))
      return SetData_impl(data, chan); else return false;}
  // #CAT_SinkChannel set the data for a given channel index -- must have done Write first! returns true if successful
  bool                  SetDataByName(const Variant& data, const String& ch_nm)
  { int chan; if (WriteAvailable() &&
                  ((chan = GetSinkChannelByName(ch_nm)) >= 0))
                return SetData_impl(data, chan); else return false;}
  // #CAT_SinkChannel set the data for given channel name -- must have done Write first! returns true if successful

  bool                  SetMatrixData(const taMatrix* data, int chan)
  { if (WriteAvailable() && SinkChannelInRange(chan))
      return SetMatrixData_impl(data, chan); else return false;}
  // #CAT_SinkChannel set the data for given Matrix channel -- must have done Write first! returns true if successful
  bool                  SetMatrixDataByName(const taMatrix* data, const String& ch_nm)
  { int chan; if (WriteAvailable() &&
                  ((chan = GetSinkChannelByName(ch_nm)) >= 0))
                return SetMatrixData_impl(data, chan); else return false;}
  // #CAT_SinkChannel set the data for given matrix channel -- must have done Write first! returns true if successful

  bool                  SetMatrixCellData(const Variant& data, int chan, int cell);
  // #CAT_SinkChannel set the data for given Matrix channel cell (flat index into matrix values) -- must have done Write first! returns true if successful
  bool                  SetMatrixCellDataByName(const Variant& data, const String& ch_nm,
                                                int cell);
  // #CAT_SinkChannel set the data for given matrix channel cell (flat index into matrix values) -- must have done Write first! returns true if successful

  taMatrix*             GetSinkMatrix(int chan)
  { if (WriteAvailable() && SinkChannelInRange(chan))
      {taMatrix* rval = GetSinkMatrix_impl(chan); if (rval) taBase::Ref(rval);
        return rval;}
    else return NULL;}
  // #IGNORE #CAT_SinkChannel gets a REF'ed matrix (you MUST UnRefDone when finished) that you can use to write to the channel; this is ONLY guaranteed to be valid until the next write operation
  taMatrix*             GetSinkMatrixByName(const String& ch_nm)
  { int chan; if (WriteAvailable() && ((chan = GetSinkChannelByName(ch_nm)) >= 0))
      {taMatrix* rval = GetSinkMatrix_impl(chan); if (rval) taBase::Ref(rval);
        return rval;}
    else return NULL;}
  // #IGNORE #CAT_SinkChannel gets a REF'ed matrix (you MUST UnRefDone when finished) that you can use to write to the channel; this is ONLY guaranteed to be valid until the next write operation

protected:
  /////////////////////////////////////////////////////////
  // DataSink impl: these all must be implemented by actual source

  virtual bool          AddItem_impl(int n) {return false;} // adds n items
  virtual void          WriteOpen_impl(bool& ok) {} // open for writing
  virtual void          WriteClose_impl() {} // close writing
  virtual void          WriteItrInit() {}   // initializes write iteration

  virtual bool          SetData_impl(const Variant& data, int chan) {return false;}
  virtual bool          SetMatrixData_impl(const taMatrix* data, int chan) {return false;}
  virtual taMatrix*     GetSinkMatrix_impl(int chan) {return NULL;}
  virtual bool          WriteItem_impl() {return true;} // write the current item

  virtual void          DeleteSinkChannel_impl(int chan) {}

private:
  NOCOPY(DataBlock)
  void                  Initialize() {}
  void                  Destroy() {}
};

SmartRef_Of(DataBlock); // DataBlockRef

#endif // DataBlock_h
