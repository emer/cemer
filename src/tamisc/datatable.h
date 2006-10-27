// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#ifndef datatable_h
#define datatable_h

#include "ta_datatable.h"
#include "ta_math.h"
#include "ta_geometry.h"
#include "minmax.h"
#include "colorscale.h" // for DT && DA Viewspecs
#include "tamisc_TA_type.h"


// forwards this file

class ClustNode; //

// todo: move float_RArray to v3compat
// remaining dependencies:
// src/pdp/netstru.h:  virtual void        ConValuesToArray(float_RArray& ary, const char* variable);
// src/pdp/netstru.h:  virtual void        ConValuesFromArray(float_RArray& ary, const char* variable);
// src/pdp/netstru.h:  virtual void        UnitValuesToArray(float_RArray& ary, const char* variable);
// src/pdp/netstru.h:  virtual void        UnitValuesFromArray(float_RArray& ary, const char* variable);
// src/tamisc/datagraph.h:  float_RArray           rows_x_axis;    // x axis values for plot rows mode
// src/tamisc/fun_lookup.h:class TAMISC_API FunLookup : public float_RArray {
// src/tamisc/fun_lookup.h:  COPY_FUNS(FunLookup, float_RArray);

class TAMISC_API float_RArray : public float_Array {
  // #NO_UPDATE_AFTER float array with range, plus a lot of other mathematical functions
public:
  enum DistMetric {		// generalized distance metrics
    SUM_SQUARES,		// sum of squares:  sum[(x-y)^2]
    EUCLIDIAN,			// Euclidian distance (sqrt of sum of squares)
    HAMMING, 			// Hamming distance: sum[abs(x-y)]
    COVAR,			// covariance: sum[(x-<x>)(y-<y>)]
    CORREL,			// correlation: sum[(x-<x>)(y-<y>)] / sqrt(sum[x^2 y^2])
    INNER_PROD,			// inner product: sum[x y]
    CROSS_ENTROPY		// cross entropy: sum[x ln(x/y) + (1-x)ln((1-x)/(1-y))]
  };

  MinMax	range;		// #NO_SAVE min-max range of the data

  virtual void	UpdateRange(float item)	{ range.UpdateRange(item); }
  virtual void	UpdateAllRange();
  // #MENU #MENU_ON_Actions update range for all items in array

  void		Set(int i, const float& item)
  { float_Array::Set(i, item); UpdateRange(item); }
  void		Add(const float& item);
  void		Insert(const float& item, int idx, int n_els=1);

  virtual float	MaxVal(int& idx, int start=0, int end = -1) const;
  // #MENU #MENU_ON_Actions #USE_RVAL value and index of the (first) element that has the maximum value
  virtual float	AbsMaxVal(int& idx, int start=0, int end = -1) const;
  // #MENU #MENU_ON_Actions #USE_RVAL value and index of the (first) element that has the maximum absolute value
  virtual float	MinVal(int& idx, int start=0, int end = -1) const;
  // #MENU #USE_RVAL value and index of the (first) element that has the minimum value
  virtual float	Sum(int start=0, int end = -1) const;
  // #MENU #MENU_ON_Actions #USE_RVAL compute the sum of the values in the array
  virtual float	Mean(int start=0, int end = -1) const;
  // #MENU #USE_RVAL compute the mean of the values in the array
  virtual float	Var(float mean=0, bool use_mean=false, int start=0, int end = -1) const;
  // #MENU #USE_RVAL compute the variance of the values, opt with given mean
  virtual float	StdDev(float mean=0, bool use_mean=false, int start=0, int end = -1) const;
  // #MENU #USE_RVAL compute the standard deviation of the values, opt with given mean
  virtual float	SEM(float mean=0, bool use_mean=false, int start=0, int end = -1) const;
  // #MENU #USE_RVAL compute the standard error of the mean of the values, opt with given mean
  virtual float	SSLength(int start=0, int end = -1) const;
  // #MENU #USE_RVAL sum-of-squares length of the array

