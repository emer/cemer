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

#ifndef tabMisc_h
#define tabMisc_h 1

// parent includes:

// member includes:
#include <taBase_RefList>
#include <taBase_FunCallList>
#include <ContextFlag>

// declare all other types mentioned but not required to include:
class taRootBase; // 
class taBase; // 


class TA_API tabMisc {
  // #NO_TOKENS #INSTANCE miscellaneous useful stuff for taBase
friend class taBase;
friend class taList_impl;
public:
  static taRootBase*    root;
  // root of the structural object hierarchy
  static taBase*        cur_undo_save_top;
  // #READ_ONLY #NO_SAVE the object under which everything is being saved for the purposes of an undo record -- only valid use is to determine if pointer is same as another one -- do NOT attempt to access the object pointed to -- don't want to change the save state
  static taBase*        cur_undo_mod_obj;
  // #READ_ONLY #NO_SAVE the object that is being directly modified, triggering an undo save -- only valid use is to determine if pointer is same as another one -- do NOT attempt to access the object pointed to -- don't want to change the save state, and it might have died or something
  static taBase*        cur_undo_save_owner;
  // #READ_ONLY #NO_SAVE for actions that alter the structural hierarchy (adding, moving, deleting items), this is the list or group that owns the objects being modified -- every object that has special is_undo_saving optimizations should check for this being non-null, and check if IsChildOf(undo_save_owner) -- if true, they should save!

  static taBase_RefList delayed_updateafteredit;
  // list of objs to be update-after-edit'd in the wait process
  static taBase_FunCallList  delayed_funcalls;
  // functions to call during the waiting process -- variant value is the object, and name is the function
  static ContextFlag    in_wait_proc; // context -- don't do WaitProc

  static void           WaitProc();
  // wait process function: process all the delayed stuff

  static void           DelayedUpdateAfterEdit(taBase* obj);
  // call update-after-edit on object in wait process (in case this does other kinds of damage..)
  static void           DelayedFunCall_gui(taBase* obj, const String& fun_name);
  // perform a delayed function call on this object of given function name (using CallFun) -- if args required they will be prompted for, but that is probably not a great idea from the user's perspective.. best for void functions -- gui version for gui feedback events -- checks for gui_active
  static void           DelayedFunCall_nogui(taBase* obj, const String& fun_name);
  // perform a delayed function call on this object of given function name (using CallFun) -- if args required they will be prompted for, but that is probably not a great idea from the user's perspective.. best for void functions -- nogui version -- doesn't check for gui

  static void           DeleteRoot(); // get rid of root, if not nuked already
  static void           WaitProc_Cleanup(); // cleanup the waitproc lists, prior to quitting (makes for a cleaner shutdown)

protected:
  static taBase_RefList delayed_close;
  // list of objs to be removed in the wait process (e.g. when objs delete themselves)

  static void           DelayedClose(taBase* obj);
  // close this object during the wait process (after other events have been processed and we are outside of any functions within the to-be-closed object) -- USE taBase APIs for this!

  static bool           DoDelayedCloses();
  static bool           DoDelayedUpdateAfterEdits();
  static bool           DoDelayedFunCalls();
  static bool           DoAutoSave();
};

#endif // tabMisc_h
