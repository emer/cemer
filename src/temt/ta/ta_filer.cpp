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


// ta_filer.cc

#include "ta_filer.h"

#include "ta_project.h" // for contexts in root

#ifdef TA_GUI
#  include "css_qt.h"
#endif // TA_GUI

#include <QFileInfo>
#include <QFile>

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>

//using namespace std;
#undef String
#define String taString


// THIS SHOULD BE EXTRACTED TO A SEPARATE FILE, OR EVEN USE STANDARD VERSION
// ============================================================================
// gzstream, C++ iostream classes wrapping the zlib compression library.
// Copyright (C) 2001  Deepak Bandyopadhyay, Lutz Kettner
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// ============================================================================
//
// File          : gzstream.h
// Revision      : $Revision: 1.7 $
// Revision_date : $Date: 2002/04/26 23:30:15 $
// Author(s)     : Deepak Bandyopadhyay, Lutz Kettner
//
// Standard streambuf implementation following Nicolai Josuttis, "The
// Standard C++ Library".
// ============================================================================

#include <zlib.h>

// ----------------------------------------------------------------------------
// Internal classes to implement gzstream. See below for user classes.
// ----------------------------------------------------------------------------

class gzstreambuf : public std::streambuf {
private:
    static const int bufferSize = 47+256;    // size of data buff
    // totals 512 bytes under g++ for igzstream at the end.

    gzFile           file;               // file handle for compressed file
    char             buffer[bufferSize]; // data buffer
    char             opened;             // open/close state of stream
    int              mode;               // I/O mode

    int flush_buffer();
public:
    gzstreambuf() : opened(0) {
        setp( buffer, buffer + (bufferSize-1));
        setg( buffer + 4,     // beginning of putback area
              buffer + 4,     // read position
              buffer + 4);    // end position
        // ASSERT: both input & output capabilities will not be used together
    }
    int is_open() { return opened; }
    gzstreambuf* open( const char* name, int open_mode);
    gzstreambuf* close();
    ~gzstreambuf() { close(); }

    virtual int     overflow( int c = EOF);
    virtual int     underflow();
    virtual int     sync();
};

class gzstreambase : virtual public std::ios {
protected:
    gzstreambuf buf;
public:
    gzstreambase() { init(&buf); }
    gzstreambase( const char* name, int open_mode);
    ~gzstreambase();
    void open( const char* name, int open_mode);
    void close();
    gzstreambuf* rdbuf() { return &buf; }
};

// ----------------------------------------------------------------------------
// User classes. Use igzstream and ogzstream analogously to ifstream and
// ofstream respectively. They read and write files based on the gz*
// function interface of the zlib. Files are compatible with gzip compression.
// ----------------------------------------------------------------------------

class igzstream : public gzstreambase, public std::istream {
public:
    igzstream() : std::istream( &buf) {}
    igzstream( const char* name, int open_mode = std::ios::in)
        : gzstreambase( name, open_mode), std::istream( &buf) {}
    gzstreambuf* rdbuf() { return gzstreambase::rdbuf(); }
    void open( const char* name, int open_mode = std::ios::in) {
        gzstreambase::open( name, open_mode);
    }
};

class ogzstream : public gzstreambase, public std::ostream {
public:
    ogzstream() : std::ostream( &buf) {}
    ogzstream( const char* name, int mode = std::ios::out)
        : gzstreambase( name, mode), std::ostream( &buf) {}
    gzstreambuf* rdbuf() { return gzstreambase::rdbuf(); }
    void open( const char* name, int open_mode = std::ios::out) {
        gzstreambase::open( name, open_mode);
    }
};

#include <string.h>  // for memcpy

// ----------------------------------------------------------------------------
// Internal classes to implement gzstream. See header file for user classes.
// ----------------------------------------------------------------------------

// --------------------------------------
// class gzstreambuf:
// --------------------------------------

