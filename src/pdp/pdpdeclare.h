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



#ifndef declare_h
#define declare_h

#include "ta_stdef.h"
#include "ta_TA_type.h"
#include "pdp_def.h"


// external defines
#ifdef TA_GUI
class taiAction;
#endif

class ColorScaleSpec;

class BaseSpec;
//class BaseSpec_List;
class BaseSpec_Group; //

// netstru.h
class SigmoidSpec;
class ConSpec;
class Connection;
class Con_Group;
class UnitSpec;
class Unit;
class Unit_Group;
class ProjectionSpec;
class Projection;
class LayerSpec;
class Layer;
class Network;

class ProjectBase;
class PDPRoot; //

class GridTableView;

#ifdef TA_GUI

// externals
class T3DataViewer;

class Xform; // #IGNORE
class Xform_List; // #IGNORE


// net_qt.h and netstru_so.h
//TODO
class Network_so; // #IGNORE
#endif // TA_GUI

#endif //declare_h


