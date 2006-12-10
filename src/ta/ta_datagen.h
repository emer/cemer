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

#ifndef ta_datagen_h
#define ta_datagen_h

#include "ta_datatable.h"
#include "ta_math.h"
#include "ta_program.h"

class TA_API taDataGen : public taNBase {
  // ##CAT_Data collection of data generation functions (e.g., drawing, random patterns)
public:

  static bool	CheckDims(float_Matrix* mat, int dims);
  // #CAT_Config check dimensionality of matrix, issue warning if not correct

  ///////////////////////////////////////////////////////////////////
  // drawing routines

  static bool 	WritePoint(float_Matrix* mat, int x, int y, float color=1.0, bool wrap=true);
  // #CAT_Draw write a single point
  static bool 	RenderLine(float_Matrix* mat, int xs, int ys, int xe, int ye,
			   float color=1.0, bool wrap=true);
  // #CAT_Draw #MENU #MENU_ON_Draw render a line from given x,y starting, ending coords in 2d space

  static bool 	WriteXPoints(float_Matrix* mat, int x, int y, const float_Matrix& color,
			     int wdth=1, bool wrap=true);
  // #CAT_Draw write a series of points of given width in x dimension using colors in order
  static bool 	WriteYPoints(float_Matrix* mat, int x, int y, const float_Matrix& color,
			     int wdth=1, bool wrap=true);
  // #CAT_Draw write a series of points of given width in y dimension using colors in order
  static bool 	RenderWideLine(float_Matrix* mat, int xs, int ys, int xe, int ye,
			       const float_Matrix& color, int wdth=1, bool wrap=true);
  // #CAT_Draw #MENU render a wide line from given x,y starting, ending coords in 2d space

  ///////////////////////////////////////////////////////////////////
  // manipulating lists of columns

//   static bool	PermutedBinary(int pat_no, int n_on);
//   // #MENU #MENU_SEP_BEFORE create permuted binary patterns of n_on 1's and rest 0's (pat_no: -1 = all pats)
//   static bool	PermutedBinary_MinDist(int pat_no, int n_on, float dist,
// 				       float_RArray::DistMetric metric=float_RArray::HAMMING,
// 				       bool norm=false, float tol=0.0f);
//   // #MENU create permuted binary patterns with dist minimum hamming distance (or dist max_correl) (pat_no: -1 = all pats)
//   static bool  FlipBits(int pat_no, int n_off, int n_on);
//   // #MENU flip n_off bits from 1's to 0's, and n_on bits from 0's to 1's (pat_no: -1 = all pats)
//   static bool  FlipBits_MinMax(int pat_no, int n_off, int n_on, float min_dist, float max_dist,
// 				float_RArray::DistMetric metric=float_RArray::HAMMING,
// 				bool norm=false, float tol=0.0f);
//   // #MENU flip bits, ensuring range within min and max distances (pat_no: -1 = all pats)
//   static bool  FlipBits_GpMinMax(int pat_no, int n_off, int n_on, float within_min_dist,
// 				  float within_max_dist, float between_dist,
// 				  float_RArray::DistMetric metric=float_RArray::HAMMING,
// 				  bool norm=false, float tol=0.0f, int st_gp=0, int ed_gp=-1);
//   // #MENU flip bits, ensuring within-group min and max distances, and between-group min dist (pat_no: -1 = all pats)

//   // helper pattern-wise functions for above generation routines
//   static bool	PermutedBinaryPat(Pattern* pat, int n_on, float on_val=1.0f, float off_val=0.0f);
//   // set pat values to permuted binary pattern of n_on on_vals and rest off_vals (pat_no: -1 = all pats)
//   static bool	FlipBitsPat(Pattern* pat, int n_off, int n_on);
//   // flip n_off of the 1 bits into the 0 state, and n_on of the 0 bits to the 1 state
//   static bool	AddNoisePat(Pattern* pat, const Random& rnd_spec);
//   // add random noise to given pattern
//   static float LastMinDist(int n, int pat_no, float_RArray::DistMetric metric=float_RArray::HAMMING,
// 			    bool norm=false, float tol=0.0f);
//   // returns minimum distance (or max correl) between last (n th) pattern and all previous
//   static float LastMinMaxDist(int n, int pat_no, float& max_dist,
// 			       float_RArray::DistMetric metric=float_RArray::HAMMING,
// 			       bool norm=false, float tol=0.0f);
//   // returns min and max distance between last (n th) pattern and all previous
//   static float GpWithinMinMaxDist(Event_MGroup* gp, int n, int pat_no, float& max_dist,
// 				   float_RArray::DistMetric metric=float_RArray::HAMMING,
// 				   bool norm=false, float tol=0.0f);
//   // returns min and max distance between last (n th) pattern and all previous within group
//   static float GpLastMinMaxDist(int gp_no, Pattern* trg_pat, int pat_no, float& max_dist, 
// 				 float_RArray::DistMetric metric=float_RArray::HAMMING,
// 				 bool norm=false, float tol=0.0f, int st_gp=0);
//   // returns min and max distance between patterns in all groups up to gp_no for pattern pat
//   static float GpMinMaxDist(Event_MGroup* gp, Pattern* trg_pat, int pat_no, float& max_dist,
// 			     float_RArray::DistMetric metric=float_RArray::HAMMING,
// 			     bool norm=false, float tol=0.0f);
//   // returns min and max distance between probe pattern and all in group

//   static bool	Clear(int pat_no=-1, float val = 0.0);
//   // #MENU #MENU_SEP_BEFORE #CONFIRM clear out given pattern number (set to given val) (pat_no: -1 = all pats)

//   static bool	AddNoise(int pat_no, const Random& rnd_spec);
//   // #MENU add random noise of specified type to the patterns (pat_no: -1 = all pats)

//   static bool	TransformPats(int pat_no, const SimpleMathSpec& trans);
//   // #MENU Apply given transformation to pattern pat_no of all events (pat_no: -1 = all pats)

  ///////////////////////////////////////////////////////////////////
  // misc data functions

  static bool	GetDirFiles(DataTable* dest, const String& dir_path, 
			    const String& filter = "", bool recursive = false,
			    const String& fname_col_nm = "FileName",
			    const String& path_col_nm = "FilePath");
  // #CAT_Files #MENU #MENU_ON_FILES #NULL_OK read file names from given directory into rows of the data table (must be passed non-null), with the file name and full path to file (including directory names) written to given string column names (these are created if they do not exist)

  void Initialize() { };
  void Destroy() { };
  TA_BASEFUNS(taDataGen);
};


/////////////////////////////////////////////////////////
//   programs to support data generation
/////////////////////////////////////////////////////////

class TA_API DataGenCall : public StaticMethodCall { 
  // call a taDataGen (data generation) function
INHERITED(StaticMethodCall)
public:
  TA_BASEFUNS(DataGenCall);
private:
  void	Initialize();
  void	Destroy()	{ };
}; 

#endif // ta_datagen_h
