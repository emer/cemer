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

#ifndef T3Panel_h
#define T3Panel_h 1

// parent includes:
#include <taViewer>

// member includes:
#include <T3DataViewRoot>
#include <T3SavedView_List>
#include <taColor>

// declare all other types mentioned but not required to include:
class T3DataView; //
class iT3Panel; //
class QPixmap; //
class T3DataViewMain; //

/* T3Panel

  The panel can hold multiple objects, however the typical cases are:
   1 object ("single")-- ex., a graph or table view
   N objects (!"single") -- ex., a network, with viewlets, graphs, etc.

  We treat some things differently, depending on single or multi:
                single          multi
  DisplayName   item name       panel name
  bg color      slave to item

*/

taTypeDef_Of(T3Panel);

class TA_API T3Panel : public taViewer {
  // ##DEF_NAME_ROOT_Panel #AKA_T3DataViewFrame a panel within set of 3d view panels containing one or more 3D view elements organize within a common overall scenegraph
INHERITED(taViewer)
friend class T3DataView;
friend class T3PanelViewer;
public:
  enum StereoView {
    STEREO_NONE,                // use monoscopic rendering (default)
    STEREO_ANAGLYPH,            // Render stereo by superimposing two images of the same scene, but with different color filters over the left and right view (or "eye").  This is a way of rendering stereo which works on any display, using color-filter glasses. Such glasses are usually cheap and easy to come by.
    STERO_QUADBUFFER,           // Render stereo by using OpenGL quad-buffers. This is the most common interface for stereo rendering for more expensive hardware devices, such as shutter glasses and polarized glasses. The well known Crystal Eyes glasses are commonly used with this type of stereo display.
    STEREO_INTERLEAVED_ROWS,    // Interleaving / interlacing rows from the left and right eye is another stereo rendering method requiring special hardware. One example of a provider of shutter glasses working with interleaved glasses is VRex: http://www.vrex.com/
    STEREO_INTERLEAVED_COLUMNS,  // Same basic technique as SoQtViewer::STEREO_INTERLEAVED_ROWS, only it is vertical lines that are interleaved / interlaced, instead of horizontal lines.
  };

  enum ColorScheme {
    BLACK_ON_GREY,  // Black text on grey background (default)
    BLACK_ON_WHITE, // Black text on white background
    WHITE_ON_BLACK, // White text on black background
    RED_ON_BLACK,   // Red text on black background
    GREEN_ON_BLACK, // Green text on black background
    BLUE_ON_BLACK,  // Blue text on black background
  };

  T3DataViewRoot        root_view; // #SHOW_TREE placeholder item -- contains the actual root(s) DataView items as children
  T3DataView_List*      root_views; // #READ_ONLY #HIDDEN #NO_SAVE #NO_FIND pointer to root_view.children -- for selecting root views
  taColor               bg_color; // background color of the panel note: alpha transparency value is also used and will be reflected in saved images!
  taColor               text_color; // color to use for text in the panel -- may need to change this from default of black depending on the bg_color
  bool                  headlight_on; // turn the camera headlight on for illuminating the scene -- turn off only if there is another source of light within the scenegraph -- otherwise it will be dark!
  StereoView            stereo_view;  // what type of stereo display to render, if any
  T3SavedView_List      saved_views;  // saved camera position views from viewer -- this is the persitent version copied from camera

  bool                  singleMode() const
    {return (root_view.children.size == 1);}
    // true when one and only one child obj -- it can control some of our properties by default
  T3DataView*           singleChild() const; // return the single child
  T3DataViewMain*       FirstChild() const; // return the first child, as a main

  inline iT3Panel* widget() {return (iT3Panel*)inherited::widget();} // lex override
  const iColor          GetBgColor() const; // #IGNORE get the effective bg color
  const iColor          GetTextColor() const; // #IGNORE get the effective text color

  virtual void          AddView(T3DataView* view); // add a view
  // #IGNORE
  virtual T3DataView*   FindRootViewOfData(taBase* data);
  // #IGNORE looks for a root view of the data, returns it if found; useful to check for existing view before adding a new one

  virtual void          ViewAll();
  // #CAT_Display reset the camera position to view everything in the display
  virtual void          GetSavedView(int view_no);
  // #CAT_Display copy given saved view on the T3ExaminerViewer to our saved view information (for persistence) -- does not actually grab the current view, just the previously saved data -- see SaveCurView
  virtual void          SetSavedView(int view_no);
  // #CAT_Display copy our saved view to the T3ExaminerViewer -- does not go to that view, just sets data -- see GoToSavedView
  virtual void          SetAllSavedViews();
  // #CAT_Display copy all our saved views to the T3ExaminerViewer -- during initialization

