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

/*
  String class implementation
 */

#include "ta_string.h"

#if (defined(WINDOWS) && (!defined(CYGWIN)))
#include <windows.h>
// the Windows string.h doesn't define strcasecmp, so we use the Win32 equivalent
#define strcasecmp(s1,s2) lstrcmp(s1, s2)
#endif

#include <ctype.h>
#include <limits.h>
#include <stdio.h>

using namespace std;

//////////////////////////
//	misc funcs	//
//////////////////////////


StrRep  _nilStrRep = { 0, 1, 1, { 0 } }; // nil StrReps point here; ref=1 so it can't be released

// capitalize a string, in-place
void _capitalize(char* p, int n) {
  char* e = &(p[n]);
  for (; p < e; ++p) {
    int at_word;
    if ((at_word = islower(*p)))
      *p = toupper(*p);
    else
      at_word = isupper(*p) || isdigit(*p);

    if (at_word) {
      while (++p < e) {
        if (isupper(*p))
          *p = tolower(*p);
	/* A '\'' does not break a word, so that "Nathan's" stays
	   "Nathan's" rather than turning into "Nathan'S". */
        else if (!islower(*p) && !isdigit(*p) && (*p != '\''))
          break;
      }
    }
  }
}

int _search(const char* s, int start, int sl, char c) {
  if (sl > 0) {
    if (start >= 0) {
      const char* a = &(s[start]);
      const char* lasta = &(s[sl]);
      while (a < lasta) if (*a++ == c) return --a - s;
    } else {
      const char* a = &(s[sl + start + 1]);
      while (--a >= s) if (*a == c) return a - s;
    }
  }
  return -1;
}

int _search(const char* s, int start, int sl, const char* t, int tl) {
  if (tl < 0) tl =  t ? strlen(t) : 0;
  if (sl > 0 && tl > 0) {
    if (start >= 0) {
      const char* lasts = &(s[sl - tl]);
      const char* lastt = &(t[tl]);
      const char* p = &(s[start]);

      while (p <= lasts) {
        const char* x = p++;
        const char* y = t;
        while (*x++ == *y++) if (y >= lastt) return --p - s;
      }
    } else {
      const char* firsts = &(s[tl - 1]);
      const char* lastt =  &(t[tl - 1]);
      const char* p = &(s[sl + start + 1]);

      while (--p >= firsts) {
        const char* x = p;
        const char* y = lastt;
        while (*x-- == *y--) if (y < t) return ++x - s;
      }
    }
  }
  return -1;
}

// string compare: first argument is known to be non-null

inline static int ncmp(const char* a, int al, const char* b, int bl)
{
  int n = (al <= bl)? al : bl;
  int diff;
  while (n-- > 0) if ((diff = *a++ - *b++) != 0) return diff;
  return al - bl;
}


//////////////////////////
//	SRep		//
//////////////////////////

#define MAXStrRep_SIZE   ((1 << (sizeof(short) * CHAR_BIT - 1)) - 1)

// note: compilers may report the size of StrRep to be an even multiple of 4
#define SIZE_OF_STRREP 13

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

// create an empty buffer -- called by routines that then fill the chars (ex. reverse, upcase, etc.)
StrRep* Snew(int slen, uint cap) {
  if (cap == 0) cap = slen;
  uint allocsize = tweak_alloc(SIZE_OF_STRREP + cap); // we tweak the alloc size to optimize -- may be larger
  cap = allocsize - SIZE_OF_STRREP; // in case we asked for more memory
#ifdef TA_PROFILE
  ++Cstring_prof::inst_cnt;
  Cstring_prof::tot_size += allocsize;
#endif
  StrRep* rval = (StrRep*)malloc(allocsize);
  if (!rval) {
    cerr << "Salloc: could not allocate memory for new string\n";
    return NULL; // note: best that we crash, since this is a serious error condition
  }
  rval->sz = cap;
  rval->len = slen;
  rval->cnt = 0;
  rval->s[slen] = '\0';
  return rval;
}

// allocate a new StrRep by copying from a given string (can be NULL)
StrRep* Salloc(const char* s, int slen, uint cap) {
  if (slen < 0) slen = s ? strlen(s) : 0;
  StrRep* rval = Snew(slen, cap); //note: alloc failure returns NULL
  if (slen && (rval->sz >= (uint)slen)) memcpy(rval->s, s, slen);
  return rval;
}

// allocate & concatenate
StrRep* Scat(StrRep* srep, const char* s, int slen) {
  return Scat(srep->s, srep->len, s, slen);
}

