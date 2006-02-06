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


/*
Copyright (C) 1988 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
*/

// all unadorned references to string are hereby taString references
// which should be used in the case that there are multiple conflicting string
// types in use.

#ifdef String
#undef String
#endif
#define String taString
#define StrRep taStrRep

#ifndef ta_string_h
#define ta_string_h 1

// ta_string is the string class used in the TA/CSS software
// it is **very loosely** based on the gnu libg++ string class, with the following
// modifications:
// - strings can be of any length (memory permitting, of course)
// - strings use reference counting to instance data -- simple string copies are just pointer ops
// - constructors and conversion operators to/from ints, floats, and doubles
// - removal of the regexp versions of functions
// - reordering and commenting of overloaded functions so that the
// 	const char* version is always the last one, so that it will
//	be the one scanned by TA
// - allocation failures cause console error to print, but the program won't crash
//
// **NOTE** prior to performing any in-place modification to the string, you must call makeUnique()

#include "ta_def.h"
#ifdef __MAKETA__
# include <iostream.h>
#else
  #include <iostream>
  #ifdef TA_USE_QT
    #include <QString>
  #endif
#endif // __MAKETA__


class TA_API StrRep;
class TA_API String;

/* StrRep - internal String representation

  Note: this structure is allocated via malloc(), so there must be no constructor/destructor.
*/

class TA_API StrRep {
friend class String;
public:
  uint			len;    // string length (not including null terminator)
  uint			sz;     // allocated space ((not including null terminator)
  uint			cnt;	// reference count (when goes to 0, instance is deleted)
  char              	s[1];   // the string starts here, null terminator always maintained
protected:
  bool			canCat(uint xtra_len) {return ((cnt == 1) && ((sz - len) >= xtra_len));}  // true if ref==1, and enough space to add
  void			ref() {++cnt;}
  void 			unRef() {if (--cnt == 0) free(this);}
  void			cat(const char* str, uint slen); // note: slen must be set, and canCat must have been true
  void			upcase();	// convert all letters to upper case; only called if cnt<=1
  void			downcase();	// convert all letters to lower case; only called if cnt<=1
  void			capitalize(); // capitalize the first letter of each word; only called if cnt<=1
  void			reverse();	// reverse order of string; only called if cnt<=1
  void			prepend(const char* str, uint slen); // note: slen must be set, and canCat must have been true
  int               	search(int, int, const char*, int = -1) const;
  int               	search(int, int, char) const;
  int               	match(int, int, int, const char*, int = -1) const;
};

extern TA_API StrRep  _nilStrRep; // an empty StrRep, for convenience

// primitive ops on StrReps -- nearly all String fns go through these.

/*
  s - pointer to the string; can be NULL
  slen -- len of the string if known; -1 means call strlen function
  cap -- capacity; 0 means use the string len
*/
StrRep*		Snew(int slen, uint cap = 0); // for an empty rep, for filling by caller; len is set
StrRep*		Salloc(const char* s, int slen = -1, uint cap = 0); // the most-used alloc
StrRep*		Scat(StrRep* srep, const char* s, uint slen = -1);
StrRep*		Scat(const char* s1, int slen1, const char* s2, int slen2); // slen can be -1
StrRep*		Sreverse(const StrRep* x);

class TA_API String { // reference counted string
friend class StrRep;
public:
// constructors & assignment
  inline String() {newRep(&_nilStrRep);} // el blanco
  inline String(const String& x) {newRep(x.mrep);} // copy constructor -- only a ref on source! (fast!)
  inline String(const char* s) {init(s, -1);} // s can be NULL
  String(const char* s, int slen) {init(s, slen);}
  String(StrRep* x) {newRep(x);} // typically only used internally
  String(uint slen, uint sz, char fill); // for allocating a writeable buffer; (1) if sz==0, then sz=slen; (2) if fill==0, then slen will be forced to 0


// conversion constructors
  String(char c) {if (c == '\0') newRep(&_nilStrRep); else init(&c, 1);}
  String(int i, const char* format = "%d");
  String(uint u, const char* format = "%u");
  String(long i, const char* format = "%ld"); //note: don't use long any more, compatibility only
  String(ulong u, const char* format = "%lu"); //note: don't use long any more, compatibility only
  String(int64_t i64, int base = 10);
  String(uint64_t u64, int base = 10);
  String(float f, const char* format = "%g");
  String(double d, const char* format = "%lg");
  String(void* p); //converts to hex
#ifdef TA_USE_QT
  String(const QString& val);
  String&           operator = (const QString& y);
  operator QString() const; //
#endif

