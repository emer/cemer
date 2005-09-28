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




#ifndef graphic_objs_h
#define graphic_objs_h

#include "ta_stdef.h"

#include <math.h>

/*
 * Alignment needs to be unsigned so that it can be stored
 * as a bit field.
 */

enum Alignment {
  TopLeft = 0,
  TopCenter = 1,
  TopRight = 2,
  CenterLeft = 3,
  Center = 4,
  CenterRight = 5,
  BottomLeft = 6,
  BottomCenter = 7,
  BottomRight = 8,
  Left = 9,
  Right = 10,
  Top = 11,
  Bottom = 12,
  HorizCenter = 13,
  VertCenter = 14
};

class PointObj {
public:
    PointObj(float = 0, float = 0);
    PointObj(PointObj*);

    float Distance(PointObj&);
public:
    float _x, _y;
};

class LineObj {
public:
    LineObj(float = 0, float = 0, float = 0, float = 0);
    LineObj(LineObj*);

    bool Contains(PointObj&);
    int Same(PointObj& p1, PointObj& p2);
    bool Intersects(LineObj&);
public:
    PointObj _p1, _p2;
};

class BoxObj {
public:
    BoxObj(float = 0, float = 0, float = 0, float = 0);
    BoxObj(BoxObj*);

    bool operator==(BoxObj&);
    bool Contains(PointObj&);
    bool Intersects(BoxObj&);
    bool Intersects(LineObj&);
    BoxObj operator-(BoxObj&);
    BoxObj operator+(BoxObj&);
    bool Within(BoxObj&);
public:
    float _left, _right;
    float _bottom, _top;
};

class MultiLineObj {
public:
  MultiLineObj(float* = NULL, float* = NULL, int = 0);
  virtual ~MultiLineObj();

  void GetBox(BoxObj& b);
  bool Contains(PointObj&);
  bool Intersects(LineObj&);
  bool Intersects(BoxObj&);
  bool Within(BoxObj&);
  void SplineToMultiLine(float* cpx, float* cpy, int cpcount);
  void ClosedSplineToPolygon(float* cpx, float* cpy, int cpcount);
protected:
  static int 		mlsize; // size (in items) of shared static buffer array
  static int 		mlcount; // count (in items) actually used in static buffer array
  static float* 	mlx; // x item buffer array
  static float* 	mly; // y item buffer array

  void GrowBuf(); // grows the shared static buffer
  bool CanApproxWithLine(
      double x0, double y0, double x2, double y2, double x3, double y3
  );
  void AddLine(double x0, double y0, double x1, double y1);
  void AddBezierArc(
      double x0, double y0, double x1, double y1,
      double x2, double y2, double x3, double y3
  );
  void CalcSection(
      float cminus1x, float cminus1y, float cx, float cy,
      float cplus1x, float cplus1y, float cplus2x, float cplus2y
  );
public:
  float* _x; float* _y;
  int _count;
};

class FillPolygonObj : public MultiLineObj {
public:
    FillPolygonObj(float* = NULL, float* = NULL, int = 0);
    virtual ~FillPolygonObj();

    bool Contains(PointObj&);
    bool Intersects(LineObj&);
    bool Intersects(BoxObj&);
protected:
    void Normalize();
protected:
    float* _normx; float* _normy;
    int _normCount;
};

class Extent {
public:
    Extent(float = 0, float = 0, float = 0, float = 0, float = 0);
    Extent(Extent&);

    bool Undefined() { return _left == _cx && _bottom == _cy; }
    bool Within(Extent& e);
    void Merge(Extent&);
public:
    /* defines lower left and center of an object */
    float _left, _bottom, _cx, _cy, _tol;
};
/* if needed...
void ArrayCopy (const float* x, const float* y, int n, float* newx, float* newy);

void ArrayDup (const float* x, const float* y, int n, float*& newx, float*& newy);
*/


/*
 * inlines
 */

inline void exch (int& a, int& b) {
    int temp = a;
    a = b;
    b = temp;
}

// calc equals within tolerance
bool equal(float a, float b, float tol) {
  return (fabs(a - b) <= tol);
}

inline int square(int a) { return a *= a; }
inline float square(float a) { return a *= a; }

inline float degrees(float rad) { return rad * 180.0 / M_PI; }
inline float radians(float deg) { return deg * M_PI / 180.0; }

inline float Distance(float x0, float y0, float x1, float y1) {
    return sqrt(float(square(x0 - x1) + square(y0 - y1)));
}

inline void Midpoint (
    double x0, double y0, double x1, double y1, double& mx, double& my
) {
    mx = (x0 + x1) / 2.0;
    my = (y0 + y1) / 2.0;
}

inline void ThirdPoint (
    double x0, double y0, double x1, double y1, double& tx, double& ty
) {
    tx = (2*x0 + x1) / 3.0;
    ty = (2*y0 + y1) / 3.0;
}

#endif
