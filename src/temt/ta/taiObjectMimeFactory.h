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

#ifndef taiObjectMimeFactory_h
#define taiObjectMimeFactory_h 1

// parent includes:
#include <taiMimeFactory>

// member includes:

// declare all other types mentioned but not required to include:
class QMimeData; //

TypeDef_Of(taiObjectMimeFactory);

class TA_API taiObjectMimeFactory: public taiMimeFactory {
INHERITED(taiMimeFactory)
public:
  static const String	text_plain; // all formats support this
  static const String	tacss_objectdesc;
  static const String	tacss_objectdata;
//static taiObjectMimeFactory* instance(); // provided by macro

  virtual void		AddSingleObject(QMimeData* md, taBase* obj);
    // used for putting one object on the clipboard
  virtual void		AddMultiObjects(QMimeData* md, taPtrList_impl* obj_list);
    // used for putting multiple objects on the clipboard
    
  TA_MFBASEFUNS(taiObjectMimeFactory);
protected:
  void			InitHeader(int cnt, QString& str); // common for single/multi
  void			AddHeaderDesc(taBase* obj, QString& str);
    // add entry for one object
  void			AddObjectData(QMimeData* md, taBase* obj, int idx); 
    // add mime entry for one obj
private:
  void	Initialize() {}
  void	Destroy() {}
};

#endif // taiObjectMimeFactory_h
