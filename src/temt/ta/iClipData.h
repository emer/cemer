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

#ifndef iClipData_h
#define iClipData_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QMimeData>
#endif

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:

/*
  The clipboard/Drag-Drop model involves single or lists of data items that can be
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

  Emergent supplies clipboard data as follows:
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
      
iClipData -- sender

  iClipData is the class used by senders to supply tacss data to the clipboard system.
  This class is derived from QMimeData, and so inherently supports Drap/Drop, and clipboard
  operations.

  This class uses taiClipSrc

To Extend taiMimeSource:

  (1) extend formats_impl() override, check for your own indexes, else delegate to baseclass

*/

class TA_API iClipData: public QMimeData { // ##NO_CSS
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
    EA_PASTE		= 0x00000040, // ex. esp for putting an item as a peer to another
    EA_DELETE		= 0x00000080,
    EA_UNLINK		= 0x00000100,
    EA_LINK		= 0x00000200,
    EA_PASTE_INTO	= 0x00000400, // ex. when pasting an item into a list itself
    EA_LINK_INTO	= 0x00000800,
    EA_PASTE_APPEND	= 0x00001000, // ex. for pasting new data rows into tables
    EA_CLEAR		= 0x00002000, // ex. for grid cells
    EA_PASTE_ASSIGN	= 0x00004000, // set sel object from obj on clipboard
    EA_DUPE		= 0x00008000, // duplicate at point of selection
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
    EA_FORB_ON_SRC_READONLY = (EA_DUPE | EA_DELETE | EA_CLEAR |
      EA_DROP_MOVE | EA_DROP_MOVE_INTO), 
      // ops forbidden when the source operation forbade Cut/Move
    EA_FORB_ON_DST_READONLY = (EA_CLEAR | EA_PASTE_XXX |
      EA_DROP_ASSIGN | EA_DROP_COPY_INTO | EA_DROP_MOVE_INTO), 
      // ops forbidden when the dst is readonly -- note this includes src ops
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
    
  iClipData(int src_edit_action_);
  
protected:
  int			src_edit_action;
  const QString		GetCommonDescHeader() const; // the common part, \n terminated
};


#endif // iClipData_h
