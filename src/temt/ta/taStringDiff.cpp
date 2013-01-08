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

#include "taStringDiff.h"




///////////////////////////////////////////////
// String-Based Text Diff Algorithm

// C++ version of C# code by Matthias Hertel:
// http://www.mathertel.de/Diff

// This Class implements the Difference Algorithm published in
// "An O(ND) Difference Algorithm and its Variations" by Eugene Myers
// Algorithmica Vol. 1 No. 2, 1986, p 251.
//
// There are many C, Java, Lisp implementations public available but they all
// seem to come from the same source (diffutils) that is under the (unfree)
// GNU public License and cannot be reused as a sourcecode for a commercial
// application.  There are very old C implementations that use other (worse)
// algorithms.  Microsoft also published sourcecode of a diff-tool (windiff)
// that uses some tree data.  Also, a direct transfer from a C source to C# is
// not easy because there is a lot of pointer arithmetic in the typical C
// solutions and i need a managed solution.  These are the reasons why I
// implemented the original published algorithm from the scratch and make it
// avaliable without the GNU license limitations.  I do not need a high
// performance diff tool because it is used only sometimes.  I will do some
// performace tweaking when needed.
//
// The algorithm itself is comparing 2 arrays of numbers so when comparing 2
// text documents each line is converted into a (hash) number. See DiffText().
//
// Some chages to the original algorithm: The original algorithm was described
// using a recursive approach and comparing zero indexed arrays.  Extracting
// sub-arrays and rejoining them is very performance and memory intensive so
// the same (readonly) data arrays are passed arround together with their
// lower and upper bounds.  This circumstance makes the LCS and SMS functions
// more complicate.  I added some code to the LCS function to get a fast
// response on sub-arrays that are identical, completely deleted or inserted.
//
// The result from a comparisation is stored in 2 arrays that flag for
// modified (deleted or inserted) lines in the 2 data arrays. These bits are
// then analysed to produce a array of Item objects.
//
// Further possible optimizations: (first rule: don't do it; second: don't do
// it yet) The arrays DataA and DataB are passed as parameters, but are never
// changed after the creation so they can be members of the class to avoid the
// paramter overhead.  In SMS is a lot of boundary arithmetic in the for-D and
// for-k loops that can be done by increment and decrement of local variables.
// The DownVector and UpVector arrays are alywas created and destroyed each
// time the SMS gets called.  It is possible to reuse tehm when transfering
// them to members of the class.  See TODO: hints.
//
// diff.cs: A port of the algorythm to C# Copyright (c) by Matthias Hertel,
// http://www.mathertel.de This work is licensed under a BSD style
// license. See http://www.mathertel.de/License.aspx

// Copyright (c) 2005-2007 by Matthias Hertel, http://www.mathertel.de/

// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.  Redistributions in binary
// form must reproduce the above copyright notice, this list of conditions and
// the following disclaimer in the documentation and/or other materials
// provided with the distribution.  Neither the name of the copyright owners
// nor the names of its contributors may be used to endorse or promote
// products derived from this software without specific prior written
// permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

//
// Changes: 2002.09.20 There was a "hang" in some situations.  Now I undestand
// a little bit more of the SMS algorithm.  There have been overlapping boxes;
// that where analyzed partial differently.  One return-point is enough.  A
// assertion was added in CreateDiffs when in debug-mode, that counts the
// number of equal (no modified) lines in both arrays.  They must be
// identical.
//
// 2003.02.07 Out of bounds error in the Up/Down vector arrays in some
// situations.  The two vetors are now accessed using different offsets that
// are adjusted using the start k-Line.  A test case is added.
//
// 2006.03.05 Some documentation and a direct Diff entry point.
//
// 2006.03.08 Refactored the API to static methods on the Diff class to make
// usage simpler.  2006.03.10 using the standard Debug class for self-test
// now.  compile with: csc /target:exe /out:diffTest.exe /d:DEBUG /d:TRACE
// /d:SELFTEST Diff.cs 2007.01.06 license agreement changed to a BSD style
// license.  2007.06.03 added the Optimize method.  2007.09.23 UpVector and
// DownVector optimization by Jan Stoklasa ().  2008.05.31 Adjusted the
// testing code that failed because of the Optimize method (not a bug in the
// diff algorithm).  2008.10.08 Fixing a test case and adding a new test case.

// Find the difference in 2 text documents, comparing by textlines.  The
// algorithm itself is comparing 2 arrays of numbers so when comparing 2 text
// documents each line is converted into a (hash) number. This hash-value is
// computed by storing all textlines into a common hashtable so i can find
// dublicates in there, and generating a new number each time a new textline
// is inserted.