  ~String() {mrep->unRef();}
  void			setRep(StrRep* rep_); // for replacing rep (non-constructor) -- rep_ must be non-null

  unsigned int		length() const {return mrep->len;} // how many characters in the string
  bool			empty() const {return mrep->len == 0;}  // true if the string is empty
  bool			isInt() const;  // true if the string contains a value that can be interpreted as an integer [+-]dd*

  int			allocation() const {return mrep->sz;}
  int			assertLength(); // call after manual buffer operations, to set length from null term -- **WARNING** only use on ref==1, and where there is a null term in the buff, and sz is not overflowed
  const char*		chars() const {return mrep->s;} // returns null-terminated string DO NOT MODIFY
  char*			chars_ptr() {makeUnique(); return mrep->s;} // returns point to the string, for in-place mods -- **this makeUnique() first, so you have a unique instance to overwrite**

  void			makeUnique(); // make sure we are only owner of mrep
  void			makeUnique(uint min_new_sz); // makeUnique(), but force sz to be at least new_sz
  void			pack(); // if there is any extra space (above a small threshold), release it

// these are strangely missing in the original
  operator unsigned char() const { return (unsigned char)strtol(chars(), NULL, 0); } 
    // note: this is for 'byte' conversion -- we hope that char per-se is 'signed char'
  operator int() const	{ return strtol(chars(), NULL, 0); }
  operator long() const	{ return strtol(chars(), NULL, 0); }
  operator float() const { return (float)atof(chars()); }
  operator double() const { return atof(chars()); }
  operator char*() const { return (char*)chars(); } //
  
  bool 			toBool() const; // accepts true as starting with t/T, or else 1
  char 			toChar() const; // if size 1, then that's it, otherwise 0
  
  // converter routines, NOTE: ok only works when linked to Qt! 
  short 		toShort(bool* ok = 0, int base = 10) const; 
  ushort 		toUShort(bool* ok = 0, int base = 10) const; 
  int 			toInt(bool* ok = 0, int base = 10) const; 
  uint 			toUInt(bool* ok = 0, int base = 10) const; 
  int64_t 		toInt64(bool* ok = 0, int base = 10) const; 
  uint64_t 		toUInt64(bool* ok = 0, int base = 10) const; 
  float 		toFloat(bool* ok = 0) const; 
  double 		toDouble(bool* ok = 0) const; 

  String&		convert(int i, const char* format = "%d");
  String&		convert(long i, const char* format = "%ld");
  String&		convert(float f, const char* format = "%g");
  String&		convert(double f, const char* format = "%lg");

  int 			HexToInt() const { return strtol(chars(), NULL, 16); }
  
  String&		operator=(const String& y) {setRep(y.mrep); return *this;}
  String&		operator=(const char* s) {return set(s, -1);}
  String&		operator=(char c) {if (c == '\0') 
    {setRep(&_nilStrRep); return *this;} else return set(&c, 1);} //

  String&           set(const char* t, int len); 
    // parameterized set -- used in assigns

// concatenation -- note: we try to do it in place, if possible (if refs==1, and enough space)
  String&		cat(const String& y);
  String&		cat(char c);
  String&		cat(const char* t);

  String&		prepend(const String& y);
  String&		prepend(char c);
  String&		prepend(const char* t); // add to beginning of string

  String&		operator += (const String& y) {return cat(y);}
  String&		operator += (const char* t) {return cat(t);}
  String&		operator += (char c) {return cat(c);}


// procedural versions:

  friend inline void     cat(const String&, const String&, String&);
  friend inline void     cat(const String&, const char*, String&);
  friend inline void     cat(const String&, const char* str, uint slen, String&); // slen must be set
  friend inline void     cat(const String&, char, String&);
  friend inline void     cat(const char*, const String&, String&);
  friend inline void     cat(const char*, const char*, String&);
  friend inline void     cat(const char*, char, String&);
    // concatenate first 2 args, store result in last arg

// searching & matching
  int               index(char        c, int startpos = 0) const;
  int               index(const String&     y, int startpos = 0) const;
  int               index(const char* t, int startpos = 0) const;
    // return position of target in string or -1 for failure

