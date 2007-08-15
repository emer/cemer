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


#include "ta_base.h"
#include "ta_group.h"
#include "ta_matrix.h"

#include "ta_TA_type.h"

// forwards
class DataBlock;

class TA_API ChannelSpec: public taNBase {
  // ##CAT_Data describes a channel of data in a DataBlock (e.g., a column of a datatable)
INHERITED(taNBase)
public: 
  int			chan_num; // #SHOW #READ_ONLY #NO_SAVE the column number (-1=at end)
  ValType 		val_type; // the type of data the channel uses
  //String		disp_opts;	// viewer display options
  
  virtual bool		isMatrix() const {return false;}
  virtual const MatrixGeom& cellGeom() const;
  virtual bool		usesCellNames() const {return false;}
  virtual const String_Matrix& cellNames() const;
  
  String GetColText(int col, int);
  TA_BASEFUNS(ChannelSpec);
private:
  void 	Copy_(const ChannelSpec& cp);
  void		Initialize();
  void		Destroy() {}
};

class TA_API MatrixChannelSpec: public ChannelSpec { 
  // describes a matrix channel of data in a DataBlock
INHERITED(ChannelSpec) 
public:
  MatrixGeom		cell_geom; //  the geom of each cell
  bool			uses_cell_names; // set 'true' if there are cell names associated
  String_Matrix		cell_names; //  #COND_EDIT_ON_uses_cell_names:true (optional) the names associated with each cell
  
  override bool		isMatrix() const {return true;}
  override const MatrixGeom& cellGeom() const {return cell_geom;}
  override bool		usesCellNames() const {return uses_cell_names;}
  override const String_Matrix& cellNames() const {return cell_names;}
  
  virtual void		SetCellGeom(bool uses_names, int dims,
    int d0, int d1=0, int d2=0, int d3=0, int d4=0);
  virtual void		SetCellGeomN(bool uses_names, const MatrixGeom& geom);
    
  void 	UpdateAfterEdit();
  void	InitLinks();
  void	CutLinks();
  TA_BASEFUNS(MatrixChannelSpec);
protected:
  void			InitCellNames();
private:
  void 	Copy_(const MatrixChannelSpec& cp);
  void		Initialize();
  void		Destroy() {CutLinks();}
};

class TA_API ChannelSpec_List: public taList<ChannelSpec> {
  // ##CAT_Data a list of channel specifications
INHERITED(taList<ChannelSpec>)
public:
  void		UpdateDataBlockSchema(DataBlock* db);

  int		NumListCols() const {return 6;}
  String 	GetColHeading(int col);

  TA_BASEFUNS_NOCOPY(ChannelSpec_List);
private:
  void		Initialize() {SetBaseType(&TA_ChannelSpec);}
  void		Destroy() {}
};

class TA_API DataBlock: public taFBase {
  // #VIRT_BASE #NO_INSTANCE ##TOKENS ##CAT_Data abstract base class interface for objects that provide (source) and/or accept (sink) data
INHERITED(taFBase)
public:
  enum DBOptions { // #BITS datablock options -- they also have individual convenience accessors
    DB_NONE		= 0,      // #NO_BIT 
    DB_INDEXABLE	= 0x0001, // items can be access randomly, by index
    DB_SEQUENCABLE	= 0x0002, // items can be accessed sequentially (note: usually true)
    DB_SOURCE		= 0x0004, // is a data source (provides data, "read")
    DB_SINK		= 0x0008, // is a data sink (accepts data, "write")
    DB_SINK_GROWABLE	= 0x0010,  // a sink that can be extended (via AddItem)
    DB_SINK_DYNAMIC	= 0x0020  // a sink whose schema can be modified
#ifndef __MAKETA__
    ,DB_IND_SEQ_SRC_SNK = 0x000F
#endif
  };
  virtual DBOptions	dbOptions() const = 0; // #CAT_Access options the instance type support
  
