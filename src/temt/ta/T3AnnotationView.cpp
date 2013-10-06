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

#include "T3AnnotationView.h"

#include <T3Annotation>
#include <T3ExaminerViewer>
#include <T3DataViewMain>
#include <T3AnnotationNode>
#include <taMath_float>

#include <taMisc>

#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>


void T3AnnotationView::Initialize(){
  data_base = &TA_T3Annotation;
}

void T3AnnotationView::Destroy() {
  CutLinks();
}

void T3AnnotationView::SetAnno(T3Annotation* ob) {
  if (Anno() == ob) return;
  SetData(ob);
  if (ob) {
    if (!name.contains(ob->name)) {
      SetName(ob->name);
    }
  }
}

void T3AnnotationView::Render_pre() {
  T3ExaminerViewer* vw = GetViewer();
  bool show_drag = false;
  if(vw)
    show_drag = vw->interactionModeOn();

  T3DataViewMain* nv = GET_MY_OWNER(T3DataViewMain);
  if(nv && !nv->ShowDraggers()) show_drag = false;

  setNode(new T3AnnotationNode(this, show_drag));
  SoSeparator* ssep = node_so()->shapeSeparator();

  T3Annotation* ob = Anno();

  ob->rend_type = ob->type;

  if(ob) {
    switch(ob->type) {
    case T3Annotation::LINE:
    case T3Annotation::ARROW:
    case T3Annotation::DOUBLEARROW:
    case T3Annotation::RECTANGLE: {
      SoDrawStyle* ds = new SoDrawStyle();
      ssep->addChild(ds);
      SoLineSet* line = new SoLineSet();
      line->vertexProperty.setValue(new SoVertexProperty());
      ssep->addChild(line);
      break;
    }
    case T3Annotation::TEXT: {
      SoSeparator* tsep = new SoSeparator();
      SoComplexity* cplx = new SoComplexity;
      cplx->value.setValue(taMisc::text_complexity);
      tsep->addChild(cplx);
      SoFont* fnt = new SoFont();
      fnt->name = (const char*)taMisc::t3d_font_name;
      fnt->size.setValue(ob->font_size);
      tsep->addChild(fnt);
      SoAsciiText* txt = new SoAsciiText();
      SoMFString* mfs = &(txt->string);
      mfs->setValue(ob->text.chars());
      tsep->addChild(txt);
      ssep->addChild(tsep);
      break;
    }
    case T3Annotation::OBJECT: {
      bool got_one = false;
      int acc = access(ob->obj_fname, F_OK);
      if (acc == 0) {
        SoInput in;
        if ((access(ob->obj_fname, F_OK) == 0) && in.openFile(ob->obj_fname)) {
          SoSeparator* root = SoDB::readAll(&in);
          if (root) {
            ssep->addChild(root);
            got_one = true;
          }
        }
      }
      if(!got_one) {
        taMisc::Warning("object file:", ob->obj_fname, "not found or unable to be loaded!");
      }
      break;
    }
    }
  }
  inherited::Render_pre();
}

