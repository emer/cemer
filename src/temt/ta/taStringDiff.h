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

#ifndef taStringDiff_h
#define taStringDiff_h 1

// parent includes:

// member includes:
#include <taStringDiffItem_PArray>
#include <taHashTable>
#include <int_PArray>
#include <taStringDiffData>

// declare all other types mentioned but not required to include:
class taStringDiffEdits; // 

class TA_API taStringDiff {
  // #NO_TOKENS computes differences between strings, on a line-by-line basis, using the SES/LCS algorithm of Myers (1986), which is used by diff -- converts strings to unique int hash codes first
public:
  enum  OutputFmt {             // format to output diff information ifor GetDiffStr function
    NORMAL,                     // default output of the unix diff utility
    CONTEXT,                    // context diff -- shows some preceeding and trailing lines
  };

  taStringDiffItem_PArray       diffs;
  // #READ_ONLY the resulting differences, as a record of changes
  taHashTable                   hash_codes;
  // #READ_ONLY hash codes used to convert lines into unique integer numbers, which are then used for diff op
  int                           cur_hash_idx;
  // #READ_ONLY current hash value index -- just counts up as hash guys are added
  taStringDiffData              data_a;
  // #READ_ONLY data for first string
  taStringDiffData              data_b;
  // #READ_ONLY data for second string
  int_PArray                    down_vector;
  // #READ_ONLY vector for the (0,0) to (x,y) search
  int_PArray                    up_vector;
  // #READ_ONLY vector for the (u,v) to (N,M) search

  void  DiffStrings(const String& str_a, const String& str_b,
            bool trimSpace = false, bool ignoreSpace = false, bool ignoreCase = false);
  // computes the difference between two Strings, in terms of lines (as in the usual line-based diff utility) -- trimSpace removes leading and trailing space from lines, ignoreSpace ignores all space chars entirely, and ignoreCase downcases everything before comparing
  void ReDiffB(const String& str_a, const String& str_b,
            bool trimSpace = false, bool ignoreSpace = false, bool ignoreCase = false);
  // *after* running previously on str_a and an previous str_b, you can now run the diff again on a *new* str_b -- this will save a fair amount of setup computation on str_a and the hash code table -- str_a *must* be the same as last time obviously..
  bool DiffFiles(const String& fname_a, const String& fname_b,
                         String& str_a, String& str_b,
         bool trimSpace = false, bool ignoreSpace = false, bool ignoreCase = false);
  // loads in two files into the given str_a,b strings, based on the given file names, then runs DiffStrings on them -- trimSpace removes leading and trailing space from lines, ignoreSpace ignores all space chars entirely, and ignoreCase downcases everything before comparing

  void  GetEdits(taStringDiffEdits& edits);
  // *after* running DiffStrings, call this to store the edits so that str_b can be reconstructed from str_b + the diffs..

  String GetDiffStr(const String& str_a, const String& str_b, OutputFmt fmt = NORMAL);
  // *after* running DiffStrings, call this to get a string representation of the differences, using the specified format

  String GetDiffStr_normal(const String& str_a, const String& str_b);
  // #IGNORE
  String GetDiffStr_context(const String& str_a, const String& str_b);
  // #IGNORE

  static String  GetDiffRange(int st, int rg);
  // get a string representation of the range -- if rg > 1, then = ", st+rg" else _nilString

  int GetLinesChanged();
  // *after* running DiffStrings, call this to get total count of lines changed (inserts + deletes) in the diffs

  void  DiffInts(const int_PArray& array_a, const int_PArray& array_b);
  // computes the difference between two arrays of integers -- underlying algorithm uses ints so this is easy..

  void  Diff_impl(const String& str_a, const String& str_b);
  // do the diff based on data in data_a and data_b -- does LCS, Optimize and CreateDiffs

  void  GetLines(taStringDiffData& ddata, const String& str);
  // find line starting positions in the text strings

  void DiffCodes(taStringDiffData& ddata, const String& str,
         bool trimSpace = false, bool ignoreSpace = false, bool ignoreCase = false);
  // convert text string into integer codes

  void  Optimize(taStringDiffData& ddata);
  // optimize sequences of modified strings to make more readable

  void  CreateDiffs(const String& str_a, const String& str_b);
  // create diff list in diffs based on results of LCS/SMS computation, stored in data_a and B

  void  Reset();
  // reset all the current data settings

protected:
  void SMS(int& sms_x, int& sms_y, int lower_a, int upper_a, int lower_b, int upper_b);
  // This is the algorithm to find the Shortest Middle Snake (SMS).
  void LCS(int lower_a, int upper_a, int lower_b, int upper_b);
  // recursive divide-and-conquer routine -- operates on data_a,b and down/up_vector structures
};

#endif // taStringDiff_h
