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
String taFiler::last_fname;
String taFiler::last_dir;

taFiler* taFiler::New(const String& dir, const String& filter, bool compress) {
  return new taFiler(dir, filter, compress);
}

taFiler::taFiler(const String& dir_, const String& filter_, bool compress_) {
  Init(dir_, filter_, compress_);
}

void taFiler::Init(const String& dir_, const String& filter_, bool compress_) {
  dir = dir_;
  filter = filter_;
  compress = compress_;
  ifstrm = NULL;
  iofbuf = NULL;
//obs  compress_pid = 0;
  ofstrm = NULL;
  fstrm = NULL;
  istrm = NULL;
  ostrm = NULL;
  open_file = false;
  select_only = false;
  file_selected = false;
  mode = NO_AUTO;
}

taFiler::~taFiler() {
  Close();
}

taFiler& taFiler::operator=(const taFiler& cp) {
  fname = cp.fname;
  mode = cp.mode;
  compress = cp.compress;
  return *this;
}


void taFiler::AutoOpen() {
  if(open_file)
    return;
  if(mode == NO_AUTO)
    return;
  if(mode == READ)
    Open(fname);
  if(mode == WRITE)
    Save(fname);
  if(mode == APPEND)
    Append(fname);
}

bool taFiler::IsOpen() {
  return open_file;
}

void taFiler::FixFileName() {
  String ext;
  if(filter != "") {
    ext = filter.from('.');
    if(ext.contains('*'))
      ext = ext.before('*');
  }
  if(!compress) {
    bool has_ext = (fname.from((int)(fname.length() - ext.length())) == ext);
    if(!has_ext)
      fname += ext;
  }
  else {
    String sfx = taMisc::compress_sfx;
    bool has_sfx = (fname.from((int)(fname.length() - sfx.length())) == sfx);
    if(!has_sfx) {
      bool has_ext = (fname.from((int)(fname.length() - ext.length())) == ext);
      if(!has_ext)
	fname += ext;
      fname += sfx;
    }
    else {
      String raw_fn = fname.before(sfx,-1);
      bool has_ext = (raw_fn.from((int)(raw_fn.length() - ext.length())) == ext);
      if(!has_ext)
	fname = raw_fn + ext + sfx;
    }
  }
}

void taFiler::GetDir() {
  dir = fname.before('/', -1);
  if(!dir.empty() && (dir != "."))
    taMisc::include_paths.AddUnique(dir); // make sure its on the include path..
}

istream* taFiler::open_read() {
  Close();
  GetDir();
  int acc = access(fname, R_OK);
  if((acc != 0) && !select_only) {
    taMisc::Error("File:", fname, "could not be opened for reading");
    return NULL;
  }
  if((acc != 0) && select_only)		// fix the file name for new files..
    FixFileName();
  open_file = true;
  last_fname = fname;
  last_dir = dir;
  String sfx = taMisc::compress_sfx;
  if(fname.from((int)(fname.length() - sfx.length())) == sfx) {
    compress = true;		// set for compressed files..
    istrm = new igzstream(fname, ios::in);
    return istrm;
  }
  fstrm = new fstream(fname, ios::in);
  istrm = (istream*)fstrm;
  return istrm;
}

bool taFiler::open_write_exist_check() {
  Close();
  GetDir();
  int acc = access(fname, F_OK);
  if(acc == 0)
    return true;
  return false;
}

