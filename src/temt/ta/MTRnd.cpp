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

// includes some stuff from here too:
/*
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote
        products derived from this software without specific prior written
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.keio.ac.jp/matumoto/emt.html
   email: matumoto@math.keio.ac.jp
*/

#include "MTRnd.h"

#include <taMisc>
#include <cmath>

TA_BASEFUNS_CTORS_DEFN(MTRndPar);
TA_BASEFUNS_CTORS_DEFN(MTRndPar_List);
TA_BASEFUNS_CTORS_DEFN(MTRnd);

///////////////////////////////////////////////////////////////////
//              First, we include the MT19937 basic RNG
//              for generating random numbers for generators!

/* A C-program for MT19937: Integer version (1999/10/28)          */
/*  genrand() generates one pseudorandom unsigned integer (32bit) */
/* which is uniformly distributed among 0 to 2^32-1  for each     */
/* call. sgenrand(seed) sets initial values to the working area   */
/* of 624 words. Before genrand(), sgenrand(seed) must be         */
/* called once. (seed is any 32-bit integer.)                     */
/*   Coded by Takuji Nishimura, considering the suggestions by    */
/* Topher Cooper and Marc Rieffel in July-Aug. 1997.              */

/* When you use this, send an email to: matumoto@math.keio.ac.jp   */
/* with an appropriate reference to your work.                     */

/* REFERENCE                                                       */
/* M. Matsumoto and T. Nishimura,                                  */
/* "Mersenne Twister: A 623-Dimensionally Equidistributed Uniform  */
/* Pseudo-Random Number Generator",                                */
/* ACM Transactions on Modeling and Computer Simulation,           */
/* Vol. 8, No. 1, January 1998, pp 3--30.                          */

#define N 624

typedef struct _ORG_STATE {
    uint32_t mt[N];
    int mti;
} _org_state;

void _sgenrand_dc(_org_state *st, uint32_t seed);
uint32_t _genrand_dc(_org_state *st);

/* Period parameters */
/* #define N 624 */
#define M 397
#define MATRIX_A   0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000   /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff   /* least significant r bits */

/* Tempering parameters */
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

/* Initializing the array with a seed */
void _sgenrand_dc(_org_state *st, uint32_t seed)
{
  int i;

  for (i=0;i<N;i++) {
    st->mt[i] = seed;
    seed = (1812433253 * (seed  ^ (seed >> 30))) + i + 1;
    /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
    /* In the previous versions, MSBs of the seed affect   */
    /* only MSBs of the array mt[].                        */
  }
  st->mti = N;
}


uint32_t _genrand_dc(_org_state *st)
{
  uint32_t y;
  static const uint32_t mag01[2]={0x0, MATRIX_A};
  /* mag01[x] = x * MATRIX_A  for x=0,1 */

  if (st->mti >= N) { /* generate N words at one time */
    int kk;

    for (kk=0;kk<N-M;kk++) {
      y = (st->mt[kk]&UPPER_MASK)|(st->mt[kk+1]&LOWER_MASK);
      st->mt[kk] = st->mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
    }
    for (;kk<N-1;kk++) {
      y = (st->mt[kk]&UPPER_MASK)|(st->mt[kk+1]&LOWER_MASK);
      st->mt[kk] = st->mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1];
    }
    y = (st->mt[N-1]&UPPER_MASK)|(st->mt[0]&LOWER_MASK);
    st->mt[N-1] = st->mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1];

    st->mti = 0;
  }

  y = st->mt[st->mti++];
  y ^= TEMPERING_SHIFT_U(y);
  y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
  y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
  y ^= TEMPERING_SHIFT_L(y);

  return y;
}

////////////////////////////////////////////////////////////////
//              dci.h -- internal stuff for dcmt

#define NOT_REJECTED 1
#define REJECTED 0
#define REDU 0
#define IRRED 1
#define NONREDU 1

extern _org_state global_mt19937;
typedef struct {int *x; int deg;} Polynomial;

typedef struct PRESCR_T {
    int sizeofA; /* parameter size */
    uint32_t **modlist;
    Polynomial **preModPolys;
} prescr_t;

typedef struct CHECK32_T {
    uint32_t upper_mask;
    uint32_t lower_mask;
    uint32_t word_mask;
} check32_t;

typedef struct EQDEG_T {
    uint32_t bitmask[32];
    uint32_t mask_b;
    uint32_t mask_c;
    uint32_t upper_v_bits;
    int shift_0;
    int shift_1;
    int shift_s;
    int shift_t;
    int mmm;
    int nnn;
    int rrr;
    int www;
    uint32_t aaa[2];
    uint32_t gupper_mask;   /** most significant  (WWW - RRR) bits **/
    uint32_t glower_mask;	/** least significant RRR bits **/
    uint32_t greal_mask;	/** upper WWW bitmask **/
    int ggap; /** difference between machine wordsize and dest wordsize **/
    int gcur_maxlengs[32];	/** for optimize_v_hard **/
    uint32_t gmax_b, gmax_c;
} eqdeg_t;


////////////////////////////////////////////////////////////////////
//              check32

#define LSB 0x1
#define WORDLEN 32

void _InitCheck32_dc(check32_t *ck, int r, int w) {
  int i;

  /* word_mask (least significant w bits) */
  ck->word_mask = 0xFFFFFFFF;
  ck->word_mask <<= WORDLEN - w;
  ck->word_mask >>= WORDLEN - w;
  /* lower_mask (least significant r bits) */
  for (ck->lower_mask=0,i=0; i<r; ++i) {
    ck->lower_mask <<= 1;
    ck->lower_mask |= LSB;
  }
  /* upper_mask (most significant (w-r) bits */
  ck->upper_mask = (~ck->lower_mask) & ck->word_mask;
}

int _CheckPeriod_dc(check32_t *ck, _org_state *st,
		    uint32_t a, int m, int n, int r, int w)
{
  int i, j, p, pp;
  uint32_t y, *x, *init, mat[2];


  p = n*w-r;
  x = (uint32_t*) malloc (2*p*sizeof(uint32_t));
  if (NULL==x) {
    taMisc::Error("malloc error in \"_CheckPeriod_dc()\"");
    exit(1);
  }

  init = (uint32_t*) malloc (n*sizeof(uint32_t));
  if (NULL==init) {
    taMisc::Error("malloc error \"_CheckPeriod_dc()\"");
    free(x);
    exit(1);
  }

  /* set initial values */
  for (i=0; i<n; ++i)
    x[i] = init[i] = (ck->word_mask & _genrand_dc(st));
  /* it is better that LSBs of x[2] and x[3] are different */
  if ( (x[2]&LSB) == (x[3]&LSB) ) {
    x[3] ^= 1;
    init[3] ^= 1;
  }

  pp = 2*p-n;
  mat[0] = 0; mat[1] = a;
  for (j=0; j<p; ++j) {

    /* generate */
    for (i=0; i<pp; ++i){
      y = (x[i]&ck->upper_mask) | (x[i+1]&ck->lower_mask);
      x[i+n] = x[i+m] ^ ( (y>>1) ^ mat[y&LSB] );
    }

    /* pick up odd subscritpt elements */
    for (i=2; i<=p; ++i)
      x[i] = x[(i<<1)-1];

    /* reverse generate */
    for (i=p-n; i>=0; --i) {
      y = x[i+n] ^ x[i+m] ^ mat[ x[i+1]&LSB ];
      y <<=1; y |= x[i+1]&LSB;

      x[i+1] = (x[i+1]&ck->upper_mask) | (y&ck->lower_mask);
      x[i] = (y&ck->upper_mask) | (x[i]&ck->lower_mask);
    }

  }

  if ((x[0]&ck->upper_mask)==(init[0]&ck->upper_mask)) {
    for (i=1; i<n; ++i) {
      if (x[i] != init[i])
        break;
    }
    if (i==n) {
      free(x); free(init);
      return IRRED;
    }
  }


  free(x); free(init);
  return REDU;
}

