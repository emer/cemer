// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
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
class SourceChannel_List;
class IDataSink;
class SinkChannel_List;


class ISequencable { // #VIRT_BASE #NO_INSTANCE #NO_TOKENS interface exposed by entities that can be sequenced, particularly DataSources
public:
  virtual int		num_items() = 0; // N<0 if items unknown, or cannot be accessed randomly
  virtual bool		is_permutable() = 0; // 'true' if can be accessed randomly
  virtual bool		has_next() = 0; // for sequential access, 'true' if can call Next()
  
  virtual void		Reset() = 0; // restores state, but is not a sequence command per se
  virtual void		Next() = 0; // for sequential access, goes to the next item
  virtual void		GoTo(uint index) = 0; // for random access, goes to the item 
  
  virtual ~ISequencable() {};
};

enum DataTransferMode {
  DTM_NONE		= 0x00, // initialization value only
  DTM_PUSH		= 0x01,
  DTM_PULL		= 0x02
#ifndef __MAKETA__
  ,DTM_BOTH		= DTM_PUSH | DTM_PULL
#endif
};

typedef taMatrix_impl* ptaMatrix_impl;

class IDataSource { // #VIRT_BASE #NO_INSTANCE #NO_TOKENS represents a source of data
friend class SourceChannel;
public:
  virtual bool		can_sequence_() const {return false;} // true if has a ISequencable interface
  virtual ISequencable* sequencer_() {return NULL;} // sequencing interface, if sequencable
  virtual SourceChannel_List& source_channels_() = 0;
  taMatrix_impl* 	GetData(SourceChannel* ch) {taMatrix_impl* data = NULL; 
    bool handled = false; GetData_(ch, data, handled); return data;}
  
  virtual ~IDataSource() {} //
protected: // SourceChannel delegates -- designed for multi-class inheritance chain
  virtual void GetData_(SourceChannel* ch, ptaMatrix_impl& data, bool& handled) {}
};

class IDataSink { // #VIRT_BASE #NO_INSTANCE #NO_TOKENS represents a consumer of data
friend class SinkChannel;
public:
  virtual SinkChannel_List& sink_channels() = 0;
  virtual ~IDataSink() {}
protected: // SinkChannel delegates -- designed for multi-class inheritance chain
  virtual void		DoAcceptData(SinkChannel* ch, taMatrix_impl* data, bool& handled) {}
  virtual void		DoConsumeData(SinkChannel* ch, bool& handled) {}
};

#endif
