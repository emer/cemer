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



#include <colorbar_qt.h>

#include <qlayout.h>
#include <QPainter>
#include <qpushbutton.h>
#include <qstring.h>
#include <QToolButton>

#include "icolor.h"
#include "ilineedit.h"

#include <math.h>

//////////////////////////
//   Bar		//
//////////////////////////

Bar::Bar(ColorScale* c, QWidget* parent)
:inherited(parent)
{
  scale = c;
  taBase::Ref(scale);
}

Bar::~Bar(){
  scale = NULL; // not really necessary...
}

int Bar::blocks() {
  int rval = (scale) ? scale->chunks : 1;
  if (rval == 0) rval = 1;  // for robustness
  return rval;
}

void Bar::SetColorScale(ColorScale* c){
  scale = c;
}


//////////////////////////
//   HCBar		//
//////////////////////////

HCBar::HCBar(ColorScale* c, QWidget* parent)
:inherited(c, parent)
{};

void HCBar::paintEvent(QPaintEvent* ev) {
  if (!scale) return; // some kind of zombie happening...
  QRect r = rect();
  int w_tot = r.width();
  if (w_tot == 0) return;  // prob should never happen

  int b = blocks(); // cache
  int x = r.x(); //  s/b 0
  int h = r.height(); // always exact
  int y = r.y(); // always exact
  QPainter paint(this);
  if (w_tot <= b ) { // less or same # strips as blocks -- subsample
    for (int j = 0; j < w_tot ; ++j) { // one strip per pixel
      int i = (j * (b - 1)) / w_tot ;
      bool ok;
      const iColor col = scale->GetColor(i, &ok);
      if (!ok) break; // shouldn't happen
      paint.fillRect(x, y, 1, h, (QColor)col); // note: QColor converted to QBrush by Qt
      x += 1;
    }
  } else { // more space than blocks -- oversample
    float w = ((float)w_tot) / b; // ideal exact value per strip
    for (int i = 0; i < b; ++i) { // one strip per color value
      bool ok;
      const iColor col = scale->GetColor(i, &ok);
      if (!ok) break; // shouldn't happen
      int wi = (int)(((i + 1) * w) - (float)x);

      paint.fillRect(x, y, wi, h, (QColor)col); // note: QColor converted to QBrush by Qt
      x += wi;
    }
  }
}


//////////////////////////
//   VCBar		//
//////////////////////////

VCBar::VCBar(ColorScale* c, QWidget* parent)
:inherited(c, parent)
{}


void VCBar::paintEvent(QPaintEvent* ev) {
  if (!scale) return; // some kind of zombie happening...
  QRect r = rect();
  int h_tot = r.height();
  if (h_tot == 0) return;  // prob should never happen

  int b = blocks(); // cache
  int x = r.x(); // always exact
  int w = r.width(); // always exact
  int y = r.y(); // s/b 0
  QPainter paint(this);
  if (h_tot <= b ) { // less or same # strips as blocks -- subsample
    for (int j = h_tot - 1; j >= 0; --j) { // one strip per pixel
      int i = (j * (b - 1)) / h_tot ;
      bool ok;
      const iColor col = scale->GetColor(i, &ok);
      if (!ok) break; // shouldn't happen
      paint.fillRect(x, y, w, 1, (QColor)col); // note: QColor converted to QBrush by Qt
      y += 1;
    }
  } else { // more space than blocks -- oversample
    float h = ((float)h_tot) / b; // ideal exact value per strip
    for (int i = b - 1; i >= 0; --i) { // one strip per color value
      bool ok;
      const iColor col = scale->GetColor(i, &ok);
      if (!ok) break; // shouldn't happen
//      int hi = (int)(((((b - 1) - i) + 1) * h) - (float)y);
      int hi = (int)(((b - i) * h) - (float)y);

      paint.fillRect(x, y, w, hi, (QColor)col); // note: QColor converted to QBrush by Qt
      y += hi;
    }
  }
}

