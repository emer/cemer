/* This is part of libio/iostream, providing -*- C++ -*- input/output.
Copyright (C) 1993 Free Software Foundation

This file is part of the GNU IO Library.  This library is free
software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option)
any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this library; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

As a special exception, if you link this library with files
compiled with a GNU compiler to produce an executable, this does not cause
the resulting executable to be covered by the GNU General Public License.
This exception does not however invalidate any other reasons why
the executable file might be covered by the GNU General Public License. */

/* IMPORTANT NOTE: this is a "dummy" version of the standard libarary stream
   interface that exposes only the functions that maketa will scan, generating
   the exposed functionality that can be used in css etc */

#ifndef _STREAMBUF_H
#define _STREAMBUF_H

#ifndef EOF
#define EOF (-1)
#endif
#ifndef NULL
#ifdef __GNUG__
#define NULL (__null)
#else
#define NULL (0)
#endif
#endif

extern "C++" {

class istream;
class ostream;
class streambuf;

// In case some header files defines these as macros.
#undef open
#undef close

typedef signed long long int streampos;
typedef signed long long int streamoff;
typedef int streamsize;

#define _S_boolalpha 	0x0001
#define _S_dec 		0x0002
#define _S_fixed  	0x0004
#define _S_hex 		0x0008
#define _S_internal  	0x0010
#define _S_left    	0x0020
#define _S_oct 		0x0040
#define _S_right 	0x0080
#define _S_scientific 	0x0100
#define _S_showbase     0x0200
#define _S_showpoint 	0x0400
#define _S_showpos 	0x0800
#define _S_skipws 	0x1000
#define _S_unitbuf 	0x2000
#define _S_uppercase 	0x4000
#define _S_adjustfield 	0x00b0
#define _S_basefield 	0x004a
#define _S_floatfield 	0x0104

   // 27.4.2.1.3  Type ios_base::iostate
#define _S_badbit 	0x01
#define _S_eofbit 	0x02
#define _S_failbit     	0x04

   // 27.4.2.1.4  Type openmode
#define _S_app 		0x01
#define _S_ate 		0x02
#define _S_bin 		0x04
#define _S_in 		0x08
#define _S_out 		0x10
#define _S_trunc 	0x20

class ios {
public:
// 27.4.2.1.2  Type ios_base::fmtflags
  enum fmtflags {
    boolalpha =   _S_boolalpha,
    dec =         _S_dec,
    fixed =       _S_fixed,
    hex =         _S_hex,
    internal =    _S_internal,
    left =        _S_left,
    oct =         _S_oct,
    right =       _S_right,
    scientific =  _S_scientific,
    showbase =    _S_showbase,
    showpoint =   _S_showpoint,
    showpos =     _S_showpos,
    skipws =      _S_skipws,
    unitbuf =     _S_unitbuf,
    uppercase =   _S_uppercase,
    adjustfield = _S_adjustfield,
    basefield =   _S_basefield,
    floatfield =  _S_floatfield
  };

  // 27.4.2.1.4  Type openmode
  enum openmode {
    app =    	_S_app,
    ate =    	_S_ate,
    binary = 	_S_bin,
    in =     	_S_in,
    out =    	_S_out,
    trunc =  	_S_trunc
  };

  // 27.4.2.1.3  Type ios_base::iostate
  enum iostate {
    badbit =  	_S_badbit,
    eofbit =  	_S_eofbit,
    failbit = 	_S_failbit,
    goodbit = 	0
  };

  // 27.4.2.1.5  Type seekdir
  enum seekdir {
    beg = 		0,
    cur = 		SEEK_CUR, // 1
    end = 		SEEK_END // 2
  };

  fmtflags flags() const;
  fmtflags setf(fmtflags val);
  void unsetf(fmtflags mask);

  ostream* tie() const;
  ostream* tie(ostream* val);

  int precision() const;
  int precision(int newp);

  int width() const;
  int width(int val);

  void clear(iostate state = 0);
  void setstate(iostate flag);

  int good() const;
  int eof() const;
  int fail() const;
  int bad() const;

  void*& pword(int);
  void* pword(int) const;
  long& iword(int);
  long iword(int) const;
};

} // extern "C++"
#endif /* _STREAMBUF_H */
