// Copyright, 1995-2007, Regents of the University of Colorado,
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

#ifndef ta_dataanal_h
#define ta_dataanal_h

#include "ta_datatable.h"
#include "ta_math.h"
#include "ta_program.h"

class ClustNode;

class TA_API ClustLink : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Math a link in the cluster tree with distance
INHERITED(taBase)
public:
  float		dist;		// distance to this node from parent
  ClustNode*	node;		// cluster node

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void	CutLinks();
  void	Copy_(const ClustLink& cp);
  TA_BASEFUNS_LITE(ClustLink);
};


class TA_API ClustNode : public taNBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Math node in clustering algorithm use one with leaves as children as a root node for cluster
INHERITED(taNBase)
public:
  taMatrix*	pat;		// pattern I point to (if leaf)
  int		leaf_idx;	// original leaf index, used for pointing into master distance table
  float_Matrix*	leaf_dists;	// distance matrix for all leaves
  float		par_dist;	// distance from parent cluster
  float		nn_dist;	// nearest neighbor (within cluster) distance
  float		tmp_dist;	// temporary distance value (for computations)
  float		y;		// y axis value
  taBase_List	children;	// my sub-nodes
  taBase_List	nns;		// nearest neighbor(s)

  ClustNode*	GetChild(int i)	const { return ((ClustLink*)children[i])->node; }
  ClustNode*	GetNN(int i)	const { return ((ClustLink*)nns[i])->node; }
  float& 	GetNNDist(int i) const { return ((ClustLink*)nns[i])->dist; }

  virtual void	SetPat(taMatrix* pt); // use setpointer to set pat
  virtual void	AddChild(ClustNode* nd, float dst = 0.0f);
  // add new child (via ClustLink)
  virtual void	LinkNN(ClustNode* nd, float dst = 0.0f);
  // add new neighbor (via ClustLink)
  virtual bool	RemoveChild(ClustNode* nd);// remove link with this node
  virtual int	FindChild(ClustNode* nd); // find child with this node link

  virtual bool 	Cluster(taMath::DistMetric metric=taMath::EUCLIDIAN,
			bool norm=false, float tol=0.0f);
  // #CAT_Cluster main call to generate the cluster: call on a root node initialized with a flat list of leaf children pointing to the patterns to be clustered

  virtual void	GraphData(DataTable* dt);
  // generate graph as X, Y, label points in a datatable, suitable for graphing
  virtual void	GraphData_impl(DataTable* dt);
  // #IGNORE implementation

  virtual void	NNeighbors(taMath::DistMetric metric, bool norm, float tol);
  // #IGNORE find nearest neighbors for everyone

  virtual bool	ClustOnClosest();
  // #IGNORE generate a new cluster by combining closest nearest neighbors together

  float	Dist(const ClustNode& oth, taMath::DistMetric metric, bool norm, float tol) const;
  // #IGNORE compute distance between this node and other, averaging over individual patterns

  virtual void	SetYs(float y_init = -1.0);
  // #IGNORE traverse the tree computing y values (starting from initial given value)

  virtual float	SetParDists(float par_d);
  // #IGNORE traverse the tree computing par_dist values (starting from given dist)

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void  InitLinks();
  void	CutLinks();
  SIMPLE_COPY(ClustNode);
  TA_BASEFUNS(ClustNode);
};


class TA_API taDataAnal : public taNBase {
  // #STEM_BASE ##CAT_Data collection of data analysis functions
INHERITED(taNBase)
public:

  static bool	GetDest(DataTable*& dest, const DataTable* src, const String& suffix);
  // #IGNORE get a dest analysis datatable -- if NULL, make one in proj.data.AnalysisData with name based on src and suffix
  static DataCol* GetMatrixDataCol(DataTable* src_data, const String& data_col_nm);
  // #IGNORE get named column from data, with checks that it is a matrix of type float or double
  static DataCol* GetStringDataCol(DataTable* src_data, const String& name_col_nm);
  // #IGNORE get named column from data, with checks that it is a non-matrix of type String
  static DataCol* GetNumDataCol(DataTable* src_data, const String& name_col_nm);
  // #IGNORE get named column from data, with checks that it is a non-matrix of numeric type
  
  ///////////////////////////////////////////////////////////////////
  // basic statistics

  static String	RegressLinear(DataTable* src_data, const String& x_data_col_nm,
			      const String& y_data_col_nm, bool render_line = true);
  // #CAT_Stats #MENU_BUTTON #MENU_ON_Stats compute linear regression (least squares fit of function y = mx + b) to given data -- if render_line, a column called "regress_line" is created and the function is generated into it as data.  Returns a string descriptor of the regression equation and r value

