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
#include <taTransform>
#include <iColor>

// declare all other types mentioned but not required to include:
class T3Node; // 
class T3DataViewRoot; //
class T3ExaminerViewer; // #IGNORE
class iT3Panel; // #IGNORE
class T3Panel;

#ifdef TA_QT3D

#ifndef __MAKETA__
#include <Qt3DCore>
typedef QPointer<T3Node> T3NodePtr;
#endif

#else

#include <SoPtr>
class T3NodePtr; //
class SoPath; // #IGNORE
SoPtr_Of(T3Node);

#endif

/*
  DataView objects that have 3D reps
  Usually inherit directly from T3DataView, or if it is a viewer-rooted
  object, T3DataViewV

  rendering goes in two steps:
  BuildAll
  Render

  subclasses may add additional steps or other ways of ordering these
*/

taTypeDef_Of(T3DataView);

class TA_API T3DataView: public taDataView, public virtual IObjectSelectable {
  // #NO_TOKENS #VIRT_BASE base class for 3d-based DataView objects
INHERITED(taDataView)
friend class T3PanelViewer;
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

  int                   flags; // #READ_ONLY #NO_SAVE any of T3DataViewFlags TODO: tbd
  taTransform*          m_transform;  // #READ_ONLY #OWN_POINTER transform, created only if not unity

  virtual const iColor  bgColor(bool& ok) const {ok = false; return iColor();}
  // #IGNORE high-level items can optionally supply a bg color, for when they are singles
  virtual bool          hasChildren() const {return false;}
  virtual bool          hasViewProperties() const {return false;}
  // true if this item has editable view properties, and should be shown

  virtual bool          expandable() const {return false;}
  virtual void          setExpandable(bool) {}
  DATAVIEW_PARENT(T3DataView) // always a T3DataView (except root guy)
  taTransform*          transform(bool auto_create = false);
  // transform, in Inventor coords
  virtual bool          fixTransformAxis();
  // make sure transform doesn't have a zero axis

  bool                  isMapped() const override;
  // only true if in gui mode and gui stuff exists
  virtual T3DataViewRoot* root();
  virtual T3Panel* GetFrame() const;
  // get the T3Panel that owns us
  virtual T3ExaminerViewer* GetViewer() const;
  // #IGNORE get the Viewer that contains us

#ifdef TA_QT3D
  T3Node*               node_so() const; // #IGNORE
  void                  AddRemoveChildNode(T3Node* node, bool adding);
  // #IGNORE can be used for manually using non-default T3Node items in a child; add in Pre_impl, remove in Clear_impl
  virtual void          DebugNodeTree();
  // #BUTTON dump a listing of all the nodes under this one's node_so
  static void           DebugNodeTree_impl(const QObject& nd, int indent);
  // #IGNORE dump a listing of all the nodes under this one's node_so
#else
  T3Node*               node_so() const {return m_node_so.ptr();} // #IGNORE
  static T3DataView*    GetViewFromPath(const SoPath* path); // #IGNORE search path backwards to find the innermost T3DataView
  void                  AddRemoveChildNode(SoNode* node, bool adding);
  // #IGNORE can be used for manually using non-default T3Node items in a child; add in Pre_impl, remove in Clear_impl
#endif

  void                  Close() override; // usually delegates to parent->CloseChild
  virtual void          CloseChild(taDataView* child) {}
  virtual void          BuildAll() {}
  // subclass-dependent operation -- reinvoked after major update -- builds any sub-dataview objects, but not the corresponding So guys (which is done in render)
  void         SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2) override;
  void         SigDestroying() override; // called by data when it is destroying -- usual action is to destroy ourself
  void         ChildClearing(taDataView* child) override; // NOTE: child is always a T3DataView
  void         ChildRendered(taDataView* child) override; //  NOTE: child is always a T3DataView

  virtual void          OnWindowBind(iT3Panel* vw);
  // #IGNORE called after the viewer creates/fills the main window (for dataviews embedded in main viewer only), or when DataView added to existing viewer
  virtual void          ReInit(); // perform a reinitialization, particularly of visual state -- overload _impl
  virtual void          UpdateChildNames(T3DataView*); // #IGNORE update child names of the indicated node

  virtual void          ViewProperties(); // #MENU #VIEWMENU show the view properties for this object

  void         CutLinks() override;
  TA_DATAVIEWFUNS(T3DataView, taDataView);

public: // ISelectable interface (only not in ISigLinkClient)
  ISelectableHost* host() const override; //
  MemberDef*   md() const override {return m_md;}
  ISelectable* par() const override;
  taiSigLink* viewLink() const override; // data of the link
  GuiContext   shType() const override {return GC_DUAL_DEF_DATA;}
  taiSigLink* clipParLink(GuiContext sh_typ = GC_DEFAULT) const override; // not par_link
protected:
  void         QueryEditActionsS_impl_(int& allowed, int& forbidden,
                                       GuiContext sh_typ) const override;

protected:
  MemberDef*            m_md; // memberdef of this item in its parent

  void                  setNode(T3Node* node); // #IGNORE make changes via this

#ifdef TA_QT3D
  virtual void          AddRemoveChildNode_impl(T3Node* node, bool adding); // generic base uses SoSeparator->addChild()/removeChild()-- replace to change
#else
  virtual void          AddRemoveChildNode_impl(SoNode* node, bool adding); // generic base uses SoSeparator->addChild()/removeChild()-- replace to change
#endif

  void         ChildRemoving(taDataView* child) override; // #IGNORE called from list; we also forward to taViewer; we also remove visually
  virtual void          Constr_Node_impl() {} // create the node_so rep -- called in RenderPre, null'ed in Clear

  virtual void          OnWindowBind_impl(iT3Panel* vw) {} // override for something this class
  void                  Clear_impl() override;
  virtual void          ReInit_impl(); // default just calls clear() on the so, if it exists
  void                  Render_pre() override; //
  void                  Render_impl() override;

  void                  SigRecvStructUpdateEnd_impl() override;
  // our own customized version, similar to generic base
  void                  SigRecvRebuildView_impl() override
  { SigRecvStructUpdateEnd_impl(); } // same as StructEnd

protected:
  T3DataView*           last_child_node; // #IGNORE last child node created, so we can pass to createnode
  void         SigRecvUpdateAfterEdit_impl() override; // called by data for an UAE
  void         UpdateAfterEdit_impl() override;

private:
#ifndef __MAKETA__
  T3NodePtr             m_node_so; // Inventor node DO NOT MOVE FROM PRIVATE!!! DON'T EVEN THINK ABOUT IT!!! YOU ARE STILL THINKING ABOUT IT... STOP!!!!!!!
#endif
  
  void  Copy_(const T3DataView& cp);
  void  Initialize();
  void  Destroy();
};

TA_SMART_PTRS(TA_API, T3DataView);

#define T3_DATAVIEWFUNS(b,i) \
  TA_DATAVIEWFUNS(b,i);


class TA_API T3DataView_PtrList: public taPtrList<T3DataView> {
  // for selection lists, aux lists, etc.
#ifndef __MAKETA__
typedef taPtrList<T3DataView> inherited;
#endif
public:

protected:
  void*        El_Ref_(void* it) override { taBase::Ref((taBase*)it); return it; } // when pushed
  void*        El_unRef_(void* it) override { taBase::UnRef((taBase*)it); return NULL; }
   //TODO: changed from unRef 11/28/05 -- this now has delete semantics, will require objects
   // that are owned elsewhere to have ++ref count before adding here
};


#endif // T3DataView_h