/////////////////////////////////////////////////////
//              prescr.c -- prescreening

#define LIMIT_IRRED_DEG 31
#define NIRREDPOLY 127
#define MAX_IRRED_DEG 9

/* list of irreducible polynomials whose degrees are less than 10 */
static const int irredpolylist[NIRREDPOLY][MAX_IRRED_DEG+1] = {
    {0,1,0,0,0,0,0,0,0,0,},{1,1,0,0,0,0,0,0,0,0,},{1,1,1,0,0,0,0,0,0,0,},
    {1,1,0,1,0,0,0,0,0,0,},{1,0,1,1,0,0,0,0,0,0,},{1,1,0,0,1,0,0,0,0,0,},
    {1,0,0,1,1,0,0,0,0,0,},{1,1,1,1,1,0,0,0,0,0,},{1,0,1,0,0,1,0,0,0,0,},
    {1,0,0,1,0,1,0,0,0,0,},{1,1,1,1,0,1,0,0,0,0,},{1,1,1,0,1,1,0,0,0,0,},
    {1,1,0,1,1,1,0,0,0,0,},{1,0,1,1,1,1,0,0,0,0,},{1,1,0,0,0,0,1,0,0,0,},
    {1,0,0,1,0,0,1,0,0,0,},{1,1,1,0,1,0,1,0,0,0,},{1,1,0,1,1,0,1,0,0,0,},
    {1,0,0,0,0,1,1,0,0,0,},{1,1,1,0,0,1,1,0,0,0,},{1,0,1,1,0,1,1,0,0,0,},
    {1,1,0,0,1,1,1,0,0,0,},{1,0,1,0,1,1,1,0,0,0,},{1,1,0,0,0,0,0,1,0,0,},
    {1,0,0,1,0,0,0,1,0,0,},{1,1,1,1,0,0,0,1,0,0,},{1,0,0,0,1,0,0,1,0,0,},
    {1,0,1,1,1,0,0,1,0,0,},{1,1,1,0,0,1,0,1,0,0,},{1,1,0,1,0,1,0,1,0,0,},
    {1,0,0,1,1,1,0,1,0,0,},{1,1,1,1,1,1,0,1,0,0,},{1,0,0,0,0,0,1,1,0,0,},
    {1,1,0,1,0,0,1,1,0,0,},{1,1,0,0,1,0,1,1,0,0,},{1,0,1,0,1,0,1,1,0,0,},
    {1,0,1,0,0,1,1,1,0,0,},{1,1,1,1,0,1,1,1,0,0,},{1,0,0,0,1,1,1,1,0,0,},
    {1,1,1,0,1,1,1,1,0,0,},{1,0,1,1,1,1,1,1,0,0,},{1,1,0,1,1,0,0,0,1,0,},
    {1,0,1,1,1,0,0,0,1,0,},{1,1,0,1,0,1,0,0,1,0,},{1,0,1,1,0,1,0,0,1,0,},
    {1,0,0,1,1,1,0,0,1,0,},{1,1,1,1,1,1,0,0,1,0,},{1,0,1,1,0,0,1,0,1,0,},
    {1,1,1,1,1,0,1,0,1,0,},{1,1,0,0,0,1,1,0,1,0,},{1,0,1,0,0,1,1,0,1,0,},
    {1,0,0,1,0,1,1,0,1,0,},{1,0,0,0,1,1,1,0,1,0,},{1,1,1,0,1,1,1,0,1,0,},
    {1,1,0,1,1,1,1,0,1,0,},{1,1,1,0,0,0,0,1,1,0,},{1,1,0,1,0,0,0,1,1,0,},
    {1,0,1,1,0,0,0,1,1,0,},{1,1,1,1,1,0,0,1,1,0,},{1,1,0,0,0,1,0,1,1,0,},
    {1,0,0,1,0,1,0,1,1,0,},{1,0,0,0,1,1,0,1,1,0,},{1,0,1,1,1,1,0,1,1,0,},
    {1,1,0,0,0,0,1,1,1,0,},{1,1,1,1,0,0,1,1,1,0,},{1,1,1,0,1,0,1,1,1,0,},
    {1,0,1,1,1,0,1,1,1,0,},{1,1,1,0,0,1,1,1,1,0,},{1,1,0,0,1,1,1,1,1,0,},
    {1,0,1,0,1,1,1,1,1,0,},{1,0,0,1,1,1,1,1,1,0,},{1,1,0,0,0,0,0,0,0,1,},
    {1,0,0,0,1,0,0,0,0,1,},{1,1,1,0,1,0,0,0,0,1,},{1,1,0,1,1,0,0,0,0,1,},
    {1,0,0,0,0,1,0,0,0,1,},{1,0,1,1,0,1,0,0,0,1,},{1,1,0,0,1,1,0,0,0,1,},
    {1,1,0,1,0,0,1,0,0,1,},{1,0,0,1,1,0,1,0,0,1,},{1,1,1,1,1,0,1,0,0,1,},
    {1,0,1,0,0,1,1,0,0,1,},{1,0,0,1,0,1,1,0,0,1,},{1,1,1,1,0,1,1,0,0,1,},
    {1,1,1,0,1,1,1,0,0,1,},{1,0,1,1,1,1,1,0,0,1,},{1,1,1,0,0,0,0,1,0,1,},
    {1,0,1,0,1,0,0,1,0,1,},{1,0,0,1,1,0,0,1,0,1,},{1,1,0,0,0,1,0,1,0,1,},
    {1,0,1,0,0,1,0,1,0,1,},{1,1,1,1,0,1,0,1,0,1,},{1,1,1,0,1,1,0,1,0,1,},
    {1,0,1,1,1,1,0,1,0,1,},{1,1,1,1,0,0,1,1,0,1,},{1,0,0,0,1,0,1,1,0,1,},
    {1,1,0,1,1,0,1,1,0,1,},{1,0,1,0,1,1,1,1,0,1,},{1,0,0,1,1,1,1,1,0,1,},
    {1,0,0,0,0,0,0,0,1,1,},{1,1,0,0,1,0,0,0,1,1,},{1,0,1,0,1,0,0,0,1,1,},
    {1,1,1,1,1,0,0,0,1,1,},{1,1,0,0,0,1,0,0,1,1,},{1,0,0,0,1,1,0,0,1,1,},
    {1,1,0,1,1,1,0,0,1,1,},{1,0,0,1,0,0,1,0,1,1,},{1,1,1,1,0,0,1,0,1,1,},
    {1,1,0,1,1,0,1,0,1,1,},{1,0,0,0,0,1,1,0,1,1,},{1,1,0,1,0,1,1,0,1,1,},
    {1,0,1,1,0,1,1,0,1,1,},{1,1,0,0,1,1,1,0,1,1,},{1,1,1,1,1,1,1,0,1,1,},
    {1,0,1,0,0,0,0,1,1,1,},{1,1,1,1,0,0,0,1,1,1,},{1,0,0,0,0,1,0,1,1,1,},
    {1,0,1,0,1,1,0,1,1,1,},{1,0,0,1,1,1,0,1,1,1,},{1,1,1,0,0,0,1,1,1,1,},
    {1,1,0,1,0,0,1,1,1,1,},{1,0,1,1,0,0,1,1,1,1,},{1,0,1,0,1,0,1,1,1,1,},
    {1,0,0,1,1,0,1,1,1,1,},{1,1,0,0,0,1,1,1,1,1,},{1,0,0,1,0,1,1,1,1,1,},
    {1,1,0,1,1,1,1,1,1,1,},
};