void T3AnnotationView::Render_impl() {
  inherited::Render_impl();

  T3AnnotationNode* node_so = (T3AnnotationNode*)this->node_so(); // cache
  if(!node_so) return;
  T3Annotation* ob = Anno();
  if(!ob) return;

  SoSeparator* ssep = node_so->shapeSeparator();

  SoTransform* tx = node_so->transform();
  tx->translation.setValue(ob->pos.x, ob->pos.y, -ob->pos.z);
  tx->scaleFactor.setValue(ob->scale.x, ob->scale.y, ob->scale.z);
  tx->rotation.setValue(SbVec3f(ob->rot.x, ob->rot.y, ob->rot.z), ob->rot.rot);

  SoMaterial* mat = node_so->material();
  mat->diffuseColor.setValue(ob->color.r, ob->color.g, ob->color.b);
  mat->transparency.setValue(1.0f - ob->color.a);

  if(ob->type != ob->rend_type) return; // not safe!

  switch(ob->type) {
  case T3Annotation::LINE:
  case T3Annotation::ARROW:
  case T3Annotation::DOUBLEARROW: {
    SoDrawStyle* ds = (SoDrawStyle*)ssep->getChild(ssep->getNumChildren()-2);
    ds->lineWidth.setValue(ob->line_width);
    SoLineSet* line = (SoLineSet*)ssep->getChild(ssep->getNumChildren()-1);

    line->numVertices.setNum(0);
    SoMFVec3f& point = ((SoVertexProperty*)line->vertexProperty.getValue())->vertex;
    point.setNum(0);

    line->numVertices.startEditing();
    point.startEditing();

    // add the two new line vertices
    int pt_idx = point.getNum();
    point.set1Value(pt_idx++, ob->start.x, ob->start.y, -ob->start.z);
    point.set1Value(pt_idx++, ob->end.x, ob->end.y, -ob->end.z);
    // add num of vertices (2) of this new line
    line->numVertices.set1Value(line->numVertices.getNum(), 2);

    if(ob->type >= T3Annotation::ARROW) {
      float ang = taMath_float::atan2(ob->end.y - ob->start.y, ob->end.x - ob->start.x);
      float aa1 = ang + .75f * taMath_float::pi;
      float aa2 = ang - .75f * taMath_float::pi;
      point.set1Value(pt_idx++, ob->end.x, ob->end.y, -ob->end.z);
      point.set1Value(pt_idx++,
                      ob->end.x + ob->arrow_size * taMath_float::cos(aa1),
                      ob->end.y + ob->arrow_size * taMath_float::sin(aa1), -ob->end.z);
      // add num of vertices (2) of this new line
      line->numVertices.set1Value(line->numVertices.getNum(), 2);

      point.set1Value(pt_idx++, ob->end.x, ob->end.y, -ob->end.z);
      point.set1Value(pt_idx++,
                      ob->end.x + ob->arrow_size * taMath_float::cos(aa2),
                      ob->end.y + ob->arrow_size * taMath_float::sin(aa2), -ob->end.z);
      // add num of vertices (2) of this new line
      line->numVertices.set1Value(line->numVertices.getNum(), 2);

      if(ob->type == T3Annotation::DOUBLEARROW) {
        float ang = taMath_float::atan2(ob->start.y - ob->end.y, ob->start.x - ob->end.x);
        float aa1 = ang + .75f * taMath_float::pi;
        float aa2 = ang - .75f * taMath_float::pi;
        point.set1Value(pt_idx++, ob->start.x, ob->start.y, -ob->start.z);
        point.set1Value(pt_idx++,
                        ob->start.x + ob->arrow_size * taMath_float::cos(aa1),
                        ob->start.y + ob->arrow_size * taMath_float::sin(aa1),
                        -ob->start.z);
        // add num of vertices (2) of this new line
        line->numVertices.set1Value(line->numVertices.getNum(), 2);

        point.set1Value(pt_idx++, ob->start.x, ob->start.y, -ob->start.z);
        point.set1Value(pt_idx++,
                        ob->start.x + ob->arrow_size * taMath_float::cos(aa2),
                        ob->start.y + ob->arrow_size * taMath_float::sin(aa2),
                        -ob->start.z);
        // add num of vertices (2) of this new line
        line->numVertices.set1Value(line->numVertices.getNum(), 2);
      }
    }

    point.finishEditing();
    line->numVertices.finishEditing();
    break;
  }
  case T3Annotation::RECTANGLE: {
    SoDrawStyle* ds = (SoDrawStyle*)ssep->getChild(ssep->getNumChildren()-2);
    ds->lineWidth.setValue(ob->line_width);
    SoLineSet* line = (SoLineSet*)ssep->getChild(ssep->getNumChildren()-1);

    line->numVertices.setNum(0);
    SoMFVec3f& point = ((SoVertexProperty*)line->vertexProperty.getValue())->vertex;
    point.setNum(0);

    line->numVertices.startEditing();
    point.startEditing();

    // add the two new line vertices
    int pt_idx = point.getNum();
    point.set1Value(pt_idx++, ob->bot_left.x, ob->bot_left.y, -ob->bot_left.z);
    point.set1Value(pt_idx++, ob->bot_left.x, ob->top_right.y, -ob->top_right.z);
    point.set1Value(pt_idx++, ob->top_right.x, ob->top_right.y, -ob->top_right.z);
    point.set1Value(pt_idx++, ob->top_right.x, ob->bot_left.y, -ob->bot_left.z);
    point.set1Value(pt_idx++, ob->bot_left.x, ob->bot_left.y, -ob->bot_left.z);
    // add num of vertices (5) of this new line
    line->numVertices.set1Value(line->numVertices.getNum(), 5);

    point.finishEditing();
    line->numVertices.finishEditing();
    break;
  }
  case T3Annotation::TEXT: {
    SoSeparator* tsep = (SoSeparator*)ssep->getChild(ssep->getNumChildren()-1);
    SoFont* fnt = (SoFont*)tsep->getChild(1);
    fnt->size.setValue(ob->font_size);
    SoAsciiText* txt = (SoAsciiText*)tsep->getChild(2);
    SoMFString* mfs = &(txt->string);
    mfs->setValue(ob->text.chars());
    break;
  }
  case T3Annotation::OBJECT: {
    break;
  }
  }
}

