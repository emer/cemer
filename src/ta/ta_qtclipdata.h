// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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


// ta_qtclipdata.h -- clipboard and drag/drop data type definitions

#ifndef TA_QTCLIPDATA_H
#define TA_QTCLIPDATA_H

#include "ta_base.h" // for list template

#ifndef __MAKETA__
# include <QMimeData>
# include <QStringList>
#endif

/*
  The clipboard/Drag-Drop model of PDP involves single or lists of data items that can be
  transferred to/from the clipboard, or transfered via drag/drop (dragdrop is mostly the
  same as clipboard, so unless otherwise noted, dd is assumed in clipboard.) Both in-process
  (inside one instance) as well as external (across instances) operations are supported.
  The system accommodates generic clipboard data (text, image, etc.), but is primarily for
  dealing with "tacss" objects, which are objects understood by the TA/CSS base system. A
  restriction of the tacss system is that every data item involved needs to have a TypeDef
  object -- if externally supplied classes must be handled, then a small wrapper class must
  be created to do the transfer.

  The clipboard facility requires the following services by the qtclipdata system:
    * taking one or more input data items, and providing a single clipboard representation
    * taking a clipboard contents, and extracting the descriptions of data being supplied
    * obtaining the actual data (ex. text stream rep) of a data item being supplied
    * communicating to a source when a data item has been taken (ex. Cut/Paste)

  A single data item is encapsulated in a taiMimeItem object for both source (i.e. to
  clipboard) and destination (i.e. from clipboard) operations.
  The member properties are (s-sending, r-receiving):
    obj -- (s/r*) the object (* r-obj only possible in-process)
    type - (s/r*) the TypeDef of the object (* r-type only possible between commensural app,
       instances, ex. leabra++ <-> leabra++, but (sometimes) not leabra++ <-> bp++)
    rep -- (s/r) the data representation, ex. a dump text stream

    typename -- (d) the type of the object
    path -- (d) the full path of the object

  The source operations are:
    * new(data item) -- creates a new sending instance (there can be multiple variants of this)
    * setDesc -- sets the desc needed for the clipboard (ex. by typename and path)
    * setData -- sets the data needed for the clipboard (ex. by dumping object to a text stream)
  The destination operations are:
    * create(data rep)
    * query -- determine the type of object, etc.
    * getDesc -- gets the desc data needed to characterize the object (ex. typename and path)
    * getData -- get the streamed data of the object (note: usually only used for external objects)
    * notifyTaken -- notifies the source that Cut/Moved data was taken

  PDP supplies clipboard data as follows:
  Higher level objects may also supply a graphical image (ex. image of the network.)

  MIME TYPE "tacss/objectdesc" -- compact description of data (no content)

    The data is an extended path string as follows:
      <src edit action>;<process id>;<taiClipData addr>;<item count N>   //note: N always >= 1
      [\n<typename>;<pdp path of object>[;<optional extended data>]*]xN

    The <src edit action> enables us to differentiate between a Cut and a Copied object
    (these flags are NOT used for drag and drop, since the dest determines whether it is a DragMove
    or a DragCopy operation).
    The <taiClipData addr> enables us to efficiently access the source directly (in-process only)
    rather than go indirectly through the clipboard.
    
    Qt4: because Qt4 started enumerating all the data formats, the actual data and command
    formats are "hidden", i.e., they don't show up when enumerating, but will exist if
    you query directly for them -- if you get the "tacss/objectdesc[Xxx]" format, you can be 
    sure you can also access the hidden formats.


  MIME TYPE "tacss/objectdata{i}" [hidden] -- i (optional, def=0) is: 0 <= i < N

    The data is the rep (ext dump save text) of the object.


  MIME TYPE "text/plain"

    The same string as the objectdesc string (primarily for diagnostic purposes).
    NOTE: this may be altered

  MIME TYPE "tacss/remdatataken{i}" [hidden] -- i (optional, def=0) is: 0 <= i < N

    This is a pseudo-mime type, used to enable receiver to communicate deletion
    back to sender when Cut or Drag/Move data has been successfully pasted.


  MIME TYPE "tacss/locdatataken{i}" [hidden] -- i (optional, def=0) is: 0 <= i < N

    This is a pseudo-mime type, used to enable in-process receiver to communicate
    back to sender when Cut data has been successfully pasted (if the receiver
    didn't just move the object).


taiClipData -- sender

  taiClipData is the class used by senders to supply tacss data to the clipboard system.
  This class is derived from QMimeData, and so inherently supports Drap/Drop, and clipboard
  operations.

  This class uses taiClipSrc


To Extend taiMimeSource:

  (1) extend formats_impl() override, check for your own indexes, else delegate to baseclass

COMMAND FORMATS

  Some formats are used to implement commands, particularly from dst to src across processes.

  MIME TYPE "tacss/locdatataken"

  A local (in-process) destination has consumed the data.

  MIME TYPE "tacss/remdatataken"

  A remote (out of process) destination has consumed the data -- can be used in a Cut to signal deletion.


*/