static void MakepreModPolys(prescr_t *pre, int mm, int nn, int rr, int ww);
static Polynomial *make_tntm( int n, int m);
static Polynomial *PolynomialDup(Polynomial *pl);
static void PolynomialMod(Polynomial *wara, const Polynomial *waru);
static Polynomial *PolynomialMult(Polynomial *p0, Polynomial *p1);
static void FreePoly( Polynomial *p);
static Polynomial *NewPoly(int degree);
static int IsReducible(prescr_t *pre, uint32_t aaa, uint32_t *polylist);
static uint32_t word2bit(Polynomial *pl);
static void makemodlist(prescr_t *pre, Polynomial *pl, int nPoly);
static void NextIrredPoly(Polynomial *pl, int nth);


int _prescreening_dc(prescr_t *pre, uint32_t aaa) {

  int i;

  for (i=0; i<NIRREDPOLY; i++) {
    if (IsReducible(pre, aaa,pre->modlist[i])==REDU)
      return REJECTED;
  }
  return NOT_REJECTED;
}

void _InitPrescreening_dc(prescr_t *pre, int m, int n, int r, int w) {
  int i;
  Polynomial *pl;

  pre->sizeofA = w;

  pre->preModPolys = (Polynomial **)malloc(
                                           (pre->sizeofA+1)*(sizeof(Polynomial*)));
  if (NULL == pre->preModPolys) {
    taMisc::Error ("malloc error in \"InitPrescreening\"");
    exit(1);
  }
  MakepreModPolys(pre, m,n,r,w);

  pre->modlist = (uint32_t**)malloc(NIRREDPOLY * sizeof(uint32_t*));
  if (NULL == pre->modlist) {
    taMisc::Error ("malloc error in \"InitPrescreening()\"");
    exit(1);
  }
  for (i=0; i<NIRREDPOLY; i++) {
    pre->modlist[i]
      = (uint32_t*)malloc( (pre->sizeofA + 1) * (sizeof(uint32_t)) );
    if (NULL == pre->modlist[i]) {
      taMisc::Error ("malloc error in \"InitPrescreening()\"");
      exit(1);
    }
  }


  for (i=0; i<NIRREDPOLY; i++) {
    pl = NewPoly(MAX_IRRED_DEG);
    NextIrredPoly(pl,i);
    makemodlist(pre, pl, i);
    FreePoly(pl);
  }

  for (i=pre->sizeofA; i>=0; i--)
    FreePoly(pre->preModPolys[i]);
  free(pre->preModPolys);

}

void _EndPrescreening_dc(prescr_t *pre) {
  int i;

  for (i=0; i<NIRREDPOLY; i++)
    free(pre->modlist[i]);
  free(pre->modlist);
}

/*************************************************/
/******          static functions           ******/
/*************************************************/

void NextIrredPoly(Polynomial *pl, int nth) {
  int i, max_deg;

  for (max_deg=0,i=0; i<=MAX_IRRED_DEG; i++) {
    if ( irredpolylist[nth][i] )
      max_deg = i;
    pl->x[i] = irredpolylist[nth][i];
  }

  pl->deg = max_deg;

}

static void makemodlist(prescr_t *pre, Polynomial *pl, int nPoly) {
  Polynomial *tmpPl;
  int i;

  for (i=0; i<=pre->sizeofA; i++) {
    tmpPl = PolynomialDup(pre->preModPolys[i]);
    PolynomialMod(tmpPl,pl);
    pre->modlist[nPoly][i] = word2bit(tmpPl);
    FreePoly(tmpPl);
  }
}

/* Pack Polynomial into a word */
static uint32_t word2bit(Polynomial *pl) {
  int i;
  uint32_t bx;

  bx = 0;
  for (i=pl->deg; i>0; i--) {
    if (pl->x[i]) bx |= 0x1;
    bx <<= 1;
  }
  if (pl->x[0]) bx |= 0x1;

  return bx;
}

/* REDU -- reducible */
/* aaa = (a_{w-1}a_{w-2}...a_1a_0 */
static int IsReducible(prescr_t *pre, uint32_t aaa, uint32_t *polylist) {
  int i;
  uint32_t x;

  x = polylist[pre->sizeofA];
  for (i=pre->sizeofA-1; i>=0; i--) {
    if (aaa&0x1)
      x ^= polylist[i];
    aaa >>= 1;
  }

  if ( x == 0 ) return REDU;
  else return NONREDU;
}

/***********************************/
/**   functions for polynomial    **/
/***********************************/

static Polynomial *NewPoly(int degree) {
  Polynomial *p;

  p = (Polynomial *)calloc( 1, sizeof(Polynomial));
  if( p==NULL ){
    taMisc::Error("calloc error in \"NewPoly()\"");
    exit(1);
  }
  p->deg = degree;

  if (degree < 0) {
    p->x = NULL;
    return p;
  }

  p->x = (int *)calloc( degree + 1, sizeof(int));
  if( p->x == NULL ){
    taMisc::Error("calloc error");
    exit(1);
  }

  return p;
}

static void FreePoly( Polynomial *p) {
  if (p->x != NULL)
    free( p->x );
  free( p );
}


/** multiplication **/
static Polynomial *PolynomialMult(Polynomial *p0,Polynomial *p1) {
  int i, j;
  Polynomial *p;

  /* if either p0 or p1 is 0, return 0 */
  if ( (p0->deg < 0) || (p1->deg < 0) ) {
    p = NewPoly(-1);
    return p;
  }

  p = NewPoly(p0->deg + p1->deg);
  for( i=0; i<=p1->deg; i++){
    if( p1->x[i] ){
      for( j=0; j<=p0->deg; j++){
        p->x[i+j] ^= p0->x[j];
      }
    }
  }

  return p;
}

/** wara mod waru **/
/** the result is stored in wara ********/
static void PolynomialMod( Polynomial *wara, const Polynomial *waru) {
  int i;
  int deg_diff;

  while( wara->deg >= waru->deg  ){
    deg_diff = wara->deg - waru->deg;
    for( i=0; i<=waru->deg; i++){
      wara->x[ i+deg_diff ] ^= waru->x[i];
    }

    for( i=wara->deg; i>=0; i--){
      if( wara->x[i] ) break;
    }
    wara->deg=i;

  }
}

static Polynomial *PolynomialDup(Polynomial *pl) {
  Polynomial *pt;
  int i;

  pt = NewPoly(pl->deg);
  for (i=pl->deg; i>=0; i--)
    pt->x[i] = pl->x[i];

  return pt;
}

/** make the polynomial  "t**n + t**m"  **/
static Polynomial *make_tntm( int n, int m) {
  Polynomial *p;

  p = NewPoly(n);
  p->x[n] = p->x[m] = 1;

  return p;
}

