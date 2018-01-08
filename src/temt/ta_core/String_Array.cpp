// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#include "String_Array.h"

#include <QStringList>
#include <int_Array>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(String_Array);
SMARTREF_OF_CPP(String_Array)

const String String_Array::blank = "";

#ifdef TA_USE_QT
void String_Array::ToQStringList(QStringList& sl) {
  sl.clear();
  for (int i = 0; i < size; ++i) {
    sl.append(FastEl(i).toQString());
  }
}
#endif // TA_USE_QT

String String_Array::ToDelimString(const String& delim) {
  String rval;
  for (int i = 0; i < size; ++i) {
    rval += FastEl(i);
    if(i < size-1) rval += delim;
  }
  return rval;
}

void String_Array::FmDelimString(const String& str, const String& delim, bool reset_first) {
  if(reset_first) Reset();
  if (delim == "") {
    for (int i = 0; i < str.length(); i++) {
      Add(str.at(i,1));
    }
  }
  else {
    String remainder = str;
    while(remainder.nonempty()) {
      if(remainder.contains(delim)) {
  	Add(remainder.before(delim));
  	remainder = remainder.after(delim);
      }
      else {
  	Add(remainder);
  	remainder = _nilString;
      }
    }
  }
}

void String_Array::Split(const String& str, const String& delim) {
  FmDelimString(str, delim);
}

int String_Array::FindContains(const String& str) {
  for (int i = 0; i < size; ++i) {
    if(FastEl(i).contains(str)) return i;
  }
  return -1;
}

int String_Array::FindContains_ci(const String& str) {
  for (int i = 0; i < size; ++i) {
    if(FastEl(i).contains_ci(str)) return i;
  }
  return -1;
}

int String_Array::FindStartsWith(const String& str) {
  for (int i = 0; i < size; ++i) {
    if(FastEl(i).startsWith(str)) return i;
  }
  return -1;
}

int String_Array::FindEndsWith(const String& str) {
  for (int i = 0; i < size; ++i) {
    if(FastEl(i).endsWith(str)) return i;
  }
  return -1;
}

String String_Array::MostFrequent() {
  if (size == 0) return _nilString;
  if (size == 1) return FastEl(0);
  
  String winning_string;  // most frequent string
  int_Array     counts;
  String        last_string;
  int           index = -1;
  
  Sort();
  counts.SetSize(this->size); // can't be bigger than the string array
  if(FastEl(0) == last_string) {  // first element could be empty!
    index++;
  }
  for (int i = 0; i < size; ++i) {
    if(FastEl(i) != last_string) {
      index++;
      last_string = FastEl(i);
    }
    counts[index]++;
  }

  int maxv = 0;
  int maxi = 0;
  int sum = 0;
  for (int i = 0; i < counts.size; ++i) {
    if (counts[i] == 0) {
      break;
    }
    if (counts[i] > maxv) {
      maxv = counts[i];
      maxi = i;
      winning_string = FastEl(sum);  // most frequent string
    }
    sum += counts[i];
  }
  return winning_string;
}
