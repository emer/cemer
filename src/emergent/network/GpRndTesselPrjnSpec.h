// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef GpRndTesselPrjnSpec_h
#define GpRndTesselPrjnSpec_h 1

// parent includes:
#include <ProjectionSpec>

// member includes:
#include <taVector2i>
#include <taVector2f>
#include <RndSeed>

// declare all other types mentioned but not required to include:

eTypeDef_Of(GpTessEl);

class E_API GpTessEl : public taOBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Spec one element of a tesselation specification for groups
INHERITED(taOBase)
public:
  taVector2i	send_gp_off;	// offset of group from current receiving group
  float		p_con;		// proportion connectivity from this group -- negative value means just make symmetric cons

  TA_SIMPLE_BASEFUNS(GpTessEl);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(GpTessEl_List);

class E_API GpTessEl_List : public taList<GpTessEl> {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Spec list of GpTessEl objects
INHERITED(taList<GpTessEl>)
public:
  TA_BASEFUNS_LITE_NOCOPY(GpTessEl_List);
private:
void	Initialize() 		{ SetBaseType(&TA_GpTessEl); }
  void 	Destroy()		{ };
};

eTypeDef_Of(GpRndTesselPrjnSpec);

class E_API GpRndTesselPrjnSpec : public ProjectionSpec {
  // specifies tesselated patterns of groups to connect with (both recv and send layers must have unit groups), optionally with random connectivity within each group (also very useful for full connectivity -- has optimized support for that) -- only 'permute' style randomness is supported, producing same number of recv connections per unit
INHERITED(ProjectionSpec)
public:
  taVector2i	recv_gp_off; 	// offset for start of recv group to begin connecting
  taVector2i	recv_gp_n;    	// number of receiving groups to connect in each dimension (-1 for all)
  taVector2i	recv_gp_skip; 	// increment for recv groups in each dimension -- 1 = connect all groups; 2 = skip every other group, etc
  taVector2i	recv_gp_group;	// group together this many unit groups under the same starting coord, resulting in a tile pattern
  taVector2f send_gp_scale;	// scale to apply to transform receiving unit group coords into sending unit group coords
  taVector2i	send_gp_border; // border size around sending layer (constant offset to add to sending offsets)
  GpTessEl_List	send_gp_offs;	// offsets of the sending unit groups
  bool		wrap;		// whether to wrap coordinates around (else clip)
  float		def_p_con;	// default probability of connectivity when new send_gp_offs are created
  bool		sym_self;	// if a self projection, make it symmetric (senders = receivers) otherwise it is not
  bool		same_seed;	// use the same random seed each time (same connect pattern)
  RndSeed	rndm_seed;	// #HIDDEN random seed

  void	Connect_impl(Projection* prjn) override;

  virtual void	GetCtrFmRecv(taVector2i& sctr, taVector2i ruc);
  // get center of sender coords from receiving coords
  virtual void  Connect_Gps(int rgpidx, int sgpidx, float p_con,
			    Projection* prjn, bool send_alloc);
  // #IGNORE impl connect send/recv gps
  virtual void  Connect_Gps_Sym(int rgpidx, int sgpidx, float p_con,
				Projection* prjn);
  // #IGNORE symmetric (p_con < 0)
  virtual void  Connect_Gps_SymSameGp(int rgpidx, int sgpidx, float p_con,
				      Projection* prjn);
  // #IGNORE symmetric, same unit group
  virtual void  Connect_Gps_SymSameLay(int rgpidx, int sgpidx, float p_con,
				      Projection* prjn);
  // #IGNORE symmetric, same layer
  virtual void  Connect_Gps_Std(int rgpidx, int sgpidx, float p_con,
				Projection* prjn);
  // #IGNORE standard, not symmetric/same
  virtual void  Connect_Gps_Full(int rgpidx, int sgpidx, Projection* prjn);
  // #IGNORE full connectivity, 
  virtual void	Connect_RecvGp(int rgpidx, const taVector2i& ruc, Projection* prjn,
			       bool send_alloc);
  // connects one recv unit to all senders

  virtual void	MakeRectangle(int width, int height, int left, int bottom);
  // #BUTTON make a connection pattern in the form of a rectangle starting at left, bottom coordinate and going right and up by width, height
  virtual void	MakeEllipse(int half_width, int half_height, int ctr_x, int ctr_y);
  // #BUTTON make a connection pattern in the form of an elipse: center is located at ctr_x,y and extends half_width and half_height therefrom
  virtual void	SetPCon(float p_con, int start = 0, int end = -1);
  // #BUTTON set p_con value for a range of send_gp_offs (default = all; end-1 = all)

  TA_SIMPLE_BASEFUNS(GpRndTesselPrjnSpec);
protected:
  void UpdateAfterEdit_impl() override;

private:
  void	Initialize();
  void	Destroy()	{ };
};
#endif // GpRndTesselPrjnSpec_h