/* taiMimeSource

   This class is used for receive or query-type operations. It wraps an existing QMimeData,
   and if that mime source is a tacss mime source, it decodes the object path information.
   But this new class delegates the QMimeData virtuals to the source it wrapped, so it
   behaves essentially identically to that source. Therefore, we pass nothing but taiMimeSource
   objects around, so that we only need one version of every function, yet can have ready access
   to the extra tacss information, that will constitute 95%+ of the actual use cases.


*/

// format indexes
#define IDX_MD_TEXT_PLAIN		0
#define IDX_MD_OBJECTDESC		1
#define IDX_MD_OBJECTDATA		2
#define IDX_MD_REMDATATAKEN		3
#define IDX_MD_LOCDATATAKEN		4
#define IDX_MD_MAX			4
#define IDX_MD_VISIBLE_MAX		1

// mime-type strings
extern QString text_plain;
extern QString tacss_objectdesc;
extern QString tacss_objectdata;
//extern const char* tacss_remdatataken; // "hidden" mime type used by remote Paste-after-Cut and similar, to force deletion of source
//extern const char* tacss_locdatataken; // "hidden" mime type used by local Paste-after-Cut and similar, to force clipboard cleanup

extern QString mime_types[IDX_MD_MAX + 1];

// forwards
class taiMimeItem;
class taiMimeItem_List;


class TA_API taiClipData: public QMimeData {
INHERITED(QMimeData)
  Q_OBJECT
public:
  enum EditAction { // extended definitions of clipboard operations for ta/pdp, divided into two field banks: OP and SRC
    EA_SRC_CUT		= 0x00001, // flag indicating the source was a Clip/Cut operation
    EA_SRC_COPY		= 0x00002, // flag indicating the source was a Clip/Copy operation
    EA_SRC_DRAG		= 0x00004, // flag indicating the source was a Drag operation

    EA_SRC_MASK		= 0x0000F,  // note: SRC_ flags could be clear if src op unknown (ex. external mime format)

    EA_CUT		= 0x00010,
    EA_COPY		= 0x00020,
    EA_PASTE		= 0x00040,
    EA_DELETE		= 0x00080,
    EA_UNLINK		= 0x00100,
    EA_LINK		= 0x00200,
    EA_SET_AS_SUBGROUP	= 0x00400, //NOTE: not needed, will be nuked
    EA_SET_AS_SUBITEM	= 0x00800,
    EA_CLIP_OP_MASK	= 0x0FFF0, // masks the clipboard op codes

