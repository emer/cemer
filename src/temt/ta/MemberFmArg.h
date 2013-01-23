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

#ifndef MemberFmArg_h
#define MemberFmArg_h 1

// parent includes:
#include <MemberProgEl>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(MemberFmArg);

class TA_API MemberFmArg: public MemberProgEl { 
  // set a member (attribute) on an object to a value given by a startup argument passed to overall program when it was run -- if argument was not set by user, nothing happens.  IMPORTANT: must also include a RegisterArgs program element BEFORE this item in the program code to register this argument and process the command list
INHERITED(MemberProgEl)
public:
  String		arg_name; // argument name -- this will be passed on the command line as <arg_name>=<value> (no spaces) (e.g., if arg_name is "rate" then command line would be rate=0.01 and internal arg name is just "rate" -- can be accessed using taMisc arg functions using that name)
  bool			update_after; // call UpdateAfterEdit after setting the member: useful for updating displays and triggering other computations based on changed value, but this comes at a performance cost 
  bool			quiet;	      // do not emit a message when arg is set and member value is assigned (otherwise, informational msg is printed -- useful for startup code output)
  
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }
  override String	GetToolbarName() const { return "memb=arg"; }

  void	GenRegArgs(Program* prog);
  // #IGNORE generate RegisterArgs code

  //  PROGEL_SIMPLE_BASEFUNS(MemberFmArg);
  void Copy_(const MemberFmArg& cp);
  SIMPLE_LINKS(MemberFmArg);
  TA_BASEFUNS(MemberFmArg);
protected:
  ProgVar* 		prv_obj; // #IGNORE used to track changes in obj type to clear expr
  
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& rval);
  override void		GenCssBody_impl(Program* prog);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif // MemberFmArg_h
