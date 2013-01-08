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

#ifndef taString_h
#define taString_h 1

// parent includes:

// member includes:

// declare all other types mentioned but not required to include:
class ostream; // 


class TA_API String {
  // string of characters with many useful methods for string manipulation
friend class taStrRep;
public:
  ////////////////////////////////////////////////
  // statics

  static const String con_0; // #IGNORE "0" occurs so often, we optimize it with a const
  static const String con_1; // #IGNORE "1" occurs so often, we optimize it with a const
  static const String con_NULL; // #IGNORE "NULL" occurs so often, we optimize it with a const

  static const String   CharToCppLiteral(char c);
  // #IGNORE converts a character to a C++ valid literal; can be embedded in a C++ string
  static const String   StringToCppLiteral(const String& str);
  // #IGNORE converts a string to a C++ valid literal

  ////////////////////////////////////////////////
  // global functions:

  TA_API friend int     split(const String& x, String res[], int maxn, const String& sep);
  // #IGNORE split string into array res at separators; return number of elements
  TA_API friend String  common_prefix(const String& x, const String& y, int startpos = 0);
  // #IGNORE
  TA_API friend String  common_suffix(const String& x, const String& y,int startpos = -1);
  // #IGNORE
  TA_API friend String  replicate(char c, int n);
  // #IGNORE
  TA_API friend String  replicate(const char* y, int n);
  // #IGNORE
  TA_API friend String  replicate(const String& y, int n);
  // #IGNORE
  TA_API friend String  join(const String src[], int n, const String& sep); //
  // #IGNORE

  // simple builtin transformations
  TA_API friend String          triml(const String& x);
  // #IGNORE trims leading spaces
  TA_API friend String          trimr(const String& x);
  // #IGNORE trims trailing spaces
  TA_API friend String          trim(const String& x);
  // #IGNORE trims leading and trailing  spaces
  TA_API friend inline String   reverse(const String& x);
  // #IGNORE
  TA_API friend inline String   upcase(const String& x);
  // #IGNORE
  TA_API friend inline String   downcase(const String& x);
  // #IGNORE
  TA_API friend inline String   capitalize(const String& x);
  // #IGNORE

  TA_API friend inline void     cat(const String&, const String&, String&);
  // #IGNORE
  TA_API friend inline void     cat(const String&, const char*, String&);
  // #IGNORE
  TA_API friend inline void     cat(const String&, const char* str, uint slen, String&);
  // #IGNORE slen must be set
  TA_API friend inline void     cat(const String&, char, String&);
  // #IGNORE
  TA_API friend inline void     cat(const char*, const String&, String&);
  // #IGNORE
  TA_API friend inline void     cat(const char*, const char*, String&);
  // #IGNORE
  TA_API friend inline void     cat(const char*, char, String&);
  // #IGNORE concatenate first 2 args, store result in last arg

  // IO
#ifdef __MAKETA__
  TA_API friend ostream&   operator<<(ostream& s, const String& x);
  TA_API friend istream&   operator>>(istream& s, String& x); //

  TA_API friend int        readline(istream& s, String& x,
                             char terminator = '\n',
                             int discard_terminator = 1);
  TA_API friend int        readline_auto(istream& strm, String& x);
  // reads a line regardless of OS terminator (n rn r) convention of the stream (discarding terminators); returns num chars read
  int                   Save_str(ostream& ostrm);
  int                   Load_str(istream& istrm);
  // #IGNORE
#else
  String&   operator<<(char c) { return cat(c); }
  String&   operator<<(const char* x) { return cat(x); }
  String&   operator<<(const String& x) { return cat(x); }
  String&   operator<<(int i) { return cat(String(i)); }
  String&   operator<<(long i) { return cat(String(i)); }
  String&   operator<<(ulong i) { return cat(String(i)); }
  String&   operator<<(ta_int64_t i) { return cat(String(i)); }
  String&   operator<<(ta_uint64_t i) { return cat(String(i)); }
  String&   operator<<(float i) { return cat(String(i)); }
  String&   operator<<(double i) { return cat(String(i)); }
  String&   operator<<(void* i) { return cat(String(i)); }
  String&   operator<<(bool i) { return cat(String(i)); }
#ifdef TA_USE_QT
  String&   operator<<(const QString& i) { return cat(String(i)); }
#endif

