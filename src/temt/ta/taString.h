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

// all unadorned references to string are hereby taString references
// which should be used in the case that there are multiple conflicting string
// types in use.

#ifdef String
#undef String
#endif
#define String taString

#ifndef taString_h
#define taString_h 1

// parent includes:

// member includes:

// declare all other types mentioned but not required to include:

// ta_string is the string class used in the TA/CSS software
// it is **very loosely** based on the gnu libg++ string class, with the following
// modifications:
// - strings can be of any length (memory permitting, of course)
// - strings use reference counting to instance data -- simple string copies are just pointer ops
// - constructors and conversion operators to/from ints, floats, and doubles
// - replacement of the regexp functionality with qt regexp
// - reordering and commenting of overloaded functions so that the
//      const char* version is always the last one, so that it will
//      be the one scanned by TA
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

#include <taAtomicInt>

class TA_API Variant;
class TA_API taStrRep;
class TA_API String; //

/* taStrRep - internal String representation

  Note: this structure is allocated via malloc(), so there must be no constructor/destructor.
*/

class TA_API taStrRep {
friend class String;
public:
  uint                  len;    // string length (not including null terminator)
  uint                  sz;     // allocated space ((not including null terminator)
#ifdef __MAKETA__
  taBasicAtomicInt      cnt;    // reference count (when goes to 0, instance is deleted)
#else
 union { // this lets us use static init of cnt to 1 for _nilStrRep
   int                   cnt_int; // though shalt never use this!!!
   taBasicAtomicInt      cnt;
 };
#endif
  char          s[1];   // the string starts here, null terminator always maintained
protected:
  bool                  canCat(uint xtra_len) {return ((cnt == 1) && ((sz - len) >= xtra_len));}  // true if ref==1, and enough space to add
  void                  ref() {cnt.ref();}
  static void           unRef(taStrRep* inst) {if (!inst->cnt.deref()) free(inst);}
  void                  cat(const char* str, uint slen); // note: slen must be set, and canCat must have been true
  void                  upcase();       // convert all letters to upper case; only called if cnt<=1
  void                  downcase();     // convert all letters to lower case; only called if cnt<=1
  void                  capitalize(); // capitalize the first letter of each word; only called if cnt<=1
  void                  reverse();      // reverse order of string; only called if cnt<=1
  void                  prepend(const char* str, uint slen); // note: slen must be set, and canCat must have been true
  int                   search(int, int, const char*, int = -1) const;
  int                   search_ci(int, int, const char*, int = -1) const;
  int                   search(int, int, char) const;
  int                   match(int, int, int, const char*, int = -1) const;
};

extern TA_API taStrRep  _nilStrRep; // an empty taStrRep, for convenience and efficiency
extern TA_API taStrRep* _nilStrRepPtr;
// this points to _nilStrRep, but can't let compiler know, because it issues warning..
#define ADDR_NIL_STR_REP _nilStrRepPtr

// primitive ops on StrReps -- nearly all String fns go through these.

/*
  s - pointer to the string; can be NULL
  slen -- len of the string if known; -1 means call strlen function
  cap -- capacity; 0 means use the string len
*/
TA_API taStrRep*                Snew(int slen, uint cap = 0); // for an empty rep, for filling by caller; len is set
TA_API taStrRep*                Salloc(const char* s, int slen = -1, uint cap = 0); // the most-used alloc
TA_API taStrRep*                Scat(taStrRep* srep, const char* s, int slen = -1);
TA_API taStrRep*                Scat(const char* s1, int slen1, const char* s2, int slen2); // slen can be -1
TA_API taStrRep*                Sreverse(const taStrRep* x);

TypeDef_Of(taString);

class TA_API taString {
  // string of characters with many useful methods for string manipulation
friend class taStrRep;
public:
  ////////////////////////////////////////////////
  // statics

  static const taString con_0; // #IGNORE "0" occurs so often, we optimize it with a const
  static const taString con_1; // #IGNORE "1" occurs so often, we optimize it with a const
  static const taString con_NULL; // #IGNORE "NULL" occurs so often, we optimize it with a const

  static const taString   CharToCppLiteral(char c);
  // #IGNORE converts a character to a C++ valid literal; can be embedded in a C++ string
  static const taString   StringToCppLiteral(const taString& str);
  // #IGNORE converts a string to a C++ valid literal

  ////////////////////////////////////////////////
  // global functions:

  TA_API friend int     split(const taString& x, taString res[], int maxn, const taString& sep);
  // #IGNORE split string into array res at separators; return number of elements
  TA_API friend taString  common_prefix(const taString& x, const taString& y, int startpos = 0);
  // #IGNORE
  TA_API friend taString  common_suffix(const taString& x, const taString& y,int startpos = -1);
  // #IGNORE
  TA_API friend taString  replicate(char c, int n);
  // #IGNORE
  TA_API friend taString  replicate(const char* y, int n);
  // #IGNORE
  TA_API friend taString  replicate(const taString& y, int n);
  // #IGNORE
  TA_API friend taString  join(const taString src[], int n, const taString& sep); //
  // #IGNORE

