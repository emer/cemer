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

#include "taString.h"

/* HISTORICAL NOTE

Earlier versions of this file were based on the GNU String class
    written by Doug Lea (dl@rocky.oswego.edu)

The class was later completely rewritten to use counting semantics, and
it is the opinion of the authors that this new class represents an original
software work for purposes of copyright.

*/

/*
  taString class implementation
 */

#if (defined(TA_OS_WIN))
#include <windows.h>
// the Windows string.h doesn't define strcasecmp, so we use the Win32 equivalent
#define strcasecmp(s1,s2) lstrcmp(s1, s2)
#endif

#include <ctype.h>
#include <limits.h>
#include <stdio.h>

using namespace std;

//////////////////////////
//      misc funcs      //
//////////////////////////


#ifndef NO_TA_BASE
#include <Variant>
#include <int_Matrix.h>

taString  taString::operator [] (const Variant& i) const {
  taString rval;
  if(i.isNumeric()) {
    rval = elem(i.toInt());
  }
  else if(i.isMatrixType()) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(i.toMatrix());
    if(!cmat) {
      error("operator[], index matrix is NULL or not an int_Matrix");
      return rval;
    }
    if(cmat->size <= 0) return rval;
    if(cmat->dims() == 2 && cmat->dim(0) == 3 && cmat->dim(1) == 1) {
      // mode = IDX_SLICE;
      int start = cmat->FastEl_Flat(0);
      int end = cmat->FastEl_Flat(1);
      int step = cmat->FastEl_Flat(2);
      if(step == 0) step = 1;
      if(start < 0) start += length();
      if(end < 0) end += (length()+1); // needs the +1 to allow -1 to be the end and do <
      if(start > length()-1) start = length()-1; // keep in bounds
      if(start < 0) start = 0;
      if(end > length()) end = length();
      if(end < 0) end = 0;
      if(end < start) {
        error("operator[] IDX_SLICE, slice end is before start. start: "+
              taString(start) + " end: " + taString(end) + " step: " + taString(step));
        return rval;
      }
      if(step > 0) {
        for(int i = start; i < end; i += step) {
          rval += elem(i);
        }
      }
      else {
        for(int i = end-1; i >= start; i += step) {
          rval += elem(i);
        }
      }
    }
    else {                      // just a bunch of coords
      for(int i=0; i< cmat->size; i++) {
        int idx = cmat->FastEl_Flat(i);
        if(idx >= 0 && idx < length())
          rval += elem(idx);
      }
    }
  }
  return rval;
}

#endif

// capitalize a string, in-place
void _capitalize(char* p, int n) {
  char* e = &(p[n]);
  for (; p < e; ++p) {
    int at_word = islower(*p);
    if (at_word) {
      *p = toupper(*p);
    }
    else {
      at_word = isupper(*p) || isdigit(*p);
    }

    if (at_word) {
      while (++p < e) {
        if (isupper(*p)) {
          *p = tolower(*p);
        }
        /* A '\'' does not break a word, so that "Nathan's" stays
           "Nathan's" rather than turning into "Nathan'S". */
        else if (!islower(*p) && !isdigit(*p) && (*p != '\'')) {
          break;
        }
      }
    }
  }
}

int _search(const char* s, int start, int sl, char c) {
  if (sl > 0) {
    if (start >= 0) {
      const char* a = &(s[start]);
      const char* lasta = &(s[sl]);
      while (a < lasta) {
        if (*a++ == c) {
          return (int)(--a - s); //safeint
        }
      }
    }
    else {
      const char* a = &(s[sl + start + 1]);
      while (--a >= s) {
        if (*a == c) {
          return (int)(a - s);
        }
      }
    }
  }
  return -1;
}

int _search(const char* s, int start, int sl, const char* t, int tl) {
  if (tl < 0) tl =  t ? (int)strlen(t) : 0;
  if (sl > 0 && tl > 0) {
    if (start >= 0) {
      const char* lasts = &(s[sl - tl]);
      const char* lastt = &(t[tl]);
      const char* p = &(s[start]);

      while (p <= lasts) {
        const char* x = p++;
        const char* y = t;
        while (*x++ == *y++) {
          if (y >= lastt) {
            return (int)(--p - s); //safeint
          }
        }
      }
    }
    else {
      const char* firsts = &(s[tl - 1]);
      const char* lastt =  &(t[tl - 1]);
      const char* p = &(s[sl + start + 1]);

      while (--p >= firsts) {
        const char* x = p;
        const char* y = lastt;
        while (*x-- == *y--) {
          if (y < t) {
            return (int)(++x - s); //safeint
          }
        }
      }
    }
  }
  return -1;
}

int _search_ci(const char* s, int start, int sl, const char* t, int tl) {
  if (tl < 0) tl =  t ? (int)strlen(t) : 0;
  if (sl > 0 && tl > 0) {
    if (start >= 0) {
      const char* lasts = &(s[sl - tl]);
      const char* lastt = &(t[tl]);
      const char* p = &(s[start]);

      while (p <= lasts) {
        const char* x = p++;
        const char* y = t;
        while (tolower(*x++) == tolower(*y++)) {
          if (y >= lastt) {
            return (int)(--p - s); //safeint
          }
        }
      }
    }
    else {
      const char* firsts = &(s[tl - 1]);
      const char* lastt =  &(t[tl - 1]);
      const char* p = &(s[sl + start + 1]);

      while (--p >= firsts) {
        const char* x = p;
        const char* y = lastt;
        while (tolower(*x--) == tolower(*y--)) {
          if (y < t) {
            return (int)(++x - s); //safeint
          }
        }
      }
    }
  }
  return -1;
}