  TA_API friend std::ostream&   operator<<(std::ostream& s, const String& x);
  TA_API friend std::istream&   operator>>(std::istream& s, String& x);
  TA_API friend int        readline(std::istream& s, String& x,
                                 char terminator = '\n',
                                 int discard_terminator = 1);
  TA_API friend int     readline_auto(std::istream& strm, String& x);
  int                   Load_str(istream& istrm); // load contents from a stream
  int                   Save_str(std::ostream& ostrm); // save contents to a stream
#endif

  ////////////////////////////////////////////////
  // constructors & assignment

  String() {newRep(ADDR_NIL_STR_REP);} // el blanco
  String(const String& x) {newRep(x.mrep);} // copy constructor -- only a ref on source! (fast!)
  String(const String* x) {if (x) newRep(x->mrep); else newRep(ADDR_NIL_STR_REP);}
  String(const char* s) {init(s, -1);} // s can be NULL
  String(const char* s, int slen) {init(s, slen);}
  String(taStrRep* x) {newRep(x);} // typically only used internally
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
  String&         operator = (const QString& y);
  const QString         toQString() const; // #IGNORE evil C++ necessitates this!!!
  operator QString() const;  // #IGNORE
  operator QVariant() const;  // #IGNORE
#endif

#ifdef DEBUG
  ~String() {taStrRep::unRef(mrep); mrep = NULL;}
#else
  ~String() {taStrRep::unRef(mrep);}
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

  String&               convert(int i, const char* format = "%d");
  // #IGNORE
  String&               convert(long i, const char* format = "%ld");
  // #IGNORE
  String&               convert(float f, const char* format = "%g");
  // #IGNORE
  String&               convert(double f, const char* format = "%lg");
  // #IGNORE

  int                   HexToInt() const { return strtol(chars(), NULL, 16); }
  // #CAT_Convert hexidecimal string to integer value

  String&               operator=(const String& y) {setRep(y.mrep); return *this;}
  String&               operator=(const char* s) {return set(s, -1);}
  String&               operator=(char c) {if (c == '\0')
    {setRep(ADDR_NIL_STR_REP); return *this;} else return set(&c, 1);} //

  String&         set(const char* t, int len);
  // #IGNORE parameterized set -- used in assigns

  ////////////////////////////////////////////////
  // concatenation -- note: we try to do it in place, if possible (if refs==1, and enough space)

  String&               cat(char c);
  String&               cat(const char* t);
  String&               cat(const String& y);
  // #CAT_Modify concatenate (add) string to end of this one

  String&               prepend(char c);
  String&               prepend(const char* t);
  String&               prepend(const String& y);
  // #CAT_Modify insert string at beginning of this one

  String&               operator += (const String& y) {return cat(y);}
  String&               operator += (const char* t) {return cat(t);}
  String&               operator += (char c) {return cat(c);}

  ////////////////////////////////////////////////
  // searching & matching

  int                   index(char c, int startpos = 0) const;
  int                   index(const char* t, int startpos = 0) const;
  int                   index(const String& y, int startpos = 0) const;
  // #CAT_Find return position of target in string or -1 for failure. startpos = starting position (- = search from end forward)
  int                   index_ci(const String& y, int startpos = 0) const;
  // #CAT_Find ci = case independent: return position of target in string or -1 for failure. startpos = starting position (- = search from end forward)

