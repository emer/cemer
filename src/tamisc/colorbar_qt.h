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

#ifndef colorbar_qt_h
#define colorbar_qt_h

#include "colorscale.h"
#include "ta_qt.h"


#ifndef __MAKETA__
#include <qobjectlist.h>
#include <qwidget.h>
#else
// following for msvc
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#endif

// forwards this file
class Bar; // #IGNORE
class HCBar; // #IGNORE
class VCBar; // #IGNORE
class ColorPad; // #IGNORE
class ScaleBar; // #IGNORE
class HCScaleBar; // #IGNORE
class VCScaleBar; // #IGNORE
//class DynamicLabel;
//class PadButton;

//////////////////////////
//   Bar		//
//////////////////////////

class Bar : public QWidget { // #IGNORE Basic properties of a bar
#ifndef __MAKETA__
typedef QWidget inherited;
#endif
  Q_OBJECT
public:
  ColorScale* 		scale;
  virtual int 		blocks();

  virtual void		SetColorScale(ColorScale* c);
  Bar(ColorScale* c, QWidget* parent = NULL);
  ~Bar();
};



//////////////////////////
//   HCBar		//
//////////////////////////

class HCBar : public Bar { // Horizontal ColorBlocks bar
#ifndef __MAKETA__
typedef Bar inherited;
#endif
  Q_OBJECT
public:
  HCBar(ColorScale* c, QWidget* parent = NULL);
protected:
  void 		paintEvent(QPaintEvent* ev); // override
};


//////////////////////////
//   VCBar		//
//////////////////////////

class VCBar : public Bar { // #IGNORE Vertical ColorBlocks Bar
#ifndef __MAKETA__
typedef Bar inherited;
#endif
  Q_OBJECT
public:
  VCBar(ColorScale* c, QWidget* parent = NULL);
protected:
  void 		paintEvent(QPaintEvent* ev); // override
};

/*TODO
class PBar : public Bar { // Palette Bar
public:
  ivPolyGlyph* box;
  ivPatch*	blockpatch;
  ivTelltaleGroup* 	grp;
  int	paintindex;

  virtual int  	GetSelected();
  virtual void	InsertBlocks(ivPolyGlyph* pg);
  void		SetColorScale(ColorScale* c);

  PBar(ColorScale* c, QWidget* parent = NULL, const char* name = NULL);
  ~PBar();
};

class VPBar : public PBar { //
public:
  VPBar(ColorScale* c, QWidget* parent = NULL, const char* name = NULL);
};

class HPBar : public PBar { //  Horizontal Palette Bar
public:
  HPBar(ColorScale* c, QWidget* parent = NULL, const char* name = NULL);
};

*/

class ColorPad : public QWidget { // #IGNORE color pads for palletes
#ifndef __MAKETA__
typedef QWidget inherited;
#endif
  Q_OBJECT
public:
  enum BlockFill {		// fill types for grid blocks
    COLOR,			// color indicates value
    AREA,			// area indicates value
    LINEAR 			// linear size of square side indicates value
  };

  ScaleBar* 	sb;
  float 	padval;
  float 	oldpadval;
  BlockFill 	fill_type;
//  ivPatch* 	thepatch;
//  ivGlyph* 	theback;
//nn???  String 	name;
//TODO  DynamicLabel* thename;
  iColor fg;			// #IGNORE foreground color;
  iColor bg;			// #IGNORE background color;
  iColor tc;			// #IGNORE text color;

  virtual void		SetFillType(BlockFill b);
  virtual void		Set(float val);
  virtual void		Reset();
  virtual void		ReFill();
  virtual float		GetVal();
  virtual void		GetColors();
  virtual void		Toggle();
  ColorPad(ScaleBar* tsb, BlockFill s = COLOR, QWidget* parent = NULL);
  ~ColorPad();
};


class ScaleBar : public QWidget { // #IGNORE  Scalebar
#ifndef __MAKETA__
typedef QWidget inherited;
#endif
  Q_OBJECT
public:
  enum SpanMode {
    RANGE,			// Range mode
    MIN_MAX 			// min max
  };
  float 		min() {return bar->scale->min;}
  float 		max() {return bar->scale->max;}
  float			range() {return bar->scale->range;}
  float			zero() {return bar->scale->zero;}

  SpanMode		sm;
  bool			adjustflag;	// do we have min/max/range adjuster buttons
  bool			editflag;	// is the label editable

  QBoxLayout*		layOuter;  // hor or vert as indicated
  Bar*			bar;		// actual bar

  iLineEdit*  		min_frep;
  iLineEdit*  		max_frep;

  QPushButton* 		enlarger;	// increase range button
  QPushButton* 		shrinker;	// decrease range button
  QPushButton* 		min_incr;	// increment minimum button
  QPushButton* 		min_decr;	// decrement minimum button
  QPushButton* 		max_incr;	// increment maximum button
  QPushButton* 		max_decr;	// decrement maximum button

  Widget_List	  	padlist; 	// #IGNORE list of color pads for palletes

  void			emit_scaleValueChanged();
  virtual void SetRange(float val);
  virtual void SetRoundRange(float val);
//obs  virtual void ModRange(float val);
//obs  virtual void ModRoundRange(float val);

//obs  virtual void FixRangeZero(); // sets range and zero according to min,max
//  virtual void UpdateMinMax(float mn, float mx);
  virtual void SetMinMax(float min,float max);
//  virtual int GetIdx(float val);	// returns index of the value
  virtual float GetVal(int idx);	// returns value of the index
//  virtual iColor* GetColor(float val, iColor** maincolor=NULL,
//			    iColor** contrast=NULL);
   // return color object for val
  virtual const iColor* GetColor(int idx);	// return color[idx];
  virtual const iColor* GetContrastColor(int idx);	// return color[idx];

