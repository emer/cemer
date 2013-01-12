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

#ifndef DataViewer_h
#define DataViewer_h 1

// parent includes:
#include <taDataView>

// member includes:
#include <String_Array>
#include <taiMiscCore>
#include <iSize>

// declare all other types mentioned but not required to include:
class MainWindowViewer; // 
class iMainWindowViewer; // 
class TypeDef; // 
class IDataViewWidget; // 
class QWidget; // 


class TA_API DataViewer : public taDataView {
  // #NO_TOKENS #VIRT_BASE ##DEF_NAME_STYLE_1 the base type for objects with a gui window rep of some kind
INHERITED(taDataView)
friend class taDataLink;
friend class DataViewer_List;
friend class MainWindowViewer;
//friend class WindowState;
public:
  enum ImageFormat {
    EPS,                        // encapsulated postscript file (only for 3D view objects)
    JPEG,                       // JPEG -- best lossy compression (small file sizes) and ubiquitous
    PNG,                        // Portable Network Graphics -- best lossless compression (larger files, but better than raw) and ubiquitous
    PPM,                        // Portable Pixmap -- good for converting to other formats -- no compression
    IV,                         // Open Inventor format (only for 3D view objects)
  };

  static void           GetFileProps(TypeDef* td, String& fltr, bool& cmprs);
  // #IGNORE get file properties for given type
  static String_Array   image_exts;
  // #HIDDEN list of image extensions in one-to-one correspondence with ImageFormat enum

  bool                  visible; // #HIDDEN whether toolbar window is being shown to user

  virtual bool          deleteOnWinClose() const {return false;}
  inline IDataViewWidget* dvwidget() const {return m_dvwidget;}
  override bool         isMapped() const; // only true if in gui mode and gui stuff exists
  MainWindowViewer*     parent() const;
  override TypeDef*     parentType() const {return &TA_MainWindowViewer;}
  QWidget*              widget();
  virtual iMainWindowViewer* viewerWindow() const;
    // #IGNORE valid if is, or is within, a main window

  override bool         isTopLevelView() const {return true;} //

  // view state properties (don't require to be mapped)
  virtual bool          isVisible() const; // whether we are supposed to be showing or not (view state)
  virtual void          setVisible(bool value, bool update_view = true); // whether we are supposed to be showing or not (view state)

  virtual void          Constr(QWidget* gui_parent = NULL); // #IGNORE constrs the gui this class NOTE: only called directly for gui tops or for items added after mapping; all others recursively call _impl, then _post
  virtual void          Constr_impl(QWidget* gui_parent); //#IGNORE NOTE: do not call directly, only called inside us or from within Constr routines of derived classes
  void                  CloseWindow() {DoActions(CLOSE_WIN_IMPL);}
   // #IGNORE closes the window or panel, removing our reference

  virtual void          Show();         // make the item visible, if this is applicable
  virtual void          Hide();         // hide (but don't delete) the item, if applicable

  virtual void          FrameSizeToSize(iSize& sz) {} // #IGNORE converts a frame size to a window/widget size -- only applies to top level wins, and is hacky/OS-dependent (we don't need the reverse, because we call frameGeometry() to get that)

  virtual bool          GetWinState(); // copy gui state to us (override impl); true if done (ie mapped)
  virtual bool          SetWinState(); // set gui state from us (override impl)

  virtual void          ResolveChanges(CancelOp& cancel_op); // resolve all changes (if mapped)
  virtual void          WindowClosing(CancelOp& cancel_op) {}
   // cb from m_widget, subordinate wins may not be cancellable
  virtual void          WidgetDeleting(); // lets us do any cleanup -- override the impl

  virtual QPixmap       GrabImage(bool& got_image);
  // #IGNORE grabs the widget image into a pixmap object
  virtual bool          SaveImageAs(const String& fname = "", ImageFormat img_fmt = PNG);
  // #BUTTON #FILE_DIALOG_SAVE #FILETYPE_Image #EXT_png,jpg,eps,ppm,iv save the image of this view to a file -- if fname is empty, it prompts the user for a name
  virtual bool          PrintImage();
  // #BUTTON print the image of this view to printer
  static bool           InitImageExts(); // initialize the image extensions, if not already done

  void  InitLinks();
  void  CutLinks();
  void  Copy_(const DataViewer& cp);
  TA_DATAVIEWFUNS(DataViewer, taDataView) //

protected:
  // from taDataView
  override void         CloseWindow_impl(); // closes the widget, only called if mapped, default calls the Close on the IDVW

  virtual IDataViewWidget* ConstrWidget_impl(QWidget* gui_parent) {return NULL;}
    // implement this to create and set the m_widget instance -- only called if !m_widget
  override void         Constr_post();
  override void         Dump_Save_pre();
  virtual void          WidgetDeleting_impl(); // lets us do any cleanup -- override the impl
  virtual void          GetWinState_impl() {} // set gui state; only called if mapped
  virtual void          SetWinState_impl() {} // fetch gui state; only called if mapped
  virtual void          Show_impl(); // only called if mapped (note: replaced in toplevelviewer)
  virtual void          Hide_impl(); // only called if mapped (note: replaced in toplevelviewer)
  virtual void          ResolveChanges_impl(CancelOp& cancel_op); // if mapped

private:
  IDataViewWidget*      m_dvwidget; // this guy can be dangerous, so we bury it

  void  Initialize();
  void  Destroy();
};

#endif // DataViewer_h