static void MakepreModPolys(prescr_t *pre, int mm, int nn, int rr, int ww) {
  Polynomial *t, *t0, *t1, *s, *s0, *s1;
  int i,j;

  j = 0;
  t = NewPoly(0);
  t->deg = 0;
  t->x[0] = 1;
  pre->preModPolys[j++] = t;

  t = make_tntm (nn, mm);
  t0 = make_tntm (nn, mm);
  s = make_tntm (nn-1, mm-1);

  for( i=1; i<(ww - rr); i++){
    pre->preModPolys[j++] = PolynomialDup(t0);
    t1 = t0;
    t0 = PolynomialMult(t0, t);
    FreePoly(t1);
  }

  pre->preModPolys[j++] = PolynomialDup(t0);

  s0 =PolynomialMult( t0, s);
  FreePoly(t0);	FreePoly(t);
  for( i=(rr-2); i>=0; i--){
    pre->preModPolys[j++] = PolynomialDup(s0);
    s1 = s0;
    s0 = PolynomialMult( s0, s);
    FreePoly(s1);
  }

  pre->preModPolys[j++] = PolynomialDup(s0);

  FreePoly(s0); FreePoly(s);
}

/////////////////////////////////////////////////////////////////
//              eqdeg.c 

/**************************************/
#define SSS 7
#define TTT 15
/* #define S00 11 */
#define S00 12
#define S01 18
/**************************************/

/** for get_tempering_parameter_hard **/
#define LIMIT_V_BEST_OPT 15
/**************************************/

#define WORD_LEN 32
#define MIN_INFINITE (-2147483647-1)

typedef struct {
  uint32_t *cf;  /* fraction part */              // status
  int start;     /* beginning of fraction part */ // idx
  int count;	   /* maximum (degree) */
  uint32_t next; /* (bp) rm (shifted&bitmasked) at the maximum degree */
} Vector;

typedef struct mask_node{
  uint32_t b,c;
  int v,leng;
  struct mask_node *next;
} MaskNode;

static inline uint32_t trnstmp(eqdeg_t *eq, uint32_t tmp) {
  tmp ^= (tmp >> eq->shift_0) & eq->greal_mask;
  return tmp;
}

static inline uint32_t masktmp(eqdeg_t *eq, uint32_t tmp) {
  tmp ^= (tmp << eq->shift_s) & eq->mask_b;
  tmp ^= (tmp << eq->shift_t) & eq->mask_c;
  return tmp;
}

static inline uint32_t lsb(eqdeg_t *eq, uint32_t x) {
  return (x >> eq->ggap) & 1;
}

static const uint8_t pivot_calc_tbl[256] = {
    0, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
    4, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
    3, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
    4, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
    2, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
    4, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
    3, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
    4, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
    1, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
    4, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
    3, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
    4, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
    2, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
    4, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
    3, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
    4, 8, 7, 8, 6, 8, 7, 8, 5, 8, 7, 8, 6, 8, 7, 8,
};

static int calc_pivot(uint32_t v);
static int push_stack(eqdeg_t *eq, uint32_t b, uint32_t c,
		      int v, uint32_t *bbb, uint32_t *ccc);
static int push_mask(eqdeg_t * eq, int l, int v,
		     uint32_t b, uint32_t c, uint32_t *bbb, uint32_t *ccc);
static int pivot_reduction(eqdeg_t *eq, int v);
static void init_tempering(eqdeg_t *eq, MTRndPar *mts);
static void free_Vector( Vector *v );
static void free_lattice( Vector **lattice, int v);
static void add(int nnn, Vector *u, Vector *v);
static void optimize_v(eqdeg_t *eq, uint32_t b, uint32_t c, int v);
static MaskNode *optimize_v_hard(eqdeg_t *eq, int v, MaskNode *prev);
static Vector *new_Vector(int nnn);
static Vector **make_lattice(eqdeg_t *eq, int v);
static void delete_MaskNodes(MaskNode *head);
static MaskNode *delete_lower_MaskNodes(MaskNode *head, int l);
static MaskNode *cons_MaskNode(MaskNode *head, uint32_t b, uint32_t c, int leng);
/* static void count_MaskNodes(MaskNode *head); */
static void next_state(eqdeg_t *eq, Vector *v, int *count);

void _get_tempering_parameter_dc(MTRndPar *mts) {
  eqdeg_t eq;
  init_tempering(&eq, mts);
  optimize_v(&eq, 0, 0, 0);
  mts->shift0 = eq.shift_0;
  mts->shift1 = eq.shift_1;
  mts->shiftB = eq.shift_s;
  mts->shiftC = eq.shift_t;
  mts->maskB = eq.mask_b >> eq.ggap;
  mts->maskC = eq.mask_c >> eq.ggap;
}

void _get_tempering_parameter_hard_dc(MTRndPar *mts) {
  int i;
  MaskNode mn0, *cur, *next;
  eqdeg_t eq;

  init_tempering(&eq, mts);

  for (i=0; i<eq.www; i++)
    eq.gcur_maxlengs[i] = -1;

  mn0.b = mn0.c = mn0.leng = 0;
  mn0.next = NULL;

  cur = &mn0;
  for (i=0; i<LIMIT_V_BEST_OPT; i++) {
    next = optimize_v_hard(&eq, i, cur);
    if (i > 0)
      delete_MaskNodes(cur);
    cur = next;
  }
  delete_MaskNodes(cur);

  optimize_v(&eq, eq.gmax_b, eq.gmax_c,i);
  mts->shift0 = eq.shift_0;
  mts->shift1 = eq.shift_1;
  mts->shiftB = eq.shift_s;
  mts->shiftC = eq.shift_t;
  mts->maskB = eq.mask_b >> eq.ggap;
  mts->maskC = eq.mask_c >> eq.ggap;
}

static int calc_pivot(uint32_t v) {
  int p1, p2, p3, p4;

  p1 = pivot_calc_tbl[v & 0xff];
  if (p1) {
    return p1 + 24 - 1;
  }
  p2 = pivot_calc_tbl[(v >> 8) & 0xff];
  if (p2) {
    return p2 + 16 - 1;
  }
  p3 = pivot_calc_tbl[(v >> 16) & 0xff];
  if (p3) {
    return p3 + 8 - 1;
  }
  p4 = pivot_calc_tbl[(v >> 24) & 0xff];
  if (p4) {
    return p4 - 1;
  }
  return -1;
}

static int is_zero(int size, Vector *v) {
  if (v->cf[0] != 0) {
    return 0;
  } else {
    return (memcmp(v->cf, v->cf + 1, sizeof(uint32_t) * (size - 1)) == 0);
  }
}

static void init_tempering(eqdeg_t *eq, MTRndPar *mts) {
  int i;

  eq->mmm = mts->mm;
  eq->nnn = mts->nn;
  eq->rrr = mts->rr;
  eq->www = mts->ww;
  eq->shift_0 = S00;
  eq->shift_1 = S01;
  eq->shift_s = SSS;
  eq->shift_t = TTT;
  eq->ggap = WORD_LEN - eq->www;
  /* bits are filled in mts->aaa from MSB */
  eq->aaa[0] = 0; eq->aaa[1] = (mts->aaa) << eq->ggap;


  for( i=0; i<WORD_LEN; i++)
    eq->bitmask[i] = 0x80000000 >> i;

  for( i=0, eq->glower_mask=0; i<eq->rrr; i++)
    eq->glower_mask = (eq->glower_mask<<1)| 0x1;

  eq->gupper_mask = ~eq->glower_mask;
  eq->gupper_mask <<= eq->ggap;
  eq->glower_mask <<= eq->ggap;

  eq->greal_mask = (eq->gupper_mask | eq->glower_mask);
}

