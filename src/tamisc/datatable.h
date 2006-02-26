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

#include "ta_group.h"
#include "ta_matrix.h"
#include "tdgeometry.h"
#include "minmax.h"
#include "aggregate.h"
#include "colorscale.h" // for DT && DA Viewspecs
#include "tamisc_TA_type.h"

#ifdef TA_GUI
#include "fontspec.h"
class DataTableModel;
#endif


// forwards this file

class String_Data; 
class float_Data;
class int_Data;
class byte_Data;

class ClustNode; //


/*obs
class TAMISC_API DataItem : public taOBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #INLINE source of a piece of data
public:

  String	name;		// #HIDDEN_INLINE name of data item
  String	disp_opts;	// #HIDDEN_INLINE default display options for item
  bool		is_string;	// #HIDDEN_INLINE is a string-valued item
  int		vec_n;		// #HIDDEN_INLINE length of vector (0 if not)

  bool 		SetName(const char* nm) 	{ name = nm; return true; }
  bool 		SetName(const String& nm) 	{ name = nm; return true; }
  String	GetName() const			{ return name; }

  // set options to encode type information
  virtual void	SetStringName(const char* nm);
  virtual void	SetNarrowName(const char* nm);
  virtual void	SetFloatVecNm(const char* nm, int n);
  virtual void	SetStringVecNm(const char* nm, int n);

  virtual void 	AddDispOption(const char* opt);
  // adds an option, checking that it is not already there first..
  bool		HasDispOption(const char* opt) const
  { return disp_opts.contains(opt); } // check if a given display option is set
  String 	DispOptionAfter(const char* opt);
  // returns portion of option after given opt fragment

  void	Initialize();
  void	Destroy()	{ };
  void	Copy_(const DataItem& cp);
  COPY_FUNS(DataItem, taOBase);
  TA_BASEFUNS(DataItem);
};


class TAMISC_API DataItem_List : public taList<DataItem> {
  // ##NO_TOKENS #NO_UPDATE_AFTER list of DataItem objects
public:
  void	Initialize() 		{SetBaseType(&TA_DataItem); };
  void 	Destroy()		{ };
  TA_BASEFUNS(DataItem_List);
};


class TAMISC_API LogData : public taBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER log data is communicated with these objects
#ifndef __MAKETA__
typedef taBase inherited;
#endif
public:
  DataItem_List	items;		// #LINK_GROUP data items for each column of log data
private:  int_Array	index;		// index of item either in r_data or s_data
  float_Array	r_data;		// float-valued data
  String_Array	s_data;		// string-valued data
public:
  int		indexSize() {return index.size;}
  virtual void	Reset();		// reset all data
  virtual void	AddFloat(DataItem* head, float val);	  // add a float data item
  virtual void	AddString(DataItem* head, char* val);
  // add a string data item (head code is set by this function)

  bool	IsString(int i)		// return true if ith item is a string
  { return ((DataItem*)items[i])->is_string; }
  bool	IsVec(int i)		// return true if ith item is a vector
  { return (((DataItem*)items[i])->vec_n > 0); }
  int	GetVecN(int i)		// return length of vector
  { return ((DataItem*)items[i])->vec_n; }

  float&   GetFloat(int i)	// get the ith data item (which better be a float)
  { return r_data[index[i]]; }
  String& GetString(int i)	// get the ith data item (which better be a str)
  { return s_data[index[i]]; }

  virtual void	InitBlankData();	// reset & add blank data items to match headers

  virtual bool	CompareItems(const LogData& cmp);
  // compare items pointers in log data's, true if same

  void	Initialize() {}
  void	Destroy()		{CutLinks(); }
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const LogData& cp);
  COPY_FUNS(LogData, taBase);
  TA_BASEFUNS(LogData);
};
*/

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

// specific ones are in the template classes: String_Data, float_Data

/*
  Display Options (subclasses add new ones -- see each class)

  HIDDEN -- forces !visible by default
  TEXT -- sets display_style to TEXT
  NARROW -- sets display_style to TEXT; also, makes it !visible by default

*/

