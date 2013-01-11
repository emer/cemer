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

#ifndef taLicense_h
#define taLicense_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taLicense : public taOBase {
  // #EDIT_INLINE license information -- attach a license to an object
INHERITED(taOBase)
public:
  enum  StdLicense {          // standard licenses in wide use
    NO_LIC,                     // no license granted -- All Rights Reserved
    GPLv2,                      // GNU General Public License (GPL), version 2
    GPLv3,                      // GNU General Public License (GPL), version 3 -- note that this has patent grant issues -- version 2 is probably safer
    BSD,                        // Berkeley Software Distribution License
    OSL3,                       // Open Software License, version 3
    AFL3,                       // Academic Free License, version 3
    MIT,                        // MIT License
    APACHE,                     // Apache License, version 2.0
    RESEARCH,                   // Research-Only License based on CU Boulder template
    CUSTOM_LIC,                 // enter custom license text
  };

  StdLicense    license;   // select a standard license to use, or CUSTOM_LIC to enter custom license text
  String        owner;          // legal name of copyright owner (e.g., Regents of University of xyz) -- will be filled in by license_owner in global preferences/options setting by default for new items
  String        org;            // name of organization that actually created the content (e.g., MyLab at University of xyz) -- defaults to owner if left blank  -- will be filled in by license_org in global preferences/options setting by default for new items
  String        year;           // copyright year (can be a range)
  String        custom;         // #CONDSHOW_ON_license:CUSTOM_LIC custom license text
  String        lic_text;       // #HIDDEN #NO_SAVE full license text -- for view license

  void          ViewLicense();  // #BUTTON view the license text based on current selection
  static String GetLicenseText(StdLicense lic, const String& owner, const String& year, const String& org=""); // get text for given license, given parameters

  TA_SIMPLE_BASEFUNS(taLicense);
private:
  void  Initialize();
  void  Destroy()    { };
};

#endif // taLicense_h
