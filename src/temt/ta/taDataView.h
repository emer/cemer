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

#ifndef taDataView_h
#define taDataView_h 1

// parent includes:
#include <taNBase>
#include <ISigLinkClient>

// smartptr, ref includes
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:

// declare all other types mentioned but not required to include:
class taiSigLink; // 
class taSigLink; // 

/* taDataView -- exemplar base class of a view of an object, of class taOBase or later

   A view is a high-level depiction of an object, ex. "Network", "Graph", etc.

  The IDataViews list object does not own the view object -- some outer controller is
  responsible for lifetime management of dataview objects.

  However, if a dataobject is destroying, it will destroy all its views

  Most DataView objs are strictly managed by a parent, and so user should not
  be able to cut/paste/del them -- this is overridden for top-level view objs
  by overidding canCutPasteDel()

*/

taTypeDef_Of(taDataView);

class TA_API taDataView: public taNBase, public virtual ISigLinkClient {
  // #NO_TOKENS ##CAT_Display base class for views of an object
INHERITED(taNBase)
friend class DataView_List;
public:
  enum DataViewAction { // #BITS enum used to (safely) manually invoke one or more _impl actions
    CONSTR_POST         = 0x001, // #BIT (only used by taViewer)
    CLEAR_IMPL          = 0x002, // #BIT (only used by T3DataView)
    RENDER_PRE          = 0x004, // #BIT
    RENDER_IMPL         = 0x008, // #BIT
    RENDER_POST         = 0x010, // #BIT
    CLOSE_WIN_IMPL      = 0x020,  // #BIT (only used by taViewer)
    RESET_IMPL          = 0x040, // #BIT
    UNBIND_IMPL         = 0x080, // #BIT disconnect everyone from a data source
    SHOWING_IMPL        = 0x100, // #BIT
    HIDING_IMPL         = 0x200 // #BIT
#ifndef __MAKETA__
    ,CLEAR_ACTS         = CLEAR_IMPL | CLOSE_WIN_IMPL // for Clear
    ,RENDER_ACTS        = CLEAR_IMPL | RENDER_PRE | RENDER_IMPL | RENDER_POST // for Render
    ,REFRESH_ACTS       = RENDER_IMPL // for Refresh
    ,RESET_ACTS         = CLEAR_IMPL | CLOSE_WIN_IMPL | RESET_IMPL // for Reset
    ,UNBIND_ACTS        = UNBIND_IMPL

    ,SHOWING_HIDING_MASK = SHOWING_IMPL | HIDING_IMPL
    ,CONSTR_MASK        = CONSTR_POST | RENDER_PRE | RENDER_IMPL | RENDER_POST |
      SHOWING_HIDING_MASK
      // mask for doing child delegations in forward order
    ,DESTR_MASK         = CLEAR_IMPL | CLOSE_WIN_IMPL | RESET_IMPL | UNBIND_IMPL
      // mask for doing child delegations in reverse order
#endif
  };

// Data members:
public:
  taBase*               m_data;         // #READ_ONLY #NO_SET_POINTER data -- referent of the item (not ref'ed)
  TypeDef*              data_base;      // #READ_ONLY #NO_SAVE Minimum type for data object

protected:
  int                   m_dbu_cnt; // data batch update count; +ve is Structural, -ve is Parameteric only
  int                   m_index; // for when in a list
  // NOTE: all Dataxxx_impl are supressed if dbuCnt or parDbuCnt <> 0 -- see ta_type.h for detailed rules
  mutable taDataView*   m_parent; // autoset on SetOwner, type checked as well
  mutable short         m_vis_cnt; // visible count -- when >0, is visible, so refresh
  mutable signed char   m_defer_refresh; // while hidden: +1, struct; -1, data

// Methods:
public:
  taBase*               data() const {return m_data;} // subclasses usually redefine a strongly typed version
  void                  SetData(taBase* ta); // #MENU set the data to which this points -- must be subclass of data_base
  int                   dbuCnt() {return m_dbu_cnt;} // batch update: -ve:data, 0:none, +ve:struct
  inline int            index() const {return m_index;} // convenience accessor
  virtual bool          isMapped() const {return true;} // for DataView classes, or anything w/ separate gui classes that get created distinct from view hierarchy
  virtual MemberDef*    md() const {return NULL;} // ISelectable property member stub
  virtual int           parDbuCnt(); // dbu of parent(s); note: only sign is accurate, not necessarily value (optimized)
  inline bool           hasParent() const {return (m_parent);} // encapsulated way to check for a par
  taDataView*           parent() const {return m_parent;} // typically lex override with strong type
  virtual TypeDef*      parentType() const {return &TA_taDataView;} // the controlling parent -- note that when in a list, this is the list owner, not the list; overrride for strong check in SetOwner
  virtual bool          isRootLevelView() const {return false;} // #IGNORE controls the default clip behavior, whereby root = allow child ops (cut, dup, etc.); not = do almost nothing
  virtual bool          isTopLevelView() const {return false;} // #IGNORE controls the default clip behavior, whereby top = do most stuff; not = do almost nothing

