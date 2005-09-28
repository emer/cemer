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


// ta_qtbrowse_def.h -- global, gui-non-dependant definitions for the qtbrowsing interface
//
// this file can(should) be included regardless of GUI state of build

#ifndef TA_QTBROWSE_DEF_H
#define TA_QTBROWSE_DEF_H


/* Node Bitmap flags

The icon client (ex. Browser) will query for an icon, passing the attributes such as Open, Linked, etc.
The icon supplier (ex. taBase object) returns an icon bitmap, and an indication of what attribute flags it
supports. The Browser can then add its own generic modifiers for attributes not supported by the supplier.
ex. the browser can add a generic small arrow to indicate a link
*/

enum NodeBitmapFlags {
  NBF_FOLDER_OPEN		= 0x01,
  NBF_LINK_ITEM			= 0x02
};


/* Action Names */

#define _fileNewAction 		"fileNewAction"
#define _fileOpenAction		"fileOpenAction"
#define _fileSaveAction		"fileSaveAction"
#define _fileSaveAsAction 	"fileSaveAsAction"
#define _filePrintAction 	"filePrintAction"
#define _fileCloseWindowAction 	"fileCloseWindowAction"
#define _editUndoAction 	"editUndoAction"
#define _editRedoAction 	"editRedoAction"
#define _editCutAction 		"editCutAction"
#define _editCopyAction		"editCopyAction"
#define _editPasteAction	"editPasteAction"
#define _editLinkAction		"editLinkAction"
#define _editDeleteAction	"editDeleteAction"
#define _viewRefreshAction	"viewRefreshAction"
#define _helpHelpAction		"helpHelpAction"
#define _helpAboutAction	"helpAboutAction"

/* Toolbar Names */

#define _defaultToolBar		"defaultToolBar"

/* NUKE

class ITypedObject {//#NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE
  // this is the interface available to the host data object
public:
  virtual void*		This() = 0; // returns the base pointer to the object ("this"); cast to type
  virtual void*		GetTypeDef() = 0; // returns the ta typedef -- do a cast to TypeDef*
  virtual bool		InheritsFrom_(void* typ) = 0; // returns true if we inherit from typ; casts typ to TypeDef*
  virtual ~ITypedObject() {}
};



class ISelectionContainer: public ITypedObject {//#NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE
  // this is the interface available to the host data object
public:
  virtual void 		SetActionEnabled_(const char* act_name, bool enabled) = 0; // enable/disable an action
  virtual void 		SetActionChecked_(const char* act_name, bool checked) = 0; // check/uncheck a toggle action
  virtual bool 		GetActionEnabled_(const char* act_name) = 0; // get enable/disable status of an action
  virtual void 		GetActionChecked_(const char* act_name) = 0; // get check/uncheck status of a toggle action

  virtual ~ISelectionContainer() {}
};




class ISelectionList {//#NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE
  // this is the interface available to the host data object
public:
  virtual int		Count_(); // number if items in list
  virtual ISelectable*	Item_(int idx); // get the item at idx
  virtual ~ISelectionList() {}
};


class ISelectable: public ITypedObject {//#NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE
  // this is the interface available to the host data object
public:
  virtual ISelectionContainer*	SelectionContainer_() = 0; // returns sc for this object
    // called by UI; item should set result=true if it can select in this context
  virtual void		CanSelect_(ISelectionList* slist, bool& result) = 0;
    // called by UI; item should set result=true if it can select in this context
  virtual bool		CanMultiSelect_() = 0;
    // generally a static property of elements of this class -- return 'true' if this item can be selected with other items
  virtual void		SelectionChanged_(ISelectionList* slist) = 0;
    // called by UI when another item has been added or removed from selection -- we should update
  virtual ~ISelectable() {}
};
*/

#endif