/*
//////////////////////////
//   Xxx		//
//////////////////////////

PBar::PBar(ColorScale* c, int h, int w, QWidget* parent, const char* name)
:inherited(c, h, w, parent, name)
{
  box = NULL;
  grp = new ivTelltaleGroup();
  ivResource::ref(grp);
  blocks = 17;
};

PBar::~PBar(){
  ivResource::unref(grp);
  ivResource::unref(blockpatch);
}

void PBar::SetColorScale(ColorScale* c){
  Bar::SetColorScale(c);
  blocks = 15;
  while(box->count() > 0){
    box->remove(0);
  }
  InsertBlocks(box);
  blockpatch->reallocate();
  blockpatch->redraw();
}

void PBar::InsertBlocks(ivPolyGlyph* pg){
  ivWidgetKit* kit = ivWidgetKit::instance();
  ivLayoutKit* layout = ivLayoutKit::instance();
  ivTelltaleState* firststate = NULL;

  iColor* selectcolor = new iColor(1.0,1.0,1.0,1.0);
  int i;
  for(i=0;i<blocks;i++){
    int idx = (int) ( (float) i * ((float) (scale->chunks -1)
			       / (float) (blocks-1)));
    ivGlyph* coloroff =
      layout->margin
      (kit->outset_frame
       (layout->flexible
	(layout->natural
	 (new ivBackground(NULL,scale->GetColor(idx)),
	  width-2,height-2),fil,8)),1);

    ivGlyph* coloron =
      new ivBorder
      (layout->margin
       (kit->outset_frame
	(layout->flexible
	 (layout->natural
	  (new ivBackground(NULL,scale->GetColor(idx)),
	   width-2,height-2),fil,8)),1),selectcolor,1);

    ivTelltaleState* bstate =
      new ivTelltaleState(ivTelltaleState::is_enabled_visible |
			ivTelltaleState::is_choosable);
    bstate->join(grp);
    if(i==0) {
      firststate = bstate;
    }
    pg->append
       (new ivButton
	(new ivChoiceItem
	 (bstate,coloroff,
	  coloroff,coloroff,coloron,coloron,
	  coloron,coloron,coloron,coloron,
	  coloroff),kit->style(),bstate,NULL));
  }
  if(firststate != NULL)
    firststate->set(ivTelltaleState::is_chosen,true);
}


int PBar::GetSelected(){
  if(box){
    int i;
    for(i=0;i<box->count();i++){
      if(((ivButton *)(box->component(i)))->state()->test
	 (ivTelltaleState::is_chosen)) {
	return i;
      }
    }
  }
  return -1;
}

//////////////////////////
//   Xxx		//
//////////////////////////

VPBar::VPBar(ColorScale* c, int h, int w, QWidget* parent, const char* name)
:inherited(c, h, w, parent, name)
{
  ivLayoutKit* layout = ivLayoutKit::instance();
  box = layout->vbox();
  InsertBlocks(box);
  blockpatch  = new ivPatch(box);
  ivResource::ref(blockpatch);
};

ivGlyph* VPBar::GetBar() {
  ivWidgetKit* kit = ivWidgetKit::instance();
  ivLayoutKit* layout = ivLayoutKit::instance();
  return (kit->inset_frame(layout->hflexible(blockpatch)));
}

//////////////////////////
//   Xxx		//
//////////////////////////

HPBar::HPBar(ColorScale* c, int b, int h, int w, QWidget* parent, const char* name)
:inherited(c, b, h, w, parent, name)
{
  ivLayoutKit* layout = ivLayoutKit::instance();
  box = layout->hbox();
  InsertBlocks(box);
  blockpatch  = new ivPatch(box);
  ivResource::ref(blockpatch);
};

ivGlyph* HPBar::GetBar() {
  ivWidgetKit* kit = ivWidgetKit::instance();
  ivLayoutKit* layout = ivLayoutKit::instance();
  return (kit->inset_frame(layout->vflexible(blockpatch)));
}
*/

//////////////////////////
//   ColorPad		//
//////////////////////////

ColorPad::ColorPad(ScaleBar* tsb, BlockFill s, QWidget* parent)
:inherited(parent) {
  sb = tsb;
  sb->padlist.Add(this);
  padval = oldpadval = 0;
  fill_type = s;
//  if(nm !=  NULL) name = nm;
  ReFill();
};

ColorPad::~ColorPad(){
}

void ColorPad::GetColors(){
  ColorScale* scale = sb->bar->scale; // cache
  if (!scale) return; // safety
  if (fill_type == COLOR){
    iColor tfg;
    iColor ttc;
    float sc_val;
    scale->GetColor(padval,sc_val,&tfg,&ttc);
    fg = tfg;
    tc = ttc;
    bg = tc;
  }
  else {
    if(padval > sb->max()) {
      fg = scale->maxout.color();
      tc = scale->maxout.contrastcolor();
    }
    else if (padval < sb->min()) {
      fg = scale->minout.color();
      tc = scale->minout.contrastcolor();
    }
    else if(padval >= sb->zero()) {
      fg = scale->GetColor(-1);
      tc = scale->GetContrastColor(-1);
    }
    else {
      fg = scale->GetColor(0);
      tc = scale->GetContrastColor(0);
    }
    bg = scale->GetColor((int)scale->zero);
  }
}

void ColorPad::ReFill(){
  /*TODO
  ivLayoutKit* layout = ivLayoutKit::instance();
  ivWidgetKit* wkit = ivWidgetKit::instance();
  thename = NULL;
  GetColors();
  float percent = 0.0f;
  if(fill_type != COLOR) percent = sb->GetAbsPercent(padval);
  switch (fill_type) {
  case COLOR:   theback = new DynamicBackground(NULL,fg); break;
  case AREA:    theback = new Spiral(percent,fg,bg,width,height);
  case LINEAR:  theback = new Spiral(percent*percent,fg,bg,width,height);
  }
  if(name.empty())  // no text
    thepatch->body(layout->fixed(theback,width,height));
  else {
    const ivFont* fnt = wkit->font();
    ivFontBoundingBox b;
    String newname = name;
    int len = newname.length();
    fnt->string_bbox(newname.chars(), len, b);
    while((len >1) && (b.width() > width)){
      len--;
      fnt->string_bbox(newname.chars(), len, b);
    }
    newname = newname.before(len);
    thename = new DynamicLabel((char *) newname,fnt,tc);
    thepatch->body(layout->fixed
		   (layout->overlay
		    (theback,
		     layout->vcenter(taivM->wrap(thename),0)),
		    width,height));
  } */
}

