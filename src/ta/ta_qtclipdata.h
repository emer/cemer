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


// ta_qtclipdata.h -- clipboard and drag/drop data type definitions -- these are all Qt CoreLib items

#ifndef TA_QTCLIPDATA_H
#define TA_QTCLIPDATA_H

#include "ta_base.h" 

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

  MIME TYPE "tacss/common" -- common header info for any tacss items

    There is only one of these, regardless of whatever other formats are included
      <src edit action>;<process id>;
      
    <src edit action> -- enables us to differentiate between a Cut and a
      Copied object (these flags are NOT used for drag and drop, 
      since the dest determines whether it is a DragMove or a DragCopy
      operation)
    <process id> -- the process id of source; lets us differentiate between
      data that comes from our own internal instance, vs. an external instance
      (note: Mac only allows one instance, so there are never out-of-process
       operations on the Mac.)

  MIME TYPE "tacss/objectdesc" -- compact description of data (no content)

    The data is a list of object descriptors, as follows:
      <item count N>;\n
      [<typename>;<pdp path of object>[;<optional extended data>]*\n]xN

    There is one type/path line per object. 

  MIME TYPE "tacss/objectdata;index=<i>" 0 <= i < N

    The data is the rep (ext dump save text) of the object.

  
  MIME TYPE "text/plain"
    
    taccs/objectdesc
      The same string as the objectdesc string (primarily for diagnostic purposes).
      
    

taiClipData -- sender

  taiClipData is the class used by senders to supply tacss data to the clipboard system.
  This class is derived from QMimeData, and so inherently supports Drap/Drop, and clipboard
  operations.

  This class uses taiClipSrc


To Extend taiMimeSource:

  (1) extend formats_impl() override, check for your own indexes, else delegate to baseclass

*/

/* taiMimeSource

   This class is used for receive or query-type operations. It wraps an existing QMimeData,
   and if that mime source is a tacss mime source, it decodes the object path information.
   But this new class delegates the QMimeData virtuals to the source it wrapped, so it
   behaves essentially identically to that source. Therefore, we pass nothing but taiMimeSource
   objects around, so that we only need one version of every function, yet can have ready access
   to the extra tacss information, that will constitute 95%+ of the actual use cases.


*/


// forwards
class taiMimeItem;
class taiMimeItem_List;
class taiMultiMimeItem;
class taiObjectMimeItem;
class taiObjectsMimeItem;
class taiMimeSource;


class TA_API taiClipData: public QMimeData { // ##NO_CSS
INHERITED(QMimeData)
  Q_OBJECT
public:
  enum EditAction { // extended definitions of clipboard operations for ta/pdp, divided into two field banks: OP and SRC
    EA_SRC_CUT		= 0x00000001, // flag indicating the source was a Clip/Cut operation
    EA_SRC_COPY		= 0x00000002, // flag indicating the source was a Clip/Copy operation
    EA_SRC_DRAG		= 0x00000004, // flag indicating the source was a Drag operation
    EA_SRC_READONLY	= 0x00000008, // flag to tell dest that Cut/Move/Link are not allowed, only Copy

    EA_SRC_MASK		= 0x0000000F,  // note: SRC_ flags could be clear if src op unknown (ex. external mime format)

    EA_CUT		= 0x00000010,
    EA_COPY		= 0x00000020,
    EA_DUPE		= 0x00008000, // duplicate at point of selection
    EA_PASTE		= 0x00000040, // ex. esp for putting an item as a peer to another
    EA_PASTE_INTO	= 0x00000400, // ex. when pasting an item into a list itself
    EA_PASTE_APPEND	= 0x00001000, // ex. for pasting new data rows into tables
    EA_PASTE_ASSIGN	= 0x00004000, // set sel object from obj on clipboard
    EA_DELETE		= 0x00000080,
    EA_CLEAR		= 0x00002000, // ex. for grid cells
    EA_UNLINK		= 0x00000100,
    EA_LINK		= 0x00000200,
    EA_LINK_INTO	= 0x00000800,
    EA_CLIP_OP_MASK	= 0x000FFFF0, // masks the clipboard op codes