gzstreambuf* gzstreambuf::open( const char* name, int open_mode) {
    if ( is_open())
        return (gzstreambuf*)0;
    mode = open_mode;
    // no append nor read/write mode
    if ((mode & std::ios::ate) || (mode & std::ios::app)
        || ((mode & std::ios::in) && (mode & std::ios::out)))
        return (gzstreambuf*)0;
    char  fmode[10];
    char* fmodeptr = fmode;
    if ( mode & std::ios::in)
        *fmodeptr++ = 'r';
    else if ( mode & std::ios::out)
        *fmodeptr++ = 'w';
    *fmodeptr++ = 'b';
    *fmodeptr = '\0';
    file = gzopen( name, fmode);
    if (file == 0)
        return (gzstreambuf*)0;
    opened = 1;
    return this;
}

gzstreambuf * gzstreambuf::close() {
    if ( is_open()) {
        sync();
        opened = 0;
        if ( gzclose( file) == Z_OK)
            return this;
    }
    return (gzstreambuf*)0;
}

int gzstreambuf::underflow() { // used for input buffer only
    if ( gptr() && ( gptr() < egptr()))
        return * reinterpret_cast<unsigned char *>( gptr());

    if ( ! (mode & std::ios::in) || ! opened)
        return EOF;
    // Josuttis' implementation of inbuf
    int n_putback = gptr() - eback();
    if ( n_putback > 4)
        n_putback = 4;
    memcpy( buffer + (4 - n_putback), gptr() - n_putback, n_putback);

    int num = gzread( file, buffer+4, bufferSize-4);
    if (num <= 0) // ERROR or EOF
        return EOF;

    // reset buffer pointers
    setg( buffer + (4 - n_putback),   // beginning of putback area
          buffer + 4,                 // read position
          buffer + 4 + num);          // end of buffer

    // return next character
    return * reinterpret_cast<unsigned char *>( gptr());
}

int gzstreambuf::flush_buffer() {
    // Separate the writing of the buffer from overflow() and
    // sync() operation.
    int w = pptr() - pbase();
    if ( gzwrite( file, pbase(), w) != w)
        return EOF;
    pbump( -w);
    return w;
}

int gzstreambuf::overflow( int c) { // used for output buffer only
    if ( ! ( mode & std::ios::out) || ! opened)
        return EOF;
    if (c != EOF) {
        *pptr() = c;
        pbump(1);
    }
    if ( flush_buffer() == EOF)
        return EOF;
    return c;
}

int gzstreambuf::sync() {
    // Changed to use flush_buffer() instead of overflow( EOF)
    // which caused improper behavior with std::endl and flush(),
    // bug reported by Vincent Ricard.
    if ( pptr() && pptr() > pbase()) {
        if ( flush_buffer() == EOF)
            return -1;
    }
    return 0;
}

// --------------------------------------
// class gzstreambase:
// --------------------------------------

gzstreambase::gzstreambase( const char* name, int mode) {
    init( &buf);
    open( name, mode);
}

gzstreambase::~gzstreambase() {
    buf.close();
}

void gzstreambase::open( const char* name, int open_mode) {
    if ( ! buf.open( name, open_mode))
        clear( rdstate() | std::ios::badbit);
}

void gzstreambase::close() {
    if ( buf.is_open())
        if ( ! buf.close())
            clear( rdstate() | std::ios::badbit);
}


// /END SECTION TO EXTRACT


//////////////////////////////////
// 	taFiler		//
//////////////////////////////////

#ifdef TA_OS_WIN

#else
#include <sys/stat.h>
#include <sys/wait.h>
#endif

int taFiler::buf_size = 1016;
String taFiler::last_fname;
ContextFlag taFiler::no_save_last_fname; 

taFiler* taFiler::New(const String& filetype_, 
  const String& exts_, FilerFlags flags_) 
{
  taFiler* rval = new taFiler(flags_);
  rval->m_dir = ".";
  rval->filetype = filetype_;
  rval->exts = exts_;
    
  return rval;
}

taFiler::taFiler(FilerFlags flags_) {
  Init(flags_);
}

