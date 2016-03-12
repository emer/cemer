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


taTypeDef_Of(GradFilter);

class E_API GradFilter : public taOBase {
  // #STEM_BASE #INLINE ##CAT_Image defines an oriented gradient filter that responds maximally to a gradient in a particular orientation
  INHERITED(taOBase)
public:
  bool          on;             // is this filter active?
  int		size;	        // #CONDSHOW_ON_on size of the filter -- typically even
  int		spacing;	// #CONDSHOW_ON_on spacing between filters -- typically half-width
  int           n_angles;       // #CONDSHOW_ON_on #DEF_4 number of orientation angles -- typically 4
  float		gain;		// #CONDSHOW_ON_on #DEF_2 overall gain multiplier applied after filtering -- only relevant if not using renormalization (otherwize it just gets renormed away)
  bool		circle_edge;	// #CONDSHOW_ON_on #DEF_true cut off the filter (to zero) outside a circle of radius half-width -- makes the filter more radially symmetric

  float_Matrix	filter;	        // #READ_ONLY #NO_SAVE #NO_COPY filter values

  virtual void	RenderFilters(float_Matrix& flt);
  // #CAT_GradFilter render filter into matrix
  virtual void	UpdateFilters();
  // #CAT_GradFilter make our personal filters (RenderFilters(filter)) according to current params

  virtual void	GridFilters(DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter gaussian into data table and generate a grid view (reset any existing data first)

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(GradFilter);
protected:
  void	UpdateAfterEdit_impl() override;
};


taTypeDef_Of(DoGRegionSpec);

class E_API DoGRegionSpec : public VisRegionSpecBase {
  // #STEM_BASE ##CAT_Image specifies a region of Difference-of-Gaussian and gradient retinal filters -- used as part of overall RetinaProc processing object -- takes image bitmap inputs and produces filter activation outputs
INHERITED(VisRegionSpecBase)
public:
  DoGFilter	dog_specs;	// Difference of Gaussian retinal filter specification
  bool          dog_color_only; // #CONDSHOW_ON_dog_specs.on only apply DoG's to color opponent channels -- not to monochrome channel -- only relevant for COLOR processing obviously..
  RenormMode	dog_renorm;	// #CONDSHOW_ON_dog_specs.on #DEF_LOG_RENORM how to renormalize the output of filters
  DataSave	dog_save;	// #CONDSHOW_ON_dog_specs.on how to save the DoG outputs for the current time step in the data table -- if SEP_MATRIX is not selected and grad is selected, an integrated output will be produced
  GradFilter	grad_specs;	// Gradient filter -- picks up on gradual changes in illumination
  RenormMode	grad_renorm;	// #DEF_LOG_RENORM how to renormalize the output of filters
  DataSave	grad_save;	// how to save the gradient outputs for the current time step in the data table -- if SEP_MATRIX is not selected and dog is selected, an integrated output will be produced
  V1KwtaSpec	kwta;	        // k-winner-take-all inhibitory dynamics for the filter output -- applied to combined feature output if they are being combined, otherwise to grad if active, otherwise to dog
  
  XYNGeom	dog_feat_geom; 	// #CONDSHOW_ON_dog_specs.on #READ_ONLY #SHOW size of one hypercolumn of features for DoG filtering -- x axis = color channel: 0 = monochrome (not if dog_color_only true), 1 = red vs. green, 2 = blue vs. yellow, y axis = 2 = on/off  (2 units total for monochrome, 4 or 6 total for color)
  taVector2i	dog_img_geom; 	// #CONDSHOW_ON_dog_specs.on #READ_ONLY #SHOW size of dog-filtered image output -- number of hypercolumns in each axis to cover entire output -- this is completely determined by retina_size, border and spacing parameters
  XYNGeom	grad_feat_geom; // #CONDSHOW_ON_grad_specs.on #READ_ONLY #SHOW size of one hypercolumn of features for gradient filtering -- x axis = angle, y axis = 2 = polarity
  taVector2i	grad_img_geom; 	// #CONDSHOW_ON_grad_specs.on #READ_ONLY #SHOW size of grad-filtered image output -- number of hypercolumns in each axis to cover entire output -- this is completely determined by retina_size, border and spacing parameters
  bool          combined_out;   // #READ_ONLY #SHOW true if generating a combined dog & gradient output
  XYNGeom	cmb_feat_geom;  // #CONDSHOW_ON_combined_out #READ_ONLY #SHOW size of one hypercolumn of features for combined dog and gradient filters, if relevant

