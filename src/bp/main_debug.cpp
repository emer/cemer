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

// this is the main that is included to make pdplib executable

#include "bp.h"
#include <pdp/pdpshell.h>
#include <pdp/netstru_extra.h>
#include <stdio.h>

void ApplyPats(Network* net) {
  Layer* lay;
  taLeafItr li;
  FOR_ITR_EL(Layer, lay, net->layers., li) {
    if(li.i == 0)
      lay->ext_flag = Unit::EXT;
    else if(li.i == net->layers.size-1)
      lay->ext_flag = Unit::TARG;
    else
      continue;
    Unit* un;
    taLeafItr ui;
    FOR_ITR_EL(Unit, un, lay->units., ui) {
      if(li.i == 0) {
	un->ext_flag = Unit::EXT;
	un->ext = MTRnd::genrand_res53();
      }
      else if(li.i==net->layers.size-1) {
	un->ext_flag = Unit::TARG;
	un->targ = MTRnd::genrand_res53();
      }
    }
  }
}

#ifdef NO_TIME
  extern "C" {
    long clock();
  }
#define CLOCKS_PER_SEC 100
#else
#include <time.h>
#endif

#ifndef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 1000000
#endif

int main(int argc, char* argv[]) {
  pdpMisc::Init_Hook = ta_Init_bp;
  pdpMisc::Main(argc, argv);

  if(argc < 2) {
    printf("must have 2 args:\n\t<n_units>\tnumber of units in each of 3 layers\n");
    printf("\t<n_pats>\tnumber of pattern presentations\n");

    return 1;
  }

  int n_units = (int)strtol(argv[1], NULL, 0);
  int n_pats = (int)strtol(argv[2], NULL, 0);
  int n_lays = 3;

  int min_new = 8;

  BpUnitSpec us;
  BpConSpec cs;
  us.bias_spec.spec = &cs;
  FullPrjnSpec ps;

  Network* net;
  Layer* lay;
  taLeafItr li;
  int l;

  net = new Network;
  net->layers.el = (void**)new taBase*[8];
  net->layers.size = n_lays;
  for(l=0; l<n_lays; l++) {
    lay = new Layer;
    net->layers.el[l] = lay;
    if(l > 0) {
      //      lay->projections.el = (void**)new taBase*[min_new];
//       lay->projections.Alloc(min_new);
//       lay->projections.size = 1;
//       Projection* prjn = new Projection;
//       lay->projections.el[0] = prjn;
      lay->projections.NewEl(1, &TA_Projection);
      Projection* prjn = (Projection*)lay->projections[0];
      prjn->layer = lay;
      prjn->from = (Layer*)net->layers.FastEl(l-1);
      prjn->spec.spec = &ps;
    }
  }
  for(l=0; l<n_lays; l++) {
    lay = (Layer*)net->layers[l];
    lay->units.NewEl(n_units, &TA_BpUnit);
    //    lay->units.el = (void**)new taBase*[n_units + min_new];
    //    lay->units.Alloc(n_units + min_new);
    //    lay->units.size = n_units;
    int u;
    for(u=0; u<n_units; u++) {
//       BpUnit* un = new BpUnit;
//       lay->units.el[u] = un;
      BpUnit* un = (BpUnit*)lay->units[u];
      un->spec.spec = &us;
      un->bias = new BpCon;
    }
  }

  // now connect them up
#ifdef OLD
  FOR_ITR_EL(Layer, lay, net->layers., li) {
    Unit* un;
    taLeafItr ui;
    FOR_ITR_EL(Unit, un, lay->units., ui) {
      if(li.i != 2) {
	//	un->send.el = (void**)new taBase*[n_units + min_new];
	un->send.Alloc(n_units + min_new);
	un->send.size = n_units;
	//	un->send.units.el = (void**)new taBase*[n_units + min_new];
	un->send.units.Alloc(n_units + min_new);
	un->send.units.size = n_units;
	Layer* rl = (Layer*)net->layers.el[li.i+1];
	un->send.prjn = (Projection*)rl->projections.FastEl(0);
	un->send.spec.spec = &cs;
	int j;
	for(j=0; j<n_units; j++) {
	  BpCon* con = new BpCon;
	  un->send.el[j] = con;
	  BpUnit* ru = (BpUnit*)rl->units.el[j];
	  un->send.units.el[j] = ru;
	}
      }
      if(li.i > 0) {
	//	un->recv.el = (void**)new taBase*[n_units + min_new];
	un->recv.Alloc(n_units + min_new);
	un->recv.size = n_units;
	//	un->recv.units.el = (void**)new taBase*[n_units + min_new];
	un->recv.units.Alloc(n_units + min_new);
	un->recv.units.size = n_units;
	un->recv.prjn = (Projection*)lay->projections.FastEl(0);
	un->recv.spec.spec = &cs;
	Layer* sl = (Layer*)net->layers.el[li.i-1];
	int j;
	for(j=0; j<n_units; j++) {
	  BpUnit* su = (BpUnit*)sl->units.el[j];
	  BpCon* con = (BpCon*)su->send.Cn(j);
	  un->recv.el[j] = con;
	  un->recv.units.el[j] = su;
	}
      }
    }
  }
#else

  // first pass: allocate
  FOR_ITR_EL(Layer, lay, net->layers., li) {
    Unit* un;
    taLeafItr ui;
    FOR_ITR_EL(Unit, un, lay->units., ui) {
      if(li.i > 0) {
	//	un->recv.NewEl(n_units, &TA_BpCon);
 	un->recv.Alloc(n_units + min_new);
 	un->recv.size = n_units;
	un->recv.units.Alloc(n_units + min_new);
	un->recv.units.size = n_units;
	un->recv.prjn = (Projection*)lay->projections.FastEl(0);
	un->recv.spec.spec = &cs;
      }
      if(li.i != 2) {
	un->send.NewEl(n_units, &TA_BpCon);
// 	un->send.Alloc(n_units + min_new);
// 	un->send.size = n_units;
	un->send.units.Alloc(n_units + min_new);
	un->send.units.size = n_units;
	Layer* rl = (Layer*)net->layers.el[li.i+1];
	un->send.prjn = (Projection*)rl->projections.FastEl(0);
	un->send.spec.spec = &cs;
      }
    }
  }
  // second pass: connect
  FOR_ITR_EL(Layer, lay, net->layers., li) {
    Unit* un;
    taLeafItr ui;
    FOR_ITR_EL(Unit, un, lay->units., ui) {
      if(li.i > 0) {
	Layer* sl = (Layer*)net->layers.el[li.i-1];
	int j;
	for(j=0; j<n_units; j++) {
	  BpUnit* su = (BpUnit*)sl->units.el[j];
 	  BpCon* con = (BpCon*)su->send.Cn(j);
 	  un->recv.el[j] = con;
	  un->recv.units.el[j] = su;
	}
      }
      if(li.i != 2) {
	Layer* rl = (Layer*)net->layers.el[li.i+1];
	int j;
	for(j=0; j<n_units; j++) {
	  BpUnit* ru = (BpUnit*)rl->units.el[j];
// 	  BpCon* con = (BpCon*)ru->recv.Cn(j);
// 	  un->send.el[j] = con;
	  un->send.units.el[j] = ru;
	}
      }
    }
  }


#endif


#ifdef USE_NET
  net = (Network*)((Project*)pdpMisc::root->projects[0])->networks[0];
#endif

  // this clears all external inputs!
  net->InitWtState();

  // so put them back in..
  ApplyPats(net);

  FILE* logfile = fopen("pdp++bench.log", "w");
  // now run the thing!

#ifdef NO_TIME
  long 	st_time, ed_time;
#else
  clock_t st_time, ed_time;
#endif
  st_time = clock();

  int pats;
  for(pats = 0; pats < n_pats; pats++) {
    ApplyPats(net);

    FOR_ITR_EL(Layer, lay, net->layers., li) {
      if(lay->lesion)	continue;
      if(!(lay->ext_flag & Unit::EXT))
	lay->Compute_Net();
      lay->Compute_Act();
    }

    float sse = 0.0f;
    Layer* lay;
    taLeafItr li;
    FOR_ITR_EL(Layer, lay, net->layers., li) {
      if(!(lay->ext_flag & Unit::TARG))
	continue;
      Unit* un;
      taLeafItr ui;
      FOR_ITR_EL(Unit, un, lay->units., ui) {
	float tmp = un->targ - un->act;
	sse += tmp * tmp;
      }
    }

//     if((pats % 10) == 0)
//       fprintf(logfile,"%d\t%g\n", pats, sse);

    for(l=net->layers.size-1; l>= 0; l--) {
      lay = (Layer*) net->layers.El(l);
      if(lay->lesion || (lay->ext_flag & Unit::EXT)) // dont compute err on inputs
	continue;

      BpUnit* u;
      taLeafItr u_itr;
      FOR_ITR_EL(BpUnit, u, lay->units., u_itr)
	u->Compute_dEdA_dEdNet();
    }

    net->Compute_dWt();
    net->UpdateWeights();
  }

  ed_time = clock();

  fclose(logfile);

  double tot_time = (double)(ed_time - st_time) / (double)CLOCKS_PER_SEC;

  double n_wts = n_units * n_units * 2.0f;   // 2 fully connected layers
  double n_con_trav = 3.0f * n_wts * n_pats;
  double con_trav_sec = ((double)n_con_trav / tot_time) / 1.0e6;

  printf("weights: %g \tpats: %d \tcon_trav: %g\n", n_wts, n_pats, n_con_trav);
  printf("secs:\tMcon/sec:\n");
  printf("%g\t%g\n", tot_time, con_trav_sec);

}
