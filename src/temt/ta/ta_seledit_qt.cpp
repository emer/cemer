// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#include "ta_seledit_qt.h"

#include "ta_qt.h"
#include "ta_qtdialog.h" // for Hilight button
#include "ta_TA_inst.h"

#include <QColor>
#include <QHeaderView>
#include <QLayout>
#include <QTableWidget>
#include <QTextEdit>

/*
class TA_API DataHostModel: public QAbstractTableModel,
  public IDataLinkClient
{
  // #NO_INSTANCE #NO_CSS class that implements the Qt Model interface for matrices; we extend it to support N-d, but only 2-d cell display; if the model has a single gui client, then set it, to avoid doing refreshes when it isn't visible
friend class taMatrix;
INHERITED(QAbstractTableModel)
  Q_OBJECT
public:
#ifndef __MAKETA__
  QPointer<QWidget>	gui_parent;
  int			matIndex(const QModelIndex& idx) const; // #IGNORE flat matrix data index
  override QMimeData* 	mimeData (const QModelIndexList& indexes) const;
  override QStringList	mimeTypes () const;
  taMisc::MatrixView 	matView() const;
#endif //note: bugs in maketa necessitated these sections
  taMatrix*		mat() const {return m_mat;}
  
  DataHostModel(taMatrix* mat_, QWidget* gui_parent = NULL); // note: mat is always valid, we destroy this on mat dest
  ~DataHostModel(); //
  
public: // required implementations
#ifndef __MAKETA__
  int 			columnCount(const QModelIndex& parent = QModelIndex()) const; // override
  QVariant 		data(const QModelIndex& index, int role = Qt::DisplayRole) const; // override
  Qt::ItemFlags 	flags(const QModelIndex& index) const; // override, for editing
//  QVariant 		headerData(int section, Qt::Orientation orientation, 
//    int role = Qt::DisplayRole) const; // override
  int 			rowCount(const QModelIndex& parent = QModelIndex()) const; // override
  bool 			setData(const QModelIndex& index, const QVariant& value, 
    int role = Qt::EditRole); // override, for editing

public: // IDataLinkClient i/f
  override void*	This() {return this;}
  override TypeDef*	GetTypeDef() const {return &TA_MatrixTableModel;}
  override bool		ignoreDataChanged() const;
  override void		DataLinkDestroying(taDataLink* dl);
  override void		DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2); 
  
protected:
  static MatrixGeom	tgeom; // #IGNORE to avoid cost of allocation in index ops, we use this for non-reentrant
 
  taMatrix*		m_mat;
  taMisc::MatrixView	m_view_layout; //#IGNORE #DEF_TOP_ZERO
  bool			m_pat_4d;
  
  bool			ValidateIndex(const QModelIndex& index) const;
  bool			ValidateTranslateIndex(const QModelIndex& index, MatrixGeom& tr_index) const;
    // translates index into matrix coords; true if the index is valid
#endif
};

//////////////////////////////////
// 	DataHostModel		//
//////////////////////////////////

MatrixGeom DataHostModel::tgeom; 

DataHostModel::DataHostModel(taMatrix* mat_, QWidget* gui_parent_) 
:inherited(NULL)
{
  m_mat = mat_;
  if (m_mat) {
    m_mat->AddDataClient(this);
  }
  gui_parent = gui_parent_;
  m_pat_4d = false;
}

DataHostModel::~DataHostModel() {
  if (m_mat) {
    m_mat->RemoveDataClient(this);
    m_mat = NULL;
  }
}

int DataHostModel::columnCount(const QModelIndex& parent) const {
  return 2;
}

QVariant DataHostModel::data(const QModelIndex& index, int role) const {
  if (!m_mat) return QVariant();
  switch (role) {
  case Qt::DisplayRole: 
  case Qt::EditRole:
    return m_mat->SafeElAsStr_Flat(matIndex(index));
//Qt::DecorationRole
//Qt::ToolTipRole
//Qt::StatusTipRole
//Qt::WhatsThisRole
//Qt::SizeHintRole -- QSize
//Qt::FontRole--  QFont: font for the text
  case Qt::TextAlignmentRole:
    return m_mat->defAlignment();
  case Qt::BackgroundColorRole : //-- QColor
 // note: only used when !(option.showDecorationSelected && (option.state
//    & QStyle::State_Selected)) 
    if (!(flags(index) & Qt::ItemIsEditable))
      return QColor(COLOR_RO_BACKGROUND);
    break;
//Qt::TextColorRole
//  QColor: color of text
//Qt::CheckStateRole
  default: break;
  }
  return QVariant();
}

void DataHostModel::DataLinkDestroying(taDataLink* dl) {
  m_mat = NULL;
}

void DataHostModel::DataDataChanged(taDataLink* dl, int dcr,
  void* op1, void* op2)
{
  //this is primarily for code-driven changes
  if (dcr <= DCR_ITEM_UPDATED_ND) {
    emit_dataChanged();
  }
  else if ((dcr == DCR_STRUCT_UPDATE_END)) {
    emit_layoutChanged();
  }
}


void DataHostModel::emit_dataChanged(int row_fr, int col_fr, int row_to, int col_to) {
  if (!m_mat) return;
  // lookup actual end values when we are called with sentinels
  if (row_to < 0) row_to = rowCount() - 1;
  if (col_to < 0) col_to = columnCount() - 1;  
  
  emit dataChanged(createIndex(row_fr, col_fr), createIndex(row_to, col_to));
}

void DataHostModel::emit_layoutChanged() {
  emit layoutChanged();
}

Qt::ItemFlags DataHostModel::flags(const QModelIndex& index) const {
  if (!m_mat) return 0;
  //TODO: maybe need to qualify!, plus drag-drop handling, etc.
  Qt::ItemFlags rval = 0;
  
  if (ValidateIndex(index)) {
    rval = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  }
  // editability is a property of the whole mat
  if (!m_mat->isGuiReadOnly()) {
    rval |= Qt::ItemIsEditable;
  }
  return rval; 
}

bool DataHostModel::ignoreDataChanged() const {
  return (gui_parent && !gui_parent->isVisible());
}

int DataHostModel::matIndex(const QModelIndex& idx) const {
  //note: we dimensionally reduce all dims >1 to 1
  return m_mat->geom.IndexFmDims2D(idx.column(), idx.row(), pat4D(), matView());
}

QMimeData* DataHostModel::mimeData (const QModelIndexList& indexes) const {
  if (!m_mat) return NULL;
  CellRange cr(indexes);
  String str = mat()->FlatRangeToTSV(cr);
  QMimeData* rval = new QMimeData;
  rval->setText(str);
  return rval;
}

QStringList DataHostModel::mimeTypes () const {
  QStringList types;
  types << "text/plain";
  return types;
}


int DataHostModel::rowCount(const QModelIndex& parent) const {
  return m_mat->rowCount(pat4D());
  //note: for visual stuff, there is always at least one row
}

bool DataHostModel::setData(const QModelIndex& index, const QVariant & value, int role) {
  if (!m_mat) return false;
  if (index.isValid() && role == Qt::EditRole) {
    m_mat->SetFmStr_Flat(value.toString(), matIndex(index));
    emit dataChanged(index, index);
    return true;
  }
  return false;
}

bool DataHostModel::ValidateIndex(const QModelIndex& index) const {
  // TODO: maybe need to check bounds???
  return (m_mat);
}

bool DataHostModel::ValidateTranslateIndex(const QModelIndex& index, MatrixGeom& tr_index) const {
  bool rval = ValidateIndex(index);
  if (rval) {
    // TODO:
  }
  return rval;
}


*/
//////////////////////////////////
//  tabSelectEditViewType 	//
/////////////////////////////////

