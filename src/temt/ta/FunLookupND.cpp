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

#include "FunLookupND.h"
#include <Tokenizer>

#include <taMisc>

#include <math.h>

using namespace std;

void FunLookupND::Initialize() {
  n_dims = 0;
}

void FunLookupND::InitLinks() {
  taNBase::InitLinks();
  taBase::Own(deltas, this);
  taBase::Own(range_mins, this);
  taBase::Own(range_maxs, this);
  taBase::Own(mesh_dim_sz, this);
  taBase::Own(mesh_pts, this);
}

void FunLookupND::CutLinks() {
  deltas.CutLinks();
  range_mins.CutLinks();
  range_maxs.CutLinks();
  mesh_dim_sz.CutLinks();
  mesh_pts.CutLinks();
  taNBase::CutLinks();
}

void FunLookupND::Copy_(const FunLookupND& cp) {
  n_dims = cp.n_dims;
  deltas = cp.deltas;
  range_mins = cp.range_mins;
  range_maxs = cp.range_maxs;
  mesh_dim_sz = cp.mesh_dim_sz;
  mesh_pts = cp.mesh_pts;
}

int FunLookupND::MeshPtToMeshIdx(int *pt) {
  int idx = 0; int mesh_dim_mul = 1;
  for (int i = 0; i < n_dims; i++) {
    idx += pt[i]*mesh_dim_mul;
    mesh_dim_mul *= mesh_dim_sz[i];
  }
  return idx;
}