  virtual MemberDef*    GetDataMemberDef() {return NULL;} // returns md if known and/or knowable (ex. NULL for list members)
  virtual String        GetLabel() const; // returns a label suitable for tabview tabs, etc.
  virtual void          SigRecvUpdateAfterEdit(); // note: normally overrride the _impl
  virtual void          SigRecvUpdateAfterEdit_Child(taDataView* chld)
    {SigRecvUpdateAfterEdit_Child_impl(chld);}
    // optionally called by child in its DUAE routine; must be added manually
  virtual void          ChildAdding(taDataView* child); // #IGNORE called from list;
  virtual void          ChildRemoving(taDataView* child) {} // #IGNORE called from list;
  virtual void          ChildClearing(taDataView* child) {} // override to implement par's portion of clear
  virtual void          ChildRendered(taDataView* child) {} // override to implement par's portion of render
  virtual void          CloseChild(taDataView* child) {}
  virtual void          SetVisible(bool showing); // called recursively when a view ctrl shows or hides
  virtual void          Render() {DoActions(RENDER_ACTS);}
    // renders the visible contents (usually override the _impls) -- MUST BE DEFINED IN SUB
  virtual void          Clear(taDataView* par = NULL) {DoActions(CLEAR_ACTS);} // clears the view (but doesn't delete any components) (usually override _impl)
  virtual void          Reset() {DoActions(RESET_ACTS);}
    // clears, and deletes any components (usually override _impls)
  virtual void          Refresh(){DoActions(REFRESH_ACTS);} // for manual refreshes -- just the impl stuff, not structural stuff
  virtual void          Unbind() {DoActions(UNBIND_ACTS);}
    // clears, and deletes any components (usually override _impls)
  virtual void          DoActions(DataViewAction acts); // do the indicated action(s) if safe in this context (ex loading, whether gui exists, etc.)

  virtual void          ItemRemoving(taDataView* item) {} // items call this on the root item -- usually used by a viewer to insure item removed from things like sel lists
  virtual void          SigDestroying() {} // called when data is destroying (m_data will already be NULL)

  // special clip op queries, including Child_xx that gets forwarded from owned lists -- most view objs are managed by an owner, exception is very top-level objs; so defaults of following basically disallow things like Cut, Paste, and Delete (Copy and Paste Assign are allowed by default) -- all in ta_qtclipdata.cpp
  virtual void          DV_QueryEditActionsS(int& allowed, int& forbidden); // #IGNORE
  virtual void          DV_QueryEditActionsD(taiMimeSource* ms,
    int& allowed, int& forbidden); // #IGNORE
  virtual void          DV_ChildQueryEditActions(const MemberDef* md,
    const taBase* child, const taiMimeSource* ms,
    int& allowed, int& forbidden); // #IGNORE note: this includes children of owned lists--md=NULL

protected: // the following just call inherited then insert the DV_ version
  void         QueryEditActionsS_impl(int& allowed, int& forbidden) override;
    // called once per src item, by controller
  void         QueryEditActionsD_impl(taiMimeSource* ms,
    int& allowed, int& forbidden) override;
  void         ChildQueryEditActions_impl(const MemberDef* md,
    const taBase* child, const taiMimeSource* ms,
    int& allowed, int& forbidden) override;

public:
  int   GetIndex() const {return m_index;}
  void  SetIndex(int value) {m_index = value;}
  taBase*       SetOwner(taBase* own); // update the parent; nulls it if not of parentType
  void  CutLinks();
  void  UpdateAfterEdit();
  TA_BASEFUNS(taDataView)

public: // ISigLinkCLient
  void*        This() override {return (void*)this;}
  TypeDef*     GetDataTypeDef() const override {
    return (m_data) ? m_data->GetTypeDef() : &TA_taBase; } // TypeDef of the data
  bool         ignoreSigEmit() const override {return (m_vis_cnt <= 0);}
  bool         isDataView() const override {return true;}
  void         SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2) override;
  void         IgnoredSigEmit(taSigLink* dl, int sls, void* op1, void* op2) override;
  void         SigLinkDestroying(taSigLink* dl) override; // called by SigLink when destroying; it will remove