int tabSelectEditViewType::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_SelectEdit))
    return (inherited::BidForView(td) +1);
  return 0;
}

void tabSelectEditViewType::CreateDataPanel_impl(taiDataLink* dl_)
{
  // we create ours first, because it should be the default
  iSelectEditPanel* bldp = new iSelectEditPanel(dl_);
  DataPanelCreated(bldp);
  inherited::CreateDataPanel_impl(dl_);
}


//////////////////////////////////
//  iSelectEditDataHostBase	//
//////////////////////////////////

iSelectEditDataHostBase::iSelectEditDataHostBase(void* base, TypeDef* td,
  bool read_only_, QObject* parent)
:inherited(base, td, read_only_, false, parent)
{
  Initialize();
  sele = (SelectEdit*)base;
}

iSelectEditDataHostBase::~iSelectEditDataHostBase() {
}

void iSelectEditDataHostBase::Initialize()
{
  sele = NULL;
  sel_edit_mbrs = true; // note: we don't actually select members, just for removal
}

void iSelectEditDataHostBase::Constr_Body() {
  if (rebuild_body) {
    meth_el.Reset();
  }
  inherited::Constr_Body();
  // we deleted the normally not-deleted methods, so redo them here
  if (rebuild_body) {
    Constr_Methods();
    frmMethButtons->setHidden(!showMethButtons());
  }
}
  
