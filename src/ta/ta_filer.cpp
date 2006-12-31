// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#include <QFileInfo>

#ifdef TA_GUI
#  include "css_qt.h"
#endif // TA_GUI


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
String taFiler::last_dir;
String taFiler::last_fname;

taFiler* taFiler::New(const String& filetype_, 
  const String& ext_, FilerFlags flags_) 
{
  taFiler* rval = new taFiler(flags_);
  rval->dir = ".";
  rval->filetype = filetype_;
  rval->ext = ext_;
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
}

taFiler::~taFiler() {
  Close();
}

taFiler& taFiler::operator=(const taFiler& cp) {
  // can't copy an open one!
  Close();
  filter = cp.filter;
  ext = cp.ext;
  dir = cp.dir;
  fname = cp.fname;
  flags = cp.flags;
  select_only = cp.select_only;
  file_selected = cp.file_selected;
  return *this;
}

void taFiler::Close() {
  compressed = false;
  file_exists = false; // exists now, but could be deleted etc.
  if (fstrm) {
    fstrm->close();
    delete fstrm;
    fstrm = NULL;
    istrm = NULL;		// these are ptrs to fstrm
    ostrm = NULL;
  }
  if (istrm) {
    delete istrm;
    istrm = NULL;
  }
  if (ostrm) {
    delete ostrm;
    ostrm = NULL;
  }
  open_file = false;
  file_selected = true;		// closing a file is as good as opening one
}

const String taFiler::filterText(bool incl_allfiles) const {
  STRING_BUF(rval, 80);
  String_PArray sa_ft;
  String_PArray sa_ex;
  sa_ft.SetFromString(filetype, ",");
  sa_ex.SetFromString(ext, ",");
  // key on the exts, and just put a ? for type if we run out
  for (int i = 0; i < sa_ex.size; ++i) {
    String tft = sa_ft.SafeEl(i);
    if (tft.empty()) tft = "?";
    String tex = sa_ex.FastEl(i);
    if (rval.nonempty()) rval.cat(";;");
    rval.cat(tft).cat(" files (");
    //note: ok if ext empty
    rval.cat("*").cat(tex);
    if (compressEnabled()) {
      rval.cat(" *").cat(tex).cat(taMisc::compress_sfx);
    }
    rval.cat(")");
  }
  if (incl_allfiles) {
    if (rval.nonempty()) rval.cat(";;");
    rval.cat("All files (*)");
  }
  return rval;
}

void taFiler::FixFileName() {
  //ONLY call this on saves, not when user has already specified a name
  // General approach:
  // if fname is an actual existing file, we don't touch it
  if (file_exists) return;
  // otherwise, if no ext was supplied and we have one, we apply it
  String act_ext; // the actual extension
  if (ext.nonempty()) act_ext = ext;
  if (compressed) act_ext += taMisc::compress_sfx;
  
  if (fname.endsWith(act_ext)) return; // done!
  
  // if user already has an extension, don't change it
  //note: because paths can have . we have to look at file only
  QFileInfo fi(fname);
  String f_only = fi.baseName();
  if (!f_only.contains('.') && ext.nonempty()) {
    // otherwise add standard extension
    fname += ext;
  }
  // if compressed, and don't have compr ext, add it
  if (compressed) {
    bool has_ext = fname.endsWith(taMisc::compress_sfx);
    if (!has_ext)
      fname += taMisc::compress_sfx;
  }
}

void taFiler::GetDir() {
  QFileInfo fi(fname);
  dir = fi.absolutePath(); // note: doesn't deref symlinks, which is probably what we want
  if (!dir.empty() && (dir != ".")) {
    taMisc::load_paths.AddUnique(dir); // make sure its on the include path..
    taMisc::css_include_paths.AddUnique(dir); // make sure its on the include path..
  }
}

bool taFiler::isCompressed() const {
  if (isOpen()) return compressed;
  else return fname.endsWith(taMisc::compress_sfx);
} 

istream* taFiler::open_read() {
  Close();
  GetDir();
  int acc = access(fname, R_OK);
  if((acc != 0) && !select_only) {
    taMisc::Error("File:", fname, "could not be opened for reading");
    return NULL;
  }
  if ((acc != 0) && select_only)	// fix the file name for new files..
    FixFileName();
  open_file = true;
  last_dir = dir;
  last_fname = fname;
  // note: it is the filename that determines compress, not the flag
  if (fname.endsWith(taMisc::compress_sfx)) {
    compressed = true;
    istrm = new igzstream(fname, ios::in);
  } else {
    fstrm = new fstream(fname, ios::in);
    istrm = (istream*)fstrm;
  }
  if (istrm->bad()) {
      taMisc::Error("File:",fname,"could not be opened for reading");
      Close();
  }
  return istrm;
}

bool taFiler::open_write_exist_check() {
  Close();
  GetDir();
  int acc = access(fname, F_OK);
  if (acc == 0)
    return true;
  return false;
}

ostream* taFiler::open_write() {
  Close();
  GetDir();
  int acc = access(fname, W_OK);
  if ((acc != 0) && (errno != ENOENT)) {
    perror("open_write: ");
    taMisc::Error("File:", fname, "could not be opened for writing");
    return NULL;
  }
  open_file = true;
  last_dir = dir;
  last_fname = fname;
  bool hasfx = fname.endsWith(taMisc::compress_sfx);
  if (hasfx) {
    compressed = true;
    ostrm = new ogzstream(fname, ios::out);
  } else {
    fstrm = new fstream(fname, ios::out);
    ostrm = (ostream*)fstrm;
  }
  if (ostrm->bad()) {
    taMisc::Error("File",fname,"could not be opened for writing");
    Close();
  }
  return ostrm;
}

ostream* taFiler::open_append() {
  Close();
  GetDir();
  int acc = access(fname, W_OK);
  if((acc != 0) && (errno != ENOENT)) {
    perror("open_append: ");
    taMisc::Error("File:", fname, "could not be opened for appending");
    return NULL;
  }
  open_file = true;
  last_dir = dir;
  last_fname = fname;
  bool hasfx = (fname.endsWith(taMisc::compress_sfx));
  if (hasfx) {
    compressed = true;
    ostrm = new ogzstream(fname, ios::out | ios::app);
  } else {
    fstrm = new fstream(fname, ios::out | ios::app);
    ostrm = (ostream*)fstrm;
  }
  if (ostrm->bad()) {
    taMisc::Error("File",fname,"could not be opened for appending");
    Close();
  }
  return ostrm;
}


istream* taFiler::Open() {
  file_selected = false;
  if (!GetFileName(fname, foOpen)) return NULL;

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

ostream* taFiler::Save() {
  if (fname.empty())
    return SaveAs();

  if (!open_write()) {
    return SaveAs();
  }
  return ostrm;
}

ostream* taFiler::SaveAs() {
  bool wasFileChosen = GetFileName(fname, foSaveAs);

  ostream* rstrm = NULL;
  if (wasFileChosen) {
    file_selected = true;
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
  bool wasFileChosen = GetFileName(fname, foAppend);

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


#ifdef TA_NO_GUI

//////////////////////////
//   taFiler		//
//////////////////////////

// this is not called in the NO_GUI version, see ta_qtdialog.cpp

bool taFiler::GetFileName(String& fname, FileOperation filerOperation) {
  return false;
}


#endif // TA_NO_GUI
