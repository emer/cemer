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

#ifndef taiSigLink_h
#define taiSigLink_h 1

// parent includes:
#include <taSigLink>

// member includes:
#include <taString>
#include <MemberDef>

// declare all other types mentioned but not required to include:
class taiWidgetActions; //
class iDialogSearch; //
class iPanelBase; //
class taiTreeNode; //
class iTreeView; //
class taSigLink_QObj; //
class taiMimeSource; //
class QIcon; //
class QVariant; //


taTypeDef_Of(taiSigLink);

class TA_API taiSigLink: public taSigLink {
  // interface for viewing system
INHERITED(taSigLink)
public:
  static String         AnonymousItemName(const String& type_name, int index); // [index]:Typename

  virtual int           checkConfigFlags() const { return 0;}
    // taBase::THIS_INVALID|CHILD_INVALID
  virtual taiSigLink*  ownLink() const {return NULL;} // owner link (NOT gui par)

  virtual void          FillContextMenu(taiWidgetActions* menu); // only override to prepend to menu
  virtual void          FillContextMenu_EditItems(taiWidgetActions* menu, int allowed) {}
  virtual bool          GetIcon(int bmf, int& flags_supported, QIcon& ic) {return false;}
  virtual taiSigLink*  GetListChild(int itm_idx) {return NULL;} // returns NULL when no more
  virtual taiSigLink*  GetListChild(void* el) {return NULL;} // get link when item is known (esp for change notifies)
  virtual String        GetPath() const {return _nilString;} // esp taBase path
  virtual String        GetPath_Long() const {return _nilString;} // esp taBase path
  virtual String        GetPathNames() const {return _nilString;} // esp taBase path
  virtual String        GetTypeDecoKey() const {return _nilString;}
  virtual String        GetStateDecoKey() const {return _nilString;}
  virtual const QVariant GetColData(const KeyString& key, int role) const
    {return QVariant();} // for getting things like status text, tooltip text, etc.
  virtual void          Search(iDialogSearch* dlg) {} // called to do a search
  virtual bool          ShowMember(MemberDef* md, TypeItem::ShowContext show_context) const
    {return false;} // asks this type if we should show the md member

  iPanelBase*           CreateDataPanel();
  taiTreeNode*      CreateTreeDataNode(MemberDef* md, taiTreeNode* parent,
    taiTreeNode* after, const String& node_name, int dn_flags = 0);
    // create the proper tree node, with a tree node as a parent
  taiTreeNode*      CreateTreeDataNode(MemberDef* md, iTreeView* parent,
    taiTreeNode* after, const String& node_name, int dn_flags = 0);
    // create the proper tree node, with a iTreeView as a parent

  taiSigLink(void* data_, taSigLink* &link_ref_);
  SL_FUNS(taiSigLink) //

protected:
  taSigLink_QObj*        qobj; // #IGNORE delegate object, when we need to connect or signal

  virtual void          Assert_QObj(); // makes sure the qobj is created
  virtual void          FillContextMenu_impl(taiWidgetActions* menu) {} // this is usually the one to override
  virtual iPanelBase*   CreateDataPanel_impl(); // default uses taiView
  taiTreeNode*      CreateTreeDataNode(MemberDef* md,
    taiTreeNode* nodePar, iTreeView* tvPar, taiTreeNode* after,
    const String& node_name, int dn_flags); // combined version, only 1 xxPar is set
  virtual taiTreeNode* CreateTreeDataNode_impl(MemberDef* md,
    taiTreeNode* nodePar, iTreeView* tvPar, taiTreeNode* after,
    const String& node_name, int dn_flags) = 0; // NOTE: only 1 of the parents is non-null -- use that version of the taiTreeNode constructor

  virtual ~taiSigLink(); // we only ever implicitly destroy, when 0 clients

public: // DO NOT CALL
  virtual void          QueryEditActions_impl(taiMimeSource* ms, int& allowed, int& forbidden) {}
    // get edit items allowed for this one item
  virtual int           EditAction_impl(taiMimeSource* ms, int ea) {return 0;}
  virtual void          ChildQueryEditActions_impl(const MemberDef* par_md, taiSigLink* child,
    taiMimeSource* ms, int& allowed, int& forbidden) {}
  virtual int           ChildEditAction_impl(const MemberDef* par_md, taiSigLink* child,
    taiMimeSource* ms, int ea) {return 0;} //
};

#endif // taiSigLink_h
