// Copyright, 1995-2007, Regents of the University of Colorado,
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


#ifndef css_builtin_h
#define css_builtin_h 1

#include "css_def.h"

// builtin function exports

class cssTA_Base;
class cssTA_Matrix;

class CSS_API cssBI { // built-in functions for css
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
  static cssElCFun*	make_matrix;
  static cssElCFun*	points_at;
  static cssElCFun*	member_fun;
  static cssElCFun*	member_call;
  static cssElCFun*	rshift;
  static cssElCFun*	lshift;
  static cssElCFun*	scoper;
  static cssElCFun*	pop;
  static cssElCFun*	cast;
  static cssElCFun*	cond;
  static cssElCFun*	switch_jump;
  static cssElCFun*	doloop;

  static cssElCFun*	push_root;	// pushes a root value on stack
  static cssElCFun*	push_next; 	// pushes next program item on stack
  static cssElCFun*	push_cur_this;	// push current this pointer on stack
  static cssElCFun*	arg_swap;	// flips the args on the stack
  static cssElCFun*	fun_done;	// call the FunDone function on obj before me (function cleanup)
  static cssElCFun*	array_alloc;
  static cssElCFun*	sstream_rewind;

  static cssElCFun*	gt;
  static cssElCFun*	lt;
  static cssElCFun*	eq;
  static cssElCFun*	ge;
  static cssElCFun*	le;
  static cssElCFun*	ne;
  static cssElCFun*	land;
  static cssElCFun*	lor;
  static cssElCFun*	lnot;
  static cssElCFun*	bitneg;
  static cssElCFun*	power;

  static cssInt*	true_int;
  static cssInt*	false_int;
  static cssConstBool*	semicolon_mark;
  static cssConstBool*	colon_mark;
  static cssConstBool*	comma_mark;

  static cssTA_Base*	root;		// root script element
  static cssTA_Matrix*  matrix_inst;	// instance of matrix for making tokens
};

#endif // css_builtin_h
