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

#ifndef iClassBrowseViewer_h
#define iClassBrowseViewer_h 1

// parent includes:
#include <iBrowseViewer>

// member includes:
#include <ClassBrowseViewer>

// declare all other types mentioned but not required to include:
class iAction;


class TA_API iClassBrowseViewer: public iBrowseViewer { // viewer window used for class browsing
  Q_OBJECT
  INHERITED(iBrowseViewer)
  friend class ClassBrowser;
public:
  ClassBrowseViewer*            browser() {return (ClassBrowseViewer*)m_viewer;}

  iClassBrowseViewer(ClassBrowseViewer* browser_, QWidget* parent = 0);
  ~iClassBrowseViewer();

public slots:
  virtual void          mnuNewBrowser(iAction* mel); // called from context 'New Browse from here'; cast obj to taiNode*
};

#endif // iClassBrowseViewer_h
