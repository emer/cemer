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

#ifndef GraphPlotView_h
#define GraphPlotView_h 1

// parent includes:
#include <GraphAxisBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(GraphPlotView);

class TA_API GraphPlotView : public GraphAxisBase {
  // parameters for plotting one column of data -- contains Y axis data as well
INHERITED(GraphAxisBase)
public:

  enum LineStyle {
    SOLID,                      // -----
    DOT,                        // .....
    DASH,                       // - - -
    DASH_DOT,                    // _._._
    LineStyle_MIN = SOLID,       // #IGNORE also the default
    LineStyle_MAX = DASH_DOT,    // #IGNORE
  };

  //NOTE: if PointStyle changed, must change T3GraphLine::MarkerStyle
  // NOTE: 0 is used as a special "NONE" pseudo-value during assignment of styles
  enum PointStyle {
    CIRCLE = 1,                 // o
    SQUARE,                     // []
    DIAMOND,                    // <>
    TRIANGLE,
    MINUS,                      // -
    BACKSLASH,
    BAR,                        // |
    SLASH,                      // /
    PLUS,                       // +
    CROSS,                      // x
    STAR,                        // *
    PointStyle_NONE = 0,        // #IGNORE pseudo value, only used during assignment of styles
    PointStyle_MIN = CIRCLE,    // #IGNORE also the default
    PointStyle_MAX = STAR,      // #IGNORE
  };

  LineStyle     line_style;     // the style in which the line is drawn
  PointStyle    point_style;    // the style in which the points are drawn
  bool          alt_y;          // use the alternate (right hand side) y axis instead of default left axis
  GraphPlotView* eff_y_axis;    // #NO_SAVE #READ_ONLY #NO_SET_POINTER #NO_COPY effective y axis for this guy at this point in time

  override void         UpdateOnFlag();

  void          CopyFromView(GraphPlotView* cp);
  // #BUTTON special copy function that just copies user view options in a robust manner

//   void InitLinks();
//   void CutLinks();
  SIMPLE_COPY(GraphPlotView);
  T3_DATAVIEWFUNS(GraphPlotView, GraphAxisBase)
protected:
  override void         UpdateAfterEdit_impl();

private:
  void                  Initialize();
  void                  Destroy() { };
};


taTypeDef_Of(GraphPlotView_List);

class TA_API GraphPlotView_List : public taList<GraphPlotView> {
  // #NO_TOKENS #NO_UPDATE_AFTER list of graph plot view objects
INHERITED(taList<GraphPlotView>)
public:
  TA_BASEFUNS_NOCOPY(GraphPlotView_List);
private:
  void  Initialize()            { SetBaseType(&TA_GraphPlotView); }
  void  Destroy()               { };
};


#endif // GraphPlotView_h
