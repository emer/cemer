// Copyright, 1995-2007, Regents of the University of Colorado,
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

#ifndef ta_imgproc_h
#define ta_imgproc_h

// image processing operations

// there are multiple levels of filtering and image processing, ordered in this file as:
// -- taImage, Canvas -- basic data and drawing on images
// -- basic image filtering: DoG, Gabor, etc
// -- taImgProc -- static container class with various image processing operations
// -- specific parameterizations for DoG and V1 levels of image processing, representing a
//	region of the retina (DoGRegionSpec, V1RegionSpec)
// -- an overall image processing object containing multiple regions, providing full
//	final coverage of the image processing from program user perspective:
//	RetinaProc, DoGRetinaProc, V1RetinaProc

// image format support:
// most ops are done on float_Matrix, which can either be a 2d greyscale or 3d color, with either 3 or 4
// values in the 3rd (outer) dimension -- in memory it is stored as a plane of each color, so often 
// processing can be done by looping over the outer color dimension and calling the 2d version on the 
// slice matrix.
// 
// the args for functions should retain separate r,g,b,a values where relevant, to make it easier to
// interface with program code, instead of requiring color objects etc.  easy to convert to an array
// internally for looping based ops

// #include "ta_math.h"
// #include "ta_program.h"
// #include "ta_geometry.h"
// #include "ta_datatable.h"
// #include "fun_lookup.h"

// #if defined(TA_GUI) && !defined(__MAKETA__)
// # include <QImage>
// # include <QPainter>
// # include <QPainterPath>
// #endif

// class QPainterPath;	// #IGNORE
// class QBrush;		// #IGNORE

// // pre-declare
// class V1RetinaProc;
// //

#endif // ta_imgproc_h
