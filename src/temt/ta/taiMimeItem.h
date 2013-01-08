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

#ifndef taiMimeItem_h
#define taiMimeItem_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


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
  TA_BASEFUNS_NOCOPY(taiMimeItem);

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

#endif // taiMimeItem_h
