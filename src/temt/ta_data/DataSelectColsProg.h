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

#ifndef DataSelectColsProg_h
#define DataSelectColsProg_h 1

// parent includes:
#include <DataSrcDestProg>

// member includes:
#include <DataOpList>

// declare all other types mentioned but not required to include:
class DataOpEl; // 


taTypeDef_Of(DataSelectColsProg);

class TA_API DataSelectColsProg : public DataSrcDestProg { 
  // ##DEF_CHILD_select_spec ##DEF_CHILDNAME_Select_Spec selects rows from src_data into dest_data according to select_spec
INHERITED(DataSrcDestProg)
public:
  DataOpList            select_spec; // #TREE_SHOW columns to select

  virtual DataOpEl*     AddColumn(const String& col_name) { return select_spec.AddColumn(col_name, GetSrcData()); }
  virtual void          AddAllColumns();
  // #BUTTON #CAT_Data add all columns from src_data to the select_spec list of ops columns 
  void                  UpdateSpecDataTable() override;

  taList_impl*          children_() override {return &select_spec; }
  Variant               Elem(const Variant& idx, IndexMode mode = IDX_UNK) const override
                            { return select_spec.Elem(idx, mode); }
  String                GetDisplayName() const override;
  String                GetToolbarName() const override { return "select\ncolumns"; }
  bool                  CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool                  CvtFmCode(const String& code) override;
  
  String                GetArgForCompletion(const String& method, const String& arg) override;
  void                  GetArgCompletionList(const String& method, const String& arg, const String_Array& arg_values, taBase* arg_obj, const String& cur_txt, Completions& list) override;
  
  TA_SIMPLE_BASEFUNS(DataSelectColsProg);
protected:
  void      UpdateAfterEdit_impl() override;
  void      CheckChildConfig_impl(bool quiet, bool& rval) override;
  bool      GenCssBody_impl(Program* prog) override;

private:
  void  Initialize();
  void  Destroy()       { CutLinks(); }
};

#endif // DataSelectColsProg_h