void FunLookupND::LoadTable(istream& is) {
  n_dims = -1;
  Tokenizer tokens(is);

  do {
    tokens.next();
    if (tokens.token == "DIMS") {
      if(n_dims != -1) {
	taMisc::Error("FunLookupND: Each mesh definition can have only one \"DIMS\" declaration");
	break;
      }

      tokens.next();
      n_dims = atoi(tokens.token.chars());
      if(n_dims == 0) {
	taMisc::Error("FunLookupND: \"DIMS\" declaration must be followed by a non-zero numeric value");
	break;
      }
      mesh_dim_sz.SetSize(n_dims);
      range_mins.SetSize(n_dims);
      deltas.SetSize(n_dims);
      range_maxs.SetSize(n_dims);
      for (int i = 0; i < n_dims; i++) mesh_dim_sz[i] = -1;
    }
    else if (tokens.token == "DIM_DEF") {
      if(n_dims == -1) {
	taMisc::Error("FunLookupND: DIM_DEF before DIMS declaration"); break;
      }

      if (!tokens.expect("[", "[\" expected after DIM_DEF\n")) break;

      tokens.next();
      if (tokens.token_type != Tokenizer::Numeric) {
	taMisc::Error("FunLookupND: DIM_DEF index must be numeric\n"); break;
      }

      int dim_def_idx = atoi(tokens.token.chars());
      if (dim_def_idx > n_dims) {
	taMisc::Error("FunLookupND: DIM_DEF index out of range\n"); break;
      }

      if (!tokens.expect("]", "\"]\" expected after DIM_DEF idx\n")) break;
      if (!tokens.expect("(", "\"(\" expected after ]\n")) break;

      tokens.next();
      range_mins[dim_def_idx] = atof(tokens.token.chars());
      if (tokens.token == "-") {
	tokens.next();
	range_mins[dim_def_idx] = -1.0*atof(tokens.token.chars());
      }

      if (tokens.token_type != Tokenizer::Numeric) {
	taMisc::Error("FunLookupND: DIM_DEF range minimum must be numeric");
	break;
      }

      if(!tokens.expect(",", "\",\" expected after range minimun\n")) {
	break;
      }

      tokens.next();
      range_maxs[dim_def_idx] = atof(tokens.token.chars());
      if (tokens.token == "-") {
	tokens.next();
	range_maxs[dim_def_idx] = -1.0*atof(tokens.token.chars());
      }
      if (tokens.token_type != Tokenizer::Numeric) {
	taMisc::Error("FunLookupND: DIM_DEF range maximum must be numeric");
	break;
      }
      if (range_maxs[dim_def_idx] <= range_mins[dim_def_idx]) {
	taMisc::Error("FunLookupND: DIM DEF range max must be greater than range min");
	break;
      }

      if (!tokens.expect(",", "\",\" expected after range maximum\n")) break;

      tokens.next();
      deltas[dim_def_idx] = atof(tokens.token.chars());
      if (deltas[dim_def_idx] == 0) {
	taMisc::Error("FunLookupND: DIM_DEF delta must be a non-zero numeric value"); break;
      }

      if (!tokens.expect(")", "\")\" expected after delta \n")) break;
      mesh_dim_sz[dim_def_idx] = (int) (( (range_maxs[dim_def_idx] -
					   range_mins[dim_def_idx])/
					  deltas[dim_def_idx])+0.5)+1;
    } else if (tokens.token == "DATA") {
      int data_sz = 1;
      bool good_dims = true;
      for (int i = 0; i < n_dims; i++) {
	if (mesh_dim_sz[i] < 0) {
	  taMisc::Error("FunLookupND: DATA section found before range/delta ",
			   "declaration for dimention");
	  good_dims = false; break;
	}
	data_sz *= mesh_dim_sz[i];
      }
      if (!good_dims) break;

      mesh_pts.Alloc(data_sz);
      mesh_pts.SetSize(data_sz);

      bool success = true;
      for (int i = 0; i < data_sz; i++) {
	tokens.next();
	if (tokens.token == "") {
	  taMisc::Error("FunLookupND: Unexpected EOF in DATA declaration");
	  success = false; break;
	}
	mesh_pts[i] = atof(tokens.token.chars());
	if (tokens.token == "-") {
	  tokens.next();
	  mesh_pts[i] =  -1.0*atof(tokens.token.chars());
	}
	if (tokens.token_type != Tokenizer::Numeric) {
	  taMisc::Error("FunLookupND: Only numeric types are allows in the DATA",
			   "section");
	  success = false; break;
	}
      }
      if (success) {
	// taMisc::Info("FunLookupND: Load Success");
	tokens.next();
	if (tokens.token != "") {
	  taMisc::Error("FunLookupND: Warning: Excess tokens after last expected token in the data section");
	}
	break;
      } else {
	break;
      }
    }
    else if (tokens.token != "") {
      taMisc::Error("Syntax error on line:", String(tokens.line_num));
      taMisc::Error("Unrecognized statement type:", String(tokens.token));
      break;
    }
  } while (tokens.token != "");
}

float FunLookupND::EvalArray(float* x, int* idx_map) {
  float rval = 0.0f;
  float wtSum = 0;
  float ptSum = 0;
  int*   meshPt = new int[n_dims];
  int   num_inter_pt_idx =  (int)powf(3.0, (double)n_dims);

  for(int inter_pt_idx = 0; inter_pt_idx < num_inter_pt_idx;  inter_pt_idx++) {
    float sumSqr = 0;
    bool  goodPt = true;
    int   leftOver = inter_pt_idx;
    for(int d = 0; d < n_dims; d++) {
      meshPt[d] = (leftOver % 3)-1;
      meshPt[d] += (idx_map ? (int)floor(0.5f + (x[idx_map[d]]-range_mins[d]) / deltas[d])
		    :	      (int)floor(0.5f + (x[d]-range_mins[d]) / deltas[d]));

      if (meshPt[d] < 0) meshPt[d] = 0;
      if (meshPt[d] >= mesh_dim_sz[d]) meshPt[d] = mesh_dim_sz[d]-1;

      float diff = (idx_map ?  (x[idx_map[d]]-range_mins[idx_map[d]]) - meshPt[d]*deltas[d]
		    :	       (x[d] - range_mins[d]) - meshPt[d]*deltas[d]);
      sumSqr += diff*diff;
      leftOver /= 3;
    }
    if (!goodPt) continue;
    int meshIdx = MeshPtToMeshIdx(meshPt);
    // Prevent possible overflows, & associated NaN issues
    if (fabsf(sumSqr) < 1.0e-4) {
      rval = mesh_pts[meshIdx];
      goto exit;
    }
    float dist = (float)sqrtf(sumSqr);
    float point_wt = 1.0f / (dist*dist);
    ptSum += point_wt*mesh_pts[meshIdx];
    wtSum += point_wt;
  }
  rval = ptSum/wtSum;
exit:
  delete[] meshPt;
  return rval;
}

