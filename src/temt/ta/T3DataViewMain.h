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

#ifndef T3DataViewMain_h
#define T3DataViewMain_h 1

// parent includes:
#include <T3DataViewPar>

// member includes:
#include <T3Annotation_List>

// declare all other types mentioned but not required to include:

taTypeDef_Of(T3DataViewMain);

class TA_API T3DataViewMain: public T3DataViewPar {
  // base class for major self-contained view objects, which can be independently positioned etc (main classes under T3DataViewRoot)
#ifndef __MAKETA__
typedef T3DataViewPar inherited;
#endif
public:
  enum TextJust {               // justification for text
    LEFT,                       // left justified text
    CENTER,                     // center justified text
    RIGHT,                      // right justified text
  };

  taTransform           main_xform;
  // this is the overall transform (position, scale, rotation) for this view object (typically can be adjusted by view's transform dragbox)
  T3Annotation_List     annotations;
  // extra annotations (lines, arrows, text, etc) to add to the 3d view
  T3DataView_List       annote_children; // #SHOW #READ_ONLY #SHOW_TREE annotation view children -- keep separate from other kids so they don't interfere

  bool         hasChildren() const override {return (children.size > 0 || annote_children.size > 0);}

  void         OnWindowBind(iT3Panel* vw) override;
  void         CloseChild(taDataView* child) override;

  bool         isTopLevelView() const override {return true;}

  virtual void          CopyFromViewFrame(T3DataViewPar* cp);
  // copy key view frame parameters from other view object

  virtual void          setInteractionModeOn(bool on_off, bool re_render = true);
  // set the interaction mode for the underlying examiner viewer -- does update and re-render unless specified as false

  virtual bool          ShowDraggers() const { return true; }
  // instructions from the parent about whether sub-guys should show draggers

  virtual void          BuildAnnotations();
  // add a call in your BuildAll() function to this -- will add all the annotation objects to the display

  virtual void          ReBuildAll();
  // calls Reset, BuildAll, Render -- a full rebuild -- when something structural has changed (outside of normal signal mechanism, which does this automatically on SigRecvStructUpdateEnd_impl

  virtual void          DataUnitsXForm(taVector3f& pos, taVector3f& size) { };
  // transform position and size values into native units of the display (e.g., for a graph, the units of what is being graphed) -- override in implementations

  virtual void          SaveImageSVG(const String& svg_fname);
  // #BUTTON #EXT_svg #FILE_DIALOG_SAVE render an svg version of this item to given file name

  virtual T3Annotation*  AnnoteLine(bool data_units, float pos_x, float pos_y, float pos_z,
                                    float size_x = 0.5, float size_y = 0.0, float size_z = 0.0,
                                    float line_width = 1.0, const String& color = "black");
  // #MENU_BUTTON #MENU_ON_Annote add a line annotation -- coordinates are in the normalized coordinates of the view (typically 1x1x1) or if data_units = true, in natural data units of the view (e.g., for a graph, the units of the data being graphed)
  virtual T3Annotation*  AnnoteStartArrow(bool data_units, float pos_x, float pos_y, float pos_z,
                                     float size_x = 0.5, float size_y = 0.0, float size_z = 0.0,
                                     float line_width = 1.0, const String& color = "black",
                                     float arrow_size = 0.02);
  // #MENU_BUTTON add a line with a starting arrow annotation -- coordinates are in the normalized coordinates of the view (typically 1x1x1)  or if data_units = true, in natural data units of the view (e.g., for a graph, the units of the data being graphed)
  virtual T3Annotation*  AnnoteEndArrow(bool data_units, float pos_x, float pos_y, float pos_z,
                                     float size_x = 0.5, float size_y = 0.0, float size_z = 0.0,
                                     float line_width = 1.0, const String& color = "black",
                                     float arrow_size = 0.02);
  // #MENU_BUTTON add a line with an ending arrow annotation -- coordinates are in the normalized coordinates of the view (typically 1x1x1)  or if data_units = true, in natural data units of the view (e.g., for a graph, the units of the data being graphed)
  virtual T3Annotation*  AnnoteDoubleArrow(bool data_units, float pos_x, float pos_y, float pos_z,
                                           float size_x = 0.5, float size_y = 0.0, float size_z = 0.0,
                                           float line_width = 1.0, const String& color = "black",
                                           float arrow_size = 0.02);
  // #MENU_BUTTON add a double-arrow annotation -- coordinates are in the normalized coordinates of the view (typically 1x1x1) or if data_units = true, in natural data units of the view (e.g., for a graph, the units of the data being graphed)
  virtual T3Annotation*  AnnoteRectangle(bool data_units, float pos_x, float pos_y, float pos_z,
                           float size_x = 0.5, float size_y = 0.5, float size_z = 0.0,
                           float line_width = 1.0, const String& color = "black",
                           bool fill = false, const String& fill_color = "white");
  // #MENU_BUTTON add a rectangle annotation -- coordinates are in the normalized coordinates of the view (typically 1x1x1) or if data_units = true, in natural data units of the view (e.g., for a graph, the units of the data being graphed)
  virtual T3Annotation*  AnnoteEllipse(bool data_units, float pos_x, float pos_y, float pos_z,
                           float size_x = 0.5, float size_y = 0.5, float size_z = 0.0,
                           float line_width = 1.0, const String& color = "black",
                           bool fill = false, const String& fill_color = "white");
  // #MENU_BUTTON add an ellipse annotation -- coordinates are in the normalized coordinates of the view (typically 1x1x1) or if data_units = true, in natural data units of the view (e.g., for a graph, the units of the data being graphed), and define the bounding box for the ellipse
  virtual T3Annotation*  AnnoteCircle(bool data_units, float ctr_x, float ctr_y, float ctr_z,
                                    float radius = 0.25,
                           float line_width = 1.0, const String& color = "black",
                           bool fill = false, const String& fill_color = "white");

  // #MENU_BUTTON add a circle annotation -- coordinates are in the normalized coordinates of the view (typically 1x1x1) or if data_units = true, in natural data units of the view (e.g., for a graph, the units of the data being graphed) (parameters converted to ellipse bounding box)
  virtual T3Annotation*  AnnoteText(bool data_units, const String& text,
                                    float pos_x, float pos_y, float pos_z,
                                    float font_size = 0.05,
                                    TextJust just = LEFT,
                                    const String& color = "black");
  // #MENU_BUTTON add a text annotation -- coordinates are in the normalized coordinates of the view (typically 1x1x1) or if data_units = true, in natural data units of the view (e.g., for a graph, the units of the data being graphed) for lower left start of text
  virtual T3Annotation*  AnnoteObject(bool data_units, const String& obj_file_path,
                                      float pos_x, float pos_y, float pos_z,
                                      const String& color = "black");
  // #MENU_BUTTON add an object (3D inventor file) annotation -- coordinates are in the normalized coordinates of the view (typically 1x1x1) or if data_units = true, in natural data units of the view (e.g., for a graph, the units of the data being graphed)
  virtual void          AnnoteClearAll();
  // #MENU_BUTTON remove all existing annotations
  
  void         InitLinks() override;
  void         CutLinks() override;
  T3_DATAVIEWFUNS(T3DataViewMain, T3DataViewPar) //
protected:
  void         DoActionChildren_impl(DataViewAction acts) override;
  void         ReInit_impl() override; //note: does a depth-first calls to children, before self

private:
  void  Copy_(const T3DataViewMain& cp);
  void  Initialize();
  void  Destroy() { CutLinks(); }
};

#endif // T3DataViewMain_h