// string compare: first argument is known to be non-null

inline static int ncmp(const char* a, int al, const char* b, int bl)
{
  int n = (al <= bl) ? al : bl;
  while (n-- > 0) {
    int diff = *a++ - *b++;
    if (diff != 0) {
      return diff;
    }
  }
  return al - bl;
}


//////////////////////////
//      SRep            //
//////////////////////////

// note: compilers may report the size of taStrRep to be an even multiple of 4
// QAtomicInt is 4 on all the platforms we support
#define SIZE_OF_STRREP (9 + sizeof(QAtomicInt))

#ifdef TA_PROFILE
class Cstring_prof {
public:
  static int inst_cnt; // use 1 to avoid check for 0
  static long long tot_size;
  ~Cstring_prof(){
   cerr << "\nStrRep inst count: " << inst_cnt << "\n avg alloc: " << (tot_size / inst_cnt) << "\n";
  }
};
int Cstring_prof::inst_cnt = 0; // note: is at least 1, because of static nilstring
long long Cstring_prof::tot_size = 0;
Cstring_prof string_prof;
#endif

taStrRep _nilStrRep = { 0, 1, {1}, { 0 } }; // nil StrReps point here, we init cnt to 1 so never released
taStrRep* _nilStrRepPtr = &_nilStrRep;

// Memory Management optimizers:
// when about to malloc/calloc etc. a number of bytes, call tweak_alloc to optimally tweak the
//  allocation request size -- especially useful where the allocator use 2^n blocks, or 16n blocks

// Platform     bits    Overhead        Min/Gran Size           Allocator
// Linux        32      8 bytes         16/8 bytes      Doug Lea's malloc
// Linux        64      16 bytes        24/8 bytes      Doug Lea's malloc
// Mac OS       32      0               16/16 bytes

#if (defined(TA_OS_LINUX))
// Linux uses Doug Lea's malloc, which has:
// a 2-int(32/64) overhead; 2-int + 1-ptr min, and 8-byte granularity
// strategy: round up to the amount that we will get allocated regardless
inline size_t tweak_alloc(size_t n) {
  return  (((n + (2 * sizeof(intptr_t))) + 7) & ~((size_t)0x7)) - (2 * sizeof(intptr_t));
}

#elif (defined(TA_OS_WIN))
inline size_t tweak_alloc(size_t n) {
#ifdef DEBUG
  return  (((n + 16) + 15) & ~((size_t)0xF)) - 16;
#else
  return  n;
#endif
}

#elif defined(TA_OS_MAC)
// Mac OS uses an overhead-less allocator (alloc address determines size, based on maps)
// Mac OS has 16-byte granularity
inline size_t tweak_alloc(size_t n) {
  return  (((n + 0) + 15) & ~((size_t)0xF)) - 0;
}

#else
 // don't know anything about this allocator
 //note: rem'ed out, to make sure we always have a definitive version while compiling
//inline size_t tweak_alloc(size_t n) {return n;}

#endif

// create an empty buffer -- called by routines that then fill the chars (ex. reverse, upcase, etc.)
TA_API taStrRep* Snew(int slen, uint cap) {
  if (cap == 0) cap = slen;
  if (cap == 0) return ADDR_NIL_STR_REP;
  uint allocsize = (uint)tweak_alloc(SIZE_OF_STRREP + cap); // we tweak the alloc size to optimize -- may be larger
  cap = allocsize - SIZE_OF_STRREP; // in case we asked for more memory
#ifdef TA_PROFILE
  ++Cstring_prof::inst_cnt;
  Cstring_prof::tot_size += allocsize;
#endif
  taStrRep* rval = (taStrRep*)malloc(allocsize);
  if (!rval) {
    cerr << "Salloc: could not allocate memory for new string\n";
    return NULL; // note: best that we crash, since this is a serious error condition
  }
  rval->sz = cap;
  rval->len = slen;
  rval->cnt.store(0); // necessary, because ctor does not run and QBasicAtomicInt is POD
  rval->s[slen] = '\0';
  return rval;
}

// allocate a new taStrRep by copying from a given string (can be NULL)
TA_API taStrRep* Salloc(const char* s, int slen, uint cap) {
  if (slen < 0) slen = s ? (int)strlen(s) : 0;
  taStrRep* rval = Snew(slen, cap); //note: alloc failure returns NULL
  if (slen && (rval->sz >= (uint)slen)) memcpy(rval->s, s, slen);
  return rval;
}

// allocate & concatenate
TA_API taStrRep* Scat(taStrRep* srep, const char* s, int slen) {
  return Scat(srep->s, srep->len, s, slen);
}

TA_API taStrRep*        Scat(const char* s1, int slen1, const char* s2, int slen2) { // slen can be -1
  if (slen1 < 0) slen1 = s1 ? (int)strlen(s1) : 0;
  if (slen2 < 0) slen2 = s2 ? (int)strlen(s2) : 0;
  uint newlen = slen1 + slen2;
  if (newlen == 0) return ADDR_NIL_STR_REP;

  taStrRep* rval = Salloc(s1, slen1, newlen); //note: alloc failure returns _nilStrRep
  if (slen2) {
    memcpy(&(rval->s[slen1]), s2, slen2);
  }
  rval->len = newlen;
  rval->s[newlen] = '\0';
  return rval;
}

TA_API taStrRep* Sreverse(const taStrRep* src)
{
  taStrRep* dest = Snew(src->len);
  char* a = dest->s;
  char* b = (char *)&(src->s[src->len]); // start at one past end; harmless if len=0 (safe deconstify)
  for (uint n = 0; n < src->len; ++n) {
      a[n] = *(--b);
  }
  return dest;
}

