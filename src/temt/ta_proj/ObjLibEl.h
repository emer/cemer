// Copyright 2017-2017, Regents of the University of Colorado,
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

#ifndef ObjLibEl_h
#define ObjLibEl_h 1

// parent includes:
#include <taList>

// member includes:
#include <String_Array>

// declare all other types mentioned but not required to include:

taTypeDef_Of(ObjLibEl);

class TA_API ObjLibEl : public taNBase {
  // ##INLINE #CAT_File an element in an object library
INHERITED(taNBase)
public:
  String        desc;           // #EDIT_DIALOG description of what this object does and when it should be used
  String        tags;           // #EDIT_DIALOG list of comma-separated tags describing function of object
  String_Array  tags_array;     // #HIDDEN #READ_ONLY #NO_SAVE parsed version of the tags list, for internal use to actually operate on the tags
  String        lib_loc;        // #EDIT_DIALOG location within the library where this object is stored
  int           lib_loc_no;     // #HIDDEN library location number -- corresponds to enum
  String        URL;            // #EDIT_DIALOG full URL to find this object
  String        filename;       // #EDIT_DIALOG file name given to this object
  String        path;           // #EDIT_DIALOG full local file path where stored on local file system -- includes file name 
  String        date;           // #EDIT_DIALOG last modify date for this object

  virtual void  TagsToArray(); // split list of tags into tags_array
  
  String GetDesc() const override { return desc; }
  String GetTypeName() const override { return lib_loc; }
  TA_SIMPLE_BASEFUNS(ObjLibEl);
private:
  void Initialize();
  void Destroy()     { };
};


taTypeDef_Of(ObjLibEl_List);

class TA_API ObjLibEl_List : public taList<ObjLibEl> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE ##CAT_File list of object library elements
INHERITED(taList<ObjLibEl>)
public:
  
  TA_BASEFUNS_NOCOPY(ObjLibEl_List);
protected:

private:
  void  Initialize();
  void  Destroy() { };
};

#endif // ObjLibEl_h
