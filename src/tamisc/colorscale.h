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



#ifndef colorscale_h
#define colorscale_h

#include "ta_group.h"
#include "tamisc_TA_type.h"

#include "icolor.h"

#include <math.h>

// NOTE: Qt color values are 0..255 , but float colors retained for ease of Iv->Qt port

// forwards this file

class RGBA;
class RGBA_List;
class TAColor;
class TAColor_List;
class ColorScaleSpec;
class ColorScaleSpec_MGroup;
class ScaleRange;
class ScaleRange_List;
class ColorScale;


class RGBA : public taNBase {
  // ##INLINE ##NO_TOKENS Red Green Blue Alpha color specification
#ifndef __MAKETA__
typedef taNBase inherited;
#endif
public:

  float	r;			// red
  float	g;			// green
  float	b;			// blue
  float	a;			// alpha (intensity, ratio of fg to bg)
  String desc;			// description of what this color is

  const iColor*		color() const; //note: always correct -- updated on call

  String ToString_RGBA();
  void 	Copy_(const RGBA& cp);
  void  UpdateAfterEdit();
  COPY_FUNS(RGBA, taNBase);
  TA_BASEFUNS(RGBA);
  RGBA(float rd, float gr, float bl, float al = 1.0); // for Iv compatibility
protected:
  iColor	color_; // #IGNORE
private:
  void  Initialize();
  void 	Destroy();
};

class RGBA_List : public taList<RGBA> {
  // ##NO_TOKENS #NO_UPDATE_AFTER list of RGBA objects
public:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
  TA_BASEFUNS(RGBA_List);
};

class TAColor : public taBase { // ##NO_TOKENS Color
public:
  const iColor* color() {return &color_;}		// #IGNORE
  const iColor*	contrastcolor() {return &contrastcolor_;}	// #IGNORE
  void SetColor(float r,float g, float b, float a=1.0,	RGBA* background=NULL);
  // #USE_RVAL #ARGC=4 #NEW_FUN
  void SetColor(RGBA* c, RGBA* background = NULL) {
    SetColor(c->r, c->g, c->b, c->a, background);
  }
  void SetColor(const iColor* c, RGBA* background = NULL){
    SetColor(c->redf(), c->greenf(), c->bluef(), c->alpha(), background);
  }
  void Initialize()	{}
  void Destroy() {}
  TA_BASEFUNS(TAColor);
protected:
  iColor 	color_;		// #IGNORE
  iColor	contrastcolor_;	// #IGNORE
};

class TAColor_List : public taList<TAColor> {
  // ##NO_TOKENS #NO_UPDATE_AFTER list of TAColor objects
public:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
  TA_BASEFUNS(TAColor_List);
};

class ColorScaleSpec : public taNBase { // Color Spectrum Data
public:
  RGBA		background;	// background color
  RGBA_List	clr;		// #NO_BROWSE group of colors

  virtual void	GenRanges(TAColor_List* cl, int chunks);

  void 	Initialize();
  void 	Destroy()		{ };
  void 	InitLinks();
  void 	Copy_(const ColorScaleSpec& cp);
  COPY_FUNS(ColorScaleSpec, taNBase);
  TA_BASEFUNS(ColorScaleSpec);
};

// note: _MGroup name is for v3.2 compatability
class ColorScaleSpec_MGroup : public taGroup<ColorScaleSpec> {
public:
  virtual void 		NewDefaults(); 	// create a set of default colors
  virtual void		SetDefaultColor();// set the default color based on gui

  override int		NumListCols() const;
  override String	GetColHeading(int col); // header text for the indicated column
  override String	ChildGetColText_impl(taBase* child, int col, int itm_idx = -1);

  void 	Initialize()	{SetBaseType(&TA_ColorScaleSpec);};
  void 	Destroy()	{ };

  TA_BASEFUNS(ColorScaleSpec_MGroup);
};

//////////////////////////
//   ScaleRange		//
//////////////////////////

class ScaleRange : public taNBase {
  // ##NO_TOKENS #UAE_OWNER saves scale ranges for different variables viewed in netview
#ifndef __MAKETA__
typedef taNBase inherited;
#endif
public:
  bool		auto_scale;
  float 	min;
  float		max;

  void		SetFromScale(ColorScale& cs);

  void 		Copy_(const ScaleRange &cp)
    {auto_scale = cp.auto_scale; min = cp.min; max = cp.max; }
  COPY_FUNS(ScaleRange, taNBase);
  TA_BASEFUNS(ScaleRange);
private:
  void 		Initialize()	{ auto_scale = false; min = 0.0f; max = 0.0f;}
  void 		Destroy()	{ }
};


class ScaleRange_List : public taList<ScaleRange> {
  // ##NO_TOKENS #NO_UPDATE_AFTER list of ScaleRange objects
#ifndef __MAKETA__
typedef taList<ScaleRange> inherited;
#endif
public:
  void			Initialize() {}
  void 			Destroy() {};
  TA_BASEFUNS(ScaleRange_List);
};



class ColorScale : public taNBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER defines a range of colors to code data values with
public:
  int			chunks;		// number of chunks to divide scale into
  float 		min;
  float 		max;
  float			range;
  float			zero;
  ColorScaleSpec* 	spec;		// specifies the color ranges
  bool			auto_scale;	// #DEF_true

  TAColor_List		colors;		// #IGNORE the actual colors
//  TAColor		background; 	// #IGNORE background color
  iColor		background; 	// #IGNORE background color

  TAColor		maxout;		// #IGNORE
  TAColor		minout;		// #IGNORE
  TAColor		nocolor;	// #IGNORE

  virtual const iColor*	Get_Background(); // #IGNORE
  float 		GetAbsPercent(float val);
  virtual const iColor*	GetColor(int idx);
  virtual const iColor* GetColor(float val, const iColor** maincolor=NULL,
			    const iColor** contrast=NULL);
  virtual const iColor*	GetContrastColor(int idx);
  int 			GetIdx(float val);
  void			DefaultChunks();

  virtual void		MapColors(); 	// generates the colors from spec
  virtual void 		NewDefaults(); //
  void			SetFromScaleRange(ScaleRange* sr);

  // funcs used to be in bar:
  void			ModRange(float val);
  void			ModRoundRange(float val);
  void			FixRangeZero();
  void			SetMinMax(float mn,float mx);
  void			UpdateMinMax(float mn, float mx); // maybe expand bounds
  bool			UpdateMinMax(float val); // maybe expand bounds, returning true if expanded

  void 	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	CutLinks();
  void  UpdateAfterEdit();
  TA_BASEFUNS(ColorScale);
  ColorScale(int chunk);
};

#endif // colorscale_h