void taStrRep::capitalize() {//note: should only be called on cnt==1
  _capitalize(s, len);
}

void taStrRep::cat(const char* str, uint slen) { // note: slen must be set, and canCat must have been true
  if (slen == 0) return;
  memcpy(&(s[len]), str, slen);
  len += slen;
  s[len] = '\0';
}

void taStrRep::downcase() { //note: should only be called on cnt==1
  for (uint n = 0; n < len; ++n) {
    if (isupper(s[n])) s[n] = tolower(s[n]);
  }
}

int taStrRep::match(int start, int sl, int exact, const char* t, int tl) const {
  if (tl < 0) tl =  t ? (int)strlen(t) : 0;

  if (start < 0) {
    start = sl + start - tl + 1;
    if (start < 0 || (exact && start != 0))
      return -1;
  } else if (exact && ((sl - start) != tl))
    return -1;

  if (sl == 0 || tl == 0 || sl - start < tl || start >= sl)
    return -1;

  int n = tl;
  const char* str = &(s[start]);
  while (--n >= 0) if (*str++ != *t++) return -1;
  return tl;
}
void taStrRep::prepend(const char* str, uint slen) { // note: slen must be set, and canCat must have been true
  if (slen == 0) return;
  memmove(&(s[slen]), s, len);
  memcpy(s, str, slen);
  len += slen;
  s[len] = '\0';
}

void taStrRep::reverse() {//note: should only be called on cnt<=1
  char* a = s;
  char* b = &(s[len - 1]);
  while (a < b) {
    char t = *a;
    *a++ = *b;
    *b-- = t;
  }
}

int taStrRep::search(int start, int sl, char c) const {
  return _search(s, start, sl, c);
}

int taStrRep::search(int start, int sl, const char* t, int tl) const {
  return _search(s, start, sl, t, tl);
}

int taStrRep::search_ci(int start, int sl, const char* t, int tl) const {
  return _search_ci(s, start, sl, t, tl);
}

void taStrRep::upcase() {//note: should only be called on cnt<=1
  for (uint n = 0; n < len; ++n) {
    if (islower(s[n])) s[n] = toupper(s[n]);
  }
}


//////////////////////////
//      taString          //
//////////////////////////

const taString taString::con_0("0");
const taString taString::con_1("1");
const taString taString::con_NULL("NULL");

taString triml(const taString& x) {
  int n = 0; // count of number to strip
  while (n < x.length()) {
    char c = x[n];
    if (!((c== ' ') || (c == '\t'))) break;
    ++n;
  }
  if (n == 0) return x;
  return x.right(x.length() - n);
}

taString trimr(const taString& x) {
  // note: c=0 when len=0
  char c = x.lastchar();
  if (!((c== ' ') || (c == '\t')))
    return x;

  taString rval(x);
  rval.makeUnique();
  do {
    rval.truncate(rval.length() - 1);
    c = rval.lastchar();
  } while ((c== ' ') || (c == '\t'));
  return rval;
}

taString trim(const taString& x) {
  return trimr(triml(x));
}

void taString::AppendCharToCppStringLiteral(taString& str, char c, bool char_mode) {
  //note: char_mode=true is for doing a single conversion for a char literal
  switch (c) { // do the specials first, since some printables are special
  case '\n': str += "\\n"; break;
  case '\t': str += "\\t"; break;
  case '\v': str += "\\v"; break;
  case '\b': str += "\\b"; break;
  case '\r': str += "\\r"; break;
  case '\f': str += "\\f"; break;
  case '\?': str += "\\?"; break;
  case '\\': str += "\\\\"; break;
  case '\'': if (char_mode) str += "\\'"; else str += "'"; break;
  case '\"': if (char_mode) str += "\""; else str += "\\\""; break;
  default:
    if ((c >= ' ') && ((uint)c < 127))
      str += c;
    else { // octal format with 3 digits is deterministically parsable in strings
      str += "\\";
      str += taString((int)c, "%o03.3"); // exactly 3 octal digits
    } break;
  }
}

const taString taString::CharToCppLiteral(char c) {
  taString rval(1, 6, '\''); // buffer
  AppendCharToCppStringLiteral(rval, c, true);
  rval += '\'';
  return rval;
}

const taString taString::StringToCppLiteral(const taString& str) {
  taString rval(1, (str.length() * 11 ) / 10, '"'); // 10% extra sized buffer
  int len = str.length(); // cache
  for (int i = 0; i < len; ++i)
    AppendCharToCppStringLiteral(rval, str.elem(i), false);
  rval += '"';
  return rval;
}


taString::taString(uint slen, uint sz, char fill) {
  if (sz == 0) sz = slen;
  if (fill == '\0') slen = 0;
  newRep(Snew(slen, sz));
  if (fill) {
    memset(mrep->s, fill, slen);
  }
}

taString::taString(bool b) {
  static taString t("true");
  static taString f("false");
  if (b) newRep(t.mrep);
  else   newRep(f.mrep);
}

taString::taString(int i,const char* format) {
  char buf[32];
  sprintf(buf, format,i);
  newRep(Salloc(buf, -1));
}

taString::taString(uint u, const char* format) {
  char buf[32];
  sprintf(buf, format,u);
  newRep(Salloc(buf, -1));
}

taString::taString(long i,const char* format) {
  char buf[32];
  sprintf(buf, format,i);
  newRep(Salloc(buf, -1));
}