  bool               contains(char        c) const;
  bool               contains(const String&     y) const; 
    // return 'true' if target appears anyhere in String

  bool               contains(char c, int pos) const;
  bool               contains(const String& y, int pos) const;
  bool               contains(const char* t, int pos) const;
  bool               contains(const char* t) const;
    // return 'true' if target appears anywhere after position pos (or before, if pos is negative) in String

  bool               matches(char c, int pos = 0) const;
  bool               matches(const String& y, int pos = 0) const;
  bool               matches(const char* t, int pos = 0) const;
    // return 'true' if target appears at position pos in String

  int               freq(char        c) const;
  int               freq(const String&     y) const;
  int               freq(const char* t) const;
    //  return number of occurences of target in String

// String extraction
  String         operator () (int pos, int len) const; // synonym for at

  String         at(const String&  x, int startpos = 0) const;
  String         at(const char* t, int startpos = 0) const;
  String         at(char c, int startpos = 0) const;
  String         at(int pos, int len) const;
  // substring at position for length

  String         before(int pos) const;
  String         before(const String& x, int startpos = 0) const;
  String         before(char c, int startpos = 0) const;
  String         before(const char* t, int startpos = 0) const; 
  // substring before (not including) target string

  String         through(int pos) const;
  String         through(const String& x, int startpos = 0) const;
  String         through(char c, int startpos = 0) const;
  String         through(const char* t, int startpos = 0) const;
  // substring through (including) target string

  String         from(int pos) const;
  String         from(const String& x, int startpos = 0) const;
  String         from(char c, int startpos = 0) const;
  String         from(const char* t, int startpos = 0) const;
  // substring from (including) target string

  String         after(int pos) const;
  String         after(const String& x, int startpos = 0) const;
  String         after(char c, int startpos = 0) const;
  String         after(const char* t, int startpos = 0) const;
  // substring after (not including) target string


// deletion


// delete the first occurrence of target after startpos

  void              del(const String& y, int startpos = 0);
  void              del(const char* t, int startpos = 0);
  void              del(char c, int startpos = 0);
  void              del(int pos, int len);
// delete len chars starting at pos
  void              remove(const char* t, int startpos = 0) { del(t, startpos); }
// remove target string from string

// global substitution: substitute all occurrences of pat with repl

  int               gsub(const String&     pat, const String&     repl);
  int               gsub(const char* pat, const String&     repl);
  int               gsub(const char* pat, const char* repl);
// global substitution: substitute all occurrences of pat with repl

// friends & utilities

// split string into array res at separators; return number of elements

  friend int        split(const String& x, String res[], int maxn,
                          const String& sep);

  friend String     common_prefix(const String& x, const String& y,
                                  int startpos = 0);
  friend String     common_suffix(const String& x, const String& y,
                                  int startpos = -1);
  friend String     replicate(char c, int n);
  friend String     replicate(const char* y, int n);
  friend String     replicate(const String& y, int n);
  friend String     join(const String src[], int n, const String& sep);

// simple builtin transformations
  friend inline String     reverse(const String& x);
  friend inline String     upcase(const String& x);
  friend inline String     downcase(const String& x);
  friend inline String     capitalize(const String& x);

// in-place versions of above -- they automatically makeUnique
  String&		reverse();	// reverse order of string
  String&		upcase();	// convert all letters to upper case
  String&		downcase();	// convert all letters to lower case
  String&		capitalize(); // capitalize the first letter of each word

  void			truncate(uint new_len); // shortens the string to new_len (if less than curr)
// element extraction
  char              operator [] (int i) const;
  char&             operator [] (int i); //writable -- note: every use calls makeUnique
  char              elem(int i) const; // get the character at index i
  char              firstchar() const {return mrep->s[0];} // get the first character
  char              lastchar() const {return mrep->s[mrep->len - 1];}  // get the last character


// conversion
                    operator const char*() const {return mrep->s;}


// IO
#ifdef __MAKETA__
  friend ostream&   operator<<(ostream& s, const String& x);
  friend istream&   operator>>(istream& s, String& x);

  friend int        readline(istream& s, String& x,
                             char terminator = '\n',
                             int discard_terminator = 1); //
#else
  friend std::ostream&   operator<<(std::ostream& s, const String& x);
  friend std::istream&   operator>>(std::istream& s, String& x);

