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

#ifndef taDataLink_h
#define taDataLink_h 1

// parent includes:

// member includes:
#include <IDataLinkClient_PtrList>

// declare all other types mentioned but not required to include:
class taBase;

/*
 * taDataLink is the interface between data objects, and the object viewing system.

 * One DataLink object is created for each data object that has active viewers.
 *
 * Any type of object can contain a taDataLink delegate -- it passes the reference to the
 * taDataLink within itself to the datalink when it creates it.
 * A DataLink will only remain alive while there are clients viewing it -- when the last
 * client removes itself, the datalink will destruct. When a DataLink object destructs,
 * it will remove itself from its data item.
 *
 * A DataLink object of the correct type for a data item is created by the taiViewType
 * object for the type. Note that the base type for all instances is taiDataLink.

 * There are two destruction scenarios, as outlined below:
 * 1) The data object is deleted (ex., user deletes from a menu item)
 *    - data will call taDataLink::DataDestroying() for each DataLink in its list
 *    - each datalink will IDataLinkClient::DataLinkDestroying for each IDataLinkClient in
 *      its clients list
 *    - if DataLink has a panel, it will:
 *       call the Browser's RemovePanel(); clear its Panel ref
 *    - (additional Viewer-dependent deleting, ex. in a tree view)
 *    - taBase will remove DataLink from the list, destroying it
 * 2) Dataclient object is deleted (ex. user closes its viewer)
 *    - CO will call IDataLink::RemoveDataClient()
 *    - if DataLink has no more COs, it will destroy itself per (1) above
 *
 * NOTE: this is an abstract type -- most of the rest of the system uses taiDataLink
 * (see ta_qtviewer.h).

*/

#define DL_FUNS(y)      TypeDef* GetTypeDef() const {return &TA_ ## y;} \

class TA_API taDataLink {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS #VIRT_BASE interface for taiDataLink and descendants
friend class taDataLinkItr;
public:
  void*                 data() const {return m_data;} // subclasses usually replace with strongly typed version
#ifndef NO_TA_BASE
  taBase*               taData() const
    {if (isBase()) return (taBase*)m_data; else return NULL;}
#endif
  virtual bool          isBase() const {return false;} // true if data is of type taBase (note: could still be null)
  inline int            dbuCnt() const {return m_dbu_cnt;} // batch update: -ve:data, 0:none, +ve:struct
  virtual bool          isEnabled() const {return true;} // status of item

  bool                  AddDataClient(IDataLinkClient* dlc); // true if added, and it had not previously been added (false is probably a bug)
  bool                  RemoveDataClient(IDataLinkClient* dlc); // returns true if removed; false is likely not a bug, just redundancy

  virtual TypeDef*      GetDataTypeDef() const {return NULL;} // TypeDef of the data
  virtual MemberDef*    GetDataMemberDef() const {return NULL;} // if a member in a class, then the MemberDef
  virtual String        GetName() const {return _nilString;}
  virtual String        GetDisplayName() const; // default return Member name if has MemberDef, else GetName
  void                  DataDestroying(); // called by host when destroying, but it is still responsible for deleting us
  virtual void          DataDataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  virtual bool          HasChildItems() {return false;} // used when node first created, to control whether we put a + expansion on it or not

  virtual int           NumListCols() const {return 1;} // number of columns in a list view for this item type
  static const KeyString key_name; // "name" note: also on taBase
  virtual const KeyString GetListColKey(int col) const {return key_name;} // key of default list view
  virtual String        GetColHeading(const KeyString& key) const {return KeyString("Item");}
    // header text for the indicated column
  virtual String        GetColText(const KeyString& key, int itm_idx = -1) const
    {return GetName();} // text for the indicated column
  virtual String        ChildGetColText(taDataLink* child, const KeyString& key,
    int itm_idx = -1) const  {return child->GetColText(key, itm_idx);}
    // default delegates to child; lists can override to control this

  virtual String&       ListClients(String& strm, int indent = 0) const;
  // list all the data clients for this object to string

  virtual TypeDef*      GetTypeDef() const;
  taDataLink(void* data_, taDataLink* &link_ref_);
  virtual ~taDataLink();
protected:
  void*                 m_data; // subclasses usually replace with strongly typed version
  taDataLink**          m_link_ref; // #IGNORE address of our reference in the data item
  IDataLinkClient_PtrList clients; // clients of this item (ex. either primary, or where it is aliased or linked)
  int                   m_dbu_cnt; // data batch update count; +ve is Structural, -ve is Parameteric only
private:
  void                  DoNotify(int dcr, void* op1_, void* op2_);
    // don't even DREAM of making this non-private!!!!
};

#endif // taDataLink_h