    EA_DRAG		= 0x10000, // initiation of a drag -- note that src can't distinguish move/copy/link ops
    EA_DROP_COPY	= 0x20000,
    EA_DROP_LINK	= 0x40000,
    EA_DROP_MOVE	= 0x80000,
    EA_DRAG_OP_MASK	= 0xF0000, // masks the drag/drop op codes
#ifndef __MAKETA__
    EA_SRC_OPS		= (EA_CUT | EA_COPY | EA_DELETE | EA_UNLINK | EA_DRAG), // src ops -- param will be a mime rep of the src obj
    EA_DROP_OPS		= (EA_DROP_COPY | EA_DROP_LINK | EA_DROP_MOVE),
    EA_DST_OPS		= (EA_PASTE | EA_LINK | EA_SET_AS_SUBGROUP | EA_SET_AS_SUBITEM | EA_DROP_OPS), //
    EA_FORB_ON_SRC_CUT	= (EA_LINK | EA_SET_AS_SUBGROUP | EA_SET_AS_SUBITEM), // dst ops forbidden when the source operation was Cut
    EA_FORB_ON_MUL_SEL	= (EA_PASTE | EA_LINK | EA_SET_AS_SUBGROUP | EA_SET_AS_SUBITEM | EA_DROP_COPY | EA_DROP_LINK | EA_DROP_MOVE),
        // dst ops forbidden when multi source operands selected
    EA_IN_PROC_OPS	= (EA_LINK | EA_SET_AS_SUBGROUP | EA_SET_AS_SUBITEM | EA_DROP_LINK), // ops that require an in-process src
#endif
    EA_OP_MASK		= 0xFFFF0 // masks all operation codes

  };
  
  enum EditResult { // passed as result in the various XxxEditAction routines
    ER_ERROR		= -2, // indicates action was attempted but an error occurred
    ER_FORBIDDEN	= -1, // indicates action is not allowed (may not have been resolvable at EditActionsAllowed stage)
    ER_IGNORED 		=  0, // indicates no action was taken, may indicate need to call another handler
    ER_OK		=  1  // indicates successful action taken
  };

  virtual int		count() const = 0; // number of items
  virtual bool		is_multi() const = 0;
  virtual taiMimeItem*	items(int i) const = 0;
//  static bool decode ( const QMimeSource * e, QPixmap & pm )

  static EditAction	ClipOpToSrcCode(int ea); // converts an op like EA_CUT into a source field like EA_SRC_CUT

  int			src_edit_action;

  taiClipData(int src_edit_action_);

  QStringList 		formats() const; // overrride
protected:
  bool			DecodeFormat(const QString& mimeType, int& fmt_num, int& index) const; // returns true if valid,
  virtual QByteArray 	encodedData_impl(int fmt_num, int index = 0) = 0; // gets the data of the type -- can be replaced or extended -- note we cheat a bit by being non-const, but Qt requires retrieveData to be const
  virtual void 		formats_impl(QStringList& list) const; // overrride
#ifndef __MAKETA__
  QVariant 		retrieveData(const QString& mimeType, QVariant::Type type) const; // #IGNORE override - queries for proper index and calls the _impl function
#endif
private:
};


class TA_API taiSingleClipData: public taiClipData { // ClipData for a single object -- simplest, most common case
INHERITED(taiClipData)
  Q_OBJECT
public:
  int			count() const {return 1;} // override
  bool			is_multi() const {return false;} // override
  taiMimeItem*		items(int i) const {return (i == 0) ? item : NULL;}

  taiSingleClipData(taiMimeItem* item_, int src_edit_action_);
    // NOTE: we take over ownership of the item
  ~taiSingleClipData();

protected:
  taiMimeItem*	item;
  QByteArray 	encodedData_impl(int fmt_num, int index = 0); // override - gets the data of the type -- can be replaced or extended
};


class TA_API taiMultiClipData: public taiClipData { // ClipData for multi selection of objects
INHERITED(taiClipData)
  Q_OBJECT
public:
  int			count() const; // override
  bool			is_multi() const {return true;} // override
  taiMimeItem*		items(int i) const;

  taiMultiClipData(taiMimeItem_List* list_, int src_edit_action_);
    // NOTE: we take over ownership of the list
  ~taiMultiClipData();

protected:
  taiMimeItem_List*	list;
  QByteArray 	encodedData_impl(int fmt_num, int index = 0); // override - gets the data of the type -- can be replaced or extended
};



//////////////////////////////////
// 	taiMimeItem		//
//////////////////////////////////

class TA_API taiMimeItem: public QObject { // we inherit from QObject so the instance can be notified if source bails -- we create an interface and multiple subclasses to keep dependencies clear (also minimizes member data, but that is usually not important)
INHERITED(QObject)
  Q_OBJECT
friend class taiMimeSource;
friend class taiSingleClipData;
friend class taiMultiClipData;
public:
  static taiMimeItem*	New(taBase* obj); // used for creating descs of taBase objects
//  static taiMimeItem*	New(void* obj, TypeDef* td); // used for creating descs of non-taBase objects