  friend int        readline(std::istream& s, String& x,
                             char terminator = '\n',
                             int discard_terminator = 1);
#endif
// status
  void     error(const char* msg) const;

//obs  int               OK() const;	// check if the string is allocated properly, etc.
protected:
  StrRep*		mrep;   // Strings are pointers to their representations
  void			init(const char* s, int slen = -1); // for calling in constructors
  void			newRep(StrRep* rep_); // for setting rep in a constructor

// some helper functions
  int               	search(int start, int sl, const char* t, int tl = -1) const
  	{return mrep->search(start, sl, t, tl);}
  int               	search(int start, int sl, char c) const {return mrep->search(start, sl, c);}
  int               	match(int start, int sl, int exact, const char* t, int tl = -1) const
  	{return mrep->match(start, sl, exact, t, tl);}
  int               	_gsub(const char*, int, const char* ,int);
  String         	_substr(int pos, int slen) const; // return a substring
private:
  int               	_gsub_lt(const char*, int, const char* ,int);
  int               	_gsub_eq(const char*, int, const char*);
  int               	_gsub_gt(const char*, int, const char* ,int);
};

//extern String _nilString; // an empty string, for convenience
//note: can't use global static, because ctor not guarenteed to run before use
#define _nilString taString()

// this is provided for placed instances, where memory is already supplied
// the owner of a placed versions MUST call s->~String() manually, to ensure proper destruction
  
inline void* operator new(size_t, String* str) { return str; }

// other externs

int        TA_API compare(const String& x, const String& y);
int        TA_API compare(const String& x, const char* y);
int        TA_API compare(const char* y, const String& x);
int        TA_API compare(const String& x, char y);
int        TA_API compare(char y, const String& x);
int        TA_API fcompare(const String& x, const String& y); // ignore case



#ifdef TA_USE_QT
// for some reason, these need to be here (inline), otherwise implicit use of these
// functions gets reported as "undefined reference to xxxx" by the linker... weird...
inline String::String(const QString& y) {
  init(y.toLatin1(), y.length());
}

inline String::operator QString() const {
  QString result(chars());
  return result;
}

inline String& String::operator = (const QString& y) {
  return set(y.toLatin1(), y.length());
}

  // converter routines, for use when linked with Qt 
inline short String::toShort(bool* ok, int base) const 
  {QString tmp(chars()); return tmp.toShort(ok, base);}
inline ushort String::toUShort(bool* ok, int base) const 
  {QString tmp(chars()); return tmp.toUShort(ok, base);}
inline int String::toInt(bool* ok, int base) const 
  {QString tmp(chars()); return tmp.toInt(ok, base);}
inline uint String::toUInt(bool* ok, int base) const 
  {QString tmp(chars()); return tmp.toUInt(ok, base);}
inline int64_t String::toInt64(bool* ok, int base) const 
  {QString tmp(chars()); return tmp.toLongLong(ok, base);}
inline uint64_t String::toUInt64(bool* ok, int base) const 
  {QString tmp(chars()); return tmp.toULongLong(ok, base);}
inline float String::toFloat(bool* ok) const 
  {QString tmp(chars()); return tmp.toFloat(ok);}
inline double String::toDouble(bool* ok) const 
  {QString tmp(chars()); return tmp.toDouble(ok);}
#else 
//NOTE: these are primarily just for maketa, and are not fully functional
inline short String::toShort(bool* ok, int base) const 
  {if (ok) *ok = true; return (short)strtol(chars(), NULL, base);}
inline ushort String::toUShort(bool* ok, int base) const 
  {if (ok) *ok = true; return (ushort)strtoul(chars(), NULL, base);}
inline int String::toInt(bool* ok, int base) const 
  {if (ok) *ok = true; return strtol(chars(), NULL, base);}
inline uint String::toUInt(bool* ok, int base) const 
  {if (ok) *ok = true; return strtoul(chars(), NULL, base);}
inline int64_t String::toInt64(bool* ok, int base) const 
  {if (ok) *ok = true; return _strtoi64(chars(), NULL, base);}
inline uint64_t String::toUInt64(bool* ok, int base) const 
  {if (ok) *ok = true; return _strtoui64(chars(), NULL, base);}
inline float String::toFloat(bool* ok) const 
  {if (ok) *ok = true; return (float)strtod(chars(), NULL);}
inline double String::toDouble(bool* ok) const 
  {if (ok) *ok = true; return strtod(chars(), NULL);}
#endif