  // simple builtin transformations
  TA_API friend taString          triml(const taString& x);
  // #IGNORE trims leading spaces
  TA_API friend taString          trimr(const taString& x);
  // #IGNORE trims trailing spaces
  TA_API friend taString          trim(const taString& x);
  // #IGNORE trims leading and trailing  spaces
  TA_API friend inline taString   reverse(const taString& x);
  // #IGNORE
  TA_API friend inline taString   upcase(const taString& x);
  // #IGNORE
  TA_API friend inline taString   downcase(const taString& x);
  // #IGNORE
  TA_API friend inline taString   capitalize(const taString& x);
  // #IGNORE

  TA_API friend inline void     cat(const taString&, const taString&, taString&);
  // #IGNORE
  TA_API friend inline void     cat(const taString&, const char*, taString&);
  // #IGNORE
  TA_API friend inline void     cat(const taString&, const char* str, uint slen, taString&);
  // #IGNORE slen must be set
  TA_API friend inline void     cat(const taString&, char, taString&);
  // #IGNORE
  TA_API friend inline void     cat(const char*, const taString&, taString&);
  // #IGNORE
  TA_API friend inline void     cat(const char*, const char*, taString&);
  // #IGNORE
  TA_API friend inline void     cat(const char*, char, taString&);
  // #IGNORE concatenate first 2 args, store result in last arg

  // IO
#ifdef __MAKETA__
  TA_API friend ostream&   operator<<(ostream& s, const taString& x);
  TA_API friend istream&   operator>>(istream& s, taString& x); //

  TA_API friend int        readline(istream& s, taString& x,
                             char terminator = '\n',
                             int discard_terminator = 1);
  TA_API friend int        readline_auto(istream& strm, taString& x);
  // reads a line regardless of OS terminator (n rn r) convention of the stream (discarding terminators); returns num chars read
  bool                  Save_str(ostream& ostrm);
  bool                  Load_str(istream& istrm);
  // #IGNORE
#else
  taString&   operator<<(char c) { return cat(c); }
  taString&   operator<<(const char* x) { return cat(x); }
  taString&   operator<<(const taString& x) { return cat(x); }
  taString&   operator<<(int i) { return cat(taString(i)); }
  taString&   operator<<(long i) { return cat(taString(i)); }
  taString&   operator<<(ulong i) { return cat(taString(i)); }
  taString&   operator<<(ta_int64_t i) { return cat(taString(i)); }
  taString&   operator<<(ta_uint64_t i) { return cat(taString(i)); }
  taString&   operator<<(float i) { return cat(taString(i)); }
  taString&   operator<<(double i) { return cat(taString(i)); }
  taString&   operator<<(void* i) { return cat(taString(i)); }
  taString&   operator<<(bool i) { return cat(taString(i)); }
#ifdef TA_USE_QT
  taString&   operator<<(const QString& i) { return cat(taString(i)); }
#endif

  TA_API friend std::ostream&   operator<<(std::ostream& s, const taString& x);
  TA_API friend std::istream&   operator>>(std::istream& s, taString& x);
  TA_API friend int        readline(std::istream& s, taString& x,
                                 char terminator = '\n',
                                 int discard_terminator = 1);
  TA_API friend int     readline_auto(std::istream& strm, taString& x);
  bool                  Load_str(std::istream& istrm); // load contents from a stream
  bool                  Save_str(std::ostream& ostrm); // save contents to a stream
#endif
  bool                  LoadFromFile(const String& fname);
  // load contents of given file into string
  bool                  SaveToFile(const String& fname);
  // save contents of string to given file name

  ////////////////////////////////////////////////
  // constructors & assignment

  taString() {newRep(ADDR_NIL_STR_REP);} // el blanco
  taString(const taString& x) {newRep(x.mrep);} // copy constructor -- only a ref on source! (fast!)
  taString(const taString* x) {if (x) newRep(x->mrep); else newRep(ADDR_NIL_STR_REP);}
  taString(const char* s) {init(s, -1);} // s can be NULL
  taString(const char* s, int slen) {init(s, slen);}
  taString(taStrRep* x) {newRep(x);} // typically only used internally
  taString(uint slen, uint sz, char fill); // for allocating a writeable buffer; (1) if sz==0, then sz=slen; (2) if fill==0, then slen will be forced to 0


  ////////////////////////////////////////////////
  // conversion constructors