  virtual void*		obj() const = 0; // object associated with desc -- only set if InProcess
  virtual TypeDef*	td() const = 0; // the TypeDef associated with type_name, or NULL if not in our type list or not tacss
  virtual String	type_name() const = 0; // empty if not a tacss mime type
  virtual bool		is_tab() const = 0; // true if the object is derived from taBase
  virtual String	path() const = 0; // if a taBase object, its full path; if not taBase, or not tacss, then NULL;

  taiMimeItem();
public slots:
  void			obj_destroyed() {obj_destroyed_impl();} // datalinks will connect to this so our obj doesn't become invalid

protected:
  virtual void		obj_destroyed_impl() {} // datalinks will connect to this so our obj doesn't become invalid
  virtual void 		LocDataTaken() {} // (s)
  virtual void 		RemDataTaken() {} // (s)
  virtual void		SetData(ostream& ost) {} // (s) get the data from the object

};


class TA_API taiMimeItem_List: public taPtrList<taiMimeItem> {
public:
  void			El_Done_(void*);	// override, when "done" (delete)
};


class TA_API tabSndMimeItem: public taiMimeItem { // specialized for taBase sending
INHERITED(taiMimeItem)
  Q_OBJECT
friend class taiMimeItem;
public:
  void*			obj() const {return mobj;}
  TypeDef*		td() const;
  String		type_name() const;
  bool			is_tab() const {return (mobj);} // only if obj actually exists
  String		path() const;

protected:
  taBase*		mobj;
  void			obj_destroyed_impl() {mobj = NULL;} // override
  void 			LocDataTaken(); // override
  void 			RemDataTaken(); // override
  void			SetData(ostream& ost); // override
  tabSndMimeItem(taBase* obj_);
};


class TA_API taiRcvMimeItem: public taiMimeItem { // specialized for tacss receiving
INHERITED(taiMimeItem)
  Q_OBJECT
friend class taiExtMimeSource;
public:
  void*			obj() const; // override -- NOTE: only called by taiExtMimeSource if we are InProcess
  TypeDef*		td() const {return mtd;}
  String		type_name() const {return mtype_name;}; // override
  bool			is_tab() const {return mis_tab;} // override
  String		path() const {return mpath;} // override

protected:
  taBase*		mobj; // cache
  TypeDef*		mtd;
  String		mtype_name;
  bool			mis_tab;
  String		mpath;
  taiRcvMimeItem(const String type_name_, const String path_); // sets td and is_tab
};


/*
  taiMimeSource works like an iterator:
    count: the number of items (0 if not tacss items), 1 if 'is_multi' is false
    index: the current index; initialized to 0, except if count=0, then -1; -1=no value -- if set out of range, is saved in range
    xxxx ITER: property of the current index; if index out of range, then values are 0 (ex. "", 0, false)

*/
class TA_API taiMimeSource: public QMimeData { // a delegate/wrapper that is used for dealing with generic Mime data, as well as decoding the tacss mime types -- acts like an iterator (for all properties marked ITER)
INHERITED(QMimeData)
  Q_OBJECT
public:
  static taiMimeSource*	New(const QMimeData* ms); // we use a static method for extensibility -- creates correct subtype
  static taiMimeSource*	New2(taiClipData* cd); // we use a static method for extensibility -- creates correct subtype

  virtual int		src_action() const = 0; // any (or none) of the EA_SRC_xxx flags
  virtual bool		is_multi() const = 0; // true if the source is multiple individual objects (multi-select)
  virtual bool		is_tacss() const = 0; // true if the mime source is a taiClipData source, otherwise false (generic QMimeData)