protected:
  void         UpdateAfterEdit_impl() override;
  virtual void          SigLinkRecv_impl(int sls, void* op1, void* op2) {}
   // called when the data item has changed, esp. ex lists and groups, *except* UAE -- we also forward the last end of a batch update
  virtual void          SigRecvUpdateAfterEdit_impl() {} // called by data for an UAE, i.e., after editing etc.
  virtual void          SigRecvUpdateAfterEdit_Child_impl(taDataView* chld) {}
  virtual void          SigRecvUpdateView_impl();
  // called for Update All Views, and at end of a DataUpdate batch
  virtual void          SigRecvRebuildView_impl() {} // called for Rebuild All Views, clients usually do beg/end both
  virtual void          SigRecvStructUpdateEnd_impl() {} // called ONLY at end of a struct update -- derived classes usually do some kind of rebuild or render
  virtual void          SigEmit_Child(taBase* child, int sls, void* op1, void* op2) {}
   // typically from an owned list
  virtual void          DoActionChildren_impl(DataViewAction acts) {} // only one action called at a time, if CONSTR do children in order, if DESTR do in reverse order; call child.DoActions(act)
  virtual void          SetVisible_impl(DataViewAction act);
    // called when a viewer hides/shows (act is one of SHOWING or HIDING)
  virtual void          Constr_post() {DoActionChildren_impl(CONSTR_POST);}
    // extend to implement post-constr
  virtual void          Clear_impl() {DoActionChildren_impl(CLEAR_IMPL);}
    // extend to implement clear
  virtual void          CloseWindow_impl() {DoActionChildren_impl(CLOSE_WIN_IMPL);}
    // extend to implement clear
  virtual void          Render_pre() {DoActionChildren_impl(RENDER_PRE);}
    // extend with pre-rendering code, if needed
  virtual void          Render_impl() {DoActionChildren_impl(RENDER_IMPL);}
    // extend with code that renders the window contents
  virtual void          Render_post() {DoActionChildren_impl(RENDER_POST);}
    // extend with post-rendering code, if needed
  virtual void          Reset_impl() {DoActionChildren_impl(RESET_IMPL);}
    // extend to implement reset
  virtual void          Unbind_impl() {DoActionChildren_impl(UNBIND_IMPL);}
    // extend to implement unbind

private:
  void  Copy_(const taDataView& cp);
  void  Initialize();
  void  Destroy() {CutLinks();}
};


TA_SMART_PTRS(TA_API, taDataView);

// for explicit lifetime management
#define TA_DATAVIEWFUNS(b,i) \
  TA_BASEFUNS(b)

// convenience, for declaring the safe strong-typed parent
#define DATAVIEW_PARENT(T) \
  T* parent() const {return (T*)m_parent;} \
  TypeDef* parentType() const {return &TA_ ## T;}

// for strongly-typed lists
#define TA_DATAVIEWLISTFUNS(B,I,T) \
  T* SafeEl(int i) const {return (T*)SafeEl_(i);} \
  T* FastEl(int i) const {return (T*)FastEl_(i);} \
  TA_BASEFUNS(B);


#endif // taDataView_h
