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

#ifndef T3NODE_SO_H
#define T3NODE_SO_H

#include "icolor.h"
#include "igeometry.h"
#include "ta_matrix.h"
#include "colorscale.h"

#include "ta_def.h"

#ifdef __MAKETA__
//dummy defines
#define SoSFEnum int
#define SoSFVec2f int
#define uint32_t uint

#else
#include <Inventor/SbColor.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFVec2f.h>
#include <Inventor/fields/SoMFVec3f.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoImage.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTriangleStripSet.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>
#endif

// forwards
class T3Node;

// externals -- note: some are here for use by other files, not this one
class SbColor; // #IGNORE
class SoCoordinate3; // #IGNORE
class SbRotation; // #IGNORE
class SbVec3f; // #IGNORE
class SoAsciiText; // #IGNORE
class SoDrawStyle; // #IGNORE
class SoFont; // #IGNORE
class SoLightModel; // #IGNORE
class SoLineSet; // #IGNORE
class SoMarkerSet; // #IGNORE
class SoMaterial; // #IGNORE
class SoMFVec3f; // #IGNORE
class SoNode; // #IGNORE
class SoPackedColor; // #IGNORE
class SoPerspectiveCamera; // #IGNORE
class SoPickStyle; // #IGNORE
class SoRotation; // #IGNORE
class SoScale; // #IGNORE
class SoSeparator; // #IGNORE
class SoTexture2; // #IGNORE
class SoTexture2Transform; // #IGNORE
class SoTranslate1Dragger; // #IGNORE
class SoTranslation; // #IGNORE
class SoTransform; // #IGNORE
class SoTriangleStripSet; // #IGNORE
class SoIndexedTriangleStripSet; // #IGNORE
class SoUnits; // #IGNORE
class SoVertexProperty; // #IGNORE

//////////////////////////
//   T3Color		//
//////////////////////////

// yet **another** color class!! but SbColor is not very convenient or intuitive, so
// we use this to facilitate api simplification

class TA_API T3Color { // ##NO_INSTANCE ##NO_TOKENS
public:
#ifdef __MAKETA__
  float 	r;
  float 	g;
  float 	b;
#else
  union {
    struct {
      float 	r;
      float 	g;
      float 	b;
    };
    float	rgb[3];
  };
#endif
  static uint32_t makePackedRGBA(float r_, float g_, float b_, float a_ = 1.0f); // #IGNORE
  static uint32_t makePackedRGBA(int r_, int g_, int b_, int a_ = 255); // #IGNORE

  void		setValue(float r_, float g_, float b_) {r = r_; g = g_; b = b_;}
  T3Color() {r = g = b = 0.0f;}
  T3Color(float r_, float g_, float b_) {r = r_; g = g_; b = b_;}
  T3Color(float x_) {r = x_; g = x_; b = x_;}
  T3Color(const iColor* cp) {if (cp) cp->getRgb(r, g, b); else r = g = b = 0.0f;}
  T3Color(const iColor& cp) {cp.getRgb(r, g, b);}
  T3Color(const SbColor& cp) {cp.getValue(r, g, b);}

  T3Color& operator =(const SbColor& cp) {cp.getValue(r, g, b); return *this;}
  T3Color& operator =(const iColor& cp) {cp.getRgb(r, g, b); return *this;}
  T3Color& operator =(float x_) {r = x_; g = x_; b = x_; return *this;}
  operator SbColor() const {return SbColor(rgb);}
};



/*
  All Nodes are implemented using an approach that is similar to, but simpler than,
  Inventor Node Kits. We expose the contents as properties (ex. drawStyle()) -- properties
  may be always created (ex in constructor) or only created on demand. The properties
  themselves are stored in fields, so they automatically participate in copying, streaming,
  etc.

  Coordinate Space

  The coordinate space for "pos" and "geom" calls follows the PDP conventions. The values
  are in integral units in some abstract 3-d space; the frame of reference is as follows:

  Dimension	PDP	Inventor
  left-->right	+x	+x
  bott->top	+y	+y
  back->front	-z	+z

  Because of how layers are stacked, the PDP-y space is scaled with respect to the other
  spaces. However, this is not done with transforms (it would distort shapes), but merely
  by scaling the PDP y factor.

  The letter 'p' in front of coordinate items (px, py, etc.) means PDP coordinates.

  TODO:

  Loading will be an issue, since the member object fields will need to be set to the
  child instances loaded. Perhaps readInstance() can be overloaded as follows:
      clear children
      set member objects to NULL (release)
      inherited::readInstance()
      set member object references by looking for them in the children() collection
*/


