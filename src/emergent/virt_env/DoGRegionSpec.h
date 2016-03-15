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

#ifndef DoGRegionSpec_h
#define DoGRegionSpec_h 1

// parent includes:
#include <VisRegionSpecBase>

// member includes:
#include <DoGFilter>
#include <XYNGeom>
#include <taVector2i>
#include <float_Matrix>
#include <V1KwtaSpec>

// declare all other types mentioned but not required to include:
class DataTable; // 


taTypeDef_Of(DoGRegionSpec);

class E_API DoGRegionSpec : public VisRegionSpecBase {
  // #STEM_BASE ##CAT_Image specifies a region of Difference-of-Gaussian retinal filters -- used as part of overall RetinaProc processing object -- takes image bitmap inputs and produces filter activation outputs
INHERITED(VisRegionSpecBase)
public:
  DoGFilter	dog_specs;	// Difference of Gaussian retinal filter specification
  DoGFilter	dog_specs_2;	// Difference of Gaussian retinal filter specification -- another set -- combined with any other sets selected
  DoGFilter	dog_specs_3;	// Difference of Gaussian retinal filter specification -- another set -- combined with any other sets selected
  bool          dog_color_only; // #CONDSHOW_ON_dog_specs.on only apply DoG's to color opponent channels -- not to monochrome channel -- only relevant for COLOR processing obviously..
  RenormMode	dog_renorm;	// #DEF_NO_RENORM how to renormalize the output of filters
  DataSave	dog_save;	// how to save the DoG outputs for the current time step in the data table
  V1KwtaSpec	dog_kwta;	// k-winner-take-all inhibitory dynamics for the DoG filter output -- applied to combined DoG feature output if there are multiple
  
  XYNGeom	dog_feat_geom; 	// #CONDSHOW_ON_dog_specs.on #READ_ONLY #SHOW size of one hypercolumn of features for DoG filtering -- x axis = color channel: 0 = monochrome (not if dog_color_only true), 1 = red vs. green, 2 = blue vs. yellow, y axis = 2 = on/off  (2 units total for monochrome, 4 or 6 total for color)
  taVector2i	dog_img_geom; 	// #CONDSHOW_ON_dog_specs.on #READ_ONLY #SHOW size of dog-filtered image output -- number of hypercolumns in each axis to cover entire output -- this is completely determined by retina_size, border and spacing parameters

  int		n_colors;	// #READ_ONLY number of color channels to be processed (1 = monochrome, 3 = full color, 2 = color + dog_color_only)
  int		n_dogs;	        // #READ_ONLY number of separate sets of dog filters in effect
  float_Matrix	dog_out_r;	// #READ_ONLY #NO_SAVE final output of the dog filter processing for the right eye -- [feat.x][feat.y][img.x][img.y]
  float_Matrix	dog_out_l;	// #READ_ONLY #NO_SAVE final output of the dog filter processing for the left eye -- [feat.x][feat.y][img.x][img.y]
  float_Matrix	dog_raw_r;	// #READ_ONLY #NO_SAVE raw, pre kwta output of the dog filter processing for the right eye -- [feat.x][feat.y][img.x][img.y]
  float_Matrix	dog_raw_l;	// #READ_ONLY #NO_SAVE raw, pre kwta output of the dog filter processing for the left eye -- [feat.x][feat.y][img.x][img.y]

  float_Matrix	kwta_gci;	// #READ_ONLY #NO_SAVE inhibitory conductances tmp for kwta
  
  virtual String GetDoGFiltName(int filt_no);
  // get name for each filter channel (0-5) = on;off;rvc;gvm;bvy;yvb

  virtual void	GraphDoGFilter(DataTable* disp_data, int n_filter = 1);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable #LABEL_GraphDoGFilter plot the difference-of-gaussians filter into data table and generate a graph -- can specify which filter to graph (1,2 or 3)
  virtual void	GridDoGFilter(DataTable* disp_data, int n_filter = 1);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable #LABEL_GridDoGFilter plot the difference-of-gaussians filter into data table and generate a grid view -- can specify which filter to graph (1,2 or 3)

  virtual void	PlotSpacing(DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable #ARGC_1 plot the arrangement of the filters (centers) in the data table using given value, and generate a grid view

  // todo: add CheckConfig!!

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(DoGRegionSpec);
protected:
  DoGFilter*    cur_dog_filter;	// currrent dog filter
  int           cur_dog_off;    // dog x offset

  void	UpdateAfterEdit_impl() override;

  void	UpdateGeom() override;

  bool NeedsInit() override;
  bool InitFilters() override;
  bool InitOutMatrix() override;
  bool InitDataTable() override;

  bool	FilterImage_impl(bool motion_only = false) override;

  virtual bool DoGFilterImage(float_Matrix* image, float_Matrix* out);
  // implementation of DoG filtering for a given image and output -- manages threaded calls to _thread version
  virtual void DoGFilterImage_thread(int thr_no);
  // threaded routine for actually filtering

  virtual bool DoGOutputToTable(DataTable* dtab, bool fmt_only = false);
  // send current time step of dog output to data table for viewing
    virtual bool DoGOutputToTable_impl(DataTable* dtab, float_Matrix* out,
                                       const String& col_sufx, bool fmt_only = false);
    // send current time step of dog output to data table for viewing
};

#endif // DoGRegionSpec_h
