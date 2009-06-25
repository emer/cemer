#ifndef QUARTER_EXAMINER_VIEWER_H
#define QUARTER_EXAMINER_VIEWER_H

// SoQtExaminerViewer written in Quarter system
// by Randall C. O'Reilly randy.oreilly@colorado.edu
// based extensively on SoQtExaminerViewer code and quarter examples
// by Coin3d.org guys -- their copyright information is appended below
// this code is also made available under the GPL and the parts that
// are written by O'Reilly and/or his collaborators and students are 
// Copyright (C) Regents of the University of Colorado,

/**************************************************************************\
 *
 *  This file is part of the SIM Quarter extension library for Coin.
 *  Copyright (C) 1998-2009 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License ("GPL") version 2
 *  as published by the Free Software Foundation.  See the file COPYING
 *  at the root directory of this source distribution for additional
 *  information about the GNU GPL.
 *
 *  For using SIM Quarter with software that can not be combined with
 *  the GNU GPL, and for taking advantage of the additional benefits of
 *  our support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See <URL:http://www.coin3d.org/> for more information.
 *
 *  Systems in Motion AS, Bygdøy allé 5, N-0257 Oslo, NORWAY. (www.sim.no)
 *
\**************************************************************************/

// the qtthumbwheel widget is released under LGPL and:
// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// see qthumbwheel.h/.cpp for more information

// this code is intentionally written with all public interfaces
// and members, and is designed for maximum code reuse for other
// projects that seek to modify it in any way possible -- it is not
// designed to be part of a binary compatible library
// if you want to write a bunch of accessor routines and hide everything,
// be my guest.  Also, everything is just in one big file so it is fully
// self contained (with the one exception of the qtthumbwheel code which is
// itself otherwise self-contained).

// the only dependencies are Quarter, Qt, and Coin.

// I did take a few liberties in modifying the original viewer in minor ways

#include <Quarter/Quarter.h>
#include <Quarter/QuarterWidget.h>

#include <Inventor/SbLinear.h>

class QtThumbWheel;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QToolButton;
class QLabel;
class QKeyEvent;
class SoCamera;

using namespace SIM::Coin3D::Quarter;

class QEVSavedCamera {
  // Saves camera parameters for the Quarter Examiner Viewer
public:
  bool		view_saved;	// whether a view is currently saved or not
  SbVec3f	pos;		// position of the camera
  SbVec3f	rot_axis;	// rotation axis
  float		rot_angle;	// rotation angle
  float		focal_dist;	// focal distance

  QEVSavedCamera();
  ~QEVSavedCamera();

  void		getCameraParams(SoCamera* cam);
  // get the camera parameters into our saved values
  bool		setCameraParams(SoCamera* cam);
  // set the camera parmeters from our saved values (returns false if no view saved)
};

class QuarterExaminerViewer : public QWidget {
  // provides a full examiner viewer interface built on top of the QuarterWidget viewer, replicating the GUI of the SoQtExaminerViewer
  Q_OBJECT
public:
  QuarterExaminerViewer(QWidget * parent = 0);
  ~QuarterExaminerViewer();

  //////////////////////////////////////////////
  //   Overall widget structure

  QVBoxLayout*	  main_vbox;	   // overall vertical box, containing main_hbox, bot_hbox
  QHBoxLayout*	  main_hbox;	   // overall horizontal box, containing lhs_vbox, quarter, rhs_vbox
  QVBoxLayout*	  lhs_vbox; 	// overall box for all decoration on the left-hand-side of widget -- contains (optional -- none in default impl) buttons at top and vrot_wheel at bottom
    QVBoxLayout*  lhs_button_vbox; // buttons on the top-left hand side (empty by default)
  QuarterWidget*  quarter;	// the quarter viewer -- in the middle of the widget
  QVBoxLayout*	  rhs_vbox; 	// overall box for all decoration on the right-hand-side of widget -- contains buttons at top and zoom_wheel at bottom
    QVBoxLayout*  rhs_button_vbox; // buttons on the top-right hand side
  QHBoxLayout*	  bot_hbox;	   // overall box for all decoration on the bottom of widget -- contains (optional -- none in default impl) buttons at right and hrot_wheel at left
    QHBoxLayout*  bot_button_hbox; // buttons on the bottom right side