/* (v-1) bitmasks of b,c */
static MaskNode *optimize_v_hard(eqdeg_t *eq, int v, MaskNode *prev_masks) {
  int i, ll, t;
  uint32_t bbb[8], ccc[8];
  MaskNode *cur_masks;

  cur_masks = NULL;

  while (prev_masks != NULL) {

    ll = push_stack(eq, prev_masks->b,prev_masks->c,v,bbb,ccc);

    for (i=0; i<ll; ++i) {
      eq->mask_b = bbb[i];
      eq->mask_c = ccc[i];
      t = pivot_reduction(eq, v+1);
      if (t >= eq->gcur_maxlengs[v]) {
        eq->gcur_maxlengs[v] = t;
        eq->gmax_b = eq->mask_b;
        eq->gmax_c = eq->mask_c;
        cur_masks = cons_MaskNode(cur_masks, eq->mask_b, eq->mask_c, t);
      }
    }
    prev_masks = prev_masks->next;
  }

  cur_masks = delete_lower_MaskNodes(cur_masks, eq->gcur_maxlengs[v]);

  return cur_masks;
}


/* (v-1) bitmasks of b,c */
static void optimize_v(eqdeg_t *eq, uint32_t b, uint32_t c, int v) {
  int i, max_len, max_i, ll, t;
  uint32_t bbb[8], ccc[8];

  ll = push_stack(eq, b,c,v,bbb,ccc);

  max_len = max_i = 0;
  if (ll > 1) {
    for (i=0; i<ll; ++i) {
      eq->mask_b = bbb[i];
      eq->mask_c = ccc[i];
      t = pivot_reduction(eq, v+1);
      if (t > max_len) {
        max_len = t;
        max_i = i;
      }
    }
  }

  if ( v >= eq->www-1 ) {
    eq->mask_b = bbb[max_i];
    eq->mask_c = ccc[max_i];
    return;
  }

  optimize_v(eq, bbb[max_i], ccc[max_i], v+1);
}

static int push_stack(eqdeg_t *eq, uint32_t b, uint32_t c, int v,
		      uint32_t *bbb, uint32_t *ccc)
{
  int i, ll, ncv;
  uint32_t cv_buf[2];

  ll = 0;

  if( (v+eq->shift_t) < eq->www ){
    ncv = 2; cv_buf[0] = c | eq->bitmask[v]; cv_buf[1] = c;
  }
  else {
    ncv = 1; cv_buf[0] = c;
  }

  for( i=0; i<ncv; ++i)
    ll += push_mask(eq, ll, v, b, cv_buf[i], bbb, ccc);

  return ll;
}

static int push_mask(eqdeg_t *eq, int l, int v, uint32_t b, uint32_t c,
		     uint32_t *bbb, uint32_t *ccc)
{
  int i, j, k, nbv, nbvt;
  uint32_t bmask, bv_buf[2], bvt_buf[2];

  k = l;
  if( (eq->shift_s+v) >= eq->www ){
    nbv = 1; bv_buf[0] = 0;
  }
  else if( (v>=eq->shift_t) && (c&eq->bitmask[v-eq->shift_t] ) ){
    nbv = 1; bv_buf[0] = b&eq->bitmask[v];
  }
  else {
    nbv = 2; bv_buf[0] = eq->bitmask[v]; bv_buf[1] = 0;
  }

  if( ((v+eq->shift_t+eq->shift_s) < eq->www) && (c&eq->bitmask[v]) ){
    nbvt = 2; bvt_buf[0] = eq->bitmask[v+eq->shift_t]; bvt_buf[1] = 0;
  }
  else {
    nbvt = 1; bvt_buf[0] = 0;
  }

  bmask = eq->bitmask[v];
  if( (v+eq->shift_t) < eq->www )
    bmask |= eq->bitmask[v+eq->shift_t];
  bmask = ~bmask;
  for( i=0; i<nbvt; ++i){
    for( j=0; j<nbv; ++j){
      bbb[k] = (b&bmask) | bv_buf[j] | bvt_buf[i];
      ccc[k] = c;
      ++k;
    }
  }

  return k-l;
}

/**********************************/
/****  subroutines for lattice ****/
/**********************************/
static int pivot_reduction(eqdeg_t *eq, int v) {
  Vector **lattice, *ltmp;
  int i;
  int pivot;
  int count;
  int min;

  eq->upper_v_bits = 0;
  for( i=0; i<v; i++) {
    eq->upper_v_bits |= eq->bitmask[i];
  }

  lattice = make_lattice(eq, v );

  for (;;) {
    pivot = calc_pivot(lattice[v]->next);
    if (lattice[pivot]->count < lattice[v]->count) {
      ltmp = lattice[pivot];
      lattice[pivot] = lattice[v];
      lattice[v] = ltmp;
    }
    add(eq->nnn, lattice[v], lattice[pivot]);
    if (lattice[v]->next == 0) {
      count = 0;
      next_state(eq, lattice[v], &count);
      if (lattice[v]->next == 0) {
        if (is_zero(eq->nnn, lattice[v])) {
          break;
        }
        while (lattice[v]->next == 0) {
          count++;
          next_state(eq, lattice[v], &count);
          if (count > eq->nnn * (eq->www-1) - eq->rrr) {
            break;
          }
        }
        if (lattice[v]->next == 0) {
          break;
        }
      }
    }
  }

  min = lattice[0]->count;
  for (i = 1; i < v; i++) {
    if (min > lattice[i]->count) {
      min = lattice[i]->count;
    }
  }
  free_lattice( lattice, v );
  return min;
}

/********************************/
/** allocate momory for Vector **/
/********************************/
static Vector *new_Vector(int nnn) {
  Vector *v;

  v = (Vector *)malloc( sizeof( Vector ) );
  if( v == NULL ){
    taMisc::Error("malloc error in \"new_Vector()\"");
    exit(1);
  }

  v->cf = (uint32_t *)calloc( nnn, sizeof( uint32_t ) );
  if( v->cf == NULL ){
    taMisc::Error("calloc error in \"new_Vector()\"");
    exit(1);
  }

  v->start = 0;

  return v;
}

/************************************************/
/* frees *v which was allocated by new_Vector() */
/************************************************/
static void free_Vector( Vector *v ) {
  if( NULL != v->cf ) free( v->cf );
  if( NULL != v ) free( v );
}

static void free_lattice( Vector **lattice, int v) {
  int i;

  for( i=0; i<=v; i++)
    free_Vector( lattice[i] );
  free( lattice );
}

/* adds v to u (then u will change) */
static void add(int nnn, Vector *u, Vector *v) {
  int i;
  int diff = (v->start - u->start + nnn) % nnn;
  for (i = 0; i < nnn - diff; i++) {
    u->cf[i] ^= v->cf[i + diff];
  }
  diff = diff - nnn;
  for (; i < nnn; i++) {
    u->cf[i] ^= v->cf[i + diff];
  }
  u->next ^=  v->next;
}

/* makes a initial lattice */
static Vector **make_lattice(eqdeg_t *eq, int v) {
  int i;
  int count;
  Vector **lattice, *bottom;

  lattice = (Vector **)malloc( (v+1) * sizeof( Vector *) );
  if( NULL == lattice ){
    taMisc::Error("malloc error in \"make_lattice\"");
    exit(1);
  }

  for( i=0; i<v; i++){ /* from 0th row to v-1-th row */
    lattice[i] = new_Vector(eq->nnn);
    lattice[i]->next = eq->bitmask[i];
    lattice[i]->start = 0;
    lattice[i]->count = 0;
  }

  bottom = new_Vector(eq->nnn); /* last row */
  for(i=0; i< eq->nnn; i++) {
    bottom->cf[i] = 0;
  }
  bottom->cf[eq->nnn -1] = 0xc0000000 & eq->greal_mask;
  bottom->start = 0;
  bottom->count = 0;
  count = 0;
  do {
    next_state(eq, bottom, &count);
  } while (bottom->next == 0);
  //    degree_of_vector(eq, top );
  lattice[v] = bottom;

  return lattice;
}