ostream* taFiler::open_write() {
  Close();
  GetDir();
  int acc = access(fname, W_OK);
  if((acc != 0) && (errno != ENOENT)) {
    perror("open_write: ");
    taMisc::Error("File:", fname, "could not be opened for writing");
    return NULL;
  }
  open_file = true;
  last_dir = dir;
  String sfx = taMisc::compress_sfx;
  bool hasfx = (fname.from((int)(fname.length() - sfx.length())) == sfx);
  if(compress || hasfx) {
    compress = true;		// set for compressed files..
    FixFileName();
    last_fname = fname;
    ostrm = new ogzstream(fname, ios::out);
    return ostrm;
  }
  else {
    FixFileName();
  }
  last_fname = fname;
  fstrm = new fstream(fname, ios::out);
  ostrm = (ostream*)fstrm;
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
  String sfx = taMisc::compress_sfx;
  bool hasfx = (fname.from((int)(fname.length() - sfx.length())) == sfx);
  if(compress || hasfx) {
    compress = true;		// set for compressed files..
    if(!hasfx)
      fname += sfx;
    last_fname = fname;
    ostrm = new ogzstream(fname, ios::out | ios::app);
    return ostrm;
  }
  last_fname = fname;
  fstrm = new fstream(fname, ios::out | ios::app);
  ostrm = (ostream*)fstrm;
  return ostrm;
}


istream* taFiler::Open(const char* nm, bool no_dlg) {
  file_selected = false;
  if (nm != NULL) {
    fname = nm;
    file_selected = true;
  }
#ifdef TA_NO_GUI
  if (no_dlg) {
    return open_read();
  } else {
    taMisc::Error("FileOpen: dialog not allowed in no-gui version of this program.");
    return NULL;
  }
#endif

#ifdef TA_GUI
  if (no_dlg || !cssiSession::in_session)
    return open_read();

  bool wasFileChosen = GetFileName(fname, foOpen);

  istream* rstrm = NULL;
  if (wasFileChosen) {
    file_selected = true;
    if(open_read() != NULL) {
      if(!select_only && (istrm->bad() || (istrm->peek() == EOF)))
	taMisc::Error("File:",fname,"could not be opened for reading or is empty");
      else if(select_only)
	Close();
      else
	rstrm = istrm;
    }
  }
  return rstrm;
#endif // TA_GUI
}

ostream* taFiler::Save(const char* nm, bool no_dlg) {
  if(nm != NULL) {
    fname = nm;
    file_selected = true;
  }
#ifdef TA_NO_GUI
  if (no_dlg) {
    return open_write();
  } else {
    taMisc::Error("FileSave: dialog not allowed in no-gui version of this program.");
    return NULL;
  }
#endif

#ifdef TA_GUI
  if (no_dlg || !cssiSession::in_session)
    return open_write();

  if(fname == "")
    return SaveAs();

  if(open_write() == NULL) {
    return SaveAs();
  }
  return ostrm;
#endif // TA_GUI
}

ostream* taFiler::SaveAs(const char* nm, bool no_dlg) {
  file_selected = false;
  if (nm != NULL) {
    fname = nm;
    file_selected = true;
  }
#ifdef TA_NO_GUI
  if (no_dlg) {
    return open_write();
  } else {
    taMisc::Error("FileSaveAs: dialog not allowed in no-gui version of this program.");
    return NULL;
  }
#endif

#ifdef TA_GUI
  if (no_dlg || !cssiSession::in_session)
    return open_write();

  bool wasFileChosen = GetFileName(fname, foSaveAs);

  ostream* rstrm = NULL;
  if (wasFileChosen) {
    file_selected = true;
    bool no_go = false;
    if(!select_only && open_write_exist_check()) {
      int chs = taMisc::Choice("File: "+fname+" exists, overwrite?", "Ok", "Cancel");
      if(chs == 1) no_go = true;
    }
    if(!no_go && (open_write() != NULL)) {
      if(ostrm->bad())
	taMisc::Error("File:",fname,"could not be opened for SaveAs");
      else if(select_only)
	Close();
      else {
	rstrm = ostrm;
      }
    }
  }
  return rstrm;
#endif // TA_GUI
}