  inline bool		isIndexable() const {return (dbOptions() & DB_INDEXABLE);} 
  // #CAT_XpertAccess true if can be accessed by index
  inline bool		isSequencable() const {return (dbOptions() & DB_SEQUENCABLE);} 
  // #CAT_XpertAccess true if can be accessed sequentially
  inline bool		isSource() const {return (dbOptions() & DB_SOURCE);} 
  // #CAT_XpertAccess true if is a data source
  inline bool		isSink() const {return (dbOptions() & DB_SINK);} 
  // #CAT_XpertAccess true if is a data sink
  inline bool		isSinkGrowable() const {return (dbOptions() & DB_SINK_GROWABLE);} 
  // #CAT_XpertAccess true if sink is growable (via AddItem)
  inline bool		isSinkDynamic() const {return (dbOptions() & DB_SINK_DYNAMIC);} 
  // #CAT_XpertAccess true if sink schema can be modified
  
  virtual int		ItemCount() const {return 0;} 
  // #CAT_Access number of items (if indexable)
  virtual int		ReadIndex() const {return -1;} 
  // #CAT_Access index of current item to read (source), if indexible
  virtual int		WriteIndex() const {return -1;} 
  // #CAT_Access index of current item to write (sink), if indexible
  
  virtual void		ResetData() {}
  // #CAT_Modify for supported devices, clears all the data (but not the schema)

  TA_ABSTRACT_BASEFUNS(DataBlock);
  
public:
  /////////////////////////////////////////////////////////
  // DataSource interface

  //////////////////////////////////
  // Reading Data: making it available

  bool			ReadOpen() { bool ok = true; ReadOpen_impl(ok); return ok;}
  // #CAT_Source opens the block for read operation -- call prior to reading data to perform any initialization required by data source
  void			ReadClose() { ReadClose_impl();}
  // #CAT_Source closes the block for read operation -- call when done reading data -- performs any cleanup/freeing of resources after reading

  bool			ReadFirst() { ReadItrInit(); return ReadNext(); }
  // #CAT_Source (re-)initializes sequential read iteration, reads first item so that it is now available for GetData routines (false if no items available)
  virtual bool		ReadNext() {return false;}
  // #CAT_Source read next item of data (sequential access) so that it is now available for GetData routines -- returns true if item available

  virtual bool		ReadItem(int idx) {return false;} 
  // #CAT_Source goes directly (random access) to item idx (-1 = last item available, otherwise must be in range 0 <= idx < ItemCount()) so that it is now available for GetData routines, returns true if item exists and was read

  virtual bool		ReadAvailable() const { return false; }
  // #CAT_Source true when a valid item is available for reading by GetData routines
  
  //////////////////////////////////
  // Source Channel Access

  virtual int		SourceChannelCount() const {return 0;}
  // #CAT_SourceChannel number of source channels available for reading
  inline bool		SourceChannelInRange(int chan) const
  { return ((chan >= 0) && (chan < SourceChannelCount()));}
  // #CAT_SourceChannel is this channel number available for reading
  virtual const String	SourceChannelName(int chan) const
  { return _nilString;}
  // #CAT_SourceChannel name of source channel from channel index
  virtual int		GetSourceChannelByName(const String& ch_nm, bool err_msg = true)
  { int scc = SourceChannelCount();
    for (int i = 0; i < scc; ++i) 
      if (ch_nm == SourceChannelName(i)) return i;
    TestError(err_msg, "GetSourceChannelByName", "channel named",ch_nm,"not found");
    return -1;}
  // #CAT_SourceChannel get the source channel number for the name; -1 if none
    