taString::taString(ulong u, const char* format) {
  char buf[32];
  sprintf(buf, format,u);
  newRep(Salloc(buf, -1));
}

taString::taString(ta_int64_t i64) {
  char buf[64];
  const char* format;
#ifdef _MSC_VER // MSVC
//  switch (base){
//  case 8: format="%I64o";break;
//  case 16: format="%I64x";break;
//  default: format="%I64d";break;
//  }
  format = "%I64d";
  _snprintf(buf, 63, format, i64);
#else // Unix
// the 'll' size should work on Linux and Unix...
//  switch (base){
//  case 8: format="%llo";break;
//  case 16: format="%llx";break;
//  default: format="%lld";break;
//  }
  format = "%lld";
  sprintf(buf, format, i64);
#endif
  newRep(Salloc(buf, -1));
}

taString::taString(ta_uint64_t u64) {
  char buf[64];
  const char* format;
#ifdef _MSC_VER // formats may be ms specific
//  switch (base){
//  case 8: format="%I64o";break;
//  case 16: format="%I64x";break;
//  default: format="%I64u";break;
//  }
  format = "%I64u";
  _snprintf(buf, 63, format, u64);
#else
//the 'ull' should work on Linux and Unix
//  switch (base){
//  case 8: format="%llo";break;
//  case 16: format="%llx";break;
//  default: format="%llu";break;
//  }
  format = "%llu";
  sprintf(buf, format, u64);
#endif
  newRep(Salloc(buf, -1));
}

taString::taString(float f,const char* format) {
  char buf[32];
  sprintf(buf,format,f);
  newRep(Salloc(buf, -1));
}

taString::taString(double f,const char* format) {
  char buf[32];
  sprintf(buf,format,f);
  newRep(Salloc(buf, -1));
}

taString::taString(void* p) {
  char buf[32];
  sprintf(buf,"%p",p);
  newRep(Salloc(buf, -1));
}

// sub routine where repl.len < pat.len (won't need new alloc)
int taString::_gsub_lt(const char* pat, int pl, const char* r, int rl) {
  int nmatches = 0;
  const char* s = chars(); // source string
  int sl = length(); //note: kept current
  int si = 0; // current src index from whence to continue searching

  while (si < sl) {
    int pos = search(si, sl, pat, pl);
    if (pos < 0)
      break;
    else {
      ++nmatches;
    }
    memcpy((void*)&(s[pos]), r, rl);
    memmove((void*)&(s[pos + rl]), &(s[pos + pl]), sl - (pos + pl) );
    if (rl > 0) si = pos + rl;
    else ++si;
    sl -= (pl - rl);
  }
  mrep->len = sl;
  mrep->s[sl] = '\0';
  return nmatches;
}

// sub routine where repl.len == pat.len (fast: replace in-place)
int taString::_gsub_eq(const char* pat, int pl, const char* r) {
  int nmatches = 0;
  const char* s = chars(); // source string
  int sl = length();
  int si = 0; // current src index from whence to continue searching
  while (si < sl) {
    int pos = search(si, sl, pat, pl);
    if (pos < 0)
      break;
    else {
      ++nmatches;
    }
    memcpy((void*)&(s[pos]), r, pl);
    si = pos + pl;
  }
  return nmatches;
}

// sub routine where repl.len > pat.len
int taString::_gsub_gt(const char* pat, int pl, const char* r, int rl) {
  int nmatches = 0;

  int sl = length(); //note: kept current
  int si = 0; // current src index from whence to continue searching

  while (si < sl) {
    int pos = search(si, sl, pat, pl);
    if (pos < 0)
      break;
    else {
      ++nmatches;
    }
    // expand if we don't have enough room
    makeUnique(sl + (rl - pl));
    const char* s = chars();
    memmove((void*)&(s[pos + rl]), &(s[pos + pl]), sl - (pos + pl) );
    memcpy((void*)&(s[pos]), r, rl);
    si = pos + rl;
    sl += (rl - pl);
    // keep it current here, so nothing funky happens when resizing
    mrep->len = sl;
    mrep->s[sl] = '\0';
  }
  return nmatches;
}

int taString::_gsub(const char* pat, int pl, const char* r, int rl) {
  makeUnique(); // just to be safe
  if (mrep->len == 0)  return 0;
  if (pl < 0) pl = pat ? (int)strlen(pat) : 0;
  if (rl < 0) rl = r ? (int)strlen(r) : 0;
  if ((pl == 0) || (mrep->len < (uint)pl))
    return 0;
  if (pl == rl)     return _gsub_eq(pat, pl, r);
  else if (rl < pl) return _gsub_lt(pat, pl, r, rl);
  else              return _gsub_gt(pat, pl, r, rl);

}

int taString::assertLength() {
  uint slen = (uint)strlen(mrep->s);
  if (slen <= mrep->sz) {
    mrep->len = slen;
    mrep->s[slen] = '\0';
  }
  return slen;
}

taString& taString::cat(const taString& y) {
  if (mrep->canCat(y.length()))
    mrep->cat(y.chars(), y.length());
  else
    ::cat(*this, y, *this);
  return *this;
}

taString& taString::cat(const char* y, int slen) {
  if (slen < 0) slen = static_cast<int>(strlen(y));
  if (y && (slen > 0) ) {
    if (mrep->canCat(slen))
      mrep->cat(y, slen);
    else
      ::cat(*this, y, slen, *this);
  }
  return *this;
}

