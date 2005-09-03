/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

#ifndef ta_filer_h
#define ta_filer_h 1

// NOTE: taFiler replaces the duo of taiGetFile and taFile from 3.x
//	the ui portion of taiGetFile has been transferred to taFiler_impl subclass
//	in ta_filer.cc for no gui, and ta_xxdialog.cc for gui
#include "ta_type.h"

class taFiler;

//////////////////////////////////
// 	taUiFiler		//
//////////////////////////////////

enum FilerOperation {
  foOpen,
  foSave,
  foSaveAs,
  foAppend
};

// A subclass in this library will implement the GetFileName method

TA_API taFiler* taFiler_CreateInstance(const String& dir = ".", const String& filter = "", bool compress = false); // creates instance

//////////////////////////////////
// 	taFiler		//
//////////////////////////////////

class TA_API taFiler : public taRefN {
  // ##NO_TOKENS associate this with each file that is managed
friend class taRefN; // for deleting
public:
  enum OpenMode {
    NO_AUTO,			// don't automatically open
    READ,			// auto open in READ mode..
    WRITE,
    APPEND
  };

  static int	buf_size;	// size of the buffer for input operations
  static String	last_fname;	// last file name processed
  static String	last_dir;	// last directory name processed

  String	filter;
  String	dir;
  String	fname;
  bool		compress;	// true if file should be auto-compressed
  bool		select_only;	// file is to be selected only (not opened)
  ifstream*	ifstrm;		// #READ_ONLY
  char*		iofbuf;		// #IGNORE buffer for input/output
//obs  pid_t		compress_pid;	// #IGNORE process id of compress child proc
  ofstream*	ofstrm;		// #READ_ONLY
  fstream*	fstrm;		// #READ_ONLY
  istream*	istrm;		// #READ_ONLY
  ostream*	ostrm;		// #READ_ONLY
  bool		open_file;	// #READ_ONLY true if there is an open file somewhere
  bool		file_selected;	// #READ_ONLY true if a file was selected last time..
  OpenMode	mode;		// #HIDDEN auto-open file in this mode (for auto)

  virtual istream*	open_read();
  virtual ostream*	open_write();
  virtual ostream* 	open_append();
  virtual bool		open_write_exist_check(); // returns true if file already exists for writing

  void			Init(const String& dir_ = ".", const String& filter_ = "", bool compress_ = false); // same as constructor
  virtual istream*	Open(const char* nm = NULL, bool no_dlg = false);
  // to get a file for reading (already exists)
  virtual ostream*	Save(const char* nm = NULL, bool no_dlg = false);
  // to save to an existing file
  virtual ostream*	SaveAs(const char* nm = NULL, bool no_dlg = false);
  // to save with a new file
  virtual ostream*	Append(const char* nm = NULL, bool no_dlg = false);
  // to save a file for appending (already exists)
  virtual void		Close();	// close the stream

  virtual void		AutoOpen();		// auto-open a file based on mode
  virtual bool		IsOpen();		// check if file is open

  virtual bool		GetFileName(String& fname, FilerOperation filerOperation) = 0; // gui-dependent routine to get filename from user
  virtual void		FixFileName(); // make sure suffix is right
  virtual void		GetDir();      // get directory from file name


  virtual taFiler& operator=(const taFiler& cp); // copy operator used extensively in src/pdp files (copies very few props)
protected:
  taFiler();
  virtual ~taFiler();
  taFiler(const String& dir_, const String& filter_, bool compress_);
private:
  taFiler(const taFiler& cp); // #IGNORE copy construction not allowed
};


#endif // ta_filer_h