//////////////////////////
//   T3Node		//
//////////////////////////

/*
  There are two levels of complexity for T3Nodes, one for terminals, and one for non-terminals.

  [] indicates optional node


T3NodeLeaf: for terminal items (no children)

  this: SoSeparator (aka "topSeparator", also aliased as "shapeSeparator")
      transform: SoTransform
      [captionSeparator]: SoSeparator (node and subnodes only created if non-blank caption set)
        [captionFont]: SoFont - only created if accessed (otherwise will inherit from parent)
        captionTransform: SoTransform
        captionNode: SoAsciiText
      txfm_shape: SoTransform
      material: SoMaterial (if any)
      shape: (type-specific shape)


T3NodeParent:

  this: SoSeparator (aka "topSeparator")
      transform: SoTransform
      shapeSeparator: SoSeparator
        txfm_shape: SoTransform
        material: SoMaterial
        shape: (type-specific shape, if any)
      [captionFont]: SoFont - only created if accessed -- NOTE: inherited by childNodes
      [captionSeparator]: SoSeparator (node and subnodes only created if non-blank caption set)
        captionTransform: SoTransform
        captionNode: SoAsciiText
      [class-dependent child node modifiers, inserted here, before childNodes]
      childNodes: SoSeparator -- the T3Node children, if any -- AUTOCREATED

NOTES:
  * the 'font' node controls the caption font, hierarchically -- therefore, if you
    need to use text for some other purpose, you must put that in your shape
    separator, and put its own font node in
  * the caption-related nodes are not directly accessible -- use the property api's
  * note the slightly different placement for the 'font' node in Parent vs. Leaf
  * a caption can have a default rotation and translation; you can explicitly set or override this
  * caption modifications are limited to:
      - rotation around the origin
      - translation of resulting rotated caption
  * clear() does not operate on children, because the T3DataView hierarchy already
      does this

NOTE: T3Node may be changed to look like this -- this change will be transparent
  if all access to the sub-items goes through topSeparator()

  this: SoGroup
    callbackList: SoSeparator
    topSeparator: SoSeparator
      transform: SoTransform



*/
class TA_API T3Node: public SoSeparator {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS  an base class for PDP project items, like networks, etc.
#ifndef __MAKETA__
typedef SoSeparator inherited;

  SO_NODE_ABSTRACT_HEADER(T3Node);
#endif // def __MAKETA__
public:

  static void		initClass();

  static void		insertChildBefore(SoGroup* group, SoNode* child, SoNode* before);
   // insert before node; before=NULL for end;
  static void		insertChildAfter(SoGroup* group, SoNode* child, SoNode* after);
   // insert before node; after=NULL for start
  static SoNode*	getNodeByName(SoGroup* group, const char* name); // find node by name, if any

  inline void* 		dataView() {return dataView_;} // #IGNORE the T3DataView that owns/created this node

  virtual SoFont*	captionFont(bool auto_create = false) = 0;
  SoAsciiText*		captionNode(bool auto_create = false);
  virtual SoSeparator*	topSeparator() {return this;}
  SoTransform*		transform() const {return transform_;} // the master transform, for the whole entity
  virtual SoSeparator*	shapeSeparator() = 0;
  SoTransform*		txfm_shape() const {return txfm_shape_;} 
    // the transform for the shape
  SoMaterial*		material() const {return material_;}
  virtual SoSeparator*	childNodes() {return NULL;} // use this to set/get T3Node children

  virtual const char*	caption();
  virtual void		setCaption(const char* value); //NOTE: if you want to transform, you MUST call transformCaption every time after calling setCaption

  virtual void		clear() {} // optional method, for clearing out the content; called from ReInit
  void			transformCaption(const iVec3f& translate); // #IGNORE
  void			transformCaption(const SbRotation& rotate, const iVec3f& translate); // #IGNORE
  void			resizeCaption(float sz);
  virtual bool		expanded() {return true;} // many nodes have an expanded and compact rep
  virtual void		setExpanded(bool value) {}