void taFiler::Init(FilerFlags flags_) {
  if (flags_ && COMPRESS_REQ_DEF) {
    if (taMisc::save_compress)
      flags_ = (FilerFlags)(flags_ | COMPRESS_REQ);
    else
      flags_ = (FilerFlags)(flags_ & ~COMPRESS_REQ);
  }
  flags = flags_;
  fstrm = NULL;
  istrm = NULL;
  ostrm = NULL;
  open_file = false;
  select_only = false;
  file_selected = false;
  compressed = false;
  file_exists = false;
  save_paths = false;
}

taFiler::~taFiler() {
  Close();
  if (save_paths && taMisc::gui_active && tabMisc::root) {
    save_paths = false;
    tabMisc::root->Save();
  }
}

taFiler& taFiler::operator=(const taFiler& cp) {
  // can't copy an open one!
  Close();
  filter = cp.filter;
  exts = cp.exts;
  m_dir = cp.m_dir;
  m_fname = cp.m_fname;
  m_tmp_fname = cp.m_fname;
  flags = cp.flags;
  select_only = cp.select_only;
  file_selected = cp.file_selected;
  return *this;
}

void taFiler::Close() {
  compressed = false;
  file_exists = false; // exists now, but could be deleted etc.
  bool was_ostrm = false;
  if (fstrm) {
    fstrm->close();
    delete fstrm;
    fstrm = NULL;
    istrm = NULL;		// these are ptrs to fstrm
    if(ostrm) was_ostrm = true;
    ostrm = NULL;
  }
  if (istrm) {
    delete istrm;
    istrm = NULL;
  }
  if (ostrm) {
    was_ostrm = true;
    delete ostrm;
    ostrm = NULL;
  }
  open_file = false;
  file_selected = true;		// closing a file is as good as opening one
  if(was_ostrm && HasFilerFlag(TMP_SAVE_FILE) && HasFilerFlag(TMP_SAVE_FILE_USED)
     && m_tmp_fname.nonempty()) {
    String cur_fnm = FileName();
    String tmp_fnm = FileName_tmp();
    if(FileExists()) {
      QFile::remove(cur_fnm.chars());
    }
    if(FileExists()) {
      taMisc::Error("Unable to overwite existing file named", cur_fnm,
		    "for saving -- most likely you do not have appropriate permissions -- file is saved to temporary filename as:", tmp_fnm);
    }
    else {
      QFile::rename(tmp_fnm.chars(), cur_fnm.chars());
    }
  }
}

String taFiler::defExt() const {
  String rval;
  // only grab first in list as default extension
  if (exts.contains(","))
    rval = exts.before(",");
  else
    rval = exts;
  return rval;
}

String taFiler::FileName() const {
  if (m_dir.empty()) 
    return m_fname;
  else return m_dir + "/" + m_fname;
}

String taFiler::FileName_tmp() const {
  if (m_dir.empty()) 
    return m_tmp_fname;
  else return m_dir + "/" + m_tmp_fname;
}

bool taFiler::RenameFile(const String& new_fname, bool remove_existing) {
  String cur_fnm = FileName();
  if(remove_existing && QFile::exists(new_fname)) {
    if(!QFile::remove(new_fname.chars())) return false;
  }
  return QFile::rename(cur_fnm.chars(), new_fname.chars());
}

bool taFiler::FileExists() {
  return QFile::exists(FileName().chars());
}

void taFiler::GetTmpFname() {
  m_tmp_fname = "savetmp_" + m_fname;
}

const String taFiler::FilterText(bool incl_allfiles, QStringList* list) const {
  STRING_BUF(rval, 80);
  String_PArray sa_ft;
  String_PArray sa_ex;
  sa_ft.SetFromString(filetype, ",");
  sa_ex.SetFromString(exts, ",");
  // key on the exts, and just put a ? for type if we run out
  for (int i = 0; i < sa_ex.size; ++i) {
    String tft = sa_ft.SafeEl(i);
    if (tft.empty()) tft = sa_ft.SafeEl(0);
    if (tft.empty()) tft = "?";
    String tex = sa_ex.FastEl(i);
    if (tex.nonempty() && tex[0] != '.')
      tex = "." + tex;
    String itm = tft.cat(" files (");
    // note: ok if tex empty
    itm.cat("*").cat(tex);
    if (CompressEnabled()) {
      itm.cat(" *").cat(tex).cat(taMisc::compress_sfx);
    }
    itm.cat(")");
    if (rval.nonempty()) rval.cat(";;").cat(itm);
    if (list)
      *list << itm;
  }
  if (incl_allfiles) {
    String itm = "All files (*)";
    if (rval.nonempty()) rval.cat(";;");
    rval.cat(itm);
    if (list)
      *list << itm;
  }
  return rval;
}

