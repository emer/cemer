// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef Unit_Group_h
#define Unit_Group_h 1

// parent includes:
#include <taGroup>
#include <Unit>

// member includes:
#include <PosVector3i>

// declare all other types mentioned but not required to include:
class Layer; //

eTypeDef_Of(Unit_Group);

class E_API Unit_Group: public taGroup<Unit> {
  // #NO_UPDATE_AFTER ##CAT_Network ##SCOPE_Network ##NO_EXPAND_ALL ##UNDO_BARRIER ##NO_UNIQUE_NAMES a group of units -- manages structural but not computational functions
INHERITED(taGroup<Unit>)
public:
  Layer*        own_lay;        // #READ_ONLY #NO_SAVE #NO_SHOW #NO_SET_POINTER layer owner
  PosVector3i   pos;            // #CAT_Structure position of group relative to the layer -- for display purposes
  String        output_name;    // #GUI_READ_ONLY #SHOW #CAT_Counter #VIEW name for the output produced by the network (algorithm/program dependent, e.g., unit name of most active unit)
  int           idx;
  // #READ_ONLY #HIDDEN #NO_COPY #NO_SAVE #CAT_Structure index of this unit_group within containing subgroup list

  //////////////////////////////////////////////////////////////////////////
  //    Unit access API -- for internal use only -- use layer-level access of units instead!!

  Unit*         UnitAtCoord(const taVector2i& coord)
  { return UnitAtCoord(coord.x,coord.y); }
  // #EXPERT #CAT_Structure returns unit at given coordinates within unit group
  Unit*         UnitAtCoord(int x, int y);
  // #EXPERT #CAT_Structure get unit from given set of x and y coordinates within this group
  taVector2i     GpLogPos();
  // #EXPERT #CAT_Structure returns unit group *logical* position in terms of layer unit group geometry gp_geom -- computed from idx -- only for subgroups

  void          GetAbsPos(taVector3i& abs_pos) { abs_pos = pos; AddRelPos(abs_pos); }
  // #EXPERT #CAT_Structure get absolute pos, which factors in offsets from Unit_Groups, Layer, and Layer_Groups
  void          AddRelPos(taVector3i& rel_pos);
  // #IGNORE add relative pos, which factors in offsets from above
  void          GetAbsPos2d(taVector2i& abs_pos) { abs_pos = pos; AddRelPos2d(abs_pos); }
  // #EXPERT #CAT_Structure get absolute pos in 2d, which factors in offsets from Unit_Groups, Layer, and Layer_Groups
  void          AddRelPos2d(taVector2i& rel_pos);
  // #IGNORE add relative pos in 2d, which factors in offsets from above

  virtual void  Copy_Weights(const Unit_Group* src);
  // #MENU #MENU_ON_Object #CAT_ObjectMgmt copies weights from other unit group (incl wts assoc with unit bias member)

