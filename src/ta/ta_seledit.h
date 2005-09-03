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

#ifndef TA_SELEDIT_H
#define TA_SELEDIT_H

#include "ta_def.h"
#include "ta_base.h"

//////////////////////////////////
//	select edit dialog	//
//////////////////////////////////

class TA_API SelectEditConfig : public taBase {
  // #EDIT_INLINE ##NO_TOKENS special parameters for controlling the select edit display
public:
  bool		auto_edit;	// automatically bring up edit dialog upon loading
  String_Array	mbr_labels;	// extra labels at the start of each member label for the selected fields
  String_Array	meth_labels;	// extra labels at the start of each method label for the selected functions

  void	Initialize();
  void	Destroy();
  void	InitLinks();
  void	Copy_(const SelectEditConfig& cp);
  COPY_FUNS(SelectEditConfig, taBase);
  TA_BASEFUNS(SelectEditConfig);
};

class TA_API SelectEdit : public taNBase {
  // ##EXT_edit Selectively edit members from different objects
public:
  SelectEditConfig config;	// special parameters for controlling the display

  taBase_List	mbr_bases;	// #LINK_GROUP #READ_ONLY #AKA_bases the bases for each element in the list
  String_Array	mbr_strs;	// #READ_ONLY #AKA_member_strs string names of mbrs on bases -- used for saving
  MemberSpace	members;	// #READ_ONLY #NO_SAVE member defs
  String_Array	mbr_base_paths; // #READ_ONLY #NO_SAVE paths to base objects for BaseChangeSave

  taBase_List	meth_bases;	// #LINK_GROUP #READ_ONLY the bases for each element in the list
  String_Array	meth_strs;	// #READ_ONLY string names of meths on bases -- used for saving
  MethodSpace	methods;	// #READ_ONLY #NO_SAVE method defs
  String_Array	meth_base_paths; // #READ_ONLY #NO_SAVE paths to base objects for BaseChangeSave

  virtual int	FindMbrBase(TAPtr base, MemberDef* md);
  // find a given base and member, returns index
  virtual bool	SelectMember(TAPtr base, MemberDef* md, const char* lbl);
  // add new member to edit if it isn't already here (returns true), otherwise update lbl (returns false)
  virtual bool	SelectMemberNm(TAPtr base, const char* md, const char* lbl);
  // add new member to edit if it isn't already here (returns true), otherwise update lbl (returns false)

  virtual int	FindMethBase(TAPtr base, MethodDef* md);
  // find a given base and method, returns index
  virtual bool	SelectMethod(TAPtr base, MethodDef* md, const char* lbl);
  // add new method to edit if it isn't already here (returns true), otherwise remove (returns false)
  virtual bool	SelectMethodNm(TAPtr base, const char* md, const char* lbl);
  // add new method to edit if it isn't already here (returns true), otherwise remove (returns false)

  virtual void	UpdateAllBases();	// perform update-after-edit on all base objects

  virtual void	RemoveField(int idx);
  //  remove edit data item at given index
  virtual void	MoveField(int from, int to);
  // #MENU #MENU_ON_SelectEdit move member to edit from index to index

  virtual void	RemoveFun(int idx);
  //  remove function at given index
  virtual void	MoveFun(int from, int to);
  // #MENU move function to edit from index to index

  virtual void	NewEdit();
  // #MENU #MENU_SEP_BEFORE closes current edit dialog and makes a new one (with any changes)

  virtual void	GetMembsFmStrs(); // get members from strings (upon loading)
  virtual void	GetMethsFmStrs(); // get methods from strings (upon loading)
  virtual void	GetAllPaths();	// get paths for all current objects

  virtual bool	BaseClosing(TAPtr base);
  // this base object is about to be closed (removed), if i edit it, then I need to save and reopen (returns true if edited)
  static bool	BaseClosingAll(TAPtr base);
  // calls base closing on all SelectEdit tokens..
  virtual void	BaseChangeSave(); // close edit dialog and save paths to current bases
  virtual void	BaseChangeReShow(); // re-show the edit dialog loading bases from saved paths

  virtual String GetMbrLabel(int idx); // get full label for member
  virtual String GetMethLabel(int idx);	// get full label for method

  int	Dump_Load_Value(istream& strm, TAPtr par=NULL);
  // reset everything before loading
  int	Dump_Save_Value(ostream& strm, TAPtr par=NULL, int indent = 0);
  // get paths before saving

  void	UpdateAfterEdit();
  void	Initialize();
  void 	InitLinks();
  void	Copy_(const SelectEdit& cp);
  COPY_FUNS(SelectEdit, taNBase);
  TA_BASEFUNS(SelectEdit);
};


#endif