void taFiler::FixFileName() {
  //ONLY call this on saves, not when user has already specified a name
  // General approach:
  // if m_fname is an actual existing file, we don't touch it
  if (file_exists) return;

  String base_fname = m_fname; // name w/o compress extension
  // if existing file has compression suffix, then activate compression
  if(m_fname.endsWith(taMisc::compress_sfx)) {
    compressed = true;
    base_fname = m_fname.before(taMisc::compress_sfx, -1);
  }

  // otherwise, if no ext was supplied and we have one, we apply it
  String act_ext = base_fname.after(".", -1); // the actual extension, if any
  if (act_ext.empty()) act_ext = defExt();
  String nocompr_ext = act_ext;
  if (compressed) act_ext += taMisc::compress_sfx;
  
  if (m_fname.endsWith(act_ext)) return; // done!

  // ends with basic extension but not compressed one -- just add compresed
  if(compressed && m_fname.endsWith(nocompr_ext)) {
    m_fname += taMisc::compress_sfx;
    return;
  }
  
  // if user already has an extension, don't change it
  //note: because paths can have . we have to look at file only
  QFileInfo fi(m_fname);
  String f_only = fi.baseName();
  if (!f_only.contains('.') && nocompr_ext.nonempty()) {
    // otherwise add standard extension
    m_fname += nocompr_ext;
  }
  // if compressed, and don't have compr ext, add it
  if (compressed) {
    bool has_ext = m_fname.endsWith(taMisc::compress_sfx);
    if (!has_ext)
      m_fname += taMisc::compress_sfx;
  }
}
/*
void taFiler::GetDir() {
  QFileInfo fi(m_fname);
  m_dir = fi.absolutePath(); // note: doesn't deref symlinks, which is probably what we want
  if (!m_dir.empty() && (m_dir != ".")) {
    taMisc::load_paths.AddUnique(m_dir); // make sure its on the include path..
    taMisc::css_include_paths.AddUnique(m_dir); // make sure its on the include path..
  }
}*/

bool taFiler::IsCompressed() const {
  if (IsOpen()) return compressed;
  else return m_fname.endsWith(taMisc::compress_sfx);
} 

istream* taFiler::open_read() {
  Close();
  // note: it is the filename that determines compress, not the flag
  QFileInfo qfi(FileName());
  if (m_fname.endsWith(taMisc::compress_sfx)) {
    compressed = true;
    if(qfi.isFile() && qfi.isReadable())
      istrm = new igzstream(FileName().chars(), ios::in);
  } else {
    fstrm = new fstream(FileName().chars(), ios::in);
    istrm = (istream*)fstrm;
  }
  // note: check "good" rather than "bad" because good is proactive, bad is only reactive
  if(!istrm || !istrm->good()) {
    taMisc::Error("File:",FileName(),"could not be opened for reading");
    Close();
    return NULL;
  }
  open_file = true;
  if (!no_save_last_fname)
    last_fname = FileName();
  return istrm;
}

bool taFiler::open_write_exist_check() {
  Close();
  QFileInfo qfi(FileName());
  if(qfi.isFile())
    return true;
  return false;
}