  // generalized distance measures follow
  virtual float	SumSquaresDist(const float_RArray& oth, bool norm = false,
			       float tolerance=0.0f, int start=0, int end = -1) const;
  // compute sum-squares dist between this and the oth, tolerance is by element
  virtual float	EuclidDist(const float_RArray& oth, bool norm = false,
			   float tolerance=0.0f, int start=0, int end = -1) const;
  // compute Euclidian dist between this and the oth, tolerance is by element
  virtual float	HammingDist(const float_RArray& oth, bool norm = false,
			    float tolerance=0.0f, int start=0, int end = -1) const;
  // compute Hamming dist between this and the oth, tolerance is by element
  virtual float	Covar(const float_RArray& oth, int start=0, int end = -1) const;
  // compute the covariance of this array the oth array
  virtual float	Correl(const float_RArray& oth, int start=0, int end = -1) const;
  // compute the correlation of this array with the oth array
  virtual float	InnerProd(const float_RArray& oth, bool norm = false,
			  int start=0, int end = -1) const;
  // compute the inner product of this array and the oth array
  virtual float	CrossEntropy(const float_RArray& oth, int start=0, int end = -1) const;
  // compute cross entropy between this and other array, this is 'p' other is 'q'

  virtual float	Dist(const float_RArray& oth, DistMetric metric, bool norm = false,
			float tolerance=0.0f, int start=0, int end = -1) const;
  // compute generalized distance metric with other array (calls appropriate fun above)
  static bool	LargerFurther(DistMetric metric);
  // returns true if a larger value of given distance metric means further apart

  virtual void	Histogram(const float_RArray& oth, float bin_size);
  // this gets a histogram (counts) of number of values in other array

  virtual void	AggToArray(const float_RArray& from, Aggregate& agg,
			   int start=0, int end = -1);
  // aggregate values from other array to this one using aggregation params of agg
  virtual float	AggToVal(Aggregate& agg, int start=0, int end = -1) const;
  // compute aggregate of values in this array using aggregation params of agg

  virtual float	NormLen(float len=1.0f, int start=0, int end = -1);
  // #MENU normalize array to total given length (1.0), returns scale
  virtual float	NormSum(float sum=1.0f, float min_val=0.0f, int start=0, int end = -1);
  // #MENU normalize array to total given sum (1.0) and min_val (0) (uses range), returns scale
  virtual float	NormMax(float max=1.0f, int start=0, int end = -1);
  // #MENU normalize array to given maximum value, returns scale
  virtual float	NormAbsMax(float max=1.0f, int start=0, int end = -1);
  // #MENU normalize array to given absolute maximum value, returns scale
  virtual void	SimpleMath(const SimpleMathSpec& math_spec, int start=0, int end = -1);
  // #MENU apply standard kinds of simple math operators to values in the array
  virtual void	SimpleMathArg(const float_RArray& arg_ary, const SimpleMathSpec& math_spec, int start=0, int end = -1);
  // apply simple math operators to values, other array provides 'arg' value for math_spec
  virtual int	Threshold(float thresh=.5f, float low=0.0f, float high=1.0f, int start=0, int end = -1);
  // #MENU threshold values in the array, low vals go to low, etc.

  ////////////////////////////////////
  // todo: above are all in taMath, below go in image processing..

  virtual void 	WritePoint(const TwoDCoord& geom, int x, int y, float color=1.0, bool wrap=true);
  // write a single point, assuming geometry geom
  virtual void 	RenderLine(const TwoDCoord& geom, int xs, int ys, int xe, int ye,
			   float color=1.0, bool wrap=true);
  // #MENU render a line from given x,y starting, ending coords in 2d space of geometry geom

  virtual void 	WriteXPoints(const TwoDCoord& geom, int x, int y, const float_RArray& color,
			     int wdth=1, bool wrap=true);
  // write a series of points of given width in x dimension using colors in order
  virtual void 	WriteYPoints(const TwoDCoord& geom, int x, int y, const float_RArray& color,
			     int wdth=1, bool wrap=true);
  // write a series of points of given width in y dimension using colors in order
  virtual void 	RenderWideLine(const TwoDCoord& geom, int xs, int ys, int xe, int ye,
			       const float_RArray& color, int wdth=1, bool wrap=true);
  // #MENU render a wide line from given x,y starting, ending coords in 2d space of geometry geom

