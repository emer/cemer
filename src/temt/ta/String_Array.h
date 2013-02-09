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

#ifndef String_Array_h
#define String_Array_h 1

// parent includes:
#include <taString>
#include <taArray>

// member includes:

// declare all other types mentioned but not required to include:
class QStringList; //


taTypeDef_Of(String_Array);

class TA_API String_Array : public taArray<String> {
  // #NO_UPDATE_AFTER #NO_TOKENS array of Strings
INHERITED(taArray<String>)
public:
  STATIC_CONST String blank; // #HIDDEN #READ_ONLY
#ifdef TA_USE_QT
  void                  ToQStringList(QStringList& sl); // #IGNORE fills a QStringList
#endif

  String        ToDelimString(const String& delim = " ");
  // generates a string of all the items on the list, using given delimiter between items
  void          FmDelimString(const String& str, const String& delim = " ", bool reset_first = true);
  // add strings to this array by parsing given string using given delimiter separating strings -- reset first = reset this array before adding (else append)

  TA_BASEFUNS(String_Array);
  TA_ARRAY_FUNS(String_Array, String)
protected:
  override Variant      El_GetVar_(const void* itm) const
    { return (Variant)(*(String*)itm); }
  int           El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((String*)a) > *((String*)b)) rval=1; else if(*((String*)a) == *((String*)b)) rval=0; return rval; }
  bool          El_Equal_(const void* a, const void* b) const
    { return (*((String*)a) == *((String*)b)); }
  String        El_GetStr_(const void* it) const { return (*((String*)it)); }
  void          El_SetFmStr_(void* it, const String& val)
  {*((String*)it) = val; }
private:
  void  Copy_(const String_Array&){}
  void Initialize()     { };
  void Destroy()        { };
};

TA_ARRAY_OPS(String_Array)

#endif // String_Array_h