  ///////////////////////////////////////////////////////////////////
  // distance matricies

  static bool	DistMatrix(float_Matrix* dist_mat, DataTable* src_data,
			   const String& data_col_nm,
			   taMath::DistMetric metric, bool norm=false, float tol=0.0f, 
			   bool incl_scalars=false);
  // #CAT_Distance compute distance matrix for given matrix data column in src_data datatable. if data_col_nm is blank, all real-valued matrix (and scalars if if incl_scalars) columns are used, summing across.  dist_mat returns a square symmetric matrix with cells as the distance between each row and every other row of matrix data.  distance data is converted to float regardless of source type (float or double)
  static bool	DistMatrixTable(DataTable* dist_mat, bool view, DataTable* src_data,
				const String& data_col_nm, const String& name_col_nm,
				taMath::DistMetric metric, bool norm=false, float tol=0.0f,
				bool incl_scalars=false);
  // #CAT_Distance #MENU_BUTTON #MENU_ON_Distance compute distance matrix table for given matrix data column in src_data datatable.  dist_mat returns a square symmetric matrix with cells as the distance between each row and every other row of matrix data.  if data_col_nm is blank, all real-valued matrix (and scalars if if incl_scalars) columns are used, summing across.  if name_col_nm is non-empty and valid, nxn scalar float rows and columns are made, with names from name_col_nm values from src_data table; otherwise a single matrix column is made, named by the src_data name + "_DistMatrix".  if view, then a grid view in a new frame is automatically created

  static bool	CrossDistMatrix(float_Matrix* dist_mat,
				DataTable* src_data_a, const String& data_col_nm_a,
				DataTable* src_data_b, const String& data_col_nm_b,
				taMath::DistMetric metric, bool norm=false, float tol=0.0f,
				bool incl_scalars=false);
  // #CAT_Distance compute cross distance matrix between two different matrix data columns in src_data_a and src_data_b datatables. if data_col_nm's are both blank, all real-valued matrix (and scalars if if incl_scalars) columns are used, summing across (only cols at same col index that match exactly will be used). dist_mat returns a symmetric matrix with cells as the distance between each row in table a versus each row of table b.  distance data is converted to float regardless of source type (float or double)
  static bool	CrossDistMatrixTable(DataTable* dist_mat, bool view,
				     DataTable* src_data_a, const String& data_col_nm_a,
				     const String& name_col_nm_a,
				     DataTable* src_data_b, const String& data_col_nm_b,
				     const String& name_col_nm_b,
				     taMath::DistMetric metric, bool norm=false, float tol=0.0f,
				     bool incl_scalars=false);
  // #CAT_Distance #MENU_BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable compute cross distance matrix table between two different matrix data columns in src_data_a and srd_data_b datatables. if data_col_nm's are both blank, all real-valued matrix (and scalars if if incl_scalars) columns are used, summing across (only cols at same col index that match exactly will be used).  dist_mat returns a symmetric matrix with cells as the distance between each row in table a versus each row of table b. if name_col_nm is non-empty and valid, nxm scalar float rows and columns are made, with names from name_col_nm values from src_data table; otherwise a single matrix column is made, named by the src_data_a + "_" + src_data_b + "_DistMatrix".  if view, then a grid view in a new frame is automatically created

  ///////////////////////////////////////////////////////////////////
  // correlation matricies

  static bool	CorrelMatrix(float_Matrix* correl_mat, DataTable* src_data,
			     const String& data_col_nm);
  // #CAT_Correlation compute correlation matrix for given matrix data column in src_data datatable.  correl_mat returns the correlation for how each cell in the matrix data varies across rows (e.g., time) as compared to all the other cells.  result is ncells x ncells symmetric square matrix.  correlation data is converted to float regardless of source type (float or double)
  static bool	CorrelMatrixTable(DataTable* correl_mat, bool view, DataTable* src_data,
				  const String& data_col_nm);
  // #CAT_Correlation #MENU_BUTTON #MENU_SEP_BEFORE #NULL_OK_0 #NULL_TEXT_0_NewDataTable compute correlation matrix for given matrix data column in src_data datatable.  correl_mat returns the correlation for how each cell in the matrix data varies across rows (e.g., time) as compared to all the other cells. a single matrix column is made, named by the src_data name + "_CorrelMatrix". if view, then a grid view in a new frame is automatically created

  // todo: do this?
//   virtual void	GpDistArray(float_RArray& within_dist_ary, float_RArray& between_dist_ary, int pat_no,
// 			    float_RArray::DistMetric metric=float_RArray::HAMMING,
// 			    bool norm=false, float tol=0.0f);
//   // get within group and between group distance matricies as arrays for events based on pattern pat_no