void taStringDiff::DiffStrings(const String& str_a, const String& str_b,
                               bool trimSpace, bool ignoreSpace, bool ignoreCase) {
  GetLines(data_a, str_a); // get starting line positions within strings
  GetLines(data_b, str_b); // get starting line positions within strings

  hash_codes.Alloc(MAX(data_a.lines, data_b.lines)); // max should be more than enough
  cur_hash_idx = 0;

  // The A-Version of the data (original data) to be compared.
  DiffCodes(data_a, str_a, trimSpace, ignoreSpace, ignoreCase);

  // The B-Version of the data (modified data) to be compared.
  DiffCodes(data_b, str_b,trimSpace, ignoreSpace, ignoreCase);

  Diff_impl(str_a, str_b);
}

void taStringDiff::Diff_impl(const String& str_a, const String& str_b) {
  data_a.InitFmData();
  data_b.InitFmData();

  int max_len = data_a.lines + data_b.lines + 1;
  down_vector.SetSize(2 * max_len + 2);
  up_vector.SetSize(2 * max_len + 2);

  LCS(0, data_a.lines, 0, data_b.lines);

  Optimize(data_a);
  Optimize(data_b);
  CreateDiffs(str_a, str_b);
}

void taStringDiff::ReDiffB(const String& str_a, const String& str_b,
                       bool trimSpace, bool ignoreSpace, bool ignoreCase) {
  GetLines(data_b, str_b); // get starting line positions within strings

  data_a.modified.InitVals(0);

  // The B-Version of the data (modified data) to be compared.
  DiffCodes(data_b, str_b, trimSpace, ignoreSpace, ignoreCase);

  Diff_impl(str_a, str_b);
}

bool taStringDiff::DiffFiles(const String& fname_a, const String& fname_b,
                             String& str_a, String& str_b,
                             bool trimSpace, bool ignoreSpace, bool ignoreCase) {
  bool rval = false;
  fstream istrm;
  int err;
  istrm.open(fname_a.chars(), ios::in);
  if(!istrm.is_open()) goto exit;
  err = str_a.Load_str(istrm);
  if(err) goto exit;

  istrm.close();
  istrm.open(fname_b.chars(), ios::in);
  if(!istrm.is_open()) goto exit;
  err = str_b.Load_str(istrm);
  if(err) goto exit;
  istrm.close();

  DiffStrings(str_a, str_b, trimSpace, ignoreSpace, ignoreCase);

exit:
  istrm.close();
  return rval;
}

void taStringDiff::GetLines(taStringDiffData& ddata, const String& str) {
  ddata.line_st.Reset();
  int st_ln = 0;
  int i;
  for(i=0; i< str.length(); i++) {
    if(str[i] != '\n') continue;
    ddata.line_st.Add(st_ln);
    st_ln = i+1;
  }
  if(st_ln != i) {
    ddata.line_st.Add(st_ln);
  }
  ddata.lines = ddata.line_st.size;
}

// This function converts all textlines of the text into unique numbers for
// every unique textline so further work can work only with simple numbers.
void taStringDiff::DiffCodes(taStringDiffData& ddata, const String& str,
                             bool trimSpace, bool ignoreSpace, bool ignoreCase) {
  ddata.data.Reset();
  for(int i=0; i< ddata.lines; i++) {
    String cur_ln = ddata.GetLine(str, i);
    if(trimSpace)
      cur_ln = trim(cur_ln);
    if(ignoreSpace) {
      cur_ln.gsub(" ", "");
      cur_ln.gsub("\t", "");
    }
    if(ignoreCase)
      cur_ln.downcase();
    taHashVal hash = taPtrList_impl::HashCode_String(cur_ln);
    int val = hash_codes.FindHashVal(hash);
    if(val < 0) {
      val = cur_hash_idx++;
      hash_codes.AddHash(hash, val);
    }
    ddata.data.Add(val);
  }
}

// If a sequence of modified lines starts with a line that contains the same
// content as the line that appends the changes, the difference sequence is
// modified so that the appended line and not the starting line is marked as
// modified.  This leads to more readable diff sequences when comparing text
// files.

void taStringDiff::Optimize(taStringDiffData& ddata) {
  int StartPos, EndPos;

  StartPos = 0;
  while (StartPos < ddata.lines) {
    while ((StartPos < ddata.lines) && !ddata.GetModified(StartPos))
      StartPos++;
    EndPos = StartPos;
    while ((EndPos < ddata.lines) && ddata.GetModified(EndPos))
      EndPos++;

    if ((EndPos < ddata.lines) && (ddata.data[StartPos] == ddata.data[EndPos])) {
      ddata.SetModified(StartPos, false);
      ddata.SetModified(EndPos, true);
    } else {
      StartPos = EndPos;
    } // if
  } // while
}


