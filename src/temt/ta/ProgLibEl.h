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

#ifndef ProgLibEl_h
#define ProgLibEl_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <String_Array>

// declare all other types mentioned but not required to include:
class Program_Group; //
class Program; //

taTypeDef_Of(ProgLibEl);

class TA_API ProgLibEl: public taNBase {
  // #INSTANCE #INLINE #CAT_Program an element in the program library
INHERITED(taNBase)
public:
  bool          is_group;       // this is a group of related programs
  String        desc;           // #EDIT_DIALOG description of what this program does and when it should be used
  String        tags;           // #EDIT_DIALOG list of comma-separated tags describing function of program
  String_Array  tags_array;     // #HIDDEN #READ_ONLY #NO_SAVE parsed version of the tags list, for internal use to actually operate on the tags
  String        lib_name;       // #EDIT_DIALOG name of library that contains this program
  String        URL;            // #EDIT_DIALOG full URL to find this program
  String        filename;       // #EDIT_DIALOG file name given to this program
  String        date;           // #EDIT_DIALOG last modify date for this program

  virtual taBase* NewProgram(Program_Group* new_owner);
  // #MENU #MENU_ON_Object #MENU_CONTEXT create a new program of this type (return value could be a Program or a Program_Group)
  virtual bool  LoadProgram(Program* prog);
  // load into given program; true if loaded, false if not
  virtual bool  LoadProgramGroup(Program_Group* prog_gp);
  // load into given program; true if loaded, false if not

  virtual bool  ParseProgFile(const String& fnm, const String& path);
  // get program information from program or program group file. is_group is set based on extension of file name (.prog or .progp)

  virtual void  ParseTags(); // parse list of tags into tags_array

  String GetDesc() const CPP11_OVERRIDE { return desc; }
  String GetTypeName() const CPP11_OVERRIDE { return lib_name; }
  // This shows up in chooser instead of ProgLibEl!

  TA_SIMPLE_BASEFUNS(ProgLibEl);
protected:

private:
  void  Initialize();
  void  Destroy();
};

#endif // ProgLibEl_h
