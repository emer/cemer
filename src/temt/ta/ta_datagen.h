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

#ifndef ta_datagen_h
#define ta_datagen_h

#include "ta_dataproc.h"

class TA_API SubMatrixOpEl : public DataOpEl {
  // sub matrix operations element -- specifies a sub-matrix column and offset of that sub-matrix into larger matrix to operate on
  INHERITED(DataOpEl)
public:
  MatrixGeom		offset;

  override String GetDisplayName() const;
  void  Initialize();
  void 	Destroy()		{ };
  TA_SIMPLE_BASEFUNS(SubMatrixOpEl);
protected:
  override void	 CheckThisConfig_impl(bool quiet, bool& rval);
};

class TA_API SubMatrixOpSpec : public DataOpBaseSpec {
  // sub matrix operations spec -- contains a list of columns that contain sub matricies, and their offsets within a larger matrix
INHERITED(DataOpBaseSpec)
public:
  DataTableRef		sub_matrix_table;
  // pointer to a sub matrix data table, for looking up the column names in the spec (different tables can be used for actual processing as long as they have the same structure)

  TA_SIMPLE_BASEFUNS(SubMatrixOpSpec);
protected:
  override void UpdateAfterEdit_impl();

private:
  void	Initialize();
  void 	Destroy()		{ };
};



class TA_API taDataGen : public taNBase {
  // #STEM_BASE ##CAT_Data collection of data generation functions (e.g., drawing, random patterns, combining lists)
INHERITED(taNBase)
public:

  enum CombineOp {		// how to combine numbers
    MULTIPLY,			// multiply the numbers
    ADD,			// add the numbers
  };

  static bool	CheckDims(float_Matrix* mat, int dims);
  // #IGNORE check dimensionality of matrix, issue warning if not correct

  static DataCol* GetFloatMatrixDataCol(const DataTable* src_data, const String& data_col_nm);
  // #IGNORE get named column from data, with checks that it is a matrix of type float
  static DataCol* GetFloatDataCol(const DataTable* src_data, const String& data_col_nm);
  // #IGNORE get named column from data, with checks that it is a scalar of type float

  static bool	GetDest(DataTable*& dest, const DataTable* src, const String& suffix);
  // #IGNORE helper function: if dest is NULL, a new one is created in proj.data.AnalysisData, with name from source + suffix

  ///////////////////////////////////////////////////////////////////
  // basic operations

  static bool Clear(DataTable* data, const String& col_nm, float val = 0.0);
  // #MENU_BUTTON #MENU_ON_Basic #CONFIRM #CAT_Basic clear existing patterns: set all values to given value in float matrix column col_nm (empty col_nm = all float matrix columns)
  static bool SimpleMath(DataTable* data, const String& col_nm, const SimpleMathSpec& math);
  // #MENU_BUTTON #CAT_Basic Apply simple math operation to all values in float matrix column col_nm (empty col_nm = all float matrix columns)

  ///////////////////////////////////////////////////////////////////
  // operations on lists of elements/conditions (combine, replicate by frequency, etc)

