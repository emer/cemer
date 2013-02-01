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

#ifndef EditMbrItem_Group_h
#define EditMbrItem_Group_h 1

// parent includes:
#include <EditMbrItem>
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:
class taBase; // 
class TypeDef; // 
class MemberDef; // 
class DataTable; // 


TypeDef_Of(EditMbrItem_Group);

class TA_API EditMbrItem_Group : public taGroup<EditMbrItem> {
  // ##CAT_Display group of select edit dialog objects
INHERITED(taGroup<EditMbrItem>)
public:
  taBase*               GetBase_Flat(int idx) const;
    // gets the flat (leaf) base -- NULL if out of bounds or doesn't exist
  EditMbrItem*          FindItemBase(taBase* base,
    MemberDef* md, int& idx) const
    {return (EditMbrItem*)SelectEditItem::StatFindItemBase(this, base, md, idx);}

  override int          NumListCols() const {return 5;}
  // base name, base type, memb name, memb type, memb label
  override String       GetColHeading(const KeyString& key) const;
  // header text for the indicated column
  override const KeyString GetListColKey(int col) const;
  override String       GetTypeDecoKey() const { return "SelectEdit"; }
  override void         SigEmit(int sls, void* op1 = NULL, void* op2 = NULL);
  TA_BASEFUNS_NOCOPY(EditMbrItem_Group);
#ifdef __MAKETA__ // dummy to supress New
  taBase*       New_gui(int n_objs=1, TypeDef* typ = NULL); // #NO_SHOW
#endif

  //////////////////////////////////////////////////
  //    Parameter Searching Interface

  virtual EditMbrItem*  FindMbrName(const String& mbr_nm, const String& label = "");
  // #CAT_Access find an item based on member name and, optionally if non-empty, the associated label
  virtual EditMbrItem*  PSearchFind(const String& mbr_nm, const String& label = "");
  // #CAT_ParamSearch find a param_search item based on member name and, optionally if non-empty, the associated label -- must be is_numeric -- issues error if not found -- used for other psearch functions
  virtual EditMbrItem*  PSearchNext(int& st_idx);
  // #CAT_ParamSearch get the next active param search item starting from the given start index -- null if this was the last one -- must have param_search.search = true -- increments st_idx to next item to search so it can be called continuously

  virtual bool          PSearchMinToCur_All();
  // #CAT_ParamSearch set current value to stored minimum value for all items in active parameter search -- call at start of searching
  virtual bool          PSearchNextIncr_Grid();
  // #CAT_ParamSearch increment the next_val for next param search item using a simple grid search algorithm -- first item is searched as an inner-loop, followed by next item, etc -- returns false when last item has been incremented to its max value (time to stop)
  virtual bool          PSearchNextToCur_All();
  // #CAT_ParamSearch set current value to stored next value for all items in active parameter search

  virtual void          PSearchConfigTable(DataTable* dat, bool all_nums=false,
                                           bool add_eval=true);
  // #CAT_ParamSearch configure data table to hold results of the parameter search -- see PSearchRecord function -- makes a column for each active search variable (or all numeric values if specified), plus an optional "eval" column to hold the overal evaluation value for this set of parameters
  virtual void          PSearchRecordData(DataTable* dat, bool all_nums=false,
                                          bool add_eval=true, double eval_val=0.0);
  // #CAT_ParamSearch record current search parameters (or all numeric values if specified) in data table (configured with PSearchConfigTable), along with the evaluation value for this set of parameters as provided -- does not add a new row to table -- can be used in conjunction with other net monitor data etc

  virtual bool&         PSearchOn(const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access gets a reference to the param_search flag for given member name and, optionally if non-empty, the associated label -- indicates whether to include item in overall search process
  virtual bool          PSearchOn_Set(bool psearch, const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access set the param_search flag for given member name and, optionally if non-empty, the associated label -- indicates whether to include item in overall search process
  virtual double&       PSearchMinVal(const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access gets a reference to the param search min_val for given member name and, optionally if non-empty, the associated label
  virtual bool          PSearchMinVal_Set(double min_val, const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access set param search min_val for given member name and, optionally if non-empty, the associated label
  virtual double&       PSearchMaxVal(const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access gets a reference to the param search max_val for given member name and, optionally if non-empty, the associated label
  virtual bool          PSearchMaxVal_Set(double max_val, const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access set param search max_val for given member name and, optionally if non-empty, the associated label
  virtual double&       PSearchNextVal(const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access gets a reference to the param search next_val for given member name and, optionally if non-empty, the associated label
  virtual bool          PSearchNextVal_Set(double next_val, const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access set param search next_val for given member name and, optionally if non-empty, the associated label
  virtual double&       PSearchIncrVal(const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access gets a reference to the param search incr_val for given member name and, optionally if non-empty, the associated label
  virtual bool          PSearchIncrVal_Set(double incr_val, const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access set param search incr_val for given member name and, optionally if non-empty, the associated label
  virtual Variant       PSearchCurVal(const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access gets current value for given member name and, optionally if non-empty, the associated label
  virtual bool          PSearchCurVal_Set(const Variant& cur_val, const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access set current value for given member name and, optionally if non-empty, the associated label
  virtual bool          PSearchNextToCur(const String& mbr_nm, const String& label = "");
  // #CAT_PSearch_Access set current value to stored next value for given member name and, optionally if non-empty, the associated label

private:
  void  Initialize() { SetBaseType(&TA_EditMbrItem);}
  void  Destroy()               { };
};

#endif // EditMbrItem_Group_h
