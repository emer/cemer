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

#ifndef ClustNode_h
#define ClustNode_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <taBase_List>
#include <ClustLink>
#include <taMath>

// declare all other types mentioned but not required to include:
class taMatrix; // 
class float_Matrix; // 
class DataTable; // 


taTypeDef_Of(ClustNode);

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

#endif // ClustNode_h