void ColorPad::SetFillType(BlockFill b){
  fill_type = b;
  ReFill();
  update();
}

void ColorPad::Reset(){ /*TODO
  GetColors();
  float percent = 0.0f;
  if (fill_type != COLOR) percent = sb->GetAbsPercent(padval);
  switch(fill_type) {
  case COLOR:  ((DynamicBackground *) theback)->SetColor(fg); break;
  case AREA:   ((Spiral *) theback)->Set(percent,fg,bg); break;
  case LINEAR: ((Spiral *) theback)->Set(percent * percent,fg,bg);
  }
  if (thename != NULL){
    if(thename != NULL) thename->SetColor(tc);
  }
  thepatch->redraw();
  ivResource::flush(); */
}

void ColorPad::Set(float val) {
  if (padval == val) return;
  oldpadval = padval;
  padval = val;
  Reset();
}

float ColorPad::GetVal(){
  return padval;
}

void ColorPad::Toggle() { /*TODO
  if (padval == ((PScaleBar*) sb)->GetSelectedVal()){
    Set(oldpadval);
  }
  else {
    Set(((PScaleBar*) sb)->GetSelectedVal());
  } */
}


//////////////////////////
//   ScaleBar		//
//////////////////////////

ScaleBar::ScaleBar(bool hor_, SpanMode sm_, bool adj, bool ed, QWidget* parent)
:inherited(parent)
{
  sm = sm_;
  Init(hor_, adj, ed);
};

ScaleBar::~ScaleBar(){
  padlist.Reset();
}

void ScaleBar::UpdatePads(){
  for (int i = 0; i < padlist.size; ++i) {
    ((ColorPad*) padlist.FastEl(i))->Reset();
  }
}

void ScaleBar::Init(bool hor_, bool adj, bool ed){
  hor = hor_;
  adjustflag = adj;
  editflag = ed;
  bar = NULL;
  enlarger = NULL;
  shrinker = NULL;
  min_incr = NULL;
  min_decr = NULL;
  max_incr = NULL;
  max_decr = NULL;

  if (hor)
    layOuter = new QHBoxLayout(this);
  else
    layOuter = new QVBoxLayout(this);
  layOuter->setMargin(0);

  min_frep = new iLineEdit(this);
  min_frep->setCharWidth(6); // make them a bit smaller
  max_frep = new iLineEdit(this);
  max_frep->setCharWidth(6); // make them a bit smaller
  if (editflag) {
    connect(min_frep, SIGNAL(editingFinished()), this, SLOT(editor_accept()) );
    connect(max_frep, SIGNAL(editingFinished()), this, SLOT(editor_accept()) );
  } else { // no editing
    min_frep->setReadOnly(true);
    max_frep->setReadOnly(true);
  }

  if (adjustflag) {
    if (sm == RANGE) {
      enlarger = new QToolButton(this);
      ((QToolButton*)enlarger)->setArrowType(Qt::UpArrow);
      connect(enlarger, SIGNAL(pressed()), this, SLOT(Incr_Range()));
      shrinker = new QToolButton(this);
      ((QToolButton*)shrinker)->setArrowType(Qt::DownArrow);
      connect(shrinker, SIGNAL(pressed()), this, SLOT(Decr_Range()));
    } else {
      min_incr = new QToolButton(this);
      ((QToolButton*)min_incr)->setArrowType(Qt::UpArrow);
      connect(min_incr, SIGNAL(pressed()), this, SLOT(Incr_Min()));
      min_decr = new QToolButton(this);
      ((QToolButton*)min_decr)->setArrowType(Qt::DownArrow);
      connect(min_decr, SIGNAL(pressed()), this, SLOT(Decr_Min()));
      max_incr = new QToolButton(this);
      ((QToolButton*)max_incr)->setArrowType(Qt::UpArrow);
      connect(max_incr, SIGNAL(pressed()), this, SLOT(Incr_Max()));
      max_decr = new QToolButton(this);
      ((QToolButton*)max_decr)->setArrowType(Qt::DownArrow);
      connect(max_decr, SIGNAL(pressed()), this, SLOT(Decr_Max()));
    }
  }
}

