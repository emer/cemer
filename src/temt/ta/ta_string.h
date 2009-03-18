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
// # include <iostream.h>
#else
  #include <iostream>
  #ifdef TA_USE_QT
    #include <QString>
    #include <QVariant>
  #endif
#endif // __MAKETA__

#include "ta_atomic.h"

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
#ifdef __MAKETA__
  QBasicAtomicInt	cnt;	// reference count (when goes to 0, instance is deleted)
#else
union { // this lets us use static init of cnt to 1 for _nilStrRep
  QBasicAtomicInt	cnt;
  int			cnt_int; // though shalt never use this!!!
};
#endif
  char	     	s[1];   // the string starts here, null terminator always maintained
protected:
  bool			canCat(uint xtra_len) {return ((cnt == 1) && ((sz - len) >= xtra_len));}  // true if ref==1, and enough space to add
  void			ref() {cnt.ref();}
  static void 		unRef(StrRep* inst) {if (!inst->cnt.deref()) free(inst);}
  void			cat(const char* str, uint slen); // note: slen must be set, and canCat must have been true
  void			upcase();	// convert all letters to upper case; only called if cnt<=1
  void			downcase();	// convert all letters to lower case; only called if cnt<=1
  void			capitalize(); // capitalize the first letter of each word; only called if cnt<=1
  void			reverse();	// reverse order of string; only called if cnt<=1
  void			prepend(const char* str, uint slen); // note: slen must be set, and canCat must have been true
  int	      		search(int, int, const char*, int = -1) const;
  int	      		search_ci(int, int, const char*, int = -1) const;
  int	      		search(int, int, char) const;
  int	      		match(int, int, int, const char*, int = -1) const;
};

extern TA_API StrRep  _nilStrRep; // an empty StrRep, for convenience and efficiency
#define ADDR_NIL_STR_REP &_nilStrRep

// primitive ops on StrReps -- nearly all String fns go through these.

/*
  s - pointer to the string; can be NULL
  slen -- len of the string if known; -1 means call strlen function
  cap -- capacity; 0 means use the string len
*/
TA_API StrRep*		Snew(int slen, uint cap = 0); // for an empty rep, for filling by caller; len is set
TA_API StrRep*		Salloc(const char* s, int slen = -1, uint cap = 0); // the most-used alloc
TA_API StrRep*		Scat(StrRep* srep, const char* s, uint slen = -1);
TA_API StrRep*		Scat(const char* s1, int slen1, const char* s2, int slen2); // slen can be -1
TA_API StrRep*		Sreverse(const StrRep* x);

class TA_API String {
  // string of characters with many useful methods for string manipulation
friend class StrRep;
public:
  ////////////////////////////////////////////////
  // statics

  static const String con_0; // #IGNORE "0" occurs so often, we optimize it with a const
  static const String con_1; // #IGNORE "1" occurs so often, we optimize it with a const
  static const String con_NULL; // #IGNORE "NULL" occurs so often, we optimize it with a const
  
  static const String	CharToCppLiteral(char c);
  // #IGNORE converts a character to a C++ valid literal; can be embedded in a C++ string
  static const String	StringToCppLiteral(const String& str);
  // #IGNORE converts a string to a C++ valid literal
  
  ////////////////////////////////////////////////
  // global functions:

  friend int        split(const String& x, String res[], int maxn, const String& sep);
  // #IGNORE split string into array res at separators; return number of elements
  friend String     common_prefix(const String& x, const String& y, int startpos = 0);
  // #IGNORE 
  friend String     common_suffix(const String& x, const String& y,int startpos = -1);
  // #IGNORE 
  friend String     replicate(char c, int n);
  // #IGNORE 
  friend String     replicate(const char* y, int n);
  // #IGNORE 
  friend String     replicate(const String& y, int n);
  // #IGNORE 
  friend String     join(const String src[], int n, const String& sep); //
  // #IGNORE 