  virtual void		addRemoveChildNode(SoNode* node, bool adding); // called by pdpDataView (default prints console error)

  T3Node(void* dataView_ = NULL);

protected:
  void* 		dataView_;
  SoAsciiText*		captionNode_;
  const char*  		getFileFormatName() const {return SoSeparator::getFileFormatName();} // override
  virtual SoSeparator*	captionSeparator(bool auto_create = false) = 0;
  SoTransform*		captionTransform(bool auto_create = false);
  virtual void		setDefaultCaptionTransform(); // call transformCaption to set the default transform; this is called after creating Node first time
  void			transformCaption(const SbRotation* rotate, const iVec3f* translate); // #IGNORE
  void 			initCommon(); // shared code, can only be called in a subclass constructor

  ~T3Node();

private:
  SoTransform*		transform_; // #IGNORE
  SoTransform*		txfm_shape_; // #IGNORE NOTE: must be created in subclass init
  SoMaterial*		material_; // #IGNORE NOTE: must be created in subclass init
};

class TA_API T3NodeLeaf: public T3Node {
  // ##NO_INSTANCE ##NO_TOKENS  an base class for PDP project items, like networks, etc.
#ifndef __MAKETA__
typedef T3Node inherited;

  SO_NODE_HEADER(T3NodeLeaf);
#endif // def __MAKETA__
public:
  static void		initClass();

  SoFont*		captionFont(bool auto_create = false); // override
  SoSeparator*		shapeSeparator() {return this;}


  T3NodeLeaf(void* dataView_ = NULL);

protected:
  SoSeparator*		captionSeparator(bool auto_create = false); // override

  ~T3NodeLeaf();
};


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

  T3NodeParent(void* dataView_ = NULL);

protected:
  SoSeparator*		captionSeparator(bool auto_create = false); // override

  ~T3NodeParent();
private:
  SoSeparator*		shapeSeparator_; // #IGNORE
  SoSeparator*		childNodes_; // #IGNORE
};

class TA_API SoFrame: public SoTriangleStripSet {
  // ##NO_INSTANCE ##NO_TOKENS quadraloidal frame
#ifndef __MAKETA__
typedef SoTriangleStripSet inherited;

  SO_NODE_HEADER(SoFrame);
#endif // def __MAKETA__
public:
  enum Orientation {
    Hor,	// lies flat in x-z plane
    Ver		// lies flat in x-y plan
  };

  static void		initClass();

  float		base; //note: these could be fields
  float		height;
  float		depth;
  float		inset; // #DEF_0.05
  Orientation	orientation; // #DEF_Hor

  SoMFVec3f& 	vertex(); //  #IGNORE accessor shortcut for vertices
  SoMFVec3f& 	normal(); // #IGNORE accessor shortcut for normals

  void		setDimensions(float bs, float ht, float dp); // use existing inset
  void		setDimensions(float bs, float ht, float dp, float in);
  void		setInset(float value);
  void		setOrientation(Orientation ori);
  SoFrame(Orientation ori = Hor, float in = 0.05f);

protected:
  const char*  	getFileFormatName() const {return SoTriangleStripSet::getFileFormatName();} // override
  void 		render(); // #IGNORE
  void 		renderH(); // #IGNORE
  void 		renderV(); // #IGNORE
};

class TA_API SoRect: public SoTriangleStripSet {
  // ##NO_INSTANCE ##NO_TOKENS  2d rectangle, primarily for images, table images, etc.
#ifndef __MAKETA__
typedef SoTriangleStripSet inherited;

  SO_NODE_HEADER(SoRect);
#endif // def __MAKETA__
public:
  static void		initClass();

  float		width;
  float		height;

  SoMFVec3f& 	vertex(); //  #IGNORE accessor shortcut for vertices
  SoMFVec3f& 	normal(); // #IGNORE accessor shortcut for normals

  void		setDimensions(float wd, float ht);
  SoRect();

protected:
  const char*  	getFileFormatName() const
  { return SoTriangleStripSet::getFileFormatName(); } // override
  void 		render(); // #IGNORE
  void 		renderH(); // #IGNORE
  void 		renderV(); // #IGNORE
};