  ///////////////////////////////////////////////////////////////////
  // standard high-dimensional data analysis methods

  static bool	Cluster(DataTable* clust_data, bool view, DataTable* src_data,
			const String& data_col_nm, const String& name_col_nm,
			taMath::DistMetric metric=taMath::EUCLIDIAN,
			bool norm=false, float tol=0.0f);
  // #CAT_HighDim #MENU_BUTTON #MENU_ON_HighDim #NULL_OK_0 #NULL_TEXT_0_NewDataTable produce a hierarchical clustering of the distances between patterns in given data column from source data, with labels from given name_col_nm, using given distance metric.  if view, the resulting data table is graphed to produce a cluster plot.  NULL DataTable = create new one

  static bool	PCAEigens(float_Matrix* eigen_vals, float_Matrix* eigen_vecs,
			  DataTable* src_data, const String& data_col_nm);
  //  #CAT_HighDim get principal components analysis (PCA) eigenvalues and eigenvectors of correlation matrix across rows for given matrix column name in source data

  static bool	PCAEigenTable(DataTable* pca_data, bool view, DataTable* src_data,
			      const String& data_col_nm);
  //  #CAT_HighDim #MENU_BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable get principal components analysis (PCA) eigenvalues and eigenvectors of correlation matrix across rows for given matrix column name in source data. NULL DataTable = create new one

  static bool	PCA2dPrjn(DataTable* prjn_data, bool view, DataTable* src_data,
			  const String& data_col_nm, const String& name_col_nm,
			  int x_axis_component=0, int y_axis_component=1);
  // #CAT_HighDim #MENU_BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable perform principal components analysis of the correlations of patterns in given columm across rows, plotting projections of patterns on the given principal components in the data table.  if name_col_nm not empty, rows are labeled with these names.  NULL DataTable = create new one

  static bool	MDS2dPrjn(DataTable* prjn_data, bool view, DataTable* src_data,
			  const String& data_col_nm, const String& name_col_nm,
			  int x_axis_component=0, int y_axis_component=1,
			  taMath::DistMetric metric=taMath::EUCLIDIAN,
			  bool norm=false, float tol=0.0);
  // #CAT_HighDim #MENU_BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable perform multidimensional scaling on the distance matrix (computed according to metric, norm, tol parameters) of patterns in column name across rows, putting the resulting projections into prjn_data.  if name_col_nm not empty, rows are labeled with these names.  NULL DataTable = create new one
  static bool	RowPat2dPrjn(DataTable* prjn_data, bool view, DataTable* src_data,
			     const String& data_col_nm, const String& name_col_nm,
			     int x_row=0, int y_row=1,
			     taMath::DistMetric metric=taMath::INNER_PROD,
			     bool norm=false, float tol=0.0);
  // #CAT_HighDim #MENU_BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable project all rows according to their projection onto the two specified rows of patterns using given distance metrics

  ///////////////////////////////////////////////////////////////////
  // data cleaning operations

  static bool	TimeAvg(DataTable* time_avg_data, bool view, DataTable* src_data,
			float avg_dt, bool float_only=true);
  // #CAT_Clean #MENU_BUTTON #MENU_ON_Clean #NULL_OK_0 #NULL_TEXT_0_NewDataTable compute the time average for all the numeric fields of source data, according to the given avg_dt (new_val = avg_dt * avg + (1-avg_dt) * old_val).  if(float_only) then int or byte data is not time averaged

  static bool	SmoothImpl(DataTable* smooth_data, bool view, DataTable* src_data,
			   float_Matrix* flt_kern, double_Matrix* dbl_kern,
			   int kern_half_wd, bool keep_edges, bool float_only);
  // #IGNORE impl function for smoothing -- takes the kernel and does the job!

  static bool	SmoothUniform(DataTable* smooth_data, bool view, DataTable* src_data,
			      int kern_half_wd, bool neg_tail = true,
			      bool pos_tail = true, bool keep_edges = true,
			      bool float_only=true);
  // #CAT_Clean #MENU_BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable uniform smoothing: compute the uniform average of all the numeric fields of source data, using a uniform kernel of given half-width.  neg and pos tail determine if kernel includes the negative (earlier) side and the positive (later) side.  if(keep_edges) then smooth data is same size as src_data, otherwise edges of kern_half_wd are lost on either side of the data.  if(float_only) then int or byte data is not averaged

