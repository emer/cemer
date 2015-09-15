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

  SearchMode            srch;           // #HIDDEN #NO_SAVE obsolete whether to use this parameter for the currently-selected search algorithm, or set it as a fixed parameter on the startup arguments
  bool                  search;         // Use this parameter in search

  bool                  record;         // #CONDEDIT_OFF_search #DEF_true whether we should record the parameter in the param sets for the run

  String                range;           // #CONDSHOW_ON_search:true specify the range and increments over which to search this parameter -- e.g. 1;2;3:1:5;10:2:20
  double                next_val;       // #HIDDEN #NO_SAVE computed next value to assign to this item in the parameter search

  double                min_val;        // #HIDDEN #NO_SAVE obsolete -- minimum value to consider for parameter searching purposes
  double                max_val;        // #HIDDEN #NO_SAVE obsolete -- maximum value to consider for parameter searching purposes
  double                incr;           // #HIDDEN #NO_SAVE obsolete -- suggested increment to use in searching this parameter (e.g., for grid search)


  TA_SIMPLE_BASEFUNS(EditParamSearch);
protected:
  void                  UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy();
};

#endif // EditParamSearch_h