  explicit taString(bool b); //note: implicit causes evil problems, esp. by converting pointers to strings
  taString(char c) {if (c == '\0') newRep(ADDR_NIL_STR_REP); else init(&c, 1);}
  taString(int i, const char* format = "%d");
  taString(uint u, const char* format = "%u");
  taString(long i, const char* format = "%ld"); //note: don't use long any more, compatibility only
  taString(ulong u, const char* format = "%lu"); //note: don't use long any more, compatibility only
  taString(ta_int64_t i64); //NOTE: no fmts because they are not cross-platform standard
  taString(ta_uint64_t u64); //NOTE: no fmts because they are not cross-platform standard
  taString(float f, const char* format = "%g");
  taString(double d, const char* format = "%lg");
  explicit taString(void* p); //converts to hex
#ifdef TA_USE_QT
  taString(const QString& val);
  taString&         operator = (const QString& y);
  const QString         toQString() const; // #IGNORE evil C++ necessitates this!!!
  operator QString() const;  // #IGNORE
  operator QVariant() const;  // #IGNORE
#endif

#ifdef DEBUG
  ~taString() {taStrRep::unRef(mrep); mrep = NULL;}
#else
  ~taString() {taStrRep::unRef(mrep);}
#endif
  ////////////////////////////////////////////////
  // basic resource allocation and infrastructure

  void                  setRep(taStrRep* rep_);
  // #IGNORE for replacing rep (non-constructor) -- rep_ must be non-null

  inline int            length() const {return mrep->len;}
  // #CAT_Access how many characters in the string
  inline bool           empty() const {return (mrep->len == 0);}
  // #CAT_Access true if the string is empty
  inline bool           nonempty() const {return (mrep->len > 0);}
  // #CAT_Access true if the string is nonempty

  int                   allocation() const {return mrep->sz;}
  // #IGNORE
  int                   assertLength();
  // #IGNORE call after manual buffer operations, to set length from null term -- **WARNING** only use on ref==1, and where there is a null term in the buff, and sz is not overflowed
  inline const char*    chars() const {return mrep->s;}
  // #IGNORE returns null-terminated string DO NOT MODIFY
  char*                 chars_ptr() {makeUnique(); return mrep->s;}
  // #IGNORE returns point to the string, for in-place mods -- **this makeUnique() first, so you have a unique instance to overwrite**

  void                  makeUnique();
  // #IGNORE make sure we are only owner of mrep
  void                  makeUnique(uint min_new_sz);
  // #IGNORE makeUnique(), but force sz to be at least new_sz
  void                  pack();
  // #IGNORE if there is any extra space (above a small threshold), release it

  ////////////////////////////////////////////////
  // conversion operators

  operator unsigned char() const { return (unsigned char)strtol(chars(), NULL, 0); }
  // note: this is for 'byte' conversion -- we hope that char per-se is 'signed char'
  operator int() const  { return strtol(chars(), NULL, 0); }
  operator uint() const { return toUInt(); }
  operator long() const { return strtol(chars(), NULL, 0); }
  operator long long() const {return toInt64();}
  operator unsigned long long() const {return toUInt64();}
  operator float() const { return (float)atof(chars()); }
  operator double() const { return atof(chars()); }
  operator char*() const { return (char*)chars(); }
  operator const char*() const {return mrep->s;}

  bool                  isInt() const;
  // #CAT_Convert true if the string contains a value that can be interpreted as an integer [+-]dd*

  bool                  toBool() const;
  // #CAT_Convert accepts true as starting with t/T, or else 1
  char                  toChar() const;
  // #CAT_Convert if size 1, then that's it, else try heuristics

  short                 toShort(bool* ok = 0, int base = 10) const;
  // #IGNORE
  ushort                toUShort(bool* ok = 0, int base = 10) const;
  // #IGNORE
  int                   toInt(bool* ok = 0, int base = 10) const;
  // #CAT_Convert to integer value -- base determines base representation (hex=16, oct=8)
  uint                  toUInt(bool* ok = 0, int base = 10) const;
  // #IGNORE
  ta_int64_t            toInt64(bool* ok = 0, int base = 10) const;
  // #IGNORE
  ta_uint64_t           toUInt64(bool* ok = 0, int base = 10) const;
  // #IGNORE
  float                 toFloat(bool* ok = 0) const;
  // #CAT_Convert to floating point (single precision) value
  double                toDouble(bool* ok = 0) const;
  // #CAT_Convert to floating point (double precision) value

  taString&               convert(int i, const char* format = "%d");
  // #IGNORE
  taString&               convert(long i, const char* format = "%ld");
  // #IGNORE
  taString&               convert(float f, const char* format = "%g");
  // #IGNORE
  taString&               convert(double f, const char* format = "%lg");
  // #IGNORE

  int                   HexToInt() const { return strtol(chars(), NULL, 16); }
  // #CAT_Convert hexidecimal string to integer value