taString& taString::cat(const char* y) {
  uint slen = y ? (uint) strlen(y) : 0;
  if (slen) {
    if (mrep->canCat(slen))
      mrep->cat(y, slen);
    else
      ::cat(*this, y, slen, *this);
  }
  return *this;
}

taString& taString::cat(char y) {
  if (y) {
    if (mrep->canCat(1))
      mrep->cat(&y, 1);
    else
      ::cat(*this, &y, 1, *this);
  }
  return *this;
}

taString& taString::capitalize() {
  makeUnique();
  mrep->capitalize();
  return *this;
}

taString& taString::convert(int i,const char* format) {
  char buf[32];
  sprintf(buf, format,i);
  setRep(Salloc(buf, -1));
  return *this;
}

taString& taString::convert(long i,const char* format) {
  char buf[32];
  sprintf(buf,format,i);
  setRep(Salloc(buf, -1));
  return *this;
}

taString& taString::convert(float f,const char* format) {
  char buf[32];
  sprintf(buf,format,f);
  setRep(Salloc(buf, -1));
  return *this;
}

taString& taString::convert(double f,const char* format) {
  char buf[32];
  sprintf(buf,format,f);
  setRep(Salloc(buf, -1));
  return *this;
}

void taString::del(int pos, int len)
{
  if ((pos < 0) || (len <= 0) || ((pos + len) > length())) return;
  int nlen = length() - len;
  int first = pos + len;
  makeUnique();
  memmove(&(mrep->s[pos]), &(mrep->s[first]), length() - first);
  mrep->len = nlen;
  mrep->s[nlen] = '\0';
}

void taString::del(const char* t, int startpos)
{
  int tlen =  t ? (int)strlen(t) : 0;
  int p = search(startpos, length(), t, tlen);
  del(p, tlen);
}

void taString::del(const taString& y, int startpos)
{
  del(search(startpos, length(), y.chars(), y.length()), y.length());
}

void taString::del(char c, int startpos)
{
  del(search(startpos, length(), c), 1);
}

taString& taString::downcase() {
  makeUnique();
  mrep->downcase();
  return *this;
}

taString taString::elidedToFirstLine() const {
  int pos_nl = index('\n');
  if (pos_nl < 0) return *this;
  return before(pos_nl).cat("...");
}

taString taString::elidedTo(int width) const {
  //NOTE: by definition, -1 means no eliding, but if not, we still
  // need to remove line breaks, so can't just return clear text
  if (width < 0)
    return *this;
  // we always remove line breaks!
  taString src(this); // cheap, and usual result
  if (contains('\n')) {
    src.makeUnique();
    src.gsub('\n', " "); //note: len stays the same
  }

  int len = length(); // cache, we use a lot
  if (len <= width)
    return src;
  // if too small to use ellipses, return chars
  if (width <= 3)
    return src.before(width);

  STRING_BUF(rval, width);
  if (width <= 8) {
    // really short, just use first chars, and put ... at end
    rval.cat(src.before(width - 1)).cat("..."); // note: elipses don't take a full 3 chars on most displays..
  } else {
    // try finding a space after 1st half of string to elide on
    int pos = src.index(' ', width / 2);
    // if not, or too far after center, just elide in center
    if ((pos < 0) || (pos > ((width * 2) / 3))) {
      // just elide in center
      pos = width / 2;
    }
    rval.cat(src.before(pos)).cat(" ~ ").cat(src.right(width - pos - 1));
  }
  return rval;
}

void taString::error(const char* msg) const {
  cerr << "taString:" <<  msg << "\n";
}

#ifdef TA_USE_QT
#include <QRegExp>

bool taString::matches_wildcard(const taString& wild) const {
#if (QT_VERSION >= 0x040600)
  QRegExp re(wild, Qt::CaseSensitive, QRegExp::WildcardUnix);
#else
  QRegExp re(wild, Qt::CaseSensitive, QRegExp::Wildcard);
#endif

  return re.exactMatch(*this);
}

bool taString::matches_regexp(const taString& wild) const {
  QRegExp re(wild, Qt::CaseSensitive, QRegExp::RegExp);
  return re.exactMatch(*this);
}

#endif

bool taString::endsWith(char c) const {
  return matches(c, length() - 1);
}

bool taString::endsWith(const taString& y) const {
  return matches(y, length() - y.length());
}

bool taString::endsWith(const char* t) const {
  return endsWith(taString(t));
}

bool taString::startsWith(char c) const {
  return matches(c, 0);
}

bool taString::startsWith(const taString& y) const {
  return matches(y, 0);
}

bool taString::startsWith(const char* t) const {
  return startsWith(taString(t));
}


void taString::init(const char* s, int slen) {
  if (slen < 0)  slen = s ? (int)strlen(s) : 0;
  if (slen == 0) newRep(ADDR_NIL_STR_REP);
  else           newRep(Salloc(s, slen, slen));
}

bool taString::isInt() const {
  bool rval = false;
  for (int i = 0; i < (int)mrep->cnt; ++i) {
    char c = mrep->s[i];
    if (i == 0) {
      if ((c == '+') || (c == '-') || (c == ' ')) continue;
    }
    if (( c < '0') || (c > '9')) break;
    if (i == ((int)mrep->cnt - 1)) rval = true;
  }
  return rval;
}

void taString::makeUnique() {
  if (mrep->cnt > 1)
    setRep(Salloc(mrep->s, mrep->len));
}

void taString::makeUnique(uint min_new_sz) {
  if ((mrep->cnt > 1) || (mrep->sz < min_new_sz))
    setRep(Salloc(mrep->s, mrep->len, min_new_sz));
}

