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

#ifndef taiFileDialogField_h
#define taiFileDialogField_h 1

// parent includes:
#include <taiText>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiFileDialogField : public taiText {
  // for FILE_DIALOG_xxx strings
  Q_OBJECT
  INHERITED(taiText)
public:
  enum FileActionType {
    FA_LOAD,
    FA_SAVE,
    FA_APPEND,
  };

  taiFileDialogField(TypeDef* typ_, IWidgetHost* host, taiData* par, QWidget* gui_parent_,
                     int flags = 0, FileActionType fact=FA_LOAD, const String& fext = "",
                     const String& ftyp = "", int fcmprs = -1);

protected slots:
  override void         btnEdit_clicked(bool);
  override void         lookupKeyPressed(); // remap to file dialog

public:
  FileActionType        file_act;       // file action to take for file dialog
  String                file_ext;       // file extention list (from EXT_xxx)
  String                file_type;      // file type information (from FILETYPE_xxx)
  int                   file_cmprs;     // compress?  from #COMPRESS -1 = default, 0 = no, 1 = yes
  taBase*               base_obj;       // taBase object for saving/loading
};


#endif // taiFileDialogField_h