  taString&               operator=(const taString& y) {setRep(y.mrep); return *this;}
  taString&               operator=(const char* s) {return set(s, -1);}
  taString&               operator=(char c) {if (c == '\0')
    {setRep(ADDR_NIL_STR_REP); return *this;} else return set(&c, 1);} //

  taString&         set(const char* t, int len);
  // #IGNORE parameterized set -- used in assigns

  ////////////////////////////////////////////////
  // concatenation -- note: we try to do it in place, if possible (if refs==1, and enough space)

  taString&               cat(char c);
  taString&               cat(const char* t);
  taString&               cat(const taString& y);
  // #CAT_Modify concatenate (add) string to end of this one

  taString&               prepend(char c);
  taString&               prepend(const char* t);
  taString&               prepend(const taString& y);
  // #CAT_Modify insert string at beginning of this one

  taString&               operator += (const taString& y) {return cat(y);}
  taString&               operator += (const char* t) {return cat(t);}
  taString&               operator += (char c) {return cat(c);}

  ////////////////////////////////////////////////
  // searching & matching

  int                   index(char c, int startpos = 0) const;
  int                   index(const char* t, int startpos = 0) const;
  int                   index(const taString& y, int startpos = 0) const;
  // #CAT_Find return position of target in string or -1 for failure. startpos = starting position (- = search from end forward)
  int                   index_ci(const taString& y, int startpos = 0) const;
  // #CAT_Find ci = case independent: return position of target in string or -1 for failure. startpos = starting position (- = search from end forward)

  bool                  contains(char c, int startpos=0) const;
  bool                  contains(const char* t, int startpos=0) const;
  bool                  contains(const taString& y, int startpos=0) const;
  // #CAT_Find return 'true' if target appears anywhere in String. startpos = starting position (- = search from end forward)
  bool                  contains_ci(const taString& y, int startpos=0) const;
  // #CAT_Find ci = case independent: return 'true' if target appears anywhere in String. startpos = starting position (- = search from end forward)

  bool                  matches(char c, int pos = 0) const;
  bool                  matches(const char* t, int pos = 0) const;
  bool                  matches(const taString& y, int pos = 0) const;
  // #CAT_Find return 'true' if target appears at position pos in String

#ifdef TA_USE_QT
  bool			matches_wildcard(const taString& wild) const;
  // #CAT_Find return 'true' if target wildcard string matches this string -- ? matches any single character, * matches a string of characters, and [...] matches any set of characters within the brackets
  bool			matches_regexp(const taString& regexp) const;
  // #CAT_Find return 'true' if target regular expression string matches this string -- . matches any single character, .* matches a string of characters, [...] matches any set of characters within the brackets, c{1,5} matches 1-5 repeats of given character, ^ anchors to start of string, $ to end, \\d = digit, \\D = non-digit, \\s = whitespace, \\S = non-whitespace, \\w = word (letter or number) \\W = not
#endif

  bool                  endsWith(char c) const;
  bool                  endsWith(const char* t) const;
  bool                  endsWith(const taString& y) const;
  // #CAT_Find return 'true' if target is at end of String

  bool                  startsWith(char c) const;
  bool                  startsWith(const char* t) const;
  bool                  startsWith(const taString& y) const;
  // #CAT_Find return 'true' if target is at start of String

  int                   freq(char        c) const;
  int                   freq(const char* t) const;
  int                   freq(const taString& y) const;
  // #CAT_Find return number of occurences of target in String

  ////////////////////////////////////////////////
  // String extraction (access)

  taString                operator () (int pos, int len) const; // synonym for at
  char                  operator [] (int i) const;
  char&                 operator [] (int i); // writable -- NOTE: every use calls makeUnique
#ifndef NO_TA_BASE
  taString                operator [] (const Variant& i) const;
  // supports slices and lists of coordinates in addition to just an int index
#endif

  char                  elem(int i) const;
  // #CAT_Access get the character at index i
  char                  firstchar() const {return mrep->s[0];}
  // #CAT_Access get the first character, '\0 if empty
  char                  lastchar() const
  {if (mrep->len) return mrep->s[mrep->len - 1]; else return '\0';}
  // #CAT_Access get the last character; '\0 if empty

  taString                at(const taString&  x, int startpos = 0) const;
  taString                at(const char* t, int startpos = 0) const;
  taString                at(char c, int startpos = 0) const;
  taString                at(int pos, int len) const;
  // #CAT_Access get substring at position for length

  taString                before(int pos) const;
  taString                before(char c, int startpos = 0) const;
  taString                before(const char* t, int startpos = 0) const;
  taString                before(const taString& x, int startpos = 0) const;
  // #CAT_Access get substring before (not including) target string. startpos = starting position (- = search from end forward)

