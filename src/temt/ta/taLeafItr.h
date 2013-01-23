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

#ifndef taLeafItr_h
#define taLeafItr_h 1

// parent includes:
#include "ta_def.h"

// member includes:

// declare all other types mentioned but not required to include:
class taGroup_impl; //

TypeDef_Of(taLeafItr);

class TA_API taLeafItr {              // contains the indicies for iterating over leafs
public:
  taGroup_impl*        cgp;            // pointer to current group
  int           g;              // index of current group
  int           i;              // index of current leaf element
};

// Iterate over each element in a list.
#define FOREACH_ELEM_IN_LIST(ELEM_TYPE, ELEM_VAR_NAME, LIST)                        \
  if (int FOREACH_itr = 0) { } else                                                 \
    for (ELEM_TYPE *ELEM_VAR_NAME = (ELEM_TYPE*) (LIST).FirstEl(FOREACH_itr);       \
         ELEM_VAR_NAME;                                                             \
         ELEM_VAR_NAME = (ELEM_TYPE*) (LIST).NextEl(FOREACH_itr))

// Iterate over all subgroups contained by the given group.
// This includes nested subgroups (i.e., uses leaf_gp, vs. gp).
#define FOREACH_SUBGROUP(GROUP_TYPE, SUBGP_VAR_NAME, GROUP)                         \
  if (int FOREACH_itr = 0) { } else                                                 \
    for (GROUP_TYPE *SUBGP_VAR_NAME = (GROUP_TYPE*) (GROUP).FirstGp(FOREACH_itr);   \
         SUBGP_VAR_NAME;                                                            \
         SUBGP_VAR_NAME = (GROUP_TYPE*) (GROUP).NextGp(FOREACH_itr))

// Iterate over each leaf element in a group.
#define FOREACH_ELEM_IN_GROUP(GROUP_TYPE, ELEM_VAR_NAME, GROUP)                     \
  if (bool FOREACH_done = false) { } else                                           \
    for (taLeafItr FOREACH_itr;                                                     \
         !FOREACH_done;                                                             \
         FOREACH_done = true)                                                       \
      for (GROUP_TYPE *ELEM_VAR_NAME = (GROUP_TYPE*) (GROUP).FirstEl(FOREACH_itr);  \
           ELEM_VAR_NAME;                                                           \
           ELEM_VAR_NAME = (GROUP_TYPE*) (GROUP).NextEl(FOREACH_itr))

// Iterate over each leaf element in a group in reverse order.
#define FOREACH_ELEM_IN_GROUP_REV(GROUP_TYPE, ELEM_VAR_NAME, GROUP)                 \
  if (bool FOREACH_done = false) { } else                                           \
    for (taLeafItr FOREACH_itr;                                                     \
         !FOREACH_done;                                                             \
         FOREACH_done = true)                                                       \
      for (GROUP_TYPE *ELEM_VAR_NAME = (GROUP_TYPE*) (GROUP).LastEl(FOREACH_itr);   \
           ELEM_VAR_NAME;                                                           \
           ELEM_VAR_NAME = (GROUP_TYPE*) (GROUP).PrevEl(FOREACH_itr))

#endif // taLeafItr_h
