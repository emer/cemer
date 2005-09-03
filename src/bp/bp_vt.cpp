/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
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

// virtual table instantiation file
 
#include "rbp.h"
#include <ta_misc/datagraph.h>
#include <ta_misc/datatable_iv.h>
#include <ta/ta_dump.h>
#include <pdp/pdpshell.h>
#include <pdp/pdp_iv.h>
#include <pdp/pdplog.h>
#include <pdp/net_iv.h>
#include <pdp/netstru_extra.h>
#include <pdp/enviro_extra.h>
#include <pdp/procs_extra.h>
#include <css/css_iv.h>
#include <css/c_ptr_types.h>
#include <css/ta_css.h>

#include <ta/enter_iv.h>
#include <InterViews/patch.h>
#include <InterViews/window.h>
#include <ta/leave_iv.h>

class GetWinPosPatch : public ivPatch {
public:
  WinBase*	obj;

  GetWinPosPatch(WinBase* wb, ivGlyph* g) : ivPatch(g) { obj = wb; }

  void 	allocate(ivCanvas* c, const ivAllocation& a, ivExtension& e) {
    ivPatch::allocate(c, a, e);
     if(obj->IsMapped()) {
      obj->win_pos.wd = obj->window->width();
#ifndef CYGWIN
      obj->win_pos.ht = obj->window->height();
#else
      obj->win_pos.ht = obj->window->height() - 20.0f; // subtract out window bar
#endif
    }
  }
};
