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



#ifndef TA_DATATABLE_SO_H
#define TA_DATATABLE_SO_H

#include "ta_def.h"
#include "t3node_so.h"

// externals
class SbVec3f; // #IGNORE
class SoAction; // #IGNORE
class SoBaseColor; // #IGNORE
class SoCube; // #IGNORE
class SoFont; // #IGNORE

// forwards
class T3TableViewNode;
class T3GridViewNode;
class T3GraphViewNode; //

/* Base class for 3d table views
  legend: *-existing; +-new this class

      *transform: SoTransform
      *captionSeparator: SoSeparator  
            shapeSeparator: SoSeparator
        *txfm_shape: SoTransform
        *material: SoMaterial
        +frame: SoFrame
        +grid: SoGroup -- for grid lines (thus, inherits frame material)
*/

class TA_API T3TableViewNode: public T3NodeParent {
// ********** OBSOLETE ******************
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
//   T3GridViewNode	//
//////////////////////////

/* 3d grid views
  legend: *-existing; +-new this class

      *transform: SoTransform (for translating to pos)
      +stage: SoSeparator -- actual area of the grid
        +mat_stage: SoMaterial -- for setting the font color 
        +txlt_stage: SoTranslation -- sets origin to upper-left of stage
        +head: SoSeparator -- for header objects
        +body: SoSeparator -- for body objects
      *captionSeparator: SoSeparator  
            shapeSeparator: SoSeparator
      *txfm_shape: SoTransform
      *material: SoMaterial
      +frame: SoFrame
      +txlt_grid: SoTranslation -- sets origin to upper-left of stage
      +grid: SoGroup -- for grid lines (thus, inherits frame material)
       
*/

class TA_API T3GridViewNode: public T3NodeLeaf {
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;

  SO_NODE_HEADER(T3GridViewNode);
#endif // def __MAKETA__
public:
  static void		initClass();

  SoFrame*		frame() const {return frame_;} 
  SoGroup*		grid() const {return grid_;}
  SoMaterial*		matStage() const {return mat_stage_;}
  SoSeparator*		header() const {return header_;}
  SoSeparator*		body() const {return body_;}

  void			setInset(float inset = 0.05f);
  void 			setGeom(int px, int py); // sets (actual) geom of view
  void 			setGeom(int px, int py, float inset); 
   // sets (actual) geom of view, including inset (if applicable)
  T3GridViewNode(void* dataView_ = NULL); // dataview is a GridTableView object

protected:
  iVec2i		geom_; //note, not a field
  SoSeparator*		stage_;
  SoMaterial*		mat_stage_;
  SoTranslation*	  txlt_stage_;
  SoSeparator*		  header_;
  SoSeparator*		  body_;//
  SoFrame*		frame_; 
  SoTranslation*	txlt_grid_;
  SoGroup*		grid_;
  virtual void		render(float inset); // called after geom and other changes
  ~T3GridViewNode();
};


//////////////////////////
//   T3GraphViewNode	//
//////////////////////////

/* 3d grid and text log views
     [...]
      canvasSeparator: SoSeparator
        txfm_canvas: SoTransform -- translates/scales so canvas is exactly geom dimensions
        canvas: SoSeparator
          header: SoGroup
          body: SoGroup
*/

class TA_API T3GraphViewNode: public T3NodeParent {
#ifndef __MAKETA__
typedef T3NodeParent inherited;

  SO_NODE_HEADER(T3GraphViewNode);
#endif // def __MAKETA__
public:
  static void		initClass();

/*TODO: graphs, axes, body, etc.  SoGroup*		header() {return header_;}
  SoGroup*		body() {return body_;} */

  T3GraphViewNode(void* dataView_ = NULL); // dataview is a TextTableView object

protected:
  ~T3GraphViewNode();
//  SoGroup*		header_;
//  SoGroup*		body_;
};


#endif

