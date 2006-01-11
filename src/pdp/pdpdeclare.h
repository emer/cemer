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
#include "tamisc_TA_type.h"


// external defines
#ifdef TA_GUI
class taiMenuEl;
#endif

class ColorScaleSpec;

class BaseSpec;
//class BaseSpec_List;
class BaseSpec_MGroup;

class Script; //

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
class LayerReader;
class LayerWriter;
class NetConduit;

class Project;
class PDPRoot;


class PDPLog;
class TextLog;
class GraphLog;
class GridLog; //


#ifdef TA_GUI

// externals
class T3DataViewer;

class Xform; // #IGNORE
class Xform_List; // #IGNORE

class PDPView;
class pdpDataViewer;
class LogView; //
class TextLogView; //
class GridLogView; //
class NetLogView; //
class GraphLogView; //


// net_qt.h and netstru_so.h
//TODO
//obs class TDTransform;
class Network_so; // #IGNORE
//class NetViewGraphic_G;
//class Layer_G;
//class LayerNameEdit;
//class LayerText_G;
//class LayerBox_G;
//class Unit_Group_G;
//class Projection_G;
//class PLine_G;
//class SelfCon_PLine_G;
//class Unit_G;
//class UnitValue_G;
//class SquareUnit_G;
//class AreaUnit_G;
//class LinearUnit_G;
//class FillUnit_G;
//class DirFillUnit_G;
//class ThreeDUnit_G;
//class RoundUnit_G;
//class HgtFieldUnit_G;
//class HgtPeakUnit_G;
//class NetEditor;

// ??.h
//class DTEditor;
//class GraphEditor;

#endif // TA_GUI

#endif //declare_h