    EA_DRAG		= 0x00100000, // initiation of a drag -- note that src can't distinguish move/copy/link ops
    EA_DROP_COPY	= 0x00200000,
    EA_DROP_LINK	= 0x00400000,
    EA_DROP_MOVE	= 0x00800000,
    EA_DROP_ASSIGN	= 0x01000000,
    EA_DROP_COPY_INTO	= 0x02000000,
    EA_DROP_LINK_INTO	= 0x04000000,
    EA_DROP_MOVE_INTO	= 0x08000000,
    EA_DRAG_OP_MASK	= 0x0FF00000, // masks the drag/drop op codes
#ifndef __MAKETA__
    EA_PASTE2		= EA_PASTE | EA_PASTE_INTO, // these guys combine both, to reduce clutter
    EA_LINK2		= EA_LINK | EA_LINK_INTO,
    EA_DROP_COPY2	= EA_DROP_COPY | EA_DROP_COPY_INTO,
    EA_DROP_LINK2	= EA_DROP_LINK | EA_DROP_LINK_INTO,
    EA_DROP_MOVE2	= EA_DROP_MOVE | EA_DROP_MOVE_INTO,
    
    EA_PASTE_XXX	= EA_PASTE | EA_PASTE_INTO | EA_PASTE_APPEND | EA_PASTE_ASSIGN, // all the pastes
    EA_SRC_OPS		= (EA_CUT | EA_COPY | EA_DUPE | EA_DELETE | EA_CLEAR | EA_UNLINK | EA_DRAG), // src ops -- param will be a mime rep of the src obj
    EA_DROP_OPS		= (EA_DROP_COPY | EA_DROP_LINK | EA_DROP_MOVE |
      EA_DROP_COPY_INTO | EA_DROP_LINK_INTO | EA_DROP_MOVE_INTO),
    EA_DST_OPS		= (EA_PASTE_XXX | EA_LINK | 
      EA_LINK_INTO | EA_DROP_OPS), //
    EA_FORB_ON_SRC_CUT	= (EA_LINK | EA_LINK_INTO), // dst ops forbidden when the source operation was Cut
    EA_FORB_ON_SRC_READONLY = (EA_DUPE | EA_DELETE | EA_DROP_MOVE | EA_DROP_MOVE_INTO), 
      // ops forbidden when the source operation forbade Cut/Move
    EA_FORB_ON_MUL_SEL	= (EA_PASTE_XXX | EA_LINK2 |
       EA_DROP_OPS),
        // dst ops forbidden when multi operands selected
    EA_IN_PROC_OPS	= (EA_DROP_MOVE2 | EA_LINK2 | EA_DROP_LINK2), // ops that require an in-process src
#endif
    EA_OP_MASK		= 0x0FFFFFF0 // masks all operation codes

  };
  
  enum EditResult { // passed as result in the various XxxEditAction routines
    ER_ERROR		= -2, // indicates action was attempted but an error occurred
    ER_FORBIDDEN	= -1, // indicates action is not allowed (may not have been resolvable at EditActionsAllowed stage)
    ER_IGNORED 		=  0, // indicates no action was taken, may indicate need to call another handler
    ER_OK		=  1  // indicates successful action taken
  }; //

  // mime-type strings
  static const String	tacss_common;
  
  static EditAction	ClipOpToSrcCode(int ea); // converts an op like EA_CUT into a source field like EA_SRC_CUT

  void			setTextData(const QString& mime_type,
    const QString& data); // sets a mimetype as data
  void			setTextFromStr(const String& str); // avoids double conversion
    
  taiClipData(int src_edit_action_);
  
protected:
  int			src_edit_action;
  const QString		GetCommonDescHeader() const; // the common part, \n terminated
};

class TA_API taiMimeFactory: public taNBase { // ##NO_CSS ##NO_MEMBERS
INHERITED(taNBase)
public:
  static const String 	text_plain;
  
