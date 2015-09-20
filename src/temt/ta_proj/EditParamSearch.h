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

#ifndef EditParamSearch_h
#define EditParamSearch_h 1

// parent includes:
#include <taOBase>

// member includes:
#include <double_Array>

// declare all other types mentioned but not required to include:


taTypeDef_Of(EditParamSearch);

class TA_API EditParamSearch: public taOBase {
  // #INLINE parameter searching values
  INHERITED(taOBase)
public:
  enum SearchMode { // whether and how to search this item
    NO,             // do not search or set this item in the startup arguments -- for display and online editing only
    SET,            // set this parameter to its current value -- this will be set on the startup arguments and recorded in the cluster run parameters -- use this for manual parameter searches
    SRCH,           // search over this parameter using the currently-selected search algorithm -- this is only for numeric items
  };

  bool                  record;         // #CONDEDIT_OFF_search #DEF_true whether we should record the parameter in the param sets for the run -- this is automatically true of anything that is being searched -- otherwise the project is saved and run with all current paramters in any case, so this option only determines which parameters are specifically recorded as being manipulated in the current run, as recorded in cluster run jobs tables, etc
  bool                  search;         // Use this parameter in a parameter search function, with values that are specified in range -- only applies when a search algorithm is also selected!  automatically records this parameter as well because it will be set by the command line and is NOT the same as that saved with the project
  String                range;          // #CONDSHOW_ON_search:true specify the values over which to search this parameter -- specific values can be listed separated by commas , and ranges can be specified using start:stop:increment (increment is optional, defaults to 1) notation as used in Matrix code -- e.g. 1,2,3:10:1,10:20:2
  double                next_val;       // #HIDDEN #NO_SAVE computed next value to assign to this item in the parameter search
  double_Array          srch_vals;      // #HIDDEN #NO_SAVE full list of search values, parsed from range expression
  
  ///////// following are obsolete 
  SearchMode            srch;           // #HIDDEN #NO_SAVE #OBSOLETE whether to use this parameter for the currently-selected search algorithm, or set it as a fixed parameter on the startup arguments
  double                min_val;        // #HIDDEN #NO_SAVE #OBSOLETE -- minimum value to consider for parameter searching purposes
  double                max_val;        // #HIDDEN #NO_SAVE #OBSOLETE -- maximum value to consider for parameter searching purposes
  double                incr;           // #HIDDEN #NO_SAVE #OBSOLETE -- suggested increment to use in searching this parameter (e.g., for grid search)

  virtual bool          ParseRange();
  // parse the range expression into srch_vals list of explicit values to search over
  virtual bool          ParseSubRange(const String& sub_range);
  // parse the sub-range expression into srch_vals list of explicit values to search over

  TA_SIMPLE_BASEFUNS(EditParamSearch);
protected:
  void                  UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy();
};

#endif // EditParamSearch_h