// same issue with this operator: must be here in .h file, or there are link errors
inline std::ostream& operator<<(std::ostream& s, const String& x)
{
   s << x.chars(); return s;
}


inline void String::newRep(StrRep* rep_) {
  rep_->ref();
  mrep = rep_;
}

inline void String::setRep(StrRep* rep_) {
  rep_->ref(); //note: implicitly handles rare case of mrep=rep_
  mrep->unRef();
  mrep = rep_;
}

// assignment

// Zillions of cats...

inline void cat(const String& x, const String& y, String& r)
{
  r.setRep(Scat(x.chars(), x.length(), y.chars(), y.length()));
}

inline void cat(const String& x, const char* y, String& r)
{
  r.setRep(Scat(x.chars(), x.length(), y, -1));
}

inline void cat(const String& x, const char* y, uint slen, String& r)
{
  r.setRep(Scat(x.chars(), x.length(), y, slen));
}

inline void cat(const String& x, char y, String& r)
{
  if (y == '\0') r = x;
  else r.setRep(Scat(x.chars(), x.length(), &y, 1));
}

inline void cat(const char* x, const String& y, String& r)
{
  r.setRep(Scat(x, -1, y.chars(), y.length()));
}

inline void cat(const char* x, const char* y, String& r)
{
  r.setRep(Scat(x, -1, y, -1));
}

inline void cat(const char* x, char y, String& r)
{
  if (y == '\0') r = x;
  else r.setRep(Scat(x, -1, &y, 1));
}

// operator versions

// constructive concatenation

inline String operator + (const String& x, const String& y)
{
  String r;  cat(x, y, r);  return r;
}

inline String operator + (const String& x, const char* y)
{
  String r; cat(x, y, r); return r;
}

inline String operator + (const String& x, char y)
{
  String r; cat(x, y, r); return r;
}

inline String operator + (const char* x, const String& y)
{
  String r; cat(x, y, r); return r;
}

inline String reverse(const String& x)
{
  String r; r.setRep(Sreverse(x.mrep)); return r;
}

inline String upcase(const String& x)
{
  String r(x); r.upcase(); return r;
}

inline String downcase(const String& x)
{
  String r(x); r.downcase(); return r;
}

inline String capitalize(const String& x)
{
  String r(x); r.capitalize(); return r;
}


// element extraction

inline char  String::operator [] (int i) const {
#ifdef DEBUG
  if (((unsigned)i) >= length()) error("invalid index");
#endif
  return mrep->s[i];
}

inline char&  String::operator [] (int i) {
#ifdef DEBUG
  if (((unsigned)i) >= length()) error("invalid index");
#endif
  makeUnique();
  return mrep->s[i];
}

inline char  String::elem (int i) const {
#ifdef DEBUG
  if (((unsigned)i) >= length()) error("invalid index");
#endif
  return mrep->s[i];
}

// searching

inline int String::index(char c, int startpos) const
{
  return search(startpos, length(), c);
}

inline int String::index(const char* t, int startpos) const
{
  return search(startpos, length(), t);
}

inline int String::index(const String& y, int startpos) const
{
  return search(startpos, length(), y.chars(), y.length());
}

inline bool String::contains(char c) const
{
  return search(0, length(), c) >= 0;
}

inline bool String::contains(const char* t) const
{
  return search(0, length(), t) >= 0;
}

inline bool String::contains(const String& y) const
{
  return search(0, length(), y.chars(), y.length()) >= 0;
}

inline bool String::contains(char c, int p) const
{
  return match(p, length(), 0, &c, 1) >= 0;
}

inline bool String::contains(const char* t, int p) const
{
  return match(p, length(), 0, t) >= 0;
}

inline bool String::contains(const String& y, int p) const
{
  return match(p, length(), 0, y.chars(), y.length()) >= 0;
}

inline bool String::matches(const String& y, int p) const
{
  return match(p, length(), 1, y.chars(), y.length()) >= 0;
}

inline bool String::matches(const char* t, int p) const
{
  return match(p, length(), 1, t) >= 0;
}

inline bool String::matches(char c, int p) const
{
  return match(p, length(), 1, &c, 1) >= 0;
}

inline int String::gsub(const String& pat, const String& r)
{
  return _gsub(pat.chars(), pat.length(), r.chars(), r.length());
}

inline int String::gsub(const char* pat, const String& r)
{
  return _gsub(pat, -1, r.chars(), r.length());
}

