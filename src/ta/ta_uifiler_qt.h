// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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


#ifndef ta_uifiler_iv_h
#define ta_uifiler_iv_h 1

// NOTE: taFiler replaces the duo of taivGetFile and taFile from 3.x
//	the ui portion of taivGetFile has been transferred to taUiFiler
//	implementations
#include <ta/ta_filer.h>
#include <ta/taiv_dialog.h>

#include <ta/enter_iv.h>
#include <InterViews/dialog.h>
#include <IV-look/button.h> //for script button
#include <InterViews/deck.h> // for hilight Button
#include <ta/leave_iv.h>

class ivWindow;
class taivDialog;

//////////////////////////////////
// 	taUiFiler_impl		//
//////////////////////////////////

class taUiFiler_impl : public taUiFiler {
public:
  ivWindow*	win;		// #IGNORE
  taivDialog*   dlg;		// #IGNORE
  bool		GetFileName(String& fname, taFiler* filer, FilerOperation filerOperation);
  
  taUiFiler_impl();	
  taUiFiler_impl(ivWindow* win_);
};

taUiFiler* taUiFiler_impl_CreateInstance();	// required proc, to create instances

#endif // ta_uifiler_iv_h
 