  inline const float& SafeMatEl(int col_dim, int row, int col) const { return SafeEl((row * col_dim) + col); }
  // safe get element assuming a matrix layout of values with column (inner) dimension size = col_dim
  inline const float& FastMatEl(int col_dim, int row, int col) const { return FastEl((row * col_dim) + col); }
  inline float& FastMatEl(int col_dim, int row, int col) { return FastEl((row * col_dim) + col); }
  // fast get element assuming a matrix layout of values with column (inner) dimension size = col_dim
  inline const float& FastMatEl1(int col_dim, int row, int col) const { return FastEl(((row-1) * col_dim) + (col-1)); }
  // fast get element assuming a matrix layout of values with column (inner) dimension size = col_dim, indicies use 1-n range instead of 0-n-1!
  inline float& FastMatEl1(int col_dim, int row, int col) { return FastEl(((row-1) * col_dim) + (col-1)); }
  inline const float& FastEl1(int idx) const { return FastEl(idx-1); }
  inline float& FastEl1(int idx) { return FastEl(idx-1); }
  // fast get element with index in 1-n range instead of 0-n-1
  virtual void 	GetMatCol(int col_dim, float_RArray& col_vec, int col_no);
  // extract given column from this matrix-formatted object
  virtual void 	GetMatRow(int col_dim, float_RArray& row_vec, int row_no);
  // extract given row from this matrix-formatted object

  // x  0 1 2 3  dim = 4
  // 0  0 1 2 3
  // 1    3 4 5 + dim = 4
  // 2      6 7 + dim + (dim - 1) = 4 + 3
  // 3        8 (y-x) + (x * dim - sum(1..x-1)); sum(1..x) = (x*(x+1)) / 2
  inline const float& SafeTriMatEl(int dim, int x, int y) const {
    if(x < y)  	return SafeEl((y-x) + (x * dim) - (((x-1) * x) / 2));
    else	return SafeEl((x-y) + (y * dim) - (((y-1) * y) / 2));
  }
  // get element assuming an upper-triangular symmetric matrix (e.g., distance matrix) of dimension dim for two items, x, y
  inline const float& FastTriMatEl(int dim, int x, int y) const {
    if(x < y)  return FastEl((y-x) + (x * dim) - (((x-1) * x) / 2));
    else       return FastEl((x-y) + (y * dim) - (((y-1) * y) / 2));
  }
  inline float& FastTriMatEl(int dim, int x, int y) {
    if(x < y)  return FastEl((y-x) + (x * dim) - (((x-1) * x) / 2));
    else       return FastEl((x-y) + (y * dim) - (((y-1) * y) / 2));
  }
  // get element assuming an upper-triangular symmetric matrix (e.g., distance matrix) of dimension dim for two items, x, y
  inline void	AllocSqMatSize(int dim) { EnforceSize(dim * dim); }
  // allocate space (enforcesize) for a square matrix of size dim
  inline void	AllocTriMatSize(int dim) { EnforceSize((dim * (dim + 1)) / 2); }
  // allocate space (enforcesize) for an upper-triangular matrix of size dim

  virtual void 	CopyFmTriMat(int dim, const float_RArray& tri_mat);
  // copy from upper-triangular symmetric matrix of given dimensionality into a full matrix

  virtual bool	TriDiagMatRed(int dim, float_RArray& diags, float_RArray& off_diags);
  // reduce current full square matrix to a tri-diagonal form using the Householder transformation (first step in computing eigenvectors/values) -- diags are eigenvalues
  virtual bool	TriDiagQL(int dim, float_RArray& diags, float_RArray& off_diags);
  // perform QL algorithm to compute eigenvector/values of a tri-diagonal matrix as computed by TriDiagMatRed (this = matrix)
  virtual bool	Eigens(int dim, float_RArray& evals);
  // compute eigenvalue/vector decomposition of this square matrix of dimension dim.  eigen values are in evals, and this matrix contains the eigenvectors
  virtual bool	MDS(int dim, float_RArray& xcoords, float_RArray& ycoords, int x_axis_component = 0, int y_axis_component = 1, bool print_eigen_vals = false);
  // perform multidiminesional scaling of this distace matrix (must be full square matrix), returning two-dimensional coordinates that best capture the distance relationships among the items in x,y coords using specified components

  void		Reset(){float_Array::Reset();range.Init(0.0f);}

  void	Initialize()		{ };
  void	Destroy()		{ };
  void	InitLinks();
  void 	Copy_(const float_RArray& cp);
  COPY_FUNS(float_RArray, float_Array);
  TA_BASEFUNS(float_RArray);
protected:
  override void	ItemAdded_(const void*, int n = 1);
  override void	ItemRemoved_(const void*, int n = 1);
  override void	ItemsChanged_();
};