ostream* taFiler::open_write() {
  Close();
  bool hasfx = m_fname.endsWith(taMisc::compress_sfx);
  String use_fnm = FileName();
  if(FileExists() && !taMisc::FileWritable(use_fnm)) {
    int chs = taMisc::Choice("File to save (" + use_fnm + ") already exists and is not set as writable by you -- overwrite anyway?", "Overwrite", "Cancel");
    if(chs == 1) return NULL;
  }
  if(HasFilerFlag(TMP_SAVE_FILE) && FileExists()) {
    SetFilerFlag(TMP_SAVE_FILE_USED);
    GetTmpFname();
    use_fnm = FileName_tmp();
  }
  else {
    ClearFilerFlag(TMP_SAVE_FILE_USED);
    m_tmp_fname = "";		// clear this too for good measure
  }
  if (hasfx) {
    compressed = true;
    ostrm = new ogzstream(use_fnm, ios::out);
  } else {
    fstrm = new fstream(use_fnm, ios::out);
    ostrm = (ostream*)fstrm;
  }
  // note: check "good" rather than "bad" because good is proactive, bad is only reactive
  if (!ostrm->good()) {
    taMisc::Error("File",use_fnm,"could not be opened for writing -- check that you have permission to write to that location");
    Close();
    return NULL;
  }
  // success, so save/cache stuff
  open_file = true;
  if (!no_save_last_fname)
    last_fname = FileName();
  return ostrm;
}

ostream* taFiler::open_append() {
  Close();
  bool hasfx = (m_fname.endsWith(taMisc::compress_sfx));
  if (hasfx) {
    compressed = true;
    ostrm = new ogzstream(FileName().chars(), ios::out | ios::app);
  } else {
    fstrm = new fstream(FileName().chars(), ios::out | ios::app);
    ostrm = (ostream*)fstrm;
  }
  // note: check "good" rather than "bad" because good is proactive, bad is only reactive
  if (!ostrm->good()) {
    taMisc::Error("File",FileName(),"could not be opened for appending -- check that you have permission to write to that location");
    Close();
    return NULL;
  }
  open_file = true;
  if (!no_save_last_fname)
    last_fname = FileName();
  return ostrm;
}


istream* taFiler::Open() {
  file_selected = false;
  if (!GetFileName(foOpen)) return NULL;

  istream* rstrm = NULL;
  file_selected = true;
  if (open_read()) {
//      if (!select_only && (istrm->bad() || (istrm->peek() == EOF)))
//NOTE: not up to us to forbid opening an empty file!
    if (select_only)
      Close();
    else
      rstrm = istrm;
  }
  return rstrm;
}

ostream* taFiler::Save(bool tmp_fname_save) {
  if (m_fname.empty())
    return SaveAs(tmp_fname_save);
  SetFilerFlagState(TMP_SAVE_FILE, tmp_fname_save);
  if (!open_write()) {
    return SaveAs(tmp_fname_save);
  }
  return ostrm;
}

ostream* taFiler::SaveAs(bool tmp_fname_save) {
  // do a first preliminary fix, which will, ex., add the default extension
  SetFilerFlagState(TMP_SAVE_FILE, tmp_fname_save);
  FixFileName();
  bool wasFileChosen = GetFileName(foSaveAs);

  ostream* rstrm = NULL;
  if (wasFileChosen) {
    file_selected = true;
    // we fix again, because of compression
    FixFileName();
    if (open_write()) {
      if (select_only)
	Close();
      else {
	rstrm = ostrm;
      }
    }
  }
  return rstrm;
}

ostream* taFiler::Append() {
  bool wasFileChosen = GetFileName(foAppend);

  ostream* rstrm = NULL;
  if (wasFileChosen) {
    file_selected = true;
    if (open_append()) {
      if (select_only)
	Close();
      else
	rstrm = ostrm;
    }
  }
  return rstrm;
}

void taFiler::SetFileName(const String& value) {
  QFileInfo fi(value);
  m_fname = fi.fileName();
  String tdir = fi.path();
  // if no path specified, assume current, else write it
  if (tdir.empty())
    m_dir = ".";
  else
    m_dir = tdir;
}
 
#ifdef TA_NO_GUI

//////////////////////////
//   taFiler		//
//////////////////////////

// this is not called in the NO_GUI version, see ta_qtdialog.cpp

bool taFiler::GetFileName(FileOperation filerOperation) {
  return false;
}


#endif // TA_NO_GUI
