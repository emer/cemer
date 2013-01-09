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

#ifndef T3NodeParent_h
#define T3NodeParent_h 1

// parent includes:
#include <T3Node>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API T3NodeParent: public T3Node {
  // ##NO_INSTANCE ##NO_TOKENS  an base class for PDP project items, like networks, etc.
#ifndef __MAKETA__
typedef T3Node inherited;

  SO_NODE_HEADER(T3NodeParent);
#endif // def __MAKETA__
public:
  static void		initClass();

  SoFont*		captionFont(bool auto_create = false); // override
  SoSeparator*		shapeSeparator() {return shapeSeparator_;}
  SoSeparator*		childNodes(); // use this to set/get T3Node children

  void			addRemoveChildNode(SoNode* node, bool adding); // override

  T3NodeParent(T3DataView* dataView_ = NULL);

protected:
  SoSeparator*		captionSeparator(bool auto_create = false); // override

  ~T3NodeParent();
private:
  SoSeparator*		shapeSeparator_; // #IGNORE
  SoSeparator*		childNodes_; // #IGNORE
};

#endif // T3NodeParent_h
