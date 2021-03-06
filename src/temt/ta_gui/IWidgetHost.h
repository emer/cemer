// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef IWidgetHost_h
#define IWidgetHost_h 1

// parent includes:
#include <ITypedObject>
#include <TypeItem>

// member includes:

// declare all other types mentioned but not required to include:
class iMainWindowViewer; // 
class taBase; // 
class TypeDef; // 
class taiWidget; //
class iColor; //


class TA_API IWidgetHost: public ITypedObject { // #VIRT_BASE #NO_INSTANCE #NO_CSS
public:
  virtual const iColor  colorOfCurRow() const = 0; // #IGNORE background color of cur row
  virtual const iColor  backgroundColor() const = 0; // #IGNORE general background color
  virtual const iColor  backgroundColorDark() const = 0; // #IGNORE dark background color, if striped
  virtual bool          isConstructed() = 0;
  virtual bool          isModal() = 0;
  virtual bool          isReadOnly() = 0;
  virtual iMainWindowViewer* viewerWindow() const {return NULL;} // used to set cliphandler
  virtual bool          HasChanged() = 0; // 'true' if has changed
  virtual void*         Root() const = 0; // root of the object
  virtual taBase*       Base() const = 0; // root of the object, if a taBase
  virtual TypeDef*      GetRootTypeDef()const  = 0; // TypeDef on the root, for casting
  virtual void          GetValue() = 0; // copy gui to value
  virtual void          GetImage() = 0; // copy value to gui
  virtual void          Changed() {} // called by embedded item to indicate contents have changed
  virtual void          SetItemAsHandler(taiWidget* item, bool set_it = true); // called by compatible controls to set or unset the control as clipboard/focus handler (usually don't need to unset); in ta_qtviewer.cpp
  virtual void          Apply_Async() = 0; // provided for the APPLY_IMMED functionality
  virtual void*         GetAlternateSelection() { return NULL; }

  IWidgetHost() {}
  virtual ~IWidgetHost() {} //
};

#endif // IWidgetHost_h
