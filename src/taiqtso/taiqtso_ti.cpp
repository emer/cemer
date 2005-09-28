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

// template instantiation file

// please include relevant files and instantiate templates here..

#include "safeptr_so.h"
#include "taiqtso_def.h"
#include <qcolor.h>
#ifdef TA_USE_INVENTOR
template class SoPtr<SoNode>;
template class SoPtr<SoGroup>;
template class SoPtr<SoSeparator>;
template class SoPtr<SoSelection>;
template class SoPtr<SoTransform>;
template class SoPtr<SoSeparatorKit>;
#endif // TA_USE_INVENTOR
// note: following are defined in tai_qtso_def.h -- they aren't really templates, but
// there was no other global .cc file in which to put them...

QColor COLOR_HILIGHT_(0xFF, 0xFF, 0xCC);
QColor COLOR_BRIGHT_HILIGHT_(0xFF, 0xFF, 0x00);

QColor& COLOR_HILIGHT = COLOR_HILIGHT_;
QColor& COLOR_BRIGHT_HILIGHT = COLOR_BRIGHT_HILIGHT_;

/*
QColor& qtsoMisc::color_hilight = COLOR_HILIGHT_;
QColor& qtsoMisc::color_bright_hilight = COLOR_BRIGHT_HILIGHT_;
*/
