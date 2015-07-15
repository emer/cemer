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

#ifndef ParamSearchAlgo_h
#define ParamSearchAlgo_h 1

// parent includes:
#include <taNBase>

// smartptr, ref includes
#include <taSmartRefT>

// member includes:

// declare all other types mentioned but not required to include:
class ClusterRun; //

taTypeDef_Of(ParamSearchAlgo);

class TA_API ParamSearchAlgo : public taNBase {
  // #VIRT_BASE ##INSTANCE #NO_INSTANCE Parameter Search algorithm base class.
  INHERITED(taNBase)
public:
  TA_ABSTRACT_BASEFUNS_NOCOPY(ParamSearchAlgo) //
  SIMPLE_CUTLINKS(ParamSearchAlgo);
  virtual void InitLinks();

  virtual void Reset();
  virtual bool CreateJobs();
  virtual void ProcessResults();
protected:
  ClusterRun *m_cluster_run;
private:
  void Initialize();
  void Destroy() { }
};

SMARTREF_OF(TA_API, ParamSearchAlgo); // ParamSearchAlgoRef

#endif // ParamSearchAlgo_h