StrRep*	Scat(const char* s1, int slen1, const char* s2, int slen2) { // slen can be -1
  if (slen1 < 0) slen1 = s1 ? strlen(s1) : 0;
  if (slen2 < 0) slen2 = s2 ? strlen(s2) : 0;
  uint newlen = slen1 + slen2;
  if (newlen == 0) return &_nilStrRep;

  StrRep* rval = Salloc(s1, slen1, newlen); //note: alloc failure returns _nilStrRep
  if (slen2) {
    memcpy(&(rval->s[slen1]), s2, slen2);
  }
  rval->len = newlen;
  rval->s[newlen] = '\0';
  return rval;
}

/* nu
// reallocate: NOTE: this is intended for inplace routines, such as gsub
// THE REP COUNT SHOULD BE ZERO --

static StrRep* Sresize(StrRep* old, uint new_sz) {
  if (old == &_nilStrRep) old = 0; // precaution
  if (old && (old->cnt > 0)) {
    cerr << "Sresize: old rep should have cnt==0; was: " << old->cnt << "\n";
  }
  StrRep* rep;
  if (old == 0)
    rep = Snew(0, new_sz);
  else if (new_sz > old->sz) {
    uint allocsize = tweak_alloc(SIZE_OF_STRREP + new_sz);
    new_sz = allocsize - SIZE_OF_STRREP; // in case we asked for more memory
#ifdef TA_PROFILE
  ++Cstring_prof::inst_cnt;
  Cstring_prof::tot_size += allocsize;
#endif
    rep = (StrRep*)realloc(old, allocsize);
    rep->sz = new_sz;
  } else
    rep = old;
  return rep;
} */

StrRep* Sreverse(const StrRep* src)
{
  StrRep* dest = Snew(src->len);
  char* a = dest->s;
  char* b = (char *)&(src->s[src->len]); // start at one past end; harmless if len=0 (safe deconstify)
  for (uint n = 0; n < src->len; ++n) {
      a[n] = *(--b);
  }
  return dest;
}

void StrRep::capitalize() {//note: should only be called on cnt==1
  _capitalize(s, len);
}

void StrRep::cat(const char* str, uint slen) { // note: slen must be set, and canCat must have been true
  if (slen == 0) return;
  memcpy(&(s[len]), str, slen);
  len += slen;
  s[len] = '\0';
}

void StrRep::downcase() { //note: should only be called on cnt==1
  for (uint n = 0; n < len; ++n) {
    if (isupper(s[n])) s[n] = tolower(s[n]);
  }
}

int StrRep::match(int start, int sl, int exact, const char* t, int tl) const {
  if (tl < 0) tl =  t ? strlen(t) : 0;

  if (start < 0) {
    start = sl + start - tl + 1;
    if (start < 0 || (exact && start != 0))
      return -1;
  } else if (exact && sl - start != tl)
    return -1;

  if (sl == 0 || tl == 0 || sl - start < tl || start >= sl)
    return -1;

  int n = tl;
  const char* str = &(s[start]);
  while (--n >= 0) if (*str++ != *t++) return -1;
  return tl;
}
void StrRep::prepend(const char* str, uint slen) { // note: slen must be set, and canCat must have been true
  if (slen == 0) return;
  memmove(&(s[slen]), s, len);
  memcpy(s, str, slen);
  len += slen;
  s[len] = '\0';
}

void StrRep::reverse() {//note: should only be called on cnt<=1
  char* a = s;
  char* b = &(s[len - 1]);
  while (a < b) {
    char t = *a;
    *a++ = *b;
    *b-- = t;
  }
}

int StrRep::search(int start, int sl, char c) const {
  return _search(s, start, sl, c);
}

int StrRep::search(int start, int sl, const char* t, int tl) const {
  return _search(s, start, sl, t, tl);
}

void StrRep::upcase() {//note: should only be called on cnt<=1
  for (uint n = 0; n < len; ++n) {
    if (islower(s[n])) s[n] = toupper(s[n]);
  }
}


//////////////////////////
//	String		//
//////////////////////////

String::String(uint slen, uint sz, char fill) {
  if (sz == 0) sz = slen;
  if (fill == '\0') slen = 0;
  newRep(Snew(slen, sz));
  if (fill) {
    memset(mrep->s, fill, slen);
  }
}

String::String(int i,const char* format) {
  char buf[32];
  sprintf(buf, format,i);
  newRep(Salloc(buf, -1));
}