class TAMISC_API DataArray_impl : public taNBase {
  // #VIRT_BASE ##NO_TOKENS #NO_INSTANCE holds a column of data;\n (a scalar cell can generally be treated as a degenerate matrix cell of dim[1])
INHERITED(taNBase)
friend class DataTable;
public:
  enum ValType {
    VT_STRING,
    VT_FLOAT,
    VT_INT,
    VT_BYTE
  };

  static void 		DecodeName(String nm, String& base_nm, int& vt, int& vec_col, int& col_cnt);
    // note: vt is -1 if unknown
  static String 	ValTypeToStr(ValType vt);


  String		disp_opts;	// viewer default display options
  bool			save_to_file;	// save this data to a file (e.g., to a log file in PDP++)?
  bool			is_matrix; // #READ_ONLY #SAVE #SHOW 'true' if the cell is a matrix, not a scalar
  MatrixGeom		cell_geom; //  #READ_ONLY #SAVE #SHOW for matrix cols, the geom of each cell
  
  virtual taMatrix* 	AR() = 0; // the matrix pointer
  virtual const taMatrix* 	AR() const = 0; // const version of the matrix pointer
  virtual bool		is_numeric() const {return false;} // true if data is float, int, or byte
  virtual bool		is_string() const {return false;}// true if data is string
  virtual int		cell_size() const // for matrix type, number of elements in each cell
     {return (is_matrix) ? cell_geom.Product() : 1;}
  virtual int		cell_dims() const // for matrix type, number of dimensions in each cell
     {return cell_geom.size;}
  virtual int		GetCellGeom(int dim) const  // for matrix type, size of given dim
    {return cell_geom.SafeEl(dim);}
  int			rows() const {return AR()->frames();}

  int			displayWidth() const; // low level display width, in tabs (8 chars/tab), taken from spec
  virtual int		maxColWidth() const {return -1;} // aprox max number of columns, in characters, -1 if variable or unknown
  virtual ValType 	valType() const = 0; // the type of data in each element

  override String 	GetColText(int col, int itm_idx = -1);
  override String	GetDisplayName() const; // #IGNORE we strip out the format characters
  
  // for accessor routines, row is absolute row in the matrix, not a DataTable row -- use the DataTable routines
  // -ve values are from end, and are valid for both low-level col access, and DataTable access
  Variant 	GetValAsVar(int row) const {return GetValAsVar_impl(row, 0);}
    // valid for all types, -ve row is from end (-1=last)
  String 	GetValAsString(int row) const {return GetValAsString_impl(row, 0);}
    // valid for all types, -ve row is from end (-1=last)
  float 	GetValAsFloat(int row) const {return GetValAsFloat_impl(row, 0);} 
    // valid if type is numeric, -ve row is from end (-1=last)
  int	 	GetValAsInt(int row) const {return GetValAsInt_impl(row, 0);} 
    // valid if type is int or byte, -ve row is from end (-1=last)
  byte	 	GetValAsByte(int row) const {return GetValAsByte_impl(row, 0);} 
    // valid only if type is byte, -ve row is from end (-1=last)
    
  // Matrix versions
  Variant 	GetValAsVarM(int row, int cell) const {return GetValAsVar_impl(row, cell);} 
    // valid for all types, -ve row is from end (-1=last)
  String 	GetValAsStringM(int row, int cell) const {return GetValAsString_impl(row, cell);} 
    // valid for all types, -ve row is from end (-1=last)
  float 	GetValAsFloatM(int row, int cell) const {return GetValAsFloat_impl(row, cell);} 
    // valid if type is numeric, -ve row is from end (-1=last)
  int	 	GetValAsIntM(int row, int cell) const {return GetValAsInt_impl(row, cell);} 
    // valid if type is int or byte, -ve row is from end (-1=last)
  byte	 	GetValAsByteM(int row, int cell) const {return GetValAsByte_impl(row, cell);} 
    // valid only if type is byte, -ve row is from end (-1=last)

  bool	 	SetValAsVar(const Variant& val, int row) 
    {return SetValAsVar_impl(val, row, 0);} 
    // valid for all types, -ve row is from end (-1=last)
  bool	 	SetValAsString(const String& val, int row) 
    {return SetValAsString_impl(val, row, 0);} 
    // valid for all types, -ve row is from end (-1=last)
  bool	 	SetValAsFloat(float val, int row) 
    // valid only if type is float, -ve row is from end (-1=last)
    {return SetValAsFloat_impl(val, row, 0);} 
  bool	 	SetValAsInt(int val, int row) 
    // valid if type is int or float, -ve row is from end (-1=last)
    {return SetValAsInt_impl(val, row, 0);} 
  bool	 	SetValAsByte(byte val, int row) 
    // valid if type is numeric, -ve row is from end (-1=last)
    {return SetValAsByte_impl(val, row, 0);} 
    
