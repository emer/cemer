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

#ifndef iTypeInfoBrowseViewer_h
#define iTypeInfoBrowseViewer_h 1

// parent includes:
#include <iBrowseViewer>

// member includes:
#include <TypeInfoBrowseViewer>

// declare all other types mentioned but not required to include:
class iAction;


class TA_API iTypeInfoBrowseViewer: public iBrowseViewer { // viewer window used for class browsing
  Q_OBJECT
  INHERITED(iBrowseViewer)
  friend class TypeInfoBrowser;
public:
  TypeInfoBrowseViewer*            browser() {return (TypeInfoBrowseViewer*)m_viewer;}

  iTypeInfoBrowseViewer(TypeInfoBrowseViewer* browser_, QWidget* parent = 0);
  ~iTypeInfoBrowseViewer();

public slots:
  virtual void          mnuNewBrowser(iAction* mel); // called from context 'New Browse from here'; cast obj to taiNode*
};

#endif // iTypeInfoBrowseViewer_h
