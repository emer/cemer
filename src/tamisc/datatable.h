/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

#ifndef datatable_h
#define datatable_h

#include "ta_group.h"
#include "tdgeometry.h"
#include "minmax.h"
#include "aggregate.h"
#include "colorscale.h" // for DT && DA Viewspecs
#include "tamisc_TA_type.h"

#ifdef TA_GUI
#include "fontspec.h"
#endif


// forwards this file

class float_Data;
class String_Data; //
class ClustNode;



class DataItem : public taOBase {
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


class DataItem_List : public taList<DataItem> {
  // ##NO_TOKENS #NO_UPDATE_AFTER list of DataItem objects
public:
  void	Initialize() 		{SetBaseType(&TA_DataItem); };
  void 	Destroy()		{ };
  TA_BASEFUNS(DataItem_List);
};


class LogData : public taBase {
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


class float_RArray : public float_Array {
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

  float		Pop();
  bool		Remove(const float& item);
  bool		Remove(uint indx, int n_els=1);
  void 		CopyVals(const taArray_impl& from, int start=0, int end=-1, int at=0);

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

  inline float& SafeMatEl(int col_dim, int row, int col) const { return SafeEl((row * col_dim) + col); }
  // safe get element assuming a matrix layout of values with column (inner) dimension size = col_dim
  inline float& FastMatEl(int col_dim, int row, int col) const { return FastEl((row * col_dim) + col); }
  // fast get element assuming a matrix layout of values with column (inner) dimension size = col_dim
  inline float& FastMatEl1(int col_dim, int row, int col) const { return FastEl(((row-1) * col_dim) + (col-1)); }
  // fast get element assuming a matrix layout of values with column (inner) dimension size = col_dim, indicies use 1-n range instead of 0-n-1!
  inline float& FastEl1(int idx) const { return FastEl(idx-1); }
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
  inline float& SafeTriMatEl(int dim, int x, int y) const {
    if(x < y)  	return SafeEl((y-x) + (x * dim) - (((x-1) * x) / 2));
    else	return SafeEl((x-y) + (y * dim) - (((y-1) * y) / 2));
  }
  // get element assuming an upper-triangular symmetric matrix (e.g., distance matrix) of dimension dim for two items, x, y
  inline float& FastTriMatEl(int dim, int x, int y) const {
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

  // updtate range after
  void		ShiftLeft(int nshft);
  int		Dump_Load_Value(istream& strm, TAPtr par=NULL);

  void		Reset(){float_Array::Reset();range.Init(0.0f);}

  void	Initialize()		{ };
  void	Destroy()		{ };
  void	InitLinks();
  void 	Copy_(const float_RArray& cp);
  COPY_FUNS(float_RArray, float_Array);
  TA_BASEFUNS(float_RArray);
};


// specific ones are in the template classes: String_Data, float_Data

/*
  Display Options (subclasses add new ones -- see each class)

  HIDDEN -- forces !visible by default
  TEXT -- sets display_style to TEXT
  NARROW -- sets display_style to TEXT; also, makes it !visible by default

*/

class DataArray_impl : public taNBase {
  // #VIRT_BASE ##NO_TOKENS #NO_UPDATE_AFTER holds array data
#ifndef __MAKETA__
typedef taNBase inherited;
#endif
public:
  enum ValType {
    VT_UNKNOWN,
    VT_STRING,
    VT_FLOAT,
    VT_INT
  };

  static void 		DecodeName(String nm, String& base_nm, ValType& vt, int& vec_col, int& col_cnt);
  static String 	ValTypeToStr(ValType vt);


  String		disp_opts;	// viewer default display options
  bool			save_to_file;	// save this data to a file (e.g., to a log file in PDP++)?

  virtual bool	is_float() {return false;} // these tests are done all the time
  virtual bool	is_string() {return false;}// these tests are done all the time

  int		displayWidth(); // low level display width, in tabs (8 chars/tab), taken from spec
  virtual int	maxColWidth() {return 0;} // aprox max number of columns, in characters
  // these are overloaded for the specific types