  // Matrix versions
  bool	 	SetValAsVarM(const Variant& val, int row, int cell) 
    {return SetValAsVar_impl(val, row, cell);} 
    // valid for all types, -ve row is from end (-1=last)
  bool	 	SetValAsStringM(const String& val, int row, int cell) 
    {return SetValAsString_impl(val, row, cell);} 
    // valid for all types, -ve row is from end (-1=last)
  bool	 	SetValAsFloatM(float val, int row, int cell) 
    // valid only if type is float, -ve row is from end (-1=last)
    {return SetValAsFloat_impl(val, row, cell);} 
  bool	 	SetValAsIntM(int val, int row, int cell) 
    // valid if type is int or float, -ve row is from end (-1=last)
    {return SetValAsInt_impl(val, row, cell);} 
  bool	 	SetValAsByteM(byte val, int row, int cell) 
    // valid if type is numeric, -ve row is from end (-1=last)
    {return SetValAsByte_impl(val, row, cell);} 

  bool		HasDispOption(const char* opt) const
  { return disp_opts.contains(opt); } // check if a given display option is set
  String 	DispOptionAfter(const char* opt) const;
  void		AddDispOption(const char* opt);

  virtual void Init(); // call this *after* creation, or in UAE, to assert matrix geometry
  void  UpdateAfterEdit();
  void	InitLinks(); //note: ok to do own AR here, because never called in constructor
  void	CutLinks(); //note: NOT ok to do disown AR here, because called in destructor
  void 	Copy_(const DataArray_impl& cp);
  COPY_FUNS(DataArray_impl, taNBase);
  TA_ABSTRACT_BASEFUNS(DataArray_impl);
  
protected:
  // in all accessor routines, -ve row is from end (-1=last)
  int			IndexOfEl_Flat(int row, int cell) const; 
    // -ve row is from end (-1=last); note: returns -ve value if out of range, so must use with SafeEl_Flat
  virtual Variant 	GetValAsVar_impl(int row, int cell) const; 
  virtual String 	GetValAsString_impl(int row, int cell) const; 
  virtual float 	GetValAsFloat_impl(int row, int cell) const 
    {return (float)GetValAsInt_impl(row, cell);}
  virtual int	 	GetValAsInt_impl(int row, int cell) const 
    {return (int)GetValAsByte_impl(row, cell);}
  virtual byte	 	GetValAsByte_impl(int row, int cell) const {return 0;} 
  
  virtual bool	 	SetValAsVar_impl(const Variant& val, int row, int cell); // true if set 
  virtual bool	 	SetValAsString_impl(const String& val, int row, int cell);  // true if set 
  virtual bool	 	SetValAsFloat_impl(float val, int row, int cell) {return false;}  // true if set 
  virtual bool	 	SetValAsInt_impl(int val, int row, int cell)  // true if set 
    {return SetValAsFloat_impl((float)val, row, cell);} 
  virtual bool	 	SetValAsByte_impl(byte val, int row, int cell)  // true if set 
    {return SetValAsInt_impl((int)val, row, cell);} 
  
private:
  void	Initialize();
  void	Destroy()	{CutLinks(); }; //
};

class TAMISC_API ColDescriptor: public taNBase { // describes a column of data in a DataTable
INHERITED(taNBase)
public:
  int			col_num; // #SHOW #READ_ONLY #NO_SAVE the column number (-1=at end)
  DataArray_impl::ValType val_type; // the type of data each cell will contain
  String		disp_opts;	// viewer display options
  bool			save_to_file;	// whether to save the column to a file
  bool			is_matrix; // 'true' if the cell is a matrix, not a scalar
  MatrixGeom		cell_geom; //  #INLINE #CONDEDIT_ON_is_matrix:true for matrix cols, the geom of each cell
  
  virtual void		CopyFromDataArray(const DataArray_impl& cp);
  