  // simple builtin transformations
  friend String		triml(const String& x);
  // #IGNORE trims leading spaces
  friend String		trimr(const String& x);
  // #IGNORE trims trailing spaces
  friend String		trim(const String& x);
  // #IGNORE trims leading and trailing  spaces
  friend inline String	reverse(const String& x);
  // #IGNORE 
  friend inline String	upcase(const String& x);
  // #IGNORE 
  friend inline String	downcase(const String& x);
  // #IGNORE 
  friend inline String	capitalize(const String& x);
  // #IGNORE 
    
  friend inline void     cat(const String&, const String&, String&);
  // #IGNORE 
  friend inline void     cat(const String&, const char*, String&);
  // #IGNORE 
  friend inline void     cat(const String&, const char* str, uint slen, String&);
  // #IGNORE slen must be set
  friend inline void     cat(const String&, char, String&);
  // #IGNORE 
  friend inline void     cat(const char*, const String&, String&);
  // #IGNORE 
  friend inline void     cat(const char*, const char*, String&);
  // #IGNORE 
  friend inline void     cat(const char*, char, String&);
  // #IGNORE concatenate first 2 args, store result in last arg

  // IO
#ifdef __MAKETA__
  friend ostream&   operator<<(ostream& s, const String& x);
  friend istream&   operator>>(istream& s, String& x);

  friend int        readline(istream& s, String& x,
			     char terminator = '\n',
			     int discard_terminator = 1);
  int			Save_str(ostream& ostrm);
  int			Load_str(istream& istrm);
  // #IGNORE 
#else
  friend std::ostream&   operator<<(std::ostream& s, const String& x); // is inline
  TA_API friend std::istream&   operator>>(std::istream& s, String& x);
  TA_API friend int        readline(std::istream& s, String& x,
				 char terminator = '\n',
				 int discard_terminator = 1);
  int			Load_str(istream& istrm); // load contents from a stream
  int			Save_str(std::ostream& ostrm); // save contents to a stream
#endif
  
  ////////////////////////////////////////////////
  // constructors & assignment

  inline String() {newRep(ADDR_NIL_STR_REP);} // el blanco
  inline String(const String& x) {newRep(x.mrep);} // copy constructor -- only a ref on source! (fast!)
  String(const String* x) {if (x) newRep(x->mrep); else newRep(ADDR_NIL_STR_REP);} 
  inline String(const char* s) {init(s, -1);} // s can be NULL
  String(const char* s, int slen) {init(s, slen);}
  String(StrRep* x) {newRep(x);} // typically only used internally
  String(uint slen, uint sz, char fill); // for allocating a writeable buffer; (1) if sz==0, then sz=slen; (2) if fill==0, then slen will be forced to 0


  ////////////////////////////////////////////////
  // conversion constructors

  explicit String(bool b); //note: implicit causes evil problems, esp. by converting pointers to strings
  String(char c) {if (c == '\0') newRep(ADDR_NIL_STR_REP); else init(&c, 1);}
  String(int i, const char* format = "%d");
  String(uint u, const char* format = "%u");
  String(long i, const char* format = "%ld"); //note: don't use long any more, compatibility only
  String(ulong u, const char* format = "%lu"); //note: don't use long any more, compatibility only
  String(ta_int64_t i64); //NOTE: no fmts because they are not cross-platform standard
  String(ta_uint64_t u64); //NOTE: no fmts because they are not cross-platform standard
  String(float f, const char* format = "%g");
  String(double d, const char* format = "%lg");
  explicit String(void* p); //converts to hex
#ifdef TA_USE_QT
  String(const QString& val);
  String&	  operator = (const QString& y);
  const QString		toQString() const; // #IGNORE evil C++ necessitates this!!!
  operator QString() const;  // #IGNORE
  operator QVariant() const;  // #IGNORE
#endif

#ifdef DEBUG
  ~String() {StrRep::unRef(mrep); mrep = NULL;}
#else
  ~String() {StrRep::unRef(mrep);}
#endif
  ////////////////////////////////////////////////
  // basic resource allocation and infrastructure

  void			setRep(StrRep* rep_);
  // #IGNORE for replacing rep (non-constructor) -- rep_ must be non-null

  inline int		length() const {return mrep->len;} 
  // #CAT_Access how many characters in the string
  inline bool		empty() const {return (mrep->len == 0);}
  // #CAT_Access true if the string is empty
  inline bool		nonempty() const {return (mrep->len > 0);}
  // #CAT_Access true if the string is nonempty