  taString                through(int pos) const;
  taString                through(char c, int startpos = 0) const;
  taString                through(const char* t, int startpos = 0) const;
  taString                through(const taString& x, int startpos = 0) const;
  // #CAT_Access get substring through (including) target string. startpos = starting position (- = search from end forward)

  taString                from(int pos) const;
  taString                from(char c, int startpos = 0) const;
  taString                from(const char* t, int startpos = 0) const;
  taString                from(const taString& x, int startpos = 0) const;
  // #CAT_Access get substring from (including) target string. startpos = starting position (- = search from end forward)

  taString                after(int pos) const;
  taString                after(char c, int startpos = 0) const;
  taString                after(const char* t, int startpos = 0) const;
  taString                after(const taString& x, int startpos = 0) const;
  // #CAT_Access get substring after (not including) target string. startpos = starting position (- = search from end forward)

  taString                between(char st_c, char ed_c, int startpos = 0) const;
  taString                between(const char* st_str, const char* ed_str, int startpos = 0) const;
  taString                between(const taString& st_str, const taString& ed_str, int startpos = 0) const;
  // #CAT_Access get substring between (not including) target strings. startpos = starting position (- = search from end forward)

  inline taString         left(int len) const {return before(len);}
  // #CAT_Access get leftmost len chars
  taString                right(int len) const;
  // #CAT_Access get rightmost len chars

  ////////////////////////////////////////////////
  // taString modification

  taString                elidedTo(int len = -1) const;
  // #CAT_Modify return a string no more than len long, no line breaks, eliding chars if needed and adding ... marks; -1 is no eliding
  taString                elidedToFirstLine() const;
  // #CAT_Modify if has newlines, elide to first line

  void                  del(const taString& y, int startpos = 0);
  void                  del(const char* t, int startpos = 0);
  void                  del(char c, int startpos = 0);
  void                  del(int pos, int len);
  // #CAT_Modify delete len chars starting at pos
  void                  remove(const taString& y, int startpos = 0) { del(y, startpos); }
  // #CAT_Modify remove target string from string. startpos = starting position (- = search from end forward)

  int                   gsub(const char* pat, const taString&     repl);
  int                   gsub(const char* pat, const char* repl);
  int                   gsub(const taString& pat, const taString& repl);
  // #CAT_Modify global substitution: substitute all occurrences of pat with repl

  taString&               repl(const char* pat, const taString&     repl);
  taString&               repl(const char* pat, const char* repl);
  taString&               repl(const taString& pat, const taString& repl);
  // #CAT_Modify global substitution: substitute all occurrences of pat with repl and return the modified string

  // in-place versions of friends -- they automatically makeUnique

  taString&               reverse();
  // #CAT_Modify reverse order of this string (NOTE: modifies this string, and also returns the resulting string value)
  taString&               upcase();
  // #CAT_Modify convert all letters to upper case of this string (NOTE: modifies this string, and also returns the resulting string value)
  taString&               downcase();
  // #CAT_Modify convert all letters to lower case of this string (NOTE: modifies this string, and also returns the resulting string value)
  taString&               capitalize();
  // #CAT_Modify capitalize the first letter of each word of this string (NOTE: modifies this string, and also returns the resulting string value)
  taString&               xml_esc();
  // #CAT_Modify xml escape, ex & to &amp; of this string (NOTE: modifies this string, and also returns the resulting string value)
  taString&               quote_esc();
  // #CAT_Modify quote escape, replacing double quotes " and back-slashes with their safely quoted forms (just adds a backslash) -- makes the string safe to print or save (NOTE: modifies this string, and also returns the resulting string value)

  void                  truncate(uint new_len);
  // #CAT_Modify shortens the string to new_len (if less than curr)

  void                  error(const char* msg) const;
  // #IGNORE report an error

protected:
  static void           AppendCharToCppStringLiteral(taString& str, char c, bool char_mode);
  // #IGNORE

  taStrRep*             mrep;   // Strings are pointers to their representations
  void                  init(const char* s, int slen = -1); // for calling in constructors
  void                  newRep(taStrRep* rep_); // for setting rep in a constructor
  taString&               cat(const char* s, int slen); // for internal use

  // some helper functions
  int                   search(int start, int sl, const char* t, int tl = -1) const
        {return mrep->search(start, sl, t, tl);}
  int                   search(int start, int sl, char c) const {return mrep->search(start, sl, c);}
  int                   search_ci(int start, int sl, const char* t, int tl = -1) const
        {return mrep->search_ci(start, sl, t, tl);}
  int                   match(int start, int sl, int exact, const char* t, int tl = -1) const
        {return mrep->match(start, sl, exact, t, tl);}
  int                   _gsub(const char*, int, const char* ,int);
  taString                _substr(int pos, int slen) const; // return a substring
private:
  int                   _gsub_lt(const char*, int, const char* ,int);
  int                   _gsub_eq(const char*, int, const char*);
  int                   _gsub_gt(const char*, int, const char* ,int);
}; //