/* these are the routines that used to be on DataTable:

  void			AddRowToArray(float_RArray& ar, int row_num) const;
  // add a row of the datatable to given array
  void			AggRowToArray(float_RArray& ar, int row_num, Aggregate& agg) const;
  // aggregate a row of the datatable to given array using parameters in agg
  float			AggRowToVal(int row_num, Aggregate& agg) const;
  // aggregate a row of the datatable to a value using parameters in agg
  void			AddArrayToRow(float_RArray& ar);
  // add contents of array to datatable
  void			AggArrayToRow(const float_RArray& ar, int row_num, Aggregate& agg);
  // aggregate contents of array to datatable at given row
  void			PutArrayToRow(const float_RArray& ar, int row_num);
  // just put array values into given row of data
  void			PutArrayToCol(const float_RArray& ar, int col);
  // just put array values into given column (if subgp >= 0, column is in given subgroup)
*/


// todo: move this to taMath::Cluster routine!

class TAMISC_API ClustLink : public taBase {
  // #INLINE #INLINE_DUMP ##NO_TOKENS ##NO_UPDATE_AFTER a link in the cluster tree with distance
public:
  float		dist;		// distance to this node from parent
  ClustNode*	node;		// cluster node

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void	CutLinks();
  void	Copy_(const ClustLink& cp);
  COPY_FUNS(ClustLink, taBase);
  TA_BASEFUNS(ClustLink);
};


class TAMISC_API ClustNode : public taNBase {
  /* ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER node in clustering algorithm
     use one with leaves as children as a root node for cluster */
INHERITED(taNBase)
public:
  float_RArray*	pat;		// pattern I point to (if leaf)
  int		leaf_idx;	// original leaf index, used for pointing into master distance table
  int		leaf_max;	// original max number of leaves
  float_RArray*	leaf_dists;	// distance matrix for all leaves
  float		par_dist;	// distance from parent cluster
  float		nn_dist;	// nearest neighbor (within cluster) distance
  float		tmp_dist;	// temporary distance value (for computations)
  float		y;		// y axis value
  taBase_List	children;	// my sub-nodes
  taBase_List	nns;		// nearest neighbor(s)

  ClustNode*	GetChild(int i)	const { return ((ClustLink*)children[i])->node; }
  ClustNode*	GetNN(int i)	const { return ((ClustLink*)nns[i])->node; }
  float& 	GetNNDist(int i) const { return ((ClustLink*)nns[i])->dist; }

  virtual void	SetPat(float_RArray* pt); // use setpointer to set pat
  virtual void	AddChild(ClustNode* nd, float dst = 0.0f);
  // add new child (via ClustLink)
  virtual void	LinkNN(ClustNode* nd, float dst = 0.0f);
  // add new neighbor (via ClustLink)
  virtual bool	RemoveChild(ClustNode* nd);// remove link with this node
  virtual int	FindChild(ClustNode* nd); // find child with this node link

  virtual void 	Cluster(float_RArray::DistMetric metric=float_RArray::EUCLIDIAN,
			bool norm=false, float tol=0.0f);
  // generate the cluster: call on a root node with a flat list of leaf children

  virtual void	Graph(ostream& strm);
  // generate commands to drive graph (or xgraph) for plotting cluster
  virtual void	Graph_impl(ostream& strm);
  // #IGNORE implementation

  virtual void	XGraph(const String& fnm, const String& title);
  // generate graph in given file name with given title, and call xgraph on result

  virtual void	GraphData(DataTable* dt);
  // generate graph as X, Y, label points in a datatable, suitable for graphing
  virtual void	GraphData_impl(DataTable* dt);
  // #IGNORE implementation

  virtual void	NNeighbors(float_RArray::DistMetric metric,
		    bool norm, float tol);
  // #IGNORE find nearest neighbors for everyone

  virtual bool	ClustOnClosest(float_RArray::DistMetric metric);
  // #IGNORE generate a new cluster by combining closest nearest neighbors together

  float	Dist(const ClustNode& oth, float_RArray::DistMetric metric,
	     bool norm, float tol) const;
  // #IGNORE compute distance between this node and other, averaging over individual patterns