  int			allocation() const {return mrep->sz;}
  // #IGNORE 
  int			assertLength();
  // #IGNORE call after manual buffer operations, to set length from null term -- **WARNING** only use on ref==1, and where there is a null term in the buff, and sz is not overflowed
  inline const char*	chars() const {return mrep->s;}
  // #IGNORE returns null-terminated string DO NOT MODIFY
  char*			chars_ptr() {makeUnique(); return mrep->s;}
  // #IGNORE returns point to the string, for in-place mods -- **this makeUnique() first, so you have a unique instance to overwrite**

  void			makeUnique(); 
  // #IGNORE make sure we are only owner of mrep
  void			makeUnique(uint min_new_sz);
  // #IGNORE makeUnique(), but force sz to be at least new_sz
  void			pack();
  // #IGNORE if there is any extra space (above a small threshold), release it

  ////////////////////////////////////////////////
  // conversion operators

  operator unsigned char() const { return (unsigned char)strtol(chars(), NULL, 0); } 
  // note: this is for 'byte' conversion -- we hope that char per-se is 'signed char'
  operator int() const	{ return strtol(chars(), NULL, 0); }
  operator uint() const	{ return toUInt(); }
  operator long() const	{ return strtol(chars(), NULL, 0); }
  operator long long() const {return toInt64();}
  operator unsigned long long() const {return toUInt64();}
  operator float() const { return (float)atof(chars()); }
  operator double() const { return atof(chars()); }
  operator char*() const { return (char*)chars(); }
  operator const char*() const {return mrep->s;}

  bool			isInt() const;
  // #CAT_Convert true if the string contains a value that can be interpreted as an integer [+-]dd*

  bool 			toBool() const;
  // #CAT_Convert accepts true as starting with t/T, or else 1
  char 			toChar() const;
  // #CAT_Convert if size 1, then that's it, else try heuristics
  
  short 		toShort(bool* ok = 0, int base = 10) const; 
  // #IGNORE
  ushort 		toUShort(bool* ok = 0, int base = 10) const; 
  // #IGNORE
  int 			toInt(bool* ok = 0, int base = 10) const; 
  // #CAT_Convert to integer value -- base determines base representation (hex=16, oct=8)
  uint 			toUInt(bool* ok = 0, int base = 10) const; 
  // #IGNORE
  ta_int64_t 		toInt64(bool* ok = 0, int base = 10) const; 
  // #IGNORE
  ta_uint64_t 		toUInt64(bool* ok = 0, int base = 10) const; 
  // #IGNORE
  float 		toFloat(bool* ok = 0) const; 
  // #CAT_Convert to floating point (single precision) value
  double 		toDouble(bool* ok = 0) const; 
  // #CAT_Convert to floating point (double precision) value

  String&		convert(int i, const char* format = "%d");
  // #IGNORE
  String&		convert(long i, const char* format = "%ld");
  // #IGNORE
  String&		convert(float f, const char* format = "%g");
  // #IGNORE
  String&		convert(double f, const char* format = "%lg");
  // #IGNORE

  int 			HexToInt() const { return strtol(chars(), NULL, 16); }
  // #CAT_Convert hexidecimal string to integer value
  
  String&		operator=(const String& y) {setRep(y.mrep); return *this;}
  String&		operator=(const char* s) {return set(s, -1);}
  String&		operator=(char c) {if (c == '\0') 
    {setRep(ADDR_NIL_STR_REP); return *this;} else return set(&c, 1);} //

  String&	  set(const char* t, int len); 
  // #IGNORE parameterized set -- used in assigns

  ////////////////////////////////////////////////
  // concatenation -- note: we try to do it in place, if possible (if refs==1, and enough space)

  String&		cat(char c);
  String&		cat(const char* t);
  String&		cat(const String& y);
  // #CAT_Modify concatenate (add) string to end of this one

  String&		prepend(char c);
  String&		prepend(const char* t);
  String&		prepend(const String& y);
  // #CAT_Modify insert string at beginning of this one