String::String(long i,const char* format) {
  char buf[32];
  sprintf(buf,format,i);
  newRep(Salloc(buf, -1));
}

String::String(unsigned long u, const char* format) {
  char buf[32];
  sprintf(buf,format,u);
  newRep(Salloc(buf, -1));
}

String::String(float f,const char* format) {
  char buf[32];
  sprintf(buf,format,f);
  newRep(Salloc(buf, -1));
}

String::String(double f,const char* format) {
  char buf[32];
  sprintf(buf,format,f);
  newRep(Salloc(buf, -1));
}

String::String(void* f,const char* format) {
  char buf[32];
  sprintf(buf,format,(long int)f);
  newRep(Salloc(buf, -1));
}

// sub routine where repl.len < pat.len (won't need new alloc)
int String::_gsub_lt(const char* pat, int pl, const char* r, int rl) {
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
int String::_gsub_eq(const char* pat, int pl, const char* r) {
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
int String::_gsub_gt(const char* pat, int pl, const char* r, int rl) {
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

int String::_gsub(const char* pat, int pl, const char* r, int rl) {
  makeUnique(); // just to be safe
  if (mrep->len == 0)  return 0;
  if (pl < 0) pl = pat ? strlen(pat) : 0;
  if (rl < 0) rl = r ? strlen(r) : 0;
  if ((pl == 0) || (mrep->len < (uint)pl))
    return 0;
  if (pl == rl)     return _gsub_eq(pat, pl, r);
  else if (rl < pl) return _gsub_lt(pat, pl, r, rl);
  else              return _gsub_gt(pat, pl, r, rl);

}

int String::assertLength() {
  uint slen = strlen(mrep->s);
  if (slen <= mrep->sz) {
    mrep->len = slen;
    mrep->s[slen] = '\0';
  }
  return slen;
}

String& String::cat(const String& y) {
  if (mrep->canCat(y.length()))
    mrep->cat(y.chars(), y.length());
  else
    ::cat(*this, y, *this);
  return *this;
}

String& String::cat(const char* y) {
  uint slen = (y) ? strlen(y) : 0;
  if (mrep->canCat(slen))
    mrep->cat(y, slen);
  else
    ::cat(*this, y, slen, *this);
  return *this;
}

String& String::cat(char y) {
  if (y) {
    if (mrep->canCat(1))
      mrep->cat(&y, 1);
    else
      ::cat(*this, &y, 1, *this);
  }
  return *this;
}

String& String::capitalize() {
  makeUnique();
  mrep->capitalize();
  return *this;
}

String& String::convert(int i,const char* format) {
  char buf[32];
  sprintf(buf, format,i);
  setRep(Salloc(buf, -1));
  return *this;
}

String& String::convert(long i,const char* format) {
  char buf[32];
  sprintf(buf,format,i);
  setRep(Salloc(buf, -1));
  return *this;
}

String& String::convert(float f,const char* format) {
  char buf[32];
  sprintf(buf,format,f);
  setRep(Salloc(buf, -1));
  return *this;
}

String& String::convert(double f,const char* format) {
  char buf[32];
  sprintf(buf,format,f);
  setRep(Salloc(buf, -1));
  return *this;
}

void String::del(int pos, int len)
{
  if ((pos < 0) || (len <= 0) || (unsigned)(pos + len) > length()) return;
  int nlen = length() - len;
  int first = pos + len;
  makeUnique();
  memmove(&(mrep->s[pos]), &(mrep->s[first]), length() - first);
  mrep->len = nlen;
  mrep->s[nlen] = '\0';
}

void String::del(const char* t, int startpos)
{
  int tlen =  t ? strlen(t) : 0;
  int p = search(startpos, length(), t, tlen);
  del(p, tlen);
}

void String::del(const String& y, int startpos)
{
  del(search(startpos, length(), y.chars(), y.length()), y.length());
}

void String::del(char c, int startpos)
{
  del(search(startpos, length(), c), 1);
}

String& String::downcase() {
  makeUnique();
  mrep->downcase();
  return *this;
}

void String::error(const char* msg) const {
  cerr << "String:" <<  msg << "\n";
}

void String::init(const char* s, int slen) {
  if (slen < 0)  slen = s ? strlen(s) : 0;
  if (slen == 0) newRep(&_nilStrRep);
  else           newRep(Salloc(s, slen, slen));
}

bool String::isInt() const {
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

void String::makeUnique() {
  if (mrep->cnt > 1)
    setRep(Salloc(mrep->s, mrep->len));
}

void String::makeUnique(uint min_new_sz) {
  if ((mrep->cnt > 1) || (mrep->sz < min_new_sz))
    setRep(Salloc(mrep->s, mrep->len, min_new_sz));
}

// the packthreshold is at least 15, since allocations typically are greater than requested
#define PACK_THRESH 15
void String::pack() {
  if (mrep->sz > (mrep->len + PACK_THRESH))
    setRep(Salloc(mrep->s, mrep->len));
}

String& String::prepend(const String& y) {
  if (mrep->canCat(y.length()))
    mrep->prepend(y.chars(), y.length());
  else
    setRep(Scat(y.chars(), y.length(), mrep->s, mrep->len));
  return *this;
}

String& String::prepend(const char* y) {
  uint slen = (y) ? strlen(y) : 0;
  if (mrep->canCat(slen))
    mrep->prepend(y, slen);
  else
    setRep(Scat(y, -1, mrep->s, mrep->len));
  return *this;
}

String& String::prepend(char y) {
  if (y) {
    if (mrep->canCat(1))
      mrep->prepend(&y, 1);
    else
      setRep(Scat(&y, 1, mrep->s, mrep->len));
  }
  return *this;
}

String& String::reverse() {
  makeUnique();
  mrep->reverse();
  return *this;
}

String& String::set(const char* s, int slen) {
  if (slen < 0)
    slen = s ? strlen(s) : 0;
  if (slen == 0) setRep(&_nilStrRep);
  else           setRep(Salloc(s, slen, slen));
  return *this;
}

void String::truncate(uint new_len) {
  if (new_len >= mrep->len) return;
  makeUnique();
  mrep->len = new_len;
  mrep->s[new_len] = '\0';
}

String& String::upcase() {
  makeUnique();
  mrep->upcase();
  return *this;
}



/*
 * substring extraction
 */


// a helper needed by at, before, etc.

String String::_substr(int first, int l) const {
  if ((first < 0) || (l == 0) || ((unsigned)(first + l) > length()))
    return _nilString;
  else if ((first == 0) && (l == (int)length())) // same as us, so don't make a new rep!
    return String(*this);
  else
    return String(&(mrep->s[first]), l);
}

String String::at(int first, int len) const {
  return _substr(first, len);
}

String String::operator() (int first, int len) const {
  return _substr(first, len);
}

String String::before(int pos) const {
  return _substr(0, pos);
}

String String::through(int pos) const {
  return _substr(0, pos+1);
}

String String::after(int pos) const {
  return _substr(pos + 1, length() - (pos + 1));
}

String String::from(int pos) const {
  return _substr(pos, length() - pos);
}

String String::at(const String& y, int startpos) const {
  int first = search(startpos, length(), y.chars(), y.length());
  return _substr(first,  y.length());
}

String String::at(const char* t, int startpos) const {
  int tlen = t ? strlen(t) : 0;
  int first = search(startpos, length(), t, tlen);
  return _substr(first, tlen);
}

String String::at(char c, int startpos) const {
  int first = search(startpos, length(), c);
  return _substr(first, 1);
}

String String::before(const String& y, int startpos) const {
  int last = search(startpos, length(), y.chars(), y.length());
  return _substr(0, last);
}

String String::before(char c, int startpos) const {
  int last = search(startpos, length(), c);
  return _substr(0, last);
}

String String::before(const char* t, int startpos) const {
  int tlen = t ? strlen(t) : 0;
  int last = search(startpos, length(), t, tlen);
  return _substr(0, last);
}

String String::through(const String& y, int startpos) const {
  int last = search(startpos, length(), y.chars(), y.length());
  if (last >= 0) last += y.length();
  return _substr(0, last);
}

String String::through(char c, int startpos) const {
  int last = search(startpos, length(), c);
  if (last >= 0) last += 1;
  return _substr(0, last);
}

String String::through(const char* t, int startpos) const {
  int tlen = t ? strlen(t) : 0;
  int last = search(startpos, length(), t, tlen);
  if (last >= 0) last += tlen;
  return _substr(0, last);
}

String String::after(const String& y, int startpos) const {
  int first = search(startpos, length(), y.chars(), y.length());
  if (first >= 0) first += y.length();
  return _substr(first, length() - first);
}

String String::after(char c, int startpos) const {
  int first = search(startpos, length(), c);
  if (first >= 0) first += 1;
  return _substr(first, length() - first);
}

String String::after(const char* t, int startpos) const {
  int tlen = t ? strlen(t) : 0;
  int first = search(startpos, length(), t, tlen);
  if (first >= 0) first += tlen;
  return _substr(first, length() - first);
}

String String::from(const String& y, int startpos) const {
  int first = search(startpos, length(), y.chars(), y.length());
  return _substr(first, length() - first);
}

String String::from(char c, int startpos) const {
  int first = search(startpos, length(), c);
  return _substr(first, length() - first);
}

String String::from(const char* t, int startpos) const {
  int tlen = t ? strlen(t) : 0;
  int first = search(startpos, length(), t, tlen);
  return _substr(first, length() - first);
}


/*
 * split/join
 */


int split(const String& src, String results[], int n, const String& sep) {
  String x = src;
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



String join(const String src[], int n, const String& sep) {
  if (n <= 0) return _nilString;

  int xlen = (n - 1) * sep.length();
  int i;
  for (i = 0; i < n; ++i)
    xlen += src[i].length();

  String x(Snew(xlen));

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

int compare(const String& x, const String& y) {
  return strcmp(x.chars(), y.chars());
}

int compare(const String& x, const char* y) {
  if (y) return strcmp(x.chars(), y);
  else return strcmp(x.chars(), "");
}

int compare(const char* y, const String& x) {
  if (y) return strcmp(y, x.chars());
  else return strcmp("", x.chars());
}

int compare(const String& x, char y) {
  if (y) return strncmp(x.chars(), &y, 1);
  else return strcmp(x.chars(), "");
}

int compare(char y, const String& x) {
  if (y) return strncmp(&y, x.chars(), 1);
  else return strcmp("", x.chars());
}


int fcompare(const String& x, const String& y) {
  return strcasecmp(x.chars(), y.chars());
}


String replicate(char c, int n) {
  String w(n, n, c);
  return w;
}

String replicate(const String& y, int n) {
  int len = y.length();
  if ((len == 0) || (n == 0)) return _nilString;
  if (len == 1) return replicate(y[0], n); //more efficient, and the most likely (ex. " ", etc.)

  int xlen = n * len;
  String w(Snew(xlen));
  char* p = w.mrep->s;
  const char* yp = y.chars();
  while (n-- > 0) {
    memcpy(p, yp, len);
    p += len;
  }
  return w;
}

String replicate(const char* y, int n) {
  int len = (y) ? (int)strlen(y) : 0;
  if ((len == 0) || (n == 0)) return _nilString;
  if (len == 1) return replicate(y[0], n); //more efficient, and the most likely (ex. " ", etc.)

  int xlen = n * len;
  String w(Snew(xlen));
  char* p = w.mrep->s;
  while (n-- > 0) {
    memcpy(p, y, len);
    p += len;
  }
  return w;
}

String common_prefix(const String& x, const String& y, int startpos) {
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

  String r(Salloc(ss, l, l));
  return r;
}

String common_suffix(const String& x, const String& y, int startpos) {
  const char* xchars = x.chars();
  const char* ychars = y.chars();
  const char* xs = &(xchars[x.length() + startpos]);
  const char* botx = xchars;
  const char* ys = &(ychars[y.length() + startpos]);
  const char* boty = ychars;
  int l;
  for (l= 0; xs >= botx && ys >= boty && *xs == *ys ; --xs, --ys, ++l);
  if (l == 0) return _nilString;

  String r(Salloc(++xs, l, l));
  return r;
}

// IO
#define ISTR_RESIZE_QUANTA 80
istream& operator>>(istream& s, String& x) {
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

int readline(istream& s, String& x, char terminator, int discard) {
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
      if (i >= x.mrep->sz - 1)
        x.makeUnique(x.mrep->sz + ISTR_RESIZE_QUANTA);
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


// from John.Willis@FAS.RI.CMU.EDU

int String::freq(const String& y) const
{
  int found = 0;
  for (unsigned int i = 0; i < length(); i++)
    if (match(i,length(),0,y.chars(),y.length()) >= 0) found++;
  return(found);
}

int String::freq(const char* t) const
{
  int found = 0;
  for (unsigned int i = 0; i < length(); i++)
    if (match(i,length(),0,t) >= 0) found++;
  return(found);
}

int String::freq(char c) const
{
  int found = 0;
  for (unsigned int i = 0; i < length(); i++)
    if (match(i,length(),0,&c,1) >= 0) found++;
  return(found);
}