  override String	GetColText(int col, int itm_idx = -1); 
  void	InitLinks();
  void	CutLinks();
  void 	Copy_(const ColDescriptor& cp);
  COPY_FUNS(ColDescriptor, taNBase);
  TA_BASEFUNS(ColDescriptor);
private:
  void		Initialize();
  void		Destroy() {CutLinks();}
};


class TAMISC_API ColDescriptor_List: public taList<ColDescriptor> {
INHERITED(taList<ColDescriptor>)
public:
  
  TA_BASEFUNS(ColDescriptor_List);
  
public:
  override void		El_SetIndex_(void* it, int idx);
  override int		NumListCols() const {return 7;} // number of columns in a list view for this item type
  override String	GetColHeading(int col); // header text for the indicated column
  
private:
  void		Initialize() {SetBaseType(&TA_ColDescriptor);}
  void		Destroy() {}
};



/*
  DataTable Notifications

  Structural Changes -- DCR_STRUCT_UPDATE_BEGIN..DCR_STRUCT_UPDATE_END
    this includes adding and removing data columns
  Row Adding/Removing -- DCR_DATA_UPDATE_BEGIN..DCR_DATA_UPDATE_END
    this includes adding and removing whole rows, or individual items to a row -- if the client
    code calls RowAdding/RowAdded

  Row Numbers
    - all fuctions using row numbers work properly for jagged tables, i.e. those to which
      columns have been added after some rows already exist
    - NOTE: functions with row numbers did NOT have this correct behavior in v3.2
*/
class TAMISC_API DataTable : public taGroup<DataArray_impl> {
  // #NO_UPDATE_AFTER table of data
INHERITED(taGroup<DataArray_impl>)
public:
//obs  static void 	SetFieldData(LogData& ld, int ldi, DataItem* ditem, DataTable* dat, int idx);
//obs  static void 	SetFieldHead(DataItem* ditem, DataTable* dat, int idx);

  int 		rows; // #READ_ONLY #NO_SAVE #SHOW NOTE: this is only valid for top-level DataTable, not its subgroups
  bool		save_data; // 'true' if data should be saved in project; typically false for logs, true for data patterns
  
  bool		hasData(int row, int col, int subgp=-1); // true if data at that cell
  bool		idx(int row_num, int col_size, int& act_idx)
    {act_idx = col_size - (rows - row_num); return act_idx >= 0;} // calculates an actual index for a col item, based on the current #rows and size of that col; returns 'true' if act_idx >= 0 (i.e., if there is a data item for that column)
  
  override void	Reset();
  virtual void	ResetData();
  // #MENU #MENU_ON_Actions deletes all the data, but keeps the column structure
  virtual void	RemoveRow(int row_num);
  // #MENU Remove an entire row of data
//TODO if needed:  virtual void	ShiftUp(int num_rows);
  // remove indicated number of rows of data at front (typically used by Log to make more room in buffer)
  virtual void	AddBlankRow();
  // #MENU add a new row to the data table, returns new row number
  virtual void	AllocRows(int n); // allocate space for at least n rows

  virtual void	SetSaveToFile(bool save_to_file);
  // #MENU set the save_to_file flag for entire group of data elements

  virtual void	AddRowToArray(float_RArray& ar, int row_num) const;
  // add a row of the datatable to given array
  virtual void	AggRowToArray(float_RArray& ar, int row_num, Aggregate& agg) const;
  // aggregate a row of the datatable to given array using parameters in agg
  virtual float	AggRowToVal(int row_num, Aggregate& agg) const;
  // aggregate a row of the datatable to a value using parameters in agg
  virtual void	AddArrayToRow(float_RArray& ar);
  // add contents of array to datatable
  virtual void	AggArrayToRow(const float_RArray& ar, int row_num, Aggregate& agg);
  // aggregate contents of array to datatable at given row
  virtual void	PutArrayToRow(const float_RArray& ar, int row_num);
  // just put array values into given row of data
  virtual void	UpdateAllRanges();
  // update all min-max range data for all float_Data elements in log

