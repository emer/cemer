// Copyright, 1995-2007, Regents of the University of Colorado,
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


#ifndef ta_filer_h
#define ta_filer_h 1

#include "ta_type.h"

// NOTE: taFiler replaces the duo of taiGetFile and taFile from 3.x
//      the ui portion of taiGetFile has been transferred to taFiler_impl subclass
//      in ta_filer.cc for no gui, and ta_xxdialog.cc for gui

/*
  compression
  Compression is controlled by 3 factors:
    comp_req flag -- when set, it acts as follows:
      Load: shows .gz files
      Save: shows the compression checkbox in file dialog
        shows .gz files in the file dialog
      SaveAs: shows the compression checkbox in file dialog
        if proposed filename has gz suffix, 'compress' check is on
    compression flag -- this is readonly and always indicate that the current
      stream is compressed
    .gz file suffix -- when present, we assume the file is compressed

    On Load, compression is SOLELY determined by the suffix of the file.
    On Save, compression is determined as follows:
      a .gz file name on the file
      the 'compress' checkbox is selected

*/

// class QStringList; // #IGNORE

class TA_API taFiler : public taRefN {
  // ##NO_TOKENS #EDIT_INLINE associate this with each file that is managed
friend class taRefN; // for deleting
friend class taiStreamArgType; // we let it access the fstrm directly, for fstream arg types
public:
  enum FileOperation {
    foOpen,
    foSave,
    foSaveAs,
    foAppend
  }; //

  // Would have liked to define the enum this way, but maketa won't have it!
  // DEF_FLAGS                = CONFIRM_OVERWRITE | FILE_MUST_EXIST | COMPRESS_ENABLE
  // DEF_FLAGS_COMPRESS       = DEF_FLAGS | COMPRESS_REQ_DEF

  enum FilerFlags { // #BITS flags to control operations
    NO_FLAGS    = 0,    // #NO_BIT
    CONFIRM_OVERWRITE           = 0x001, // for Save/Append, warn if file already exists
    FILE_MUST_EXIST             = 0x002, // for Open, file must exist
    COMPRESS_ENABLE             = 0x004, // enables use of compression/compressed files
    COMPRESS_REQ                = 0x008, // if compr enabled, requests it by default
    COMPRESS_REQ_DEF            = 0x100, // use taiMisc default for compression
    TMP_SAVE_FILE               = 0x200, // save to a temporary file name if the save file exists
    TMP_SAVE_FILE_USED          = 0x400, // a temporary save file was actually used on the last save -- Close function will rename

    DEF_FLAGS                   = 0x007, // #NO_BIT default flags for no compression (compression is always enabled by default, just not requested)
    DEF_FLAGS_COMPRESS          = 0x107 // #NO_BIT default flags for compression w/ autocompress
  };

  static int buf_size;          // #HIDDEN #NO_SAVE size of the buffer for input operations
  static String last_fname;     // #HIDDEN #NO_SAVE last filename processed
  static ContextFlag no_save_last_fname; // #IGNORE hack to prevent recursive saves from saving fname

  static taFiler*       New(const String& filetypes = "All",
    const String& exts = _nilString, FilerFlags flags = DEF_FLAGS);
  // creates instance; filetype/ext are ,separated lists; ext items in a file are space-separated; context is for saving/restoring last_dir ('' = "(none"))

  String        filter;         // the QFileDialog-compatible filter expression
  String        filetype;       // the filetype, ex Project
  bool          select_only;    // file is to be selected only (not opened)

  istream*      istrm;          // #READ_ONLY #NO_SAVE
  ostream*      ostrm;          // #READ_ONLY #NO_SAVE
  bool          open_file;      // #READ_ONLY #NO_SAVE true if there is an open file somewhere
  bool          file_selected;  // #READ_ONLY #NO_SAVE true if a file was selected last time..

  String        defExt() const; // the default extension (if any) including '.'

  inline void           SetFilerFlag(FilerFlags flg) { flags = (FilerFlags)(flags | flg); }
  // set flag state on
  inline void           ClearFilerFlag(FilerFlags flg) { flags = (FilerFlags)(flags & ~flg); }
  // clear flag state (set off)
  inline bool           HasFilerFlag(FilerFlags flg) const { return (flags & flg); }
  // check if flag is set
  inline void           SetFilerFlagState(FilerFlags flg, bool on)
  { if(on) SetFilerFlag(flg); else ClearFilerFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)