// the packthreshold is at least 15, since allocations typically are greater than requested
#define PACK_THRESH 15
void taString::pack() {
  if (mrep->sz > (mrep->len + PACK_THRESH))
    setRep(Salloc(mrep->s, mrep->len));
}

taString& taString::prepend(const taString& y) {
  if (mrep->canCat(y.length()))
    mrep->prepend(y.chars(), y.length());
  else
    setRep(Scat(y.chars(), y.length(), mrep->s, mrep->len));
  return *this;
}

taString& taString::prepend(const char* y) {
  uint slen = (y) ? (uint)strlen(y) : 0;
  if (mrep->canCat(slen))
    mrep->prepend(y, slen);
  else
    setRep(Scat(y, -1, mrep->s, mrep->len));
  return *this;
}

taString& taString::prepend(char y) {
  if (y) {
    if (mrep->canCat(1))
      mrep->prepend(&y, 1);
    else
      setRep(Scat(&y, 1, mrep->s, mrep->len));
  }
  return *this;
}

taString& taString::reverse() {
  makeUnique();
  mrep->reverse();
  return *this;
}

bool taString::LoadFromFile(const String& fname) {
  fstream strm;
  strm.open(fname, ios::in);
  if(!strm.good() || strm.bad()) {
    strm.close();
    return false;
  }
  bool rval = Load_str(strm);
  strm.close();
  return rval;
}

bool taString::SaveToFile(const String& fname) {
  fstream strm;
  strm.open(fname, ios::out);
  if(!strm.good() || strm.bad()) {
    strm.close();
    return false;
  }
  bool rval = Save_str(strm);
  strm.close();
  return rval;
}

bool taString::Load_str(std::istream& istrm) {
  truncate(0);
  const int buf_len = 1024;
  char buf[buf_len];
  bool done = false;
  while (!done) {
    istrm.read(buf, buf_len);
    int n_read = buf_len;
    if (istrm.bad()) break;
    if (istrm.eof()) {
      n_read = static_cast<int>(istrm.gcount());
      done = true;
    }
    // append num read
    cat(buf, n_read);
  }

  if (istrm.bad()) return false;
  else return true;
}

bool taString::Save_str(std::ostream& ostrm) {
  ostrm.write(mrep->s, mrep->len);
  if (ostrm.bad()) return false;
  else return true;
}

taString& taString::set(const char* s, int slen) {
  if (slen < 0)
    slen = s ? (int)strlen(s) : 0;
  if (slen == 0) setRep(ADDR_NIL_STR_REP);
  else           setRep(Salloc(s, slen, slen));
  return *this;
}

bool taString::toBool() const {
  if (mrep->len == 0) return false;
  char c = (*this)[0];
  return ((c == 't') || (c == '1') || (c == 'T'));
}

char taString::toChar() const {
  if (mrep->len == 1) return (*this)[0];
  else return '\0';;
}

void taString::truncate(uint new_len) {
  if (new_len >= mrep->len) return;
  makeUnique();
  mrep->len = new_len;
  mrep->s[new_len] = '\0';
}

taString& taString::upcase() {
  makeUnique();
  mrep->upcase();
  return *this;
}

/*
 * substring extraction
 */

// a helper needed by at, before, etc.

taString taString::_substr(int first, int len) const {
  if ((first < 0) || (len <= 0) || (first >= length()))
    return _nilString;
  else if ((first == 0) && (len == length())) // same as us, so don't make a new rep!
    return taString(*this);
  else {
    if ((first + len) > length())
      len = length() - first;
    return taString(&(mrep->s[first]), len);
  }
}

taString taString::at(int first, int len) const {
  return _substr(first, len);
}

taString taString::operator() (int first, int len) const {
  return _substr(first, len);
}

taString taString::before(int pos) const {
  return _substr(0, pos);
}

taString taString::through(int pos) const {
  return _substr(0, pos+1);
}

taString taString::after(int pos) const {
  return _substr(pos + 1, length() - (pos + 1));
}

taString taString::right(int len) const {
  return _substr(length() - len, len);
}

taString taString::from(int pos) const {
  return _substr(pos, length() - pos);
}

taString taString::at(const taString& y, int startpos) const {
  int first = search(startpos, length(), y.chars(), y.length());
  return _substr(first,  y.length());
}

taString taString::at(const char* t, int startpos) const {
  int tlen = t ? (int)strlen(t) : 0;
  int first = search(startpos, length(), t, tlen);
  return _substr(first, tlen);
}

taString taString::at(char c, int startpos) const {
  int first = search(startpos, length(), c);
  return _substr(first, 1);
}

taString taString::before(const taString& y, int startpos) const {
  int last = search(startpos, length(), y.chars(), y.length());
  return _substr(0, last);
}

taString taString::before(char c, int startpos) const {
  int last = search(startpos, length(), c);
  return _substr(0, last);
}

taString taString::before(const char* t, int startpos) const {
  int tlen = t ? (int)strlen(t) : 0;
  int last = search(startpos, length(), t, tlen);
  return _substr(0, last);
}

taString taString::through(const taString& y, int startpos) const {
  int last = search(startpos, length(), y.chars(), y.length());
  if (last >= 0) last += y.length();
  return _substr(0, last);
}

taString taString::through(char c, int startpos) const {
  int last = search(startpos, length(), c);
  if (last >= 0) last += 1;
  return _substr(0, last);
}

taString taString::through(const char* t, int startpos) const {
  int tlen = t ? (int)strlen(t) : 0;
  int last = search(startpos, length(), t, tlen);
  if (last >= 0) last += tlen;
  return _substr(0, last);
}