  float_Matrix	dog_out_r;	// #READ_ONLY #NO_SAVE output of the dog filter processing for the right eye -- [feat.x][feat.y][img.x][img.y]
  float_Matrix	dog_out_l;	// #READ_ONLY #NO_SAVE output of the dog filter processing for the left eye -- [feat.x][feat.y][img.x][img.y]

  float_Matrix	grad_out_r;	// #READ_ONLY #NO_SAVE output of the gradient filter processing for the right eye -- [feat.x][feat.y][img.x][img.y]
  float_Matrix	grad_out_l;	// #READ_ONLY #NO_SAVE output of the gradient filter processing for the left eye -- [feat.x][feat.y][img.x][img.y]

  float_Matrix	cmb_out_r;	// #READ_ONLY #NO_SAVE output of the combined filter processing for the right eye -- [feat.x][feat.y][img.x][img.y]
  float_Matrix	cmb_out_l;	// #READ_ONLY #NO_SAVE output of the combined filter processing for the left eye -- [feat.x][feat.y][img.x][img.y]

  float_Matrix	kwta_out_r;	// #READ_ONLY #NO_SAVE output of the kwta processing for the right eye -- [feat.x][feat.y][img.x][img.y]
  float_Matrix	kwta_out_l;	// #READ_ONLY #NO_SAVE output of the kwta processing for the left eye -- [feat.x][feat.y][img.x][img.y]

  float_Matrix	kwta_gci;	// #READ_ONLY #NO_SAVE inhibitory conductances tmp for kwta
  
  virtual String GetDoGFiltName(int filt_no);
  // get name for each filter channel (0-5) = on;off;rvc;gvm;bvy;yvb

  virtual void	GraphDoGFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable #LABEL_GraphDoGFilter plot the difference-of-gaussians filter into data table and generate a graph
  virtual void	GridDoGFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable #LABEL_GridDoGFilter plot the difference-of-gaussians filter into data table and generate a grid view
  virtual void	GridGradFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable #LABEL_GridGradFilter plot the gradient filterinto data table and generate a grid view

  virtual void	PlotSpacing(DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable #ARGC_1 plot the arrangement of the filters (centers) in the data table using given value, and generate a grid view

  // todo: add CheckConfig!!

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(DoGRegionSpec);
protected:
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

  virtual bool GradFilterImage(float_Matrix* image, float_Matrix* out);
  // implementation of Grad filtering for a given image and output -- manages threaded calls to _thread version
  virtual void GradFilterImage_thread(int thr_no);
  // threaded routine for actually filtering

  virtual bool CombineOuts(float_Matrix* cmb, float_Matrix* dog_out,
                           float_Matrix* grad_out);
  // combine dog and grad outputs

  virtual bool DoGOutputToTable(DataTable* dtab, bool fmt_only = false);
  // send current time step of dog output to data table for viewing
    virtual bool DoGOutputToTable_impl(DataTable* dtab, float_Matrix* out,
                                       const String& col_sufx, bool fmt_only = false);
    // send current time step of dog output to data table for viewing

  virtual bool GradOutputToTable(DataTable* dtab, bool fmt_only = false);
  // send current time step of grad output to data table for viewing
    virtual bool GradOutputToTable_impl(DataTable* dtab, float_Matrix* out,
                                       const String& col_sufx, bool fmt_only = false);
    // send current time step of grad output to data table for viewing
    
  virtual bool CombinedOutputToTable(DataTable* dtab, bool fmt_only = false);
  // send current time step of combined output to data table for viewing
    virtual bool CombinedOutputToTable_impl(DataTable* dtab, float_Matrix* out,
                                       const String& col_sufx, bool fmt_only = false);
    // send current time step of combined output to data table for viewing
    
};

#endif // DoGRegionSpec_h