void ScaleBar::InitLayout() {
//TODO : add remainder of controls, ex. min/max types
  if (hor) {
    layOuter->addWidget(min_frep);
    if (bar) { 
      bar->setMaximumHeight(min_frep->height()); // make them the same
      layOuter->addWidget(bar, 1);
    }
    layOuter->addWidget(max_frep);
    if (shrinker) { //note: both or none
      // put them side by side, to save vert room
      QBoxLayout* layBut = new QHBoxLayout; layOuter->addLayout(layBut);
      layBut->setMargin(0); //spacing=2
      layBut->setSpacing(0); //abut
      layBut->addWidget(enlarger);
      layBut->addWidget(shrinker);
     }
  } else { // vert
    if (enlarger) { //note: both or none
      QBoxLayout* layBut = new QHBoxLayout; layOuter->addLayout(layBut);
      layBut->setMargin(0); //spacing=2
      layBut->addWidget(enlarger);
      layBut->addWidget(shrinker);
     }
    layOuter->addWidget(max_frep);
    if (bar) layOuter->addWidget(bar);
    layOuter->addWidget(min_frep);
  }
}

void ScaleBar::Adjust(){
  emit_scaleValueChanged();
}

void ScaleBar::editor_accept() {
  // we only do anything if the values actually changed
  String min_str(min(), "%5.3f");
  String max_str(max(), "%5.3f");
  // check the string values, because converting numbers will cause roundoff
  if ((min_str == min_frep->text()) && (max_str == max_frep->text())) return;


  if (!GetScaleValues()) return;
  UpdatePads();
  Adjust();
}

void ScaleBar::emit_scaleValueChanged() {
  emit scaleValueChanged();
}

/* implicit
void ScaleBar::editor_reject(ivFieldEditor*) {
  UpdateScaleValues();
  UpdatePads();
  Adjust();
} */

#define scalebar_low_value	1.0e-10

const iColor ScaleBar::GetColor(int idx, bool* ok){
  int i = idx;
  if(i < 0) i = bar->scale->colors.size + i;
  return(bar->scale->GetColor(i, ok));
}

const iColor ScaleBar::GetContrastColor(int idx, bool* ok){
  int i = idx;
  if(i < 0) i = bar->scale->colors.size + i;
  return(bar->scale->GetContrastColor(i, ok));
}

float ScaleBar::GetVal(int idx) {
  return (((idx * (max()-min()))/(bar->blocks()-1)) + min());
}

bool ScaleBar::GetScaleValues(){
  bool ok = true;
  if (editflag) {
    float min_t = min_frep->text().toFloat(&ok);
    float max_t;
    if (ok)
      max_t = max_frep->text().toFloat(&ok);
    if (!ok) {
      UpdateScaleValues(); // set them back
      return ok;
    }
    bar->scale->SetMinMax(min_t, max_t);
  }
  return ok;
}
void ScaleBar::Incr_Range(){
  int i = 1; // used to be a param
  if(adjustflag) {
    GetScaleValues();
    int j;
    for(j=0;j<i;j++){
      float incr = ::max((float)(fabs((double)range())*.111111)
			       ,(float) scalebar_low_value);
      bar->scale->ModRange(range() + incr);
    }
    UpdateScaleValues();
    UpdatePads();
    Adjust();
  }

};

void ScaleBar::Decr_Range(){
  int i = 1; // used to be a param
  if (adjustflag) {
    GetScaleValues();
    int j;
    for(j=0;j<i;j++){
      float decr =  ::max((fabs((double) range() )*.1),scalebar_low_value);
      bar->scale->ModRange(range() - decr);
    }
    UpdateScaleValues();
    UpdatePads();
    Adjust();
  }
};

void ScaleBar::Incr_Min(){
  int i = 1; // used to be a param
  if (adjustflag) {
    GetScaleValues();
    int j;
    float min_t = min();
    float newmin = min_t;
    for(j=0;j<i;j++) {
      newmin = min_t  + ::max((float)(fabs((double) min_t)*.111111)
				    ,(float) scalebar_low_value);
      if(newmin > max()) newmin = max();
    }
    bar->scale->SetMinMax(newmin, max());
    UpdateScaleValues();
    UpdatePads();
    Adjust();

  }
}
void ScaleBar::Decr_Min(){
  int i = 1; // used to be a param
  if (adjustflag) {
    GetScaleValues();
    int j;
    float min_t = min();
    for (j=0;j<i;j++) {
      min_t -= ::max((fabs((double) min_t)*.1),scalebar_low_value);
    }
    bar->scale->SetMinMax(min_t, max());
    UpdateScaleValues();
    UpdatePads();
    Adjust();
  }
}

void ScaleBar::Incr_Max(){
  int i = 1; // used to be a param
  if (adjustflag) {
    GetScaleValues();
    int j;
    float max_t = max();
    for(j=0;j<i;j++) {
      max_t += ::max((fabs((double)max_t) * 0.111111),scalebar_low_value);
    }
    bar->scale->SetMinMax(min(), max_t);
    UpdateScaleValues();
    UpdatePads();
    Adjust();
  }
}
void ScaleBar::Decr_Max(){
  int i = 1; // used to be a param
  if (adjustflag) {
    GetScaleValues();
    int j;
    float max_t = max();
    float newmax = max_t;
    for(j=0;j<i;j++) {
      newmax = max_t - ::max((fabs((double) max_t )*.1),scalebar_low_value);
      if(newmax < min()) newmax = min();
      max_t = newmax;
    }
    bar->scale->SetMinMax(min(), max_t);
    UpdateScaleValues();
    UpdatePads();
    Adjust();
  }
}
void ScaleBar::SetRange(float val){
  bar->scale->ModRange(val);
  UpdateScaleValues();
};

