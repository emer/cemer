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

#ifndef DataSelectEl_h
#define DataSelectEl_h 1

// parent includes:
#include <DataOpEl>

// member includes:
#include <ProgVar>

// declare all other types mentioned but not required to include:


TypeDef_Of(DataSelectEl);

class TA_API DataSelectEl : public DataOpEl {
  // one element of a data selection specification -- for matrix cells with cell size > 1, the cmp value is treated as a cell index and if the value at this index is 1 (true) then result is true for EQUAL and not true for NOTEQUAL
  INHERITED(DataOpEl)
public:
  enum Relations {
    EQUAL,		// #LABEL_=
    NOTEQUAL,		// #LABEL_!=
    LESSTHAN,		// #LABEL_<
    GREATERTHAN,	// #LABEL_>
    LESSTHANOREQUAL,	// #LABEL_<=
    GREATERTHANOREQUAL, // #LABEL_>=
    CONTAINS,		// for strings: contains this value
    NOT_CONTAINS,	// for strings: doesn't contain this value
  };

  bool		on;		// use this selection criterion?  can be useful to have various selections available but not enabled as needs change.  see also enable_var to dynamically determine use of selection crtiteria based on a variable.
  Relations	rel;		// #CONDEDIT_ON_on relation of column to expression for selection
  bool		use_var;	// #CONDEDIT_ON_on if true, use a program variable to specify the selection value
  Variant	cmp;		// #CONDEDIT_ON_use_var:false&&on literal compare value of column to this comparison value
  ProgVarRef	var;		// #CONDEDIT_ON_use_var&&on variable that contains the comparison value: note -- this MUST be a global var in vars or args, not in local vars!
  ProgVarRef	enable_var;	// #CONDEDIT_ON_on optional variable that is evaluated as either true or false *at the start of the select procedure* to determine if this select criterion is enabled -- can setup a large set of criteria and flexibly enable them as appropriate. note -- this MUST be a global var in vars or args, not in local vars!
  bool		act_enabled;	// #READ_ONLY #NO_SAVE actual enabled value to use -- reflects on && enable_var

  bool	Eval(const Variant& val); // evaluate expression

  virtual void 	UpdateEnabled(); // update the act_enabled flag based on flag and variable

  override String GetDisplayName() const;
  void  Initialize();
  void 	Destroy()		{ };
  TA_SIMPLE_BASEFUNS(DataSelectEl);
protected:
  override void	 CheckThisConfig_impl(bool quiet, bool& rval);
};

#endif // DataSelectEl_h