class TA_API SoLineBox3d : public SoIndexedLineSet {
  // ##NO_INSTANCE ##NO_TOKENS a 3d box draw with lines, starting at 0,0,0 and going to 1,-1,1 (note: uses TA Y coords, not GL's)
#ifndef __MAKETA__
typedef SoIndexedLineSet inherited;

  SO_NODE_HEADER(SoLineBox3d);
#endif // def __MAKETA__
public:
  static void		initClass();

  float		width;		// x
  float		height;		// y
  float		depth;		// z

  void 		render();

  SoLineBox3d(float wd = 1.0f, float ht = 1.0f, float dp = 1.0f);
protected:
};

class TA_API SoImageEx: public SoSeparator { 
  // ##NO_INSTANCE ##NO_TOKENS taImage-compatible image viewer -- width will always be 1; height will then be h/w ratio
#ifndef __MAKETA__
typedef SoSeparator inherited;

  SO_NODE_HEADER(SoImageEx);
#endif // def __MAKETA__
public:
  static void		initClass();
  
  SoTexture2*		texture;
  
  void		setImage(const QImage& src);
  void		setImage(const taMatrix& src, bool top_zero = false);
    // gray: X,Y; rgb: X,Y,[rgb] -- tz false is normal convention for pdp
  
  SoImageEx();
protected:
  SoRect*		shape;
  byte_Matrix		img;
  iVec2i		d; // cached for clarity
  
  void		adjustScale(); // called after setting image to adjust aspect
  void		setImage2(const QImage& src);
  void		setImage3(const QImage& src);
  void		setImage2(const taMatrix& src, bool top_zero);
  void		setImage3(const taMatrix& src, bool top_zero);
  ~SoImageEx();
};

class TA_API SoMatrixGrid: public SoSeparator {
  // ##NO_INSTANCE ##NO_TOKENS renders a matrix as a grid of 3d blocks, in X-Y plane, with block height = Z axis.  size = 1x1 unit
#ifndef __MAKETA__
typedef SoSeparator inherited;

  SO_NODE_HEADER(SoMatrixGrid);
#endif // def __MAKETA__
public:
  enum MatrixLayout { 	// order of display for matrix cols
    BOT_ZERO, 		// row zero is displayed at bottom of cell (default)
    TOP_ZERO 		// row zero is displayed at top of cell (ex. for images)
  };

  static void		initClass();

  MatrixLayout	mat_layout; 	// #DEF_BOT_ZERO layout of matrix and image cells
  taMatrix*	matrix;	    	// the matrix to render (required!)
  bool		odd_y;		// how to deal with odd-dimensional matricies: put extra odd dimension in the y axis (else x)
  ColorScale*	scale; 		// The color scale for this display (required!)
  bool		val_text;	// display text representation of values?

  int		max_txt_len;	// #DEF_6 maximum text length (determines font size)
  float		spacing;	// #DEF_0.1 space between cell blocks, as a proportion of max of X, Y cell size
  float		block_height;	// #DEF_0.2 how tall (in Z dimension) to make the blocks (relative to the max of their X or Y size)
  float		trans_max;	// #DEF_0.6 maximum transparency value (for zero scaled values)

  void		setMatrix(taMatrix* mat, bool oddy);
  void		setColorScale(ColorScale* cs);
  void		setLayout(MatrixLayout layout);
  void		setValText(bool val_txt);
  void		setBlockHeight(float blk_ht);

  void 		render();
  // basic render -- generates static structures and then calls renderValues
  void		renderValues();
  // optimized render update for when only the matrix values have changed (matrix geom MUST not have changed)

  void 		ValToDispText(float val, String& str);
  // get text representation of value

  SoTransform*	transform() const { return transform_; }
  // the master transform, for the whole entity
  taMatrix*	getMatrix() const { return matrix; }

  SoMatrixGrid(taMatrix* mat = NULL, bool oddy = true, ColorScale* cs = NULL, MatrixLayout layout = BOT_ZERO, bool val_txt = false);
  ~SoMatrixGrid();

protected:
  SoIndexedTriangleStripSet* shape_;
  SoTransform*		transform_; // for entire object: goes first in this
  SoVertexProperty*	vtx_prop_;
  SoSeparator* 		cell_text_; // cell text variables
  SoFont* 		cell_fnt_; //fnt we add to the text (not refed)

