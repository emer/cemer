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
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TAIQTSO_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TAIQTSO_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef TAIQTSO_EXPORTS
#define TAIQTSO_API __declspec(dllexport)
#else
#define TAIQTSO_API __declspec(dllimport)
#endif

// This class is exported from the taiqtso.dll
class TAIQTSO_API Ctaiqtso {
public:
	Ctaiqtso(void);
	// TODO: add your methods here.
};

extern TAIQTSO_API int ntaiqtso;

TAIQTSO_API int fntaiqtso(void);