void iSelectEditDataHostBase::Constr_Methods() {
  inherited::Constr_Methods();
  Insert_Methods();
  if (cur_menu != NULL) {// for safety... cur_menu should be the SelectEdit menu
    cur_menu->AddSep();
  }

  taGroupItr itr;
  EditMthItem_Group* grp;
  //int set_idx = 0;
  FOR_ITR_GP(EditMthItem_Group, grp, sele->mths., itr) {
    if (grp->size == 0) continue;
    //note: root group uses only buttons (hard wired)
    EditMthItem_Group::MthGroupType group_type = grp->group_type;
    
    // make a menu or button group if needed
    String men_nm = grp->GetDisplayName();
    if (men_nm.empty()) // shouldn't happen
      men_nm = "Actions";
    switch (group_type) {
    case EditMthItem_Group::GT_MENU: {
      cur_menu = ta_menus.FindName(men_nm);
      if (cur_menu == NULL) {
        cur_menu = menu->AddSubMenu(men_nm);
        ta_menus.Add(cur_menu);
      }
    } break;
    case EditMthItem_Group::GT_MENU_BUTTON: { 
      cur_menu_but = ta_menu_buttons.FindName(men_nm);
      if (cur_menu_but == NULL) {
        cur_menu_but = taiActions::New
          (taiMenu::buttonmenu, taiMenu::normal, taiMisc::fonSmall,
                  NULL, this, NULL, widget());
        cur_menu_but->setLabel(men_nm);
        DoAddMethButton((QPushButton*)cur_menu_but->GetRep()); // rep is the button for buttonmenu
        ta_menu_buttons.Add(cur_menu_but);
      }
    } break;
    default: break; // nothing for butts
    } // switch group_type
          
    for (int i = 0; i < grp->size; ++i) {
      EditMthItem* item = grp->FastEl(i);
      MethodDef* md = item->mth;
      taBase* base = item->base;
      if ((md->im == NULL) || (base == NULL)) continue;
      taiMethodData* mth_rep = md->im->GetMethodRep(base, this, NULL, frmMethButtons);
      if (mth_rep == NULL) continue;
      meth_el.Add(mth_rep);
  
      //NOTE: for seledit functions, we never place them on the last menu or button, because that may
      // make no sense -- the label specifies the place, or Actions if no label
      String mth_cap = item->caption();
      String statustip = item->desc;
      switch (group_type) {
      case EditMthItem_Group::GT_BUTTONS:  {
        AddMethButton(mth_rep, mth_cap);
      } break;
      case EditMthItem_Group::GT_MENU: {
//        mth_rep->AddToMenu(cur_menu);
        taiAction* act = cur_menu->AddItem(mth_cap, taiMenu::use_default,
              taiAction::action, mth_rep, SLOT(CallFun()) );
        if (statustip.nonempty())
          act->setStatusTip(statustip);
      } break;
      case EditMthItem_Group::GT_MENU_BUTTON: { 
//        mth_rep->AddToMenu(cur_menu_but);
        taiAction* act = cur_menu_but->AddItem(mth_cap, taiMenu::use_default,
              taiAction::action, mth_rep, SLOT(CallFun()) );
        if (statustip.nonempty())
          act->setStatusTip(statustip);
      } break;
      } // switch group_type
    }
  } // groups
}

taBase* iSelectEditDataHostBase::GetMembBase_Flat(int idx) {
  return sele->mbrs.GetBase_Flat(idx);
}

taBase* iSelectEditDataHostBase::GetMethBase_Flat(int idx) {
  return sele->mths.GetBase_Flat(idx);
}


void iSelectEditDataHostBase::mnuRemoveMember_select(int idx) {
  sele->RemoveField(idx);
}

void iSelectEditDataHostBase::mnuRemoveMethod_select(int idx) {
  sele->RemoveFun(idx);
}



//////////////////////////
//  iSelectEditDataHost	//
//////////////////////////

iSelectEditDataHost::iSelectEditDataHost(void* base, TypeDef* td,
  bool read_only_, QObject* parent)
:inherited(base, td, read_only_, parent)
{
  Initialize();
}

iSelectEditDataHost::~iSelectEditDataHost() {
}

void iSelectEditDataHost::Initialize()
{
}

void iSelectEditDataHost::ClearBody_impl() {
  // we also clear all the methods, and then rebuild them
  ta_menus.Reset();
  ta_menu_buttons.Reset();
//  meth_el.Reset(); // must defer deletion of these, because the MethodData objects are used in menu calls, so can't be
  layMethButtons = NULL;
  DeleteChildrenLater(frmMethButtons);
  show_meth_buttons = false;

  // note: no show menu in this class
  cur_menu = NULL;
  cur_menu_but = NULL;
  if (menu) {
    menu->Reset();
  }
  inherited::ClearBody_impl();
}

