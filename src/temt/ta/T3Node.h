// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef T3Node_h
#define T3Node_h 1

// parent includes:
#include "ta_def.h"

#ifndef __MAKETA__
#include <Inventor/nodes/SoSeparator.h>
#endif

// member includes:

// declare all other types mentioned but not required to include:

// externals -- note: some are here for use by other files, not this one
class SoGroup; //
class SbColor; // 
class SoCoordinate3; // 
class SbRotation; // 
class SbVec3f; // 
class SoAsciiText; // 
class SoDrawStyle; // 
class SoFont; // 
class SoLightModel; // 
class SoLineSet; // 
class SoMarkerSet; // 
class SoMaterial; // 
class SoMFVec3f; // 
class SoNode; // 
class SoPackedColor; // 
class SoPerspectiveCamera; // 
class SoPickStyle; // 
class SoRotation; // 
class SoScale; // 
class SoSeparator; // 
class SoTexture2; // 
class SoTexture2Transform; // 
class SoTranslate1Dragger; // 
class SoTranslation; // 
class SoTransform; // 
class SoTriangleStripSet; // 
class SoIndexedTriangleStripSet; // 
class SoUnits; // 
class SoVertexProperty; // 
class SoTransformBoxDragger; // 
class SoCalculator; // 
class SoIndexedLineSet; // 
class SoDrawStyle; // 
class T3DataView;  //
class iVec3f; //

/*
  All Nodes are implemented using an approach that is similar to, but simpler than,
  Inventor Node Kits. We expose the contents as properties (ex. drawStyle()) -- properties
  may be always created (ex in constructor) or only created on demand. The properties
  themselves are stored in fields, so they automatically participate in copying, streaming,
  etc.

  Coordinate Space

  The coordinate space for "pos" and "geom" calls follows the emergent conventions. The values
  are in integral units in some abstract 3-d space; the frame of reference is as follows:

  Dimension	emergent Inventor
  left-->right	+x	+x
  bott->top	+y	+y
  back->front	-z	+z

  Because of how layers are stacked, the emergent-y space is scaled with respect to the other
  spaces. However, this is not done with transforms (it would distort shapes), but merely
  by scaling the emergent y factor.

  The letter 'p' in front of coordinate items (px, py, etc.) means emergent coordinates.

  TODO:

  Loading will be an issue, since the member object fields will need to be set to the
  child instances loaded. Perhaps readInstance() can be overloaded as follows:
      clear children
      set member objects to NULL (release)
      inherited::readInstance()
      set member object references by looking for them in the children() collection
*/


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
taTypeDef_Of(T3Node);

// NOTE: unfortunately we cannot mark T3Nodes as ##NO_MEMBERS because we need the initClass method which is called to initialize Coin3d Types

class TA_API T3Node: public SoSeparator {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS a base class for Inventor (Coin3d) nodes -- are paired with corresponding T3DataView object that provides persistence and generally controls behavior of this node -- T3Node should be minimal container object
#ifndef __MAKETA__
typedef SoSeparator inherited;

  SO_NODE_ABSTRACT_HEADER(T3Node);
#endif // def __MAKETA__
public:

  static void		initClass();

  static void		insertChildBefore(SoGroup* group, SoNode* child, SoNode* before);
   // #IGNORE insert before node; before=NULL for end;
  static void		insertChildAfter(SoGroup* group, SoNode* child, SoNode* after);
   // #IGNORE insert before node; after=NULL for start
  static SoNode*	getNodeByName(SoGroup* group, const char* name);
  // #IGNORE find node by name, if any

  inline T3DataView*	dataView() {return dataView_;} // #IGNORE the T3DataView that owns/created this node

  virtual SoFont*	captionFont(bool auto_create = false) = 0; // #IGNORE 
  SoAsciiText*		captionNode(bool auto_create = false); // #IGNORE 
  SoSeparator*	        topSeparator() {return this;} // #IGNORE 
  SoTransform*		transform() const {return transform_;} // #IGNORE the master transform, for the whole entity
  virtual SoSeparator*	shapeSeparator() = 0;                  // #IGNORE 
  SoTransform*		txfm_shape() const {return txfm_shape_;}
  // #IGNORE the transform for the shape
  SoMaterial*		material() const {return material_;} // #IGNORE 
  virtual SoSeparator*	childNodes() {return NULL;} // #IGNORE use this to set/get T3Node children

  virtual const char*	caption();
  virtual void		setCaption(const char* value); //NOTE: if you want to transform, you MUST call transformCaption every time after calling setCaption

  virtual void		clear() {} // optional method, for clearing out the content; called from ReInit
  void			transformCaption(const iVec3f& translate); // #IGNORE
  void			transformCaption(const SbRotation& rotate, const iVec3f& translate); // #IGNORE
  void			resizeCaption(float sz);
  virtual bool		expanded() {return true;} // many nodes have an expanded and compact rep
  virtual void		setExpanded(bool value) {}

  virtual void		addRemoveChildNode(SoNode* node, bool adding); // #IGNORE called by pdpDataView (default prints console error)

  T3Node(T3DataView* dataView_ = NULL);

protected:
  SoAsciiText*		captionNode_;
  const char*  		getFileFormatName() const {return "Separator"; }
  // makes output files fully general
  virtual SoSeparator*	captionSeparator(bool auto_create = false) = 0;
  SoTransform*		captionTransform(bool auto_create = false);
  virtual void		setDefaultCaptionTransform(); // call transformCaption to set the default transform; this is called after creating Node first time
  void			transformCaption(const SbRotation* rotate, const iVec3f* translate); // #IGNORE
  void 			initCommon(); // shared code, can only be called in a subclass constructor

  ~T3Node();

private:
  T3DataView*           dataView_; // #IGNORE private since it can be accessed through dataView().
  SoTransform*		transform_; // #IGNORE
  SoTransform*		txfm_shape_; // #IGNORE NOTE: must be created in subclass init
  SoMaterial*		material_; // #IGNORE NOTE: must be created in subclass init
};

#endif // T3Node_h
