// lef.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <fstream>

using namespace std;

/*int any_to_unix() {
  char c;
  while (true) {
    c = cin.get();
    if (c == EOF) break;
    if (c == '\r') {
      if (cin.peek() == '\n') { // DOS
        c = cin.get(); // discard cr and fetch the lf
      } else { // MAC
        c = '\n'; // convert the cr to the lf
      }
    }
    cout.put(c);
  }
  return 0;
} */

int any_to_unix(ifstream& bin, ofstream& bout) {
  char c;
  while (true) { //note: should already have converted to lf's, but just in case...
    c = bin.get();
    if (c == EOF) break;
    if (c == '\r') {
      if (bin.peek() == '\n') { // DOS
        c = bin.get(); // discard cr and fetch the lf
      } else { // MAC
        c = '\n'; // convert the cr to the lf
      }
    }
    bout.put(c);
  }
  return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
  if (argc < 2) {
    cerr << "Syntax: lef [filein] fileout\n";
    cerr << "  if 'filein' not supplied, stdin used\n";
    return 1;
  }
  ifstream* bin;
  char* fileout;
  if (argc == 2) {
    bin = (ifstream*)(&cin);
    fileout = argv[1];
  } else { 
    bin = new ifstream(argv[1]);
    if (!bin->good()) {
      cerr << "Could not open filein.\n";
      return 2;
    }
    fileout = argv[2];
  }
  ofstream bout(fileout, ios_base::binary);

  // only support any to unix for now
  int rval = 0;
  rval = any_to_unix(*bin, bout);
	return rval;
}