taString taString::after(const taString& y, int startpos) const {
  int first = search(startpos, length(), y.chars(), y.length());
  if (first >= 0) first += y.length();
  return _substr(first, length() - first);
}

taString taString::after(char c, int startpos) const {
  int first = search(startpos, length(), c);
  if (first >= 0) first += 1;
  return _substr(first, length() - first);
}

taString taString::after(const char* t, int startpos) const {
  int tlen = t ? (int)strlen(t) : 0;
  int first = search(startpos, length(), t, tlen);
  if (first >= 0) first += tlen;
  return _substr(first, length() - first);
}


taString taString::between(const taString& st_str, const taString& ed_str, int startpos) const {
  taString aft = after(st_str, startpos);
  return aft.before(ed_str);
}

taString taString::between(char st_c, char ed_c, int startpos) const {
  taString aft = after(st_c, startpos);
  return aft.before(ed_c);
}

taString taString::between(const char* st_str, const char* ed_str, int startpos) const {
  taString aft = after(st_str, startpos);
  return aft.before(ed_str);
}

taString taString::from(const taString& y, int startpos) const {
  int first = search(startpos, length(), y.chars(), y.length());
  return _substr(first, length() - first);
}

taString taString::from(char c, int startpos) const {
  int first = search(startpos, length(), c);
  return _substr(first, length() - first);
}

taString taString::from(const char* t, int startpos) const {
  int tlen = t ? (int)strlen(t) : 0;
  int first = search(startpos, length(), t, tlen);
  return _substr(first, length() - first);
}


/*
 * split/join
 */


int split(const taString& src, taString results[], int n, const taString& sep) {
  taString x = src;
  const char* s = x.chars();
  int sl = x.length();
  int i = 0;
  int pos = 0;
  while (i < n && pos < sl) {
    int p = x.search(pos, sl, sep.chars(), sep.length());
    if (p < 0)
      p = sl;
    results[i].setRep(Salloc(&(s[pos]), p - pos));
    i++;
    pos = p + sep.length();
  }
  return i;
}



taString join(const taString src[], int n, const taString& sep) {
  if (n <= 0) return _nilString;

  int xlen = (n - 1) * sep.length();
  int i;
  for (i = 0; i < n; ++i)
    xlen += src[i].length();

  taString x(Snew(xlen));

  int j =  0; // next starting position to copy
  for (i = 0; i < n - 1; ++i) {
    memcpy(&(x.mrep->s[j]), src[i].chars(), src[i].length());
    j += src[i].length();
    memcpy(&(x.mrep->s[j]), sep.chars(), sep.length());
    j += sep.length();
  }
  memcpy(&(x.mrep->s[j]), src[i].chars(), src[i].length());
  return x;
}

/*
 misc
*/

int compare(const taString& x, const taString& y) {
  return strcmp(x.chars(), y.chars());
}

int compare(const taString& x, const char* y) {
  if (y) return strcmp(x.chars(), y);
  else return strcmp(x.chars(), "");
}

int compare(const char* y, const taString& x) {
  if (y) return strcmp(y, x.chars());
  else return strcmp("", x.chars());
}

int compare(const taString& x, char y) {
  if (y) return strncmp(x.chars(), &y, 1);
  else return strcmp(x.chars(), "");
}

int compare(char y, const taString& x) {
  if (y) return strncmp(&y, x.chars(), 1);
  else return strcmp("", x.chars());
}


int fcompare(const taString& x, const taString& y) {
  return strcasecmp(x.chars(), y.chars());
}


taString replicate(char c, int n) {
  taString w(n, n, c);
  return w;
}

taString replicate(const taString& y, int n) {
  int len = y.length();
  if ((len == 0) || (n == 0)) return _nilString;
  if (len == 1) return replicate(y[0], n); //more efficient, and the most likely (ex. " ", etc.)

  int xlen = n * len;
  taString w(Snew(xlen));
  char* p = w.mrep->s;
  const char* yp = y.chars();
  while (n-- > 0) {
    memcpy(p, yp, len);
    p += len;
  }
  return w;
}

taString replicate(const char* y, int n) {
  int len = (y) ? (int)strlen(y) : 0;
  if ((len == 0) || (n == 0)) return _nilString;
  if (len == 1) return replicate(y[0], n); //more efficient, and the most likely (ex. " ", etc.)

  int xlen = n * len;
  taString w(Snew(xlen));
  char* p = w.mrep->s;
  while (n-- > 0) {
    memcpy(p, y, len);
    p += len;
  }
  return w;
}

taString common_prefix(const taString& x, const taString& y, int startpos) {
  const char* xchars = x.chars();
  const char* ychars = y.chars();
  const char* xs = &(xchars[startpos]);
  const char* ss = xs;
  const char* topx = &(xchars[x.length()]);
  const char* ys = &(ychars[startpos]);
  const char* topy = &(ychars[y.length()]);
  int l;
  for (l=0; xs < topx && ys < topy && *xs++ == *ys++; ++l);
  if (l == 0) return _nilString;

  taString r(Salloc(ss, l, l));
  return r;
}

taString common_suffix(const taString& x, const taString& y, int startpos) {
  const char* xchars = x.chars();
  const char* ychars = y.chars();
  const char* xs = &(xchars[x.length() + startpos]);
  const char* botx = xchars;
  const char* ys = &(ychars[y.length() + startpos]);
  const char* boty = ychars;
  int l;
  for (l= 0; xs >= botx && ys >= boty && *xs == *ys ; --xs, --ys, ++l);
  if (l == 0) return _nilString;

  taString r(Salloc(++xs, l, l));
  return r;
}