  override String 	GetColText(int col, int itm_idx = -1);
  override String	GetDisplayName() const; // #IGNORE we strip out the format characters
  virtual ValType 	valType()  {return VT_STRING;} // overridden in numeric subclasses
  virtual String 	GetValAsString(int row) {return _nilString;} // overridden in subclasses
  virtual float 	GetValAsFloat(int row) {return 0.0f;} // overridden in subclasses
  virtual int	 	GetValAsInt(int row) {return 0;} // overridden in subclasses

  virtual taArray_base* 	AR()	{ return NULL; } // the array pointer
/*  virtual void	NewAR() { };
  // #MENU #MENU_ON_Object create an array for yourself
  virtual void	SetAR(taArray_base*) { };	// set AR to existing array
*/
  bool		HasDispOption(const char* opt) const
  { return disp_opts.contains(opt); } // check if a given display option is set
  String 	DispOptionAfter(const char* opt);
  void		AddDispOption(const char* opt);

  void	Initialize();
  void	Destroy()	{ };
  void 	Copy_(const DataArray_impl& cp);
  COPY_FUNS(DataArray_impl, taNBase);
  TA_ABSTRACT_BASEFUNS(DataArray_impl);
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
class DataTable : public taGroup<DataArray_impl> {
  // #NO_UPDATE_AFTER table of data
#ifndef __MAKETA__
typedef taGroup<DataArray_impl> inherited;
#endif
public:
  static void 	SetFieldData(LogData& ld, int ldi, DataItem* ditem, DataTable* dat, int idx);
  static void 	SetFieldHead(DataItem* ditem, DataTable* dat, int idx);

  int 		rows; // #READ_ONLY #NO_SAVE #SHOW NOTE: this is only valid for top-level DataTable, not its subgroups

  bool		idx(int row_num, int col_size, int& act_idx)
    {act_idx = col_size - (rows - row_num); return act_idx >= 0;} // calculates an actual index for a col item, based on the current #rows and size of that col; returns 'true' if act_idx >= 0 (i.e., if there is a data item for that column)
  override void	Reset();
  virtual void	ResetData();
  // #MENU #MENU_ON_Actions deletes all the data, but keeps the column structure
  virtual void	RemoveRow(int row_num);
  // #MENU Remove an entire row of data
  virtual void	ShiftUp(int num_rows);
  // remove indicated number of rows of data at front (typically used by Log to make more room in buffer)
  virtual void	AddRow(LogData& ld); // add a row from the given log data
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
   // create new column of data of indicated type
  virtual float_Data*	NewColFloat(const char* col_nm); // create new column of floating point data
  virtual float_Data*	NewColInt(const char* col_nm); 	 // create new column of integer-level data (= narrow display, actually stored as float)
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

  virtual float_RArray* GetColFloatArray(int col, int subgp=-1);
  // get float_RArray for given column (if subgp >= 0, column is in given subgroup)
  virtual String_Array* GetColStringArray(int col, int subgp=-1);
  // get string data for given column (if subgp >= 0, column is in given subgroup)

  virtual void	PutArrayToCol(const float_RArray& ar, int col, int subgp=-1);
  // just put array values into given column (if subgp >= 0, column is in given subgroup)

  virtual void 	SetCols(LogData& ld);
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

  int  		MinLength();		// #IGNORE
  int  		MaxLength();		// #IGNORE

  override int		NumListCols() const {return 3;} // Name, data type (float, etc.), disp options
  override String	GetColHeading(int col); // header text for the indicated column