  virtual int		count() const = 0; // number of items
  int			index() const; // current index value; -1 if none
  void			setIndex(int val) {iter_idx = ((val >= 0) && (val < count())) ? val : -1;}
    // sets index; must be -1 or in range, else sets to -1
  String		type_name() const {return in_range() ? item()->type_name() : _nilString;}
    // ITER empty if not a tacss mime type
  TypeDef*		td() const {return in_range() ? item()->td() : NULL;}
    // ITER the TypeDef associated with type_name, or NULL if not in our type list or not tacss
  void*			object() const;
    // gets the object, if possible -- only valid for IsThisProcess true
  taBase*		tab_object() const; // gets a taBase object, if possible, otherwise NULL
    // gets the object, if possible -- only valid for IsThisProcess true
  int			object_data(istringstream& result) const;
    // #IGNORE gets the object data for the current item; returns number of bytes
  String		path() const {return in_range() ? item()->path() : _nilString;};
    // ITER if a taBase object, its full path; if not taBase, or not tacss, then NULL;
  bool			is_tab() const {return in_range() ? item()->is_tab() : false;};
    // ITER true if the object is derived from taBase
  QStringList 		formats() const; // override
  bool			hasFormat(const QString& mimeType) const; // override
  int			data(const QString& mimeType, taString& result) const; // provides data to a String; returns # bytes
  int			data(const QString& mimeType, istringstream& result) const; // #IGNORE provides data to an istrstream; returns # bytes
  void			loc_data_taken() const; // sends a loc_data_taken for the current index; only call if consumer didn't move/consume the item
  void			rem_data_taken() const; // sends a rem_data_taken for the current index, called by consumer when CUT-like item is accepted

#ifndef __MAKETA__
  using QMimeData::data; // lets us access the inherited version
#endif

  virtual bool		IsThisProcess() const = 0; // true if object originates in this process (ie, we can do low-level object-based ops)

  ~taiMimeSource();
protected:
  const QMimeData* 	ms;
  int			iter_idx; // iteration index: =-1, not started yet; >=0 < items.size, in range; =size, past end
  bool			in_range() const {return ((iter_idx >= 0) && (iter_idx < count()));}// true if index in range
  virtual taiMimeItem*	item() const = 0; // current item -- must always be checked with in_range before access

  void			AssertList(); // makes sure list is constructed
#ifndef __MAKETA__
  QVariant 		retrieveData(const QString& mimetype, QVariant::Type type) const; //overrride
#endif
  taiMimeSource(const QMimeData* ms); // creates an instance from a non-null ms; if ms is tacss, fields are decoded
};

class TA_API taiIntMimeSource: public taiMimeSource { // a taiMimeSource that wraps our own in-process taiClipData
INHERITED(taiMimeSource)
  Q_OBJECT
friend class taiMimeSource;
public:
  int			src_action() const {return cd->src_edit_action;} // override
  bool			is_multi() const  {return cd->is_multi();} // override
  bool			is_tacss() const  {return true;} // override

  int			count() const {return cd->count();} // number of items
  bool			IsThisProcess() const {return true;} // override

  ~taiIntMimeSource();
protected:
  taiClipData*		cd; // must be set -- is also set as ms
  taiMimeItem*		item() const {return cd->items(iter_idx);} // override

  taiIntMimeSource(taiClipData* cd); // creates an instance from a cd
};

class TA_API taiExtMimeSource: public taiMimeSource { // a taiMime that wraps data from the clipboard etc.
INHERITED(taiMimeSource)
  Q_OBJECT
friend class taiMimeSource;
public:
  int			src_action() const {return msrc_action;} // override
  bool			is_multi() const  {return (count() > 1);} // override
  bool			is_tacss() const  {return (count() >= 1);} // override

  int			count() const {return list.size;} // number of items
  bool			IsThisProcess() const; // override

  ~taiExtMimeSource();
protected:
  taiMimeItem_List	list;
  int			msrc_action;
  int			process_id;
  taiMimeItem*		item() const {return list.SafeEl(iter_idx);} // override
  bool			DecodeDesc(String arg); // decode the full description, return 'true' if valid, build list from desc

  taiExtMimeSource(const QMimeData* ms); // creates an instance from a non-null ms; if ms is tacss, fields are decoded
};

typedef taiMimeSource* taiMimeSourcePtr;



#endif