  static bool	CrossLists(DataTable* crossed_output, const DataTable* data_list_1,
			   const DataTable* data_list_2, const DataTable* data_list_3=NULL,
			   const DataTable* data_list_4=NULL, const DataTable* data_list_5=NULL);
  // #MENU_BUTTON #MENU_ON_Lists #CAT_Lists #NULL_OK_0 #NULL_OK_3 #NULL_OK_4 #NULL_OK_5 creates a full set of combination of elements from two or more lists (of conditions or other items) -- for each row in the first list, replicate all rows in the second list, and so on..
  static bool	CombineFrequencies(DataTable* freq_output, const DataTable* data_list_in,
				   const String& freq_col_nm = "frequency",
				   CombineOp opr = MULTIPLY,
				   bool renorm_freqs=true);
  // #MENU_BUTTON #CAT_Lists #NULL_OK_0 for a data table containing a list of items with multiple frequency_x columns (e.g., as created by the CrossLists function), this will combine the frequencies into one overall frequency in the output table freq_output, using given operation.  If renorm_freqs, then overall frequencies are renormalized as probabilities to sum to 1
  static bool	ReplicateByFrequency(DataTable* repl_output, const DataTable* data_list_in,
				     int total_number, const String& freq_col_nm = "frequency",
				     bool renorm_freqs=true);
  // #MENU_BUTTON #CAT_Lists #NULL_OK_0 replicate the items in the input data by the number given in the frequency column times the total_number value (equivalent to PERMUTED form of frequency sampling -- without replacement, always the same number), optionally renormalizing the frequency values to sum to 1 (does not affect data_list_in table)
  static bool	SampleByFrequency(DataTable* repl_output, const DataTable* data_list_in,
				  int n_samples=1, const String& freq_col_nm = "frequency",
				  bool renorm_freqs=true);
  // #MENU_BUTTON #CAT_Lists #NULL_OK_0 sample the items in the input data as a function of the probability value given in the frequency column, with n_samples taken per row (equivalent to RANDOM form of frequency sampling -- with replacement -- total N varies), optionally renormalizing the frequency values to sum to 1 (does not affect data_list_in table)
  static bool	NsByFrequency(DataTable* repl_output, const DataTable* data_list_in,
			      int total_number, const String& freq_col_nm = "frequency",
			      bool renorm_freqs=true);
  // #MENU_BUTTON #CAT_Lists #NULL_OK_0 compute the numbers of items in the input data that would be produced by the ReplicateByFrequency fnction (frequency column times the total_number value, optionally renormalizing the frequency values to sum to 1 (does not affect data_list_in table))
  static int 	ProbSelectRow(DataTable* data_table, const String& key_col,
			      const String& key_val, const String& p_col);
  // #MENU_BUTTON #CAT_Lists select a row from data table from among subset of rows that have key_val for column key_col using probabilities given in p_col column -- very useful for randomly generating events based on a set of probabilities for given options at each point
  static int 	ProbSelectColNo(DataTable* data_table, int row_no,
				const String& colval1, const String& colp1,
				const String& colval2, const String& colp2,
				const String& colval3="", const String& colp3="",
				const String& colval4="", const String& colp4="",
				const String& colval5="", const String& colp5="",
				const String& colval6="", const String& colp6="",
				const String& colval7="", const String& colp7="",
				const String& colval8="", const String& colp8="");
  // #MENU_BUTTON #CAT_Lists select a column number from data table based on probabilities associated with different columns -- the colval is the name of the column to return the column number of given the associated probability in colp -- very useful for randomly generating events based on a set of probabilities for given options at each point
  static Variant ProbSelectColVal(DataTable* data_table, int row_no,
				  const String& colval1, const String& colp1,
				  const String& colval2, const String& colp2,
				  const String& colval3="", const String& colp3="",
				  const String& colval4="", const String& colp4="",
				  const String& colval5="", const String& colp5="",
				  const String& colval6="", const String& colp6="",
				  const String& colval7="", const String& colp7="",
				  const String& colval8="", const String& colp8="");
  // #MENU_BUTTON #CAT_Lists select a column value from data table based on probabilities associated with different columns -- the colval is the name of the column to return the column value of given the associated probability in colp -- very useful for randomly generating events based on a set of probabilities for given options at each point



  ///////////////////////////////////////////////////////////////////
  // drawing routines

  static bool 	WritePoint(float_Matrix* mat, int x, int y, float color=1.0, bool wrap=true);
  // #CAT_Draw write a single point
  static bool 	RenderLine(float_Matrix* mat, int xs, int ys, int xe, int ye,
			   float color=1.0, bool wrap=true);
  // #CAT_Draw #MENU_BUTTON #MENU_ON_Draw render a line from given x,y starting, ending coords in 2d space