  DataArray_impl*	NewCol(DataArray_impl::ValType val_type, const char* col_nm);
   // #MENU #MENU_ON_Table create new scalar column of data of specified type
  DataArray_impl*	NewColMatrix(DataArray_impl::ValType val_type, const char* col_nm,
    int dims = 1, int d0 = 0, int d1 = 0, int d2 = 0, int d3 = 0);
   // #MENU #MENU_ON_Table create new matrix column of data of specified type, with specified cell geom
  DataArray_impl*	NewColMatrixGeom(DataArray_impl::ValType val_type, const char* col_nm,
    const MatrixGeom& cell_geom);
   // create new matrix column of data of specified type, with specified cell geom
  virtual float_Data*	NewColFloat(const char* col_nm); // create new column of floating point data
  virtual int_Data*	NewColInt(const char* col_nm); 	 // create new column of integer-level data (= narrow display, actually stored as float)
  virtual String_Data*	NewColString(const char* col_nm); // create new column of string data
  virtual DataTable*	NewGroupFloat(const char* base_nm, int n); // create new sub-group of floats of size n, named as base_nm_index
  virtual DataTable*	NewGroupInt(const char* base_nm, int n); // create new sub-group of ints of size n, named as base_nm_index
  virtual DataTable*	NewGroupString(const char* base_nm, int n); // create new sub-group of strings of size n, named as base_nm_index

  virtual DataArray_impl* GetColData(int col, int subgp=-1);
  // get data for given column (if subgp >= 0, column is in given subgroup)

  virtual float_Data* GetColFloatData(int col, int subgp=-1);
  // get float_Data for given column (if subgp >= 0, column is in given subgroup)
  virtual String_Data* GetColStringData(int col, int subgp=-1);
  // get string data for given column (if subgp >= 0, column is in given subgroup)

  virtual float_Matrix* GetColFloatArray(int col, int subgp=-1);
  // get float_RArray for given column (if subgp >= 0, column is in given subgroup)
  virtual String_Matrix* GetColStringArray(int col, int subgp=-1);
  // get string data for given column (if subgp >= 0, column is in given subgroup)

  virtual void	PutArrayToCol(const float_RArray& ar, int col, int subgp=-1);
  // just put array values into given column (if subgp >= 0, column is in given subgroup)

  virtual void	SetColName(const char* col_nm, int col, int subgp=-1);
  // set column name for given column (if subgp >= 0, column is in given subgroup)
  virtual void	AddColDispOpt(const char* dsp_opt, int col, int subgp=-1);
  // add display option for given column (if subgp >= 0, column is in given subgroup)
  
  virtual void	SetFloatVal(float val, int col, int row, int subgp=-1);
  // set float/int data for given column, row (if subgp >= 0, column is in given subgroup)
  virtual void	SetStringVal(const char* val, int col, int row, int subgp=-1);
  // set string data for given column, row (if subgp >= 0, column is in given subgroup)

  virtual void	SetLastFloatVal(float val, int col, int subgp=-1);
  // set last row float/int data for given column (if subgp >= 0, column is in given subgroup)
  virtual void	SetLastStringVal(const char* val, int col, int subgp=-1);
  // set last row string data for given column (if subgp >= 0, column is in given subgroup)

  virtual float GetFloatVal(int col, int row, int subgp=-1);
  // get float data for given column, row (if subgp >= 0, column is in given subgroup)
  virtual String GetStringVal(int col, int row, int subgp=-1);
  // get string data for given column, row (if subgp >= 0, column is in given subgroup)
  
  String 	GetValAsString(int col, int row, int subgp=-1);
  // get data of any type, in String form, for given column, row (if subgp >= 0, column is in given subgroup); if data is NULL, then "n/a" is returned
  void 		SetValAsString(const String& val, int col, int row, int subgp=-1);
  // set data of any type, in String form, for given column, row (if subgp >= 0, column is in given subgroup); does nothing if no cell

  Variant 	GetValAsVar(int col, int row, int subgp=-1);
  // get data of any type, in Variant form, for given column, row (if subgp >= 0, column is in given subgroup); Invalid/NULL if no cell
  bool 		SetValAsVar(const Variant& val, int col, int row, int subgp=-1);
  // set data of any type, in String form, for given column, row (if subgp >= 0, column is in given subgroup); does nothing if no cell