  float		max_xy;		// max of unit x,y cell size
  float		cl_spc;		// computed cell spacing
  float		blk_ht;		// computed block height
  
  void		render_block_idx(int c00_0, int mat_idx, 
				 int32_t* coords_dat, int32_t* norms_dat,
				 int32_t* mats_dat, int& cidx, int& nidx, int& midx);
  // render one set of block indicies
  void		render_text(bool build_text, int& t_idx, float xp, float xp1, float yp,
			    float yp1, float zp);
  // setup all the cell_text_ stuff
};

// redefine various dragger controls!

class TA_API SoBigScaleUniformScaler: public SoSeparator { 
  // ##NO_INSTANCE ##NO_TOKENS big version of the scaleUniformScaler
#ifndef __MAKETA__
typedef SoSeparator inherited;
  SO_NODE_HEADER(SoBigScaleUniformScaler);
#endif // def __MAKETA__
public:
  static void		initClass();
  SoBigScaleUniformScaler(float cube_size=0.6f);
};

class TA_API SoBigTransformBoxRotatorRotator: public SoSeparator { 
  // ##NO_INSTANCE ##NO_TOKENS big version of the transformBoxRotatorRotator
#ifndef __MAKETA__
typedef SoSeparator inherited;
  SO_NODE_HEADER(SoBigTransformBoxRotatorRotator);
#endif // def __MAKETA__
public:
  static void		initClass();
  SoBigTransformBoxRotatorRotator(float line_width=.4f);
};

class TA_API SoScrollBar: public SoSeparator { 
  // ##NO_INSTANCE ##NO_TOKENS a scrollbar for scrolling a view, uses same interface as QScrollBar from Qt -- length is 1.0, default orientation is in X plane -- put transform in front to change
#ifndef __MAKETA__
typedef SoSeparator inherited;
  SO_NODE_HEADER(SoScrollBar);
#endif // def __MAKETA__
public:
  static void		initClass();
  SoScrollBar(int min_=0, int max_=10, int val_=0, int ps_=5, int ss_=1,
	      float wdth_ = .05f, float dpth_ = .01f);

  int	value() 		{ return value_; }	
  int	minimum()		{ return minimum_; }
  int	maximum()		{ return maximum_; }
  int	pageStep()		{ return pageStep_; }
  int	singleStep()		{ return singleStep_; }
  float width()			{ return width_; } // width is Y axis (length is X) 
  float depth()			{ return depth_; } // depth is Z axis

  void	setValue(int new_val);
  void	setMinimum(int new_min);
  void	setMaximum(int new_max);
  void	setPageStep(int new_ps);
  void	setSingleStep(int new_ss);
  void  setWidth(float new_width);
  void  setDepth(float new_depth);

  SoMaterial* getBoxMat() 		{ return box_mat_; }
  SoMaterial* getSlideMat() 		{ return slide_mat_; }
  SoMaterial* getActiveMat() 		{ return active_mat_; }

protected:
  float	width_;
  float	depth_;	
  int	minimum_;
  int	maximum_;
  int	value_;
  int	pageStep_;		// also controls the size of the bar
  int	singleStep_;

  // listed as ordered elements under overall sep
  SoMaterial* 	  box_mat_;	// box material
  SoCube* 	  box_;		// containing box for slider
  SoMaterial* 	  slide_mat_;	// slider material (inactive)
  SoTranslation*  pos_;		// position of slider
  SoTranslate1Dragger* dragger_; // the dragger

  // this is the slider that replaces guy in dragger
  SoSeparator*	slider_sep_;	// slider separator
  SoTransform*	slider_tx_;	// slider transform (rotate)
  SoCylinder*	slider_;	// slider itself

  SoSeparator*	active_sep_; 	// active slider sep
  SoMaterial* 	active_mat_;	// slider material (active)

  void	fixValues();		// make sure values are sensible
  float	getPos();		// get position for slider based on value
  void	repositionSlider();	// reposition the slider based on current values
  float	sliderSize();		// get size of slider

};



#endif