void iSelectEditDataHost::Constr_Data_Labels() {
  // delete all previous sele members
  membs.ResetItems();
  dat_cnt = 0;
  // mark place
  String nm;
  String help_text;
  MembSet* memb_set = NULL;
  
  bool def_grp = true; // first one
  taGroupItr itr;
  EditMbrItem_Group* grp;
  int set_idx = 0;
  FOR_ITR_GP(EditMbrItem_Group, grp, sele->mbrs., itr) {
    if (grp->size == 0) {def_grp = false; continue; }
    membs.SetMinSize(set_idx + 1);
    memb_set = membs.FastEl(set_idx);
    // make a group header
    if (!def_grp) {
      iLabel* lbl = new iLabel(grp->GetName(), body);
      AddSectionLabel(-1, lbl,
        "");
    }
    for (int i = 0; i < grp->size; ++i) {
      EditMbrItem* item = grp->FastEl(i);
      MemberDef* md = item->mbr;
      if (md->im == NULL) continue; // shouldn't happen
      taiData* mb_dat = md->im->GetDataRep(this, NULL, body);
      memb_set->memb_el.Add(md);
      memb_set->data_el.Add(mb_dat);
      QWidget* data = mb_dat->GetRep();
      //int row = AddData(-1, data);
  
      help_text = item->GetDesc();
      String new_lbl = item->caption();
      //obs GetName(md, nm, help_text); //note: we just call this to get the help text
      AddNameData(-1, new_lbl, help_text, data, mb_dat, md);
      ++dat_cnt;
    }
    def_grp = false;
    ++set_idx;
  }
}

void iSelectEditDataHost::DoRemoveSelEdit() {
   // removes the sel_item_index item 
  int sel_item_index = membs.GetFlatDataIndex(sel_item_dat);
  if (sel_item_index >= 0) {
    sele->RemoveField(sel_item_index);
  }
#ifdef DEBUG
  else
    taMisc::Error("iSelectEditDataHost::DoRemoveSelEdit: could not find item");
#endif
}

void iSelectEditDataHost::FillLabelContextMenu_SelEdit(QMenu* menu,
  int& last_id)
{
  int sel_item_index = membs.GetFlatDataIndex(sel_item_mbr, sel_item_base);
  if (sel_item_index < 0) return; 
  //QAction* act = 
  menu->addAction("Remove from SelectEdit", this, SLOT(DoRemoveSelEdit()));
}

void iSelectEditDataHost::GetImage_Membs_def() {
  int itm_idx = 0;
  for (int j = 0; j < membs.size; ++j) {
    MembSet* ms = membs.FastEl(j);
    for (int i = 0; i < ms->data_el.size; ++i) {
      taiData* mb_dat = ms->data_el.FastEl(i);
      MemberDef* md = ms->memb_el.SafeEl(i);
      EditMbrItem* item = sele->mbrs.Leaf(itm_idx);
      if ((item == NULL) || (item->base == NULL) || (md == NULL) || (mb_dat == NULL))
        taMisc::Error("iSelectEditDataHost::GetImage_impl(): unexpected md or mb_dat=NULL at i ", String(i), "\n");
      else {
        md->im->GetImage(mb_dat, item->base); // need to do this first, to affect visible
      }
      ++itm_idx;
    }
  }
}

void iSelectEditDataHost::GetValue_Membs_def() {
  int itm_idx = 0;
  for (int j = 0; j < membs.size; ++j) {
    MembSet* ms = membs.FastEl(j);
    for (int i = 0; i < ms->data_el.size; ++i) {
      taiData* mb_dat = ms->data_el.FastEl(i);
      MemberDef* md = ms->memb_el.SafeEl(i);
      EditMbrItem* item = sele->mbrs.Leaf(itm_idx);
      if ((item == NULL) || (item->base == NULL) || (md == NULL) || (mb_dat == NULL))
        taMisc::Error("iSelectEditDataHost::GetImage_impl(): unexpected md or mb_dat=NULL at i ", String(i), "\n");
      else {
        bool first_diff = true;
        md->im->GetMbrValue(mb_dat, item->base, first_diff); 
        if (!first_diff)
          taiMember::EndScript(item->base);
	item->base->UpdateAfterEdit(); // call UAE on item bases because won't happen elsewise!
      }
      ++itm_idx;
    }
  }
}


//////////////////////////
//   taiDataDelegate	//
//////////////////////////


taiDataDelegate::taiDataDelegate(taiEditDataHost* edh_) 
{
  edh = edh_;
}

