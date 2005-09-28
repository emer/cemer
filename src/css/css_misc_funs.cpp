// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
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

#include "css_misc_funs.h"

#if (defined(WIN32) && (!defined(CYGWIN)))

#include <windows.h>

String_Array& Dir(const char* dir_n) {
  static String_Array entries;
  String dir_nm(dir_n);
  dir_nm += "\\*";

  entries.Reset();
  WIN32_FIND_DATA FindFileData;
  HANDLE hFind = NULL;

  hFind = FindFirstFile(dir_nm.chars(), &FindFileData);

  if (hFind == INVALID_HANDLE_VALUE) {
    taMisc::Error("*** Could not open directory:", dir_n);
  } else {
    entries.Add(FindFileData.cFileName);
    while (FindNextFile(hFind, &FindFileData) != 0) {
      entries.Add(FindFileData.cFileName);
    }
    
    DWORD dwError = GetLastError();
    if (dwError == ERROR_NO_MORE_FILES) {
       FindClose(hFind);
    } else {
       taMisc::Error("*** FindNextFile error. Error is %u\n", ((String)dwError).chars());
    }
  }
  return entries;
}

#else // Unix
#include <dirent.h>

String_Array& Dir(const char* dir_n) {
  static String_Array entries;
  String dir_nm = ".";
  if(dir_n != NULL)
    dir_nm = dir_n;

  entries.Reset();
  DIR* dirp = opendir(dir_nm);
  if(dirp == NULL) {
    taMisc::Error("*** Could not open directory:", (const char*)dir_nm);
    return entries;
  }
  struct dirent* dp;
  while((dp = readdir(dirp))) {
    String nm = dp->d_name;
    entries.Add(nm);
  }
  return entries;
}

#endif 

String_Array& ReadLine(istream& fh) { //note: works for all three line ends: lf (unix), cr (mac), crlf (dos)
  static String_Array data;
  data.Reset();
  if(fh.bad() || fh.eof())
    return data;
  while(true) {
    int c;
    while (((c=fh.peek()) == ' ') || (c == '\t')) fh.get();
    if((c == '\n') || (c == '\r') || fh.eof()) {
      fh.get();		// get the terminator..
      if (c == '\r') { // dos, 2nd char
        if (fh.peek() == '\n') fh.get();
      }
      break;
    }
    data.Add("");
    String str = data.Peek();
    while(((c=fh.get()) != ' ') && (c != '\t') && (c != '\n') && (c != '\r') && !fh.eof())
      str += (char)c;
    if (fh.eof())	break;
    if((c == '\n') || (c == '\r'))	{
      if ((c == '\r') && (fh.peek() == '\n')) fh.get();
      break;
    }
  }
  return data;
}