  static QByteArray	StrToByteArray(const String& str);
  static QByteArray	StrToByteArray(const QString& str);
    // convenience, for converting strings to bytearrays
    
  TA_BASEFUNS(taiMimeFactory);//
private:
  void	Initialize() {}
  void	Destroy() {}
};

#define TA_MFBASEFUNS(T) \
  static T* instance() {static T* in = NULL; if (!in) \
    in = (T*)(taiMimeFactory_List::StatGetInstanceByType(&TA_##T)); \
    return in;} \
  TA_BASEFUNS(T)
    

class TA_API taiMimeFactory_List: public taList<taiMimeFactory> {
INHERITED(taList<taiMimeFactory>)
public:
  static taiMimeFactory_List* instance() {return g_instance;}
  static void		setInstance(taiMimeFactory_List* value);
   // we set this during InitLinks for the app-wide global instance
    
  static taiMimeFactory* StatGetInstanceByType(TypeDef* td);
    // get an instance of the exact factory from the global list, making if needed
  
  taiMimeFactory*	GetInstanceByType(TypeDef* td);
    // get an instance of the exact factory, making if needed
    
  TA_BASEFUNS_NCOPY(taiMimeFactory_List);
protected:
  static taiMimeFactory_List*	g_instance; // we set this during InitLinks for the global guy
private:
  void	Initialize() {SetBaseType(&TA_taiMimeFactory);}
  void	Destroy();
};



class TA_API taiObjectMimeFactory: public taiMimeFactory {
INHERITED(taiMimeFactory)
public:
  static const String	text_plain; // all formats support this
  static const String	tacss_objectdesc;
  static const String	tacss_objectdata;
//static taiObjectMimeFactory* instance(); // provided by macro

  virtual void		AddSingleObject(QMimeData* md, taBase* obj);
    // used for putting one object on the clipboard
  virtual void		AddMultiObjects(QMimeData* md, taPtrList_impl* obj_list);
    // used for putting multiple objects on the clipboard
    
  TA_MFBASEFUNS(taiObjectMimeFactory);
protected:
  void			InitHeader(int cnt, QString& str); // common for single/multi
  void			AddHeaderDesc(taBase* obj, QString& str);
    // add entry for one object
  void			AddObjectData(QMimeData* md, taBase* obj, int idx); 
    // add mime entry for one obj
private:
  void	Initialize() {}
  void	Destroy() {}
};



class TA_API taiMimeItem: public taOBase { // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS object that encapsulates the info for one object or other item of clipboard data
INHERITED(taOBase)
public:
  enum MimeItemFlags { // #BITS
    MIF_ZOMBIE		= 0x0001, // constr process failed -- we are a zombie
    MIF_DECODED		= 0x0002 // true once we decode
  };
  
  static int 		data(const QMimeData* md, const QString& mimeType,
    taString& result); // convenience data accessor 
  
  static taiMimeItem* 	ExtractByType(TypeDef* td, taiMimeSource* ms, 
    const String& mimetype = _nilString);
    // return an instance of td, a taiMimeItem class, if possible

  
  QByteArray 		data(const QString& mimeType) const;
  inline int		flags() const {return m_flags;}
  bool			isThisProcess() const; // from ms
  const QMimeData*	mimeData() const;
  inline taiMimeSource*	ms() const {return m_ms;} 
  virtual const String  subkey() const {return _nilString;} 
    // subkeys are a type-dependent way to have more than one guy of the type
  
  bool			Constr(taiMimeSource* ms, const String& subkey = _nilString);
    // returns true if ok, otherwise false, and zombie set
    
  void	SetIndex(int idx) {m_index = idx;} // iml index as convenience
  int	GetIndex() const {return m_index;}
  TA_BASEFUNS_NCOPY(taiMimeItem);

public: // TAI_xxx instance interface -- used for dynamic creation
  virtual taiMimeItem* 	Extract(taiMimeSource* ms, 
    const String& mimetype = _nilString) {return NULL;}
    // if this type can be made from the given md and using the optionally specified specific mimetype (otherwise its default mimetype, or set of possible types is used); NULL result means no, otherwise the newly created and constructed instance is supplied

protected:
  int			m_index;
  int			m_flags;
  taiMimeSource*	m_ms;
  
  inline bool		isDecoded() const {return (m_flags & MIF_DECODED);}
  
  void			AssertData(); // insures data is fetched/decoded
  virtual bool		Constr_impl(const String& subkey) {return true;}
    // returns true if constr went ok
  virtual void		DecodeData_impl() {}
private:
  void	Initialize();
  void	Destroy() {}
};


class TA_API taiMimeItem_List: public taList<taiMimeItem> {
INHERITED(taList<taiMimeItem>)
public:
  TA_BASEFUNS(taiMimeItem_List);
private:
  NCOPY(taiMimeItem_List)
  void	Initialize() {SetBaseType(&TA_taiMimeItem);}
  void	Destroy() {}
};

class TA_API taiMultiMimeItem: public taiMimeItem { // #VIRT_BASE
INHERITED(taiMimeItem)
public:
  
  inline int		count() const {return items.size;}
  taiMimeItem*		item(int idx) const {return items.FastEl(idx);} //note: can be replaced with strongly typed version
  
  void	InitLinks();
  void	CutLinks();
  TA_ABSTRACT_BASEFUNS(taiMultiMimeItem);
  
protected:
  taiMimeItem_List	items; // the subitems
  
private:
  NCOPY(taiMultiMimeItem)
  void	Initialize();
  void	Destroy() {CutLinks();}
};

class TA_API taiObjectMimeItem: public taiMimeItem { // for tacss object
INHERITED(taiMimeItem)
friend class taiObjectsMimeItem;
friend class taiMimeSource; //TEMP
public:
  inline String		typeName() {return m_type_name;};
  inline String		path() {return m_path;}
  TypeDef*		td() {return m_td;}
  taBase*		obj(); // note: only valid when in-process
  
  int			GetObjectData(istringstream& result);
    // #IGNORE gets the object data for the current item; returns number of bytes

  TA_BASEFUNS_NCOPY(taiObjectMimeItem);
protected:
  String		m_type_name;
  TypeDef*		m_td;
  String		m_path;
  taBase*		m_obj;
  override bool		Constr_impl(const String&);
  override void		DecodeData_impl();
private:
  void	Initialize();
  void	Destroy() {}
};

class TA_API taiObjectsMimeItem: public taiMultiMimeItem { // for tacss objects
INHERITED(taiMultiMimeItem)
public:
  bool			allBase() const; // true if all objs derive from taBase
  bool			isMulti() const  {return (items.size > 1);}
  taiObjectMimeItem*	item(int idx) const 
    {return (taiObjectMimeItem*)items.FastEl(idx);} 

  TypeDef*		CommonSubtype() const; // type of item (if 1) or common subtype if multiple

  TA_BASEFUNS_NCOPY(taiObjectsMimeItem);
  
public: // TAI_xxx instance interface -- used for dynamic creation
  override taiMimeItem* Extract(taiMimeSource* ms, 
    const String& mimetype = _nilString);

protected:
  mutable unsigned char	m_all_base; // 0=unknown; -1 no, 1=yes
  override bool		Constr_impl(const String&);
private:
  void	Initialize();
  void	Destroy() {}
}; //



/*
  For taBase objects, taiMimeSource works like an iterator:
    count: the number of items (0 if not tacss items), 1 if 'isMulti' is false
    index: the current index; initialized to 0, except if count=0, then -1; -1=no value -- if set out of range, is saved in range
    xxxx ITER: property of the current index; if index out of range, then values are 0 (ex. "", 0, false)

*/
class TA_API taiMimeSource: public QObject { // #NO_CSS #NO_MEMBERS a delegate/wrapper that is used for dealing with generic Mime data, as well as decoding the tacss mime types -- acts like an iterator (for all properties marked ITER)
INHERITED(QObject)
  Q_OBJECT
public:
  static taiMimeSource*	New(const QMimeData* md);
  static taiMimeSource*	NewFromClipboard(); // whatever is on clipboard
public:
  QByteArray 		data(const QString& mimeType) const;
  int			data(const QString& mimeType, taString& result) const; // provides data to a String; returns # bytes
  int			data(const QString& mimeType, istringstream& result) const; // #IGNORE provides data to an istrstream; returns # bytes
  QStringList 		formats() const; // override
  bool			hasFormat(const QString& mimeType) const;
  const QMimeData*	mimeData() const {return m_md;}
  int			srcAction() const {return m_src_action;}
  bool			isThisProcess() const {return m_this_proc;} // override
  
  taiObjectsMimeItem*	objects() const; // convenience GetMimeItem accessor for this type -- NULL if not tacss on clipboard

  taiMimeItem*		GetMimeItem(TypeDef* td, const String& subkey = _nilString);
    // get a guy of specified taiMimeItem type, using optional subkey; NULL if that type not supported; note: we check our list first, before trying to make a new guy
    
  void			ResolveObjects(); // for inProcess, binds all objects to the paths, prior to opeations
  
  ~taiMimeSource();

public slots:
  void			ms_destroyed(); // mostly for debug

protected:
  const QMimeData* 	m_md;
  taiMimeItem_List	items;
  // the following are extracted from the common tacss header info:
  int			m_src_action;
  bool			m_this_proc;
  
  void			Decode(); // decodes the guy, noop if decoded
  bool			Decode_common(String arg);
  
  taiMimeSource(const QMimeData* m_md); // creates an instance from a non-null m_md; if m_md is tacss, fields are decoded


public: // compatability interface
/*TODO: this interface is only for taiObjectsMimeItem accessing using an
  iteration index -- this is too complicated, but is left this way 
  for the time being, since all the taBase Query/Action classes are based
  on this -- to standardize this, we'd have to change the semantics of those
  calls such that terminal guys can grok looking at all objects, not just
  one at a time.
*/
  int			count() const {return (mi) ? mi->count() : 0;}
  bool			isMulti() const  {return (mi) ? (mi->count() > 1) : false;}
  bool			isObject() const;

  int			objectData(istringstream& istr);
  taBase*		tabObject() const; 
  
  int			index() const; // current index value; -1 if none
  void			setIndex(int val) 
    {iter_idx = ((val >= 0) && (val < count())) ? val : -1;}
    // sets index; must be -1 or in range, else sets to -1
  String		typeName() const 
    {return (isObject() && inRange()) ? 
      ((taiObjectMimeItem*)item())->typeName() : _nilString;}
    // ITER empty if not a tacss object mime type
  TypeDef*		td() const 
    {return (isObject() && inRange()) ?
      ((taiObjectMimeItem*)item())->td() : NULL;}
    // ITER the TypeDef associated with typeName, or NULL if not in our type list or not tacss
  bool			isBase() const 
   {return (isObject() && inRange()) ? true : false;};
    // ITER true if the object is derived from taBase NOTE: all objects are base
  String		path() const 
    {return (isObject() && inRange()) ? 
      ((taiObjectMimeItem*)item())->path() : _nilString;};
    // ITER if a taBase object, its full path
protected: // compatability interface
  int			m_itm_cnt;
  mutable taiObjectsMimeItem*	mi; // cache
  int			iter_idx; // iteration index: =-1, not started yet; >=0 < items.size, in range; =size, past end
  bool			inRange() const {return ((iter_idx >= 0) && (iter_idx < count()));}// true if index in range
  taiMimeItem*		item() const {return item(iter_idx);} // current item -- must always be checked with inRange before access

  taiMimeItem*		item(int idx) const {
    if (mi) return mi->item(idx); else return NULL;} // TEMP
};


#endif

