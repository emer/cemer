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

#ifndef css_builtin_h
#define css_builtin_h 1

// builtin function exports

class cssTA_Base;

class cssBI { // built-in functions for css
public:
  static cssElCFun*	asgn;
  static cssElCFun*	asgn_add;
  static cssElCFun*	asgn_sub;
  static cssElCFun*	asgn_mult;
  static cssElCFun*	asgn_div;
  static cssElCFun*	asgn_mod;
  static cssElCFun*	asgn_lshift;
  static cssElCFun*	asgn_rshift;
  static cssElCFun*	asgn_and;
  static cssElCFun*	asgn_xor;
  static cssElCFun*	asgn_or;
  static cssElCFun*	init_asgn;
  static cssElCFun*	asgn_post_pp;
  static cssElCFun*	asgn_pre_pp;
  static cssElCFun*	asgn_post_mm;
  static cssElCFun*	asgn_pre_mm;
  static cssElCFun*	new_opr;
  static cssElCFun*	del_opr;
  static cssElCFun*	constr;	// construct an object
  static cssElCFun*	add;
  static cssElCFun*	sub;
  static cssElCFun*	mul;
  static cssElCFun*	div;
  static cssElCFun*	modulo;
  static cssElCFun*	bit_and;
  static cssElCFun*	bit_xor;
  static cssElCFun*	bit_or;
  static cssElCFun*	neg;
  static cssElCFun*	addr_of;
  static cssElCFun*	de_ptr;
  static cssElCFun*	de_array;
  static cssElCFun*	points_at;
  static cssElCFun*	member_fun;
  static cssElCFun*	rshift;
  static cssElCFun*	lshift;
  static cssElCFun*	scoper;
  static cssElCFun*	pop;
  static cssElCFun*	cast;
  static cssElCFun*	cond;
  static cssElCFun*	ifcd;
  static cssElCFun*	switch_jump;

  static cssElCFun*	push_root;	// pushes a root value on stack
  static cssElCFun*	push_next; 	// pushes next program item on stack
  static cssElCFun*	push_cur_this;	// push current this pointer on stack
  static cssElCFun*	arg_swap;	// flips the args on the stack
  static cssElCFun*	array_alloc;
  static cssElCFun*	sstream_rewind;

  static cssInt*	true_int;
  static cssInt*	false_int;
  static cssElCFun*	gt;
  static cssElCFun*	lt;
  static cssElCFun*	eq;
  static cssElCFun*	ge;
  static cssElCFun*	le;
  static cssElCFun*	ne;
  static cssElCFun*	land;
  static cssElCFun*	lor;
  static cssElCFun*	lnot;
  static cssElCFun*	power;

  static cssTA_Base*	root;		// root script element
};

#endif // css_builtin_h
