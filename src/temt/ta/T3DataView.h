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

#ifndef T3DataView_h
#define T3DataView_h 1

// parent includes:
#include <taDataView>
#include <IObjectSelectable>

// member includes:

// declare all other types mentioned but not required to include:
class T3DataView; //
class T3Node; // #IGNORE
class T3DataViewRoot; //
class T3ExaminerViewer; //
class iT3DataViewFrame; //
class T3NodePtr; //
class SoPath; // #IGNORE

/*
  DataView objects that have 3D reps
  Usually inherit directly from T3DataView, or if it is a viewer-rooted
  object, T3DataViewV

  rendering goes in two steps:
  BuildAll
  Render

  subclasses may add additional steps or other ways of ordering these
*/

class TA_API T3DataView_List: public DataView_List { // ##NO_TOKENS
INHERITED(DataView_List)
friend class T3DataView;
public:

 virtual T3DataView*  FindData(taBase* dat, int& idx);
 // find DataView guy with data() == dat (fills in its index in list), returns NULL & idx=-1 for not found

  TA_DATAVIEWLISTFUNS(T3DataView_List, DataView_List, T3DataView)
private:
  NOCOPY(T3DataView_List)
  void                  Initialize() {SetBaseType(&TA_T3DataView);}
  void                  Destroy() {}
};


class TA_API T3DataView: public taDataView, public virtual IObjectSelectable {
  // #NO_TOKENS #VIRT_BASE base class for 3d-based DataView objects
INHERITED(taDataView)
friend class T3DataViewer;
friend class T3DataView_List;
friend class T3DataViewPar;
public:
  enum DataNodeFlags { //TODO: following copied from browsing; will need to be modified
    DNF_IS_FOLDER       = 0x001, // true for list/group folder nodes (note: does *not* indicate whether item can contain other things or not)
    DNF_IS_ALIAS        = 0x002, // true after children have been created (after clicking on node)
    DNF_UPDATE_NAME     = 0x004, // typically for list items, update the visual name (tree, tab, etc.) after item edited
    DNF_CAN_BROWSE      = 0x008, // can be a new browser root
    DNF_CAN_DRAG        = 0x010, // 16 can allow drags
    DNF_NO_CAN_DROP     = 0x020, // 32 cannot accept drops
    DNF_IS_MEMBER       = 0x040, // 64 true for members (and root), not for list/group items -- helps node configure edit ops
    DNF_IS_LIST_NODE    = 0x080 // true for nodes in a list view (in panel, not on tree)
  };

  static T3DataView*    GetViewFromPath(const SoPath* path); // #IGNORE search path backwards to find the innermost T3DataView

  int                   flags; // #READ_ONLY #NO_SAVE any of T3DataViewFlags TODO: tbd
  taTransform*       m_transform;  // #READ_ONLY #OWN_POINTER transform, created only if not unity

  virtual const iColor  bgColor(bool& ok) const {ok = false; return iColor();}
    // high-level items can optionally supply a bg color, for when they are singles
  virtual bool          hasChildren() const {return false;}
  virtual bool          hasViewProperties() const {return false;}
  // true if this item has editable view properties, and should be shown

  virtual bool          expandable() const {return false;}
  virtual void          setExpandable(bool) {}
  DATAVIEW_PARENT(T3DataView) // always a T3DataView (except root guy)
  taTransform*       transform(bool auto_create = false);  // transform, in Inventor coords
  virtual bool          fixTransformAxis();
  // make sure transform doesn't have a zero axis

  override bool         isMapped() const; // only true if in gui mode and gui stuff exists
  T3Node*               node_so() const {return m_node_so.ptr();} //
  virtual T3DataViewRoot* root();
  virtual T3DataViewFrame* GetFrame() const;
  // get the T3DataViewFrame that owns us
  virtual T3ExaminerViewer* GetViewer() const;
  // #IGNORE get the Viewer that contains us

