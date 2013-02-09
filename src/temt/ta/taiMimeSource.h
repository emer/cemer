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

#ifndef taiMimeSource_h
#define taiMimeSource_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QObject>
#endif

// member includes:
#include <taString>
#include <taiMimeItem_List>
#include <taiObjectsMimeItem>

// declare all other types mentioned but not required to include:
class QMimeData; //
class QString; //
class QByteArray; //
class QStringList; //

/* taiMimeSource

   This class is used for receive or query-type operations. It wraps an existing QMimeData,
   and if that mime source is a tacss mime source, it decodes the object path information.
   But this new class delegates the QMimeData virtuals to the source it wrapped, so it
   behaves essentially identically to that source. Therefore, we pass nothing but taiMimeSource
   objects around, so that we only need one version of every function, yet can have ready access
   to the extra tacss information, that will constitute 95%+ of the actual use cases.
*/

/*
  For taBase objects, taiMimeSource works like an iterator:
    count: the number of items (0 if not tacss items), 1 if 'isMulti' is false
    index: the current index; initialized to 0, except if count=0, then -1; -1=no value -- if set out of range, is saved in range
    xxxx ITER: property of the current index; if index out of range, then values are 0 (ex. "", 0, false)

*/

taTypeDef_Of(taiMimeSource);

class TA_API taiMimeSource: public QObject { // #NO_CSS #NO_MEMBERS a delegate/wrapper that is used for dealing with generic Mime data, as well as decoding the tacss mime types -- acts like an iterator (for all properties marked ITER)
INHERITED(QObject)
  Q_OBJECT
public:
  static taiMimeSource*	New(const QMimeData* md);
  static taiMimeSource*	NewFromClipboard(); // whatever is on clipboard
public:
  QByteArray 		data(const QString& mimeType) const;
  int			data(const QString& mimeType, taString& result) const; // provides data to a String; returns # bytes
#ifndef __MAKETA__
  int			data(const QString& mimeType, std::istringstream& result) const; // #IGNORE provides data to an istrstream; returns # bytes
#endif
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

#ifndef __MAKETA__
  int			objectData(std::istringstream& istr);
#endif
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

#endif // taiMimeSource_h