  static bool 	WriteXPoints(float_Matrix* mat, int x, int y, const float_Matrix& color,
			     int wdth=1, bool wrap=true);
  // #CAT_Draw write a series of points of given width in x dimension using colors in order
  static bool 	WriteYPoints(float_Matrix* mat, int x, int y, const float_Matrix& color,
			     int wdth=1, bool wrap=true);
  // #CAT_Draw write a series of points of given width in y dimension using colors in order
  static bool 	RenderWideLine(float_Matrix* mat, int xs, int ys, int xe, int ye,
			       const float_Matrix& color, int wdth=1, bool wrap=true);
  // #CAT_Draw #MENU_BUTTON render a wide line from given x,y starting, ending coords in 2d space

  ///////////////////////////////////////////////////////////////////
  // random pattern generation

  static bool AddNoiseMat(float_Matrix* mat, const Random& rnd_spec);
  // #CAT_Random add random noise to given pattern

  static bool AddNoise(DataTable* data, const String& col_nm, const Random& rnd_spec);
  // #MENU_BUTTON #MENU_ON_Random #CAT_Random add random noise of specified type to the patterns (pat_no: -1 = all pats)

  static bool	PermutedBinaryMat(float_Matrix* mat, int n_on, float on_val=1.0f, float off_val=0.0f);
  // #CAT_Random set matrix values to permuted binary pattern of n_on on_vals and rest off_vals
  static bool	PermutedBinary(DataTable* data, const String& col_nm, int n_on, float on_val=1.0f, float off_val=0.0f);
  // #MENU_BUTTON #CAT_Random create permuted binary patterns of n_on on_vals (1's) and rest off_vals (0's) in given col (must be float matrix) (col_nm = empty = all float matrix columns)
  static bool PermutedBinary_MinDist(DataTable* data, const String& col_nm, int n_on,
				       float dist, taMath::DistMetric metric=taMath::HAMMING,
				       bool norm=false, float tol=0.0f);
  // #MENU_BUTTON #CAT_Random create permuted binary patterns with dist minimum hamming distance (or dist max_correl) (col nm = empty = all float matrix columns)

  static bool FlipBitsMat(float_Matrix* mat, int n_off, int n_on);
  // #CAT_Random flip n_off of the 1 bits into the 0 state, and n_on of the 0 bits to the 1 state
  static bool FlipBits(DataTable* data, const String& col_nm, int n_off, int n_on);
  // #MENU_BUTTON #CAT_Random flip n_off bits from 1's to 0's, and n_on bits from 0's to 1's in float matrix column col_nm (col_nm empty = all float matrix columns)
  static bool FlipBits_MinMax(DataTable* data, const String& col_nm, int n_off, int n_on,
			       float min_dist, float max_dist,
			       taMath::DistMetric metric=taMath::HAMMING,
			       bool norm=false, float tol=0.0f);
  // #MENU_BUTTON flip bits, ensuring range within min and max distances (pat_no: -1 = all pats)
//   static bool  FlipBits_GpMinMax(int pat_no, int n_off, int n_on, float within_min_dist,
// 				  float within_max_dist, float between_dist,
// 				  taMath::DistMetric metric=taMath::HAMMING,
// 				  bool norm=false, float tol=0.0f, int st_gp=0, int ed_gp=-1);
//   // #MENU_BUTTON flip bits, ensuring within-group min and max distances, and between-group min dist (pat_no: -1 = all pats)

  // helper pattern-wise functions for above generation routines
  static float LastMinDist(DataCol* da, int row,
			   taMath::DistMetric metric=taMath::HAMMING,
			   bool norm=false, float tol=0.0f);
  // #CAT_Distance returns minimum distance (or max correl) between last (n th) pattern and all previous
  static float LastMinMaxDist(DataCol* da, int row, float& max_dist,
			      taMath::DistMetric metric=taMath::HAMMING,
			      bool norm=false, float tol=0.0f);
  // #CAT_Distance returns min and max distance between last (n th) pattern and all previous

//   static float GpWithinMinMaxDist(Event_MGroup* gp, int n, int pat_no, float& max_dist,
// 				   taMath::DistMetric metric=taMath::HAMMING,
// 				   bool norm=false, float tol=0.0f);
//   // #CAT_Distance returns min and max distance between last (n th) pattern and all previous within group
//   static float GpLastMinMaxDist(int gp_no, Pattern* trg_pat, int pat_no, float& max_dist, 
// 				 taMath::DistMetric metric=taMath::HAMMING,
// 				 bool norm=false, float tol=0.0f, int st_gp=0);
//   // #CAT_Distance returns min and max distance between patterns in all groups up to gp_no for pattern pat
//   static float GpMinMaxDist(Event_MGroup* gp, Pattern* trg_pat, int pat_no, float& max_dist,
// 			     taMath::DistMetric metric=taMath::HAMMING,
// 			     bool norm=false, float tol=0.0f);
//   // #CAT_Distance returns min and max distance between probe pattern and all in group