QWidget* taiDataDelegate::createEditor(QWidget* parent, 
    const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  MemberDef* md = NULL;
  taBase* base = NULL;
  if (IndexToMembBase(index, md, base)) {
    if (md->im == NULL) goto exit; // shouldn't happen
    // we create a wrap widget for all of these guys, mostly so that smaller
    // guys like Combo don't try to be stretched the whole way
//    QWidget* np = new QWidget(parent);
//    QHBoxLayout* hbl = new QHBoxLayout(np);
//    hbl->setMargin(0);
//    hbl->setSpacing(0);
    
    dat = md->im->GetDataRep(edh, NULL, parent);
    dat->SetBase(base);
    dat->SetMemberDef(md);
    QWidget* rep = dat->GetRep();
//    hbl->addWidget(rep);
//    hbl->addStretch();
    connect(rep, SIGNAL(destroyed(QObject*)),
      this, SLOT(rep_destroyed(QObject*)) );
    return rep;
  }
exit:
  return inherited::createEditor(parent, option, index);
}

bool taiDataDelegate::eventFilter(QObject *object, QEvent *event)
{
  QWidget *editor = qobject_cast<QWidget*>(object);
  if (!editor)
      return false;
  if (event->type() == QEvent::KeyPress) {
    switch (static_cast<QKeyEvent *>(event)->key()) {
/*  case Qt::Key_Tab:
      emit commitData(editor);
      emit closeEditor(editor, QAbstractItemDelegate::EditNextItem);
      return true;
    case Qt::Key_Backtab:
      emit commitData(editor);
      emit closeEditor(editor, QAbstractItemDelegate::EditPreviousItem);
      return true;*/
    case Qt::Key_Enter:
    case Qt::Key_Return:
      if (qobject_cast<QTextEdit*>(editor))
          return false; // don't filter enter key events for QTextEdit
      // We want the editor to be able to process the key press
      // before committing the data (e.g. so it can do
      // validation/fixup of the input).
      if (QLineEdit* e = qobject_cast<QLineEdit*>(editor))
        if (!e->hasAcceptableInput())
          return false;
      QMetaObject::invokeMethod(this, "_q_commitDataAndCloseEditor",
        Qt::QueuedConnection, Q_ARG(QWidget*, editor));
      return false;
    case Qt::Key_Escape:
      // don't commit data
      emit closeEditor(editor, QAbstractItemDelegate::RevertModelCache);
      break;
    default:
      return false;
    }
    if (editor->parentWidget())
        editor->parentWidget()->setFocus();
    return true;
  } 
/*  else if (event->type() == QEvent::FocusOut) {
    if (!editor->isActiveWindow() || (QApplication::focusWidget() != editor)) {
      QWidget *w = QApplication::focusWidget();
      while (w) { // don't worry about focus changes internally in the editor
        if (w == editor)
            return false;
        w = w->parentWidget();
      }
      // The window may lose focus during an drag operation.
      // i.e when dragging involves the taskbar on Windows.
      if (QDragManager::self() && QDragManager::self()->object != 0)
        return false;
      // Opening a modal dialog will start a new eventloop
      // that will process the deleteLater event.
      if (QApplication::activeModalWidget()
        && !QApplication::activeModalWidget()->isAncestorOf(editor)
        && qobject_cast<QDialog*>(QApplication::activeModalWidget()))
        return false;
      emit commitData(editor);
      emit closeEditor(editor, NoHint);
    }
  } */
  else if (event->type() == QEvent::ShortcutOverride) {
    if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Escape) {
      event->accept();
      return true;
    }
  }
  return false;
}

void taiDataDelegate::GetImage() const {
  if (!dat) return;
  edh->Updating(true);
  dat->mbr->im->GetImage(dat, dat->Base());
  edh->Updating(false);
}

void taiDataDelegate::GetValue() const {
  if (!dat) return;
  taBase* base = dat->Base(); // cache
  bool first_diff = true;
  dat->mbr->im->GetMbrValue(dat, base, first_diff); 
  if (!first_diff)
    taiMember::EndScript(base);
  base->UpdateAfterEdit(); // call UAE on item bases because won't happen elsewise!
  // this is paradoxically an AutoApply
//  edh->Revert();
}

void taiDataDelegate::rep_destroyed(QObject* rep) {
#ifdef DEBUG
    cerr << "rep_destroyed for (dat/rep): " <<
      dat->metaObject()->className() << "/" <<
      rep->metaObject()->className() << "\n";
#endif
  if (dat && (dat->GetRep() == rep)) {
    dat->Delete();
    dat = NULL;
  }
  // if user left the edit unchanged, then assume this was a cancel, and Revert
  if (edh->isModified())
    edh->Revert();
}