  // dumping and loading -- see .cpp file for detailed format information, not saved as standard taBase obj
  virtual void 	SaveHeader(ostream& strm); // saves header information, tab-separated, 
  virtual void 	SaveData(ostream& strm); // saves data, one line per rec, tab-separated
  virtual void 	LoadHeader(istream& strm); // loads header information -- preserves current headers if possible
  virtual void 	LoadData(istream& strm, int max_recs = -1); // loads data, up to max num of recs (-1 for all)
  
  int  		MinLength();		// #IGNORE
  int  		MaxLength();		// #IGNORE

  override int		NumListCols() const {return 3;} // Name, data type (float, etc.), disp options
  override String	GetColHeading(int col); // header text for the indicated column

#ifdef TA_GUI
  QAbstractItemModel*	GetDataModel(); // returns new if none exists, or existing -- enables views to be shared
#endif

  void	Initialize();
  void	Destroy();
  void 	Copy_(const DataTable& cp);
  COPY_FUNS(DataTable, taGroup<DataArray_impl>);
  TA_BASEFUNS(DataTable); //

public: // DO NOT USE THE FOLLOWING IN NEW CODE -- preferred method is to add a new row, then set values
  virtual void	RowAdding(); // indicate beginning of column-at-a-time data adding NOT NESTABLE
  virtual void	RowAdded(); // indicates end of column at-a-time adding (triggers row added notification) -- this routine also gets called by the AddRow and similar functions (w/o calling RowAdding)
  void	AddFloatVal_deprecated(float val, int col, int subgp=-1) {AddFloatVal(val, col, subgp);} // #IGNORE
  void	AddStringVal_deprecated(const char* val, int col, int subgp=-1) {AddStringVal(val, col, subgp);} // #IGNORE

protected:
#ifdef TA_GUI
  DataTableModel*	m_dtm; // #IGNORE note: once we create, always exists
#endif
  virtual DataArray_impl*	NewCol_impl(DataArray_impl::ValType val_type, const char* col_nm);
   // low-level create routine, shared by scalar and matrix creation, must be wrapped in StructUpdate
private:
  virtual void	AddFloatVal(float val, int col, int subgp=-1);
  // add float/int data for given column (if subgp >= 0, column is in given subgroup)
  virtual void	AddStringVal(const char* val, int col, int subgp=-1);
  // add string data for given column (if subgp >= 0, column is in given subgroup)

};

template<class T> 
class TAMISC_API DataArray : public DataArray_impl { // #VIRT_BASE #NO_INSTANCE template for common elements
public:
  override taMatrix* 	AR()	{ return &ar; } // the array pointer
  override const taMatrix* AR() const { return &ar; } // the array pointer

  void	CutLinks()
    {ar.CutLinks(); DataArray_impl::CutLinks();}
  void	Copy_(const DataArray<T>& cp)  {ar = cp.ar;}
  COPY_FUNS(DataArray<T>, DataArray_impl);
  TA_ABSTRACT_TMPLT_BASEFUNS(DataArray, T); //
public: //DO NOT ACCESS DIRECTLY
  T		ar;		// #NO_SAVE #SHOW #BROWSE the array itself
private:
  void	Initialize()		{}
  void	Destroy()		{ CutLinks(); }
};

class TAMISC_API String_Data : public DataArray<String_Matrix> {
  // string data
INHERITED(DataArray<String_Matrix>)
friend class DataTable;
public:
  override bool		is_string() const {return true;} 
  override ValType 	valType() const  {return VT_STRING;}

  TA_BASEFUNS(String_Data);

private:
  void	Initialize() {}
  void	Destroy() {}
};


class TAMISC_API float_Data : public DataArray<float_Matrix> {
  // floating point data
INHERITED(DataArray<float_Matrix>)
friend class DataTable;
public:
  override bool		is_numeric() const {return true;} 
  override int		maxColWidth() const {return 7;} // assumes sign, int: 6 dig's; float: 5 dig's, decimal point
  override ValType 	valType() const {return VT_FLOAT;}

  TA_BASEFUNS(float_Data);
  
protected:
  override float 	GetValAsFloat_impl(int row, int cell) const
    {return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell));}
  override bool	 	SetValAsFloat_impl(float val, int row, int cell)
    {ar.Set_Flat(IndexOfEl_Flat(row, cell), val); return true;}

private:
  void	Initialize() {}
  void	Destroy() {}
};

