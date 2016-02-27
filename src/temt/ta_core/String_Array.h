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
  static const String blank; // #HIDDEN #READ_ONLY
#ifdef TA_USE_QT
  void                  ToQStringList(QStringList& sl); // #IGNORE fills a QStringList
#endif

  String        ToDelimString(const String& delim = " ");
  // generates a string of all the items on the list, using given delimiter between items
  String        Join(const String& delim = " ")
  { return ToDelimString(delim); }
  // generates a string of all the items on the list, using given delimiter between items (same as ToDelimString)
  void          FmDelimString(const String& str, const String& delim = " ", bool reset_first = true);
  // add strings to this array by parsing given string using given delimiter separating strings -- reset first = reset this array before adding (else append)

  void          Split(const String& str, const String& delim = "");
  // Convenience method that calls FmDelimString for splitting a string into an array. Splits on every character by default.


  TA_BASEFUNS(String_Array);
  TA_ARRAY_FUNS(String_Array, String)
protected:
  Variant      El_GetVar_(const void* itm) const override
    { return (Variant)(*(String*)itm); }
  int           El_Compare_(const void* a, const void* b) const override 
  { int rval=-1; if(*((String*)a) > *((String*)b)) rval=1; else if(*((String*)a) == *((String*)b)) rval=0; return rval; }
  bool          El_Equal_(const void* a, const void* b) const override 
    { return (*((String*)a) == *((String*)b)); }
  String        El_GetStr_(const void* it) const override { return (*((String*)it)); }
  void          El_SetFmStr_(void* it, const String& val) override 
  {*((String*)it) = val; }
private:
  void  Copy_(const String_Array&){}
  void Initialize()     { };
  void Destroy()        { };
};

TA_ARRAY_OPS(TA_API, String_Array)

#endif // String_Array_h