  virtual void          SaveCurView(int view_no);
  // #CAT_Display save the current T3 examiner viewer view parameters to the given view (on us and the viewer) -- for programmatic usage
  virtual void          GoToSavedView(int view_no);
  // #CAT_Display tell the viewer to go to given saved view parameters (copies our parameters in case they have been locally modified programmatically)

  void  SetCameraPos(int view_no, float x, float y, float z)
  { saved_views.SetCameraPos(view_no, x,y,z); }
  // #CAT_Display for given view number, set camera position
  void  SetCameraOrient(int view_no, float x, float y, float z, float r)
  { saved_views.SetCameraOrient(view_no, x,y,z,r); }
  // #CAT_Display for given view number, set camera orientation -- x,y,z axis and r rotation value
  void  SetCameraFocDist(int view_no, float fd)
  { saved_views.SetCameraFocDist(view_no, fd); }
  // #CAT_Display for given view number, set camera focal distance

  void  GetCameraPos(int view_no, float& x, float& y, float& z)
  { saved_views.GetCameraPos(view_no, x,y,z); }
  // #CAT_Display for given view number, set camera position
  void  GetCameraOrient(int view_no, float& x, float& y, float& z, float& r)
  { saved_views.GetCameraOrient(view_no, x,y,z,r); }
  // #CAT_Display for given view number, set camera orientation -- x,y,z axis and r rotation value
  void  GetCameraFocDist(int view_no, float& fd)
  { saved_views.GetCameraFocDist(view_no, fd); }
  // #CAT_Display for given view number, set camera focal distance

  virtual void          EditView(T3DataViewMain* view);
  // #CAT_Display #BUTTON #FROM_GROUP_root_views edit given view within this panel -- can provide more detailed view control information than what is present in the middle view control panel

  QPixmap      GrabImage(bool& got_image) override;
  bool         SaveImageAs(const String& fname = "", ImageFormat img_fmt = PNG) override;
  virtual  void         SetImageSize(int width, int height);
  // #CAT_Display #BUTTON set size of 3d viewer (and thus SaveImageAs image) to given size parameters
  virtual  void         SetColorScheme(ColorScheme color_scheme);
  // #CAT_Display #BUTTON Choose a preset color scheme
  virtual  void         GridLayout(int n_horiz = 2, float horiz_space = 0.2,
                                   float vert_space = 0.2, bool save_views=false);
  // #CAT_Display #BUTTON layout the elements within this view in a grid, starting with the first item in the lower left corner, moving horizontally and up through the items, with given number of items along the horizontal axis, and given spacing between items (can be negative to pack closer together)  -- if save_views then the saved views 1-n will be aligned with each of the elements in turn

  virtual bool          SaveImageEPS(const String& fname = "");
  // #CAT_Display save image in EPS format
  virtual bool          SaveImageIV(const String& fname = "");
  // #CAT_Display save image in IV (open inventor) format

  virtual void          CopyFromViewPanel(T3Panel* cp);
  // #CAT_Display copy key view information from another view panel (saved views, other view info)

  void  SigEmit(int sls, void* op1 = NULL, void* op2 = NULL); // we notify viewer
  void  InitLinks();
  void  CutLinks();
  TA_DATAVIEWFUNS(T3Panel, taViewer)

protected:
  static void           SoSelectionCallback(void* inst, SoPath* path); // #IGNORE
  static void           SoDeselectionCallback(void* inst, SoPath* path); // #IGNORE


  void         Constr_impl(QWidget* gui_parent = NULL) override;
  IViewerWidget* ConstrWidget_impl(QWidget* gui_parent) override; // #IGNORE
  void         Constr_post() override;
  void         WindowClosing(CancelOp& cancel_op) override; // #IGNORE
  void         Clear_impl() override; // #IGNORE
  void         Render_pre() override; // #IGNORE
  void         Render_impl() override;  // #IGNORE
  void         Render_post() override; // #IGNORE
  void         Reset_impl() override; //  #IGNORE
private:
  void  SetTextBgColor(const String &new_text_color, const String &new_bg_color);
  void  Copy_(const T3Panel& cp);
  void                  Initialize();
  void                  Destroy();
};

#endif // T3Panel_h