// Find the difference in 2 arrays of integers.
void taStringDiff::DiffInts(const int_PArray& array_a, const int_PArray& array_b) {
  // The A-Version of the data (original data) to be compared.
  data_a.data = array_a;
  // The B-Version of the data (modified data) to be compared.
  data_b.data = array_b;

  Diff_impl("", "");            // null strings
}

// This is the algorithm to find the Shortest Middle Snake (SMS).

void taStringDiff::SMS(int& sms_x, int& sms_y, int lower_a, int upper_a, int lower_b, int upper_b) {
  int max_len = data_a.lines + data_b.lines + 1;

  int down_k = lower_a - lower_b; // the k-line to start the forward search
  int up_k = upper_a - upper_b; // the k-line to start the reverse search

  int delta = (upper_a - lower_a) - (upper_b - lower_b);
  bool odddelta = (delta & 1) != 0;

  // The vectors in the publication accepts negative indexes. the vectors
  // implemented here are 0-based and are access using a specific offset:
  // up_off up_vector and down_off for DownVektor
  int down_off = max_len - down_k;
  int up_off = max_len - up_k;

  int max_d = ((upper_a - lower_a + upper_b - lower_b) / 2) + 1;

  // init vectors
  down_vector[down_off + down_k + 1] = lower_a;
  up_vector[up_off + up_k - 1] = upper_a;

  for (int d = 0; d <= max_d; d++) {

    // Extend the forward path.
    for (int k = down_k - d; k <= down_k + d; k += 2) {
      // find the only or better starting point
      int x, y;
      if (k == down_k - d) {
        x = down_vector[down_off + k + 1]; // down
      } else {
        x = down_vector[down_off + k - 1] + 1; // a step to the right
        if ((k < down_k + d) && (down_vector[down_off + k + 1] >= x))
          x = down_vector[down_off + k + 1]; // down
      }
      y = x - k;

      // find the end of the furthest reaching forward d-path in diagonal k.
      while ((x < upper_a) && (y < upper_b) && (data_a.data[x] == data_b.data[y])) {
        x++; y++;
      }
      down_vector[down_off + k] = x;

      // overlap ?
      if (odddelta && (up_k - d < k) && (k < up_k + d)) {
        if (up_vector[up_off + k] <= down_vector[down_off + k]) {
          sms_x = down_vector[down_off + k];
          sms_y = down_vector[down_off + k] - k;
          return;
        } // if
      } // if

    } // for k

    // Extend the reverse path.
    for (int k = up_k - d; k <= up_k + d; k += 2) {
      // find the only or better starting point
      int x, y;
      if (k == up_k + d) {
        x = up_vector[up_off + k - 1]; // up
      } else {
        x = up_vector[up_off + k + 1] - 1; // left
        if ((k > up_k - d) && (up_vector[up_off + k - 1] < x))
          x = up_vector[up_off + k - 1]; // up
      } // if
      y = x - k;

      while ((x > lower_a) && (y > lower_b) && (data_a.data[x - 1] == data_b.data[y - 1])) {
        x--; y--; // diagonal
      }
      up_vector[up_off + k] = x;

      // overlap ?
      if (!odddelta && (down_k - d <= k) && (k <= down_k + d)) {
        if (up_vector[up_off + k] <= down_vector[down_off + k]) {
          sms_x = down_vector[down_off + k];
          sms_y = down_vector[down_off + k] - k;
          return;
        } // if
      } // if

    } // for k

  } // for d

  taMisc::Error("taStringDiff: the algorithm should never come here!");
}

// This is the divide-and-conquer implementation of the longes
// common-subsequence (LCS) algorithm.  The published algorithm passes
// recursively parts of the A and B sequences.  To avoid copying these
// arrays the lower and upper bounds are passed while the sequences stay
// constant.

void taStringDiff::LCS(int lower_a, int upper_a, int lower_b, int upper_b) {
  // Fast walkthrough equal lines at the start
  while (lower_a < upper_a && lower_b < upper_b && data_a.data[lower_a] == data_b.data[lower_b]) {
    lower_a++; lower_b++;
  }

  // Fast walkthrough equal lines at the end
  while (lower_a < upper_a && lower_b < upper_b && data_a.data[upper_a - 1] == data_b.data[upper_b - 1]) {
    --upper_a; --upper_b;
  }

  if (lower_a == upper_a) {
    // mark as inserted lines.
    while (lower_b < upper_b)
      data_b.SetModified(lower_b++, true);

  } else if (lower_b == upper_b) {
    // mark as deleted lines.
    while (lower_a < upper_a)
      data_a.SetModified(lower_a++, true);

  } else {
    // Find the middle snakea and length of an optimal path for A and B
    int sms_x, sms_y;
    SMS(sms_x, sms_y, lower_a, upper_a, lower_b, upper_b);

    // The path is from LowerX to (x,y) and (x,y) to UpperX
    LCS(lower_a, sms_x, lower_b, sms_y);
    LCS(sms_x, upper_a, sms_y, upper_b);  // 2002.09.20: no need for 2 points
  }
}


