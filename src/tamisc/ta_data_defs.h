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



// ta_data_defs.h -- interfaces and basic definitions for the pdp data system

#ifndef TA_DATA_DEFS_H
#define TA_DATA_DEFS_H

#include "tamisc_stdef.h"
#include "tamisc_TA_type.h"

// externals
class taMatrix_impl;
class SourceChannel;
class SinkChannel;

// forwards this file
class IDataSource;
class ISequencable;
class IDataSink;


class ISequencable { // #VIRT_BASE #NO_INSTANCE #NO_TOKENS interface exposed by entities that can be sequenced, particularly DataSources
public:
  virtual int		num_items() = 0; // N<0 if items unknown, or cannot be accessed randomly
  virtual bool		is_indexable() = 0; // 'true' if can be accessed by index
  virtual bool		is_sequential() = 0; // 'true' if can be accessed sequentially
  
  virtual void		GoToItem(int index) = 0; // for indexed access, goes to the item 
  
  virtual void		ResetItem() = 0; // for seq access, resets iteration state, but is not a sequence command per se
  virtual bool		NextItem() = 0; // for seq access, goes to the next item, 'true' if there was a next item
  
  virtual ~ISequencable() {};
};

enum DataTransferMode {  
  DTM_PUSH		= 0x01, // #LABEL_Push
  DTM_PULL		= 0x02, // #LABEL_Pull
  DTM_BOTH		= 0x03  // #LABEL_Both
};

typedef taMatrix_impl* ptaMatrix_impl;

class IDataSource { // #VIRT_BASE #NO_INSTANCE #NO_TOKENS represents a source of data
friend class SourceChannel;
public:
  virtual bool		can_sequence() const {return false;} // true if has a ISequencable interface
  virtual ISequencable* sequencer() {return NULL;} // sequencing interface, if sequencable
  virtual int		source_channel_count() = 0; // number of source channels
  virtual SourceChannel* source_channel(int idx) = 0; // get a source channel
  
  virtual ~IDataSource() {} //
protected: 
  // SourceChannel delegates -- designed for multi-class inheritance chain
  virtual void 		DoProduceData(SourceChannel* ch, ptaMatrix_impl& data, bool& handled) {} // #IGNORE
};

class IDataSink { // #VIRT_BASE #NO_INSTANCE #NO_TOKENS represents a consumer of data
friend class SinkChannel;
public:
  virtual int		sink_channel_count() = 0; // number of sink channels
  virtual SinkChannel* 	sink_channel(int idx) = 0; // get a sink channel
  virtual ~IDataSink() {} //
protected: 
  // SinkChannel delegates -- designed for multi-class inheritance chain
  virtual void		DoConsumeData(SinkChannel* ch, bool& handled) {} // #IGNORE
};

#endif
