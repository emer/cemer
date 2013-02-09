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

#ifndef taiObjectMimeItem_h
#define taiObjectMimeItem_h 1

// parent includes:
#include <taiMimeItem>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taiObjectMimeItem);

class TA_API taiObjectMimeItem: public taiMimeItem { // for tacss object
INHERITED(taiMimeItem)
friend class taiObjectsMimeItem;
friend class taiMimeSource; //TEMP
public:
  inline String		typeName() {return m_type_name;};
  inline String		path() {return m_path;}
  TypeDef*		td() {return m_td;}
  taBase*		obj(); // note: only valid when in-process
  
#ifndef __MAKETA__
  int			GetObjectData(std::istringstream& result);
  // #IGNORE gets the object data for the current item; returns number of bytes
#endif

  TA_BASEFUNS_NOCOPY(taiObjectMimeItem);
protected:
  String		m_type_name;
  TypeDef*		m_td;
  String		m_path;
  taBase*		m_obj;
  override bool		Constr_impl(const String&);
  override void		DecodeData_impl();
private:
  void	Initialize();
  void	Destroy() {}
};

#endif // taiObjectMimeItem_h