static void next_state(eqdeg_t *eq, Vector *v, int *count) {
  uint32_t tmp;

  do {
    tmp = ( v->cf[v->start] & eq->gupper_mask )
      | ( v->cf[(v->start + 1) % eq->nnn] & eq->glower_mask );
    v->cf[v->start] = v->cf[(v->start + eq->mmm) % eq->nnn]
      ^ ( (tmp>>1) ^ eq->aaa[lsb(eq, tmp)] );
    v->cf[v->start] &= eq->greal_mask;
    tmp = v->cf[v->start];
    v->start = (v->start + 1) % eq->nnn;
    v->count++;
    tmp = trnstmp(eq, tmp);
    tmp = masktmp(eq, tmp);
    v->next = tmp & eq->upper_v_bits;
    (*count)++;
    if (*count > eq->nnn * (eq->www-1) - eq->rrr) {
      break;
    }
  } while (v->next == 0);
}

/***********/
static MaskNode *cons_MaskNode(MaskNode *head, uint32_t b, uint32_t c, int leng) {
  MaskNode *t;

  t = (MaskNode*)malloc(sizeof(MaskNode));
  if (t == NULL) {
    taMisc::Error("malloc error in \"cons_MaskNode\"");
    exit(1);
  }

  t->b = b;
  t->c = c;
  t->leng = leng;
  t->next = head;

  return t;
}

static void delete_MaskNodes(MaskNode *head) {
  MaskNode *t;

  while(head != NULL) {
    t = head->next;
    free(head);
    head = t;
  }
}

static MaskNode *delete_lower_MaskNodes(MaskNode *head, int l) {
  MaskNode *s, *t, *tail;

  s = head;
  while(1) { /* heading */
    if (s == NULL)
      return NULL;
    if (s->leng >= l)
      break;
    t = s->next;
    free(s);
    s = t;
  }

  head = tail = s;

  while (head != NULL) {
    t = head->next;
    if (head->leng < l) {
      free(head);
    }
    else {
      tail->next = head;
      tail = head;
    }
    head = t;
  }

  tail->next = NULL;
  return s;
}

/////////////////////////////////////////////////////////////////////
//              seive.c -- main code


#define MAX_SEARCH 10000

_org_state global_mt19937;

/*******************************************************************/
static uint32_t nextA(_org_state *org, int w);
static uint32_t nextA_id(_org_state *org, int w, int id, int idw);
static void make_masks(int r, int w, MTRndPar *mts);
static int get_irred_param(check32_t *ck, prescr_t *pre, _org_state *org,
			   MTRndPar *mts,int id, int idw);
static bool init_mt_search(MTRndPar* mts, check32_t *ck, prescr_t *pre, int w, int p);
static void end_mt_search(prescr_t *pre);
static void copy_params_of_MTRndPar(MTRndPar *src, MTRndPar *dst);
static int proper_mersenne_exponent(int p);
/*******************************************************************/

/* When idw==0, id is not embedded into "a" */
#define FOUND 1
#define NOT_FOUND 0
static int get_irred_param(check32_t *ck, prescr_t *pre, _org_state *org,
			   MTRndPar *mts, int id, int idw)
{
  int i;
  uint32_t a;

  for (i=0; i<MAX_SEARCH; i++) {
    if (idw == 0)
      a = nextA(org, mts->ww);
    else
      a = nextA_id(org, mts->ww, id, idw);
    if (NOT_REJECTED == _prescreening_dc(pre, a) ) {
      if (IRRED
          == _CheckPeriod_dc(ck, org, a,mts->mm,mts->nn,mts->rr,mts->ww)) {
        mts->aaa = a;
        break;
      }
    }
  }

  if (MAX_SEARCH == i) {
    taMisc::Error("was not able to find a good parameter for id:", String(id), "bailing!");
    return NOT_FOUND;
  }
  return FOUND;
}


static uint32_t nextA(_org_state *org, int w) {
  uint32_t x, word_mask;

  word_mask = 0xFFFFFFFF;
  word_mask <<= WORDLEN - w;
  word_mask >>= WORDLEN - w;

  x = _genrand_dc(org);
  x &= word_mask;
  x |= (LSB << (w-1));

  return x;
}

static uint32_t nextA_id(_org_state *org, int w, int id, int idw) {
  uint32_t x, word_mask;

  word_mask = 0xFFFFFFFF;
  word_mask <<= WORDLEN - w;
  word_mask >>= WORDLEN - w;
  word_mask >>= idw;
  word_mask <<= idw;

  x = _genrand_dc(org);
  x &= word_mask;
  x |= (LSB << (w-1));
  x |= (uint32_t)id; /* embedding id */

  return x;
}

static void make_masks(int r, int w, MTRndPar *mts) {
  int i;
  uint32_t ut, wm, um, lm;

  wm = 0xFFFFFFFF;
  wm >>= (WORDLEN - w);

  ut = 0;
  for (i=0; i<r; i++) {
    ut <<= 1;
    ut |= LSB;
  }

  lm = ut;
  um = (~ut) & wm;

  mts->wmask = wm;
  mts->umask = um;
  mts->lmask = lm;
}

static bool init_mt_search(MTRndPar *mts, check32_t *ck, prescr_t *pre, int w, int p) {
  int n, m, r;

  if ( (w>32) || (w<31) ) {
    taMisc::Error ("Sorry, currently only w = 32 or 31 is allowded.");
    return false;
  }

  if ( !proper_mersenne_exponent(p) ) {
    if (p<521) {
      taMisc::Error ("\"p\" is too small.");
      return false;
    }
    else if (p>44497){
      taMisc::Error ("\"p\" is too large.");
      return false;
    }
    else {
      taMisc::Error ("\"p\" is not a Mersenne exponent.");
      return false;
    }
  }

  n = p/w + 1; /* since p is Mersenne Exponent, w never divids p */

  m = n/2;
  if (m < 2) m = n-1;
  r = n * w - p;

  make_masks(r, w, mts);
  _InitPrescreening_dc(pre, m, n, r, w);
  _InitCheck32_dc(ck, r, w);

  mts->mm = m;
  mts->nn = n;
  mts->rr = r;
  mts->ww = w;

  return true;
}

static void end_mt_search(prescr_t *pre) {
  _EndPrescreening_dc(pre);
}

static void copy_params_of_MTRndPar(MTRndPar *src, MTRndPar *dst) {
  dst->nn = src->nn;
  dst->mm = src->mm;
  dst->rr = src->rr;
  dst->ww = src->ww;
  dst->wmask = src->wmask;
  dst->umask = src->umask;
  dst->lmask = src->lmask;
}

static int proper_mersenne_exponent(int p) {
  switch(p) {
  case 521:
  case 607:
  case 1279:
  case 2203:
  case 2281:
  case 3217:
  case 4253:
  case 4423:
  case 9689:
  case 9941:
  case 11213:
  case 19937:
  case 21701:
  case 23209:
  case 44497:
    return 1;
  default:
    return 0;
  }
}


////////////////////////////////////////////////////////////////
//              Main Gen Code