//extern taString _nilString; // an empty string, for convenience
//note: can't use global static, because ctor not guarenteed to run before use
#define _nilString taString()

#define STRING_BUF(name, size)  taString name(0, size, '\0')

// this is provided for placed instances, where memory is already supplied
// the owner of a placed versions MUST call s->~String() manually, to ensure proper destruction

inline void* operator new(size_t, taString* str) { return str; }

// other externs

int        TA_API compare(const taString& x, const taString& y);
int        TA_API compare(const taString& x, const char* y);
int        TA_API compare(const char* y, const taString& x);
int        TA_API compare(const taString& x, char y);
int        TA_API compare(char y, const taString& x);
int        TA_API fcompare(const taString& x, const taString& y); // ignore case



#ifdef TA_USE_QT
// for some reason, these need to be here (inline), otherwise implicit use of these
// functions gets reported as "undefined reference to xxxx" by the linker... weird...
inline taString::taString(const QString& y) {
  init(y.toLatin1(), y.length());
}

inline taString::operator QString() const {
  QString result(chars());
  return result;
}

inline taString::operator QVariant() const {
  QVariant result(chars());
  return result;
}

inline taString& taString::operator = (const QString& y) {
  return set(y.toLatin1(), y.length());
}

inline const QString taString::toQString() const {
  return QString(chars());
}

  // converter routines, for use when linked with Qt
inline short taString::toShort(bool* ok, int base) const
  {QString tmp(chars()); return tmp.toShort(ok, base);}
inline ushort taString::toUShort(bool* ok, int base) const
  {QString tmp(chars()); return tmp.toUShort(ok, base);}
inline int taString::toInt(bool* ok, int base) const
  {QString tmp(chars()); return tmp.toInt(ok, base);}
inline uint taString::toUInt(bool* ok, int base) const
  {QString tmp(chars()); return tmp.toUInt(ok, base);}
inline ta_int64_t taString::toInt64(bool* ok, int base) const
  {QString tmp(chars()); return tmp.toLongLong(ok, base);}
inline ta_uint64_t taString::toUInt64(bool* ok, int base) const
  {QString tmp(chars()); return tmp.toULongLong(ok, base);}
inline float taString::toFloat(bool* ok) const
{ QString tmp(chars()); bool okk; float rval = tmp.toFloat(&okk);
  if(!okk && freq(',') == 1) { tmp.replace(',', '.'); rval = tmp.toFloat(&okk); }
  if(ok) *ok = okk; return rval; }
inline double taString::toDouble(bool* ok) const
{ QString tmp(chars()); bool okk; double rval = tmp.toDouble(&okk);
  if(!okk && freq(',') == 1) { tmp.replace(',', '.'); rval = tmp.toDouble(&okk); }
  if(ok) *ok = okk; return rval; }
#else
//NOTE: these are primarily just for maketa, and are not fully functional
inline short taString::toShort(bool* ok, int base) const
  {if (ok) *ok = true; return (short)strtol(chars(), NULL, base);}
inline ushort taString::toUShort(bool* ok, int base) const
  {if (ok) *ok = true; return (ushort)strtoul(chars(), NULL, base);}
inline int taString::toInt(bool* ok, int base) const
  {if (ok) *ok = true; return strtol(chars(), NULL, base);}
inline uint taString::toUInt(bool* ok, int base) const
  {if (ok) *ok = true; return strtoul(chars(), NULL, base);}
inline ta_int64_t taString::toInt64(bool* ok, int base) const
  {if (ok) *ok = true; return strtoll(chars(), NULL, base);}
inline ta_uint64_t taString::toUInt64(bool* ok, int base) const
  {if (ok) *ok = true; return strtoull(chars(), NULL, base);}
inline float taString::toFloat(bool* ok) const
  {if (ok) *ok = true; return (float)strtod(chars(), NULL);}
inline double taString::toDouble(bool* ok) const
  {if (ok) *ok = true; return strtod(chars(), NULL);}
#endif

inline void taString::newRep(taStrRep* rep_) {
  rep_->ref();
  mrep = rep_;
}

inline void taString::setRep(taStrRep* rep_) {
  rep_->ref(); //note: implicitly handles rare case of mrep=rep_
  taStrRep::unRef(mrep);
  mrep = rep_;
}

// assignment

// Zillions of cats...

TA_API inline void cat(const taString& x, const taString& y, taString& r)
{
  r.setRep(Scat(x.chars(), x.length(), y.chars(), y.length()));
}

TA_API inline void cat(const taString& x, const char* y, taString& r)
{
  r.setRep(Scat(x.chars(), x.length(), y, -1));
}