  //////////////////////////////////////////////
  //   Thumbwheels

  QtThumbWheel*	  zoom_wheel;	// the zoom wheel (dolly in SoQt)
  QtThumbWheel*	  vrot_wheel;	// the vertical rotation wheel (Rotx in SoQt)
  QtThumbWheel*	  hrot_wheel;	// the horizontal rotation wheel (Roty in SoQt)
  QLabel*	  zoom_lbl;	// labels
  QLabel*	  vrot_lbl;
  QLabel*	  hrot_lbl;

  //////////////////////////////////////////////
  //   Standard view buttons (on RHS)

  QToolButton*	  interact_button; // (red arrow) -- mouse actions interact with elements in the display
  QToolButton*	  view_button;     // (white hand) -- mouse actions move view around
  QToolButton*	  home_button;     // (home) -- restores display to the home viewing configuration
  QToolButton* 	  set_home_button; // (blue home) -- set the home viewing configuration for home button to current configuration
  QToolButton*	  view_all_button; // (eyeball) -- repositions camera so everything is in view
  QToolButton* 	  seek_button;	   // (flashlight) -- zooms display to view clicked objects
  QToolButton* 	  snapshot_button; // (camera) -- save an image of the current view to a file
  QToolButton* 	  print_button;    // (printer) -- print current view to a file

  QEVSavedCamera  saved_home;	   // saved home view information

  //////////////////////////////////////////////
  //   Constructor helper methods

  void    Constr_RHS_Buttons();		// construct right-hand-side pushbuttons
  void    Constr_LHS_Buttons();		// construct left-hand-side pushbuttons
  void    Constr_Bot_Buttons();		// construct bottom pushbuttons

  //////////////////////////////////////////////
  //   Functions that actually do stuff

  virtual SoCamera*  	getViewerCamera();
  // helper function get the quarter viewer camera (not immediately avail on quarter widget)
  virtual void 	  	viewAll();
  // view all objects in scene -- compared to QuarterWidget's default version, this one doesn't leave such a huge margin around everything so you really fill the window

  virtual void		zoomView(const float zoom_value);
  // zoom view camera in/out by given amount: adjusts both camera pos and focal length -- associated with the zoom_wheel on the right side of viewer
  virtual void		horizRotateView(const float rot_value);
  // horizontally rotate view camera (actually around the vertical Y axis) -- associated with the hrot_wheel on bottom of viewer
  virtual void		vertRotateView(const float rot_value);
  // vertically rotate view camera (actually around the horizontal or X axis) -- associated with the vrot_wheel on left hand side of viewer
  virtual void		RotateView(const SbVec3f& axis, const float ang);
  // implementation function that will rotate view camera given angle (in radians) around given axis

  virtual void		setInteractionModeOn(bool onoff);
  // set the interaction mode on or off (if off, then it is in view mode) -- also updates button states

  virtual void		saveHome();
  // save the current camera view information to the 'home' view
  virtual void		goHome();
  // restore the saved camera view information to the current view

  virtual QImage	grabImage();
  // grab the current viewer image to a pixmap
  virtual void		saveImage(const QString& fname);
  // save the current viewer image to given file name
  virtual void		printImage();
  // print the current viewer image to a printer

public slots:
  void hrotwheelChanged(int value);
  void vrotwheelChanged(int value);
  void zoomwheelChanged(int value);

  void interactbuttonClicked();
  void viewbuttonClicked();
  void homebuttonClicked();
  void sethomebuttonClicked();
  void viewallbuttonClicked();
  void seekbuttonClicked();
  void snapshotbuttonClicked();
  void printbuttonClicked();

signals:
  void homeSaved();		// the home location was saved (e.g., can now save to more permanent storage)

protected:
  // start values on wheels for computing deltas
  int	hrot_start_val;
  int	vrot_start_val;
  int	zoom_start_val;

  void keyPressEvent(QKeyEvent* e);
};

#endif // QUARTER_EXAMINER_VIEWER_H

