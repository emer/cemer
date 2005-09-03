/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// netstru_qtso.h -- qt and inventor controls/objects for network structures

// NOTE: functions inside "ifdef GUI" blocks are implemented in netstru_qtso.cc

#ifndef NETSTRU_SO_H
#define NETSTRU_SO_H

#include "t3node_so.h"

// externals
class SbVec3f; // #IGNORE
class SoAction; // #IGNORE
class SoBaseColor; // #IGNORE
class SoCone; // #IGNORE
class SoCube; // #IGNORE
class SoCylinder; // #IGNORE
class SoFont; // #IGNORE

// forwards
class T3UnitNode;
class T3UnitGroupNode;
class T3LayerNode;
class T3PrjnNode;
class T3NetNode;

class T3LogViewNode;
class T3GridLogViewBaseNode;


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

class T3UnitNode: public T3NodeLeaf { //
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;

  SO_NODE_ABSTRACT_HEADER(T3UnitNode);
#endif // def __MAKETA__
friend class T3UnitGroupNode;
public:
  static float		base_height; // #DEF_0.1 height when not active or empty
  static float		max_height; // #DEF_0.8 height when fully active

  static void		initClass();

  virtual void		setAppearance(float act, const T3Color& color);  // act is -1:1
  virtual void 		setPicked(bool value);
  T3UnitNode(void* dataView_ = NULL);

protected:
  void			setDefaultCaptionTransform(); // override, sets text justif and transform for 3D
  ~T3UnitNode();
};

/*TODO
class T3UnitNode_Color: public T3UnitNode { // 2d color
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;

  SO_NODE_HEADER(T3UnitNode);
#endif // def __MAKETA__
friend class T3UnitGroupNode;
public:
  static float		base_height; // #DEF_0.1 height when not active or empty
  static float		max_height; // #DEF_0.8 height when fully active

  static void		initClass();

  SoCylinder*		shape() {return shape_;}
  void			getPos(int& x, int& y);
  void 			setPos(int x, int y); // set relative position within unitgroup

  void			setAppearance(float act, const T3Color& color);  // act is 0-1
  T3UnitNode(void* dataView_ = NULL);

protected:
  ~T3UnitNode();
private:
  SoCylinder*		shape_; //#IGNORE
}; */


class T3UnitNode_Cylinder: public T3UnitNode { // 2d color
#ifndef __MAKETA__
typedef T3UnitNode inherited;
  SO_NODE_HEADER(T3UnitNode_Cylinder);
#endif // def __MAKETA__
friend class T3UnitGroupNode;
public:
  static float		shape_radius; //  radius (also used for _Circle)

  static void		initClass();

  SoCylinder*		shape() {return shape_;}

  void			setAppearance(float act, const T3Color& color); // override
  T3UnitNode_Cylinder(void* dataView_ = NULL);

protected:
  ~T3UnitNode_Cylinder();
private:
  SoCylinder*		shape_; //#IGNORE
};

class T3UnitNode_Circle: public T3UnitNode { // 2d color
#ifndef __MAKETA__
typedef T3UnitNode inherited;
  SO_NODE_HEADER(T3UnitNode_Circle);
#endif // def __MAKETA__
friend class T3UnitGroupNode;
public:
  static void		initClass();

  SoCylinder*		shape() {return shape_;}

  T3UnitNode_Circle(void* dataView_ = NULL);

protected:
//  void			setDefaultCaptionTransform(); // override
  ~T3UnitNode_Circle();
private:
  SoCylinder*		shape_; //#IGNORE
};


class T3UnitNode_Block: public T3UnitNode { // 2d color
#ifndef __MAKETA__
typedef T3UnitNode inherited;
  SO_NODE_HEADER(T3UnitNode_Block);
#endif // def __MAKETA__
friend class T3UnitGroupNode;
public:
  static float		shape_width; //  (also used for _Rect)
  static float		shape_depth; //  (also used for _Rect)

  static void		initClass();

  SoCube*		shape() {return shape_;}

  void			setAppearance(float act, const T3Color& color); // override
  T3UnitNode_Block(void* dataView_ = NULL);

protected:
  ~T3UnitNode_Block();
private:
  SoCube*		shape_; //#IGNORE
};

class T3UnitNode_Rect: public T3UnitNode { // 2d color
#ifndef __MAKETA__
typedef T3UnitNode inherited;
  SO_NODE_HEADER(T3UnitNode_Rect);
#endif // def __MAKETA__
friend class T3UnitGroupNode;
public:
  static void		initClass();

  SoCube*		shape() {return shape_;}

  T3UnitNode_Rect(void* dataView_ = NULL);

protected:
//  void			setDefaultCaptionTransform(); // override
  ~T3UnitNode_Rect();
private:
  SoCube*		shape_; //#IGNORE
};


//////////////////////////
//   T3UnitGroupNode	//
//////////////////////////

class T3UnitGroupNode: public T3NodeParent {
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

  void 			setGeom(int px, int py); // sets (actual) geom of group; creates/positions units;
  SoCube*		shape() {return shape_;}

