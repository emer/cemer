// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



// netstru_qtso.h -- qt and inventor controls/objects for network structures

// NOTE: functions inside "ifdef GUI" blocks are implemented in netstru_qtso.cc

#ifndef NETSTRU_SO_H
#define NETSTRU_SO_H

#include "pdp_def.h"
#include "t3node_so.h"

// externals
class SbVec3f; // #IGNORE
class SoAction; // #IGNORE
class SoBaseColor; // #IGNORE
class SoCone; // #IGNORE
class SoCube; // #IGNORE
class SoCylinder; // #IGNORE
class SoFont; // #IGNORE
class SoIndexedTriangleStripSet; // #IGNORE

// forwards
class T3UnitNode;
class T3UnitGroupNode;
class T3LayerNode;
class T3PrjnNode;
class T3NetNode;


//////////////////////////
//   T3UnitNode		//
//////////////////////////

/*
    this: SoSeparator - the object itself
      transform: SoTransform
      material: SoMaterial
      [drawStyle: SoDrawStyle] (inserted when picked, to make wireframe instead of solid)
      shape: SoCylinder (could be changed to something else)
*/

class PDP_API T3UnitNode: public T3NodeLeaf { //
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;

  SO_NODE_ABSTRACT_HEADER(T3UnitNode);
#endif // def __MAKETA__
friend class T3UnitGroupNode;
public:
  static float	base_height; // #DEF_0.1 height when not active or empty
  static float	max_height; // #DEF_0.8 height when fully active

  static void	initClass();

  virtual void	setAppearance(float act, const T3Color& color, float max_z, float trans);
  // act is -1:1; max_z is net->max_size.z; trans is transparency
  virtual void 	setPicked(bool value);
  T3UnitNode(void* dataView_ = NULL, float max_x = 1.0f, float max_y = 1.0f,
	     float max_z = 1.0f, float un_spc = .01f);

protected:
  float			spacing;		      // unit spacing
  void			setDefaultCaptionTransform(); // override, sets text justif and transform for 3D
  ~T3UnitNode();
};

/*TODO
class PDP_API T3UnitNode_Color: public T3UnitNode { // 2d color
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;

  SO_NODE_HEADER(T3UnitNode);
#endif // def __MAKETA__
friend class T3UnitGroupNode;
public:
  static float	base_height; // #DEF_0.1 height when not active or empty
  static float	max_height; // #DEF_0.8 height when fully active

  static void	initClass();

  SoCylinder*	shape() {return shape_;}
  void		getPos(int& x, int& y);
  void 		setPos(int x, int y); // set relative position within unitgroup

  void		setAppearance(float act, const T3Color& color, float max_z, float trans);
  T3UnitNode(void* dataView_ = NULL, float max_x = 1.0f, float max_y = 1.0f,
  float max_z = 1.0f, float un_spc = .01f);

protected:
  ~T3UnitNode();
private:
  SoCylinder*		shape_; //#IGNORE
}; */


class PDP_API T3UnitNode_Cylinder: public T3UnitNode { // 2d color
#ifndef __MAKETA__
typedef T3UnitNode inherited;
  SO_NODE_HEADER(T3UnitNode_Cylinder);
#endif // def __MAKETA__
friend class T3UnitGroupNode;
public:
  static void	initClass();

  SoCylinder*	shape() {return shape_;}

  void		setAppearance(float act, const T3Color& color, float max_z, float trans);

  T3UnitNode_Cylinder(void* dataView_ = NULL, float max_x = 1.0f, float max_y = 1.0f,
		      float max_z = 1.0f, float un_spc = .01f);

protected:
  ~T3UnitNode_Cylinder();
private:
  SoCylinder*		shape_; //#IGNORE
};

class PDP_API T3UnitNode_Circle: public T3UnitNode { // 2d color
#ifndef __MAKETA__
typedef T3UnitNode inherited;
  SO_NODE_HEADER(T3UnitNode_Circle);
#endif // def __MAKETA__
friend class T3UnitGroupNode;
public:
  static void	initClass();

  SoCylinder*	shape() {return shape_;}

  T3UnitNode_Circle(void* dataView_ = NULL, float max_x = 1.0f, float max_y = 1.0f,
		    float max_z = 1.0f, float un_spc = .01f);

protected:
//  void			setDefaultCaptionTransform(); // override
  ~T3UnitNode_Circle();
private:
  SoCylinder*		shape_; //#IGNORE
};


class PDP_API T3UnitNode_Block: public T3UnitNode { // 2d color
#ifndef __MAKETA__
typedef T3UnitNode inherited;
  SO_NODE_HEADER(T3UnitNode_Block);
#endif // def __MAKETA__
friend class T3UnitGroupNode;
public:
  static void	initClass();

  SoCube*	shape() {return shape_;}

