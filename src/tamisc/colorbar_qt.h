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



#ifndef colorbar_qt_h
#define colorbar_qt_h

#include "colorscale.h"
#include "ta_matrix.h"
#include "ta_qt.h"


#ifndef __MAKETA__
# include <QWidget>
#else
// following for msvc
# ifdef min
#   undef min
# endif
# ifdef max
#   undef max
# endif
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

class TAMISC_API ColorPad : public QWidget { // #IGNORE color pads for palletes
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


class TAMISC_API ScaleBar : public QWidget { // #IGNORE  Scalebar
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

  QAbstractButton* 	enlarger;	// increase range button
  QAbstractButton* 	shrinker;	// decrease range button
  QAbstractButton* 	min_incr;	// increment minimum button
  QAbstractButton* 	min_decr;	// decrement minimum button
  QAbstractButton* 	max_incr;	// increment maximum button
  QAbstractButton* 	max_decr;	// decrement maximum button

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
  
  void 		InitLayout(); // call this in final inherited class constructor
private:
  void 		Init(bool hor, bool adj, bool ed);
};


class TAMISC_API HCScaleBar : public ScaleBar { //  scale bar with horizontal color blocks
#ifndef __MAKETA__
typedef ScaleBar inherited;
#endif
  Q_OBJECT
public:

  HCScaleBar(ColorScale* c, SpanMode sm, bool adj = false, bool ed = true,
    QWidget* parent = NULL);
private:
  void		Init(ColorScale* c);
};


class TAMISC_API VCScaleBar : public ScaleBar {//  #IGNORE scale bar with vertical color blocks
#ifndef __MAKETA__
typedef ScaleBar inherited;
#endif
  Q_OBJECT
public:

  VCScaleBar(ColorScale* c, SpanMode sm, bool adj = false, bool ed = true,
    QWidget* parent = NULL);
private:
  void		Init(ColorScale* c);
};

class TAMISC_API ColorMatrixGrid: QWidget {
  // a grid for visually depicting and/or editing grid data (usually 2d)
INHERITED(QWidget)
  Q_OBJECT
public:
  int		cellSize() const {return m_cellSize;} // h/w of each grid square; 0=auto
  void		setCellSize(int value);

  ColorMatrixGrid(QWidget* parent = NULL);

protected:
  int		m_cellSize;
  
  void 		paintEvent(QPaintEvent* event); // override
  
private:
  void		Init();
};

/*TODO
class TAMISC_API PScaleBar : public ScaleBar {
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

class TAMISC_API HPScaleBar : public PScaleBar {
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

class TAMISC_API VPScaleBar : public PScaleBar {
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


class TAMISC_API PadButton : public ivActiveHandler {
public:
  ColorPad* 	cp;

  operator ColorPad*(){ return cp;};
  virtual void		Set(float val) { cp->Set(val);}
  void press(const ivEvent&);
  PadButton(ivStyle* s, PScaleBar* sb,
	    ColorPad::BlockFill sh=ColorPad::COLOR, int w=8,int h=8);
};

class TAMISC_API PaintPad : public ivInputHandler {
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

class TAMISC_API DynamicBackground : public ivMonoGlyph {
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


class TAMISC_API CachePatch_Impl {
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

class TAMISC_API CachePatch : public ivPatch {
public:
  void request(ivRequisition&) const;
  CachePatch_Impl *impl;

  CachePatch(ivGlyph*);
  ~CachePatch(){ delete impl;}
}; */

#endif // colorbar_h
