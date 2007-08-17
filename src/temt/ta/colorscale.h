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



#ifndef colorscale_h
#define colorscale_h

#include "ta_group.h"
#include "ta_TA_type.h"

#include "icolor.h"

#ifndef __MAKETA__
# include <math.h>
#endif

// NOTE: Qt color values are 0..255 , but float colors retained for ease of Iv->Qt port

// forwards this file

class taColor;
class RGBA;
class RGBA_List;
class TAColor;
class TAColor_List;
class ColorScaleSpec;
class ColorScaleSpec_Group;
class ScaleRange;
class ScaleRange_List;
class ColorScale;
typedef iColor* ptr_iColor; // hacks needed to force creation of TA_const_iColor
typedef ptr_iColor const_iColor;

class TA_API taColor : public taBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Display Red Green Blue Alpha color value
INHERITED(taBase)
public:
  bool 			no_a; // #NO_SHOW #NO_SAVE control using a, by context
  float			r; // red
  float			g; // green
  float			b; // blue
  float			a; // #CONDSHOW_ON_no_a:false alpha (intensity, ratio of fg to bg)

  const iColor		color() const;
  void			setColor(const iColor& cp);
  
  void			Set(float r_, float g_, float b_, float a_ = 1)
     {r=r_; g=g_; b=b_; a=a_;}
  TA_BASEFUNS_LITE(taColor);

  			operator iColor() const {return color();}

private:
  void 	Copy_(const taColor& cp) {r=cp.r; g=cp.g; b=cp.b; a=cp.a;}// not no_a
  void  Initialize() {no_a = false; r = g = b = 0; a = 1;}
  void 	Destroy() {}
};

class TA_API RGBA : public taNBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS ##CAT_Display Red Green Blue Alpha color specification
INHERITED(taNBase)
public:
  String 		name; // description of what this color is
  float			r; // red
  float			g; // green
  float			b; // blue
  float			a; // alpha (intensity, ratio of fg to bg)
  String 		desc; // description of what this color is

  const iColor		color() const; //note: always correct -- updated on call

  String ToString_RGBA() const;
  String	GetDesc() const			{ return desc; }
  TA_BASEFUNS_LITE(RGBA);
  RGBA(float rd, float gr, float bl, float al = 1.0); // for Iv compatibility
protected:
  override void 	UpdateAfterEdit_impl(); // don't use C names
  //note: we handle both directions of copy to/from taColor
  override void		CanCopyCustom_impl(bool to, const taBase* cp,
    bool quiet, bool& allowed, bool& forbidden) const;
  override void		CopyFromCustom_impl(const taBase* cp);
  override void		CopyToCustom_impl(taBase* to) const;
    
private:
  void 	Copy_(const RGBA& cp);
  void  Initialize();
  void 	Destroy();
};

class TA_API RGBA_List : public taList<RGBA> {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Display list of RGBA objects
INHERITED(taList<RGBA>)
public:
  void	Initialize() 		{SetBaseType(&TA_RGBA); };
  void 	Destroy()		{ };
  TA_BASEFUNS_NOCOPY(RGBA_List);
};

class TA_API TAColor : public taBase { // ##NO_TOKENS Color
INHERITED(taBase)
public:
  const iColor 	color() {return color_;}		// #IGNORE
  const iColor	contrastcolor() {return contrastcolor_;}	// #IGNORE
  void SetColor(const iColor& c, RGBA* background = NULL){
    SetColor(c.redf(), c.greenf(), c.bluef(), c.alphaf(), background);
  }
  void SetColor(RGBA* c, RGBA* background = NULL) {
    SetColor(c->r, c->g, c->b, c->a, background);
  }
  void SetColor(float r,float g, float b, float a=1.0,	RGBA* background=NULL);
  // #USE_RVAL #ARGC=4 #NEW_FUN
  TA_BASEFUNS_LITE(TAColor);
protected:
  iColor 	color_;		// #IGNORE
  iColor	contrastcolor_;	// #IGNORE
private:
  void	Copy_(const TAColor& cp) 
    { color_ = cp.color_; contrastcolor_ = cp.contrastcolor_;}
  void Initialize()	{}
  void Destroy() {}
};

class TA_API TAColor_List : public taList<TAColor> {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Display list of TAColor objects
INHERITED(taList<TAColor>)
public:
  TA_BASEFUNS_NOCOPY(TAColor_List);
private:
  void	Initialize() 		{SetBaseType(&TA_TAColor); };
  void 	Destroy()		{ };
};

class TA_API ColorScaleSpec : public taNBase {
  // ##CAT_Display Color Spectrum Data
INHERITED(taNBase)
public:
  RGBA		background;	// background color
  RGBA_List	clr;		// #HIDDEN_TREE group of colors

  virtual void	GenRanges(TAColor_List* cl, int chunks);

