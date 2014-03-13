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

#ifndef VEMuscle_h
#define VEMuscle_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <taVector3f>
#include <taSmartRefT>

// declare all other types mentioned but not required to include:
class VEArm; //

taTypeDef_Of(VEMuscle);

class TA_API VEMuscle : public taNBase {
  // the basic stuff for all muscles
  INHERITED(taNBase)
public:

  taVector3f 	IPprox;    // proximal/medial insertion point
  taVector3f 	IPdist;    // distal/lateral insertion point
  taVector3f 	p3;        // point of intersection with the bending line
  bool 		bend;      // true if the muscle is currently bending

  virtual taVector3f 	Contract(float stim);
  // #IGNORE Returns the force vector (pointing towards the proximal insertion point) resulting from a given stimulation of the muscle;

  virtual float Length();       // Returns current length of muscle
  virtual float Speed();  	// Returns muscle's contraction speed one world stepsize ago
  virtual void UpOld();		// update past values stored
  virtual void InitBuffs();     // Initialize the buffers that store past values
  virtual float Old_Length();   // Delayed value of muscle length
  virtual float Old_Speed();    // Delayed value of muscle speed
  virtual VEArm* GetArm();      // Get pointer to VEArm containing muscle

  TA_SIMPLE_BASEFUNS(VEMuscle);
private:

  void Initialize();
  void Destroy();
};

SMARTREF_OF(VEMuscle); // VEMuscleRef

#endif // VEMuscle_h