inline int String::gsub(const char* pat, const char* r)
{
  return _gsub(pat, -1, r, -1);
}



// a zillion comparison operators

#ifdef TA_USE_QT
inline bool operator==(const QString& x, const String& y) {
  return compare(String(x), y) == 0;
}
inline bool operator==(const String& x, const QString& y) {
  return compare(x, String(y)) == 0;
}
#endif

inline bool operator==(const String& x, const String& y)
{
  return compare(x, y) == 0;
}

inline bool operator!=(const String& x, const String& y)
{
  return compare(x, y) != 0;
}

inline bool operator>(const String& x, const String& y)
{
  return compare(x, y) > 0;
}

inline bool operator>=(const String& x, const String& y)
{
  return compare(x, y) >= 0;
}

inline bool operator<(const String& x, const String& y)
{
  return compare(x, y) < 0;
}

inline bool operator<=(const String& x, const String& y)
{
  return compare(x, y) <= 0;
}



// const String&, char*
inline bool operator==(const String& x, char* t)
{
  return compare(x, t) == 0;
}

inline bool operator!=(const String& x, char* t)
{
  return compare(x, t) != 0;
}

inline bool operator>(const String& x, char* t)
{
  return compare(x, t) > 0;
}

inline bool operator>=(const String& x, char* t)
{
  return compare(x, t) >= 0;
}

inline bool operator<(const String& x, char* t)
{
  return compare(x, t) < 0;
}

inline bool operator<=(const String& x, char* t)
{
  return compare(x, t) <= 0;
}

// const char*, String&
inline bool operator==(char* x, const String& t)
{
  return compare(x, t) == 0;
}

inline bool operator!=(char* x, const String& t)
{
  return compare(x, t) != 0;
}

inline bool operator>(char* x, const String& t)
{
  return compare(x, t) > 0;
}

inline bool operator>=(char* x, const String& t)
{
  return compare(x, t) >= 0;
}

inline bool operator<(char* x, const String& t)
{
  return compare(x, t) < 0;
}

inline bool operator<=(char* x, const String& t)
{
  return compare(x, t) <= 0;
}


// const String&, const char*
inline bool operator==(const String& x, const char* t)
{
  return compare(x, t) == 0;
}

inline bool operator!=(const String& x, const char* t)
{
  return compare(x, t) != 0;
}

inline bool operator>(const String& x, const char* t)
{
  return compare(x, t) > 0;
}

inline bool operator>=(const String& x, const char* t)
{
  return compare(x, t) >= 0;
}

inline bool operator<(const String& x, const char* t)
{
  return compare(x, t) < 0;
}

inline bool operator<=(const String& x, const char* t)
{
  return compare(x, t) <= 0;
}

// const const char*, String&
inline bool operator==(const char* x, const String& t)
{
  return compare(x, t) == 0;
}

inline bool operator!=(const char* x, const String& t)
{
  return compare(x, t) != 0;
}

inline bool operator>(const char* x, const String& t)
{
  return compare(x, t) > 0;
}

inline bool operator>=(const char* x, const String& t)
{
  return compare(x, t) >= 0;
}

inline bool operator<(const char* x, const String& t)
{
  return compare(x, t) < 0;
}

inline bool operator<=(const char* x, const String& t)
{
  return compare(x, t) <= 0;
}

// const String&, char
inline bool operator==(const String& x, char t)
{
  return compare(x, t) == 0;
}

inline bool operator!=(const String& x, char t)
{
  return compare(x, t) != 0;
}

inline bool operator>(const String& x, char t)
{
  return compare(x, t) > 0;
}

inline bool operator>=(const String& x, char t)
{
  return compare(x, t) >= 0;
}

inline bool operator<(const String& x, char t)
{
  return compare(x, t) < 0;
}

inline bool operator<=(const String& x, char t)
{
  return compare(x, t) <= 0;
}


// const char,  String&
inline bool operator==(char x, const String& t)
{
  return compare(x, t) == 0;
}

inline bool operator!=(char x, const String& t)
{
  return compare(x, t) != 0;
}

inline bool operator>(char x, const String& t)
{
  return compare(x, t) > 0;
}

inline bool operator>=(char x, const String& t)
{
  return compare(x, t) >= 0;
}

inline bool operator<(char x, const String& t)
{
  return compare(x, t) < 0;
}

inline bool operator<=(char x, const String& t)
{
  return compare(x, t) <= 0;
}


#endif