  static const KeyString key_bkclr;
  static const KeyString key_clr0;
  static const KeyString key_clr1;
  static const KeyString key_clr2;
  static const KeyString key_clr3;
  static const KeyString key_clr4;
  override String GetColText(const KeyString& key, int itm_idx) const;
  void 	InitLinks();
  TA_BASEFUNS(ColorScaleSpec);
private:
  void 	Copy_(const ColorScaleSpec& cp);
  void 	Initialize();
  void 	Destroy()		{ };
};

SmartRef_Of(ColorScaleSpec,TA_ColorScaleSpec); // ColorScaleSpecRef

class TA_API ColorScaleSpec_Group : public taGroup<ColorScaleSpec> {
  // ##CAT_Display group of color scale specs
INHERITED(taGroup<ColorScaleSpec>)
public:
  virtual void 		NewDefaults(); 	// create a set of default colors
  virtual void		SetDefaultColor();// set the default color based on gui

  override int		NumListCols() const;
  override const KeyString GetListColKey(int col) const;
  override String	GetColHeading(const KeyString& key) const; // header text for the indicated column

  TA_BASEFUNS_NOCOPY(ColorScaleSpec_Group);
private:
  void 	Initialize()	{SetBaseType(&TA_ColorScaleSpec);};
  void 	Destroy()	{ };
};

//////////////////////////
//   ScaleRange		//
//////////////////////////

class TA_API ScaleRange : public taNBase {
  // ##NO_TOKENS ##CAT_Display saves scale ranges for different variables viewed in netview
INHERITED(taNBase)
public:
  bool		auto_scale;
  float 	min;
  float		max;

  void		SetFromScale(ColorScale& cs);

  TA_BASEFUNS(ScaleRange);
protected:
  override void		UpdateAfterEdit_impl();

private:
  void 		Copy_(const ScaleRange &cp)
    {auto_scale = cp.auto_scale; min = cp.min; max = cp.max; }
  void 		Initialize()	{ auto_scale = false; min = 0.0f; max = 0.0f;}
  void 		Destroy()	{ }
};


class TA_API ScaleRange_List : public taList<ScaleRange> {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Display list of ScaleRange objects
INHERITED(taList<ScaleRange>)
public:
  void			Initialize() {SetBaseType(&TA_ScaleRange);}
  void 			Destroy() {};
  TA_BASEFUNS_NOCOPY(ScaleRange_List);
};

class TA_API ColorScale : public taNBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Display defines a range of colors to code data values with
INHERITED(taNBase)
public:
  static const iColor	def_color; 
  static float		sc_val_def; // #HIDDEN def arg for sc_val

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

  virtual void		SetColorSpec(ColorScaleSpec* color_spec);
  // #BUTTON set the color scale spec to determine the palette of colors representing values

  virtual const iColor	Get_Background(); // #IGNORE
  float 		GetAbsPercent(float val);
  virtual const iColor	GetColor(int idx, bool* ok = NULL);
  virtual const iColor GetColor(float val, iColor* maincolor=NULL,
	iColor* contrast=NULL, float& sc_val = sc_val_def);
  // #IGNORE
  virtual const iColor	GetContrastColor(int idx, bool* ok = NULL);
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

  void	InitLinks();
  void	CutLinks();
  void  UpdateAfterEdit();
  TA_BASEFUNS(ColorScale);
  ColorScale(int chunk);
private:
  SIMPLE_COPY(ColorScale) //note: added 4/12/07 for consistency, but may not be good
  void 	Initialize();
  void 	Destroy();
};

SmartRef_Of(ColorScale,TA_ColorScale); // ColorScaleRef

class TA_API ViewColor : public taNBase {
  // ##INLINE ##NO_TOKENS ##CAT_Display view color specification -- name lookup of color highlighting for view display
INHERITED(taNBase)
public:
  bool		use_fg;			// #APPLY_IMMED use a special foreground color
  RGBA		fg_color;		// #CONDEDIT_ON_use_fg:true foreground color
  bool		use_bg;			// use a special background color
  RGBA		bg_color;		// #CONDEDIT_ON_use_fg:true background color
  String 	desc;			// description of this view color item

  override String	GetDesc() const { return desc; }

  void	InitLinks();
  TA_BASEFUNS(ViewColor);
private:
  SIMPLE_COPY(ViewColor);
  void  Initialize();
  void 	Destroy();
};

class TA_API ViewColor_List : public taList<ViewColor> {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Display list of ViewColor items
INHERITED(taList<ViewColor>)
public:

  virtual bool	FindMakeViewColor(const String& nm, const String& dsc,
				  bool fg, const String& fg_color_name,
				  bool bg=false, const String& bg_color_name="");
  // find view color of given name -- if not there, make it, with given params (returns false if didn't already exist)

  TA_BASEFUNS_NOCOPY(ViewColor_List);
private:
  void	Initialize() 		{ SetBaseType(&TA_ViewColor); }
  void 	Destroy()		{ };
};


#endif // colorscale_h



