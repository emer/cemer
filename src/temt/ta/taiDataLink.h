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

#ifndef taiDataLink_h
#define taiDataLink_h 1

// parent includes:
#include <taDataLink>

// member includes:
#include <taString>
#include <MemberDef>

// declare all other types mentioned but not required to include:
class taiActions; //
class iSearchDialog; //
class iDataPanel; //
class taiTreeDataNode; //
class iTreeView; //
class DataLink_QObj; //
class taiMimeSource; //
class QIcon; //
class QVariant; //


TypeDef_Of(taiDataLink);

class TA_API taiDataLink: public taDataLink {
  // interface for viewing system
INHERITED(taDataLink)
public:
  static String         AnonymousItemName(const String& type_name, int index); // [index]:Typename

  virtual int           checkConfigFlags() const { return 0;}
    // taBase::THIS_INVALID|CHILD_INVALID
  virtual taiDataLink*  ownLink() const {return NULL;} // owner link (NOT gui par)

  virtual void          FillContextMenu(taiActions* menu); // only override to prepend to menu
  virtual void          FillContextMenu_EditItems(taiActions* menu, int allowed) {}
  virtual bool          GetIcon(int bmf, int& flags_supported, QIcon& ic) {return false;}
  virtual taiDataLink*  GetListChild(int itm_idx) {return NULL;} // returns NULL when no more
  virtual taiDataLink*  GetListChild(void* el) {return NULL;} // get link when item is known (esp for change notifies)
  virtual String        GetPath() const {return _nilString;} // esp taBase path
  virtual String        GetPath_Long() const {return _nilString;} // esp taBase path
  virtual String        GetPathNames() const {return _nilString;} // esp taBase path
  virtual String        GetTypeDecoKey() const {return _nilString;}
  virtual String        GetStateDecoKey() const {return _nilString;}
  virtual const QVariant GetColData(const KeyString& key, int role) const
    {return QVariant();} // for getting things like status text, tooltip text, etc.
  virtual void          Search(iSearchDialog* dlg) {} // called to do a search
  virtual bool          ShowMember(MemberDef* md, TypeItem::ShowContext show_context) const
    {return false;} // asks this type if we should show the md member

  iDataPanel*           CreateDataPanel();
  taiTreeDataNode*      CreateTreeDataNode(MemberDef* md, taiTreeDataNode* parent,
    taiTreeDataNode* after, const String& node_name, int dn_flags = 0);
    // create the proper tree node, with a tree node as a parent
  taiTreeDataNode*      CreateTreeDataNode(MemberDef* md, iTreeView* parent,
    taiTreeDataNode* after, const String& node_name, int dn_flags = 0);
    // create the proper tree node, with a iTreeView as a parent

  taiDataLink(void* data_, taDataLink* &link_ref_);
  DL_FUNS(taiDataLink) //

protected:
  DataLink_QObj*        qobj; // #IGNORE delegate object, when we need to connect or signal

  virtual void          Assert_QObj(); // makes sure the qobj is created
  virtual void          FillContextMenu_impl(taiActions* menu) {} // this is usually the one to override
  virtual iDataPanel*   CreateDataPanel_impl(); // default uses taiView
  taiTreeDataNode*      CreateTreeDataNode(MemberDef* md,
    taiTreeDataNode* nodePar, iTreeView* tvPar, taiTreeDataNode* after,
    const String& node_name, int dn_flags); // combined version, only 1 xxPar is set
  virtual taiTreeDataNode* CreateTreeDataNode_impl(MemberDef* md,
    taiTreeDataNode* nodePar, iTreeView* tvPar, taiTreeDataNode* after,
    const String& node_name, int dn_flags) = 0; // NOTE: only 1 of the parents is non-null -- use that version of the taiTreeNode constructor

  virtual ~taiDataLink(); // we only ever implicitly destroy, when 0 clients

public: // DO NOT CALL
  virtual void          QueryEditActions_impl(taiMimeSource* ms, int& allowed, int& forbidden) {}
    // get edit items allowed for this one item
  virtual int           EditAction_impl(taiMimeSource* ms, int ea) {return 0;}
  virtual void          ChildQueryEditActions_impl(const MemberDef* par_md, taiDataLink* child,
    taiMimeSource* ms, int& allowed, int& forbidden) {}
  virtual int           ChildEditAction_impl(const MemberDef* par_md, taiDataLink* child,
    taiMimeSource* ms, int ea) {return 0;} //
};

#endif // taiDataLink_h
