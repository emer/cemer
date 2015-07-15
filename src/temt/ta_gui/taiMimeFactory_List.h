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

#ifndef taiMimeFactory_List_h
#define taiMimeFactory_List_h 1

// parent includes:
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:
class taiMimeFactory; // 
taTypeDef_Of(taiMimeFactory);
class TypeDef; // 


taTypeDef_Of(taiMimeFactory_List);

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
    
  TA_BASEFUNS_NOCOPY(taiMimeFactory_List);
protected:
  static taiMimeFactory_List*	g_instance; // we set this during InitLinks for the global guy
private:
  void	Initialize() {SetBaseType(&TA_taiMimeFactory);}
  void	Destroy();
};

#endif // taiMimeFactory_List_h