  bool                  contains(char c, int startpos=0) const;
  bool                  contains(const char* t, int startpos=0) const;
  bool                  contains(const String& y, int startpos=0) const;
  // #CAT_Find return 'true' if target appears anywhere in String. startpos = starting position (- = search from end forward)
  bool                  contains_ci(const String& y, int startpos=0) const;
  // #CAT_Find ci = case independent: return 'true' if target appears anywhere in String. startpos = starting position (- = search from end forward)

  bool                  matches(char c, int pos = 0) const;
  bool                  matches(const char* t, int pos = 0) const;
  bool                  matches(const String& y, int pos = 0) const;
  // #CAT_Find return 'true' if target appears at position pos in String

#ifdef TA_USE_QT
  bool			matches_wildcard(const String& wild) const;
  // #CAT_Find return 'true' if target wildcard string matches this string -- ? matches any single character, * matches a string of characters, and [...] matches any set of characters within the brackets
  bool			matches_regexp(const String& regexp) const;
  // #CAT_Find return 'true' if target regular expression string matches this string -- . matches any single character, .* matches a string of characters, [...] matches any set of characters within the brackets, c{1,5} matches 1-5 repeats of given character, ^ anchors to start of string, $ to end, \\d = digit, \\D = non-digit, \\s = whitespace, \\S = non-whitespace, \\w = word (letter or number) \\W = not
#endif

  bool                  endsWith(char c) const;
  bool                  endsWith(const char* t) const;
  bool                  endsWith(const String& y) const;
  // #CAT_Find return 'true' if target is at end of String

  bool                  startsWith(char c) const;
  bool                  startsWith(const char* t) const;
  bool                  startsWith(const String& y) const;
  // #CAT_Find return 'true' if target is at start of String

  int                   freq(char        c) const;
  int                   freq(const char* t) const;
  int                   freq(const String& y) const;
  // #CAT_Find return number of occurences of target in String

  ////////////////////////////////////////////////
  // String extraction (access)

  String                operator () (int pos, int len) const; // synonym for at
  char                  operator [] (int i) const;
  char&                 operator [] (int i); // writable -- NOTE: every use calls makeUnique
#ifndef NO_TA_BASE
  String                operator [] (const Variant& i) const;
  // supports slices and lists of coordinates in addition to just an int index
#endif

  char                  elem(int i) const;
  // #CAT_Access get the character at index i
  char                  firstchar() const {return mrep->s[0];}
  // #CAT_Access get the first character, '\0 if empty
  char                  lastchar() const
  {if (mrep->len) return mrep->s[mrep->len - 1]; else return '\0';}
  // #CAT_Access get the last character; '\0 if empty

  String                at(const String&  x, int startpos = 0) const;
  String                at(const char* t, int startpos = 0) const;
  String                at(char c, int startpos = 0) const;
  String                at(int pos, int len) const;
  // #CAT_Access get substring at position for length

  String                before(int pos) const;
  String                before(char c, int startpos = 0) const;
  String                before(const char* t, int startpos = 0) const;
  String                before(const String& x, int startpos = 0) const;
  // #CAT_Access get substring before (not including) target string. startpos = starting position (- = search from end forward)

  String                through(int pos) const;
  String                through(char c, int startpos = 0) const;
  String                through(const char* t, int startpos = 0) const;
  String                through(const String& x, int startpos = 0) const;
  // #CAT_Access get substring through (including) target string. startpos = starting position (- = search from end forward)

  String                from(int pos) const;
  String                from(char c, int startpos = 0) const;
  String                from(const char* t, int startpos = 0) const;
  String                from(const String& x, int startpos = 0) const;
  // #CAT_Access get substring from (including) target string. startpos = starting position (- = search from end forward)

  String                after(int pos) const;
  String                after(char c, int startpos = 0) const;
  String                after(const char* t, int startpos = 0) const;
  String                after(const String& x, int startpos = 0) const;
  // #CAT_Access get substring after (not including) target string. startpos = starting position (- = search from end forward)

