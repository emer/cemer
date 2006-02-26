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


#include <math.h>

#include "css_def.h"

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
CSS_API Real fact_ln(int n);			/* ln of factorial for given n */
CSS_API Real bico_ln(int n, int j);		/* ln of n choose j */

CSS_API Real hyperg(int j, int s, int t, int n); /* hypergeometric (j t's of sample s in n) */

CSS_API Real gamma_ln(Real z);			/* ln of gamma function (not distribution)
					   generalization of (n-1)! to real values */
CSS_API Real gamma_p(Real a, Real x);		/* incomplete gamma function */
CSS_API Real gamma_q(Real a, Real x);		/* incomplete gamma function (complement of p)*/
CSS_API Real beta(Real z, Real w); 		/* beta function */
CSS_API Real beta_i(Real a, Real b, Real x); 	/* incomplete beta function */

/* probability functions */

CSS_API Real binom_den(int n, int j, Real p);	/* binomial probability function */
CSS_API Real binom_cum(int n, int j, Real p); 	/* cumulative binomial probability */
CSS_API Real binom_dev(int n, Real p);		/* binomial deviate: p prob with n trials */

CSS_API Real poisson_den(int j, Real l);	/* poisson distribution */
CSS_API Real poisson_cum(int j, Real l);	/* cumulative Poisson P_l(<j) (0 thru j-1) */
CSS_API Real poisson_dev(Real l);		/* poisson deviate:  mean is l */

CSS_API Real gamma_den(int j, Real l, Real t); 	/* gamma probability distribution:
					   j events, l lambda, t time */
CSS_API Real gamma_cum(int j, Real l, Real t);	/* gamma cumulative */
CSS_API Real gamma_dev(int j);			/* gamma deviate: how long to wait
					   until j events with  unit lambda */

CSS_API Real gauss_den(Real x);			/* gaussian distribution */
CSS_API Real gauss_cum(Real z);			/* cumulative gaussian (unit variance) to z */
CSS_API Real gauss_inv(Real p);			/* inverse of the cumulative for p: z value for given p */
CSS_API Real gauss_dev();			/* gaussian deviate: normally distributed */
#ifdef HP800
CSS_API Real erf(Real x);			/* error function */
#endif
CSS_API Real erf_c(Real x);			/* complement of the error function */

/* statisitics functions */

CSS_API Real chisq_p(Real X, Real v); 		/* P(X^2 | v) */
CSS_API Real chisq_q(Real X, Real v); 		/* Q(X^2 | v) (complement) */
CSS_API Real students_cum(Real t, Real df);	/* cumulative student's distribution df deg of free t test*/
CSS_API Real students_den(Real t, Real df);	/* density fctn of student's distribution df deg of free t test*/
CSS_API Real Ftest_q(Real F, Real v1, Real v2); /* F distribution probability F | (v1 < v2) */


