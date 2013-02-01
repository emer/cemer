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

#ifndef taiEditorWidgetsOfProgramCtrl_h
#define taiEditorWidgetsOfProgramCtrl_h 1

// parent includes:
#include <taiEditorWidgetsOfClass>
#include <IRefListClient>

// member includes:
#include <taBase_RefList>

// declare all other types mentioned but not required to include:
class Program;

TypeDef_Of(taiEditorWidgetsOfProgramCtrl);

class TA_API taiEditorWidgetsOfProgramCtrl : public taiEditorWidgetsOfClass, public virtual IRefListClient {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit only selected items from a range of ta-base objects
INHERITED(taiEditorWidgetsOfClass)
  Q_OBJECT
public: //
// We use sets: 0:name/desc; 1: gp guys (ex step); set2: args; set3: vars
  enum CtrlMembSets {
    MS_PROG,
//     MS_GP,
    MS_ARGS,
    MS_VARS, // note: must ARGS..VARS must be in numerical sequence

    MS_CNT      = 4,
  };

  inline Program*       prog() const {return (Program*)root;}


  taiEditorWidgetsOfProgramCtrl(Program* base, bool read_only_ = false,
                       bool modal_ = false, QObject* parent = 0);
  taiEditorWidgetsOfProgramCtrl()                { };
  ~taiEditorWidgetsOfProgramCtrl();

  override bool ShowMember(MemberDef* md) const; //

protected: //
  // we maintain several lists to simply mgt and notify handling
  taBase_RefList        refs; // the data members from the Program
  taBase_RefList        refs_struct; // structural guys: arg and var lists themselves, gp
  override void         Enum_Members();
  override void         Constr_Data_Labels();
  override void         Cancel_impl();
  override MemberDef*   GetMemberPropsForSelect(int sel_idx, taBase** base,
    String& lbl, String& desc); // (use sel_item_idx) enables things like ProgCtrl to play
  override void         GetValue_Membs_def();
  override void         GetImage_Membs();

public: // IRefListClient i/f
  TYPED_OBJECT(taiEditorWidgetsOfProgramCtrl);
  override void         SigDestroying_Ref(taBase_RefList* src, taBase* ta);
  override void         SigEmit_Ref(taBase_RefList* src, taBase* ta,
    int sls, void* op1, void* op2);
};


#endif // taiEditorWidgetsOfProgramCtrl_h
