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

#include <math.h>

/* the value of a 'Real' should be defined.. (it is already in css) */
/* todo: figure out a way to conditionalize this... */
typedef double Real;

/* "special" math functions built into css (and available by including this .h) */
/* these are all derived from chapters 6 & 7 of "Numerical Recipies in C" */

/* _ln is the natural logarithm of */
/* _p is the probability of a given statistic */
/* _q is the complement of the probability of a given statistic */
/* _c is the complement of a function */
/* _cum is the cumulative (indefinite integral) aka distribution function */
/* _den is the density function */

/* helpful functions */
Real fact_ln(int n);			/* ln of factorial for given n */
Real bico_ln(int n, int j);		/* ln of n choose j */

Real hyperg(int j, int s, int t, int n); /* hypergeometric (j t's of sample s in n) */

Real gamma_ln(Real z);			/* ln of gamma function (not distribution)
					   generalization of (n-1)! to real values */
Real gamma_p(Real a, Real x);		/* incomplete gamma function */
Real gamma_q(Real a, Real x);		/* incomplete gamma function (complement of p)*/
Real beta(Real z, Real w); 		/* beta function */
Real beta_i(Real a, Real b, Real x); 	/* incomplete beta function */

/* probability functions */

Real binom_den(int n, int j, Real p);	/* binomial probability function */
Real binom_cum(int n, int j, Real p); 	/* cumulative binomial probability */
Real binom_dev(int n, Real p);		/* binomial deviate: p prob with n trials */

Real poisson_den(int j, Real l);	/* poisson distribution */
Real poisson_cum(int j, Real l);	/* cumulative Poisson P_l(<j) (0 thru j-1) */
Real poisson_dev(Real l);		/* poisson deviate:  mean is l */

Real gamma_den(int j, Real l, Real t); 	/* gamma probability distribution:
					   j events, l lambda, t time */
Real gamma_cum(int j, Real l, Real t);	/* gamma cumulative */
Real gamma_dev(int j);			/* gamma deviate: how long to wait
					   until j events with  unit lambda */

Real gauss_den(Real x);			/* gaussian distribution */
Real gauss_cum(Real z);			/* cumulative gaussian (unit variance) to z */
Real gauss_inv(Real p);			/* inverse of the cumulative for p: z value for given p */
Real gauss_dev();			/* gaussian deviate: normally distributed */
#ifdef HP800
Real erf(Real x);			/* error function */
#endif
Real erf_c(Real x);			/* complement of the error function */

/* statisitics functions */

Real chisq_p(Real X, Real v); 		/* P(X^2 | v) */
Real chisq_q(Real X, Real v); 		/* Q(X^2 | v) (complement) */
Real students_cum(Real t, Real df);	/* cumulative student's distribution df deg of free t test*/
Real students_den(Real t, Real df);	/* density fctn of student's distribution df deg of free t test*/
Real Ftest_q(Real F, Real v1, Real v2); /* F distribution probability F | (v1 < v2) */