  String&		operator += (const String& y) {return cat(y);}
  String&		operator += (const char* t) {return cat(t);}
  String&		operator += (char c) {return cat(c);}

  ////////////////////////////////////////////////
  // searching & matching

  int	      		index(char c, int startpos = 0) const;
  int			index(const char* t, int startpos = 0) const;
  int			index(const String& y, int startpos = 0) const;
  // #CAT_Find return position of target in string or -1 for failure. startpos = starting position (- = search from end forward)
  int			index_ci(const String& y, int startpos = 0) const;
  // #CAT_Find ci = case independent: return position of target in string or -1 for failure. startpos = starting position (- = search from end forward)

  bool			contains(char c, int startpos=0) const;
  bool			contains(const char* t, int startpos=0) const;
  bool			contains(const String& y, int startpos=0) const;
  // #CAT_Find return 'true' if target appears anywhere in String. startpos = starting position (- = search from end forward)
  bool			contains_ci(const String& y, int startpos=0) const;
  // #CAT_Find ci = case independent: return 'true' if target appears anywhere in String. startpos = starting position (- = search from end forward)

  bool			matches(char c, int pos = 0) const;
  bool			matches(const char* t, int pos = 0) const;
  bool			matches(const String& y, int pos = 0) const;
  // #CAT_Find return 'true' if target appears at position pos in String

  bool			endsWith(char c) const;
  bool			endsWith(const char* t) const;
  bool			endsWith(const String& y) const;
  // #CAT_Find return 'true' if target is at end of String

  bool			startsWith(char c) const;
  bool			startsWith(const char* t) const;
  bool			startsWith(const String& y) const;
  // #CAT_Find return 'true' if target is at start of String

  int			freq(char        c) const;
  int			freq(const char* t) const;
  int			freq(const String& y) const;
  // #CAT_Find return number of occurences of target in String

  ////////////////////////////////////////////////
  // String extraction (access)

  String		operator () (int pos, int len) const; // synonym for at
  char			operator [] (int i) const;
  char&			operator [] (int i); // writable -- NOTE: every use calls makeUnique

  char			elem(int i) const;
  // #CAT_Access get the character at index i
  char			firstchar() const {return mrep->s[0];}
  // #CAT_Access get the first character, '\0 if empty
  char			lastchar() const
  {if (mrep->len) return mrep->s[mrep->len - 1]; else return '\0';}
  // #CAT_Access get the last character; '\0 if empty

  String		at(const String&  x, int startpos = 0) const;
  String		at(const char* t, int startpos = 0) const;
  String		at(char c, int startpos = 0) const;
  String		at(int pos, int len) const;
  // #CAT_Access get substring at position for length

  String		before(int pos) const;
  String		before(char c, int startpos = 0) const;
  String		before(const char* t, int startpos = 0) const; 
  String		before(const String& x, int startpos = 0) const;
  // #CAT_Access get substring before (not including) target string. startpos = starting position (- = search from end forward)

  String		through(int pos) const;
  String		through(char c, int startpos = 0) const;
  String		through(const char* t, int startpos = 0) const;
  String		through(const String& x, int startpos = 0) const;
  // #CAT_Access get substring through (including) target string. startpos = starting position (- = search from end forward)

  String		from(int pos) const;
  String		from(char c, int startpos = 0) const;
  String		from(const char* t, int startpos = 0) const;
  String		from(const String& x, int startpos = 0) const;
  // #CAT_Access get substring from (including) target string. startpos = starting position (- = search from end forward)

  String		after(int pos) const;
  String		after(char c, int startpos = 0) const;
  String		after(const char* t, int startpos = 0) const;
  String		after(const String& x, int startpos = 0) const;
  // #CAT_Access get substring after (not including) target string. startpos = starting position (- = search from end forward)

  String		between(char st_c, char ed_c, int startpos = 0) const;
  String		between(const char* st_str, const char* ed_str, int startpos = 0) const;
  String		between(const String& st_str, const String& ed_str, int startpos = 0) const;
  // #CAT_Access get substring between (not including) target strings. startpos = starting position (- = search from end forward)

