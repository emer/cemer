/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
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

#ifndef TA_VIRTENV_SO_H
#define TA_VIRTENV_SO_H

#include "ta_TA_type.h"

/////////////////////////////////////////////////////////////////////////
//		So Inventor Code

#include "t3node_so.h"

class SoOffscreenRenderer; // #IGNORE
class SoSwitch;		   // #IGNORE
class SoDirectionalLight;  // #IGNORE

class TA_API T3VEWorld : public T3NodeParent {
  // world parent for virtual environment 
#ifndef __MAKETA__
typedef T3NodeParent inherited;
  SO_NODE_HEADER(T3VEWorld);
#endif // def __MAKETA__
public:
  static void	initClass();

  T3VEWorld(void* world = NULL);

  void			setSunLightDir(float x_dir, float y_dir, float z_dir);
  void			setSunLightOn(bool on);
  SoDirectionalLight* 	getSunLight()	  { return sun_light; }

  void			setCamLightDir(float x_dir, float y_dir, float z_dir);
  void			setCamLightOn(bool on);
  SoDirectionalLight* 	getCamLight()	  { return cam_light; }

  SoGroup*		getLightGroup() { return light_group; }
  SoSwitch*		getCameraSwitch() { return camera_switch; }
  SoSwitch*		getTextureSwitch() { return textures; }

protected:
  SoDirectionalLight* 	sun_light;
  SoDirectionalLight* 	cam_light;
  SoGroup*		light_group;
  SoSwitch*		camera_switch; // switching between diff cameras
  SoSwitch*		textures;      // group of shared textures (always switched off -- used by nodes)
  
  ~T3VEWorld();
};

class TA_API T3VEObject : public T3NodeParent {
  // object parent for virtual environment 
#ifndef __MAKETA__
typedef T3NodeParent inherited;
  SO_NODE_HEADER(T3VEObject);
#endif // def __MAKETA__
public:
  static void	initClass();

  T3VEObject(void* obj = NULL);

protected:
  ~T3VEObject();
};

class TA_API T3VEBody : public T3NodeLeaf {
  // body for virtual environment 
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;
  SO_NODE_HEADER(T3VEBody);
#endif // def __MAKETA__
public:
  static void	initClass();

  T3VEBody(void* bod = NULL, bool show_drag = false);

protected:
  bool			 show_drag_;
  T3TransformBoxDragger* drag_;	// my position dragger

  ~T3VEBody();
};

class TA_API T3VESpace : public T3NodeParent {
  // space parent for virtual environment 
#ifndef __MAKETA__
typedef T3NodeParent inherited;
  SO_NODE_HEADER(T3VESpace);
#endif // def __MAKETA__
public:
  static void	initClass();

  T3VESpace(void* obj = NULL);

protected:
  ~T3VESpace();
};

class TA_API T3VEStatic : public T3NodeLeaf {
  // static item for virtual environment 
#ifndef __MAKETA__
typedef T3NodeLeaf inherited;
  SO_NODE_HEADER(T3VEStatic);
#endif // def __MAKETA__
public:
  static void	initClass();

  T3VEStatic(void* stat = NULL, bool show_drag = false);

protected:
  bool			 show_drag_;
  T3TransformBoxDragger* drag_;	// my position dragger

  ~T3VEStatic();
};

#endif // TA_VIRTENV_SO_H
