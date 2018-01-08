// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef GraphAxisBase_h
#define GraphAxisBase_h 1

// parent includes:
#include <T3DataView>

// member includes:
#include <FixedMinMax>
#include <RGBA>
#include <MinMax>
#include <DataCol>
#include <String_Array>
#include <taMath_float>

// declare all other types mentioned but not required to include:
class GraphColView; // 
class T3DataView_List; // 
class GraphTableView; // 
class T3Axis; // 

taTypeDef_Of(GraphAxisBase);

class TA_API GraphAxisBase : public T3DataView {
  // ##EDIT_INLINE base class for data about axis on a graph
INHERITED(T3DataView)
public:
  enum AxisType { X, Y, Z };

  bool                  on;             // is this axis active for displaying info
  AxisType              axis;           // #READ_ONLY #SHOW type of axis this is, for rendering purposes
  GraphColView*         col_lookup;     // #NULL_OK #FROM_LIST_col_list #NO_SAVE #NO_EDIT #NO_UPDATE_POINTER #NO_COPY lookup a column of data for this axis -- only for lookup purposes -- fills in the name and is reset to NULL -- name is what is actually used
  String                col_name;       // #ADD_COMPLETER_SIMPLE name of column of data for this axis
  String                col_name_good;  // #READ_ONLY #SHOW previous non-NULL column name -- used for restore previous columns
  bool                  is_string;      // #READ_ONLY #NO_COPY true if column is a string
  bool                  is_matrix;      // #READ_ONLY #NO_COPY true if column is a matrix (else a scalar)
  int                   n_cells;        // #READ_ONLY #NO_COPY number of cells if a matrix
  int                   matrix_cell;    // #CONDSHOW_ON_is_matrix if column is a matrix, this is the cell within that matrix to plot -- specify -1 for all lines (only valid for data lines, not X or Z axes)
  FixedMinMax           fixed_range;    // fixed min/max range values for display (if not fixed, automatically set to min/max of data)
  bool                  show_axis_label;  // the axis label - display it?
  bool                  show_alt_axis_label;  // only pertains to the alt Y axis label - display it?

  RGBA                  color;          // #NO_AUTO_NAME color of the line and points

  MinMax                data_range;     // #READ_ONLY #NO_SAVE actual min and max of data (including fixed range)
  MinMax                range;          // #READ_ONLY #NO_SAVE actual display range of the axis data
  bool                  flip;           // flip the display of values on this axis

  int                   n_ticks;        // #DEF_10 number of ticks desired
  float                 axis_length;    // #READ_ONLY #NO_COPY in view units (width or depth)
  float                 start_tick;     // #READ_ONLY #NO_SAVE #NO_COPY first tick mark here
  float                 tick_incr;      // #READ_ONLY #NO_SAVE #NO_COPY increment for tick marks
  int                   act_n_ticks;    // #READ_ONLY #NO_SAVE #NO_COPY actual number of ticks
  double                units;          // #READ_ONLY #NO_SAVE #NO_COPY order of the units displayed (i.e. divide by this)

  T3DataView_List*      col_list;       // #READ_ONLY #NO_SAVE #NO_COPY list of columns for the col_lookup
  
  String_Array          group_by_values; // #READ_ONLY #NO_SAVE values of "group by" column
  bool                  group_by_initialized; // #READ_ONLY #NO_SAVE mark so we don't unnecessarily repeat this expensive operation
  virtual void          SetColPtr(GraphColView* cgv);
  GraphColView*         GetColPtr(); // get column pointer from col_name
  DataCol*              GetDAPtr();  // get dataarray ptr
  GraphTableView*       GetGTV();

  bool                  isString()      { return is_string; }

  inline float          GetDataVal(DataCol* dc, int row) {
    if(dc->is_matrix) return dc->GetValAsFloatM(row, matrix_cell);
    return dc->GetValAsFloat(row);
  }
  // main access to numerical data, dealing with matrix vs. not 
  inline String         GetDataString(DataCol* dc, int row) {
    if(dc->is_matrix) return dc->GetValAsStringM(row, matrix_cell);
    return dc->GetValAsString(row);
  }
  // main access to string data, dealing with matrix vs. not 


  ///////////////////////////////////////////////////
  //    Range Management

  virtual void          SetRange_impl(float first, float last);
  // set range to known good starting range values (fixed vals will still override); calls UpdateRange_impl to get "nice" vals still
  virtual bool          UpdateRange_impl(float first, float last);
  // update range with new min/max data -- returns true if range actually changed -- finds a "nice number" for min and max based on n_ticks, etc
  virtual void          ComputeRange();
  // compute range information based on data column, call UpdateRange_impl
  virtual bool          UpdateRange();
  // update range information based on last cell in data column, call UpdateRange_impl & returns true if a new range update
  virtual void          SetRange(float min, float max);
  // set the range as specified

  virtual void          ComputeTicks();
  // compute the start_tick, tick_incr, and act_n_ticks vals, based on current range info

  ///////////////////////////////////////////////////
  //    Rendering

  inline float          DataToPlot(float data)
  { if(range.Range() == 0.0f) return 0.0f;
    if(taMath_float::isnan(data)) data = 0.0f;
    float rval = axis_length * range.Normalize(data); if(flip) rval = axis_length - rval;
    return rval; }
  // convert data value to plotting value
  inline float          DistToPlot(float dist) // convert data value to plotting value
  { if(range.Range() == 0.0f) return 0.0f; return axis_length * range.Scale() * dist; }
  // convert a distance value in data units to plotting distance (doesn't subtract off min)
  virtual void          RenderAxis(T3Axis* t3ax, const iVec3f& off,
                                   int n_ax = 0, bool ticks_only=false,
                                   String* rnd_svg = NULL);
  // #IGNORE draw the actual axis in a given direction -- if n_ax > 0 then it is an alternative one (only for Y)

  ///////////////////////////////////////////////////
  //    Misc

  virtual void          InitFromUserData();
  // initialize various settings from the user data of the data column
  bool                  hasViewProperties() const override { return true; }
  virtual void          UpdateOnFlag();
  // update the 'on' flag for this column, taking into account whether there is actually any data column set (if not, on must be false)
  virtual void          UpdateFmColLookup();
  // if col_lookup is set, update our values from it
  virtual void          UpdateFmDataCol();
  // update various settings from the DataCol (matrix, string, etc)
  virtual void          RestoreGoodCol();
  // if current col_name is empty and col_name_good is non-empty, then copy col_name_good to col_name and try to plot that..

  void                  CopyFromView_base(GraphAxisBase* cp);
  // special copy function that just copies user view options in a robust manner
  void                  GetMemberCompletionList(const MemberDef* md, const String& cur_txt, Completions& completions) override;

  void InitLinks() override;
  void CutLinks() override;
  SIMPLE_COPY(GraphAxisBase);
  T3_DATAVIEWFUNS(GraphAxisBase, T3DataView)
protected:
  void        RenderAxis_X(T3Axis* t3ax, const iVec3f& off,
                           bool ticks_only=false, String* rnd_svg = NULL);
  void        RenderAxis_Z(T3Axis* t3ax, const iVec3f& off,
                           bool ticks_only=false, String* rnd_svg = NULL);
  void        RenderAxis_Y(T3Axis* t3ax, const iVec3f& off,
                           int n_ax = 0, bool ticks_only=false,
                           String* rnd_svg = NULL);

  void        UpdateAfterEdit_impl() override;
private:
  void        Initialize();
  void        Destroy();
};

#endif // GraphAxisBase_h
