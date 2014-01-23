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

#ifndef ScriptBase_h
#define ScriptBase_h 1

// parent includes:
#include <AbstractScriptBase>

// member includes:

// declare all other types mentioned but not required to include:
class taFiler; // 


taTypeDef_Of(ScriptBase);

class TA_API ScriptBase: public AbstractScriptBase {
  // #VIRT_BASE #NO_INSTANCE class for adding a script to other objects
public:
  taFiler*      script_file;    // file to use for the script
  String        script_string; // #EDIT_DIALOG script text to use for the script
  String        script_filename; // #HIDDEN #READ_ONLY #FILE_DIALOG_LOAD file name for the script -- used only for saving and loading

  ScriptSource scriptSource() CPP11_OVERRIDE;
  const String scriptFilename() CPP11_OVERRIDE;
  const String scriptString() CPP11_OVERRIDE {return script_string;}

  virtual void  LoadScript(const String& file_nm);
  // load script from script file and compile it
  virtual void  LoadScriptString(const String& string);
  // load script from script string and compile it
  virtual void  SetScript(const String& file_nm);
  // set the script file name (also clears script string)
  virtual void  UpdateScriptFile();
  // #IGNORE call this function in the UpdateAfterEdit of object to ensure script file name is properly saved and loaded

  void          Copy_(const ScriptBase& cp);
  ScriptBase();
  ~ScriptBase();
};

#endif // ScriptBase_h