  virtual void SetColorScale(ColorScale* c);

//  virtual float GetAbsPercent(float val);

  virtual void UpdatePads();
  virtual void Adjust(); //TODO: prob not needed
  virtual bool GetScaleValues();      // gets from the edit into nums; if conv error, then returns false and sets values back
  virtual void UpdateScaleValues();   // puts from nums into glyph



  ScaleBar(bool hor, SpanMode sm, bool adj, bool ed, QWidget* parent = NULL);
  virtual ~ScaleBar();

public slots:
  virtual void editor_accept();
  //nn virtual void editor_reject(ivFieldEditor*);
  virtual void Incr_Range();
  virtual void Decr_Range();
  virtual void Incr_Min();
  virtual void Incr_Max();
  virtual void Decr_Min();
  virtual void Decr_Max();

#ifndef __MAKETA__
signals:
  void		scaleValueChanged(); // one of the values was changed
#endif

protected:
  bool		hor; // true for horizontal, false for vertical
  void 		initLayout(); // call this in final inherited class constructor
private:
  void 		init(bool hor, bool adj, bool ed);
};


class HCScaleBar : public ScaleBar { //  scale bar with horizontal color blocks
#ifndef __MAKETA__
typedef ScaleBar inherited;
#endif
  Q_OBJECT
public:

  HCScaleBar(ColorScale* c, SpanMode sm, bool adj = false, bool ed = true,
    QWidget* parent = NULL);
private:
  void		init(ColorScale* c);
};


class VCScaleBar : public ScaleBar {//  #IGNORE scale bar with vertical color blocks
#ifndef __MAKETA__
typedef ScaleBar inherited;
#endif
  Q_OBJECT
public:

  VCScaleBar(ColorScale* c, SpanMode sm, bool adj = false, bool ed = true,
    QWidget* parent = NULL);
private:
  void		init(ColorScale* c);
};

/*TODO
class PScaleBar : public ScaleBar {
  //  Scalebar with a pallete
public:

  PScaleBar(float mn, float mx,bool adj=false,bool ed=true);
  PScaleBar(float r,bool adj = false,bool ed=true);
  virtual ~PScaleBar()          { Destroy(); }

  int GetSelected();

  float 		tag[4];
  String 		stag[4];
  PadButton* 		pb[4];
  ivPolyGlyph* 		fbox;
  ivPolyGlyph* 		tagpads;

  virtual void MakePads();
  virtual void MakeTags();
  virtual void Get_Tags(ivFieldEditor*);
  virtual void Set_Tags(ivFieldEditor*);
  virtual void GetScaleValues();

  void	SetColorScale(ColorScale* c);

  virtual void Destroy();

  virtual ivGlyph* GenIH(ivGlyph* g);
  float GetSelectedVal();
};

class HPScaleBar : public PScaleBar {
  //  Horizontal Scalebar with a pallete
public:

  virtual DAStepper* GenEnlarger();
  virtual DAStepper* GenShrinker();
  virtual ivGlyph*	GetLook();

   HPScaleBar(float min, float max, bool adj, bool ed,ColorScale* c,
	     int b=16,int h=16, int w=16);
  HPScaleBar(float r, bool adj, bool ed,ColorScale* c,
	     int b=16,int h=16, int w=16);
};

class VPScaleBar : public PScaleBar {
  //  Vertical Scalebar with a pallete
public:
  virtual DAStepper* GenEnlarger();
  virtual DAStepper* GenShrinker();
  virtual ivGlyph*	GetLook();

  VPScaleBar(float min, float max, bool adj, bool ed,ColorScale* c,
	     int b=16,int h=16, int w=16);
  VPScaleBar(float r, bool adj, bool ed,ColorScale* c,
	     int b=16,int h=16, int w=16);
};


class PadButton : public ivActiveHandler {
public:
  ColorPad* 	cp;

  operator ColorPad*(){ return cp;};
  virtual void		Set(float val) { cp->Set(val);}
  void press(const ivEvent&);
  PadButton(ivStyle* s, PScaleBar* sb,
	    ColorPad::BlockFill sh=ColorPad::COLOR, int w=8,int h=8);
};

class PaintPad : public ivInputHandler {
public:
  ColorPad* pb;
  void* obj;			    // object to pass to change_notify
  static ivCursor* painter_cursor;

  void (*change_notify)(void*); // function to call if changed
  virtual void press(const ivEvent& e);
  virtual void release(const ivEvent& e);
  virtual void drag(const ivEvent& e);
  virtual void move(const ivEvent& e);
  PaintPad(ColorPad* c,void* o, void (*cn)(void*));
};

class DynamicBackground : public ivMonoGlyph {
public:
  DynamicBackground(ivGlyph* body, const iColor*);
  virtual ~DynamicBackground();
  virtual void SetColor(iColor*);
  virtual void allocate(ivCanvas*, const ivAllocation&, ivExtension&);
  virtual void draw(ivCanvas*, const ivAllocation&) const;
  virtual void print(ivPrinter*, const ivAllocation&) const;
protected:
  const iColor* color_;
};


class CachePatch_Impl {
public:
  enum FakeLevel {
    FIRST,
    ONCE,
    ALWAYS
  };

  ivRequisition	oldreq;
  FakeLevel	fakeflag;
  CachePatch_Impl(){};
  ~CachePatch_Impl(){};
};

class CachePatch : public ivPatch {
public:
  void request(ivRequisition&) const;
  CachePatch_Impl *impl;

  CachePatch(ivGlyph*);
  ~CachePatch(){ delete impl;}
}; */

#endif // colorbar_h