  String                between(char st_c, char ed_c, int startpos = 0) const;
  String                between(const char* st_str, const char* ed_str, int startpos = 0) const;
  String                between(const String& st_str, const String& ed_str, int startpos = 0) const;
  // #CAT_Access get substring between (not including) target strings. startpos = starting position (- = search from end forward)

  inline String         left(int len) const {return before(len);}
  // #CAT_Access get leftmost len chars
  String                right(int len) const;
  // #CAT_Access get rightmost len chars

  ////////////////////////////////////////////////
  // String modification

  String                elidedTo(int len = -1) const;
  // #CAT_Modify return a string no more than len long, no line breaks, eliding chars if needed and adding ... marks; -1 is no eliding
  String                elidedToFirstLine() const;
  // #CAT_Modify if has newlines, elide to first line

  void                  del(const String& y, int startpos = 0);
  void                  del(const char* t, int startpos = 0);
  void                  del(char c, int startpos = 0);
  void                  del(int pos, int len);
  // #CAT_Modify delete len chars starting at pos
  void                  remove(const String& y, int startpos = 0) { del(y, startpos); }
  // #CAT_Modify remove target string from string. startpos = starting position (- = search from end forward)

  int                   gsub(const char* pat, const String&     repl);
  int                   gsub(const char* pat, const char* repl);
  int                   gsub(const String& pat, const String& repl);
  // #CAT_Modify global substitution: substitute all occurrences of pat with repl

  String&               repl(const char* pat, const String&     repl);
  String&               repl(const char* pat, const char* repl);
  String&               repl(const String& pat, const String& repl);
  // #CAT_Modify global substitution: substitute all occurrences of pat with repl and return the modified string

  // in-place versions of friends -- they automatically makeUnique

  String&               reverse();
  // #CAT_Modify reverse order of this string (NOTE: modifies this string, and also returns the resulting string value)
  String&               upcase();
  // #CAT_Modify convert all letters to upper case of this string (NOTE: modifies this string, and also returns the resulting string value)
  String&               downcase();
  // #CAT_Modify convert all letters to lower case of this string (NOTE: modifies this string, and also returns the resulting string value)
  String&               capitalize();
  // #CAT_Modify capitalize the first letter of each word of this string (NOTE: modifies this string, and also returns the resulting string value)
  String&               xml_esc();
  // #CAT_Modify xml escape, ex & to &amp; of this string (NOTE: modifies this string, and also returns the resulting string value)
  String&               quote_esc();
  // #CAT_Modify quote escape, replacing double quotes " and back-slashes with their safely quoted forms (just adds a backslash) -- makes the string safe to print or save (NOTE: modifies this string, and also returns the resulting string value)

  void                  truncate(uint new_len);
  // #CAT_Modify shortens the string to new_len (if less than curr)

  void                  error(const char* msg) const;
  // #IGNORE report an error

protected:
  static void           AppendCharToCppStringLiteral(String& str, char c, bool char_mode);
  // #IGNORE

  taStrRep*             mrep;   // Strings are pointers to their representations
  void                  init(const char* s, int slen = -1); // for calling in constructors
  void                  newRep(taStrRep* rep_); // for setting rep in a constructor
  String&               cat(const char* s, int slen); // for internal use

  // some helper functions
  int                   search(int start, int sl, const char* t, int tl = -1) const
        {return mrep->search(start, sl, t, tl);}
  int                   search(int start, int sl, char c) const {return mrep->search(start, sl, c);}
  int                   search_ci(int start, int sl, const char* t, int tl = -1) const
        {return mrep->search_ci(start, sl, t, tl);}
  int                   match(int start, int sl, int exact, const char* t, int tl = -1) const
        {return mrep->match(start, sl, exact, t, tl);}
  int                   _gsub(const char*, int, const char* ,int);
  String                _substr(int pos, int slen) const; // return a substring
private:
  int                   _gsub_lt(const char*, int, const char* ,int);
  int                   _gsub_eq(const char*, int, const char*);
  int                   _gsub_gt(const char*, int, const char* ,int);
}; //

#endif // taString_h
