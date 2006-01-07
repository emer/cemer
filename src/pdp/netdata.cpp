// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



// netdata.cc


#include "netdata.h"
/*nn
#include "netstru.h"
//#include "sched_proc.h" // for trialproc check in enview::update_display
#include "pdpshell.h"

#ifdef TA_GUI
#include "ta_qtgroup.h"
#include "pdplog_qtso.h"
#endif


#include <math.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>


//////////////////////////
//   MatrixGroup_List	//
//////////////////////////

void MatrixGroup_List::Initialize() {
  SetBaseType(&TA_taMatrix_Group);
}

void* MatrixGroup_List::El_Own_(void* it_) {
  void* rval = inherited::El_Own_(it_);
  
  taMatrix_Group* it = static_cast<taMatrix_Group*>(it_);
  //TODO: maybe should do a more thorough check of compatibility...
  if ((it == NULL) || (it->leaves != 0)) return rval;
  
  DataSet* own = GET_MY_OWNER(DataSet);
  if (own == NULL) return rval;
  
  // initialize the data 
  own->InitDataItem(it);
  return rval;
}


//////////////////////////
//   DataSet		//
//////////////////////////

void DataSet::Initialize() {
}

void DataSet::Destroy() {
}

void DataSet::InitLinks() {
  inherited::InitLinks();
  taBase::Own(source_channels, this);
  taBase::Own(data, this);
}

void DataSet::CutLinks() {
  data.CutLinks();
  source_channels.CutLinks();
  inherited::CutLinks();
}

void DataSet::Copy_(const DataSet& cp) {
  data.Reset(); // prevents issues with changes to the schema
  source_channels = cp.source_channels;
  data = cp.data;
}

void DataSet::DoProduceData(SourceChannel* ch, ptaMatrix_impl& data, bool& handled) {
//TODO: find the corresponding data item, based on channel, and current itr index
}

void DataSet::InitDataItem(taMatrix_Group* item) {
  InitDataItem_impl(&source_channels, item);
}

void DataSet::InitDataItem_impl(SourceChannel_Group* ch_gp, taMatrix_Group* data_gp) {
  // do the items 
  for (int i = 0; i < ch_gp->size; ++i) {
    SourceChannel* chan = (SourceChannel*)ch_gp->FastEl(i);
    // need to make the proper type of matrix
    taMatrix_impl* mat = (taMatrix_impl*)taBase::MakeToken(chan->matrix_type);
    InitMatrix(chan, mat);
    data_gp->Add(mat);
  }
  
  // recursively do the subgroups
  for (int g = 0; g < ch_gp->gp.size; ++g) {
    SourceChannel_Group* sub_ch_gp = (SourceChannel_Group*)ch_gp->FastGp(g);
    taMatrix_Group* new_gp = (taMatrix_Group*)data_gp->NewGp(1);
    new_gp->name = sub_ch_gp->name; //todo: maybe should use SetName, for notification
    InitDataItem_impl(sub_ch_gp, new_gp);
  }
}
 
void DataSet::InitFromConduit(NetConduit* cond) {
  if (cond == NULL) return;
  Reset();
  //TODO: more
}

 
void DataSet::InitMatrix(SourceChannel* chan, taMatrix_impl* mat) {
  // set geometry
  mat->SetGeomN(chan->geom);
}


void DataSet::Reset() {
  ResetData();
  source_channels.Reset();
}

void DataSet::ResetData() {
  data.Reset();
}
*/
