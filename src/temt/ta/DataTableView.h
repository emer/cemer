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

#ifndef DataTableView_h
#define DataTableView_h 1

// parent includes:
#include <T3DataViewMain>

// member includes:
#include <MinMaxInt>
#ifndef __MAKETA__
#include <QPointer>
#endif

// declare all other types mentioned but not required to include:
class taSigLink; // 
class DataTable; // 
class DataColView; // 
class iDataTableView_Panel; //


TypeDef_Of(DataTableView);

class TA_API DataTableView : public T3DataViewMain {
  // #VIRT_BASE base class of grid and graph views
INHERITED(T3DataViewMain)
public:
  int           view_rows;      // maximum number of rows visible
  MinMaxInt     view_range;     // range of visible rows (max is the last row visible, not the last+1; range = view_rows-1)

  bool          display_on;     // #DEF_true 'true' if display should be updated
  bool          manip_ctrl_on;  // #DEF_true display the manipulation controls on objects for positioning etc

  virtual const String  caption() const; // what to show in viewer

  DataTable*            dataTable() const {return (DataTable*)data();}
  virtual void          setDataTable(DataTable* dt);
  // #MENU #NO_NULL build the view from the given table

  void                  setDisplay(bool value); // use this to change display_on
  override void         setDirty(bool value); // set for all changes on us or below
  inline int            rows() const {return m_rows;}
  bool                  isVisible() const; // gui_active, mapped and display_on

  DataColView*          colView(int i) const
  { return (DataColView*)children.SafeEl(i); }
  inline int            colViewCount() const { return children.size;}

  /////////////////////////////////////////////
  //    Main interface: init/update (impl in subclasses)

  virtual void          InitDisplay(bool init_panel = true) { };
  // does a hard reset on the display, reinitializing variables etc.  Note does NOT do Updatedisplay -- that is a separate step
  virtual void          UpdateDisplay(bool update_panel = true) { };
  // full re-render of the display (generally calls Render_impl)

  virtual void          InitPanel();
  // lets panel init itself after struct changes
  virtual void          UpdatePanel();
  // after changes to props

  virtual void          ClearData();
  // Clear the display and the data
  virtual void          ViewRow_At(int start);
  // #BUTTON start viewing at indicated viewrange value

  virtual void          RowBackAll();
  virtual void          RowBackPg();
  virtual void          RowBack1();
  virtual void          RowFwd1();
  virtual void          RowFwdPg();
  virtual void          RowFwdAll();

  override void         SigDestroying();
  override void         BuildAll();

  virtual void          UpdateName();  // update name from data table

  void  Initialize();
  void  Destroy()       { CutLinks(); }
  void  InitLinks();
  void  CutLinks();
  void  Copy_(const DataTableView& cp);
  T3_DATAVIEWFUNS(DataTableView, T3DataViewMain) //

// ISigLinkClient i/f
  override void         IgnoredSigEmit(taSigLink* dl, int dcr,
    void* op1, void* op2); //

// ISelectable i/f
  override GuiContext   shType() const {return GC_DUAL_DEF_VIEW;}

protected:
#ifndef __MAKETA__
  QPointer<iDataTableView_Panel> m_lvp; //note: will be a subclass of this, per the log type
#endif
  int                   m_rows; // cached rows, we use to calc deltas etc.
  int                   updating; // to prevent recursion

  override void         UpdateAfterEdit_impl();

  virtual void          ClearViewRange();
  // sets view range back to beginning (grid adds cols, graph adds TBA)
  virtual void          MakeViewRangeValid();
  // adjust row/col etc. to be valid

  virtual int           CheckRowsChanged(int& orig_rows);
  // check if datatable rows is same as last render (updates m_rows and returns any delta, 0 if no change)

  override void         Unbind_impl(); // unbinds table

  override void         SigRecvUpdateView_impl();
  override void         SigRecvUpdateAfterEdit_impl();
  override void         DoActionChildren_impl(DataViewAction acts);

  void                  UpdateFromDataTable(bool first_time = false);
  // called if data set to table, or needs to be updated; calls _child then _this
  virtual void          UpdateFromDataTable_child(bool first);
  // does kids, usually not overridden
  virtual void          UpdateFromDataTable_this(bool first);
  // does me (*after* kids, so you can refer to them)
  virtual void          DataTableUnlinked(); // called if data is NULL or destroys

  override void         Render_pre();
  override void         Render_impl();
  override void         Render_post();
  override void         Reset_impl();
};

#endif // DataTableView_h