// callback for netview transformer dragger
void T3AnnotationNode_DragFinishCB(void* userData, SoDragger* dragr) {
  SoTransformBoxDragger* dragger = (SoTransformBoxDragger*)dragr;
  T3AnnotationNode* nvoso = (T3AnnotationNode*)userData;
  T3AnnotationView* nvov = static_cast<T3AnnotationView*>(nvoso->dataView());
  T3Annotation* nvo = nvov->Anno();
  T3DataViewMain* nv = GET_OWNER(nvov, T3DataViewMain);

  SbRotation cur_rot;
  cur_rot.setValue(SbVec3f(nvo->rot.x, nvo->rot.y, nvo->rot.z), nvo->rot.rot);

  SbVec3f trans = dragger->translation.getValue();
  cur_rot.multVec(trans, trans); // rotate the translation by current rotation
  trans[0] *= nvo->scale.x;  trans[1] *= nvo->scale.y;  trans[2] *= nvo->scale.z;
  taVector3f tr(trans[0], trans[1], -trans[2]);
  nvo->pos += tr;

  const SbVec3f& scale = dragger->scaleFactor.getValue();
  taVector3f sc(scale[0], scale[1], scale[2]);
  if(sc < .1f) sc = .1f;        // prevent scale from going to small too fast!!
  nvo->scale *= sc;

  SbVec3f axis;
  float angle;
  dragger->rotation.getValue(axis, angle);
  if(axis[0] != 0.0f || axis[1] != 0.0f || axis[2] != 1.0f || angle != 0.0f) {
    SbRotation rot;
    rot.setValue(SbVec3f(axis[0], axis[1], axis[2]), angle);
    SbRotation nw_rot = rot * cur_rot;
    nw_rot.getValue(axis, angle);
    nvo->rot.SetXYZR(axis[0], axis[1], axis[2], angle);
  }

//   float h = 0.04f; // nominal amount of height, so we don't vanish
  nvoso->txfm_shape()->scaleFactor.setValue(1.0f, 1.0f, 1.0f);
  nvoso->txfm_shape()->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  nvoso->txfm_shape()->translation.setValue(0.0f, 0.0f, 0.0f);
  dragger->translation.setValue(0.0f, 0.0f, 0.0f);
  dragger->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 0.0f);
  dragger->scaleFactor.setValue(1.0f, 1.0f, 1.0f);

  nvo->UpdateAfterEdit();
  //  nv->Refresh();
}

