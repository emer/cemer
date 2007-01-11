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

  MIME TYPE "tacss/matrixdesc" -- description of matrix data (no content)

    <cols>;<rows>;
      cols >= 1; rows >= 1

    The data itself (text/plain) is in TSV format.
    
    Note that this format is primarily to make decoding of the data faster
    and more definite where tacss is the source of the data, compared with
    just parsing the text/plain data (which the decoder can do, to import
    spreadsheet data.)
    .
    
  MIME TYPE "tacss/tabledesc" -- description of table data (no content)

    The data describes the 2-d-flattened geometry of each of the cols in the clip
      <cols>;<rows>;<col0-cols>;<col0-rows>[<col1-cols>;<col1-rows> ...];
      cols >=1; rows >= 1
      for scalar cols: colx-cols=colx-rows=1
      
    The data itself (text/plain) is in a TSV tabular form, of total
    Sigma(colx-cols)x=0:N by <rows> * Max(colx-rows) -- non-existent values
    will just have blank entries.
      
  
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


class TA_API taiClipData: public QMimeData { // ##NO_CSS
INHERITED(QMimeData)
  Q_OBJECT
public:
  enum EditAction { // extended definitions of clipboard operations for ta/pdp, divided into two field banks: OP and SRC
    EA_SRC_CUT		= 0x00001, // flag indicating the source was a Clip/Cut operation
    EA_SRC_COPY		= 0x00002, // flag indicating the source was a Clip/Copy operation
    EA_SRC_DRAG		= 0x00004, // flag indicating the source was a Drag operation
    EA_SRC_READONLY	= 0x00008, // flag to tell dest that Cut/Move/Link are not allowed, only Copy

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
    EA_FORB_ON_SRC_READONLY = (EA_LINK | EA_SET_AS_SUBGROUP | EA_SET_AS_SUBITEM |
      EA_DROP_LINK | EA_DROP_MOVE), 
      // dst ops forbidden when the source operation forbade Cut/Move
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
  }; //

  // mime-type strings
  static const String	tacss_common;
  
  static EditAction	ClipOpToSrcCode(int ea); // converts an op like EA_CUT into a source field like EA_SRC_CUT

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
    
  TA_BASEFUNS(taiMimeFactory);
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
public:
  static taiMimeFactory_List* instance() {return g_instance;}
  static void		setInstance(taiMimeFactory_List* value);
   // we set this during InitLinks for the app-wide global instance
    
  static taiMimeFactory* StatGetInstanceByType(TypeDef* td);
    // get an instance of the exact factory from the global list, making if needed
  
  taiMimeFactory*	GetInstanceByType(TypeDef* td);
    // get an instance of the exact factory, making if needed
    
  TA_BASEFUNS(taiMimeFactory_List);
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


//////////////////////////////////
// 	taiMimeItem		//
//////////////////////////////////

class TA_API taiMimeItem: public QObject { // object that encapsulates the info for one object or other single item of clipboard data, either sending or receiving, in-process or external
INHERITED(QObject)
  Q_OBJECT
friend class taiMimeSource;
public:
  enum ItemFlags { // #BITS
    IF_THIS_PROCESS	= 0x0001, // when sent/received in process
    IF_SEND		= 0x0002 // on for Send, off for Receive
  };
  
  inline bool		isThisProcess() const {return m_this_proc;}
  void			setThisProcess(bool value) {m_this_proc = value;}
  virtual bool		isObject() const {return false;}
  virtual bool		isMatrix() const {return false;}
  virtual bool		isTable() const {return false;}
  