void ScaleBar::SetColorScale(ColorScale* c){
  if(bar != NULL) bar->SetColorScale(c);
}

void ScaleBar::SetMinMax(float mn,float mx){
  if ((mn == min()) && (mx == max())) return;
  bar->scale->SetMinMax(mn, mx);
  UpdateScaleValues();
}

// rounds up the range to the nearest 1,2,or 5 value
void ScaleBar::SetRoundRange(float val){
  bar->scale->ModRange(val);
  UpdateScaleValues();
}

/*void ScaleBar::UpdateMinMax(float mn, float mx) {
  if(mn < min) min = mn;
  if(mx > max) max = mx;
  FixRangeZero();
} */

void ScaleBar::UpdateScaleValues(){
  String min_str(min(), "%5.3f");
  String max_str(max(), "%5.3f");

  min_frep->setText(min_str);
  max_frep->setText(max_str);
  UpdatePads();
}


//////////////////////////
//    HCScaleBar	//
//////////////////////////

HCScaleBar::HCScaleBar(ColorScale* c, SpanMode sm, bool adj, bool ed, QWidget* parent)
:ScaleBar(true, sm, adj, ed, parent)
{
  Init(c);
}

void HCScaleBar::Init(ColorScale* c) {
  bar = new HCBar(c, this);
  InitLayout();
  UpdateScaleValues();
}


//////////////////////////
//    VCScaleBar	//
//////////////////////////

VCScaleBar::VCScaleBar(ColorScale* c, SpanMode sm, bool adj, bool ed, QWidget* parent)
:ScaleBar(false, sm, adj, ed, parent)
{
  Init(c);
}

void VCScaleBar::Init(ColorScale* c) {
  bar = new VCBar(c, this);
  InitLayout();
  UpdateScaleValues();
}


//////////////////////////
//    ColorMatrixGrid	//
//////////////////////////

ColorMatrixGrid::ColorMatrixGrid(QWidget* parent)
:inherited(parent)
{
  Init();
}

void ColorMatrixGrid::Init() {
  m_cellSize = 3;
}

void ColorMatrixGrid::paintEvent(QPaintEvent* event) {
  QPainter painter(this);
//TEMP
  painter.setPen(Qt::blue);
  painter.setFont(QFont("Arial", 10));
  painter.drawText(rect(), Qt::AlignCenter, "(Colorgrid TBD)");
//</TEMP>
}

void ColorMatrixGrid::setCellSize(int value) {
  if (value < 0) value = 0;
  if (m_cellSize == value) return;
  m_cellSize = value;
  update();
}



