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



#ifndef fun_lookup_h
#define fun_lookup_h

#include "datatable.h"
#include "tamisc_TA_type.h"

class FunLookup : public float_RArray {
  // function lookup for non-computable functions and optimization
public:
  MinMaxRange	x_range;	// range of the x axis
  float		res;		// resolution of the function
  float		res_inv;	// #READ_ONLY #NO_SAVE 1/res: speeds computation because multiplies are faster than divides

  inline float	Yval(float x) const	
    // get y value at given x value (no interpolation)
    { return SafeEl( (int) ((x - x_range.min) * res_inv)); }

  inline float	Xval(int idx)	// get x value for given index position within list
  { return x_range.min + ((float)idx * res); }

  inline float	Eval(float x) 	{
    // get value via linear interpolation between points..
    int idx = (int) floor((x - x_range.min) * res_inv);
    if(idx < 0) return FastEl(0); if(idx >= size-1) return FastEl(size-1);
    float x_0 = x_range.min + (res * (float)idx);
    float y_0 = FastEl(idx);
    float y_1 = FastEl(idx+1);
    return y_0 + (y_1 - y_0) * ((x - x_0) * res_inv);
  }

  virtual void	AllocForRange(); // allocate values for given range and resolution

  virtual void	Plot(ostream& strm); // generate file for plotting function

  virtual void	Convolve(const FunLookup& src, const FunLookup& con);
  // convolve source array with convolve array and put result here

  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy()		{ };
  void	InitLinks();
  void 	Copy_(const FunLookup& cp);
  COPY_FUNS(FunLookup, float_RArray);
  TA_BASEFUNS(FunLookup);
};

class Tokenizer {
  // #IGNORE this is a basic class for parsing a stream of input into alpha/numeric categories. written by Dan Cer
protected:
  istream  	*ifstrm;	// the input stream
  char 		last_char;	// last character read
  char 		get_char();
  void 		unget_char();
public:
  enum Token_Type {
    Alpha,
    Numeric,
    Other
  };

  int 		line_num;	// line number within the input file
  Token_Type 	token_type; 	// Type of the last token
  String 	token;		// the string value of the token

  virtual bool 	next();		// get next element: main interface
  virtual void 	error_msg(char *msg_part1, char *msg_part2 = "");
  virtual bool 	expect(char *token_str, char *err_msg);

  Tokenizer(istream &ifstrm);
  virtual ~Tokenizer();
};

// the following was written by Dan Cer (modified by O'Reilly)
class FunLookupND : public taNBase {
  // #NO_UPDATE_AFTER n-dimensional function lookup: function is defined for n_dims inputs and produces a corresponding interpolated output
public:
  int 		n_dims;		// number of dimensions
  float_Array 	mesh_pts;	// the n-dimensional mesh of function values at specific points in n_dims space
  int_Array   	mesh_dim_sz;	// [n_dims] number of points along each dimension
  float_Array 	deltas;		// [n_dims] size of the mesh increment for each dimension
  float_Array 	range_mins;	// [n_dims] minimum dimension values for each dimension
  float_Array 	range_maxs;	// [n_dims] maximum dimension values for each dimension

  int	MeshPtToMeshIdx(int* pt);
  // #IGNORE converts an n_dims specification of the location of a given point into an index into the array

  float EvalArray(float* x, int* idx_map = NULL);
  // evaluate function given an n_dims array of dimension input values (index map for mapping the array onto internally-stored dimensions if non-NULL)
  float EvalArgs(float d0, float d1=0.0f, float d2=0.0f, float d3=0.0f, float d4=0.0f,
		 float d5=0.0f, float d6=0.0f, float d7=0.0f, float d8=0.0f, float d9=0.0f);
  // #BUTTON evaluate function given n_dims supplied values as args (in direct correspondence with internally-stored dimensions)

  virtual void  LoadTable(istream& is);
  // #BUTTON load a lookup table of function values at specific points in n_dims space
  virtual void  ListTable(ostream& strm = cout);
  // #BUTTON #ARGC_0 #CONFIRM output the lookup table in text format
  virtual void  ShiftNorm(float desired_mean);
  // #BUTTON normalize the function values via an additive shift to achieve overall desired mean
  virtual void  MulNorm(float desired_mean);
  // #BUTTON normalize the function values by multiplying positive and negative values by separate scaling factors to achieve desired mean

  void  Initialize();
  void  Destroy()       { CutLinks(); }
  void  InitLinks();
  void  CutLinks();
  void 	Copy_(const FunLookupND& cp);
  COPY_FUNS(FunLookupND, taNBase);
  TA_BASEFUNS(FunLookupND);
};


#endif // fun_lookup_h