ostream* taFiler::Append(const char* nm, bool no_dlg) {
  file_selected = false;
  if(nm != NULL) {
    fname = nm;
    file_selected = true;
  }
#ifdef TA_NO_GUI
  if(no_dlg) {
    return open_append();
  } else {
    taMisc::Error("FileAppend: dialog not allowed in no-gui version of this program.");
    return NULL;
  }
#endif

#ifdef TA_GUI
  if (no_dlg || !cssiSession::in_session)
    return open_append();

  bool wasFileChosen = GetFileName(fname, foAppend);

  ostream* rstrm = NULL;
  if (wasFileChosen) {
    file_selected = true;
    if(open_append() != NULL) {
      if(ostrm->bad())
	taMisc::Error("File",fname,"could not be opened for append");
      else if(select_only)
	Close();
      else
	rstrm = ostrm;
    }
  }
  return rstrm;
#endif // TA_GUI
}

void taFiler::Close() {
  if(fstrm != NULL) {
    fstrm->close();
    delete fstrm;
    fstrm = NULL;
    istrm = NULL;		// these are ptrs to fstrm
    ostrm = NULL;
  }
  if(ifstrm != NULL) {
    ifstrm->close();
    delete ifstrm;
    ifstrm = NULL;
    istrm = NULL;
  }
  if(ofstrm != NULL) {
#ifdef SOLARIS
    int fd = ofstrm->rdbuf()->fd();
#endif
    ofstrm->close();
#ifdef SOLARIS
    close(fd);			// just in case -- doesn't get closed on solaris.
#endif
    delete ofstrm;
    ofstrm = NULL;
    ostrm = NULL;
  }
  if(istrm != NULL) {
    delete istrm;
    istrm = NULL;
  }
  if(ostrm != NULL) {
    delete ostrm;
    ostrm = NULL;
  }
  if(iofbuf != NULL) {
    delete [] iofbuf;
    iofbuf = NULL;
  }
/* note: 99.99% sure obsolete
  if(compress_pid > 0) {	// must wait around for child to die..
    pid_t wait_pid;
    int wstatus;
    do {
      wait_pid = waitpid(compress_pid, &wstatus, 0);
    } while((wait_pid == -1) && (errno == EINTR));
    compress_pid = 0;
  }
*/
  open_file = false;
  file_selected = true;		// closing a file is as good as opening one
}


#ifdef TA_NO_GUI

//////////////////////////
//   taFiler		//
//////////////////////////

bool taFiler::GetFileName(String& fname, FilerOperation filerOperation) {
  bool result = false;
 /*  TODO: implement, except maybe it isn't even called in TA_NO_GUI version!!!
  chooser->style()->attribute("filter", "on");
  chooser->style()->attribute("defaultSelection", fname);
  chooser->style()->attribute("filterPattern", filer->filter);
  ivCoord x=0; ivCoord y=0;
  String win_title;
  switch (filerOperation) {
  case foOpen:
    chooser->style()->attribute("open", "Open");
    chooser->style()->attribute("caption", "Select File to Open for Reading");
    win_title = String("Open: ") + filer->filter;
    break;
  case foSave:
    // TODO: will this ever be called???
    return false;
    break;
  case foSaveAs:
    chooser->style()->attribute("open", "Save");
    chooser->style()->attribute("caption", "Select File to Save for Writing");
    win_title = String("Save: ") + filer->filter;
    break;
  case foAppend:
    chooser->style()->attribute("open", "Append");
    chooser->style()->attribute("caption", "Select File to Append for Writing");
    win_title = String("Append: ") + filer->filter;
    break;
  }

  chooser->style()->attribute("name", win_title);
  chooser->style()->attribute("title", win_title);
  chooser->style()->attribute("iconName", win_title);
  taiDialog::GetPointerPos(x,y);
  result = (chooser->post_at_aligned(x, y, POINTER_ALIGN_X, POINTER_ALIGN_Y) != 0);

  if (result) {
    fname = chooser->selected()->string();
  }

  if(chooser != NULL) {
    ivResource::unref(chooser);
    ivResource::flush();
    chooser = NULL;
  }
 */
  return result;
}

taFiler* taFiler_CreateInstance(const String& dir, const String& filter, bool compress) {
  return new taFiler_impl(dir, filter, compress);
}


#endif // TA_NO_GUI