class TAMISC_API int_Data : public DataArray<int_Matrix> {
  // int data
INHERITED(DataArray<int_Matrix>)
friend class DataTable;
public:
  override bool		is_numeric() const {return true;} // 
  override int		maxColWidth() const {return 11;} // assumes sign, 10 digs
  override ValType 	valType() const {return VT_INT;}

  TA_BASEFUNS(int_Data);
  
protected:
  override int 		GetValAsInt_impl(int row, int cell) const
    {return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell));}
  override bool	 	SetValAsInt_impl(int val, int row, int cell)
    {ar.Set_Flat(IndexOfEl_Flat(row, cell), val); return true;}

private:
  void	Initialize() {}
  void	Destroy() {}
};

class TAMISC_API byte_Data : public DataArray<byte_Matrix> {
  // byte data
INHERITED(DataArray<byte_Matrix>)
friend class DataTable;
public:
  override bool		is_numeric() const {return true;} // 
  override int		maxColWidth() const {return 3;} // assumes 3 digs
  override ValType 	valType() const {return VT_BYTE;}

  TA_BASEFUNS(byte_Data);
  
protected:
  override byte 	GetValAsByte_impl(int row, int cell) const
    {return ar.SafeEl_Flat(IndexOfEl_Flat(row, cell));}
  override bool	 	SetValAsByte_impl(byte val, int row, int cell)
    {ar.Set_Flat(IndexOfEl_Flat(row, cell), val); return true;}

private:
  void	Initialize() {}
  void	Destroy() {}
};



class TAMISC_API ClustLink : public taBase {
  // #INLINE ##NO_TOKENS ##NO_UPDATE_AFTER a link in the cluster tree with distance
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
  /* ##INLINE ##NO_TOKENS #NO_UPDATE_AFTER node in clustering algorithm
     use one with leaves as children as a root node for cluster */
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

  virtual void	XGraph(const char* fnm, const char* title);
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

class TAMISC_API DA_ViewSpec : public taNBase {
  // ##SCOPE_DT_ViewSpec base specification for the display of log data_array (DA)
public:
  DataArray_impl*	data_array;	// #READ_ONLY #NO_SAVE the data array
  String		display_name;	// name used in display
  bool	        	visible;	// visibility flag

  virtual void	UpdateView(); 	// #BUTTON Update view to reflect current changes
  virtual void	SetGpVisibility(bool visible);
  // #BUTTON set the visibility of all members of this group of items
  virtual void	CopyToGp(MemberDef* member);
  // #BUTTON copy given member value setting to all view specs within this same group

  virtual bool	BuildFromDataArray(DataArray_impl* tda=NULL);

  virtual String	ValAsString(int row); //note: row should be valid
  static String	CleanName(String& name);

  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
  void	CutLinks();
  void	Copy_(const DA_ViewSpec& cp);
  COPY_FUNS(DA_ViewSpec,taNBase);
  TA_BASEFUNS(DA_ViewSpec);
};

class TAMISC_API DT_ViewSpec :  public taGroup<DA_ViewSpec> {
  // base specification for the display of log data_table (DT)
#ifndef __MAKETA__
typedef taGroup<DA_ViewSpec> inherited;
#endif
public:
  DataTable*		data_table;	// #READ_ONLY the data table;
  String		display_name;	// name used in display
  bool	        	visible; 	// visibility flag
#ifdef TA_GUI
  FontSpec		def_font; // default font/size for text
#endif
  virtual bool	BuildFromDataTable(DataTable* tdt=NULL);
  virtual void	ReBuildFromDataTable();

  virtual void	SetDispNms(const char* base_name);
  // #BUTTON set display_name for all view specs in group to base_name + "_" + no where no is number in group
  virtual void	RmvNmPrefix();
  // #BUTTON set display_name for all view specs in group to current name without prefix (i.e., "prefix_rest" -> "rest")
  virtual void	SetVisibility(bool visible);
  // #BUTTON set the visibility of all members of this group of items

  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy();
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const DT_ViewSpec& cp);
  COPY_FUNS(DT_ViewSpec, taGroup<DA_ViewSpec>);
  TA_BASEFUNS(DT_ViewSpec);
};