  void	Initialize();
  void	Destroy();
  TA_BASEFUNS(DataTable); //

public: // DO NOT USE THE FOLLOWING IN NEW CODE -- preferred method is to add a new row, then set values
  virtual void	RowAdding(); // indicate beginning of column-at-a-time data adding NOT NESTABLE
  virtual void	RowAdded(); // indicates end of column at-a-time adding (triggers row added notification) -- this routine also gets called by the AddRow and similar functions (w/o calling RowAdding)
  void	AddFloatVal_deprecated(float val, int col, int subgp=-1) {AddFloatVal(val, col, subgp);} // #IGNORE
  void	AddStringVal_deprecated(const char* val, int col, int subgp=-1) {AddStringVal(val, col, subgp);} // #IGNORE

private:
  virtual void	AddFloatVal(float val, int col, int subgp=-1);
  // add float/int data for given column (if subgp >= 0, column is in given subgroup)
  virtual void	AddStringVal(const char* val, int col, int subgp=-1);
  // add string data for given column (if subgp >= 0, column is in given subgroup)

};

template<class T> class DataArray : public DataArray_impl {
public:
  override taArray_base* 	AR()	{ return &ar; } // the array pointer

  void	Initialize()		{}
  void	Destroy()		{ CutLinks(); }
  void	InitLinks()
    {DataArray_impl::InitLinks(); taBase::Own(ar, this); }
  void	CutLinks()
    {ar.CutLinks(); DataArray_impl::CutLinks();}
  void	Copy_(const DataArray<T>& cp)  {ar = cp.ar;}
  COPY_FUNS(DataArray<T>, DataArray_impl);
  TA_TMPLT_BASEFUNS(DataArray, T); //
public: //DO NOT ACCESS DIRECTLY
  T		ar;		// #NO_SAVE #SHOW #BROWSE the array itself

};

class float_Data : public DataArray<float_RArray> {
  // #NO_UPDATE_AFTER floating point data
friend class DataTable;
public:
  override bool	is_float() {return true;} // these tests are done all the time
  override int	maxColWidth() {return 7;} // assumes sign, int: 6 dig's; float: 5 dig's, decimal point

  override float 	GetValAsFloat(int row);
  override String 	GetValAsString(int row);
  virtual ValType 	valType();

  void	Initialize();
  void	Destroy()		{ };
  TA_BASEFUNS(float_Data);
protected:
  ValType		m_valType; //note: set when accessed type
};

class DString_Array : public String_Array {
#ifndef __MAKETA__
typedef String_Array inherited;
#endif
friend class String_Data;
public:
  override void		Add_(void* it); // #IGNORE
  override void		Reset();
  void	Initialize()	{m_maxColWidth = 0;}
  void	Destroy()	{ }
  void	Copy_(const DString_Array& cp) {m_maxColWidth = cp.m_maxColWidth;}
  COPY_FUNS(DString_Array, String_Array);
  TA_BASEFUNS(DString_Array);
protected:
  int		m_maxColWidth; // note: only aprox, and can be too large if lines deleted
};

class String_Data : public DataArray<DString_Array> {
  // #NO_UPDATE_AFTER string data
#ifndef __MAKETA__
typedef DataArray<String_Array> inherited;
#endif
public:
  override bool		is_string() {return true;}// these tests are done all the time
  override int		maxColWidth(); // note: aprox, esp if items get deleted
  override String 	GetValAsString(int row);
  void	Initialize();
  void	Destroy()	{}
  TA_BASEFUNS(String_Data);
};


class ClustLink : public taBase {
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


class ClustNode : public taNBase {
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

class DA_ViewSpec : public taNBase {
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

class DT_ViewSpec :  public taGroup<DA_ViewSpec> {
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
class DA_TextViewSpec: public DA_ViewSpec {
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

class DA_NetViewSpec: public DA_ViewSpec {
  // data-array view spec for netview based display
public:
  int	label_index;	// index into the netview's label list
  void  Initialize();
  void  Destroy();
  void Copy_(const DA_NetViewSpec& cp) { label_index = cp.label_index;}
  COPY_FUNS(DA_NetViewSpec,DA_ViewSpec);
  TA_BASEFUNS(DA_NetViewSpec);
};

class DA_GridViewSpec : public DA_ViewSpec {
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

class DT_GridViewSpec : public DT_ViewSpec {
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