  const Variant		GetData(int chan) 
  { if (ReadAvailable() && SourceChannelInRange(chan)) 
      return GetData_impl(chan);
    else return _nilVariant;}
  // #CAT_SourceChannel get source data from given channel index -- must have done Read first!
  const Variant		GetDataByName(const String& ch_nm) 
  { int chan; if (ReadAvailable() &&
		  ((chan = GetSourceChannelByName(ch_nm)) >= 0)) 
		return GetData_impl(chan);
    else return _nilVariant; }
  // #CAT_SourceChannel get source data from given channel name -- must have done Read first!
  taMatrix*		GetMatrixData(int chan) 
  { if (ReadAvailable() && SourceChannelInRange(chan))
      return GetMatrixData_impl(chan);
    else return NULL;}
  // #CAT_SourceChannel get source data from Matrix channel -- must have done Read first; note: you must taBase::Ref/unRefDone the matrix
  taMatrix*		GetMatrixDataByName(const String& ch_nm) 
  { int chan; if (ReadAvailable() &&
		  ((chan = GetSourceChannelByName(ch_nm)) >= 0)) 
		return GetMatrixData_impl(chan);
    else return NULL;}
  // #CAT_SourceChannel get source data from Matrix channel -- must have done Read first; note: you must taBase::Ref/unRefDone the matrix
  const Variant		GetMatrixCellData(int chan, int cell) 
  { taMatrix* mat = GetMatrixData(chan);  if(mat) return mat->SafeElAsVar_Flat(cell);
    else return _nilVariant; }
  // #CAT_SourceChannel get source data cell from Matrix channel -- must have done Read first; cell is one element in matrix data, using a flat index regardless of dimensionality
  const Variant		GetMatrixCellDataByName(const String& ch_nm, int cell) 
  { taMatrix* mat = GetMatrixDataByName(ch_nm);  if(mat) return mat->SafeElAsVar_Flat(cell);
    else return _nilVariant; }
  // #CAT_SourceChannel get source data cell from Matrix channel of given name -- must have done Read first; cell is one element in matrix data, using a flat index regardless of dimensionality

protected:
  /////////////////////////////////////////////////////////
  // DataSource impl: these all must be implemented by actual source

  virtual const Variant	GetData_impl(int chan) {return _nilVariant;}
  virtual taMatrix*	GetMatrixData_impl(int chan) {return NULL;}
  virtual void		ReadItrInit() {}
  virtual void		ReadOpen_impl(bool& ok) {}
  virtual void		ReadClose_impl() {}
  virtual bool		ReadItem_impl() {return true;}
  
public: 
  /////////////////////////////////////////////////////////
  // DataSink interface

  //////////////////////////////////
  // Writing Data: sending it out

  bool			WriteOpen() { bool ok = true; WriteOpen_impl(ok); return ok;}
  // #CAT_Sink opens the block for write operation -- call prior to writing any data to perform any initialization required by data sink
  void			WriteClose() { WriteClose_impl();}
  // #CAT_Sink closes the block for write operation -- call when done writing data -- performs any post-writing cleanup/closing of files, flushing of buffers, etc

  bool			WriteFirst() { WriteItrInit(); return WriteNext(); }
  // #CAT_Sink (re-)initializes sequential write iteration, makes first item of data available for wirting with SetData routines (false if no items available)
  virtual bool		WriteNext() { return false; } 
  // #CAT_Sink goes to next item of data (sequential acccess) for writing by SetData routines, creating a new one if at end; true if item available

  virtual bool		WriteItem(int idx) {return false;} 
  // #CAT_Sink goes directly (random access) to item idx (in range 0 <= idx < ItemCount()) for SetData writing routines, if 1+end, adds a new item; true if item available
  virtual bool		WriteAvailable() const {return false;}
  // #CAT_Sink true when a valid item is available for writing by SetData routines
  
  //////////////////////////////////
  // Sink Channel Access

  virtual int		SinkChannelCount() const {return 0;}
  // #CAT_SinkChannel number of sink channels available for writing 
  inline bool		SinkChannelInRange(int chan) const
  { return ((chan >= 0) && (chan < SinkChannelCount()));}
  // #CAT_SinkChannel is this channel number available for writing
  virtual const String	SinkChannelName(int chan) const {return _nilString;}
  // #CAT_SinkChannel Get name of sink channel from channel index

