/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

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
