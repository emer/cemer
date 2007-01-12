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
class SoCalculator; // #IGNORE
class SoTransformBoxDragger; // #IGNORE

// forwards
class T3GridViewNode;
class T3GraphViewNode; //

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
  static float 		drag_size; // = .04 size of dragger control object
  static float 		frame_margin; // = .05 size of margin around stage for frame
  static float 		frame_width; // = .02 width of frame itself
  static void		initClass();

  SoFrame*		frame() const {return frame_;} 
  SoGroup*		grid() const {return grid_;}
  SoMaterial*		matStage() const {return mat_stage_;}
  SoSeparator*		header() const {return header_;}
  SoSeparator*		body() const {return body_;}

  virtual void		render();
  virtual void		setWidth(float wdth);
  virtual float		getWidth() { return width_; }

  T3GridViewNode(void* dataView_ = NULL, float wdth=1.0f); // dataview is a GridTableView object
protected:
  float			width_;
  SoSeparator*		stage_;
  SoMaterial*		mat_stage_;
  SoTranslation*	  txlt_stage_;
  SoSeparator*		  header_;
  SoSeparator*		  body_;//
  SoFrame*		frame_; 
  SoTranslation*	txlt_grid_;
  SoGroup*		grid_;

  SoSeparator*		drag_sep_;
  SoTransform*		drag_xf_;
  SoTransformBoxDragger* dragger_;
  SoCalculator*		drag_trans_calc_;

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

