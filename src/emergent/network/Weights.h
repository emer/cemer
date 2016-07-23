// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef Weights_h
#define Weights_h 1

// parent includes:
#include <taNBase>
#include "network_def.h"

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(Weights);

class E_API Weights : public taNBase {
  // Saves network weights in memory for easy and fast access
INHERITED(taNBase)
public:
  int           batch;          // #READ_ONLY #SHOW batch counter: number of times network has been trained over a full sequence of epochs (updated by program)
  int           epoch;          // #READ_ONLY #SHOW epoch counter: number of times a complete set of training patterns has been presented (updated by program)
  bool          save_with_proj; // save these weights with the project file -- NOTE that this can greatly increase the size of the project file!
  bool          auto_load;      // #CONDSHOW_OFF_save_with_proj automatically load weights from file named in load_file -- this makes the project file smaller, but requires managing multiple different files
  bool          quiet_load;     // suppress warning messages when loading weights files into the network (not recommended!)
  String        load_file;      // #CONDSHOW_ON_auto_load name of file to load weights from for auto_load
  bool          auto_init;      // #CONDSHOW_ON_save_with_proj||auto_load automatically initialize network weights with these weights whenever network is built -- only avail for save_with_proj or auto_load weights (for initial auto-build) -- first set of weights with this setting is actually used, if multiple
  String        wt_file;        // #HIDDEN the weights, encoded as a file

  virtual bool  HasWeights(bool err = true);
  // check if we have weights currently -- if err then emit error if we don't

  virtual void  WeightsFmNet();
  // #BUTTON save weights from the network we're associated with into this weights object
  virtual bool  WeightsToNet();
  // #BUTTON load weights from this weights file into the network we're associated with 

  virtual bool  SaveWeights(const String& fname="");
  // #BUTTON #MENU #EXT_wts #COMPRESS #CAT_File #FILETYPE_Weights #FILE_DIALOG_SAVE write saved weight values in this object out to given file
  virtual bool  LoadWeights(const String& fname="");
  // #BUTTON #MENU #EXT_wts #COMPRESS #CAT_File #FILETYPE_Weights #FILE_DIALOG_LOAD read weight values in from given file into this object

  DumpQueryResult Dump_QuerySaveMember(MemberDef* md) override;

  TA_SIMPLE_BASEFUNS(Weights);
protected:
  void  UpdateAfterEdit_impl() override;
private:
  void Initialize();
  void Destroy()     { };
};

#endif // Weights_h