  void                  AddRemoveChildNode(SoNode* node, bool adding);
    // can be used for manually using non-default T3Node items in a child; add in Pre_impl, remove in Clear_impl
  override void         Close(); // usually delegates to parent->CloseChild
  virtual void          CloseChild(taDataView* child) {}
  virtual void          BuildAll() {}
  // subclass-dependent operation -- reinvoked after major update -- builds any sub-dataview objects, but not the corresponding So guys (which is done in render)
  override void         DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);
  override void         DataDestroying(); // called by data when it is destroying -- usual action is to destroy ourself
  override void         ChildClearing(taDataView* child); // NOTE: child is always a T3DataView
  override void         ChildRendered(taDataView* child); //  NOTE: child is always a T3DataView

  virtual void          OnWindowBind(iT3DataViewFrame* vw); // called after the viewer creates/fills the main window (for dataviews embedded in main viewer only), or when DataView added to existing viewer
  virtual void          ReInit(); // perform a reinitialization, particularly of visual state -- overload _impl
  virtual void          UpdateChildNames(T3DataView*); // #IGNORE update child names of the indicated node

  virtual void          ViewProperties(); // #MENU #VIEWMENU show the view properties for this object

  override void         CutLinks();
  TA_DATAVIEWFUNS(T3DataView, taDataView);

public: // ISelectable interface (only not in IDataLinkClient)
  override ISelectableHost* host() const; //
  override MemberDef*   md() const {return m_md;}
  override ISelectable* par() const;
  override taiDataLink* viewLink() const; // data of the link
  override GuiContext   shType() const {return GC_DUAL_DEF_DATA;}
  override taiDataLink* clipParLink(GuiContext sh_typ = GC_DEFAULT) const; // not par_link
protected:
//  override void               FillContextMenu_impl(taiActions* menu, GuiContext sh_typ);
  override void         QueryEditActionsS_impl_(int& allowed, int& forbidden,
    GuiContext sh_typ) const;

protected:
  MemberDef*            m_md; // memberdef of this item in its parent

  void                  setNode(T3Node* node); // make changes via this

  virtual void          AddRemoveChildNode_impl(SoNode* node, bool adding); // generic base uses SoSeparator->addChild()/removeChild()-- replace to change
  override void         ChildRemoving(taDataView* child); // #IGNORE called from list; we also forward to DataViewer; we also remove visually
  virtual void          Constr_Node_impl() {} // create the node_so rep -- called in RenderPre, null'ed in Clear

  virtual void          OnWindowBind_impl(iT3DataViewFrame* vw) {} // override for something this class
  override void         Clear_impl();
  virtual void          ReInit_impl(); // default just calls clear() on the so, if it exists
  override void         Render_pre(); //
  override void         Render_impl();

  override void         DataStructUpdateEnd_impl(); // our own customized version, similar to generic base
  override void         DataRebuildView_impl() {DataStructUpdateEnd_impl();} // same as StructEnd

protected:
  T3DataView*           last_child_node; // #IGNORE last child node created, so we can pass to createnode
  override void         DataUpdateAfterEdit_impl(); // called by data for an UAE
  override void         UpdateAfterEdit_impl();

private:
  T3NodePtr             m_node_so; // Inventor node DO NOT MOVE FROM PRIVATE!!! DON'T EVEN THINK ABOUT IT!!! YOU ARE STILL THINKING ABOUT IT... STOP!!!!!!!
  void  Copy_(const T3DataView& cp);
  void  Initialize();
  void  Destroy();
};
TA_SMART_PTRS(T3DataView);

#define T3_DATAVIEWFUNS(b,i) \
  TA_DATAVIEWFUNS(b,i);


class TA_API T3DataView_PtrList: public taPtrList<T3DataView> {
  // for selection lists, aux lists, etc.
#ifndef __MAKETA__
typedef taPtrList<T3DataView> inherited;
#endif
public:

protected:
  override void*        El_Ref_(void* it) { taBase::Ref((taBase*)it); return it; } // when pushed
  override void*        El_unRef_(void* it) { taBase::UnRef((taBase*)it); return NULL; }
   //TODO: changed from unRef 11/28/05 -- this now has delete semantics, will require objects
   // that are owned elsewhere to have ++ref count before adding here
};


#endif // T3DataView_h
