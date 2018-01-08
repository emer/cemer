// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#ifndef iDimEdit_h
#define iDimEdit_h 1

#include "ta_def.h"

#include <QWidget>

class QHBoxLayout; // 
class QLabel; //
class iSpinBox; //

//////////////////////////////////
// 	iDimEdit		//
//////////////////////////////////

class TA_API iDimEdit: public QWidget {
  // #IGNORE provides a horizontal striped background, to highlight items
  Q_OBJECT
public:
  QHBoxLayout*		lay;
  QLabel*		lblDims;
  iSpinBox*		spnDims;
  QWidget*		wdgDimEdits;
  QHBoxLayout*		layDimEdits;
  QWidgetList		dimLabels;
  QWidgetList		dimEdits;


  int			dims() const; // number of dims
  void			setDims(int value);
  int			dim(int idx) const; // value of a given dim
  void			setDim(int idx, int value);
  int			maxDims() const {return m_maxDims;} // default=8
  void			setMaxDims(int value);
  bool			isDimsReadOnly() const {return m_dimsReadOnly;}
  void			setDimsReadOnly(bool value);
  bool 			isReadOnly() const {return m_readOnly;}
  
  iDimEdit(QWidget* parent = NULL);
  ~iDimEdit();
  
public slots:
  void			setReadOnly(bool value);

#ifndef __MAKETA__
signals:
  void			changed(iDimEdit* sender); // called after user changes a dim and/or number of dims
#endif
  
protected:
  int			m_maxDims;
  int			m_changing; // changing ref count
  bool			m_dimsReadOnly; // dimension only
  bool			m_readOnly;  //all
  bool			inRange(int idx) const;
  void			setDims_impl(int value);
  
protected slots:
  void			dim_valueChanged(int value);
  void			dims_valueChanged(int value);
  
private:
  void			init();
};

#endif // iDimEdit_h