/*
  Additional Display Options
    WIDTH=i (i: int) -- sets default column width to i tabs (8 chars/ tab)
    NARROW -- in addition to base spec, also sets column with to 1 tab

*/
class TAMISC_API DA_TextViewSpec: public DA_ViewSpec {
  // data-array view spec for text-based display
public:
  int		width;			// width of the column, in tabs (8 chars/tab)

  bool		BuildFromDataArray(DataArray_impl* tda=NULL);

  void 	Initialize();
  void	Destroy();
  void Copy_(const DA_TextViewSpec& cp) { width = cp.width;}
  COPY_FUNS(DA_TextViewSpec,DA_ViewSpec);
  TA_BASEFUNS(DA_TextViewSpec);
};

class TAMISC_API DA_NetViewSpec: public DA_ViewSpec {
  // data-array view spec for netview based display
public:
  int	label_index;	// index into the netview's label list
  void  Initialize();
  void  Destroy();
  void Copy_(const DA_NetViewSpec& cp) { label_index = cp.label_index;}
  COPY_FUNS(DA_NetViewSpec,DA_ViewSpec);
  TA_BASEFUNS(DA_NetViewSpec);
};

class TAMISC_API DA_GridViewSpec : public DA_ViewSpec {
  // information for display of a data array in a grid display
public:
  enum DisplayStyle {
    TEXT,			// Draw using text only
    BLOCK,			// Draw using color block only
    TEXT_AND_BLOCK 		// Draw using both color block with text
  };

  PosTDCoord	pos;		// position of the data in absolute coordinates
  DisplayStyle  display_style;	// can display as text, block, or both
  bool		scale_on;	// adjust overall scale including this data

  bool		BuildFromDataArray(DataArray_impl* tda=NULL);

  void 	Initialize();
  void	Destroy();
  void 	InitLinks();
  void	Copy_(const DA_GridViewSpec& cp);
  COPY_FUNS(DA_GridViewSpec, DA_ViewSpec);
  TA_BASEFUNS(DA_GridViewSpec);
};

class TAMISC_API DT_GridViewSpec : public DT_ViewSpec {
  // information for display of a datatable in a grid display
public:
  enum BlockFill {		// ways that grid blocks can be filled
    COLOR,			// color indicates value
    AREA,			// area indicates value
    LINEAR 			// linear size of square side indicates value
  };

  enum MatrixLayout { // order of display of the grid elements
    DEFAULT,			// use current default layout
    LFT_RGT_BOT_TOP, // [3412] Incr col first, then decr row, start at bot left
    LFT_RGT_TOP_BOT, // [1234] Incr col first, then incr row, start at top left
    BOT_TOP_LFT_RGT, // [2413] Decr row first, then incr col, start at bot left
    TOP_BOT_LFT_RGT  // [1324] Incr row first, then incr col, start at top left
  };

  PosTDCoord	pos;		// position of the datatable in absolute coordinates
  PosTDCoord	geom;		// relative geometry (maximum extent) of the datatable, just for El's, not subgroups
  PosTDCoord	full_geom;	// #HIDDEN #NO_SAVE full absolute geometry (maximum extent) of everything under this one
  MatrixLayout	layout;		// current layout of the data table

  bool          use_gp_name;    // use the group name instead of the El names
  DA_GridViewSpec::DisplayStyle  display_style;	// can display as text, block, or both
  bool		scale_on;	// adjust overall scale including this data (or not)
  bool		customized;	// #READ_ONLY did the use customize the positions of elements in here?  if so, don't redo layout with new items

  virtual int 	UpdateLayout(MatrixLayout ml=DEFAULT);
  // #MENU #MENU_ON_Actions enforce the geometry to fit with no spaces or overlap, returns maxx
  virtual void 	UpdateGeom();
  // #MENU Get the Geometry from the positions of visibles

  bool		BuildFromDataTable(DataTable* tdt=NULL);
  virtual void	GetMinMaxScale(MinMax& mm, bool first=true); // get min and max data range for scaling

  void		Reset();

  void 	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy();
  void 	InitLinks();
  void	Copy_(const DT_GridViewSpec& cp);
  COPY_FUNS(DT_GridViewSpec, DT_ViewSpec);
  TA_BASEFUNS(DT_GridViewSpec);
};

#endif // datatable_h
