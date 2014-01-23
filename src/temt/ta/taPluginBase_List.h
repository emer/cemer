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

#ifndef taPluginBase_List_h
#define taPluginBase_List_h 1

// parent includes:
#include <taPluginBase>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taPluginBase_List);

class TA_API taPluginBase_List: public taList<taPluginBase> {
  // ##CHILDREN_INLINE ##NO_UPDATE_AFTER ##NO_TOKENS plugins available to the program (also used for deps)
INHERITED(taList<taPluginBase>)
public:
  taPluginBase*		FindUniqueId(const String& value); // find by unique_id
  
  TA_BASEFUNS_NOCOPY(taPluginBase_List);
  
protected:
// forbid most clip ops, since we are managed based on existing plugins
  virtual void		QueryEditActions_impl(const taiMimeSource* ms,
    int& allowed, int& forbidden);
  void		ChildQueryEditActions_impl(const MemberDef* md, const taBase* child,
    const taiMimeSource* ms, int& allowed, int& forbidden) CPP11_OVERRIDE;

private:
  void	Initialize() {}
  void	Destroy() {}
};

#endif // taPluginBase_List_h