  taiMimeItem();
  
protected:
  bool			m_this_proc; 
  virtual void		objDestroyed_impl() {} // datalinks will connect to this so our obj doesn't become invalid
  virtual void		SetData(ostream& ost, const String& fmt = _nilString) {} // (s) get the data from the object
};


class TA_API taiMimeItem_List: public taPtrList<taiMimeItem> {
public:
  void			El_Done_(void*);	// override, when "done" (delete)
};

/*
class TA_API taiMatDataMimeItem: public taiMimeItem { // for matrix and table data
INHERITED(taiMimeItem)
  Q_OBJECT
public:
  
public: // i/f for tabular data guy
  override bool		isMatrix() const;
  override bool		isTable() const;
  virtual void		GetDataGeom(int& cols, int& rows) const = 0;
    // number of cols/rows in the overall data
  virtual void		GetColGeom(int col, int& cols, int& rows) const = 0;
    // 2-d geom of the indicated column; always 1x1 (scalar) for matrix data
  virtual void		GetMaxRowGeom(int& max_row) const = 0;
    // longest cell geom determines overall row geom
    
protected:
  int			m_data_type; // one of ST_MATRIX_DATA or TABLE_DATA
  override void 	GetFormats_impl(QStringList& list, int idx) const; 
  taiMatDataMimeItem(int data_type);
}; 


class TA_API taiRcvMatDataMimeItem: public taiMatDataMimeItem { 
  // for received mat or table data, or compatible foreign mat data
INHERITED(taiMatDataMimeItem)
  Q_OBJECT
friend class taiExtMimeSource;
public:
  
public: // i/f for tabular data guy
  void			GetDataGeom(int& cols, int& rows) const
    {cols = m_cols;  rows = m_rows;}
  void			GetColGeom(int col, int& cols, int& rows) const;
  void			GetMaxRowGeom(int& max_row) const {max_row = m_max_row;} 

protected:
  int			m_cols;
  int			m_rows;
  int			m_max_row;
  taBase_List		m_geoms; // list of GeomData
  
  void			DecodeMatrixDesc(String& arg); // same for both
  void			DecodeTableDesc(String& arg); // the extra stuff
  
  taiRcvMatDataMimeItem(int data_type);
};*/


class TA_API taiObjDataMimeItem: public taiMimeItem { // for tacss objects
INHERITED(taiMimeItem)
  Q_OBJECT
  
public:
  override bool		isObject() const {return true;}
  
public: // i/f for object guys -- not used for others
  virtual void*		obj() const {return m_obj;}
  virtual TypeDef*	td() const {return NULL;}
  virtual String	typeName() const {return _nilString;}
  virtual bool		isBase() const {return false;}
  virtual String	path() const {return _nilString;}
public slots:
  void			objDestroyed() {objDestroyed_impl();} // datalinks will connect to this so our obj doesn't become invalid


protected:
  taBase*		m_obj; 
  taiObjDataMimeItem();
};


class TA_API taiRcvMimeItem: public taiObjDataMimeItem { // specialized for tacss receiving
INHERITED(taiObjDataMimeItem)
  Q_OBJECT
friend class taiMimeSource;
public:
  virtual void*		obj() const;
  TypeDef*		td() const {return mtd;}
  String		typeName() const {return mtype_name;}; // override
  bool			isBase() const {return m_is_base;} // override
  String		path() const {return mpath;} // override

protected:
  TypeDef*		mtd;
  String		mtype_name;
  bool			m_is_base;
  String		mpath;
  taiRcvMimeItem(const String type_name_, const String path_); // sets td and isBase
};


/*
  taiMimeSource works like an iterator:
    count: the number of items (0 if not tacss items), 1 if 'isMulti' is false
    index: the current index; initialized to 0, except if count=0, then -1; -1=no value -- if set out of range, is saved in range
    xxxx ITER: property of the current index; if index out of range, then values are 0 (ex. "", 0, false)

*/
class TA_API taiMimeSource: public QObject { // a delegate/wrapper that is used for dealing with generic Mime data, as well as decoding the tacss mime types -- acts like an iterator (for all properties marked ITER)
INHERITED(QObject)
  Q_OBJECT
public:
  enum SourceType {
    ST_UNDECODED,	// still need to decode
    ST_UNKNOWN,		// not a tacss type
    ST_OBJECT,		// tacss object
    ST_MATRIX_DATA,	// matrix, or matrix-compatible data (ex Excel)
    ST_TABLE_DATA	// tacss Table data
  };
  
  static taiMimeSource*	New(const QMimeData* ms); // we use a static method for extensibility -- creates correct subtype
//  static taiMimeSource*	New2(taiClipData* cd); // we use a static method for extensibility -- creates correct subtype
  static taiMimeSource*	NewFromClipboard(); // whatever is on clipboard

  int			srcAction() const {return m_src_action;} // override
  bool			isMulti() const  {return (count() > 1);} // override
  bool			isObject() const {return (m_src_type == ST_OBJECT);}
  bool			isTabularData() const 
    {return ((m_src_type == ST_MATRIX_DATA) || (m_src_type == ST_TABLE_DATA));}
  bool			isMatrixData() const {return (m_src_type == ST_MATRIX_DATA);}
  bool			isTableData() const {return (m_src_type == ST_TABLE_DATA);}