  virtual int		GetSinkChannelByName(const String& ch_nm, bool err_msg = true)
  { int scc = SinkChannelCount();
    for (int i = 0; i < scc; ++i) 
      if (ch_nm == SinkChannelName(i)) return i;
    TestError(err_msg, "GetSourceChannelByName", "channel named",ch_nm,"not found");
    return -1;}
  // #CAT_SinkChannel get the channel number for the name; -1 if none
    
  virtual bool		AddSinkChannel(ChannelSpec* cs) 
  { return false; }
  // #CAT_SinkChannel add a sink channel; returns true if ok, and sets the chan_num 
  virtual bool		AssertSinkChannel(ChannelSpec* cs) 
  // #CAT_SinkChannel make sure sink channel exists; returns true if ok, and sets the chan_num 
  { return false; }
  void			DeleteSinkChannel(int chan) 
  { if (isSinkDynamic() && (chan >= 0) && (chan < SinkChannelCount()))
      DeleteSinkChannel_impl(chan);}
  // #CAT_SinkChannel delete given sink channel
  void			DeleteSinkChannelByName(const String& ch_nm)
  { DeleteSinkChannel(GetSinkChannelByName(ch_nm)); }
  // #CAT_SinkChannel delete given sink channel indicated by name

  bool			SetData(const Variant& data, int chan) 
  { if (WriteAvailable() && SinkChannelInRange(chan)) 
      return SetData_impl(data, chan); else return false;}
  // #CAT_SinkChannel set the data for a given channel index -- must have done Write first! returns true if successful
  bool			SetDataByName(const Variant& data, const String& ch_nm) 
  { int chan; if (WriteAvailable() &&
		  ((chan = GetSinkChannelByName(ch_nm)) >= 0)) 
		return SetData_impl(data, chan); else return false;}
  // #CAT_SinkChannel set the data for given channel name -- must have done Write first! returns true if successful
 
  bool			SetMatrixData(const taMatrix* data, int chan) 
  { if (WriteAvailable() && SinkChannelInRange(chan)) 
      return SetMatrixData_impl(data, chan); else return false;}
  // #CAT_SinkChannel set the data for given Matrix channel -- must have done Write first! returns true if successful
  bool			SetMatrixDataByName(const taMatrix* data, const String& ch_nm) 
  { int chan; if (WriteAvailable() &&
		  ((chan = GetSinkChannelByName(ch_nm)) >= 0)) 
		return SetMatrixData_impl(data, chan); else return false;}
  // #CAT_SinkChannel set the data for given matrix channel -- must have done Write first! returns true if successful
    
  bool			SetMatrixCellData(const Variant& data, int chan, int cell) 
  { taMatrix* mat = GetSinkMatrix(chan);  if(mat) { mat->SetFmVar_Flat(data, cell);
      return true; } else return false; }
  // #CAT_SinkChannel set the data for given Matrix channel cell (flat index into matrix values) -- must have done Write first! returns true if successful
  bool			SetMatrixCellDataByName(const taMatrix* data, const String& ch_nm,
						int cell) 
  { taMatrix* mat = GetSinkMatrixByName(ch_nm);  if(mat) { mat->SetFmVar_Flat(data, cell);
      return true; } else return false; }
  // #CAT_SinkChannel set the data for given matrix channel cell (flat index into matrix values) -- must have done Write first! returns true if successful
    