  ///////////////////////////////////////////////////////////////////
  // sub-matrix operations -- read/write from sub matricies to/from larger matrix

  static bool	WriteFmSubMatricies(DataTable* dest, const String& dest_col_nm, 
				    DataTable* sub_mtx_src, SubMatrixOpSpec* spec, 
				    bool reset_first = true);
  // #CAT_SubMatrix #MENU_BUTTON #MENU_ON_SubMatrix write to column dest_col_nm in destination table from sub matricies in sub_mtx_src according to columns and offsets in given spec -- if reset_first, dest is reset first  -- rows are always set to number in source

  static bool	ReadToSubMatricies(DataTable* src, const String& src_col_nm, 
				   DataTable* sub_mtx_dest, SubMatrixOpSpec* spec,
				   bool reset_first = true);
  // #CAT_SubMatrix #MENU_BUTTON #MENU_ON_SubMatrix read from column src_col_nm in source table to sub matricies in sub_mtx_dest according to columns and offsets in given spec -- if reset_first, dest is reset first  -- rows are always set to number in source

  static bool	WriteFmSubMatricies_Render(DataTable* dest, const String& dest_col_nm, 
					   DataTable* sub_mtx_src, SubMatrixOpSpec* spec, 
					   taMatrix::RenderOp render_op,
					   bool reset_first = true);
  // #CAT_SubMatrix #MENU_BUTTON #MENU_ON_SubMatrix write to column dest_col_nm in destination using given rendering operation from sub matricies in sub_mtx_src according to columns and offsets in given spec -- if reset_first, dest is reset first  -- rows are always set to number in source

  static bool	ReadToSubMatricies_Render(DataTable* src, const String& src_col_nm, 
					  DataTable* sub_mtx_dest, SubMatrixOpSpec* spec,
					  taMatrix::RenderOp render_op,
					  bool reset_first = true);
  // #CAT_SubMatrix #MENU_BUTTON #MENU_ON_SubMatrix read from column src_col_nm in source using given rendering operation to sub matricies in sub_mtx_dest according to columns and offsets in given spec -- if reset_first, dest is reset first  -- rows are always set to number in source

  ///////////////////////////////////////////////////////////////////
  // 	FeatPats -- Generating larger patterns from Feature patterns

  static taMatrix*  GetFeatPatNo(DataTable* feat_vocab, const String& feat_col_nm, int row_no);
  // #CAT_FeatPats get feature pattern matrix from given row of feature vocabulary data table, from feature pattern column name -- column must have 2d matrix cells -- important: must ref the return val in calling function (use taMatrixPtr)
  static taMatrix*  GetFeatPatName(DataTable* feat_vocab, const String& feat_col_nm,
				   const String& feat_row_nm, const String& name_col_nm = "Name");
  // #CAT_FeatPats get feature pattern matrix from row with given name (in column name_col_nm) of feature vocabulary data table, from feature pattern column name -- column must have 2d matrix cells -- important: must ref the return val in calling function (use taMatrixPtr)
  static taMatrix*  GetRndFeatPat(DataTable* feat_vocab, const String& feat_col_nm);
  // #CAT_FeatPats get a random feature pattern from feature vocabulary data table, given feature pattern column name -- column must have 2d matrix cells -- important: must ref the return val in calling function (use taMatrixPtr)

