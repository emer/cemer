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


// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// Hack file for maketa because it chokes on Qt header files

#ifndef TAMISC_MAKETA_DEFS_H
#define TAMISC_MAKETA_DEFS_H

#ifdef __MAKETA__

#include "ta_maketa_defs.h"

#ifdef TA_USE_INVENTOR
class SoBase {// ##NO_INSTANCE ##NO_TOKENS
public:
	int dummy; // #IGNORE
	virtual ~SoBase() {}
};

class SoFieldContainer: public SoBase {
public:
	int dummy; // #IGNORE
};

class SoNode: public SoFieldContainer {
public:
	int dummy; // #IGNORE
};

class SoGroup: public SoNode {
public:
	int dummy; // #IGNORE
};

class SoSeparator: public SoGroup {
public:
	int dummy; // #IGNORE
};

class SoTriangleStripSet: public SoBase {
public:
  int dummy; // #IGNORE
};

#endif

#endif // __MAKETA__
#endif // QTMAKETA_H
