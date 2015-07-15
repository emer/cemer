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

#ifndef NewViewHelper_h
#define NewViewHelper_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:
class T3Panel;
class taBase;


class TA_API NewViewHelper {
  // A helper class for the New() methods of VEWorldView, NetView, GridTableView, and GraphTableView.  Its responsibility is to validate parameters when the user requests a new view for some object.
public:
  // Constructor takes parameters for the frame that the new
  // object will be represented in, the object, and the generic
  // name of the object (e.g., "network" or "world").
  // TODO: obj probably should be a const taBase *.
  NewViewHelper(T3Panel *&fr, taBase *obj, const char *objName);
  
  // Run a series of validity checks on the parameters.  The bool
  // parameter specifies if there should only be one instance of
  // the object rendered in the frame (used for network views).
  // Some of the checks may prompt the user with dialogs or
  // produce warnings.  The frame pointer may be modified.
  bool isValid(bool enforceOneObjectInstancePerFrame = false);
  
  // Render and view the frame.
  void showFrame();

private:
  // Check if the selected frame is in the same project as the object.
  bool inSameProject();

  // Check if the object is already being viewed in the frame.
  bool objectAlreadyInFrame();

  // Check if a frame is available to add the new object to.
  // Creates a new frame if not.
  bool getFrame();

  T3Panel *&fr_;
  taBase *obj_;
  const char *objName_;
};

#endif // NewViewHelper_h