  static bool	    GenRndFeatPats(DataTable* dest, const String& dest_col, int n_pats,
				   DataTable* feat_vocab, const String& feat_col_nm);
  // #CAT_FeatPats #MENU_BUTTON #MENU_ON_FeatPats generate n_pats new patterns (rows) in dest data table by stacking together randomly generated features from the feature vocabulary table -- automatically detects how many features to use based on relative geometries (dest must have same innermost (x) dimension as feat pats, next (y) must be even multiple, feat pats must be 2d matrix)

  // todo: min dist version!?

  static bool	    GenItemsFmProtos(DataTable* items, const String& dest_col, 
				     DataTable* protos, int n_items, float flip_pct,
				     DataTable* feat_vocab, const String& feat_col_nm);
  // #CAT_FeatPats #MENU_BUTTON #MENU_ON_FeatPats generate n_items items (per prototype) from prototypes contained in protos table, by flipping flip_pct proportion of the features to a new random feature value in feat_vocab (could end up being the same value by chance!) -- automatically detects how many features to use based on relative geometries (dest must have same innermost (x) dimension as feat pats, next (y) must be even multiple, feat pats must be 2d matrix)

  static bool	    GenNamedFeatPats(DataTable* dest, const String& dest_col,
				     DataTable* feat_vocab, const String& feat_col_nm,
				     DataTable* names, const String& name_col_nm,
				     const String& feat_name_col_nm = "Name");
  // #CAT_FeatPats #MENU_BUTTON #MENU_ON_FeatPats generate patterns in dest table (dest_col) by stacking together feature patterns from feat_vocab (feat_col_nm) based on list of names given in names table (name_col_nm) -- name column must contain a String matrix -- items are looked up by name in the feat_name_col_nm -- automatically detects how many features to use based on relative geometries (dest must have same innermost (x) dimension as feat pats, next (y) must be even multiple, feat pats must be 2d matrix)

///////////////////////////////////////////////////////////////////
// gen sorted permutations

  static bool       GenSortedPermutations(DataTable* p,int n);
  // #CAT_Lists generate a sorted list of all possible n! permutations in sorted order and write them to input table p. if n=3, permutations will contain a table with a 3x1 matrix column that has 3! = 6 rows with the following data: {1,2,3}, {1,3,2}, {2,1,3}, {2,3,1}, {3,1,2}, {3,2,1}. any n > 0 works. adapted from "Practical Algorithms in C++"
  static void       GSP_permute(DataTable* p,int_Matrix* v,int start,int n);
  // #IGNORE
  static void       GSP_write(DataTable* p,int_Matrix* v,int size);
  // #IGNORE
  static void       GSP_swap(int_Matrix* v,int i,int j);
  // #IGNORE
  static void       GSP_rotateLeft(int_Matrix* v,int start,int n);
  // #IGNORE

  ///////////////////////////////////////////////////////////////////
  // misc data sources

  static bool	GetDirFiles(DataTable* dest, const String& dir_path, 
			    const String& filter = "", bool recursive = false,
			    const String& fname_col_nm = "FileName",
			    const String& path_col_nm = "FilePath");
  // #CAT_Files #MENU_BUTTON #MENU_ON_Files #NULL_OK_0 #NULL_TEXT_0_NewDataTable read file names from given directory into rows of the data table (must be passed non-null, is NOT reset prior to adding new rows!), with the file name and full path to file (including directory names) written to given string column names (these are created if they do not exist)

  override String 	GetTypeDecoKey() const { return "DataTable"; }
  void Initialize() { };
  void Destroy() { };
  TA_BASEFUNS_NOCOPY(taDataGen);
};


/////////////////////////////////////////////////////////
//   programs to support data generation
/////////////////////////////////////////////////////////

class TA_API DataGenCall : public StaticMethodCall { 
  // call a taDataGen (data generation) function
INHERITED(StaticMethodCall)
public:
  override String	GetToolbarName() const { return "data gen()"; }
  TA_BASEFUNS_NOCOPY(DataGenCall);
private:
  void	Initialize();
  void	Destroy()	{ };
}; 

#endif // ta_datagen_h