  taMatrix*		GetSinkMatrix(int chan)
  { if (WriteAvailable() && SinkChannelInRange(chan)) 
      {taMatrix* rval = GetSinkMatrix_impl(chan); if (rval) taBase::Ref(rval);
        return rval;} 
    else return NULL;}
  // #CAT_SinkChannel gets a REF'ed matrix (you MUST UnRefDone when finished) that you can use to write to the channel; this is ONLY guaranteed to be valid until the next write operation
  taMatrix*		GetSinkMatrixByName(const String& ch_nm)
  { int chan; if (WriteAvailable() && ((chan = GetSinkChannelByName(ch_nm)) >= 0)) 
      {taMatrix* rval = GetSinkMatrix_impl(chan); if (rval) taBase::Ref(rval);
        return rval;} 
    else return NULL;}
  // #CAT_SinkChannel gets a REF'ed matrix (you MUST UnRefDone when finished) that you can use to write to the channel; this is ONLY guaranteed to be valid until the next write operation

protected:
  /////////////////////////////////////////////////////////
  // DataSink impl: these all must be implemented by actual source

  virtual bool		AddItem_impl(int n) {return false;} // adds n items
  virtual void		WriteOpen_impl(bool& ok) {} // open for writing
  virtual void		WriteClose_impl() {} // close writing
  virtual void		WriteItrInit() {}   // initializes write iteration
  
  virtual bool		SetData_impl(const Variant& data, int chan) {return false;}
  virtual bool		SetMatrixData_impl(const taMatrix* data, int chan) {return false;}
  virtual taMatrix*	GetSinkMatrix_impl(int chan) {return NULL;}
  virtual bool		WriteItem_impl() {return true;} // write the current item

  virtual void		DeleteSinkChannel_impl(int chan) {}

private:
  NOCOPY(DataBlock)
  void			Initialize() {}
  void			Destroy() {}
};

SmartRef_Of(DataBlock,TA_DataBlock); // DataBlockRef

class TA_API DataBlock_Idx : public DataBlock { 
 // #VIRT_BASE #NO_INSTANCE base class for a data source and/or sink that supports random access to data via an index
INHERITED(DataBlock)
public: 
  override bool		ReadAvailable() const
  { return ((rd_itr >= 0) && (rd_itr < ItemCount())); }
  
  override bool		ReadNext() 
  { if (rd_itr < -1) return false;  ++rd_itr;
    if (rd_itr >= ItemCount()) {rd_itr = -2; return false;}
    return ReadItem_impl(); } 
  override bool		ReadItem(int idx) 
  { if(idx < 0) idx = ItemCount()-1; if((idx < 0) || (idx >= ItemCount())) return false;
    rd_itr = idx;  return ReadItem_impl();} 
    
  override bool		WriteAvailable() const
    {return ((wr_itr >= 0) && (wr_itr < ItemCount()));}
  override bool		WriteNext() 
    {if (wr_itr < -1) return false;  ++wr_itr;
     if ((wr_itr == ItemCount()) && (isSinkGrowable())) {AddItem_impl(1);}
     if (wr_itr >= ItemCount()) {wr_itr = -2; return false;}
     return WriteItem_impl();} 
  override bool		WriteItem(int idx) 
    {if ((idx == ItemCount()) && (isSinkGrowable())) {AddItem_impl(1);}
     if ((idx < 0) || (idx >= ItemCount())) return false;
     wr_itr = idx;  return WriteItem_impl();} 
 
  override int		ReadIndex() const {return rd_itr;} 
  override int		WriteIndex() const {return wr_itr;} 

  TA_ABSTRACT_BASEFUNS(DataBlock_Idx);
protected:
  int			rd_itr;
  // an int iterator for reading: -3=ReadItem error, -2=EOF, -1=BOF, >=0 is valid item
  int			wr_itr;
  // an int iterator for writing: -3=WriteItem error, -2=EOF, -1=BOF, >=0 is valid item
     
  override void		ReadItrInit() {rd_itr = -1;}
  override void		WriteItrInit() {wr_itr = -1;}
private:
  void	Copy_(const DataBlock_Idx& cp) { rd_itr = -2; }
  void			Initialize() {rd_itr = -2; wr_itr = -2;}
  void			Destroy() {}
};

#endif