  void		setAppearance(float act, const T3Color& color, float max_z, float trans);
  T3UnitNode_Block(void* dataView_ = NULL, float max_x = 1.0f, float max_y = 1.0f,
		   float max_z = 1.0f, float un_spc = .01f);

protected:
  ~T3UnitNode_Block();
private:
  SoCube*		shape_; //#IGNORE
};

class PDP_API T3UnitNode_Rect: public T3UnitNode { // 2d color
#ifndef __MAKETA__
typedef T3UnitNode inherited;
  SO_NODE_HEADER(T3UnitNode_Rect);
#endif // def __MAKETA__
friend class T3UnitGroupNode;
public:
  static void		initClass();

  SoCube*		shape() {return shape_;}

  T3UnitNode_Rect(void* dataView_ = NULL, float max_x = 1.0f, float max_y = 1.0f,
		  float max_z = 1.0f, float un_spc = .01f);

protected:
//  void			setDefaultCaptionTransform(); // override
  ~T3UnitNode_Rect();
private:
  SoCube*		shape_; //#IGNORE
};


//////////////////////////
//   T3UnitGroupNode	//
//////////////////////////

class PDP_API T3UnitGroupNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  SO_NODE_HEADER(T3UnitGroupNode);
#endif // def __MAKETA__
public:
  static float		height; // nominal amount of height, so doesn't vanish
  static float		inset; // amount inset from the layer

  static void		initClass();
  static void		shapeCallback(void* data, SoAction* act);
  static void		drawGrid(T3UnitGroupNode* node);

//  void			clearUnits(); // remove all units
//  void			addUnit(int x, int y); // remove all units
  SoFont*		unitCaptionFont(bool auto_create = false);

  void 			setGeom(int px, int py, float max_x, float max_y, float max_z);
  // sets (actual) geom of group; creates/positions units; setes max_size
  SoIndexedTriangleStripSet* shape() {return shape_;}
  SoSeparator*		unitText() {return unit_text_;} // extra text of unit vars
  SoSeparator*		getUnitText();		      // get a new unit text separator
  SoSeparator*		removeUnitText();	      // remove unit text separator

  T3UnitGroupNode(void* dataView_ = NULL, bool no_unts = false);

protected:
  iVec2i		geom; //note, not a field
  iVec3f		max_size; // maximum size of network x,y,z
  SoFont*		unitCaptionFont_;
  bool			no_units; // summary mode: no unit objects exist
  ~T3UnitGroupNode();

private:
  SoIndexedTriangleStripSet*	shape_;
  SoSeparator* 		unit_text_; // unit text variables
};



//////////////////////////
//   T3LayerNode	//
//////////////////////////


class PDP_API T3LayerNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  SO_NODE_HEADER(T3LayerNode);
#endif // def __MAKETA__

public:
  static float 		height; // height of the layer shape itself

  static void		initClass();

  void 			setGeom(int px, int py, float max_x, float max_y, float max_z);
  // sets (actual) geom of layer
//  SoCube*		shape() {return shape_;}

  T3LayerNode(void* dataView_ = NULL);

protected:
  iVec2i		geom; //note, not a field
  iVec3f		max_size; // maximum size of network x,y,z
  void			render(); // called after pos/geom changes
  ~T3LayerNode();

private:
//  SoCube*		shape_; //#IGNORE

//  SoCube* 		shapes_[4];
//  SoTransform* 		shapeTrsfm_[4];
  SoFrame*		shape_;
};



//////////////////////////
//   T3PrjnNode		//
//////////////////////////

class PDP_API T3PrjnNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  SO_NODE_HEADER(T3PrjnNode);
#endif // def __MAKETA__
public:
  static void		initClass();

  void			setEndPoint(const SbVec3f& value); // #IGNORE sets endpoint, relative to its origin

  T3PrjnNode(void* dataView_ = NULL, float rad = .01f);

protected:
  float			radius;
  SoTransform*		trln_prjn; // #IGNORE
  SoTransform*		rot_prjn; // #IGNORE
  SoTransform*		trln_arr; // #IGNORE
  SoCone*		arr_prjn;  // #IGNORE arrow head
  SoCylinder*		line_prjn;  // #IGNORE line

  ~T3PrjnNode();
private:
  void			init();
};



class PDP_API T3NetNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  SO_NODE_HEADER(T3NetNode);

#endif // def __MAKETA__

public:
  static void		initClass();

  SoFrame*		shape() {return shape_;}
  SoSeparator*		netText() {return net_text_;} // extra text of network vars etc
  SoSeparator*		getNetText();		      // get a new net text separator

  T3NetNode(void* dataView_ = NULL);

protected:
  void		setDefaultCaptionTransform(); // override
  ~T3NetNode();

private:
  SoFrame*		shape_; //#IGNORE
  SoSeparator* 		net_text_; // network text variables
};



#endif // NETSTRU_SO_H