  static bool	SmoothGauss(DataTable* smooth_data, bool view, DataTable* src_data,
			    int kern_half_wd, float kern_sigma, bool neg_tail = true,
			    bool pos_tail = true, bool keep_edges = true,
			    bool float_only=true);
  // #CAT_Clean #MENU_BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable gaussian smoothing: compute the gaussian-convolved average for all the numeric fields of source data, using a gaussian kernel of given half-width and sigma (std deviation).  neg and pos tail determine if kernel includes the negative (earlier) side and the positive (later) side.  if(keep_edges) then smooth data is same size as src_data, otherwise edges of kern_half_wd are lost on either side of the data.  if(float_only) then int or byte data is not averaged

  static bool	SmoothExp(DataTable* smooth_data, bool view, DataTable* src_data,
			  int kern_half_wd, float kern_exp, bool neg_tail = true,
			  bool pos_tail = false, bool keep_edges = true,
			  bool float_only=true);
  // #CAT_Clean #MENU_BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable exponential smoothing: compute the exponentially-convolved average for all the numeric fields of source data, using an exponential kernel of given half-width and exponent.  neg and pos tail determine if kernel includes the negative (earlier) side and the positive (later) side. if(keep_edges) then smooth data is same size as src_data, otherwise edges of kern_half_wd are lost on either side of the data.  if(float_only) then int or byte data is not averaged

  static bool	SmoothPow(DataTable* smooth_data, bool view, DataTable* src_data,
			  int kern_half_wd, float kern_exp, bool neg_tail = true,
			  bool pos_tail = false, bool keep_edges = true,
			  bool float_only=true);
  // #CAT_Clean #MENU_BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable power-function smoothing: compute the power-function-convolved average for all the numeric fields of source data, using an power-function kernel of given half-width and exponent (typically negative).  neg and pos tail determine if kernel includes the negative (earlier) side and the positive (later) side. if(keep_edges) then smooth data is same size as src_data, otherwise edges of kern_half_wd are lost on either side of the data.  if(float_only) then int or byte data is not averaged

  ///////////////////////////////////////////////////////////////////
  // data graphing prep functions

  static bool	Matrix3DGraph(DataTable* data, const String& x_axis_col, const String& z_axis_col);
  // #CAT_Graph #MENU_BUTTON #MENU_ON_Graph prepare data for a 3D matrix graph, where data is plotted by X and Z axis values -- sorts data by X then Z, then adds a duplicate copy of data sorted by Z then X, which produces a matrix grid in a graph view plot (turn off the Z neg draw flag)


//   static bool	PatFreqArray(float_RArray& freqs, int pat_no, float act_thresh = .5f, bool proportion = false);
//   // get frequency (proportion) of pattern activations greater than act_thresh across events
//   static bool	PatFreqText(float act_thresh = .5f, bool proportion = false, ostream& strm = cerr);
//   // #MENU_BUTTON #ARGC_2 report frequency (proportion) of pattern values greater than act_thresh across events, to a text output (most useful if pattern values are named in value_names)
//   static bool	PatFreqGrid(GridLog* disp_log, float act_thresh = .5f, bool proportion = false);
//   // #MENU_BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable report frequency (proportion) of pattern values greater than act_thresh across events, to a grid log (NULL = make new log)
//   static bool	PatAggArray(float_RArray& agg_vals, int pat_no, Aggregate& agg);
//   // aggregate pattern pat_no values over events to given array object
//   static bool	PatAggText(Aggregate& agg, ostream& strm = cerr);
//   // #MENU_BUTTON #ARGC_1 aggregate patterns over events and print aggregated results to a text output (most useful if pattern values are named in value_names)
//   static bool	PatAggGrid(GridLog* disp_log, Aggregate& agg);
//   // #MENU_BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable aggregate patterns over events and plot aggregated results in a grid log (NULL = make new log)
//   static bool 	EventFreqText(bool proportion = false, ostream& strm = cerr);
//   // #MENU_BUTTON #ARGC_1 report frequency (proportion) of event names in the environment

  override String 	GetTypeDecoKey() const { return "DataTable"; }
  void Initialize() { };
  void Destroy() { };
  TA_BASEFUNS_NOCOPY(taDataAnal);
};

/////////////////////////////////////////////////////////
//   programs to support data generation
/////////////////////////////////////////////////////////

class TA_API DataAnalCall : public StaticMethodCall { 
  // call a taDataAnal (data analysis) function
INHERITED(StaticMethodCall)
public:
  TA_BASEFUNS_NOCOPY(DataAnalCall);
private:
  void	Initialize();
  void	Destroy()	{ };
}; 

#endif // ta_dataanal_h