  virtual void  SaveWeights_strm(std::ostream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #EXT_wts #COMPRESS #CAT_File write weight values out in a simple ordered list of weights (optionally in binary fmt)
  virtual int   LoadWeights_strm(std::istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
                                 bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File read weight values in from a simple ordered list of weights (optionally in binary fmt) -- rval is taMisc::ReadTagStatus = END_TAG if successful
  static int    SkipWeights_strm(std::istream& strm, RecvCons::WtSaveFormat fmt = RecvCons::TEXT,
                                 bool quiet = false);
  // #EXT_wts #COMPRESS #CAT_File skip over weight values -- rval is taMisc::ReadTagStatus = END_TAG if successful

  virtual void  SaveWeights(const String& fname="", RecvCons::WtSaveFormat fmt = RecvCons::TEXT);
  // #MENU #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_SAVE write weight values out in a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)
  virtual int   LoadWeights(const String& fname="",
                            RecvCons::WtSaveFormat fmt = RecvCons::TEXT, bool quiet = false);
  // #MENU #EXT_wts #COMPRESS #CAT_File #FILE_DIALOG_LOAD read weight values in from a simple ordered list of weights (optionally in binary fmt) (leave fname empty to pull up file chooser)

  virtual void  MonitorVar(NetMonitor* net_mon, const String& variable);
  // #BUTTON #CAT_Statistic monitor (record in a datatable) the given variable on this unit group

  virtual void  TransformWeights(const SimpleMathSpec& trans);
  // #MENU #MENU_SEP_BEFORE #CAT_Learning apply given transformation to weights
  virtual void  AddNoiseToWeights(const Random& noise_spec);
  // #MENU #CAT_Learning add noise to weights using given noise specification
  virtual int   PruneCons(const SimpleMathSpec& pre_proc,
                             Relation::Relations rel, float cmp_val);
  // #MENU #USE_RVAL #CAT_Structure remove weights that (after pre-proc) meet relation to compare val
  virtual int   LesionCons(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure remove connections with prob p_lesion (permute = fixed no. lesioned)
  virtual int   LesionUnits(float p_lesion, bool permute=true);
  // #MENU #USE_RVAL #CAT_Structure turn on unit LESIONED flags with prob p_lesion (permute = fixed no. lesioned)
  virtual void  UnLesionUnits();
  // #MENU #USE_RVAL #CAT_Structure un-lesion units: turn off all unit LESIONED flags
  virtual void  UpdtAfterNetModIfNecc();
  // #IGNORE call network UpdtAfterNetMod only if it is not otherwise being called at a higher level

  virtual bool  UnitValuesToArray(float_Array& ary, const String& variable);
  // #CAT_Structure adds values of variable from the units into the given array
  virtual bool  UnitValuesToMatrix(float_Matrix& mat, const String& variable);
  // #CAT_Structure adds values of variable from the units into the given matrix
  virtual bool  UnitValuesFromArray(float_Array& ary, const String& variable);
  // #CAT_Structure sets unit values from values in the given array
  virtual bool  UnitValuesFromMatrix(float_Matrix& mat, const String& variable);
  // #CAT_Structure sets unit values from values in the given array

  virtual DataTable*    VarToTable(DataTable* dt, const String& variable);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Structure send given variable to data table -- number of columns depends on variable (for connection variables, specify r. or s. (e.g., r.wt)) -- this uses a NetMonitor internally, so see documentation there for more information
  virtual DataTable*    ConVarsToTable(DataTable* dt, const String& var1, const String& var2 = "",
               const String& var3 = "", const String& var4 = "", const String& var5 = "",
               const String& var6 = "", const String& var7 = "", const String& var8 = "",
               const String& var9 = "", const String& var10 = "", const String& var11 = "",
               const String& var12 = "", const String& var13 = "", const String& var14 = "",
               Projection* prjn=NULL);
  // #MENU #NULL_OK_0 #NULL_TEXT_0_NewTable #CAT_Statistics record given connection-level variable to data table with column names the same as the variable names, and one row per *connection* (unlike monitor-based operations which create matrix columns) -- this is useful for performing analyses on learning rules as a function of sending and receiving unit variables -- uses receiver-based connection traversal -- connection variables are just specified directly by name -- corresponding receiver unit variables are "r.var" and sending unit variables are "s.var" -- prjn restricts to that prjn

  virtual bool  VarToVarCopy(const String& dest_var, const String& src_var);
  // #CAT_Structure copy one unit variable to another (un->dest_var = un->src_var) for all units within this unit group (must be a float type variable)
  virtual bool  VarToVal(const String& dest_var, float val);
  // #CAT_Structure set variable to given value for all units within this unit group (must be a float type variable)

  virtual Unit* MostActiveUnit(int& idx);
  // #CAT_Activation Return the unit with the highest activation (act) value -- index of unit is returned in idx

  // implement save_rmv_units:
  bool Dump_QuerySaveChildren() CPP11_OVERRIDE;
  taObjDiffRec* GetObjDiffVal(taObjDiff_List& odl, int nest_lev,
                                       MemberDef* memb_def=NULL, const void* par=NULL,
                                       TypeDef* par_typ=NULL, taObjDiffRec* par_od=NULL) const CPP11_OVERRIDE;

  // we maintain our index in any owning list
  int  GetIndex() const CPP11_OVERRIDE { return idx; }
  void SetIndex(int i) CPP11_OVERRIDE { idx = i; }

  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "Unit"; }

  void  InitLinks();
  void  CutLinks();
  void  Copy_(const Unit_Group& cp);
  TA_BASEFUNS(Unit_Group);
protected:
  void UpdateAfterEdit_impl() CPP11_OVERRIDE;
private:
  void  Initialize();
  void  Destroy()               { CutLinks(); }
};

#endif // Unit_Group_h
