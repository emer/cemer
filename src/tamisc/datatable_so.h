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



#ifndef DATATABLE_SO_H
#define DATATABLE_SO_H

#include "tamisc_def.h"
#include "t3node_so.h"

// externals
class SbVec3f; // #IGNORE
class SoAction; // #IGNORE
class SoBaseColor; // #IGNORE
class SoCube; // #IGNORE
class SoFont; // #IGNORE

// forwards
class T3TableViewNode;
class T3GridTableViewNode;
class T3GraphTableViewNode; //

//////////////////////////
//   T3TableViewNode	//
//////////////////////////

/* Base class for 3d table views

      shapeSeparator: SoSeparator
        txfm_shape: SoTransform
        material: SoMaterial
        frame: SoFrame (note: existence/display is modal)
      canvasSeparator: SoSeparator
        txfm_canvas: SoTransform -- translates/scales so canvas is exactly geom dimensions
        canvas: SoSeparator
*/

class TAMISC_API T3TableViewNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  SO_NODE_HEADER(T3TableViewNode);
#endif // def __MAKETA__
public:
  static void		initClass();

  SoFrame*		frame() {return frame_;} // note: may be null
  bool			showFrame();
  virtual void		setShowFrame(bool value, float inset = 0.05f);
    // note: inset ignored when value=false
  void 			setGeom(int px, int py, int pz); // sets (actual) geom of view
  void 			setGeom(int px, int py, int pz, float inset); 
   // sets (actual) geom of view, including inset (if applicable)
  T3TableViewNode(void* dataView_ = NULL); // dataview is a TableView object

protected:
  iVec3i		geom_; //note, not a field
  virtual void		render(float inset); // called after geom and other changes
  ~T3TableViewNode();

private:
  SoFrame*		frame_; // NULL if not shown
};

//////////////////////////
//   T3GridTableViewNode	//
//////////////////////////

/* 3d grid and text views
     [...]
        header: SoGroup
        body: SoGroup
*/

class TAMISC_API T3GridTableViewNode: public T3TableViewNode {
#ifndef __MAKETA__
typedef T3TableViewNode inherited;

  SO_NODE_HEADER(T3GridTableViewNode);
#endif // def __MAKETA__
public:
  static void		initClass();

  SoSeparator*		grid() {return grid_;}
  SoGroup*		header() {return header_;}
  SoGroup*		body() {return body_;}

  T3GridTableViewNode(void* dataView_ = NULL); // dataview is a TextTableView object

protected:
  ~T3GridTableViewNode();
  SoSeparator*		grid_;
  SoGroup*		header_;
  SoGroup*		body_;//
};


//////////////////////////
//   T3GraphTableViewNode	//
//////////////////////////

/* 3d grid and text log views
     [...]
      canvasSeparator: SoSeparator
        txfm_canvas: SoTransform -- translates/scales so canvas is exactly geom dimensions
        canvas: SoSeparator
          header: SoGroup
          body: SoGroup
*/

class TAMISC_API T3GraphTableViewNode: public T3TableViewNode {
#ifndef __MAKETA__
typedef T3TableViewNode inherited;

  SO_NODE_HEADER(T3GraphTableViewNode);
#endif // def __MAKETA__
public:
  static void		initClass();

/*TODO: graphs, axes, body, etc.  SoGroup*		header() {return header_;}
  SoGroup*		body() {return body_;} */

  T3GraphTableViewNode(void* dataView_ = NULL); // dataview is a TextTableView object

protected:
  ~T3GraphTableViewNode();
//  SoGroup*		header_;
//  SoGroup*		body_;
};


#endif