  inline String		left(int len) const {return before(len);}
  // #CAT_Access get leftmost len chars
  String		right(int len) const;
  // #CAT_Access get rightmost len chars
  
  ////////////////////////////////////////////////
  // String modification

  String		elidedTo(int len = -1) const;
  // #CAT_Modify return a string no more than len long, no line breaks, eliding chars if needed and adding ... marks; -1 is no eliding
  String		elidedToFirstLine() const;
  // #CAT_Modify if has newlines, elide to first line
  
  void			del(const String& y, int startpos = 0);
  void			del(const char* t, int startpos = 0);
  void			del(char c, int startpos = 0);
  void			del(int pos, int len);
  // #CAT_Modify delete len chars starting at pos
  void			remove(const String& y, int startpos = 0) { del(y, startpos); }
  // #CAT_Modify remove target string from string. startpos = starting position (- = search from end forward)

  int			gsub(const char* pat, const String&     repl);
  int			gsub(const char* pat, const char* repl);
  int			gsub(const String& pat, const String& repl);
  // #CAT_Modify global substitution: substitute all occurrences of pat with repl

  String&		repl(const char* pat, const String&     repl);
  String&		repl(const char* pat, const char* repl);
  String&		repl(const String& pat, const String& repl);
  // #CAT_Modify global substitution: substitute all occurrences of pat with repl and return the modified string

  // in-place versions of friends -- they automatically makeUnique

  String&		reverse();
  // #CAT_Modify reverse order of this string (NOTE: modifies this string, and also returns the resulting string value)
  String&		upcase();
  // #CAT_Modify convert all letters to upper case of this string (NOTE: modifies this string, and also returns the resulting string value)
  String&		downcase();
  // #CAT_Modify convert all letters to lower case of this string (NOTE: modifies this string, and also returns the resulting string value)
  String&		capitalize();
  // #CAT_Modify capitalize the first letter of each word of this string (NOTE: modifies this string, and also returns the resulting string value)
  String&		xml_esc();
  // #CAT_Modify xml escape, ex & to &amp; of this string (NOTE: modifies this string, and also returns the resulting string value)

  void			truncate(uint new_len); 
  // #CAT_Modify shortens the string to new_len (if less than curr)
  
  void			error(const char* msg) const;
  // #IGNORE report an error

protected:
  static void 		AppendCharToCppStringLiteral(String& str, char c, bool char_mode);
  // #IGNORE
  
  StrRep*		mrep;   // Strings are pointers to their representations
  void			init(const char* s, int slen = -1); // for calling in constructors
  void			newRep(StrRep* rep_); // for setting rep in a constructor
  String&		cat(const char* s, int slen); // for internal use

  // some helper functions
  int		 	search(int start, int sl, const char* t, int tl = -1) const
  	{return mrep->search(start, sl, t, tl);}
  int		 	search(int start, int sl, char c) const {return mrep->search(start, sl, c);}
  int		 	search_ci(int start, int sl, const char* t, int tl = -1) const
  	{return mrep->search_ci(start, sl, t, tl);}
  int		 	match(int start, int sl, int exact, const char* t, int tl = -1) const
  	{return mrep->match(start, sl, exact, t, tl);}
  int		 	_gsub(const char*, int, const char* ,int);
  String         	_substr(int pos, int slen) const; // return a substring
private:
  int		 	_gsub_lt(const char*, int, const char* ,int);
  int		 	_gsub_eq(const char*, int, const char*);
  int		 	_gsub_gt(const char*, int, const char* ,int);
}; //

//extern String _nilString; // an empty string, for convenience
//note: can't use global static, because ctor not guarenteed to run before use
#define _nilString taString()

#define STRING_BUF(name, size)  String name(0, size, '\0')

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

inline String::operator QVariant() const {
  QVariant result(chars());
  return result;
}

inline String& String::operator = (const QString& y) {
  return set(y.toLatin1(), y.length());
}