void MTRndPar::Initialize() {
  aaa = 0;
  mm = 0;
  nn = 0;
  rr = 0;
  ww = 0;
  wmask = 0;
  umask = 0;
  lmask = 0;
  shift0 = 0;
  shift1 = 0;
  shiftB = 0;
  shiftC = 0;
  maskB = 0;
  maskC = 0;
  mti = 0;
  double_cached = false;
  cached_double = 0.0f;

  state = NULL;
}

void MTRndPar::Destroy() {
  if(state != NULL) {
    delete[] state;
    state = NULL;
  }
}

void MTRndPar::AllocState() {
  state = new uint32_t[nn];
}

/*
   w -- word size
   p -- Mersenne Exponent
   seed -- seed for original mt19937 to generate parameter.
*/
bool MTRndPar::GenerateParams(int w, int p, uint32_t seed) {
  prescr_t pre;
  _org_state org;
  check32_t ck;

  _sgenrand_dc(&org, seed);
  bool rval = init_mt_search(this, &ck, &pre, w, p);
  if(!rval) return rval;

  if ( NOT_FOUND == get_irred_param(&ck, &pre, &org, this, 0,0) ) {
    return false;
  }
  _get_tempering_parameter_hard_dc(this);
  end_mt_search(&pre);
  return true;
}

/*
   w -- word size
   p -- Mersenne Exponent
*/
#define DEFAULT_ID_SIZE 16
/* id <= 0xffff */

bool MTRndPar::GenerateParamsID(int w, int p, int id, uint32_t seed) {
  prescr_t pre;
  _org_state org;
  check32_t ck;

  _sgenrand_dc(&org, seed);
  if (id > 0xffff) {
    taMisc::Error("\"id\" must be less than 65536");
    return false;
  }
  if (id < 0) {
    taMisc::Error("\"id\" must be positive");
    return false;
  }

  bool rval = init_mt_search(this, &ck, &pre, w, p);
  if (!rval) return false;

  if ( NOT_FOUND == get_irred_param(&ck, &pre, &org,
                                    this, id, DEFAULT_ID_SIZE) ) {
    return false;
  }
  _get_tempering_parameter_hard_dc(this);
  end_mt_search(&pre);
  return true;
}

////////////////////////////////////////////////////////////////
//              Actually use params to generate numbers!

void MTRndPar::InitSeed(uint32_t seed) {
  if(!state) {
    AllocState();
  }

  int i;
  for (i=0; i<nn; i++) {
    state[i] = seed;
    seed = (1812433253 * (seed  ^ (seed >> 30))) + i + 1;
    /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
    /* In the previous versions, MSBs of the seed affect   */
    /* only MSBs of the array mt[].                        */
  }
  mti = nn;

  for (i=0; i<nn; i++)
    state[i] &= wmask;
}

uint32_t MTRndPar::GenRandInt32() {
  if(!state) {
    AllocState();
  }
  
  uint32_t *st, uuu, lll, aa, x;
  int k,n,m,lim;

  if ( mti >= nn ) {
    n = nn; m = mm;
    aa = aaa;
    st = state;
    uuu = umask; lll = lmask;

    lim = n - m;
    for (k=0; k<lim; k++) {
      x = (st[k]&uuu)|(st[k+1]&lll);
      st[k] = st[k+m] ^ (x>>1) ^ (x&1U ? aa : 0U);
    }
    lim = n - 1;
    for (; k<lim; k++) {
      x = (st[k]&uuu)|(st[k+1]&lll);
      st[k] = st[k+m-n] ^ (x>>1) ^ (x&1U ? aa : 0U);
    }
    x = (st[n-1]&uuu)|(st[0]&lll);
    st[n-1] = st[m-1] ^ (x>>1) ^ (x&1U ? aa : 0U);
    mti=0;
  }

  x = state[mti];
  mti += 1;
  x ^= x >> shift0;
  x ^= (x << shiftB) & maskB;
  x ^= (x << shiftC) & maskC;
  x ^= x >> shift1;

  return x;
}

double MTRndPar::GenRandGaussDev() {
  if(double_cached) {
    double_cached = false;
    return cached_double;
  }

  double fac, r, v1, v2;
  do {
    v1 = 2.0 * GenRandRes53() - 1.0;
    v2 = 2.0 * GenRandRes53() - 1.0;
    r = v1 * v1 + v2 * v2;
  }
  while (r >= 1.0 || r == 0);

  fac = sqrt(-2.0 * log(r) / r);
  cached_double = v1 * fac;
  double_cached = true;
  return v2 * fac;
}

uint32_t MTRndPar::GetCurSeed() {
  if(!state) {
    return 0;
  }
  return state[mti];
}


////////////////////////////////
//      MTRndPar_List

bool MTRndPar_List::GenerateParamsID(int w, int p, int n_ids, uint32_t seed) {
  SetSize(n_ids);

  prescr_t pre;
  _org_state org;
  check32_t ck;

  _sgenrand_dc(&org, seed);

  MTRndPar template_mts;
  bool rval = init_mt_search(&template_mts, &ck, &pre, w, p);
  if (!rval) return false;

  int id = 0;
  int count = 0;
  while(count < n_ids) {
    MTRndPar* mts = FastEl(count);
    copy_params_of_MTRndPar(&template_mts, mts);

    if ( NOT_FOUND == get_irred_param(&ck, &pre, &org, mts,
                                      id, DEFAULT_ID_SIZE) ) {
      id++;
      continue;
    }
    _get_tempering_parameter_hard_dc(mts);
    taMisc::Info("successfully completed params for MT PRNG id:", String(id),
                 "count:", String(count));
    id++;
    count++;
  }

  end_mt_search(&pre);
  return rval;
}

void MTRndPar_List::InitSeeds(uint32_t seed) {
  for(int i=0; i<size; i++) {
    FastEl(i)->InitSeed(seed);
  }
}

////////////////////////////////
//      MTRnd

const int MTRnd::max_gens = 100;
MTRndPar_List MTRnd::mtrnds;

void MTRnd::Initialize() {
}

void MTRnd::Destroy() {
}

MTRndPar* MTRnd::GetRnd(int thr_no) {
  if(thr_no >= mtrnds.size || thr_no < 0) {
    taMisc::Error("MTRnd: thread number:", String(thr_no),
                  "out of range for number of parallel RNG's:", String(mtrnds.size));
    return NULL;
  }
  return mtrnds.FastEl(thr_no);
}
    
void MTRnd::InitSeeds(uint32_t seed) {
  mtrnds.InitSeeds(seed);
}

uint32_t MTRnd::GetTimePidSeed() {
  int pid = taMisc::ProcessId();
  int tc = taMisc::TickCount(); // ms since system started
  ulong sdval = (ulong)tc * (ulong)pid;
  uint32_t rval = sdval & 0xffffffffUL;
  return rval;
}

void MTRnd::GenInitParams(int n_gens, const String& save_file_name) {
  // int prime = 521;     // for testing -- runs in a few seconds
  // int prime = 4423;    // takes under an hour for 100
  // int prime = 9941;    // takes several hours
  int prime = 19937;   // this takes several days

  taMisc::Warning("This can take quite a long time -- be patient or kill it!!!  Using  prime value:", String(prime), "(19937 takes several days for 100, computation is O(prime^3)) -- generating a total of:", String(n_gens),
                  "generators");

  mtrnds.GenerateParamsID(32, prime, n_gens, GetTimePidSeed());
  String svstr = "static const char* mtdefparams[] = {\n";
  for(int i=0;i<mtrnds.size;i++) {
    MTRndPar* rnd = mtrnds.FastEl(i);
    String vlstr = rnd->GetValStr();
    svstr << "\"" << vlstr << "\",\n";
  }
  svstr << "};\n";
  svstr.SaveToFile(save_file_name);
  taMisc::Info("MTRnd params saved to:", save_file_name);
}