void taiDataDelegate::setEditorData(QWidget* editor,
    const QModelIndex& index) const
{
  if (!dat) return;
/*nn  // confirm that this editor goes with the dat!
  if (dat->GetRep() != editor) {
#ifdef DEBUG
    cerr << "Unexpected editor, not same as dat!\n";
#endif
    return;
  }*/
#ifdef DEBUG
    cerr << "setEditorData for (dat/rep): " <<
      dat->metaObject()->className() << "/" <<
      dat->GetRep()->metaObject()->className() << "\n";
#endif 
  GetImage();
}

void taiDataDelegate::setModelData(QWidget* editor,
  QAbstractItemModel* model, const QModelIndex& index ) const
{
  if (!dat) return;
/*nn  // confirm that this editor goes with the dat!
  if (dat->GetRep() != editor) {
#ifdef DEBUG
    cerr << "Unexpected editor, not same as dat!\n";
#endif
    return;
  }*/
#ifdef DEBUG
    cerr << "setModelData for (dat/rep): " <<
      dat->metaObject()->className() << "/" <<
      dat->GetRep()->metaObject()->className() << "\n";
#endif 
  GetValue();
}


//////////////////////////
//  iSelectEditDataHost2	//
//////////////////////////

iSelectEditDataHost2::iSelectEditDataHost2(void* base, TypeDef* td,
  bool read_only_, QObject* parent)
:inherited(base, td, read_only_, parent)
{
  Initialize();
  sele = (SelectEdit*)base;
}

iSelectEditDataHost2::~iSelectEditDataHost2() {
  delete sed; // hope it's ref is deleted too!
  sed = NULL;
}

void iSelectEditDataHost2::Initialize()
{
  tw = NULL;
  sele = NULL;
  sel_edit_mbrs = true; // note: we don't actually select members, just for removal
  sed = new SelectEditDelegate(sele, this);
}

void iSelectEditDataHost2::Constr_Body_impl() {
  if (tw) return;
  tw = new QTableWidget(widget());
  tw->setColumnCount(2);
  tw->horizontalHeader()->setVisible(false);
  tw->horizontalHeader()->setStretchLastSection(true); // note: works if header invis
  tw->verticalHeader()->setVisible(false);
  tw->setSortingEnabled(false);
  // colors
  QPalette palette(tw->palette());
  palette.setColor(QPalette::Base, bg_color);
  palette.setColor(QPalette::AlternateBase, bg_color_dark);
  tw->setPalette(palette);
  tw->setAlternatingRowColors(true);
  tw->setContextMenuPolicy(Qt::CustomContextMenu);
  
  tw->setItemDelegateForColumn(1, sed);
  vblDialog->addWidget(tw, 1);
  connect(tw, SIGNAL(currentCellChanged(int, int, int, int)), 
    this, SLOT(tw_currentCellChanged(int, int, int, int)) );
  connect(tw, SIGNAL(customContextMenuRequested(const QPoint&)), 
    this, SLOT(tw_customContextMenuRequested(const QPoint&)) );
  body = tw;
}
 
void iSelectEditDataHost2::Constr_Box() {
  row_height = taiM->max_control_height(ctrl_size); // 3 if using line between; 2 if 
}
 

void iSelectEditDataHost2::ClearBody_impl() {
  // we also clear all the methods, and then rebuild them
  ta_menus.Reset();
  ta_menu_buttons.Reset();
//  meth_el.Reset(); // must defer deletion of these, because the MethodData objects are used in menu calls, so can't be
  layMethButtons = NULL;
  DeleteChildrenLater(frmMethButtons);
  show_meth_buttons = false;

  // note: no show menu in this class
  cur_menu = NULL;
  cur_menu_but = NULL;
  if (menu) {
    menu->Reset();
  }
//??  inherited::ClearBody_impl();
}