/*
PScaleBar::PScaleBar(float mn, float mx,bool adj,bool ed)
: ScaleBar(mn,mx,adj,ed) {
  tagpads = NULL;
};vGlyph* HCScaleBar::GetLook(){
  ivWidgetKit* wkit = ivWidgetKit::instance();
  ivLayoutKit* layout = ivLayoutKit::instance();
  if(adjustflag){
    if(sm == RANGE) {
      scalebar =
	(wkit->inset_frame
	 (layout->hbox
	  (layout->vcenter(min_label,0),
	   layout->vcenter(wkit->outset_frame
			   (layout->vflexible(bar->GetBar())),0),
	   layout->vcenter(max_label,0),
	   layout->vcenter
	   (layout->vflexible(layout->natural(shrinker,24,16)),0),
	   layout->vcenter
	   (layout->vflexible(layout->natural(enlarger,24,16)),0)
	   )));
    }
    else {
      scalebar =
	(wkit->inset_frame
	 (layout->hbox
	  (layout->vcenter
	   (layout->vbox
	    (layout->hcenter(layout->natural(min_incr,24,16),0),
	     layout->hcenter(layout->natural(min_decr,24,16),0)),0),
	   layout->vcenter(min_label,0),
	   layout->vcenter(wkit->outset_frame
			   (layout->vflexible(bar->GetBar())),0),
	   layout->vcenter(max_label,0),
	   layout->vcenter
	   (layout->vbox
	    (layout->hcenter(layout->natural(max_incr,24,16),0),
	     layout->hcenter(layout->natural(max_decr,24,16),0)),0)
	   )));
    }
  }
  else {
    if(sm == RANGE) {
      scalebar =
	(wkit->inset_frame
	 (layout->hbox
	  (layout->vcenter(min_label,0),
	   layout->vcenter(wkit->outset_frame
                           (layout->vflexible(bar)),0),
	   layout->vcenter(max_label,0)
	  )));
    }
    else {
      scalebar =
	(wkit->inset_frame
	 (layout->hbox
	  (layout->vcenter(min_label,0),
	   layout->vcenter(wkit->outset_frame
                           (layout->vflexible(bar)),0),
	   layout->vcenter(max_label,0)
	    )));
    }
  }
  return GenIH(scalebar);
}



PScaleBar::PScaleBar(float r,bool adj,bool ed)
: ScaleBar(r,adj,ed) {
  tagpads=NULL;
};

void PScaleBar::Destroy()
{
   if (tagpads) { ivResource::unref(tagpads); tagpads = NULL; }
   if (fbox) { ivResource::unref(fbox); fbox = NULL; }
}

ivGlyph* PScaleBar::GenIH(ivGlyph* g){
  ivGlyph* rg = ScaleBar::GenIH(g);
  if(ihan){
    int i;
    for(i=0;i<4;i++){
      ihan->append_input_handler(((ivInputHandler *) fbox->component(i)));
      ihan->focus(((ivInputHandler *) fbox->component(i)));
    }
  }
  return rg;
}

void PScaleBar::MakeTags(){
  ivLayoutKit* layout = ivLayoutKit::instance();
  ivWidgetKit* wkit = ivWidgetKit::instance();
  ivDialogKit*  dkit = ivDialogKit::instance();

  int i;
  tag[0] = -1; tag[1] = 0; tag[2] = .5; tag[3]= 1;
  fbox = layout->hbox();
  ivResource::ref(fbox);
  for(i=0;i<4;i++){
    stag[i] = "";
    stag[i] += String(tag[i]);
    fbox->append(dkit->field_editor
		 ((char *) stag[i],wkit->style(),
		  new FieldEditorCallback(PScaleBar)
		  (this,&PScaleBar::Get_Tags,&PScaleBar::Set_Tags)
		  ));
  }
}

void PScaleBar::MakePads(){
  ivWidgetKit* wkit = ivWidgetKit::instance();
  ivLayoutKit* layout = ivLayoutKit::instance();

  if(!tagpads) {
    tagpads=layout->hbox();
    ivResource::ref(tagpads);
  }
  int i;
  for(i=0;i<4;i++){
    ivTelltaleState* bstate =
      new ivTelltaleState(ivTelltaleState::is_enabled_visible |
			ivTelltaleState::is_choosable);
    bstate->join(((PBar *)bar)->grp);
    iColor* selectcolor = new iColor(1.0,1.0,1.0,1.0);
    pb[i] = new PadButton(wkit->style(),this);
    pb[i]->Set(tag[i]);
    ivGlyph* coloroff = layout->margin (wkit->outset_frame(pb[i]),1);
    ivGlyph* coloron = new ivBorder(layout->margin(wkit->outset_frame(pb[i]),1),selectcolor,1);
    ivButton* tagbut =  new ivButton
      (new ivChoiceItem
       (bstate,coloroff,
	coloroff,coloroff,coloron,coloron,
	coloron,coloron,coloron,coloron,
	coloroff),wkit->style(),bstate,NULL);
    tagpads->append(tagbut);
    if(i==3) bstate->set(ivTelltaleState::is_chosen,true);
  }
}

void PScaleBar::SetColorScale(ColorScale* c) {
  ScaleBar::SetColorScale(c);
  // also all the tag's colorpads
  int i;
  for(i=0;i<4;i++){
    pb[i]->cp->Reset(); // force a change
  }
}

int PScaleBar::GetSelected() {
  int result =((PBar *) bar)->GetSelected();
  if(result) {
    return result;
  }
  else {
    int i;
    for(i=0;i<4;i++){
      if(((ivButton *)(tagpads->component(i)))->state()->test
	 (ivTelltaleState::is_chosen)) {
	float pushval = atof(min_frep->text()->string());
	return GetIdx(pushval);
      }
    }
  }
  return -1;
}


void PScaleBar::GetScaleValues(){
  ScaleBar::GetScaleValues();
  Get_Tags(NULL);
}

void PScaleBar::Get_Tags(ivFieldEditor*){
  int i;
  for(i=0;i<4;i++){
    float pushval =
      atof(((ivFieldEditor*)fbox->component(i))->text()->string());
    pb[i]->Set(pushval);
    tag[i] = pushval;
    stag[i] = "";
    stag[i] += String(tag[i]);
  }
}

void PScaleBar::Set_Tags(ivFieldEditor*){
  int i;
  for(i=0;i<4;i++){
    pb[i]->Set(tag[i]);
    ((ivFieldEditor *) fbox->component(i))->field((char *) stag[i]);
    ((ivFieldEditor *) fbox->component(i))->select(0);
  }
}

float PScaleBar::GetSelectedVal() {
  int sval = ((PBar *) bar)->GetSelected();
  if(sval != -1) {
    return (GetVal(sval));
  }
  else {
    int i;
    for(i=0;i<4;i++){
      if(((ivButton *)(tagpads->component(i)))->state()->test
	 (ivTelltaleState::is_chosen)) {
	float pushval =
	  atof(((ivFieldEditor*)fbox->component(i))->text()->string());
	return pushval;
      }
    }
  }
  return 0; // ??
}



HPScaleBar::HPScaleBar(float amin, float amax, bool adj, bool ed,ColorScale* c,
		       int b,int h,int w)
: PScaleBar(amin,amax,adj,ed) {
  bar = new HPBar(c,b,h,w);
  ivResource::ref(bar);
  Initialize();
  MakeTags();
  MakePads();
};

HPScaleBar::HPScaleBar(float r,bool adj,bool ed,ColorScale* c,
	   int b,int h, int w)
: PScaleBar(r,adj,ed) {
  bar = new HPBar(c,b,h,w);
  ivResource::ref(bar);
  Initialize();
  MakeTags();
  MakePads();
}

ivGlyph* HPScaleBar::GetLook(){
  ivWidgetKit* wkit = ivWidgetKit::instance();
  ivLayoutKit* layout = ivLayoutKit::instance();
  ivGlyph* tagbox = layout->hbox();
  int i;
  for(i=0;i<4;i++){
    tagbox->append
      (layout->vcenter
       (wkit->outset_frame
	(layout->hbox(layout->vcenter(tagpads->component(i),0),
		      layout->vcenter(fbox->component(i),0))),0));
  }
  if(adjustflag){
    if(sm == RANGE) {
      scalebar =
	(wkit->inset_frame
	 (layout->hbox
	  (layout->vcenter
	   (layout->vflexible(layout->natural(shrinker,24,16)),0),
	   layout->vcenter(min_label,0),
	   layout->vcenter(wkit->outset_frame
			   (layout->vflexible(bar->GetBar())),0),
	   layout->vcenter(max_label,0),
	   layout->vcenter
	   (layout->vflexible(layout->natural(enlarger,24,16)),0)
	   )));
    }
    else {
      scalebar =
	(wkit->inset_frame
	 (layout->vbox
	  (layout->hcenter(tagbox,0),
	   layout->hbox
	   (layout->vcenter
	    (layout->vbox
	     (layout->hcenter(layout->natural(min_incr,24,16),0),
	      layout->hcenter(layout->natural(min_decr,24,16),0)),0),
	    layout->vcenter(min_label,0),
	    layout->vcenter(wkit->outset_frame
			    (layout->vflexible(bar->GetBar())),0),
	    layout->vcenter(max_label,0),
	    layout->vcenter
	    (layout->vbox
	     (layout->hcenter(layout->natural(max_incr,24,16),0),
	      layout->hcenter(layout->natural(max_decr,24,16),0)),0)
	    ))));
    }
  }
  else {
    if(sm == RANGE) {
      scalebar =
	(wkit->inset_frame
	 (layout->hbox
	  (layout->vcenter(min_label,0),
	   layout->vcenter(wkit->outset_frame
                           (layout->vflexible(bar->GetBar())),0),
	   layout->vcenter(max_label,0)
	  )));
    }
    else {
      scalebar =
	(wkit->inset_frame
	 (layout->hbox
	  (layout->vcenter(min_label,0),
	   layout->vcenter(wkit->outset_frame
                           (layout->vflexible(bar->GetBar())),0),
	   layout->vcenter(max_label,0)
	    )));
    }
  }
  return GenIH(scalebar);
}


VPScaleBar::VPScaleBar(float amin, float amax, bool adj, bool ed,ColorScale* c,
		       int b,int h,int w)
: PScaleBar(amin,amax,adj,ed) {
  bar = new VPBar(c,b,h,w);
  ivResource::ref(bar);
  Initialize();
  MakeTags();
  MakePads();
};


VPScaleBar::VPScaleBar(float r,bool adj,bool ed,ColorScale* c,
	   int b,int h, int w)
: PScaleBar(r,adj,ed) {
  bar = new VPBar(c,b,h,w);
  ivResource::ref(bar);
  Initialize();
  MakeTags();
  MakePads();
}

ivGlyph* VPScaleBar::GetLook(){
  ivWidgetKit* wkit = ivWidgetKit::instance();
  ivLayoutKit* layout = ivLayoutKit::instance();
  if(adjustflag) {
    if(sm == RANGE) {
      scalebar =
	(layout->vbox
	 (layout->hcenter
	  (layout->hflexible(layout->natural(enlarger,32,20)),0),
	  layout->hcenter(min_label,0),
	  layout->hcenter
	  (wkit->outset_frame
	   (layout->hflexible(bar->GetBar())),0),
	  layout->hcenter(max_label,0),
	  layout->hcenter
	  (layout->hflexible(layout->natural(shrinker,32,20)),0)
	  ));
    }
    else {
      scalebar =
	(wkit->inset_frame
	 (layout->vbox
	  (layout->hcenter
	   (layout->vbox
	    (layout->hcenter(layout->natural(min_incr,32,20),0),
	     layout->hcenter(layout->natural(min_decr,32,20),0)),0),
	   layout->hcenter(min_label,0),
	   layout->hcenter(wkit->outset_frame
			   (layout->hflexible(bar->GetBar())),0),
	   layout->hcenter(max_label,0),
	   layout->hcenter
	   (layout->vbox
	    (layout->hcenter(layout->natural(max_incr,32,20),0),
	     layout->hcenter(layout->natural(max_decr,32,20),0)),0)
	   )));
    }
  }
  else {
    if(sm == RANGE) {
      scalebar =
	(wkit->inset_frame
	 (layout->vbox
	  (layout->hcenter(min_label,0),
	   layout->hcenter(wkit->outset_frame
                           (layout->hflexible(bar)),0),
	   layout->hcenter(max_label,0)
	  )));
    }
    else {
      scalebar =
	(wkit->inset_frame
	 (layout->vbox
	  (layout->hcenter(min_label,0),
	   layout->hcenter(wkit->outset_frame
                           (layout->hflexible(bar)),0),
	   layout->hcenter(max_label,0)
	    )));
    }
  }
  return GenIH(scalebar);
}

DynamicBackground::DynamicBackground(ivGlyph* body, const iColor* c) : ivMonoGlyph(body) {
    color_ = c;
    ivResource::ref(color_);
}

DynamicBackground::~DynamicBackground() {
    ivResource::unref(color_);
}

void DynamicBackground::allocate(ivCanvas* c, const ivAllocation& a, ivExtension& ext) {
    ivMonoGlyph::allocate(c, a, ext);
    ext.merge(c, a);
}

void DynamicBackground::draw(ivCanvas* c, const ivAllocation& a) const {
    ivExtension ext;
    ext.set(c, a);
    if (c->damaged(ext)) {
	c->fill_rect(a.left(), a.bottom(), a.right(), a.top(), color_);
    }
    ivMonoGlyph::draw(c, a);
}

void DynamicBackground::print(ivPrinter* p, const ivAllocation& a) const {
    p->fill_rect(a.left(), a.bottom(), a.right(), a.top(), color_);
    ivMonoGlyph::print(p, a);
}

void DynamicBackground::SetColor(iColor* c){
  ivResource::ref(c);
  ivResource::unref(color_);
  color_ = c;
}





PadButton::PadButton(ivStyle* s,PScaleBar* sb,
		     ColorPad::BlockFill sh,int w, int h)
: ivActiveHandler(cp = new ColorPad(sb,sh,w,h),s) {
}

void PadButton::press(const ivEvent&) {
  cp->Toggle();
}

ivCursor* PaintPad::painter_cursor;

PaintPad::PaintPad(ColorPad* c,void* o, void (*cn)(void*))
: ivInputHandler(new ivPatch(c),  ivWidgetKit::instance()->style()) {
  pb = c;
  obj = o;
  change_notify = cn;
  if (painter_cursor == NULL) {
    ivBitmap* painter = new ivBitmap(painter_bits, painter_width, painter_height,
				painter_x_hot, painter_y_hot);
    ivBitmap* painter_mask = new ivBitmap(painterMask_bits, painterMask_width, painterMask_height,
				     painterMask_x_hot, painterMask_y_hot);
    painter_cursor = new ivCursor(painter, painter_mask);
  }
}


void PaintPad::release(const ivEvent& e){
  ivWindow* win = e.window();
  if (win->cursor() == painter_cursor) {
    win->pop_cursor();
  }
}


void PaintPad::press(const ivEvent& e) {
  ivInputHandler::press(e);
  pb->Toggle();
  if(change_notify != NULL) change_notify(obj);
  ivWindow* win = e.window();
  if (win->cursor() != painter_cursor) {
    win->push_cursor();
    win->cursor(painter_cursor);
  }
};

void PaintPad::drag(const ivEvent& e) {
  ivInputHandler::drag(e);
  ivHandler* h = handler();
  e.ungrab(h);
};

void PaintPad::move(const ivEvent& e) {
  if(e.left_is_down()) {
    pb->Set(((PScaleBar *) pb->sb)->GetSelectedVal());
    if(change_notify != NULL) change_notify(obj);
  }
  else  if(e.middle_is_down()) {
    pb->Set(pb->oldpadval);
    if(change_notify != NULL) change_notify(obj);
  }
  else {
    // turn off painter cursor if mouse is not down
    ivWindow* win = e.window();
    if (win->cursor() == painter_cursor) {
      win->pop_cursor();
    }
  }
}


//////////////////////////
// 	CachePatch	//
//////////////////////////

// the cache patch always requests its first request.
// it is used for the linespace/axes box which changes size
// see docs/graphlogviewnotes for details

CachePatch::CachePatch(ivGlyph* body) : ivPatch(body) {
  impl = new CachePatch_Impl;
  impl->fakeflag = CachePatch_Impl::FIRST;
}

void CachePatch::request(ivRequisition& r) const {
  switch(impl->fakeflag) {
  case CachePatch_Impl::FIRST:
    ivPatch::request(r); // requset and store
    impl->oldreq = r;
    impl->fakeflag = CachePatch_Impl::ALWAYS;
    break;
  case CachePatch_Impl::ONCE:
    ivPatch::request(r); // request but don't store
    impl->fakeflag = CachePatch_Impl::ALWAYS;
    break;
  case CachePatch_Impl::ALWAYS:
    r = impl->oldreq; // use initial request
    break;
  }
  return;
}
*/