  inline bool           CompressEnabled() const {return HasFilerFlag(COMPRESS_ENABLE);}
  // true if client enabled use of compression
  inline bool           CompressReq() const
  { return HasFilerFlag(COMPRESS_REQ) && HasFilerFlag(COMPRESS_ENABLE);}
  // true if client requested compression AND it is enabled
  bool                  IsOpen() const {return open_file;}
  // true if file is open
  bool                  IsCompressed() const;
  // if open, indicates actual compr status, else indicates .gz suffix on fname

  const String          FilterText(bool incl_allfiles = true, QStringList* list = NULL) const;
  // #IGNORE Qt-compatible filter text

  String                FileName() const; // full file name: concats dir and fname
  String                FileName_tmp() const; // full temporary file name: concats dir and tmp_fname
  void                  SetFileName(const String& value); // parses out into dir and name, setting in us
  inline String         dir() const {return m_dir;} // the directory path to the file
  inline String         fname() const {return m_fname;} // the name (no path) of the file
  void                  SetFname(const String& val) {m_fname = val;}
  bool                  RenameFile(const String& new_fname, bool remove_existing = false);
  // rename current file name to given file -- uses QFile::rename (and remove before if currently exists and remove_existing = true) -- returns success in renaming (true = success)
  bool                  FileExists();
  // check if the current file already exists -- true if yes, false if no
  void                  GetTmpFname();
  // get a temporary file name into m_tmp_fname

  // the low-level api functions work directly on the fname
  virtual istream*      open_read();
  // #IGNORE
  virtual ostream*      open_write();
  // #IGNORE
  virtual ostream*      open_append();
  // #IGNORE
  virtual bool          open_write_exist_check();
  // returns true if file already exists for writing

  virtual istream*      Open();
  // #MENU to open an existing file for reading; starts with curr filename if any
  virtual ostream*      Save(bool tmp_fname_save = true);
  // #MENU to save to an existing file; requests a new filename if doesn't exist  -- if tmp_fname_save is true, and chosen file name already exists, saves to a temporary file and is then renamed to target file on Close() -- provides safety in case of crashing
  virtual ostream*      SaveAs(bool tmp_fname_save = true);
  // #MENU to save to a new file -- if tmp_fname_save is true, and chosen file name already exists, saves to a temporary file and is then renamed to target file on Close() -- provides safety in case of crashing
  virtual ostream*      Append();
  // #MENU to open an existing file for appending
  virtual void          Close();
  // #MENU close the stream (will also manage temporary file renaming if necessary)

  virtual bool          GetFileName(FileOperation filerOperation); // gui-dependent routine to get filename from user -- it is set in dir/fname; false if user cancels
  virtual void          FixFileName(); // make sure suffix is right

  virtual taFiler& operator=(const taFiler& cp); // copy operator used extensively in src/pdp files (copies very few props)
protected:
  String        m_dir;          // the directory path to the file
  String        m_fname;        // the name (no path) of the file
  String        m_tmp_fname;    // the name (no path) of the temporary file, if used
  String        exts;           // the default extensions (if any), including '.'; comma-sep
  FilerFlags    flags;
  fstream*      fstrm;          // #READ_ONLY the underlying file
  bool          compressed; // set when file has .gz suffix
  bool          file_exists; // set by GetFileName if the file actually exists
  bool          save_paths; // save path changes, but only on Close (*after* file save)

  taFiler(FilerFlags flags);
  virtual ~taFiler();
private:
  taFiler(const taFiler& cp); // #IGNORE copy construction not allowed
  void  Init(FilerFlags flags_); // same as constructor
};

namespace taFilerUtil
{
  String dottedExtension(const String &extension);
  String undottedExtension(const String &extension);
  String getFiletype(int idx, const String_PArray &filetypes);
  String getDottedExtension(int idx, const String_PArray &extensions);
  String makeFilter(const String &filetype, const String &extension, bool isCompressEnabled);
  void addFilter(const String &newFilter, String &filters, QStringList *list);
}

#endif // ta_filer_h