float FunLookupND::EvalArgs(float d0, float d1, float d2, float d3, float d4,
			    float d5, float d6, float d7, float d8, float d9) 
{
  float rval = 0.0f;
  if(n_dims > 10) {
    taMisc::Warning("*** FunLookupND::EvalArgs: n_dims:", String(n_dims),
		  "larger than max number of args (9)!");
    return rval;
  }
  float* x = new float[n_dims];
  x[0] = d0; x[1] = d1; x[2] = d2; x[3] = d3; x[4] = d4;
  x[5] = d5; x[6] = d6; x[7] = d7; x[8] = d8; x[9] = d9;
  rval = EvalArray(x);
//exit:
  delete[] x;
  return rval;
}


String& FunLookupND::ListTable(String& strm) {
  strm<<"Mesh Values:\n";
  strm<<"====================\n";
  for (int i=0; i < mesh_pts.size; i++) {
    strm<<"\tmesh_pts["<<i<<"]: "<<mesh_pts[i]<<"\n";
  }
  strm<<"\n\n";
  return strm;
}

void FunLookupND::ShiftNorm(float desired_mean) {
  double orig_mean = 0;
  for (int i = 0; i < mesh_pts.size; i++) {
    orig_mean += mesh_pts[i];
  }
  orig_mean /= mesh_pts.size;
  double norm_term = desired_mean - orig_mean;
  for (int i = 0; i < mesh_pts.size; i++) {
    mesh_pts[i] += norm_term;
  }

  // test code
  // double new_mean = 0;
  // for (int i = 0; i < mesh_pts.size; i++) {
  //   new_mean += mesh_pts[i];
  // }
  // new_mean /= mesh_pts.size;
  // taMisc::DebugInfo("Orig mean:", String(orig_mean));
  // taMisc::DebugInfo("new mean: ", String(new_mean));
  // taMisc::DebugInfo("Desired mean: ", String(desired_mean));
  // end test code
}

void FunLookupND::MulNorm(float desired_mean) {
  double pos_mag = 0;
  double neg_mag = 0;

  double orig_mean = 0;
  for (int i = 0; i < mesh_pts.size; i++) {
    if (mesh_pts[i] > 0) {
      pos_mag += mesh_pts[i];
    } else {
      neg_mag -= mesh_pts[i];
    }
    orig_mean += mesh_pts[i];
  }
  // debug code
  taMisc::DebugInfo("pos mag: ", String(pos_mag));
  taMisc::DebugInfo("neg mag: ", String(neg_mag));

  // end debug code
  double pos_norm_term, neg_norm_term;
  if (pos_mag > neg_mag) {
    pos_norm_term = 1;
    neg_norm_term = pos_mag/neg_mag;
  } else {
    neg_norm_term = 1;
    pos_norm_term = neg_mag/pos_mag;
  }

  for (int i = 0; i < mesh_pts.size; i++) {
    mesh_pts[i] = (mesh_pts[i] > 0 ? pos_norm_term*mesh_pts[i] : neg_norm_term*mesh_pts[i]);
  }

  // test code
  // double new_mean = 0;
  // for (int i = 0; i < mesh_pts.size; i++) {
  //   new_mean += mesh_pts[i];
  // }
  // new_mean /= mesh_pts.size;
  // taMisc::DebugInfo("Orig mean:", String(orig_mean));
  // taMisc::DebugInfo("new mean: ", String(new_mean));
  // taMisc::DebugInfo("Desired mean: ", String(desired_mean));
  // end test code
}