  int			count() const {return list.size;} // number of items
  bool			IsThisProcess() const {return m_this_proc;} // override
  
  void			Decode(); // decodes the guy, noop if decoded

  inline SourceType	type() const {return m_src_type;}

  void*			object() const;
    // gets the object, if possible -- only valid for IsThisProcess true
  taBase*		tabObject() const; // gets a taBase object, if possible, otherwise NULL -- only valid for IsThisProcess true
  int			objectData(istringstream& result) const;
    // #IGNORE gets the object data for the current item; returns number of bytes
  QStringList 		formats() const; // override
  bool			hasFormat(const QString& mimeType) const;
  QByteArray 		data(const QString& mimeType) const;
  int			data(const QString& mimeType, taString& result) const; // provides data to a String; returns # bytes
  int			data(const QString& mimeType, istringstream& result) const; // #IGNORE provides data to an istrstream; returns # bytes

  virtual TypeDef*	CommonSubtype() const; // type of item (if 1) or common subtype if multiple

  ~taiMimeSource();

public slots:
  void			ms_destroyed(); // mostly for debug

public: // iteration guys
  int			index() const; // current index value; -1 if none
  void			setIndex(int val) 
    {iter_idx = ((val >= 0) && (val < count())) ? val : -1;}
    // sets index; must be -1 or in range, else sets to -1
public: // object i/f
  String		typeName() const 
    {return (isObject() && inRange()) ? 
      ((taiObjDataMimeItem*)item())->typeName() : _nilString;}
    // ITER empty if not a tacss object mime type
  TypeDef*		td() const 
    {return (isObject() && inRange()) ?
      ((taiObjDataMimeItem*)item())->td() : NULL;}
    // ITER the TypeDef associated with typeName, or NULL if not in our type list or not tacss
  bool			isBase() const 
   {return (isObject() && inRange()) ?
     ((taiObjDataMimeItem*)item())->isBase() : false;};
    // ITER true if the object is derived from taBase
  String		path() const 
    {return (isObject() && inRange()) ? 
      ((taiObjDataMimeItem*)item())->path() : _nilString;};
    // ITER if a taBase object, its full path
    
public: // tabular data i/f
/*  void		GetDataGeom(int& cols, int& rows) const
   {if (isTabularData() && inRange())
     ((taiMatDataMimeItem*)item())->GetDataGeom(cols, rows);}
    // ITER  number of cols/rows in the overall data
  void		GetColGeom(int col, int& cols, int& rows) const
   {if (isTabularData() && inRange())
     ((taiMatDataMimeItem*)item())->GetColGeom(col, cols, rows);}
   // 2-d geom of the indicated column; always 1x1 (scalar) for matrix data
  void		GetMaxRowGeom(int& max_row) const
   {if (isTabularData() && inRange())
     ((taiMatDataMimeItem*)item())->GetMaxRowGeom(max_row);} */


protected:
  const QMimeData* 	ms;
  int			iter_idx; // iteration index: =-1, not started yet; >=0 < items.size, in range; =size, past end
  SourceType		m_src_type;
  bool			inRange() const {return ((iter_idx >= 0) && (iter_idx < count()));}// true if index in range
  taiMimeItem*		item() const {return item(iter_idx);} // current item -- must always be checked with inRange before access

  void			AssertList(); // makes sure list is constructed

  taiMimeSource(const QMimeData* ms); // creates an instance from a non-null ms; if ms is tacss, fields are decoded
protected: // was in ExtMimeSource
  taiMimeItem_List	list;
  // the following are extracted from the common tacss header info:
  int			m_itm_cnt;
  int			m_src_action;
  bool			m_this_proc;
  
  taiMimeItem*		item(int idx) const {return list.SafeEl(idx);} // override
  virtual void		Decode_impl(); 
  bool			Decode_common(String arg);
  bool			DecodeDesc_object(String arg); // decode the full description, return 'true' if valid, build list from desc
  bool			DecodeDesc_matrix(String arg); // decode the full description, return 'true' if valid, build list from desc
  bool			DecodeDesc_table(String arg); // decode the full description, return 'true' if valid, build list from desc
  bool			TryDecode_matrix(); // true if we can interpret as matrix data 
  
};


#endif