taString& taString::xml_esc() {
  makeUnique();
  gsub("&", "&amp;");
  gsub("<", "&lt;");
  gsub(">", "&gt;");
  gsub("'", "&apos;");
  gsub("\"", "&quot;");
  return *this;
}

taString& taString::xml_unesc() {
  makeUnique();
  gsub("&amp;" , "&");
  gsub("&lt;"  , "<");
  gsub("&gt;"  , ">");
  gsub("&apos;", "'");
  gsub("&quot;", "\"");
  return *this;
}

taString& taString::quote_esc() {
  makeUnique();
  gsub("\\", "\\\\");
  gsub("\"", "\\\"");
  gsub("\n", "\\n");
  gsub("\r", "\\r");
  gsub("\t", "\\t");
  return *this;
}

taString& taString::quote_unesc() {
  makeUnique();
  gsub("\\\\", "\\");
  gsub("\\\"", "\"");
  gsub("\\n", "\n");
  gsub("\\r", "\r");
  gsub("\\t", "\t");
  return *this;
}

// IO
#define ISTR_RESIZE_QUANTA 80
TA_API istream& operator>>(istream& s, taString& x) {
  //  if (!s.ipfx(0) || (!(s.flags() & ios::skipws) && !ws(s)))
  if ((!(s.flags() & ios::skipws) && !ws(s)))
  {
    s.clear(ios::failbit|s.rdstate()); // Redundant if using GNU iostreams.
    return s;
  }
  int ch;
  uint i = 0;
  x.makeUnique(x.mrep->len + ISTR_RESIZE_QUANTA);
  register streambuf *sb = s.rdbuf();
  while ((ch = sb->sbumpc()) != EOF)
  {
    if (isspace(ch))
      break;
    if (i >= x.mrep->sz - 1)
      x.makeUnique(x.mrep->sz + ISTR_RESIZE_QUANTA);
    x.mrep->s[i++] = ch;
  }
  x.mrep->s[i] = '\0';
  x.mrep->len = i;
  if (i == 0) s.clear(ios::failbit|s.rdstate());
  if (ch == EOF) s.clear(ios::eofbit|s.rdstate());
  return s;
}

TA_API std::ostream& operator<<(std::ostream& s, const taString& x) {
  s << x.chars();
  return s;
}

TA_API int readline(istream& s, taString& x, char terminator, int discard) {
//   if (!s.ipfx(0))
//     return 0;
  int ch;
  uint i = 0;
  x.makeUnique(x.mrep->len + ISTR_RESIZE_QUANTA);
  register streambuf *sb = s.rdbuf();
  while ((ch = sb->sbumpc()) != EOF)
  {
    if (ch != terminator || !discard)
    {
      if (i >= x.mrep->sz - 1) {
        x.mrep->len = i; // need to set length to this point, so resize works/copies
        x.makeUnique(x.mrep->sz + ISTR_RESIZE_QUANTA);
      }
      x.mrep->s[i++] = ch;
    }
    if (ch == terminator)
      break;
  }
  x.mrep->s[i] = 0;
  x.mrep->len = i;
  if (ch == EOF) s.clear(ios::eofbit|s.rdstate());
  return i;
}

TA_API int readline_auto(istream& strm, taString& x) {
  int c;
  uint i = 0;
  x.makeUnique(x.mrep->len + ISTR_RESIZE_QUANTA);

  // read the characters, stopping at an eof or eol char
  while (((c = strm.peek()) != EOF) && !((c == '\n') || (c == '\r'))) {
    if (i >= x.mrep->sz - 1) {
      x.mrep->len = i; // need to set length to this point, so resize works/copies
      x.makeUnique(x.mrep->sz + ISTR_RESIZE_QUANTA);
    }
    x.mrep->s[i++] = (char)c;
    strm.get();
  }

  // discard the eol char(s)
  strm.get();
  // for Windows files, grab the extra cr
  if ((c == '\r') && ((c = strm.peek()) == '\n'))
    strm.get();
  // terminate
  x.mrep->s[i] = 0;
  x.mrep->len = i;
  return i;
}


// from John.Willis@FAS.RI.CMU.EDU

int taString::freq(const taString& y) const
{
  int found = 0;
  for (int i = 0; i < length(); i++)
    if (match(i,length(),0,y.chars(),y.length()) >= 0) found++;
  return(found);
}

int taString::freq(const char* t) const
{
  int found = 0;
  for (int i = 0; i < length(); i++)
    if (match(i,length(),0,t) >= 0) found++;
  return(found);
}

int taString::freq(char c) const
{
  int found = 0;
  for (int i = 0; i < length(); i++)
    if (match(i,length(),0,&c,1) >= 0) found++;
  return(found);
}

taString taString::wrap(int width) const {
  if (length() <= width)
    return *this;

  int i = 0;
  int k, counter;
  while(i < length())
  {
    for (counter = 0; counter < width; counter++)
    {
      // check if end of string reached
      if (i == length())
      {
        return *this;
      }
      // if we find a newline reset the counter
      if (mrep->s[i] == '\n')
      {
        counter = 1;
      }
      i++;
    }
    // check for whitespace
    if (isspace(mrep->s[i]))
    {
      mrep->s[i] = '\n';
      i++;
    }
    else
    {
      // check for nearest whitespace back in string
      for (k = i; k > 0; k--)
      {
        if (isspace(mrep->s[k]))
        {
          mrep->s[k] = '\n';
          // set string index back to character after this one
          i = k + 1;
          break;
        }
      }
    }
  }
  return *this;
}

