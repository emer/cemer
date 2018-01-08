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

#ifndef ObjLibrary_h
#define ObjLibrary_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <ObjLibEl>

// declare all other types mentioned but not required to include:
class taProjVersion; //

taTypeDef_Of(ObjLibrary);

class TA_API ObjLibrary : public taNBase {
  // #CAT_File a library of objects that are stored in various locations in the file system and the web
INHERITED(taNBase)
public:
  enum LibLocs {               // library locations
    USER_LIB,                  // user's personal library -- located in app user dir (~/lib/emergent or ~/Library/Emergent)
    SYSTEM_LIB,                 // local system library, installed with software, in /usr/local/share/Emergent/
    WEB_APP_LIB,                // web-based application-specific library (e.g., emergent) -- set in obj_lib_app_wiki setting
    WEB_SCI_LIB,                // web-based scientifically oriented library (e.g., CCN) -- set in obj_lib_sci_wiki setting
    WEB_USER_LIB,               // web-based user's library (e.g., from lab wiki) -- set in obj_lib_user_wiki setting
    N_LIB_LOCS,                 // #HIDDEN number of library locations
  };

  bool           is_built; // has library list been built
  ObjLibEl_List  library; // the list of library elements
  String_Array   file_paths;    // #HIDDEN file paths corresponding to LibLocs -- for wikis this is where files are cached locally
  String_Array   wiki_names;    // #HIDDEN wiki names corresponding to LibLocs -- for local files this is localhost
  String_Array   lib_loc_names; // #HIDDEN names of all the library locations
  String         file_subdir;   // #HIDDEN name of subdirectory within various file paths to look for object files
  String         file_ext;      // #HIDDEN extension for files to add to library
  String         wiki_category; // #HIDDEN category name for wiki pages with the target object
  TypeDef*       obj_type;      // #HIDDEN base class of type of object in library

  virtual void  BuildLibrary();
  // #CAT_ObjLib if not yet is_built, search paths to find all available objects in this library

  virtual void  BrowseLibrary(LibLocs location);
  // pull up a web browser or file browser for given location 
  
  virtual int   FindNameInLocIdx(LibLocs location, const String& el_name);
  // #CAT_ObjLib find index of element with given name in library
  virtual int   FindNameInLocNmIdx(const String& lib_loc, const String& el_name);
  // #CAT_ObjLib find index of element with given name in library
  virtual ObjLibEl*  FindNameInLocEl(LibLocs location, const String& el_name);
  // #CAT_ObjLib find element with given name in library
  virtual ObjLibEl*  FindNameInLocNmEl(const String& lib_loc, const String& el_name);
  // #CAT_ObjLib find element with given name in library
  
  virtual bool  SaveToLibrary(LibLocs location, taBase* obj);
  // #CAT_ObjLib save given object to given library location -- creates new program element too if it doesn't yet already exist
  virtual bool  SaveToFile(LibLocs lib_loc, taBase* obj);
  // #CAT_ObjLib save to file
  virtual bool  SaveToWiki(LibLocs lib_loc, taBase* obj);
  // #CAT_ObjLib save to wiki

  virtual void  AddFromFiles(LibLocs lib_loc);
  // #CAT_ObjLib add items to library from given path
  virtual void  AddFromWiki(LibLocs lib_loc);
  // #CAT_ObjLib add items to library from given wiki location

  virtual ObjLibEl* NewLibRec(LibLocs lib_loc, const String& fname, const String& obj_nm);
  // #CAT_ObjLib create a new library record with initial info filled in automatically
  
  virtual bool  EnsureDownloaded(ObjLibEl* lib_el);
  // #CAT_ObjLib ensure that the latest project file has been downloaded
  
  virtual void  BuildLibrary_impl();
  // #IGNORE impl of build library
  virtual void  InitLibrary();
  // #IGNORE initialize paths etc in the library
  virtual void  InitWikiData(const String& wiki_name, const String& lib_loc_name);
  // #IGNORE init wiki

  // the following are the key classes that need to be redfined in subclasses:
  // subclasses also need to init various members above for specific types -- see ProgLib
  // as example

  virtual bool  SetLibElFromFile(ObjLibEl* lib_el);
  // #IGNORE parse dump file for name, tags, desc info, to populate ObjLibEl -- should work for most classes..
  
  virtual void  SetWikiInfoToObj(taBase*obj, const String& wiki_name) { };
  // #IGNORE subclasses must implement this to set wiki name if relevant into object, prior to uploading to wiki

  virtual void  GetWikiInfoFromObj
    (taBase* obj, String*& tags, String*& desc, taProjVersion*& version, String*& author,
     String*& email, String*& pub_cite) { };
  // #IGNORE subclasses must implement this to get wiki info from object, prior to uploading to wiki
  

  
  TA_SIMPLE_BASEFUNS(ObjLibrary);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ObjLibrary_h
