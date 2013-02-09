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

#ifndef ColCalcExpr_h
#define ColCalcExpr_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:
class DataCol; // 
class DataTableCols; // 

// note: the ColCalcExpr could be augmented to update with column name changes
// as in ProgExpr
// but it has a catch-22 with requiring a reference to data columns which have
// yet to be defined because they contain this object..

taTypeDef_Of(ColCalcExpr);

class TA_API ColCalcExpr: public taOBase {
  // ##NO_TOKENS ##INSTANCE ##EDIT_INLINE ##CAT_Data a column calculation expression
INHERITED(taOBase)
public:

  String        expr;           // #EDIT_DIALOG enter the expression here -- column value will be set to this.\nyou can just type in names of other columns (value is corresponding row's value) or literal values, or math expressions, etc.\nenclose strings in double quotes.\ncolumn names will be checked and automatically updated
  DataCol* col_lookup;  // #NULL_OK #NO_EDIT #NO_SAVE #FROM_GROUP_data_cols #NO_UPDATE_POINTER lookup a program variable and add it to the current expression (this field then returns to empty/NULL)

  DataTableCols*        data_cols;
  // #READ_ONLY #HIDDEN #NO_SAVE data table columns (set from owner field)

  virtual bool  SetExpr(const String& ex);
  // set to use given expression -- use this interface for future compatibility

//   virtual bool       ParseExpr();
//   // parse the current expr for variables and update vars and var_expr accordingly (returns false if there are some bad_vars)
  virtual String GetFullExpr() const;
  // get full expression with variable names substituted appropriately -- use this interface instead of referring to raw expr, for future compatibility

  String        GetName() const;

  void  InitLinks();
  void  CutLinks();
  void  Copy_(const ColCalcExpr& cp);
  TA_BASEFUNS(ColCalcExpr);
protected:

  override void         UpdateAfterEdit_impl();
//   override void      CheckThisConfig_impl(bool quiet, bool& rval);
//   override void              SmartRef_SigDestroying(taSmartRef* ref, taBase* obj);
//   override void              SmartRef_SigEmit(taSmartRef* ref, taBase* obj,
//                                           int sls, void* op1_, void* op2_);

private:
  void  Initialize();
  void  Destroy();
};

#endif // ColCalcExpr_h