inline const QString String::toQString() const {
  return QString(chars());
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
inline ta_int64_t String::toInt64(bool* ok, int base) const 
  {QString tmp(chars()); return tmp.toLongLong(ok, base);}
inline ta_uint64_t String::toUInt64(bool* ok, int base) const 
  {QString tmp(chars()); return tmp.toULongLong(ok, base);}
inline float String::toFloat(bool* ok) const 
{ QString tmp(chars()); bool okk; float rval = tmp.toFloat(&okk);
  if(!okk && freq(',') == 1) { tmp.replace(',', '.'); rval = tmp.toFloat(&okk); } 
  if(ok) *ok = okk; return rval; }
inline double String::toDouble(bool* ok) const 
{ QString tmp(chars()); bool okk; double rval = tmp.toDouble(&okk);
  if(!okk && freq(',') == 1) { tmp.replace(',', '.'); rval = tmp.toDouble(&okk); } 
  if(ok) *ok = okk; return rval; }
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
inline ta_int64_t String::toInt64(bool* ok, int base) const 
  {if (ok) *ok = true; return strtoll(chars(), NULL, base);}
inline ta_uint64_t String::toUInt64(bool* ok, int base) const 
  {if (ok) *ok = true; return strtoull(chars(), NULL, base);}
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
  StrRep::unRef(mrep);
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
// only MSVC could be so anal as to not coerce char* to const char* ...

inline String operator + (const String& x, const String& y)
{
  String r;  cat(x, y, r);  return r;
}

inline String operator + (const String& x, const char* y)
{
  String r; cat(x, y, r); return r;
}

inline String operator + (const String& x, char* y)
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

inline String operator + (char* x, const String& y)
{
  String r; cat(x, y, r); return r;
}
#ifdef TA_USE_QT
inline String operator + (const QString& x, const String& y)
{
  String r;  
  r.setRep(Scat(x.toLatin1(), x.length(), y.chars(), y.length()));
  return r;
}

inline String operator + (const String& x, const QString& y)
{
  String r;  
  r.setRep(Scat(x.chars(), x.length(), y.toLatin1(), y.length()));
  return r;
}
#endif

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
  if (i >= length()) error("invalid index");
#endif
  return mrep->s[i];
}

inline char&  String::operator [] (int i) {
#ifdef DEBUG
  if (i >= length()) error("invalid index");
#endif
  makeUnique();
  return mrep->s[i];
}

inline char  String::elem (int i) const {
#ifdef DEBUG
  if (i >= length()) error("invalid index");
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

inline int String::index_ci(const String& y, int startpos) const
{
  return search_ci(startpos, length(), y.chars(), y.length());
}

inline bool String::contains(char c, int p) const
{
  return search(p, length(), c) >= 0;
}

inline bool String::contains(const char* t, int p) const
{
  return search(p, length(), t) >= 0;
}

inline bool String::contains(const String& y, int p) const
{
  return search(p, length(), y.chars(), y.length()) >= 0;
}

inline bool String::contains_ci(const String& y, int p) const
{
  return search_ci(p, length(), y.chars(), y.length()) >= 0;
}

inline bool String::matches(const String& y, int p) const
{
  return match(p, length(), 0, y.chars(), y.length()) >= 0;
}

inline bool String::matches(const char* t, int p) const
{
  return match(p, length(), 0, t) >= 0;
}

inline bool String::matches(char c, int p) const
{
  return match(p, length(), 0, &c, 1) >= 0;
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

inline String& String::repl(const String& pat, const String& r)
{
  _gsub(pat.chars(), pat.length(), r.chars(), r.length());
  return *this;
}

inline String& String::repl(const char* pat, const String& r)
{
  _gsub(pat, -1, r.chars(), r.length());
  return *this;
}

inline String& String::repl(const char* pat, const char* r)
{
  _gsub(pat, -1, r, -1);
  return *this;
}



// a zillion comparison operators

#ifdef TA_USE_QT
inline bool operator==(const QString& x, const String& y) {
  return compare(String(x), y) == 0;
}
inline bool operator==(const String& x, const QString& y) {
  return compare(x, String(y)) == 0;
}
inline bool operator!=(const QString& x, const String& y) {
  return compare(String(x), y) != 0;
}
inline bool operator!=(const String& x, const QString& y) {
  return compare(x, String(y)) != 0;
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