// this is 100 with prime = 4423 for the time being

static const char* mtdefparams[] = {
"{aaa=2435055616: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3840732416: maskC=3754983424: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2764374017: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3019828864: maskC=3723067392: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3528458242: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=2913955712: maskC=3738009600: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3323265027: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3169172352: maskC=4151607296: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3309174788: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3019156864: maskC=3757342720: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2431975429: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3874860672: maskC=3184885760: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2268266502: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3840208128: maskC=3754196992: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3453091847: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=2465642368: maskC=4283924480: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3988127752: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3144347008: maskC=3752689664: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3003777033: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3472305792: maskC=3184852992: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3129212938: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3837717888: maskC=3755442176: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3214016523: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3182357888: maskC=3756359680: mti=17: double_cached=false: cached_double=0: }",
"{aaa=4160421900: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3854364032: maskC=3757408256: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2221735949: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3852922240: maskC=3754786816: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2870607886: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3920590208: maskC=3754754048: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2735734799: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=4093064832: maskC=3202842624: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2447507472: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3975789440: maskC=3077668864: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3487629329: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3145379200: maskC=3757539328: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2291269650: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=2801461888: maskC=3754524672: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3636854803: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3841284992: maskC=2136309760: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3191275540: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3987795328: maskC=3755311104: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3555524629: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3111840640: maskC=3757146112: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2526543894: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3971280256: maskC=3755245568: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3002662935: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=4083919488: maskC=3218178048: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2485387288: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3755308928: maskC=4017455104: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3918987289: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=2780097664: maskC=3755311104: mti=17: double_cached=false: cached_double=0: }",
"{aaa=4228513818: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3974346112: maskC=2142535680: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2684616731: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3145002368: maskC=3753213952: mti=17: double_cached=false: cached_double=0: }",
"{aaa=4183293980: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3442718336: maskC=3203891200: mti=17: double_cached=false: cached_double=0: }",
"{aaa=4029153309: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3988188544: maskC=3755802624: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2729115678: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3690311424: maskC=3176431616: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3806330911: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3069467264: maskC=4292640768: mti=17: double_cached=false: cached_double=0: }",
"{aaa=4105830432: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=4084657536: maskC=3755835392: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3847749665: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=4093569664: maskC=3755573248: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2334588962: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=4092391040: maskC=3721199616: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2943287331: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=2611633024: maskC=4285726720: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2505768996: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=2612836992: maskC=4157374464: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2577924133: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3988188544: maskC=3755835392: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3644981286: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=1991604096: maskC=4023746560: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2966618151: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=4088901248: maskC=3184852992: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2889875496: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=2612227968: maskC=4252303360: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3404660777: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=2783899008: maskC=3752296448: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2405564458: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=4078005888: maskC=3722280960: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2719809579: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3143953792: maskC=3753213952: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2880307244: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3647449728: maskC=3218440192: mti=17: double_cached=false: cached_double=0: }",
"{aaa=4093313069: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3019304320: maskC=3752689664: mti=17: double_cached=false: cached_double=0: }",
"{aaa=4257873966: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3153390976: maskC=3752951808: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2807824431: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3854101888: maskC=3756851200: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2835873840: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3866028672: maskC=3754393600: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2530476081: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3019221632: maskC=4292182016: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2448752690: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3136876160: maskC=3719135232: mti=17: double_cached=false: cached_double=0: }",
"{aaa=4121296947: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3131894912: maskC=3756720128: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3270705204: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=4092915072: maskC=3755507712: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3459776565: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3921131264: maskC=3201630208: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2752446518: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=2603965056: maskC=2144698368: mti=17: double_cached=false: cached_double=0: }",
"{aaa=4123197495: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3152735872: maskC=3721363456: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2350121016: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=2617031552: maskC=4149575680: mti=17: double_cached=false: cached_double=0: }",
"{aaa=4211671097: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=4088508032: maskC=3186065408: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2939813946: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3149720960: maskC=3746922496: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2874409019: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3409948288: maskC=3201531904: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2493317180: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3870666624: maskC=3183837184: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3141861437: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3740118912: maskC=3185541120: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2834759742: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=4088770432: maskC=3177021440: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3723558975: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=2390014592: maskC=4158947328: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2262499392: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3018648960: maskC=3750264832: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2164981825: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3983338752: maskC=3754229760: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3818127426: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3077928704: maskC=3738632192: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3396730947: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3853430144: maskC=3755311104: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3161849924: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=2611764096: maskC=4283924480: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2483617861: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=2310452864: maskC=4292214784: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3121217606: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=4075777792: maskC=3720708096: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3961585735: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3987664256: maskC=3757408256: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2185297992: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=4083477888: maskC=3754491904: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2696806473: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3019173248: maskC=3751051264: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3708682314: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3854364032: maskC=3755245568: mti=17: double_cached=false: cached_double=0: }",
"{aaa=4070309963: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3153915264: maskC=3752624128: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3146186828: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3614947200: maskC=3201794048: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2201223245: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=4005018496: maskC=3075964928: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3756195918: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=2797136768: maskC=3746791424: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3378118735: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3449239424: maskC=3202842624: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3558146128: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3035388544: maskC=4292083712: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2515927121: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=2583452288: maskC=4292116480: mti=17: double_cached=false: cached_double=0: }",
"{aaa=4218552402: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=4021743232: maskC=3722215424: mti=17: double_cached=false: cached_double=0: }",
"{aaa=4135256147: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3484364416: maskC=3183935488: mti=17: double_cached=false: cached_double=0: }",
"{aaa=4217503828: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3468406656: maskC=3178037248: mti=17: double_cached=false: cached_double=0: }",
"{aaa=4288872533: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3446519680: maskC=3201630208: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2731212886: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3971280256: maskC=3755573248: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2490826839: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=2533144448: maskC=4283400192: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2288058456: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=4084657536: maskC=3755573248: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3970039897: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3111578496: maskC=3752001536: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2765226074: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=4084525952: maskC=3749806080: mti=17: double_cached=false: cached_double=0: }",
"{aaa=4111597659: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=4005001088: maskC=3747676160: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2207514716: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3144740096: maskC=3754196992: mti=17: double_cached=false: cached_double=0: }",
"{aaa=4230217821: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3145526656: maskC=3752689664: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2339438686: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3131928192: maskC=3739811840: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2486501471: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3421723520: maskC=2136408064: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3617063008: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3128356736: maskC=3736403968: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3638952033: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3181833600: maskC=3754524672: mti=17: double_cached=false: cached_double=0: }",
"{aaa=2429943906: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=3991727488: maskC=3755507712: mti=17: double_cached=false: cached_double=0: }",
"{aaa=3799515235: mm=69: nn=139: rr=25: ww=32: wmask=4294967295: umask=4261412864: lmask=33554431: shift0=12: shift1=18: shiftB=7: shiftC=15: maskB=2611764096: maskC=4283924480: mti=17: double_cached=false: cached_double=0: }",
};

void MTRnd::LoadInitParams() {
  mtrnds.SetSize(max_gens);
  for(int i=0; i<max_gens; i++) {
    String ldstr = mtdefparams[i];
    GetRnd(i)->SetValStr(ldstr);
  }
  InitSeeds(GetTimePidSeed());
}

