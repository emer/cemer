// Copyright 2017, Regents of the University of Colorado,
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

#include "iViewPanelOfNetwork.h"
#include <NetView>
#include <Network>
#include <Layer>
#include <BaseSpec>
#include <T3ExaminerViewer>
#include <taiWidgetField>
#include <taiWidgetComboBox>
#include <taiWidgetGroupElChooser>
#include <iHColorScaleBar>
#include <iTreeView>
#include <iTreeViewItem>
#include <iMethodButtonMgr>
#include <iLineEdit>
#include <iFlowLayout>
#include <iTreeListWidget>

#include <taMisc>
#include <taiMisc>

#include <QVBoxLayout>
#include <QCheckBox>
#include <QToolBar>
#include <QLabel>
#include <QPushButton>
#include <QAction>
#include <QTabWidget>
#include <QPushButton>

NetView* iViewPanelOfNetwork::getNetView() {
  return static_cast<NetView *>(m_dv);
}

iViewPanelOfNetwork::iViewPanelOfNetwork(NetView* dv_)
:inherited(dv_)
{
//  int font_spec = taiMisc::fonMedium;
  int font_spec = taiMisc::fonBig;
  m_cur_spec = NULL;
  req_full_render = false;
  req_full_build = false;

  T3ExaminerViewer* vw = dv_->GetViewer();
  if(vw) {
    connect(vw, SIGNAL(dynbuttonActivated(int)), this, SLOT(dynbuttonActivated(int)) );
    connect(vw, SIGNAL(unTrappedKeyPressEvent(QKeyEvent*)), this, SLOT(unTrappedKeyPressEvent(QKeyEvent*)) );
  }
  
  widg = new QWidget();
  layTopCtrls = new QVBoxLayout(widg); //layWidg->addLayout(layTopCtrls);
  layTopCtrls->setSpacing(2);
  layTopCtrls->setMargin(2);

  layViewParams = new QVBoxLayout(); layTopCtrls->addLayout(layViewParams);
  layViewParams->setSpacing(2);
  layViewParams->setMargin(0);

  ////////////////////////////////////////////////////////////////////////////
  layDispCheck = new QHBoxLayout();  layViewParams->addLayout(layDispCheck);
  chkDisplay = new QCheckBox("Display", widg);
  connect(chkDisplay, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layDispCheck->addWidget(chkDisplay);
  layDispCheck->addSpacing(taiM->hsep_c);

  lblLayLayout = taiM->NewLabel("2/3D:", widg, font_spec);
  lblLayLayout->setToolTip(taiMisc::ToolTipPreProcess("Use 2D or 3D layout of layers (2D is big flat plane, 3D is stacked layer planes)"));
  layDispCheck->addWidget(lblLayLayout);
  cmbLayLayout = dl.Add
    (new taiWidgetComboBox(true, TA_NetView.sub_types.FindName("LayerLayout"),
                           this, NULL, widg, taiWidget::flgAutoApply));
  layDispCheck->addWidget(cmbLayLayout->GetRep());
  layDispCheck->addSpacing(taiM->hsep_c);

  chkLayMove = new QCheckBox("Lay\nMv", widg);
  chkLayMove->setToolTip(taiMisc::ToolTipPreProcess("Turn on the layer moving controls when in the manipulation mode (red arrow) of viewer -- these can sometimes interfere with viewing weights, so you can turn them off here (but then you won't be able to move layers around in the GUI)"));
  connect(chkLayMove, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layDispCheck->addWidget(chkLayMove);
  layDispCheck->addSpacing(taiM->hschk_c);

  chkNetText = new QCheckBox("Net\nText", widg);
  chkNetText->setToolTip(taiMisc::ToolTipPreProcess("Display the current state of various counters and stats"));
  connect(chkNetText, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layDispCheck->addWidget(chkNetText);
  layDispCheck->addSpacing(taiM->hschk_c);

  lblUnitText = taiM->NewLabel("Unit:\nText", widg, font_spec);
  lblUnitText->setToolTip(taiMisc::ToolTipPreProcess("What text to display for each unit (values, names)"));
  layDispCheck->addWidget(lblUnitText);
  cmbUnitText = dl.Add(new taiWidgetComboBox(true, TA_NetView.sub_types.FindName("UnitTextDisplay"),
                                this, NULL, widg, taiWidget::flgAutoApply));
  layDispCheck->addWidget(cmbUnitText->GetRep());
  layDispCheck->addSpacing(taiM->hschk_c);

  lblDispMode = taiM->NewLabel("Style", widg, font_spec);
  lblDispMode->setToolTip(taiMisc::ToolTipPreProcess("How to display unit values.  3d Block (default) is optimized for maximum speed."));
  layDispCheck->addWidget(lblDispMode);
  cmbDispMode = dl.Add(new taiWidgetComboBox(true, TA_NetView.sub_types.FindName("UnitDisplayMode"),
    this, NULL, widg, taiWidget::flgAutoApply));
  layDispCheck->addWidget(cmbDispMode->GetRep());
  layDispCheck->addSpacing(taiM->hsep_c);

  lblPrjnDisp = taiM->NewLabel("Prjn\nDisp", widg, font_spec);
  lblPrjnDisp->setToolTip("How to display projections between layers:\n\
L_R_F: Left = sender, Right = receiver, all arrows at the Front of the layer\n\
L_R_B: Left = sender, Right = receiver, all arrows at the Back of the layer\n\
B_F: Back = sender, Front = receiver, all arrows in the middle of the layer");
  layDispCheck->addWidget(lblPrjnDisp);
  cmbPrjnDisp = dl.Add(new taiWidgetComboBox(true, TA_NetViewParams.sub_types.FindName("PrjnDisp"),
                                this, NULL, widg, taiWidget::flgAutoApply));
  layDispCheck->addWidget(cmbPrjnDisp->GetRep());
  layDispCheck->addStretch(10);

  ////////////////////////////////////////////////////////////////////////////
  layFontsEtc = new QHBoxLayout();  layViewParams->addLayout(layFontsEtc);

  lblConType = taiM->NewLabel("Con\nType:", widg, font_spec);
  lblConType->setToolTip(taiMisc::ToolTipPreProcess("Display only selected types of connections?"));
  layFontsEtc->addWidget(lblConType);
  cmbConType = dl.Add(new taiWidgetComboBox(true, 
		TA_NetView.sub_types.FindName("ConType"),
                               this, NULL, widg, taiWidget::flgAutoApply));
  layFontsEtc->addWidget(cmbConType->GetRep());
  layFontsEtc->addSpacing(taiM->hsep_c);

  chkShowIconified = new QCheckBox("Show\nIcon", widg);
  chkShowIconified->setToolTip(taiMisc::ToolTipPreProcess("Show iconified layers -- if this is off, then iconified layers are not displayed at all -- otherwise they are displayed with their name and optional iconified value, but projections are not displayed in any case"));
  connect(chkShowIconified, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layFontsEtc->addWidget(chkShowIconified);
  layFontsEtc->addSpacing(taiM->hschk_c);

  lblPrjnWdth = taiM->NewLabel("Prjn\nWdth", widg, font_spec);
  lblPrjnWdth->setToolTip(taiMisc::ToolTipPreProcess("Width of projection lines -- .002 is default (very thin!) -- increase if editing projections so they are easier to select."));
  layFontsEtc->addWidget(lblPrjnWdth);
  fldPrjnWdth = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layFontsEtc->addWidget(fldPrjnWdth->GetRep());
  ((iLineEdit*)fldPrjnWdth->GetRep())->setCharWidth(6);
  layFontsEtc->addSpacing(taiM->hsep_c);

  lblUnitTrans = taiM->NewLabel("Trans\nparency", widg, font_spec);
  lblUnitTrans->setToolTip(taiMisc::ToolTipPreProcess("Unit maximum transparency level: 0 = all units opaque; 1 = inactive units are completely invisible.\n .6 = default; transparency is inversely related to value magnitude."));
  layFontsEtc->addWidget(lblUnitTrans);
  fldUnitTrans = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layFontsEtc->addWidget(fldUnitTrans->GetRep());
  ((iLineEdit*)fldUnitTrans->GetRep())->setCharWidth(6);  layFontsEtc->addSpacing(taiM->hsep_c);
  layFontsEtc->addSpacing(taiM->hsep_c);

  lblUnitFont = taiM->NewLabel("Font\nSize", widg, font_spec);
  lblUnitFont->setToolTip(taiMisc::ToolTipPreProcess("Unit text font size (as a proportion of entire network display). .02 is default."));
  layFontsEtc->addWidget(lblUnitFont);
  fldUnitFont = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layFontsEtc->addWidget(fldUnitFont->GetRep());
  ((iLineEdit*)fldUnitFont->GetRep())->setCharWidth(6);
  layFontsEtc->addSpacing(taiM->hsep_c);

  lblLayFont = taiM->NewLabel("Layer\nFont Sz", widg, font_spec);
  lblLayFont->setToolTip(taiMisc::ToolTipPreProcess("Layer name font size (as a proportion of entire network display). .04 is default."));
  layFontsEtc->addWidget(lblLayFont);
  fldLayFont = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layFontsEtc->addWidget(fldLayFont->GetRep());
  ((iLineEdit*)fldLayFont->GetRep())->setCharWidth(6);
  layFontsEtc->addSpacing(taiM->hsep_c);

  lblMinLayFont = taiM->NewLabel("Min Sz", widg, font_spec);
  lblMinLayFont->setToolTip(taiMisc::ToolTipPreProcess("Minimum layer name font size (as a proportion of entire network display) -- prevents font from shrinking too small for small layers. .01 is default."));
  layFontsEtc->addWidget(lblMinLayFont);
  fldMinLayFont = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layFontsEtc->addWidget(fldMinLayFont->GetRep());
  ((iLineEdit*)fldMinLayFont->GetRep())->setCharWidth(6);
  layFontsEtc->addSpacing(taiM->hsep_c);

  chkXYSquare = new QCheckBox("XY\nSquare", widg);
  chkXYSquare->setToolTip(taiMisc::ToolTipPreProcess("Make the X and Y size of network the same, so that unit cubes are always square (but can waste a certain amount of display space)."));
  connect(chkXYSquare, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layFontsEtc->addWidget(chkXYSquare);
  layFontsEtc->addSpacing(taiM->hschk_c);
  
  chkLayGp = new QCheckBox("Lay\nGp", widg);
  chkLayGp->setToolTip(taiMisc::ToolTipPreProcess("Display boxes around layer groups."));
  connect(chkLayGp, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layFontsEtc->addWidget(chkLayGp);
  layFontsEtc->addSpacing(taiM->hschk_c);
  layFontsEtc->addStretch(10);

  ////////////////////////////////////////////////////////////////////////////
  layDisplayValues = new QVBoxLayout();  layTopCtrls->addLayout(layDisplayValues); //gbDisplayValues);
  layDisplayValues->setSpacing(2);
  layDisplayValues->setMargin(0);

  layColorScaleCtrls = new QHBoxLayout();  layDisplayValues->addLayout(layColorScaleCtrls);

  chkSnapBord = new QCheckBox("Snap\nBord", widg);
  chkSnapBord->setToolTip(taiMisc::ToolTipPreProcess("Whether to display unit snapshot value snap as a border around units"));
  connect(chkSnapBord, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layColorScaleCtrls->addWidget(chkSnapBord);
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  lblSnapBordWdth = taiM->NewLabel("Bord\nWdth", widg, font_spec);
  lblSnapBordWdth->setToolTip(taiMisc::ToolTipPreProcess("Width of snap border lines"));
  layColorScaleCtrls->addWidget(lblSnapBordWdth);
  fldSnapBordWdth = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layColorScaleCtrls->addWidget(fldSnapBordWdth->GetRep());
  ((iLineEdit*)fldSnapBordWdth->GetRep())->setCharWidth(6);
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  lblUnitSpacing = taiM->NewLabel("Unit\nSpace", widg, font_spec);
  lblUnitSpacing->setToolTip(taiMisc::ToolTipPreProcess("Spacing between units, as a proportion of the total width of the unit box"));
  layColorScaleCtrls->addWidget(lblUnitSpacing);
  fldUnitSpacing = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layColorScaleCtrls->addWidget(fldUnitSpacing->GetRep());
  ((iLineEdit*)fldUnitSpacing->GetRep())->setCharWidth(6);
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  chkWtLines = new QCheckBox("wt\nLines", widg);
  chkWtLines->setToolTip(taiMisc::ToolTipPreProcess("Whether to display connection weight values as colored lines, with color and transparency varying as a function of magnitude"));
  connect(chkWtLines, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layColorScaleCtrls->addWidget(chkWtLines);
  layColorScaleCtrls->addSpacing(taiM->hschk_c);

  chkWtLineSwt = new QCheckBox("s.wt", widg);
  chkWtLineSwt->setToolTip(taiMisc::ToolTipPreProcess("Display the sending weights out of the unit instead of the receiving weights into it"));
  connect(chkWtLineSwt, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layColorScaleCtrls->addWidget(chkWtLineSwt);
  layColorScaleCtrls->addSpacing(taiM->hschk_c);

  lblWtLineWdth = taiM->NewLabel("Wdth", widg, font_spec);
  lblWtLineWdth->setToolTip(taiMisc::ToolTipPreProcess("Width of weight lines -- 0 = thinnest lines (-1 = no lines, redundant with turning wt_lines off)"));
  layColorScaleCtrls->addWidget(lblWtLineWdth);
  fldWtLineWdth = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layColorScaleCtrls->addWidget(fldWtLineWdth->GetRep());
  ((iLineEdit*)fldWtLineWdth->GetRep())->setCharWidth(6);
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  lblWtLineThr = taiM->NewLabel("Thr", widg, font_spec);
  lblWtLineThr->setToolTip(taiMisc::ToolTipPreProcess("Threshold for displaying weight lines: weight magnitudes below this value are not shown -- if a layer to project onto is selected (Wt Prjn) then if this value is < 0, intermediate units in the weight projection that are below the K un threshold will be zeroed."));
  layColorScaleCtrls->addWidget(lblWtLineThr);
  fldWtLineThr = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layColorScaleCtrls->addWidget(fldWtLineThr->GetRep());
  ((iLineEdit*)fldWtLineThr->GetRep())->setCharWidth(6);
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  int list_flags = taiWidget::flgNullOk | taiWidget::flgAutoApply;

  lblWtPrjnLay = taiM->NewLabel("Wt\nPrjn", widg, font_spec);
  lblWtPrjnLay->setToolTip(taiMisc::ToolTipPreProcess("Layer to project weight values onto, from currently selected unit in view -- values are visible on all units in the wt_prjn unit variable if this setting is non-null -- setting this value causes expensive weight projection computation for every update"));
  layColorScaleCtrls->addWidget(lblWtPrjnLay);
  gelWtPrjnLay = dl.Add(new taiWidgetGroupElChooser(&TA_Layer_Group, this, NULL, widg, list_flags));
  layColorScaleCtrls->addWidget(gelWtPrjnLay->GetRep());

  lblWtPrjnKUn = taiM->NewLabel("K un", widg, font_spec);
  lblWtPrjnKUn->setToolTip(taiMisc::ToolTipPreProcess("Number of top K strongest units to propagate weight projection values through to other layers -- smaller numbers produce more selective and often interpretable results, though they are somewhat less representative."));
  layColorScaleCtrls->addWidget(lblWtPrjnKUn);
  fldWtPrjnKUn = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layColorScaleCtrls->addWidget(fldWtPrjnKUn->GetRep());
  ((iLineEdit*)fldWtPrjnKUn->GetRep())->setCharWidth(6);
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  lblWtPrjnKGp = taiM->NewLabel("K gp", widg, font_spec);
  lblWtPrjnKGp->setToolTip(taiMisc::ToolTipPreProcess("Number of top K strongest unit groups (where groups are present) to propagate weight projection values through to other layers (-1 or 0 to turn off this feature) -- smaller numbers produce more selective and often interpretable results, though they are somewhat less representative."));
  layColorScaleCtrls->addWidget(lblWtPrjnKGp);
  fldWtPrjnKGp = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layColorScaleCtrls->addWidget(fldWtPrjnKGp->GetRep());
  ((iLineEdit*)fldWtPrjnKGp->GetRep())->setCharWidth(6);
  layColorScaleCtrls->addSpacing(taiM->hsep_c);

  layColorScaleCtrls->addStretch(10);

  ////////////////////////////////////////////////////////////////////////////
  layiColorBar = new QHBoxLayout();  layDisplayValues->addLayout(layiColorBar);

  chkAutoScale = new QCheckBox("Auto\nScale", widg);
  chkAutoScale->setToolTip(taiMisc::ToolTipPreProcess("Automatically scale min and max values of colorscale based on values of variable being displayed"));
  connect(chkAutoScale, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layiColorBar->addWidget(chkAutoScale);

  butScaleDefault = new QPushButton("Defaults", widg);
  butScaleDefault->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  butScaleDefault->setMaximumWidth(taiM->maxButtonWidth() / 2);
  layiColorBar->addWidget(butScaleDefault);
  connect(butScaleDefault, SIGNAL(pressed()), this, SLOT(butScaleDefault_pressed()) );

  cbar = new iHColorScaleBar(&(dv_->scale), iColorScaleBar::RANGE, true, true, widg);
  connect(cbar, SIGNAL(scaleValueChanged()), this, SLOT(scalebar_changed()) );
  cbar->setMinimumWidth(300);
  layiColorBar->addWidget(cbar); // stretchfact=1 so it stretches to fill the space

  butSetColor = new QPushButton("Colors", widg);
  butSetColor->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  butSetColor->setMaximumWidth(taiM->maxButtonWidth() / 2);
  layiColorBar->addWidget(butSetColor);
  connect(butSetColor, SIGNAL(pressed()), this, SLOT(butSetColor_pressed()) );
  layiColorBar->addStretch(0);  // can't be > 0 else colorbar dissappears!

  ////////////////////////////////////////////////////////////////////////////
  layHistory = new QHBoxLayout();  layDisplayValues->addLayout(layHistory);

  histTB = new QToolBar(widg);
  layHistory->addWidget(histTB);

  histTB->setMovable(false);
  histTB->setFloatable(false);

  chkHist = new QCheckBox("Hist: Save", histTB);
  chkHist->setToolTip(taiMisc::ToolTipPreProcess("Save display value history, which can then be replayed using VCR-style buttons in this toolbar -- value to the right is number of steps to save"));
  connect(chkHist, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  histTB->addWidget(chkHist);

  fldHistMax = dl.Add(new taiWidgetField(&TA_int, this, NULL, widg));
  fldHistMax->rep()->setCharWidth(4);
  histTB->addWidget(fldHistMax->GetRep());

  histTB->addSeparator();

  actBack_All = histTB->addAction("|<");
  actBack_All->setToolTip(taiMisc::ToolTipPreProcess("Back all the way to first saved history of display values"));
  connect(actBack_All, SIGNAL(triggered()), this, SLOT(hist_back_all()) );

  actBack_F = histTB->addAction("<<");
  actBack_F->setToolTip(taiMisc::ToolTipPreProcess("Back ff steps in history of display values (edit field on far right to change ff steps to take)"));
  connect(actBack_F, SIGNAL(triggered()), this, SLOT(hist_back_f()) );

  actBack = histTB->addAction("<");
  actBack->setToolTip(taiMisc::ToolTipPreProcess("Back one step in history of display values"));
  connect(actBack, SIGNAL(triggered()), this, SLOT(hist_back()) );

  actFwd = histTB->addAction(">" );
  actFwd->setToolTip(taiMisc::ToolTipPreProcess("Forward one step in history of display values"));
  connect(actFwd, SIGNAL(triggered()), this, SLOT(hist_fwd()) );

  actFwd_F = histTB->addAction(">>" );
  actFwd_F->setToolTip(taiMisc::ToolTipPreProcess("Forward ff steps in history of display values (edit field on far right to change ff steps to take)"));
  connect(actFwd_F, SIGNAL(triggered()), this, SLOT(hist_fwd_f()) );

  actFwd_All = histTB->addAction(">|" );
  actFwd_All->setToolTip(taiMisc::ToolTipPreProcess("Forward all the way to current display values (will now track current values as they come in)"));
  connect(actFwd_All, SIGNAL(triggered()), this, SLOT(hist_fwd_all()) );

  histTB->addSeparator();

  QLabel* lblbk = taiM->NewLabel("Hist, Pos:", histTB, font_spec);
  lblbk->setToolTip(taiMisc::ToolTipPreProcess("number of stored display states in the history buffer right now, and current position relative to the last update (pos numbers = further back in time)"));
  histTB->addWidget(lblbk);

  lblHist = taiM->NewLabel("100, 100", histTB, font_spec);
  lblHist->setToolTip(taiMisc::ToolTipPreProcess("number of steps back in history currently viewing"));
  histTB->addWidget(lblHist);

  histTB->addSeparator();

  QLabel* lblff = taiM->NewLabel("ff:", histTB, font_spec);
  lblff->setToolTip(taiMisc::ToolTipPreProcess("number of steps to take when going fast-forward or fast-back through history"));
  histTB->addWidget(lblff);

  fldHistFF = dl.Add(new taiWidgetField(&TA_int, this, NULL, widg));
  fldHistFF->rep()->setCharWidth(4);
  histTB->addWidget(fldHistFF->GetRep());

  histTB->addSeparator();

  actMovie = histTB->addAction("Movie");
  actMovie->setToolTip(taiMisc::ToolTipPreProcess("record individual frames of the netview display from current position through to the end of the history buffer, as movie frames -- use ffmpeg http://ffmpeg.org to compile the individual PNG frames into an mp4 movie -- e.g., ffmpeg -framerate 10 -i movie_img_%05d.png -vcodec libx264 -pix_fmt yuv420p -crf 25 movie.mp4"));
  connect(actMovie, SIGNAL(triggered()), this, SLOT(hist_movie()) );

  QLabel* lblsz = taiM->NewLabel("sz:", histTB, font_spec);
  lblsz->setToolTip(taiMisc::ToolTipPreProcess("Size of movie frames to save (width x height"));
  histTB->addWidget(lblsz);

  fldMovieW = dl.Add(new taiWidgetField(&TA_int, this, NULL, widg));
  fldMovieW->rep()->setCharWidth(4);
  histTB->addWidget(fldMovieW->GetRep());

  fldMovieH = dl.Add(new taiWidgetField(&TA_int, this, NULL, widg));
  fldMovieH->rep()->setCharWidth(4);
  histTB->addWidget(fldMovieH->GetRep());
  
  layHistory->addStretch(10);
//   histTB->addStretch();

//   iSplitter* splt = new iSplitter(Qt::Vertical);
//   setCentralWidget(splt);

//   splt-> addWidget(widg);

  setCentralWidget(widg);

  tw = new QTabWidget(this);
  ////////////////////////////////////////////////////////////////////////////
  lvDisplayValues = new QTreeWidget();
  tw->addTab(lvDisplayValues, "Unit Display Values");
  lvDisplayValues->setRootIsDecorated(true); // makes it look like a list
  QStringList unit_var_hdr;
  unit_var_hdr << "      Variable" << "        Description";
  lvDisplayValues->setHeaderLabels(unit_var_hdr);
  lvDisplayValues->setSortingEnabled(false);
  lvDisplayValues->setSelectionMode(QAbstractItemView::SingleSelection);
  //layDisplayValues->addWidget(lvDisplayValues, 1);
  
//  lvDisplayValues->setDragEnabled(true);
//  lvDisplayValues->setAcceptDrops(true);
//  lvDisplayValues->setDropIndicatorShown(true);
//  lvDisplayValues->setDragDropMode(QAbstractItemView::InternalMove);

  connect(lvDisplayValues, SIGNAL(itemSelectionChanged()), this,
          SLOT(lvDisplayValues_selectionChanged()) );
  connect(lvDisplayValues, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this,
          SLOT(lvDisplayValues_itemClicked(QTreeWidgetItem*, int)) );

  ////////////////////////////////////////////////////////////////////////////
  // Spec tree

  tvSpecs = new iTreeView(NULL, iTreeView::TV_AUTO_EXPAND);
  tw->addTab(tvSpecs, "Spec Explorer");
  tvSpecs->ctxt_name = "SPEC";
  tvSpecs->setColumnCount(2);
  tvSpecs->setSortingEnabled(false);// only 1 order possible
  tvSpecs->setHeaderText(0, "Spec");
//   tvSpecs->setColumnWidth(0, 160); // more width for spec column
  tvSpecs->setHeaderText(1, "Description");
  tvSpecs->setColFormat(1, iTreeView::CF_ELIDE_TO_FIRST_LINE); // in case of multi-line specs
  tvSpecs->setColKey(1, taBase::key_desc); //note: ProgVars and Els have nice disp_name desc's
  //enable dnd support
  tvSpecs->setDragEnabled(true);
  tvSpecs->setAcceptDrops(true);
  tvSpecs->setDropIndicatorShown(true);
  tvSpecs->setHighlightRows(true);

  if(dv_->net()) {
    taBase* specs_ = &(dv_->net()->specs);
    MemberDef* md = dv_->net()->GetTypeDef()->members.FindName("specs");
    if (specs_) {
      taiSigLink* dl = (taiSigLink*)specs_->GetSigLink();
      if (dl) {
        dl->CreateTreeDataNode(md, tvSpecs, NULL, "specs");
      }
    }
  }

  tvSpecs->resizeColumnToContents(0); // just make sure everythign fits

  tvSpecs->Connect_SelectableHostNotifySignal(this,
    SLOT(tvSpecs_Notify(ISelectableHost*, int)) );
//   connect(tvSpecs, SIGNAL(ItemSelected(iTreeViewItem*)),
//     this, SLOT(tvSpecs_ItemSelected(iTreeViewItem*)) );

  ////////////////////////////////////////////////////////////////////////////
  state_values = new iTreeListWidget(this);

  connect(state_values, SIGNAL(ListOrderChange(int, int)), this, SLOT(NetStateItemMoved(int, int)) );

  tw->addTab(state_values, "Net State Values");
  
  state_values->setRootIsDecorated(true); // makes it look like a list

  QStringList state_var_hrd;
  state_var_hrd << "          Variable" << "        Description";
  state_values->setHeaderLabels(state_var_hrd);
  state_values->setSortingEnabled(false);
  state_values->setSelectionMode(QAbstractItemView::SingleSelection);
  state_values->setDragEnabled(true);
  state_values->setDropIndicatorShown(true);
  state_values->setDragDropMode(QAbstractItemView::InternalMove);

  connect(state_values, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this,
          SLOT(NetStateValues_itemClicked(QTreeWidgetItem*, int)) );

  layTopCtrls->addWidget(tw);

  ////////////////////////////////////////////////////////////////////////////
  // Command Buttons
  widCmdButtons = new QWidget(widg);
  iFlowLayout* fl = new iFlowLayout(widCmdButtons);
//  layTopCtrls->addWidget(widCmdButtons);
  layOuter->addWidget(widCmdButtons);

  meth_but_mgr = new iMethodButtonMgr(widCmdButtons, fl, widCmdButtons);
  meth_but_mgr->Constr(getNetView()->net());

  MakeButtons(layOuter);
}

iViewPanelOfNetwork::~iViewPanelOfNetwork()
{
  if (NetView *nv = getNetView()) {
    nv->nvp = NULL;
  }
}

void iViewPanelOfNetwork::UpdatePanel_impl() {
  inherited::UpdatePanel_impl();
  meth_but_mgr->GetImage();
  
  ++updating;
  NetView* nv = getNetView(); // cache
  if (!nv) return;
  if(req_full_build) {
    req_full_build = false;
    nv->Reset();
    nv->BuildAll();
  }
  if(req_full_render) {
    req_full_render = false;
    nv->Render();
  }

  if(nv->net()) {
    bool ok;
    this->setStyleSheet("iViewPanel { background-color: #" +
                        nv->net()->GetEditColor(ok).toString() + "; }");
  }
  
  chkDisplay->setChecked(nv->display);
  chkLayMove->setChecked(nv->lay_mv);
  chkNetText->setChecked(nv->net_text);
  cmbLayLayout->GetEnumImage(nv->lay_layout);
  cmbConType->GetEnumImage(nv->con_type);
  cmbUnitText->GetEnumImage(nv->unit_text_disp);
  cmbDispMode->GetEnumImage(nv->unit_disp_mode);
  cmbPrjnDisp->GetEnumImage(nv->view_params.prjn_disp);
  fldPrjnWdth->GetImage((String)nv->view_params.prjn_width);

  chkSnapBord->setChecked(nv->snap_bord_disp);
  fldSnapBordWdth->GetImage((String)nv->snap_bord_width);
  fldUnitSpacing->GetImage((String)nv->view_params.unit_spacing);

  chkWtLines->setChecked(nv->wt_line_disp);
  chkWtLineSwt->setChecked(nv->wt_line_swt);
  fldWtLineWdth->GetImage((String)nv->wt_line_width);
  fldWtLineThr->GetImage((String)nv->wt_line_thr);
  fldWtPrjnKUn->GetImage((String)nv->wt_prjn_k_un);
  fldWtPrjnKGp->GetImage((String)nv->wt_prjn_k_gp);
  gelWtPrjnLay->GetImage(&(nv->net()->layers), nv->wt_prjn_lay.ptr());

  chkShowIconified->setChecked(nv->show_iconified);
  fldUnitTrans->GetImage((String)nv->view_params.unit_trans);
  fldUnitFont->GetImage((String)nv->font_sizes.unit);
  fldLayFont->GetImage((String)nv->font_sizes.layer);
  fldMinLayFont->GetImage((String)nv->font_sizes.layer_min);
  chkXYSquare->setChecked(nv->view_params.xy_square);
  chkLayGp->setChecked(nv->view_params.show_laygp);

  chkHist->setChecked(nv->hist_save);
  fldHistMax->GetImage((String)nv->hist_max);
  lblHist->setText(String(nv->ctr_hist_idx.length) + ", " + String(nv->ctr_hist_idx.length-nv->hist_idx) + "  ");
  fldHistFF->GetImage((String)nv->hist_ff);

  fldMovieW->GetImage((String)nv->movie_size.x);
  fldMovieH->GetImage((String)nv->movie_size.y);
  
  // update var selection
  int i = 0;
  QTreeWidgetItemIterator it(lvDisplayValues);
  QTreeWidgetItem* item = NULL;
  while (*it) {
    item = *it;
    bool is_selected = (nv->cur_unit_vals.FindEl(item->text(0)) >= 0);
    item->setSelected(is_selected);
    // if list is size 1 make sure that there is a scale_range entry for this one
    ++it;
    ++i;
  }
  // spec highlighting
  BaseSpec* cspc = m_cur_spec; // to see if it changes, if not, we force redisplay
  iTreeViewItem* tvi = dynamic_cast<iTreeViewItem*>(tvSpecs->currentItem());
  tvSpecs_ItemSelected(tvi); // manually invoke slot
  if (cspc == m_cur_spec)
    setHighlightSpec(m_cur_spec, true);

  ColorScaleFromData();
  --updating;
}

void iViewPanelOfNetwork::GetValue_impl() {
  inherited::GetValue_impl();
  NetView* nv = getNetView(); // cache
  if (!nv) return;
  req_full_render = true;       // everything requires a re-render
  req_full_build = false;

  nv->display = chkDisplay->isChecked();
  nv->lay_mv = chkLayMove->isChecked();
  nv->net_text = chkNetText->isChecked();

  int ll;
  cmbLayLayout->GetEnumValue(ll);
  nv->lay_layout = (NetView::LayerLayout)ll;

  int utd;
  cmbUnitText->GetEnumValue(utd);
  nv->unit_text_disp = (NetView::UnitTextDisplay)utd;

  // unit disp mode is only guy requiring full build!
  int udm;
  cmbDispMode->GetEnumValue(udm);
  req_full_build = req_full_build || (nv->unit_disp_mode != udm);
  nv->unit_disp_mode = (NetView::UnitDisplayMode)udm;

  int pd;
  cmbPrjnDisp->GetEnumValue(pd);
  if(nv->view_params.prjn_disp != (NetViewParams::PrjnDisp)pd) {
    req_full_build = true;
  }
  nv->view_params.prjn_disp = (NetViewParams::PrjnDisp)pd;

  nv->view_params.prjn_width = (float)fldPrjnWdth->GetValue();

  int ct;
  cmbConType->GetEnumValue(ct);
  nv->con_type = (NetView::ConType)ct;

  nv->view_params.unit_trans = (float)fldUnitTrans->GetValue();
  nv->font_sizes.unit = (float)fldUnitFont->GetValue();
  nv->font_sizes.layer = (float)fldLayFont->GetValue();
  nv->font_sizes.layer_min = (float)fldMinLayFont->GetValue();

  nv->snap_bord_disp = chkSnapBord->isChecked();
  nv->snap_bord_width = (float)fldSnapBordWdth->GetValue();
  nv->view_params.unit_spacing = (float)fldUnitSpacing->GetValue();

  nv->wt_line_disp = chkWtLines->isChecked();
  nv->wt_line_swt = chkWtLineSwt->isChecked();
  nv->wt_line_width = (float)fldWtLineWdth->GetValue();
  nv->wt_line_thr = (float)fldWtLineThr->GetValue();
  nv->wt_prjn_k_un = (float)fldWtPrjnKUn->GetValue();
  nv->wt_prjn_k_gp = (float)fldWtPrjnKGp->GetValue();
  nv->wt_prjn_lay = (Layer*)gelWtPrjnLay->GetValue();
  bool cur_si = chkShowIconified->isChecked();
  if(cur_si != nv->show_iconified) {
    nv->show_iconified = cur_si;
    req_full_build = true;
  }
  nv->view_params.xy_square = chkXYSquare->isChecked();
  nv->view_params.show_laygp = chkLayGp->isChecked();

  nv->hist_save = chkHist->isChecked();
  nv->hist_max = (int)fldHistMax->GetValue();
  nv->hist_ff = (int)fldHistFF->GetValue();
  nv->movie_size.x = (int)fldMovieW->GetValue();
  nv->movie_size.y = (int)fldMovieH->GetValue();

  nv->SetScaleData(chkAutoScale->isChecked(), cbar->min(), cbar->max(), false);
}

void iViewPanelOfNetwork::CopyFrom_impl() {
  if (NetView *nv = getNetView()) {
    nv->CallFun("CopyFromView");
  }
}

void iViewPanelOfNetwork::butScaleDefault_pressed() {
  if (updating) return;
  if (NetView *nv = getNetView()) {
    nv->SetScaleDefault();
    nv->UpdateDisplay(true);
  }
}

void iViewPanelOfNetwork::butSetColor_pressed() {
  if (updating) return;
  if (NetView *nv = getNetView()) {
    nv->CallFun("SetColorSpec");
  }
  repaint();
}

void iViewPanelOfNetwork::scalebar_changed() {
  Changed();                  // overall edit changed
  if (updating) return;
  if (NetView *nv = getNetView()) {
    ++updating;
    if(nv->scale.auto_scale) {
      nv->scale.auto_scale = false;
      chkAutoScale->setChecked(false); // raises signal on widget..
    }
    --updating;
  }
}

void iViewPanelOfNetwork::hist_back_all() {
  if (updating) return;
  if (NetView *nv = getNetView()) {
    nv->HistBackAll();
  }
}

void iViewPanelOfNetwork::hist_back_f() {
  if (updating) return;
  if (NetView *nv = getNetView()) {
    nv->HistBackF();
  }
}

void iViewPanelOfNetwork::hist_back() {
  if (updating) return;
  if (NetView *nv = getNetView()) {
    nv->HistBack1();
  }
}

void iViewPanelOfNetwork::hist_fwd() {
  if (updating) return;
  if (NetView *nv = getNetView()) {
    nv->HistFwd1();
  }
}

void iViewPanelOfNetwork::hist_fwd_f() {
  if (updating) return;
  if (NetView *nv = getNetView()) {
    nv->HistFwdF();
  }
}

void iViewPanelOfNetwork::hist_fwd_all() {
  if (updating) return;
  if (NetView *nv = getNetView()) {
    nv->HistFwdAll();
  }
}

void iViewPanelOfNetwork::hist_movie() {
  if (updating) return;
  if (NetView *nv = getNetView()) {
    nv->HistMovie(nv->movie_size.x, nv->movie_size.y);
  }
}

void iViewPanelOfNetwork::ColorScaleFromData() {
  if (NetView *nv = getNetView()) {
    ++updating;
    cbar->UpdateScaleValues();
    if (chkAutoScale->isChecked() != nv->scale.auto_scale) {
      //note: raises signal on widget! (grr...)
      chkAutoScale->setChecked(nv->scale.auto_scale);
    }
    --updating;
  }
}

void iViewPanelOfNetwork::GetUnitVars() {
  NetView *nv = getNetView();
  if (!nv) return;

  lvDisplayValues->clear();
  if (nv->membs.size == 0) return;

  nv->InitHotVars();
  
  MemberDef* md;
  for (int i=0; i < nv->membs.size; i++) {
    md = nv->membs[i];
    if (md->HasOption("NO_VIEW")) continue;
    QStringList itm;
    itm << md->name << md->desc;
    QTreeWidgetItem* titm = new QTreeWidgetItem(lvDisplayValues, itm);
    if(nv->hot_vars.FindEl(md->name) < 0)
      titm->setCheckState(0, Qt::Unchecked);
    else
      titm->setCheckState(0, Qt::Checked);
  }
  lvDisplayValues->resizeColumnToContents(0);
}

void iViewPanelOfNetwork::GetNetVars() {
  NetView *nv = getNetView();
  if (!nv) return;
  
  Network* net = nv->net();
  if (!net) return;
  TypeDef* td = net->GetTypeDef();
  if (!td) return;
  
  state_values->clear();

  MemberDef* md;
  for (int i=0; i < nv->state_items.size; i++) {
    NetViewStateItem* item = nv->state_items.SafeEl(i);
    if (item) {
      QTreeWidgetItem* titm = new QTreeWidgetItem(state_values);
      if (item->net_member) {
        md = td->members.FindName(item->name);
        if (md) {
          titm->setText(0, item->name);
          titm->setText(1, md->desc);
        }
      }
      else {
        titm->setText(0, item->name);
        titm->setText(1, "monitor variable");
      }
      if(item->display)
      titm->setCheckState(0, Qt::Checked);
    else
      titm->setCheckState(0, Qt::Unchecked);
    }
  }
  state_values->resizeColumnToContents(0);
}

void iViewPanelOfNetwork::InitPanel() {
  if (NetView *nv = getNetView()) {
    ++updating;
    // fill monitor values
    GetUnitVars();
    GetNetVars();
    --updating;
  }
}

void iViewPanelOfNetwork::lvDisplayValues_selectionChanged() {
  if (updating) return;

  NetView *nv = getNetView();
  if (!nv) return;

  // redo the list each time, to guard against stale values
  nv->cur_unit_vals.Reset();
  QList<QTreeWidgetItem*> items(lvDisplayValues->selectedItems());
  QTreeWidgetItem* item = NULL;
  for (int j = 0; j < items.size(); ++j) {
    item = items.at(j);
    nv->cur_unit_vals.Add(item->text(0));
  }

  if (MemberDef *md = (MemberDef*) nv->membs.FindName(nv->cur_unit_vals.SafeEl(0))) {
    nv->setUnitDispMd(md);
    nv->UpdateViewerModeForMd(md);
  }
  ColorScaleFromData();
  //nv->InitDisplay(false);
  // note: init will reset history etc and is now unnec for updating view guys..
  nv->UpdateDisplay(false);
}

void iViewPanelOfNetwork::lvDisplayValues_itemClicked(QTreeWidgetItem* item, int col) {
  if (updating) return;

  NetView *nv = getNetView();
  if (!nv) return;
  Qt::CheckState chk = item->checkState(col);
  String nm = item->text(0);
  if(chk == Qt::Checked) {
    nv->hot_vars.AddUnique(nm);
  }
  else {
    nv->hot_vars.RemoveEl(nm);
  }
  nv->UpdateDisplay(false);
}

void iViewPanelOfNetwork::NetStateValues_itemClicked(QTreeWidgetItem* changed_item, int col) {
  if (updating) return;
  NetView *nv = getNetView();
  if (!nv) return;
  
  nv->NetStateItemDisplayChange(changed_item->text(0), changed_item->checkState(col));
  nv->UpdateDisplay(false);
}

void iViewPanelOfNetwork::NetStateItemMoved(int from_index, int to_index) {
  if (updating) return;
  NetView *nv = getNetView();
  if (!nv) return;
  
  nv->NetStateItemMoved(from_index, to_index);
  nv->UpdateDisplay(false);
}

void iViewPanelOfNetwork::setHighlightSpec(BaseSpec* spec, bool force) {
  if ((spec == m_cur_spec) && !force) return;
  m_cur_spec = spec;
  if (NetView *nv = getNetView()) {
    nv->SetHighlightSpec(spec);
  }
}

void iViewPanelOfNetwork::tvSpecs_Notify(ISelectableHost* src, int op) {
  NetView *nv = getNetView();
  if (!nv) return;

  switch (op) {
    //case ISelectableHost::OP_GOT_FOCUS:
    //  return;

    case ISelectableHost::OP_SELECTION_CHANGED:
    {
      taBase* new_base = NULL;
      ISelectable* si = src->curItem();
      if (si && si->link()) {
        new_base = si->link()->taData(); // NULL if not a taBase, shouldn't happen
      }
      setHighlightSpec((BaseSpec*)new_base);
      //    nv->UpdateDisplay(true);

      break;
    }

    //case ISelectableHost::OP_DESTROYING:
    //  return;

    default:
      return;
  }
}

void iViewPanelOfNetwork::tvSpecs_ItemSelected(iTreeViewItem* item) {
  NetView *nv = getNetView();
  if (!nv) return;

  BaseSpec* spec = NULL;
  if (item) {
    taBase* ld_ = (taBase*)item->linkData();
    if (ld_->InheritsFrom(TA_BaseSpec))
      spec = (BaseSpec*)ld_;
  }
  setHighlightSpec(spec);
//   nv->UpdateDisplay(true);
}


void iViewPanelOfNetwork::viewWin_NotifySignal(ISelectableHost* src, int op) {
  if (NetView *nv = getNetView()) {
    nv->viewWin_NotifySignal(src, op);
  }
}

void iViewPanelOfNetwork::dynbuttonActivated(int but_no) {
  NetView *nv = getNetView();
  if (!nv) return;

  T3ExaminerViewer* vw = nv->GetViewer();
  if (!vw) return;

  iAction* dyb = vw->getDynButton(but_no);
  if (!dyb) return;

  nv->ClearCaption();
  
  String nm = dyb->text();
  nv->cur_unit_vals.Reset();
  nv->cur_unit_vals.Add(nm);
  if (MemberDef *md = (MemberDef*)nv->membs.FindName(nm)) {
    nv->setUnitDispMd(md);
    nv->UpdateViewerModeForMd(md);
    vw->setDynButtonChecked(but_no, true, true); // mutex
  }
  ColorScaleFromData();
  
//   nv->InitDisplay(false);
  nv->UpdateDisplay(true);     // update panel
}

void iViewPanelOfNetwork::unTrappedKeyPressEvent(QKeyEvent* e) {
  if (NetView *nv = getNetView()) {
    nv->unTrappedKeyPressEvent(e);
  }
}