void iSelectEditDataHost2::Constr_Data_Labels() {
  // delete all previous sele members
  membs.ResetItems();
  tw->clear();
  // mark place
  String nm;
  String help_text;
  MembSet* ms = NULL;
  
  bool def_grp = true; // first one
  taGroupItr itr;
  EditMbrItem_Group* grp;
  int set_idx = 0;
  int row = 0;
  FOR_ITR_GP(EditMbrItem_Group, grp, sele->mbrs., itr) {
    if (grp->size == 0) {def_grp = false; continue; }
    membs.SetMinSize(set_idx + 1);
    ms = membs.FastEl(set_idx);
    // make a group header
    if (!def_grp) {
      tw->setRowCount(row+1);
      int item_flags = Qt::ItemIsEnabled; //Qt::ItemIsUserCheckable;
      ms->text = grp->GetName();
      ms->show = true;
      QTableWidgetItem* twi = new QTableWidgetItem;
      twi->setText(ms->text);
      twi->setFlags((Qt::ItemFlags)item_flags);
      //TODO: Bold
      QFont f(tw->font());
      f.setBold(true);
      twi->setFont(f);
      tw->setItem(row, 0, twi);
      tw->setSpan(row, 0, 1, 2);
      tw->setRowHeight(row, row_height);
      ++row;
    }
    tw->setRowCount(row + grp->size);
    for (int i = 0; i < grp->size; ++i) {
      int item_flags = 0;
      EditMbrItem* item = grp->FastEl(i);
      MemberDef* md = item->mbr;
      if (md->im == NULL) continue; // shouldn't happen
     // taiData* mb_dat = md->im->GetDataRep(this, NULL, body);
      ms->memb_el.Add(md);
     // ms->data_el.Add(mb_dat);
     
      // label item
      QTableWidgetItem* twi = new QTableWidgetItem;
      twi->setText(item->caption());
      twi->setStatusTip(item->GetDesc());
      twi->setToolTip(item->GetDesc());
      item_flags = Qt::ItemIsEnabled;
      twi->setFlags((Qt::ItemFlags)item_flags);
      tw->setItem(row, 0, twi);
      
      // data item
      twi = new QTableWidgetItem;
      item_flags = Qt::ItemIsEnabled; // |Qt::ItemIsSelectable;
      //TODO: check for READONLY
      bool ro = false;
      if (!ro)
        item_flags |= Qt::ItemIsEditable;
      twi->setFlags((Qt::ItemFlags)item_flags);
      // set EditMbrItem into 1.data
      twi->setData(Qt::UserRole, QVariant((ta_intptr_t)item));
      
      tw->setItem(row, 1, twi);
      tw->setRowHeight(row, row_height);
      ++row;
    }
    def_grp = false;
    ++set_idx;
  }
  tw->resizeColumnToContents(0); // note: don't do 1 here, do it in GetImage
}

void iSelectEditDataHost2::GetImage_Membs_def() {
  for (int row = 0; row < tw->rowCount(); ++row) {
    QTableWidgetItem* it = tw->item(row, 1);
    if (!it) continue;
    EditMbrItem* item = dynamic_cast<EditMbrItem*>(
      (taBase*)(it->data(Qt::UserRole).value<ta_intptr_t>()));
    if ((item == NULL) || (item->base == NULL)) continue;
    void* off = item->mbr->GetOff(item->base);
    String txt = item->mbr->type->GetValStr(off, item->base->GetOwner(),
      item->mbr, TypeDef::SC_DISPLAY); 
    it->setText(txt);
    it->setToolTip(txt); // for when over
  }
  sed->GetImage(); // if a ctrl is active
  tw->resizeColumnToContents(1);
}

void iSelectEditDataHost2::DoRemoveSelEdit() {
   // removes the sel_item_index item 
  int sel_item_index = -1;
  sele->mbrs.FindItemBase(sel_item_base, sel_item_mbr, sel_item_index);
  if (sel_item_index >= 0) {
    sele->RemoveField(sel_item_index);
  }
#ifdef DEBUG
  else
    taMisc::Error("iSelectEditDataHost::DoRemoveSelEdit: could not find item");
#endif
}

void iSelectEditDataHost2::FillLabelContextMenu_SelEdit(QMenu* menu,
  int& last_id)
{
  int sel_item_index = -1;
  sele->mbrs.FindItemBase(sel_item_base, sel_item_mbr, sel_item_index);
  if (sel_item_index < 0) return; 
  //QAction* act = 
  menu->addAction("Remove from SelectEdit", this, SLOT(DoRemoveSelEdit()));
}

void iSelectEditDataHost2::GetValue_Membs_def() {
/*TODO  int itm_idx = 0;
  for (int j = 0; j < membs.size; ++j) {
    MembSet* ms = membs.FastEl(j);
    for (int i = 0; i < ms->data_el.size; ++i) {
      taiData* mb_dat = ms->data_el.FastEl(i);
      MemberDef* md = ms->memb_el.SafeEl(i);
      EditMbrItem* item = sele->mbrs.Leaf(itm_idx);
      if ((item == NULL) || (item->base == NULL) || (md == NULL) || (mb_dat == NULL))
        taMisc::Error("iSelectEditDataHost2::GetImage_impl(): unexpected md or mb_dat=NULL at i ", String(i), "\n");
      else {
        bool first_diff = true;
        md->im->GetMbrValue(mb_dat, item->base, first_diff); 
        if (!first_diff)
          taiMember::EndScript(item->base);
	item->base->UpdateAfterEdit(); // call UAE on item bases because won't happen elsewise!
      }
      ++itm_idx;
    }
  }*/
  sed->GetValue();
}