  T3UnitGroupNode(void* dataView_ = NULL);

protected:
  iVec2i		geom; //note, not a field
  SoFont*		unitCaptionFont_;
  ~T3UnitGroupNode();

private:
  SoCube*		shape_; // #IGNORE
  SoGroup*		units_; // #IGNORE
};



//////////////////////////
//   T3LayerNode	//
//////////////////////////


class T3LayerNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  SO_NODE_HEADER(T3LayerNode);
#endif // def __MAKETA__

public:
  static float 		height; // height of the layer shape itself

  static void		initClass();

  void 			setGeom(int px, int py); // sets (actual) geom of layer
//  SoCube*		shape() {return shape_;}

  T3LayerNode(void* dataView_ = NULL);

protected:
  iVec2i		geom; //note, not a field
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

class T3PrjnNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  SO_NODE_HEADER(T3PrjnNode);
#endif // def __MAKETA__
public:
  static void		initClass();

  void			setEndPoint(const SbVec3f& value); // #IGNORE sets endpoint, relative to its origin

  T3PrjnNode(void* dataView_ = NULL);

protected:
  SoTransform*		trln_prjn; // #IGNORE
  SoTransform*		rot_prjn; // #IGNORE
  SoTransform*		trln_arr; // #IGNORE
  SoCone*		arr_prjn;  // #IGNORE arrow head
  SoCylinder*		line_prjn;  // #IGNORE line

  ~T3PrjnNode();
private:
  void			init();
};



class T3NetNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  SO_NODE_HEADER(T3NetNode);

#endif // def __MAKETA__

public:
  static void		initClass();

  SoCube*		shape() {return shape_;}

  T3NetNode(void* dataView_ = NULL);

protected:
  void		setDefaultCaptionTransform(); // override
  ~T3NetNode();

private:
  SoCube*		shape_; //#IGNORE
};


//////////////////////////
//   T3LogViewNode	//
//////////////////////////

/* Base class for 3d log views

      shapeSeparator: SoSeparator
        txfm_shape: SoTransform
        material: SoMaterial
        frame: SoFrame (note: existence/display is modal)
      canvasSeparator: SoSeparator
        txfm_canvas: SoTransform -- translates/scales so canvas is exactly geom dimensions
        canvas: SoSeparator
*/

class T3LogViewNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  SO_NODE_HEADER(T3LogViewNode);
#endif // def __MAKETA__
public:
  static void		initClass();

  SoSeparator*		canvas() {return canvas_;} // note: transformed to compensate for frame inset, if any
  SoFrame*		frame() {return frame_;} // note: may be null
  bool			showFrame();
  virtual void		setShowFrame(bool value);
  void 			setGeom(int px, int py, int pz); // sets (actual) geom of log
  T3LogViewNode(void* dataView_ = NULL); // dataview is a LogView object

protected:
  iVec3i		geom_; //note, not a field
  SoSeparator*		canvas_;
#ifndef __MAKETA__
#endif
  virtual void		render(); // called after geom and other changes
  ~T3LogViewNode();

private:
  SoFrame*		frame_; // NULL if not shown
  SoSeparator*		canvasSeparator_;
  SoTransform*		txfm_canvas_;
};

//////////////////////////
//   T3GridLogViewBaseNode	//
//////////////////////////

/* 3d grid and text log views
     [...]
      canvasSeparator: SoSeparator
        txfm_canvas: SoTransform -- translates/scales so canvas is exactly geom dimensions
        canvas: SoSeparator
          header: SoGroup
          body: SoGroup
*/

class T3GridLogViewBaseNode: public T3LogViewNode {
#ifndef __MAKETA__
typedef T3LogViewNode inherited;

  SO_NODE_HEADER(T3GridLogViewBaseNode);
#endif // def __MAKETA__
public:
  static void		initClass();

  SoGroup*		header() {return header_;}
  SoGroup*		body() {return body_;}

  T3GridLogViewBaseNode(void* dataView_ = NULL); // dataview is a TextLogView object

protected:
  ~T3GridLogViewBaseNode();
  SoGroup*		header_;
  SoGroup*		body_;
};


//////////////////////////
//   T3GraphLogViewNode	//
//////////////////////////

/* 3d grid and text log views
     [...]
      canvasSeparator: SoSeparator
        txfm_canvas: SoTransform -- translates/scales so canvas is exactly geom dimensions
        canvas: SoSeparator
          header: SoGroup
          body: SoGroup
*/

class T3GraphLogViewNode: public T3LogViewNode {
#ifndef __MAKETA__
typedef T3LogViewNode inherited;

  SO_NODE_HEADER(T3GraphLogViewNode);
#endif // def __MAKETA__
public:
  static void		initClass();

/*TODO: graphs, axes, body, etc.  SoGroup*		header() {return header_;}
  SoGroup*		body() {return body_;} */

  T3GraphLogViewNode(void* dataView_ = NULL); // dataview is a TextLogView object

protected:
  ~T3GraphLogViewNode();
//  SoGroup*		header_;
//  SoGroup*		body_;
};




#endif // NETSTRU_SO_H