// Scan the tables of which lines are inserted and deleted,
// producing an edit script in forward order.

void taStringDiff::CreateDiffs(const String& str_a, const String& str_b) {
  diffs.Reset();

  int start_a, start_b;
  int line_a = 0;
  int line_b = 0;

  while (line_a < data_a.lines || line_b < data_b.lines) {
    if ((line_a < data_a.lines) && (!data_a.GetModified(line_a))
        && (line_b < data_b.lines) && (!data_b.GetModified(line_b))) {
      // equal lines
      line_a++;
      line_b++;

    } else {
      // maybe deleted and/or inserted lines
      start_a = line_a;
      start_b = line_b;

      while (line_a < data_a.lines && (line_b >= data_b.lines || data_a.GetModified(line_a)))
        // while (line_a < data_a.lines && data_a.GetModified(line_a])
        line_a++;

      while (line_b < data_b.lines && (line_a >= data_a.lines || data_b.GetModified(line_b)))
        // while (line_b < data_b.lines && data_b.GetModified(line_b])
        line_b++;

      if ((start_a < line_a) || (start_b < line_b)) {
        taStringDiffItem nw_itm;
        // store a new difference-item
        nw_itm.start_a = start_a;
        nw_itm.start_b = start_b;
        nw_itm.delete_a = line_a - start_a;
        nw_itm.insert_b = line_b - start_b;
        if(str_b.nonempty() && nw_itm.insert_b > 0) {
          nw_itm.insert_b_str = data_b.GetLine(str_b, start_b, line_b-1);
        }
        diffs.Add(nw_itm);
      } // if
    } // if
  } // while
}

void taStringDiff::GetEdits(taStringDiffEdits& edits) {
  edits.diffs.Reset();
  edits.diffs = diffs;
  edits.line_st.Reset();
  edits.line_st = data_a.line_st;
}

String taStringDiff::GetDiffStr(const String& str_a, const String& str_b, OutputFmt fmt) {
  if(diffs.size == 0) return "No differences found!\n";
  if(fmt == NORMAL) {
    return GetDiffStr_normal(str_a, str_b);
  }
  else {                        // context
    return GetDiffStr_context(str_a, str_b);
  }
}

String taStringDiff::GetDiffRange(int st, int rg) {
  if(rg > 1)
    return "," + String(st + rg);
  return _nilString;
}

String taStringDiff::GetDiffStr_normal(const String& str_a, const String& str_b) {
  String rval;
  for(int i=0;i<diffs.size;i++) {
    taStringDiffItem& df = diffs[i];
    bool chg = false;
    if(df.delete_a == df.insert_b) {
      rval += String(df.start_a+1) + "c" + String(df.start_b+1) +
        GetDiffRange(df.start_b, df.insert_b) + "\n";
      chg = true;
    }
    if(df.delete_a > 0) {
      if(!chg) {
        rval += String(df.start_a+1) +
          GetDiffRange(df.start_a, df.delete_a)
          + "d" + String(df.start_b+1) + "\n";
      }
      for(int l=df.start_a; l<df.start_a + df.delete_a; l++)
        rval += "< " + data_a.GetLine(str_a, l) + "\n";
      if(chg) {
        rval += "---\n";
      }
    }
    if(df.insert_b > 0) {
      if(!chg) {
        rval += String(df.start_a+1) + "a" + String(df.start_b+1) +
          GetDiffRange(df.start_b, df.insert_b) + "\n";
      }
      for(int l=df.start_b; l<df.start_b + df.insert_b; l++)
        rval += "> " + data_b.GetLine(str_b, l) + "\n";
    }
  }
  return rval;
}

String taStringDiff::GetDiffStr_context(const String& str_a, const String& str_b) {
  return "Context format not currently supported -- please use NORMAL\n";
}

int taStringDiff::GetLinesChanged() {
  int tot = 0;
  for(int i=0;i<diffs.size;i++) {
    taStringDiffItem& df = diffs[i];
    tot += df.insert_b + df.delete_a;
  }
  return tot;
}

void taStringDiff::Reset() {
  diffs.Reset();
  hash_codes.Reset();
  cur_hash_idx = 0;
  data_a.Reset();
  data_b.Reset();
  down_vector.Reset();
  up_vector.Reset();
}