  virtual void	SetYs(float y_init = -1.0);
  // #IGNORE traverse the tree computing y values (starting from initial given value)

  virtual float	SetParDists(float par_d, float_RArray::DistMetric metric);
  // #IGNORE traverse the tree computing par_dist values (starting from given dist)

  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void  InitLinks();
  void	CutLinks();
  SIMPLE_COPY(ClustNode);
  COPY_FUNS(ClustNode, taNBase);
  TA_BASEFUNS(ClustNode);
};

/*
  Additional Display Options
    WIDTH=i (i: int) -- sets default column width to i chars
    NARROW -- in addition to base spec, also sets column with to 8 chars
*/

class TAMISC_API GridColViewSpec : public DataColViewSpec {
  // information for display of a data array in a grid display
INHERITED(DataColViewSpec)
public:
  enum DisplayStyle {
    TEXT	= 0x01,	// Draw using text only (default for scalar cells)
    BLOCK	= 0x02,	// Draw using blocks only (default for matrix cells)
    TEXT_AND_BLOCK = 0x03, // Draw using both blocks and text
    IMAGE = 0x04	// Draw an image (assumes col type has BW or Color image data)
#ifndef __MAKETA__
    ,TEXT_MASK = 0x01 // mask to see if TEXT is in use
    ,BLOCK_MASK = 0x02 // mask to see if BLOCK is in use
#endif
  };

  enum MatrixLayout { // order of display for matrix cols
    TOP_ZERO, // row zero is displayed at top of cell (ex. for images)
    BOT_ZERO // row zero is displayed at bottom of cell (ex for patterns)
  };

  enum BlockColor { // ways that grid blocks can be filled
    COLOR,	// color indicates value
    BW_SHADE, 	// black&white shading indicates value
    SOLID	// no shading (only for use with Area/Linear/Height)
  };
  
  enum BlockFill { // ways that grid blocks can be filled
    FILL,	// just fill the whole block
    AREA,	// area indicates value
    LINEAR, 	// linear size of square side indicates value
    HEIGHT	// extrude into view; 3D height indicates value
  };
  
  DisplayStyle  display_style;	// can display as text and/or block, or image
  int		text_width;	// #CONDEDIT_ON_display_style:TEXT for text cols, width of the column in chars
  FontSpec	font; // font for this column
  MatrixLayout	layout;	// #EXPERT #CONDEDIT_OFF_display_style:TEXT layout of matrix cells
  BlockColor	block_color; // #CONDEDIT_OFF_display_style:TEXT,IMAGE color of matrix cells
  BlockFill	block_fill; // #CONDEDIT_OFF_display_style:TEXT,IMAGE fill of matrix cells
  bool		scale_on; // #CONDEDIT_ON_display_style:BLOCK,TEXT_AND_BLOCK adjust overall colorscale to include this data
  
  float 	col_width; // #IGNORE calculated col_width 
  float		row_height; // #IGNORE calculated row height

  override void		setFont(const FontSpec& value);
  
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const GridColViewSpec& cp);
  COPY_FUNS(GridColViewSpec, DataColViewSpec);
  TA_BASEFUNS(GridColViewSpec);
protected:
  void			BuildFromDataArray_impl(bool first);
  virtual void 		InitDisplayParams();
private:
  void 	Initialize();
  void	Destroy() {}
};

class TAMISC_API GridTableViewSpec : public DataTableViewSpec {
  // information for display of a datatable in a grid display
INHERITED(DataTableViewSpec)
public:

  inline int		colSpecCount() const {return col_specs.size;}
  GridColViewSpec*	colSpec(int idx) const 
    {return (GridColViewSpec*)col_specs.SafeEl(idx);}
  
  virtual int 	UpdateLayout();
  // #MENU #MENU_ON_Actions enforce the geometry to fit with no spaces or overlap, returns maxx
  virtual void	GetMinMaxScale(MinMax& mm, bool first=true); // get min and max data range for scaling

  void 	Initialize();
  void	Destroy();
  void	Copy_(const GridTableViewSpec& cp);
  COPY_FUNS(GridTableViewSpec, DataTableViewSpec);
  TA_BASEFUNS(GridTableViewSpec);
protected:
  override void		ReBuildFromDataTable_impl();
  override void		Reset_impl();
  override void 	UpdateAfterEdit_impl();
};

#endif // datatable_h
