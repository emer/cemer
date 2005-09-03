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
// ivcrbuttons.h -- frame with vcr-style buttons, and a single pressed signal

#ifndef IVCRBUTTONS_H
#define IVCRBUTTONS_H

#include "qtdefs.h"

#include <qpushbutton.h>
#include <qhbuttongroup.h>

class iVcrButtons: public QVButtonGroup {
  Q_OBJECT
#ifndef __MAKETA__
typedef QVButtonGroup inherited;
#endif
public:
  enum ButtonIds {
    BUT_BEG_ID,
    BUT_FREV_ID,
    BUT_REV_ID,
    BUT_STOP_ID,
    BUT_PAUSE_ID,
    BUT_PLAY_ID,
    BUT_FFWD_ID,
    BUT_END_ID,
  };

  enum Buttons {
    BUT_BEG		=0x001,
    BUT_FREV		=0x002,
    BUT_REV		=0x004,
    BUT_STOP		=0x008,
    BUT_PAUSE		=0x010,
    BUT_PLAY		=0x020,
    BUT_FFWD		=0x040,
    BUT_END		=0x080,
  };

  bool			hilight() {return mhilight;}
  virtual void		setHilight(bool value);
  iVcrButtons(QWidget* parent = 0, const char* name = 0);
  iVcrButtons(int buttons, QWidget* parent = 0, const char* name = 0);

protected:
  void		init(int buttons);
  bool mhilight; // inner margin
};


#endif

