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

#ifndef iDataTableEditor_h
#define iDataTableEditor_h 1

// parent includes:
#include <ISelectableHost>
#include <ISigLinkClient>

// member includes:
#ifndef __MAKETA__
#include <QModelIndex>
#endif
#include <taMatrix>
#include <DataTable>

// declare all other types mentioned but not required to include:
class QVBoxLayout; //  
class QSplitter; //  
class iDataTableView; // 
class iMatrixEditor; // 
class ISelectable; // 
class QObject; //  
class iMainWindowViewer; //  
class taProject; // 
class ISelectable_PtrList; // 
class QWidget; //  
class TypeDef; // 
class taiSigLink; // 
class taSigLink; // 
class DataTable; // 
class iDataTableModel; // 
class QModelIndex; //
class QEvent; //


TypeDef_Of(iDataTableEditor);

class TA_API iDataTableEditor: public QWidget,  public virtual ISelectableHost,
                               public virtual ISigLinkClient {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS editor for data tables
  Q_OBJECT
INHERITED(QWidget)
public:
  QVBoxLayout*          layOuter;
  QSplitter*            splMain;
  iDataTableView*         tvTable; // the main table
  iMatrixEditor*          tvCell; // a matrix cell in the table (only shown if needed)

  DataTable*            dt() const {return m_dt;}
  void                  setDataTable(DataTable* dt); // only called once
  iDataTableModel*       dtm() const;

  virtual void          ScrollToBottom(); // scroll table to last row

  void                  Refresh(); // for manual refresh

  iDataTableEditor(QWidget* parent = NULL);
  ~iDataTableEditor();

public slots:
  void                  tvTable_currentChanged(const QModelIndex& index); // #IGNORE
  void                  tvTable_dataChanged(const QModelIndex& topLeft,
                                            const QModelIndex & bottomRight); // #IGNORE
  void                  tvTable_layoutChanged(); // #IGNORE

public: // ITypedObject i/f
  override void*        This() {return this;}
  override TypeDef*     GetTypeDef() const {return &TA_iDataTableEditor;}

public: // ISelectableHost i/f
  override bool         hasMultiSelect() const {return false;} // always
  override QWidget*     widget() {return this;} //
protected:
  override void         UpdateSelectedItems_impl(); //

public: // ISigLinkClient i/f
  override void         SigLinkDestroying(taSigLink* dl);
  override void         SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2);

protected:
  DataTableRef          m_dt;
  taMatrix*             m_cell_par; // parent of cell -- we link to it
  taMatrixPtr           m_cell; // current cell
  QModelIndex           m_cell_index; // we keep this to refresh cell if data changes

  override bool         eventFilter(QObject* obj, QEvent* event);

  void                  setCellMat(taMatrix* cell, const QModelIndex& index,
                                   bool pat_4d = false);
  void                  ConfigView(); // setup or change view, esp after col ins/deletes
};

#endif // iDataTableEditor_h
