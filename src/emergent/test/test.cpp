#include "test.h"
//#include "ta_list.h"
#include "ta/ta_string.h"

#include <qapplication.h>

#include <iostream>
#include <stdio.h>

using namespace std;

int main(int argc, char* argv[]) {
  QApplication myApp(argc, argv);
  cerr << "test testing...\n";
  
/*  taPlainArray<String> myStrings;
  myStrings.Add("The rain in spain...\n");
  myStrings.Add("...falls mainly in the plain.\n");
  cerr << myStrings[0] << myStrings[1]; */

/*  MyTemplate_int myTemplate_int;
  myTemplate_int.setVar(10);
  cerr << "The value is: " << myTemplate_int.myVar << "\n";
*/
/*
  String s1 = "The rain in spain.";
  s1.gsub("ain", "ane");
  cerr << s1 << " s/b:The rane in spane.\n";

  String s2 = "The rain in spain.";
  s2.gsub("ain", "ye");
  cerr << s2 << " s/b:The rye in spye.\n";

  String s3 = "The rain in spain.";
  s3.gsub("ain", "inging");
  cerr << s3 << " s/b:The ringing in spinging.\n";
*/
  return 0;
}