TA_API inline void cat(const taString& x, const char* y, uint slen, taString& r)
{
  r.setRep(Scat(x.chars(), x.length(), y, slen));
}

TA_API inline void cat(const taString& x, char y, taString& r)
{
  if (y == '\0') r = x;
  else r.setRep(Scat(x.chars(), x.length(), &y, 1));
}

TA_API inline void cat(const char* x, const taString& y, taString& r)
{
  r.setRep(Scat(x, -1, y.chars(), y.length()));
}

TA_API inline void cat(const char* x, const char* y, taString& r)
{
  r.setRep(Scat(x, -1, y, -1));
}

TA_API inline void cat(const char* x, char y, taString& r)
{
  if (y == '\0') r = x;
  else r.setRep(Scat(x, -1, &y, 1));
}

// operator versions

// constructive concatenation
// only MSVC could be so anal as to not coerce char* to const char* ...

inline taString operator + (const taString& x, const taString& y)
{
  taString r;  cat(x, y, r);  return r;
}

inline taString operator + (const taString& x, const char* y)
{
  taString r; cat(x, y, r); return r;
}

inline taString operator + (const taString& x, char* y)
{
  taString r; cat(x, y, r); return r;
}

inline taString operator + (const taString& x, char y)
{
  taString r; cat(x, y, r); return r;
}

inline taString operator + (const char* x, const taString& y)
{
  taString r; cat(x, y, r); return r;
}

inline taString operator + (char* x, const taString& y)
{
  taString r; cat(x, y, r); return r;
}
#ifdef TA_USE_QT
inline taString operator + (const QString& x, const taString& y)
{
  taString r;
  r.setRep(Scat(x.toLatin1(), x.length(), y.chars(), y.length()));
  return r;
}

inline taString operator + (const taString& x, const QString& y)
{
  taString r;
  r.setRep(Scat(x.chars(), x.length(), y.toLatin1(), y.length()));
  return r;
}
#endif

TA_API inline taString reverse(const taString& x)
{
  taString r; r.setRep(Sreverse(x.mrep)); return r;
}

TA_API inline taString upcase(const taString& x)
{
  taString r(x); r.upcase(); return r;
}

TA_API inline taString downcase(const taString& x)
{
  taString r(x); r.downcase(); return r;
}

TA_API inline taString capitalize(const taString& x)
{
  taString r(x); r.capitalize(); return r;
}


// element extraction

inline char  taString::operator [] (int i) const {
#ifdef DEBUG
  if (i >= length()) error("invalid index");
#endif
  return mrep->s[i];
}

inline char&  taString::operator [] (int i) {
#ifdef DEBUG
  if (i >= length()) error("invalid index");
#endif
  makeUnique();
  return mrep->s[i];
}

inline char  taString::elem (int i) const {
#ifdef DEBUG
  if (i >= length()) error("invalid index");
#endif
  return mrep->s[i];
}

// searching

inline int taString::index(char c, int startpos) const
{
  return search(startpos, length(), c);
}

inline int taString::index(const char* t, int startpos) const
{
  return search(startpos, length(), t);
}

inline int taString::index(const taString& y, int startpos) const
{
  return search(startpos, length(), y.chars(), y.length());
}

inline int taString::index_ci(const taString& y, int startpos) const
{
  return search_ci(startpos, length(), y.chars(), y.length());
}

inline bool taString::contains(char c, int p) const
{
  return search(p, length(), c) >= 0;
}

inline bool taString::contains(const char* t, int p) const
{
  return search(p, length(), t) >= 0;
}

inline bool taString::contains(const taString& y, int p) const
{
  return search(p, length(), y.chars(), y.length()) >= 0;
}

inline bool taString::contains_ci(const taString& y, int p) const
{
  return search_ci(p, length(), y.chars(), y.length()) >= 0;
}

inline bool taString::matches(const taString& y, int p) const
{
  return match(p, length(), 0, y.chars(), y.length()) >= 0;
}

inline bool taString::matches(const char* t, int p) const
{
  return match(p, length(), 0, t) >= 0;
}

inline bool taString::matches(char c, int p) const
{
  return match(p, length(), 0, &c, 1) >= 0;
}

inline int taString::gsub(const taString& pat, const taString& r)
{
  return _gsub(pat.chars(), pat.length(), r.chars(), r.length());
}

inline int taString::gsub(const char* pat, const taString& r)
{
  return _gsub(pat, -1, r.chars(), r.length());
}

inline int taString::gsub(const char* pat, const char* r)
{
  return _gsub(pat, -1, r, -1);
}

inline taString& taString::repl(const taString& pat, const taString& r)
{
  _gsub(pat.chars(), pat.length(), r.chars(), r.length());
  return *this;
}

inline taString& taString::repl(const char* pat, const taString& r)
{
  _gsub(pat, -1, r.chars(), r.length());
  return *this;
}

