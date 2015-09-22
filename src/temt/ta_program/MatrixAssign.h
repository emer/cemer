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

#ifndef MatrixAssign_h
#define MatrixAssign_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgVar>
#include <ProgExpr>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(MatrixAssign);

class TA_API MatrixAssign : public ProgEl {
  // assign an expression to a matrix or data table column, using matrix indexing notation to access the matrix cell(s) to be assigned
INHERITED(ProgEl)
public:
  ProgVarRef      variable;
  // #ITEM_FILTER_StdProgVarFilter #CUST_CHOOSER_NewProgVarCustChooser variable for the data table or matrix to assign to
  bool            data_table;
  // #READ_ONLY true if the variable points to a data table, in which case there is a separate expression for the column(s) of the data table to operate on
  ProgExprShort    col;
  // #CONDSHOW_ON_data_table #BROWSER_EDIT_LOOKUP expression for the data table column(s) to assign to -- can be a number, a name surrounded in double-quotes, or a range (using start:stop:incr range notation)
  ProgExprShort    dim0;
  // #BROWSER_EDIT_LOOKUP expression for the first dimension -- specifies an index or range of indexes (using start:stop:incr range notation)
  ProgExprShort    dim1;
  // #BROWSER_EDIT_LOOKUP expression for the second dimension -- specifies an index or range of indexes (using start:stop:incr range notation)
  ProgExprShort    dim2;
  // #BROWSER_EDIT_LOOKUP expression for the third dimension -- specifies an index or range of indexes (using start:stop:incr range notation)
  ProgExprShort    dim3;
  // #BROWSER_EDIT_LOOKUP expression for the fourth dimension -- specifies an index or range of indexes (using start:stop:incr range notation)
  ProgExprShort    dim4;
  // #BROWSER_EDIT_LOOKUP expression for the fifth dimension -- specifies an index or range of indexes (using start:stop:incr range notation)
  ProgExprShort    dim5;
  // #BROWSER_EDIT_LOOKUP expression for the sixth dimension -- specifies an index or range of indexes (using start:stop:incr range notation)
 
  ProgExpr        expr;
  // #BROWSER_EDIT_LOOKUP expression to assign to given matrix cell(s)
  
  bool            CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool            CvtFmCode(const String& code) override;
  bool            ChooseMe() override;
  // #IGNORE pop chooser for selecting object of this type

  virtual String  GetIndexExpr() const; // get full []... index expressions
  
  String          GetDisplayName() const override;
  String          GetTypeDecoKey() const override { return "ProgVar"; }
  String          GetToolbarName() const override { return "matrix="; }

  PROGEL_SIMPLE_BASEFUNS(MatrixAssign);
protected:
  void            CheckThisConfig_impl(bool quiet, bool& rval) override;
  void            GenCssBody_impl(Program* prog) override;

private:
  void Initialize();
  void Destroy()     { };
};

#endif // MatrixAssign_h