void iSelectEditDataHost2::tw_currentCellChanged(int row, 
    int col, int previousRow, int previousColumn)
{
  if ((row < 0) || (col < 1)) return;
  QTableWidgetItem* twi = tw->item(row, col);
  if (!twi) return;
  tw->editItem(twi);
}

void iSelectEditDataHost2::tw_customContextMenuRequested(const QPoint& pos)
{
  int row = tw->rowAt(pos.y());
  int col = tw->columnAt(pos.x());
  if ((row < 0) || (col != 0)) return;
  // we want the data item for the label, to get its goodies...
  QTableWidgetItem* twi = tw->item(row, 1);
  if (!twi) return; // ex. right clicking on a section
  
  EditMbrItem* item = dynamic_cast<EditMbrItem*>(
    (taBase*)(twi->data(Qt::UserRole).value<ta_intptr_t>()));
  if ((item == NULL) || (item->base == NULL)) return;
    
  sel_item_mbr = item->mbr;
  sel_item_base = item->base;
  QMenu* menu = new QMenu(widget());
  int last_id = -1;
  FillLabelContextMenu(menu, last_id);
  if (menu->actions().count() > 0)
    menu->exec(tw->mapToGlobal(pos));
  delete menu;
  
}

//////////////////////////
//   SelectEditDelegate	//
//////////////////////////

QTableWidgetItem* /*SelectEditDelegate::*/ItemFromIndex(QTableWidget* tw,
  const QModelIndex& index)
{
  QTableWidgetItem* rval = tw->item(index.row(), index.column());
  return rval;
}

SelectEditDelegate::SelectEditDelegate(SelectEdit* sele_,
  iSelectEditDataHost2* sedh_)
:inherited(sedh_)
{
  sele = sele_;
  sedh = sedh_;
}

QWidget* SelectEditDelegate::createEditor(QWidget* parent, 
    const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QWidget* rval = inherited::createEditor(parent, option, index);
  QSize sz(rval->size());
  if (sz.width() > sedh->tw->columnWidth(1)) {
    sedh->tw->setColumnWidth(1,sz.width());
  }
  // also, delete the text so it doesn't show through
  QTableWidgetItem* twi = sedh->tw->item(index.row(), index.column());
  if (twi) {
    twi->setText(QString());
  }  
  return rval;
}

bool SelectEditDelegate::IndexToMembBase(const QModelIndex& index,
    MemberDef*& mbr, taBase*& base) const
{
  QTableWidgetItem* twi = sedh->tw->item(index.row(), index.column());
  if (twi) {
    EditMbrItem* item = dynamic_cast<EditMbrItem*>(
      (taBase*)(twi->data(Qt::UserRole).value<ta_intptr_t>()));
    if (item) {
      mbr = item->mbr;
      base = item->base;
      return true;
    }
  }
  return false;
}



//////////////////////////
//   iSelectEditPanel 	//
//////////////////////////

iSelectEditPanel::iSelectEditPanel(taiDataLink* dl_)
:inherited(dl_)
{
  SelectEdit* se_ = sele();
  se = NULL;
  if (se_) {
    switch (taMisc::edit_style) {
    case taMisc::ES_WIDGETS:
      se = new iSelectEditDataHost(se_, se_->GetTypeDef()); 
      break;
    case taMisc::ES_TABLE:
      se = new iSelectEditDataHost2(se_, se_->GetTypeDef()); 
      break;
    }
    if (taMisc::color_hints & taMisc::CH_EDITS) {
      bool ok;
      iColor bgcol = se_->GetEditColorInherit(ok);
      if (ok) se->setBgColor(bgcol);
    }
  }
}

iSelectEditPanel::~iSelectEditPanel() {
  if (se) {
    delete se;
    se = NULL;
  }
}

void iSelectEditPanel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  //NOTE: don't need to do anything because DataModel will handle it
}

bool iSelectEditPanel::HasChanged() {
  return se->HasChanged();
}

bool iSelectEditPanel::ignoreDataChanged() const {
  return !isVisible();
}

void iSelectEditPanel::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  se->ConstrEditControl();
  setCentralWidget(se->widget()); //sets parent
  setButtonsWidget(se->widButtons);
}

void iSelectEditPanel::UpdatePanel_impl() {
  if (se) se->ReShow_Async();
}

void iSelectEditPanel::ResolveChanges_impl(CancelOp& cancel_op) {
 // per semantics elsewhere, we just blindly apply changes
  if (se && se->HasChanged()) {
    se->Apply();
  }
}