inline taString& taString::repl(const char* pat, const char* r)
{
  _gsub(pat, -1, r, -1);
  return *this;
}



// a zillion comparison operators

#ifdef TA_USE_QT
inline bool operator==(const QString& x, const taString& y) {
  return compare(taString(x), y) == 0;
}
inline bool operator==(const taString& x, const QString& y) {
  return compare(x, taString(y)) == 0;
}
inline bool operator!=(const QString& x, const taString& y) {
  return compare(taString(x), y) != 0;
}
inline bool operator!=(const taString& x, const QString& y) {
  return compare(x, taString(y)) != 0;
}
#endif

inline bool operator==(const taString& x, const taString& y)
{
  return compare(x, y) == 0;
}

inline bool operator!=(const taString& x, const taString& y)
{
  return compare(x, y) != 0;
}

inline bool operator>(const taString& x, const taString& y)
{
  return compare(x, y) > 0;
}

inline bool operator>=(const taString& x, const taString& y)
{
  return compare(x, y) >= 0;
}

inline bool operator<(const taString& x, const taString& y)
{
  return compare(x, y) < 0;
}

inline bool operator<=(const taString& x, const taString& y)
{
  return compare(x, y) <= 0;
}



// const taString&, char*
inline bool operator==(const taString& x, char* t)
{
  return compare(x, t) == 0;
}

inline bool operator!=(const taString& x, char* t)
{
  return compare(x, t) != 0;
}

inline bool operator>(const taString& x, char* t)
{
  return compare(x, t) > 0;
}

inline bool operator>=(const taString& x, char* t)
{
  return compare(x, t) >= 0;
}

inline bool operator<(const taString& x, char* t)
{
  return compare(x, t) < 0;
}

inline bool operator<=(const taString& x, char* t)
{
  return compare(x, t) <= 0;
}

// const char*, taString&
inline bool operator==(char* x, const taString& t)
{
  return compare(x, t) == 0;
}

inline bool operator!=(char* x, const taString& t)
{
  return compare(x, t) != 0;
}

inline bool operator>(char* x, const taString& t)
{
  return compare(x, t) > 0;
}

inline bool operator>=(char* x, const taString& t)
{
  return compare(x, t) >= 0;
}

inline bool operator<(char* x, const taString& t)
{
  return compare(x, t) < 0;
}

inline bool operator<=(char* x, const taString& t)
{
  return compare(x, t) <= 0;
}


// const taString&, const char*
inline bool operator==(const taString& x, const char* t)
{
  return compare(x, t) == 0;
}

inline bool operator!=(const taString& x, const char* t)
{
  return compare(x, t) != 0;
}

inline bool operator>(const taString& x, const char* t)
{
  return compare(x, t) > 0;
}

inline bool operator>=(const taString& x, const char* t)
{
  return compare(x, t) >= 0;
}

inline bool operator<(const taString& x, const char* t)
{
  return compare(x, t) < 0;
}

inline bool operator<=(const taString& x, const char* t)
{
  return compare(x, t) <= 0;
}

// const const char*, taString&
inline bool operator==(const char* x, const taString& t)
{
  return compare(x, t) == 0;
}

inline bool operator!=(const char* x, const taString& t)
{
  return compare(x, t) != 0;
}

inline bool operator>(const char* x, const taString& t)
{
  return compare(x, t) > 0;
}

inline bool operator>=(const char* x, const taString& t)
{
  return compare(x, t) >= 0;
}

inline bool operator<(const char* x, const taString& t)
{
  return compare(x, t) < 0;
}

inline bool operator<=(const char* x, const taString& t)
{
  return compare(x, t) <= 0;
}

// const taString&, char
inline bool operator==(const taString& x, char t)
{
  return compare(x, t) == 0;
}

inline bool operator!=(const taString& x, char t)
{
  return compare(x, t) != 0;
}

inline bool operator>(const taString& x, char t)
{
  return compare(x, t) > 0;
}

inline bool operator>=(const taString& x, char t)
{
  return compare(x, t) >= 0;
}

inline bool operator<(const taString& x, char t)
{
  return compare(x, t) < 0;
}

inline bool operator<=(const taString& x, char t)
{
  return compare(x, t) <= 0;
}


// const char,  taString&
inline bool operator==(char x, const taString& t)
{
  return compare(x, t) == 0;
}

inline bool operator!=(char x, const taString& t)
{
  return compare(x, t) != 0;
}

inline bool operator>(char x, const taString& t)
{
  return compare(x, t) > 0;
}

inline bool operator>=(char x, const taString& t)
{
  return compare(x, t) >= 0;
}

inline bool operator<(char x, const taString& t)
{
  return compare(x, t) < 0;
}

inline bool operator<=(char x, const taString& t)
{
  return compare(x, t) <= 0;
}

#endif // taString_h
