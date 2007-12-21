// Copyright, 1995-2007, Regents of the University of Colorado,
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

// ta_qttype.cc

#include "ta_qt.h"
#include "ta_qtdata.h"
#include "ta_qtdialog.h"
#include "ta_qtviewer.h"
#include "ta_qttype.h"
#include "ta_qtgroup.h"
#include "ta_defaults.h"
#include "ta_project.h"
#include "css_qt.h"
#include "css_basic_types.h"
#include "css_ta.h"
#include "ta_TA_type.h"
//TODO: determine why classes from igeometry.h end up in ta_misc TA file, not ta file --
// igeometry.h is included in ta_qt.h
//#  include "ta_misc_TA_type.h"



/* NUKE
#include <ta/enter_iv.h>
#include <InterViews/font.h>
#include <IV-look/kit.h>
#include <IV-look/dialogs.h>
#include <InterViews/layout.h>
#include <InterViews/style.h>
#include <InterViews/window.h>
#include <ta/leave_iv.h>
*/


//////////////////////////
//    taiTypeBase	//
//////////////////////////

const iColor taiTypeBase::def_color; 

taiTypeBase::taiTypeBase() {
  typ = NULL;
  init();
}

taiTypeBase::taiTypeBase(TypeDef* typ_) {
  typ = typ_;
  init();
}

void taiTypeBase::init() {
  bid = 0;
  m_sub_types = NULL;
  no_setpointer = false;
}

taiTypeBase::~taiTypeBase() {
  if (m_sub_types != NULL) {
    delete m_sub_types;
    m_sub_types = NULL;
  }
}


//////////////////////////
// 	taiType	//
//////////////////////////

bool taiType::CheckProcessCondMembMeth(const String condkey,
    TypeItem* memb_meth, const void* base, bool& is_on, bool& val_is_eq) 
{
  // format: [CONDEDIT|GHOST]_[ON|OFF]_member[:value{,value}]
  String optedit = memb_meth->OptionAfter(condkey + "_");
  if (optedit.empty()) return false;
  
  String onoff = optedit.before('_');
  is_on = (onoff == "ON"); //NOTE: should probably verify if OFF, otherwise it is an error

  optedit = optedit.after('_');
  String val = optedit.after(':');
  
  // find the member name -- depends on mode, see below
  String mbr;
  if (val.empty())
    mbr = optedit; // entire thing is the member, implied boolean
  else
    mbr = optedit.before(':');
  
  TAPtr tab = (TAPtr)base;
  MemberDef* md = NULL;
  void* mbr_base = NULL;	// base for conditionalizing member itself
  void* mbr_par_base = (void*)base;	// base for parent of member
  if (mbr.contains('.')) {
    String par_path = mbr.before('.', -1);
    MemberDef* par_md = NULL;
    TAPtr par_par = (TAPtr)tab->FindFromPath(par_path, par_md);
    if ((par_par == NULL) || !(par_md->type->InheritsFrom(&TA_taBase))) {
      taMisc::Error("CONDEDIT: can't find parent of member:", par_path);
      return false;
    }
    String subpth = mbr.after('.', -1);
    md = par_par->FindMembeR(subpth, mbr_base);
    mbr_par_base = (void*)par_par;
  } else {
    md = tab->FindMembeR(mbr, mbr_base);
  }
  if ((md == NULL) || (mbr_base == NULL)) {
    // this can happen in valid cases (selectedit), and the msg is annoying
    //    taMisc::Warning("taiType::CheckProcessCondMembMeth: conditionalizing member", mbr, "not found!");
    return false;
  }
  
  if (val.empty()) {
    // implied boolean member mode (note: legacy for GHOST, new for CONDEDIT
    // for GHOST, it is new to support dotted submembers
    // just get it as a Variant and interpret as boolean
    Variant mbr_val(md->type->GetValVar(mbr_base, md));
    val_is_eq = mbr_val.toBool();
  } else {
    // explicit value mode (note: legacy for CONDEDIT, new for GHOST

    String mbr_val = md->type->GetValStr(mbr_base, mbr_par_base, md);
    val_is_eq = false;
    while (true) {
      String nxtval;
      if (val.contains(',')) {
        nxtval = val.after(',');
        val = val.before(',');
      }
      if(md->type->InheritsFormal(TA_enum) && mbr_val.contains('|')) { // bits!
	if(mbr_val.contains(val)) {
	  String aft = mbr_val.after(val);
	  String bef = mbr_val.before(val);
	  if((aft.empty() || aft.firstchar() == '|') && (bef.empty() || bef.lastchar() == '|')) {
	    // make sure it is not just a subset of something else..
	    val_is_eq = true;
	    break;
	  }
	}
      }
      else {
	if (val == mbr_val) {
	  val_is_eq = true;
	  break;
	}
      }
      if (!nxtval.empty())
        val = nxtval;
      else
        break;
    }
  }
  return true;
}

void taiType::Initialize() {
}

void taiType::Destroy() {
}

// add "this" type to the td->it slot, or link in lower slot as per the bid

void taiType::AddToType(TypeDef* td) {
  taiType* cur_it = td->it;		// the current it
  taiType** ptr_to_it = &(td->it);	// were to put one
  while(cur_it != NULL) {
    if(bid < cur_it->bid) {		// we are lower than current
      ptr_to_it = cur_it->addr_sub_types();	// put us on its sub_types
      cur_it = cur_it->sub_types();	// and compare to current sub_type
    }
    else {
      m_sub_types = cur_it;		// we are higher, current is our sub
      cur_it = NULL;			// and we are done
    }
  }
  *ptr_to_it = this;			// put us here
}

taiData* taiType::GetDataRep(IDataHost* host_, taiData* par, QWidget* gui_parent_,
	taiType* parent_type_, int flags)
{
  //note: user can pass in flgReadOnly to force readonly, but we can also force it
  bool ro = isReadOnly(par, host_);
  // must also take account of whether parent_type is read only
  if (parent_type_ != NULL)
    ro = ro || parent_type_->isReadOnly(par);
  if (ro)
    flags |= taiData::flgReadOnly;
  if (requiresInline())
    flags |= taiData::flgInline;
  if ((flags & taiData::flgReadOnly) && !handlesReadOnly()) {
    return taiType::GetDataRep_impl(host_, par, gui_parent_, flags);
  } else if ((flags & taiData::flgInline) && allowsInline()) {
    return GetDataRepInline_impl(host_, par, gui_parent_, flags);
  } else {
    return GetDataRep_impl(host_, par, gui_parent_, flags);
  }
}

taiData* taiType::GetDataRep_impl(IDataHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_) 
{
  // taiField: your friend when all else fails...
  taiField* rval = new taiField(typ, host_, par, gui_parent_, flags_);
  return rval;
}

taiData* taiType::GetDataRepInline_impl(IDataHost* host_, taiData* par,
  QWidget* gui_parent, int flags_) 
{
  // base type doesn't know what to do for inline, so just returns the basic guy
  return GetDataRep_impl(host_, par, gui_parent, flags_);
}

void taiType::GetImage(taiData* dat, const void* base) {
//  bool ro = isReadOnly(dat);
  // use the exact criteria we used in the GetRep
  bool ro = dat->HasFlag(taiData::flgReadOnly);
  if (ro && !handlesReadOnly()) {
    taiType::GetImage_impl(dat, base);
  } else {
    GetImage_impl(dat, base);
  }
}

void taiType::GetImage_impl(taiData* dat, const void* base) {
  //TODO: shouldn't this strval follow same logic as the one in GetDataRep
  String strval = typ->GetValStr(base);
  taiField* rval = (taiField*)dat;
  rval->GetImage(strval);
}

void taiType::GetValue(taiData* dat, void* base) {
//  bool ro = isReadOnly(dat);
  // use the exact criteria we used in the GetRep
  bool ro = dat->HasFlag(taiData::flgReadOnly);
  if (!ro || handlesReadOnly()) {
    GetValue_impl(dat, base);
  }
  //note: we don't do anything if ro and type doesn't handle it!
}

void taiType::GetValue_impl(taiData* dat, void* base) {
  taiField* rval = (taiField*)dat;
  String strval(rval->GetValue());
  typ->SetValStr(strval, base);
}

bool taiType::isReadOnly(taiData* dat, IDataHost* host_) { // used in GetImage and GetValue
  // ReadOnly if host_ is RO, OR par is RO, OR directives state RO
  bool rval = false;
  if (dat != NULL) {
    rval = rval || dat->readOnly();
    if (host_ == NULL)
      host_ = dat->host;
  }
  if (host_ != NULL) {
    rval = rval || host_->isReadOnly();
  }
  rval = rval || typ->HasOption("READ_ONLY");
  return rval;
}



//////////////////////////
//  taiIntType		//
//////////////////////////

int taiIntType::BidForType(TypeDef* td){
//NOTE: we can't properly handle uints, so we don't bid for them
// we left the handler code in the other routines, in case we implement them
  // we handle all numeric int types < 32 bits but NOT uint/ulong
  if (td->DerivesFrom(&TA_int) 
    || td->DerivesFrom(&TA_short) || td->DerivesFrom(&TA_unsigned_short)
    || td->DerivesFrom(&TA_signed_char) || td->DerivesFrom(&TA_unsigned_char)
  )
    return (taiType::BidForType(td) +1);
  return 0;
}

taiData* taiIntType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
//TODO: the taiIncrField control can only handle int values, so can't handle uint range
// should either replace with a DoubleSpin, or longlongspin
  taiIncrField* rval = new taiIncrField(typ, host_, par, gui_parent_, flags_);
  // put limits on values -- start w/ explicit ones, them limit them by datatype
  int min = INT_MIN;
  if (typ->HasOption("POS_ONLY")) // do this one first, then max of min
    min = 0;
  String val = typ->OptionAfter("MIN_");
  int it;
  // we are very conservative, and make sure the value can be converted
  if (val.nonempty() && val.isInt()) {
    it = val.toInt();
    min = MAX(min, it);
  }
  int max = INT_MAX;
  val = typ->OptionAfter("MAX_");
  if (val.nonempty() && val.isInt()) {
    it = val.toInt();
    max = MIN(max, it);
  }
  if (typ->DerivesFrom(&TA_int)) {
    min = MAX(min, INT_MIN);  max = MIN(max, INT_MAX);
//  } else if (typ->DerivesFrom(&TA_unsigned_int)) {
//    min = 0;  max = INT_MAX;//NOTE: does not cover entire uint range
  } else if (typ->DerivesFrom(&TA_short)) {
    min = MAX(min, SHRT_MIN);  max = MIN(max, SHRT_MAX);
  } else if (typ->DerivesFrom(&TA_unsigned_short)) {
    min = MAX(min, 0);  max = MIN(max, USHRT_MAX);
  } else if (typ->DerivesFrom(&TA_signed_char)) {
    min = MAX(min, SCHAR_MIN);  max = MIN(max, SCHAR_MAX);
  } else { //if typ->DerivesFrom(&TA_unsigned_char)
    min = MAX(min, 0);  max = MIN(max, UCHAR_MAX);
  }
  rval->setMinimum(min);
  rval->setMaximum(max);
  return rval;
}

void taiIntType::GetImage_impl(taiData* dat, const void* base) {
  int val = 0;
  if (typ->DerivesFrom(&TA_int)) {
    val = *((int*)base);
  } else if (typ->DerivesFrom(&TA_unsigned_int)) {
    val = (int)*((uint*)base); //NOTE: overflow issue
  } else if (typ->DerivesFrom(&TA_short)) {
    val = (int)*((short*)base); 
  } else if (typ->DerivesFrom(&TA_unsigned_short)) {
    val = (int)*((unsigned short*)base); 
  } else if (typ->DerivesFrom(&TA_signed_char)) {
    val = (int)*((signed char*)base); 
  } else if (typ->DerivesFrom(&TA_unsigned_char)) {
    val = (int)*((unsigned char*)base); 
  } //note: should never not be one of these
  taiIncrField* rval = (taiIncrField*)dat;
  rval->GetImage(val);
}

void taiIntType::GetValue_impl(taiData* dat, void* base) {
  taiIncrField* rval = (taiIncrField*)dat;
  int val = rval->GetValue();
  if (typ->DerivesFrom(&TA_int)) {
    *((int*)base) = val;
  } else if (typ->DerivesFrom(&TA_unsigned_int)) {
   *((uint*)base) = (uint)val; //NOTE: range issue
  } else if (typ->DerivesFrom(&TA_short)) {
    *((short*)base) = (short)val; 
  } else if (typ->DerivesFrom(&TA_unsigned_short)) {
    *((unsigned short*)base) = (unsigned short)val; 
  } else if (typ->DerivesFrom(&TA_signed_char)) {
    *((signed char*)base) = (signed char)val; 
  } else if (typ->DerivesFrom(&TA_unsigned_char)) {
    *((unsigned char*)base) = (unsigned char)val; 
  } //note: should never not be one of these
}


//////////////////////////
//  taiInt64Type	//
//////////////////////////

int taiInt64Type::BidForType(TypeDef* td){
  // we handle all 64-bit types
  if (td->DerivesFrom(&TA_int64_t) || td->DerivesFrom(&TA_uint64_t))
    return (taiType::BidForType(td) +1);
  return 0;
}

//TODO: we really are still just using the taiType defaults
// need to create a 64-bit spin, or at least a customized edit


////////////////////////
//  taiEnumType     //
////////////////////////

void taiEnumType::Initialize() {
}

int taiEnumType::BidForType(TypeDef* td){
  if (td->InheritsFormal(TA_enum))
    return (taiType::BidForType(td) +1);
  return 0;
}

taiData* taiEnumType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  isBit = ((typ != NULL) && (typ->HasOption("BITS")));
  if (isBit) {
    return new taiBitBox(true, typ, host_, par, gui_parent_, flags_);
  } else if (flags_ & taiData::flgReadOnly) {
    return new taiField(typ, host_, par, gui_parent_, flags_);
  } else {
    taiComboBox* rval = new taiComboBox(true, typ,host_, par, gui_parent_, flags_);
    return rval;
  }
}

void taiEnumType::GetImage_impl(taiData* dat, const void* base) {
  if (isBit) {
    taiBitBox* rval = (taiBitBox*)dat;
    rval->GetImage(*((int*)base));
  } else if (isReadOnly(dat)) {
    taiField* rval = (taiField*)(dat);
    String str;
    EnumDef* ed = typ->enum_vals.FindNo(*((int*)base));
    if (ed != NULL) {
      str = ed->GetLabel();
    } else {
      str = String(*((int*)base));
    }
    rval->GetImage(str);
  } else {
    taiComboBox* rval = (taiComboBox*)dat;
    EnumDef* td = typ->enum_vals.FindNo(*((int*)base));
    if (td != NULL)
      rval->GetImage(td->idx);
  }
}

void taiEnumType::GetValue_impl(taiData* dat, void* base) {
  if (isBit) {
    taiBitBox* rval = (taiBitBox*)dat;
    rval->GetValue(*((int*)base));
  } else if (!isReadOnly(dat)) {
    taiComboBox* rval = (taiComboBox*)dat;
    int itm_no = -1;
    rval->GetValue(itm_no);
    EnumDef* td = NULL;
    if ((itm_no >= 0) && (itm_no < typ->enum_vals.size))
      td = typ->enum_vals.FastEl(itm_no);
    if (td != NULL)
      *((int*)base) = td->enum_no;
  }
}


////////////////////////
//  taiBoolType     //
////////////////////////

int taiBoolType::BidForType(TypeDef* td){
  if(td->InheritsFrom(TA_bool))
    return (taiType::BidForType(td) +1);
  return 0;
}

taiData* taiBoolType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_){
  taiToggle* rval = new taiToggle(typ, host_, par, gui_parent_, flags_);
  return rval;
}

void taiBoolType::GetImage_impl(taiData* dat, const void* base) {
  bool val = *((bool*)base);
  taiToggle* rval = (taiToggle*)dat;
  rval->GetImage(val);
}

void taiBoolType::GetValue_impl(taiData* dat, void* base) {
  taiToggle* rval = (taiToggle*)dat;
  *((bool*)base) = rval->GetValue();
}


//////////////////////////
//  taiStringType	//
//////////////////////////

int taiStringType::BidForType(TypeDef* td){
  if (td->InheritsFrom(TA_taString))
    return(taiType::BidForType(td) + 2); // outbid class
  return 0;
}

taiData* taiStringType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  taiField* rval = new taiField(typ, host_, par, gui_parent_, flags_);
  return rval;
}

void taiStringType::GetImage_impl(taiData* dat, const void* base) {
  dat->GetImage_(base);
}

void taiStringType::GetValue_impl(taiData* dat, void* base) {
  dat->GetValue_(base); //noop for taiEditButton
}



////////////////////////
//  taiVariantType   //
////////////////////////

int taiVariantType::BidForType(TypeDef* td){
  if(td->InheritsFrom(TA_Variant))
    return(taiType::BidForType(td) + 2); // outbid taiClass type
  return 0;
}

taiData* taiVariantType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  taiVariant* rval = new taiVariant(host_, par, gui_parent_, flags_);
  return rval;
}

void taiVariantType::GetImage_impl(taiData* dat, const void* base) {
  if (!base) return; // error
  taiVariant* rval = (taiVariant*)dat;
  rval->GetImage(*(Variant*)base);
}

void taiVariantType::GetValue_impl(taiData* dat, void* base) {
  if (!base) return; // error
  taiVariant* rval = (taiVariant*)dat;
  
  rval->GetValue(*(Variant*)base);
}


//////////////////////////
//  taiClassType	//
//////////////////////////

int taiClassType::BidForType(TypeDef* td) {
//temp
if (td->name == "taSmartRef") {
  int i = 0;
  ++i;
}
  if(td->InheritsFormal(TA_class)) //iCoord handled by built-in type system
    return (taiType::BidForType(td) +1);
  return 0;
}

taiData* taiClassType::GetDataRep(IDataHost* host_, taiData* par, QWidget* gui_parent_,
  taiType* parent_type_, int flags_) 
{
  if (typ->HasOption("INLINE") || typ->HasOption("EDIT_INLINE"))
    flags_ |= taiData::flgInline;
  return inherited::GetDataRep(host_, par, gui_parent_, parent_type_, flags_); 
}

taiData* taiClassType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  taiEditButton* rval = taiEditButton::New(NULL, NULL, typ, host_, par, gui_parent_, flags_);
  return rval;
}

taiData* taiClassType::GetDataRepInline_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  taiPolyData* rval = taiPolyData::New(true, typ, host_, par, gui_parent_, flags_);
  return rval;
}


void taiClassType::GetImage_impl(taiData* dat, const void* base) {
  dat->GetImage_(base);
}

void taiClassType::GetValue_impl(taiData* dat, void* base) {
  dat->GetValue_(base); //noop for taiEditButton
}

bool taiClassType::CanBrowse() {
  //TODO: add additionally supported base types
  return (typ->InheritsFrom(TA_taBase)  && !typ->HasOption("HIDDEN"));
}


//////////////////////////////////
//  taiMatrixGeomType		//
//////////////////////////////////

int taiMatrixGeomType::BidForType(TypeDef* td) {
  if (td->InheritsFrom(TA_MatrixGeom))
    return (taiClassType::BidForType(td) +1);
  return 0;
}

taiData* taiMatrixGeomType::GetDataRepInline_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  taiDimEdit *rval = new taiDimEdit(typ, host_, par, gui_parent_, flags_);
  return rval;
}


//////////////////////////////////
//  taiColorType		//
//////////////////////////////////

int taiColorType::BidForType(TypeDef* td) {
//TODO: we can handle other color guys, just subclass
  if (td->InheritsFrom(TA_taColor))
    return (inherited::BidForType(td) +1);
  return 0;
}

taiData* taiColorType::GetDataRepInline_impl(IDataHost* host_, taiData* par,
 QWidget* gui_parent_, int flags_) 
{
  taiColor *rval = new taiColor(typ, host_, par, gui_parent_, flags_);
  return rval;
}


//////////////////////////////////
//  taitaColorType		//
//////////////////////////////////

int taitaColorType::BidForType(TypeDef* td) {
  if (td->InheritsFrom(TA_taColor))
    return (inherited::BidForType(td) +1);
  return 0;
}

void taitaColorType::GetImage_impl(taiData* dat_, const void* base) {
  taiColor* dat = dynamic_cast<taiColor*>(dat_); // for safety
  if (!dat) return;
  const taColor* col = static_cast<const taColor*>(base);
  dat->GetImage(iColor(col->r, col->g, col->b, col->a));
}

void taitaColorType::GetValue_impl(taiData* dat_, void* base) {
  taiColor* dat = dynamic_cast<taiColor*>(dat_); // for safety
  if (!dat) return;
  iColor icol = dat->GetValue();
  taColor* col = static_cast<taColor*>(base);
  col->Set(icol.redf(), icol.greenf(), icol.bluef(), icol.alphaf()); 
}

//////////////////////////////////
// 	gpiListType		//
//////////////////////////////////

int gpiListType::BidForType(TypeDef* td) {
  if (td->InheritsFrom(TA_taList_impl))
    return (taiClassType::BidForType(td) +1);
  return 0;
}
taiData* gpiListType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  gpiListEditButton *rval = new gpiListEditButton(NULL, typ, host_, par, gui_parent_, flags_);
  return rval;
}


//////////////////////////////////
// 	gpiGroupType		//
//////////////////////////////////

int gpiGroupType::BidForType(TypeDef* td) {
  if(td->InheritsFrom(TA_taGroup_impl))
    return (gpiListType::BidForType(td) +1);
  return 0;
}
taiData* gpiGroupType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  gpiGroupEditButton *rval = new gpiGroupEditButton(NULL, typ, host_, par, gui_parent_, flags_);
  return rval;
}


//////////////////////////////////
// 	gpiArray_Type		//
//////////////////////////////////

int gpiArray_Type::BidForType(TypeDef* td) {
  if (td->InheritsFrom(TA_taArray)) { // bid higher than the class  type
    return (taiClassType::BidForType(td) +1);
  }
  else {
    return 0;
  }
}

taiData* gpiArray_Type::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  gpiArrayEditButton *rval =
    new gpiArrayEditButton(NULL, typ, host_, par, gui_parent_, flags_);
  return rval;
}



//////////////////////////////////
//  	taiTokenPtrType  	//
//////////////////////////////////

int taiTokenPtrType::BidForType(TypeDef* td) {
  if(((td->ptr == 1) && td->DerivesFrom(TA_taBase)) ||
     ((td->ptr == 0) && (td->DerivesFrom(TA_taSmartPtr) || 
      td->DerivesFrom(TA_taSmartRef))) )
    return (taiType::BidForType(td) +1);
  return 0;
}

taiData* taiTokenPtrType::GetDataRep_impl(IDataHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_) 
{
  // setting mode now is good for rest of life
  if (typ->DerivesFrom(TA_taBase))
    mode = MD_BASE;
  else if (typ->DerivesFrom(TA_taSmartPtr)) 
    mode = MD_SMART_PTR;
  else if (typ->DerivesFrom(TA_taSmartRef))
    mode = MD_SMART_REF;
  
  TypeDef* npt = GetMinType(NULL); // only a min type
  bool ro = isReadOnly(par, host_);
  if (ro)
    flags_ |= taiData::flgReadOnly;
  else
    flags_ |= (taiData::flgEditOk | taiData::flgEditDialog);
  if (!npt->tokens.keep)
    flags_ |= taiData::flgNoTokenDlg; // no dialog
  flags_ |= (taiData::flgNullOk);
  taiTokenPtrButton* rval = new taiTokenPtrButton(npt, host_, par, gui_parent_, flags_);
  return rval;
}

TypeDef* taiTokenPtrType::GetMinType(const void* base) {
  // the min type is at least the type of the member, but can be more derived
  TypeDef* rval = NULL;
  // first, we'll try to get a bare minimum type, from the member type itself
  switch (mode) {
  case MD_BASE: {
    rval = typ->GetNonPtrType();
  } break;
  case MD_SMART_PTR: {
    rval = taSmartPtr::GetBaseType(typ);
  } break;
  case MD_SMART_REF: {
    //note: don't know anything about the type w/o an instance
    if (base) {
      taSmartRef& ref = *((taSmartRef*)base);
      rval = ref.GetBaseType();
    } else {
      rval = &TA_taBase; 
    }
  } break;
  }
  return rval;
}


void taiTokenPtrType::GetImage_impl(taiData* dat, const void* base) {
  TypeDef* npt = typ->GetNonPtrType();
  bool ro = isReadOnly(dat);
  if (ro || !npt->tokens.keep) {
    taiEditButton *ebrval = (taiEditButton*) dat;
    ebrval->GetImage_(*((void**) base));
  }
  else {
    taiTokenPtrButton* rval = (taiTokenPtrButton*)dat;
    rval->GetImage(*((TAPtr*)base), npt); // default typ, no scope
  }
}

void taiTokenPtrType::GetValue_impl(taiData* dat, void* base) {
  TypeDef* npt = typ->GetNonPtrType();
  bool ro = isReadOnly(dat);
  if(ro) {
    // do nothing
  }
  else if(!npt->tokens.keep) {
    taMisc::Warning("taiTokenPtrType::GetValue_impl: Value not set for type:",npt->name,
		    "because it is not keeping tokens");
  }
  else {
    taiTokenPtrButton* rval = (taiTokenPtrButton*)dat;
    if(!no_setpointer)
      taBase::SetPointer((TAPtr*)base, (TAPtr)rval->GetValue());
    else
      *((void**)base) = rval->GetValue();
/*type must derive from taBase, otherwise we wouldn't have bid!!!
        if(!no_setpointer && typ->DerivesFrom(TA_taBase))
      taBase::SetPointer((TAPtr*)base, (TAPtr)rval->GetValue());
    else
      *((void**)base) = rval->GetValue(); */
  }
}

taBase* taiTokenPtrType::GetTokenPtr(const void* base) const {
  taBase* tok_ptr = NULL; // this is the addr of the token, in the member
  switch (mode) {
  case MD_BASE: 
  case MD_SMART_PTR:  // is binary compatible
  {
    tok_ptr = *((taBase**)base);
  } break;
  case MD_SMART_REF: {
    taSmartRef& ref = *((taSmartRef*)base);
    tok_ptr = ref.ptr();
  } break;
  }
  return tok_ptr;
}

//////////////////////////
//    taiTypePtr	//
//////////////////////////

void taiTypePtr::Initialize() {
  orig_typ = NULL;
}

int taiTypePtr::BidForType(TypeDef* td) {
  if(td->DerivesFrom(TA_TypeDef) && (td->ptr == 1))
    return taiType::BidForType(td) + 1;
  return 0;
}

// todo: the problem is that it doesn't know at this point what the base is
// and can't therefore figure out what kind of datarep to use..
// need to have a datarep that is a "string or type menu" kind of thing..

taiData* taiTypePtr::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  if (orig_typ == NULL)
    return taiType::GetDataRep_impl(host_, par, gui_parent_, flags_);

  taiTypeDefButton* rval =
    new taiTypeDefButton(typ, host_, par, gui_parent_, flags_);
  return rval;
}

void taiTypePtr::GetImage_impl(taiData* dat, const void* base) {
  if (orig_typ == NULL) {
    taiType::GetImage_impl(dat, base);
    return;
  }

  taiTypeDefButton* rval = (taiTypeDefButton*)dat;
  TypeDef* typ_ = (TypeDef*)*((void**)base);
  rval->GetImage((TypeDef*)*((void**)base), typ_);
}

void taiTypePtr::GetValue_impl(taiData* dat, void* base) {
  if (orig_typ == NULL) {
    taiType::GetValue_impl(dat, base);
    return;
  }

  taiTypeDefButton* rval = (taiTypeDefButton*)dat;
  *((void**)base) = rval->GetValue();
}

////////////////////////
//  taiFilePtrType  //
////////////////////////

int taiFilePtrType::BidForType(TypeDef* td) {
  if(td->DerivesFrom("taFiler") && (td->ptr == 1))
    return (taiType::BidForType(td) +1);
  return 0;
}

taiData* taiFilePtrType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  return new taiFileButton(typ,  host_, par, gui_parent_, flags_);
}

void taiFilePtrType::GetImage_impl(taiData* dat, const void* base){
  taiFileButton* fbut = (taiFileButton*) dat;
  // note: we are a taFiler*
  fbut->SetFiler(*((taFiler**)base));
  fbut->GetImage();
}

void taiFilePtrType::GetValue_impl(taiData* dat, void* base) {
  taiFileButton* rval = (taiFileButton*)dat;
  // safely replace filer, using ref counting
  taRefN::SetRefDone(*((taRefN**)base), rval->GetFiler());
}



//////////////////////////
// 	taiEdit	//
//////////////////////////

void taiEdit::AddEdit(TypeDef* td) {
  taiEdit* cur_ie = td->ie;
  taiEdit** ptr_to_ie = (taiEdit **) &(td->ie);
  while(cur_ie != NULL) {
    if(bid < cur_ie->bid) {
      ptr_to_ie = cur_ie->addr_sub_types();
      cur_ie = cur_ie->sub_types();
    }
    else {
      m_sub_types = cur_ie;
      cur_ie = NULL;
    }
  }
  *ptr_to_ie = this;
}

taiEditDataHost* taiEdit::CreateDataHost(void* base, bool read_only) {
  return new taiEditDataHost(base, typ, read_only);
}

int taiEdit::Edit(void* base, bool readonly, const iColor& bgcol) {
  taiEditDataHost* host = NULL;
  // get currently active win -- we will only look in any other window
  iMainWindowViewer* cur_win = taiMisc::active_wins.Peek_MainWindow();
  host = taiMisc::FindEdit(base, cur_win);
  if (host == NULL) {
    host = CreateDataHost(base, readonly);

    if (typ->HasOption("NO_OK"))
      host->no_ok_but = true;
    if (typ->HasOption("NO_CANCEL"))
      host->read_only = true;
    if (taMisc::color_hints & taMisc::CH_EDITS) {
      if (&bgcol == &def_color) {
        bool ok = false;
        iColor bg = GetBackgroundColor(base, ok);
        if (ok) host->setBgColor(bg);
      } else 
        host->setBgColor(bgcol);
    }
    host->Constr("", "");
//TODO: no longer supported:    host->cancel_only = readonly;
    return host->Edit(false);
  } else if (!host->modal) {
    host->Raise();
  }
  return 2;
}

int taiEdit::EditDialog(void* base, bool read_only, bool modal,
  const iColor& bgcol) 
{
  taiEditDataHost* host = NULL;
  if (!modal) {
    host = taiMisc::FindEditDialog(base, read_only);
    if (host) {
      host->Raise();
      return 2;
    }
  }
  host = CreateDataHost(base, read_only);
  if (taMisc::color_hints & taMisc::CH_EDITS) {
    if (&bgcol == &def_color) {
      bool ok = false;
      iColor bg = GetBackgroundColor(base, ok);
      if (ok) host->setBgColor(bg);
    } else 
        host->setBgColor(bgcol);
  } else {
  //TODO: maybe we always null out, or should we allow caller to specify?
    //bgcol = NULL; 
  }
  host->Constr("", "", taiDataHost::HT_DIALOG);
  return host->Edit(modal);
  
}

EditDataPanel* taiEdit::EditNewPanel(taiDataLink* link, void* base,
   bool read_only, const iColor& bgcol) 
{
  taiEditDataHost* host = CreateDataHost(base, read_only);
  if (taMisc::color_hints & taMisc::CH_EDITS) {
    if (&bgcol == &def_color) {
      bool ok = false;
      iColor bg = GetBackgroundColor(base, ok);
      if (ok) host->setBgColor(bg);
    } else 
      host->setBgColor(bgcol);
  } else {
  //TODO: maybe we always null out, or should we allow caller to specify?
    //bgcol = NULL; 
  }

  host->Constr("", "", taiDataHost::HT_PANEL, true);
  EditDataPanel* rval = host->EditPanelDeferred(link);
  return rval;
}

EditDataPanel* taiEdit::EditPanel(taiDataLink* link, void* base,
   bool read_only, iMainWindowViewer* not_in_win, const iColor& bgcol) 
{
  taiEditDataHost* host = NULL;
  host = taiMisc::FindEditPanel(base, read_only, not_in_win);
  if (host) {
    host->Raise();
    return host->dataPanel();
  } else { 
    return EditNewPanel(link, base, read_only, bgcol);
  }
}

const iColor taiEdit::GetBackgroundColor(void* base, bool& ok) {
  if (typ->InheritsFrom(&TA_taBase) && base) {
    return ((taBase*)base)->GetEditColorInherit(ok);
  } 
  ok = false;
  return def_color;
}


//////////////////////////
// 	taiMember	//
//////////////////////////

// macro for doing safe casts of types -- helps to uncover errors
// when the actual class is not the expected class
// (of course this NEVER happens... uh, huh... ;) )
#define QCAST_MBR_SAFE_EXIT(qtyp, rval, dat) \
  qtyp rval = qobject_cast<qtyp>(dat); \
  if (!rval) { \
    taMisc::Error("QCAST_MBR_SAFE_EXIT: expect " #qtyp "for mbr", \
    mbr->name, "but was:", \
      dat->metaObject()->className()); \
    return; \
  }


void taiMember::EndScript(const void* base) {
  if(taMisc::record_script == NULL)
    return;
  if((((TAPtr)base)->GetOwner() == NULL) && ((TAPtr)base != tabMisc::root))
    return;	// no record for unowned objs (except root)!
  *taMisc::record_script << "}" << endl;
}

bool taiMember::isReadOnly(taiData* dat, IDataHost* host_) {
  // ReadOnly if parent type is RO, or par is RO, OR directives state RO
  bool rval = taiType::isReadOnly(dat, host_);
  rval = rval || mbr->HasOption("READ_ONLY") || //note: 'IV' only for legacy support
    mbr->HasOption("IV_READ_ONLY") || mbr->HasOption("GUI_READ_ONLY");
  return rval;
}
bool taiMember::handlesReadOnly() {
  return mbr->type->it->handlesReadOnly();
}


taiData* taiMember::GetDataRep(IDataHost* host_, taiData* par, QWidget* gui_parent_,
	taiType* parent_type_, int flags)
{
  bool ro = isReadOnly(par, host_);
  // we must communicate read_only when getting item
  //TODO: probably should also use parent_type in determining ro, as base class does
  if (ro)
    flags |= taiData::flgReadOnly;
  if (((mbr->HasOption(TypeItem::opt_inline)) ||
       (mbr->HasOption(TypeItem::opt_edit_inline))) 
    && mbr->type->it->allowsInline())
    flags |= taiData::flgInline;
  if (mbr->HasOption(TypeItem::opt_EDIT_DIALOG)) // if a string field, puts up an editor button
    flags |= taiData::flgEditDialog;
  if (mbr->HasOption(TypeItem::opt_APPLY_IMMED))
    flags |= taiData::flgAutoApply;
  if (mbr->HasOption("NO_EDIT_APPLY_IMMED"))
    flags |= taiData::flgNoEditDialogAutoApply; // just in case this is needed
  if (mbr->HasOption("NO_ALPHA")) // for color types only, ignored by others
    flags |= taiData::flgNoAlpha; // just in case this is needed
    
  ro = (flags & taiData::flgReadOnly); // just for clarity and parity with Image/Value
  taiData* rval = NULL;
  if (ro || !isCondEdit()) { // condedit is irrelevant
    rval = GetArbitrateDataRep(host_, par, gui_parent_, flags);
  } else { // rw && condEdit 
    taiDataDeck* deck = new taiDataDeck(typ, host_, par, gui_parent_, flags);
  
    deck->InitLayout();
    // rw child -- always the impl of this guy
    gui_parent_ = deck->GetRep();
    taiData* child = GetArbitrateDataRep(host_, deck, gui_parent_, flags);;
    deck->AddChildWidget(child->GetRep());
    child->SetMemberDef(mbr); // not used much, ex. used by taiField for edit dialog info
    
    // ro child, either ro of this guy, or default if we don't handle ro
    flags |= taiData::flgReadOnly;
    child = GetArbitrateDataRep(host_, deck, gui_parent_, flags);;
    deck->AddChildWidget(child->GetRep());
    child->SetMemberDef(mbr); // not used much, ex. used by taiField for edit dialog info
    deck->EndLayout();
    rval = deck;
  }
  rval->SetMemberDef(mbr); // not used much, ex. used by taiField for edit dialog info
  return rval;
}

taiData* taiMember::GetDataRep_impl(IDataHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_) 
{
  taiData* dat = mbr->type->it->GetDataRep(host_, par, gui_parent_, this, flags_);
  return dat;
}

//NOTE: we always delegate to _impl because those do the readonly delegation by
// calling the non-impl version of their functions
void taiMember::GetImage(taiData* dat, const void* base) {
  bool is_visible = true;
  if (isCondShow()) {
    bool is_on = false; // defaults here make it editable in test chain below
    bool val_is_eq = false;
    //note: we don't care if processed or not -- flag defaults make it editable
    CheckProcessCondMembMeth("CONDSHOW", mbr, base, is_on, val_is_eq);
    is_visible = ((is_on && val_is_eq) || (!is_on && !val_is_eq));
    dat->setVisible(is_visible);
  }
  // note: we must always fall through and get image, even when invisible, for
  // when we change visibility, so the result is valid
  
  // note: must use identical criteria for ro here as when we did GetDataRep
  bool ro = dat->HasFlag(taiData::flgReadOnly) ;
  
  if (ro || !isCondEdit()) { // condedit is irrelevant
    GetArbitrateImage(dat, base);
  } else { // rw && condEdit
    QCAST_MBR_SAFE_EXIT(taiDataDeck*, deck, dat)
    bool is_on = false; // defaults here make it editable in test chain below
    bool val_is_eq = false;
    int img = 0;
    //note: we don't care if processed or not -- flag defaults make it editable
    CheckProcessCondMembMeth("CONDEDIT", mbr, base, is_on, val_is_eq);
    if (is_on) {
      if (val_is_eq) img = 0; // editable
      else           img = 1;	// not editable
    } else {
      if (val_is_eq) img = 1;	// not editable
      else           img = 0;	// editable
    }
    deck->GetImage(img); // this is the one that is visible
    // NOTE: we must *always* get both images, so the val img is valid if we switch to rw
    GetArbitrateImage(deck->data_el.FastEl(0), base);
    GetArbitrateImage(deck->data_el.FastEl(1), base);
  }
}

void taiMember::GetImage_impl(taiData* dat, const void* base) {
  mbr->type->it->GetImage(dat, mbr->GetOff(base));
  GetOrigVal(dat, base);
}

void taiMember::GetMbrValue(taiData* dat, void* base, bool& first_diff) {
  bool is_visible = true;
  if (isCondShow()) {
    bool is_on = false; // defaults here make it editable in test chain below
    bool val_is_eq = false;
    //note: we don't care if processed or not -- flag defaults make it editable
    CheckProcessCondMembMeth("CONDSHOW", mbr, base, is_on, val_is_eq);
    is_visible = ((is_on && val_is_eq) || (!is_on && !val_is_eq));
    dat->setVisible(is_visible);
  }
  // note: we must always fall through and get value, even when invisible, for
  // when we change visibility, so the result is actually saved!
  
  // note: must use identical criteria for ro here as when we did GetDataRep
  bool ro = dat->HasFlag(taiData::flgReadOnly);
  if (ro) return; // duh!
  
  if (!isCondEdit()) { 
    // can never be ro! so it was always this one:
    GetArbitrateMbrValue(dat, base, first_diff);
  } else { // note: we only do this if we aren't RO, otherwise there is no point
    QCAST_MBR_SAFE_EXIT(taiDataDeck*, deck, dat)
/*nn  bool is_on = false; // defaults here make it editable in test chain below
    bool val_is_eq = false;
    int img = 0;
    //note: we don't care if processed or not -- flag defaults make it editable
    CheckProcessCondMembMeth("CONDEDIT", mbr, base, is_on, val_is_eq);
    if (is_on) {
      if (val_is_eq) img = 0; // editable
      else           img = 1;	// not editable
    } else {
      if (val_is_eq) img = 1;	// not editable
      else           img = 0;	// editable
    }*/
    // NOTE: we must *always* get the rw val in case we had switched editability
    dat = deck->data_el.FastEl(0);
    GetArbitrateMbrValue(dat, base, first_diff);
  }
  CmpOrigVal(dat, base, first_diff);
}

void taiMember::GetOrigVal(taiData* dat, const void* base) {
  dat->orig_val = mbr->type->GetValStr(mbr->GetOff(base));
  // if a default value was specified, compare and set the highlight accordingly
  if (!isReadOnly(dat)) { 
    String defval = mbr->OptionAfter("DEF_");
    if (!defval.empty()) {
      if (dat->orig_val != defval)
        dat->setHighlight(true);
      else
        dat->setHighlight(false);
    }
  }
}

void taiMember::GetMbrValue_impl(taiData* dat, void* base) {
  mbr->type->it->GetValue(dat, mbr->GetOff(base));
}

bool taiMember::isCondEdit() const { //return false; //TEMP
  String optedit = mbr->OptionAfter("CONDEDIT_");
  return optedit.nonempty();
}

bool taiMember::isCondShow() const {
  String optedit = mbr->OptionAfter("CONDSHOW_");
  return optedit.nonempty();
}

void taiMember::StartScript(const void* base) {
  if((taMisc::record_script == NULL) || !typ->InheritsFrom(TA_taBase))
    return;

  if((((TAPtr)base)->GetOwner() == NULL) && ((TAPtr)base != tabMisc::root))
    return;	// no record for unowned objs (except root)!
  TAPtr tab = (TAPtr)base;
  *taMisc::record_script << "{ " << typ->name << "* ths = "
			   << tab->GetPath() << ";" << endl;
}

void taiMember::CmpOrigVal(taiData* dat, const void* base, bool& first_diff) {
  if((taMisc::record_script == NULL) || !typ->InheritsFrom(TA_taBase))
    return;
  if((((TAPtr)base)->GetOwner() == NULL) && ((TAPtr)base != tabMisc::root))
    return;	// no record for unowned objs (except root)!
  String new_val = mbr->type->GetValStr(mbr->GetOff(base));
  if(dat->orig_val == new_val)
    return;
  if(first_diff)
    StartScript(base);
  first_diff = false;
  new_val.gsub("\"", "\\\"");	// escape the quotes..
  *taMisc::record_script << "  ths->" << mbr->name << " = \""
			   << new_val << "\";" << endl;
}

void taiMember::AddMember(MemberDef* md) {
  taiMember* cur_im = md->im;
  taiMember** ptr_to_im = (taiMember **) &(md->im);
  while(cur_im != NULL) {
    if(bid < cur_im->bid) {
      ptr_to_im = cur_im->addr_sub_types();
      cur_im = cur_im->sub_types();
    }
    else {
      m_sub_types = cur_im;
      cur_im = NULL;
    }
  }
  *ptr_to_im = this;
}

TypeDef* taiMember::GetTargetType(const void* base) {
  TypeDef* targ_typ = typ; // may get overridden by comment directives
  if (!mbr)  return targ_typ;
  // a XxxDef* can have one of three options to specify the
  // target type for its XxxDef menu.
  // 1) a TYPE_xxxx in its comment directives
  // 2) a TYPE_ON_xxx in is comment directives, specifying the name
  //    of the member in the same object which is a TypeDef*
  // 3) Nothing, which defaults to the type of the object the memberdef
  //      is in.

  String mb_nm = mbr->OptionAfter("TYPE_ON_");
  if (!mb_nm.empty()) {
//    taBase* base = (taBase*)host->cur_base; //TODO: highly unsafe cast -- should provide As_taBase() or such in taiDialog
    if (base) {
      void* adr; // discarded
      MemberDef* tdmd = ((taBase*)base)->FindMembeR(mb_nm, adr); //TODO: highly unsafe cast!!
      if (tdmd != NULL)
        targ_typ = *((TypeDef **) tdmd->GetOff(base));
    }
    return targ_typ;
  } 
  
  mb_nm = mbr->OptionAfter("TYPE_");
  if (!mb_nm.empty()) {
    targ_typ = taMisc::types.FindName(mb_nm);
    return targ_typ;
  }
  return targ_typ;
};

taiData* taiMember::GetArbitrateDataRep(IDataHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_)
{
  taiData* rval = NULL;
  
/*if (m_sub_types) {
    rval = sub_types()->GetDataRep(host_, rval, gui_parent_, NULL, flags_);
  } else {*/
    bool ro = (flags_ & taiData::flgReadOnly); 
    if (ro && !handlesReadOnly()) {
      rval = taiMember::GetDataRep_impl(host_, par, gui_parent_, flags_);
    } else {
      rval = GetDataRep_impl(host_, par, gui_parent_, flags_);
    }
//}
  return rval;
}

void taiMember::GetArbitrateImage(taiData* dat, const void* base) {
/*if (m_sub_types) {
    sub_types()->GetImage(dat, base);
  } else {*/
    bool ro = (dat->HasFlag(taiData::flgReadOnly)); 
    if (ro && !handlesReadOnly()) {
      taiMember::GetImage_impl(dat, base);
    } else {
      GetImage_impl(dat, base);
    }
// }
}

void taiMember::GetArbitrateMbrValue(taiData* dat, void* base, bool& first_diff) {
/*if (m_sub_types) {
    sub_types()->GetMbrValue(dat, base, first_diff);
  } else {*/
    bool ro = (dat->HasFlag(taiData::flgReadOnly)); 
    if (ro && !handlesReadOnly()) {
      taiMember::GetMbrValue_impl(dat, base);
    } else {
      GetMbrValue_impl(dat, base);
    }
//}
}

//////////////////////////////////
//  	taiTokenPtrMember  	//
//////////////////////////////////

int taiTokenPtrMember::BidForMember(MemberDef* md, TypeDef* td) {
  if (td->InheritsFrom(&TA_taBase) &&
     ((md->type->ptr == 1) && md->type->DerivesFrom(TA_taBase)) ||
     ((md->type->ptr == 0) && (md->type->DerivesFrom(TA_taSmartPtr) || 
      md->type->DerivesFrom(TA_taSmartRef))) )
     return inherited::BidForMember(md,td) + 1;
  return 0;
}

taiData* taiTokenPtrMember::GetDataRep_impl(IDataHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_) 
{
  // setting mode now is good for rest of life
  if (mbr->type->DerivesFrom(TA_taBase))
    mode = MD_BASE;
  else if (mbr->type->DerivesFrom(TA_taSmartPtr)) 
    mode = MD_SMART_PTR;
  else if (mbr->type->DerivesFrom(TA_taSmartRef))
    mode = MD_SMART_REF;
    
  TypeDef* npt = GetMinType(NULL); // note: this will only be a min type
  if (!mbr->HasOption("NO_NULL"))
    flags_ |= taiData::flgNullOk;
  // options that require non-readonly
  if (!(flags_ & taiData::flgReadOnly)) {
    if (!mbr->HasOption("NO_EDIT")) //note: #EDIT is the default
      flags_ |= taiData::flgEditOk;
    
    if (flags_ & taiData::flgEditOk)
      flags_ |= taiData::flgEditDialog;
  }
/*TODO: prob can't have disabling for no keep tokens, because sometimes
  we don't know the type
  if (!npt->tokens.keep)
    flags_ |= taiData::flgNoTokenDlg; // no dialog */
  taiTokenPtrButton* rval = new taiTokenPtrButton(npt, host_, par, gui_parent_,
	flags_);
  String filt_nm = mbr->OptionAfter("ITEM_FILTER_");
  if (filt_nm.nonempty()) {
    TypeDef* par_typ = mbr->owner->owner;
    if (par_typ) {
      MethodDef* md = par_typ->methods.FindName(filt_nm);
      if (md && md->is_static)
        rval->item_filter = (item_filter_fun)(md->addr);
    }
  }
  return rval;
}

TypeDef* taiTokenPtrMember::GetMinType(const void* base) {
  // the min type is at least the type of the member, but can be more derived
  TypeDef* rval = NULL;
  // first, we'll try to get a bare minimum type, from the member type itself
  switch (mode) {
  case MD_BASE: {
    rval = mbr->type->GetNonPtrType();
  } break;
  case MD_SMART_PTR: {
    rval = taSmartPtr::GetBaseType(mbr->type);
  } break;
  case MD_SMART_REF: {
    //note: don't know anything about the type w/o an instance
    if (base) {
      taSmartRef& ref = *((taSmartRef*)(mbr->GetOff(base)));
      rval = ref.GetBaseType();
    } else {
      rval = &TA_taBase; 
    }
  } break;
  }
  
  // now, further restrict according to type directives
  TypeDef* dir_type = NULL;
  // dynamic (member-based) type scoping
  String tmp = mbr->OptionAfter("TYPE_ON_");
  if (tmp.nonempty()) {
    if (base) {
      MemberDef* md = typ->members.FindName(tmp);
      if (md && (md->type->ptr == 1) && md->type->DerivesFrom(&TA_TypeDef))
        dir_type = (TypeDef*)*((void**)md->GetOff(base)); // set according to value of this member
    }
  } else {
    // static type scoping
    tmp = mbr->OptionAfter("TYPE_");
    if (tmp.nonempty()) {
      dir_type = taMisc::types.FindName(tmp);
    }
  }
  if (dir_type && dir_type->InheritsFrom(rval))
    rval = dir_type;
  
  return rval;
}

taBase* taiTokenPtrMember::GetTokenPtr(const void* base) const {
  taBase* tok_ptr = NULL; // this is the addr of the token, in the member
  switch (mode) {
  case MD_BASE: 
  case MD_SMART_PTR:  // is binary compatible
  {
    tok_ptr = *((taBase**)mbr->GetOff(base));
  } break;
  case MD_SMART_REF: {
    taSmartRef& ref = *((taSmartRef*)(mbr->GetOff(base)));
    tok_ptr = ref.ptr();
  } break;
  }
  return tok_ptr;
}

void taiTokenPtrMember::GetImage_impl(taiData* dat, const void* base) {
  taBase* tok_ptr = GetTokenPtr(base); // this is the addr of the token, in the member
  TypeDef* targ_typ = GetMinType(base);
  
  TAPtr scope = NULL;
  if (!mbr->HasOption("NO_SCOPE")) {
    scope = (TAPtr)base;
/*nn    if((rval->host != NULL) && (rval->host)->GetBaseTypeDef()->InheritsFrom(TA_taBase))
      scope = (TAPtr)(rval->host)->Base(); */
  }
  TypeDef* scope_type = NULL;
  String sctyp = mbr->OptionAfter("SCOPE_");
  if(!sctyp.empty()) {
    scope_type = taMisc::types.FindName(sctyp);
  }
    
  taiTokenPtrButton* tpb = (taiTokenPtrButton*)dat;
  tpb->GetImage(tok_ptr, targ_typ, scope, scope_type);
  GetOrigVal(dat, base);
}

void taiTokenPtrMember::GetMbrValue_impl(taiData* dat, void* base) {
//note: in 3.2 we bailed if not keeping tokens, but that is complicated to test
// and could modally depend on dynamic type directives, so we just always set
    
  taiTokenPtrButton* rval = (taiTokenPtrButton*)dat;
  switch (mode) {
  case MD_BASE:
    if (no_setpointer)
      *((void**)mbr->GetOff(base)) = rval->GetValue();
    else
      taBase::SetPointer((TAPtr*)mbr->GetOff(base), (TAPtr)rval->GetValue());
    break;
  case MD_SMART_PTR: //WARNING: use of no_setpointer on smartptrs is not defined!
    taBase::SetPointer((TAPtr*)mbr->GetOff(base), (TAPtr)rval->GetValue());
    break;
  case MD_SMART_REF: {
    taSmartRef& ref = *((taSmartRef*)(mbr->GetOff(base)));
    ref = (taBase*)rval->GetValue();
  } break;
  }
}


/////////////////////////////
//    taiDefaultToken    //
/////////////////////////////

int taiDefaultToken::BidForMember(MemberDef* md, TypeDef* td) {
  TypeDef* mtd = md->type;
  if (((mtd->ptr == 1) && mtd->DerivesFrom(TA_taBase)) &&
     md->HasOption("DEFAULT_EDIT"))
    return taiTokenPtrMember::BidForMember(md,td)+10;
  return 0;
}

taiData* taiDefaultToken::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  taiDefaultEdit* tde = new taiDefaultEdit(mbr->type->GetNonPtrType());
  taiEditButton *rval =	taiEditButton::New(NULL,	tde, mbr->type->GetNonPtrType(),
     host_, par, gui_parent_, flags_);
  return rval;
}

void taiDefaultToken::GetImage_impl(taiData* dat, const void* base) {

  taiEditButton* rval = (taiEditButton*)dat;
  taBase* token_ptr = GetTokenPtr(base);
  rval->GetImage_(token_ptr);
  if (token_ptr) {
    rval->typ = token_ptr->GetTypeDef();
  }
}

void taiDefaultToken::GetMbrValue_impl(taiData*, void*) {
  return;
}

//////////////////////////////////
//	taiSubTokenPtrMember	//
//////////////////////////////////

int taiSubTokenPtrMember::BidForMember(MemberDef* md, TypeDef* td) {
  if((md->type->ptr == 1) && (md->OptionAfter("SUBTYPE_") != ""))
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

taiData* taiSubTokenPtrMember::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  TypeDef* td = NULL;
  String typ_nm = mbr->OptionAfter("SUBTYPE_");
  if (!typ_nm.empty())
    td = taMisc::types.FindName((char*)typ_nm);
  if (td == NULL)
    td = mbr->type;
  if (mbr->HasOption("NULL_OK"))
    flags_ |= taiData::flgNullOk;
  if (!mbr->HasOption("NO_EDIT"))
    flags_ |= taiData::flgEditOk;
  taiSubToken* rval =
    new taiSubToken( taiMenu::buttonmenu, taiMisc::fonSmall, td, host_, par, gui_parent_, flags_);
  return rval;
}


void taiSubTokenPtrMember::GetImage_impl(taiData* dat, const void* base){
  void* new_base = mbr->GetOff(base);
  taiSubToken* rval = (taiSubToken*)dat;
//nn, done in GetImage  rval->UpdateMenu();
  rval->GetImage(base,*((void **)new_base));
  GetOrigVal(dat, base);
}

void taiSubTokenPtrMember::GetMbrValue_impl(taiData* dat, void* base) {
  void* new_base = mbr->GetOff(base);
  taiSubToken* rval = (taiSubToken*)dat;
  if (!no_setpointer && mbr->type->DerivesFrom(TA_taBase))
    taBase::SetPointer((TAPtr*)new_base, (TAPtr)rval->GetValue());
  else
    *((void**)new_base) = rval->GetValue();
}



//////////////////////////////////
//	taiTypePtrMember	//
//////////////////////////////////

int taiTypePtrMember::BidForMember(MemberDef* md, TypeDef* td) {
  if ((md->type->ptr == 1) &&
     ((md->OptionAfter("TYPE_") != "") || (md->OptionAfter("TYPE_ON_") != "")
      || (md->HasOption("NULL_OK")))
     && md->type->DerivesFrom(TA_TypeDef))
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

taiData* taiTypePtrMember::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_,
  int flags_) 
{
  if (mbr->HasOption("NULL_OK"))
    flags_ |= taiData::flgNullOk;
  taiTypeDefButton* rval =
    new taiTypeDefButton(mbr->type, host_, par, gui_parent_, flags_);
  return rval;
}

void taiTypePtrMember::GetImage_impl(taiData* dat, const void* base){
  void* new_base = mbr->GetOff(base);
  taiTypeDefButton* rval = (taiTypeDefButton*)dat;
  TypeDef* td = NULL;
  String mb_nm = mbr->OptionAfter("TYPE_ON_");
  if (mb_nm != "") {
    MemberDef* md = typ->members.FindName(mb_nm);
    if (md != NULL)
      //TODO: should check that member is a TypeDef!!!
      td = (TypeDef*)*((void**)md->GetOff(base)); // set according to value of this member
  } else {
    mb_nm = mbr->OptionAfter("TYPE_");
    if(mb_nm == "this")
      td = typ;
    else if(mb_nm != "")
      td = taMisc::types.FindName((char*)mb_nm);
  }
  TypeDef* targ_typ = (td) ? td : mbr->type;
  rval->GetImage((TypeDef*)*((void**)new_base), targ_typ);
  GetOrigVal(dat, base);
}

void taiTypePtrMember::GetMbrValue_impl(taiData* dat, void* base) {
  void* new_base = mbr->GetOff(base);
  taiTypeDefButton* rval = (taiTypeDefButton*)dat;
  TypeDef* nw_typ = (TypeDef*)rval->GetValue();
  if (mbr->HasOption("NULL_OK"))
    *((void**)new_base) = nw_typ;
  else {
    if (nw_typ)
      *((void**)new_base) = nw_typ;
  }
}


//////////////////////////////////
//	taiEnumTypePtrMember	//
//////////////////////////////////

int taiEnumTypePtrMember::BidForMember(MemberDef* md, TypeDef* td) {
  if (md->HasOption("ENUM_TYPE"))
    return (inherited::BidForMember(md,td) + 1);
  return 0;
}

taiData* taiEnumTypePtrMember::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, 
  int flags_) 
{
  if (mbr->HasOption("NULL_OK"))
    flags_ |= taiData::flgNullOk;
  taiEnumTypeDefButton* rval =
    new taiEnumTypeDefButton(mbr->type, host_, par, gui_parent_, flags_);
  return rval;
}



//////////////////////////////////////////
//	taiMemberDefPtrMember		//
//////////////////////////////////////////

int taiMemberDefPtrMember::BidForMember(MemberDef* md, TypeDef* td) {
  if ((md->type->ptr == 1) && (md->type->DerivesFrom(TA_MemberDef)))
    return (inherited::BidForMember(md,td) + 1);
  return 0;
}

taiData* taiMemberDefPtrMember::GetDataRep_impl(IDataHost* host_, taiData* par, 
  QWidget* gui_parent_, int flags_) 
{
  taiMemberDefButton* rval =  new taiMemberDefButton(typ, host_, par, gui_parent_, flags_);
  return rval;
}

void taiMemberDefPtrMember::GetImage_impl(taiData* dat, const void* base){
  void* new_base = mbr->GetOff(base);
  taiMemberDefButton* rval = (taiMemberDefButton*)dat;
  MemberDef* cur_sel = *((MemberDef**)(new_base));
  rval->GetImage(cur_sel, GetTargetType(base));
  GetOrigVal(dat, base);
}

void taiMemberDefPtrMember::GetMbrValue_impl(taiData* dat, void* base) {
  void* new_base = mbr->GetOff(base);
  taiMemberDefButton* rval = (taiMemberDefButton*)dat;
  *((MemberDef**)new_base) = rval->GetValue();
}


//////////////////////////////////////////
//	taiMethodDefPtrMember		//
//////////////////////////////////////////

int taiMethodDefPtrMember::BidForMember(MemberDef* md, TypeDef* td) {
  if ((md->type->ptr == 1) && (md->type->DerivesFrom(TA_MethodDef)))
    return (inherited::BidForMember(md,td) + 1);
  return 0;
}

taiData* taiMethodDefPtrMember::GetDataRep_impl(IDataHost* host_, taiData* par, 
  QWidget* gui_parent_, int flags_) 
{
  taiMethodDefButton* rval = new taiMethodDefButton(typ, host_, 
    par, gui_parent_, flags_);
  return rval;
}

void taiMethodDefPtrMember::GetImage_impl(taiData* dat, const void* base){
  void* new_base = mbr->GetOff(base);
  taiMethodDefButton* rval = (taiMethodDefButton*)dat;
  MethodDef* cur_sel = *((MethodDef**)(new_base));
  rval->GetImage(cur_sel, GetTargetType(base));
  GetOrigVal(dat, base);
}

void taiMethodDefPtrMember::GetMbrValue_impl(taiData* dat, void* base) {
  void* new_base = mbr->GetOff(base);
  taiMethodDefButton* rval = (taiMethodDefButton*)dat;
  *((MethodDef**)new_base) = rval->GetValue();
}


//////////////////////////////
//	taiFunPtrMember   //
//////////////////////////////

int taiFunPtrMember::BidForMember(MemberDef* md, TypeDef* td) {
  if (md->fun_ptr)
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

taiData* taiFunPtrMember::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  taiButtonMenu* rval = new taiButtonMenu(taiMenu::radio_update, taiMisc::fonSmall,
      typ, host_, par, gui_parent_, flags_);
  rval->AddItem("NULL");
  rval->AddSep();
  MethodDef* fun;
  for (int i = 0; i < TA_taRegFun.methods.size; ++i) {
    fun = TA_taRegFun.methods.FastEl(i);
    if (mbr->CheckList(fun->lists))
      rval->AddItem((char*)fun->name, (void*)fun->addr);
  }
  return rval;
}

void taiFunPtrMember::GetImage_impl(taiData* dat, const void* base){
  void* new_base = mbr->GetOff(base);
  taiButtonMenu* rval = (taiButtonMenu*)dat;
  if(*((void**)new_base) == NULL) {
    rval->GetImageByData(Variant(0));
    return;
  }
  int cnt;
  MethodDef* fun = TA_taRegFun.methods.FindOnListAddr(*((ta_void_fun*)new_base),
							mbr->lists, cnt);
  if (fun)
    rval->GetImageByIndex(cnt + 1); //1 for NULL item
  GetOrigVal(dat, base);
}

void taiFunPtrMember::GetMbrValue_impl(taiData* dat, void* base) {
  void* new_base = mbr->GetOff(base);
  taiButtonMenu* rval = (taiButtonMenu*)dat;
  taiAction* cur = rval->curSel();
  if (cur != NULL)
    *((void**)new_base) = cur->usr_data.toPtr();
}

//////////////////////////////
//	taiCondEditMember  //
//////////////////////////////

/*
int taiCondEditMember::BidForMember(MemberDef* md, TypeDef* td) {
  if (!td->InheritsFrom(TA_taBase))
    return 0;
  String optedit = md->OptionAfter("CONDEDIT_");
  if (!optedit.empty()) {
    return (taiMember::BidForMember(md,td) + 100); // 100 = definitely do this over other!
  } 
  return 0;
}

taiData* taiCondEditMember::GetDataRep_impl(IDataHost* host_,
   taiData* par, QWidget* gui_parent_, int flags_) 
{
  // only get read-only for modal dialogs (otherwise, no chance to update!)
  bool use_ro = (!host_->isModal()); 
  
  if (use_ro) flags_ |= taiData::flgCondEditUseRO;
  
  taiDataDeck* rval = new taiDataDeck(typ, host_, par, gui_parent_, flags_);

  rval->InitLayout();
  taiData* child;
  if (m_sub_types)
    child = sub_types()->GetDataRep(host_, rval, gui_parent_, NULL, flags_);
  else
    child = taiMember::GetDataRep_impl(host_, rval, gui_parent_, flags_);
  rval->AddChildWidget(child->GetRep());
  if (use_ro) {
    child = taiMember::GetDataRep_impl(host_, rval, gui_parent_, (flags_ | taiData::flgReadOnly));
    rval->AddChildWidget(child->GetRep());
  }
  rval->EndLayout();
  return rval;
}


void taiCondEditMember::GetImage_impl(taiData* dat, const void* base){
  QCAST_MBR_SAFE_EXIT(taiDataDeck*, rval, dat)
  if(m_sub_types) {
    sub_types()->GetImage(rval->data_el.FastEl(0), base);
  }
  else {
    taiMember::GetImage_impl(rval->data_el.FastEl(0), base);
  }
  bool use_ro = rval->HasFlag(taiData::flgCondEditUseRO); 
  if (use_ro) {	// only if two options (i.e., !host_->waiting)
//    ro_im->GetImage(rval->data_el.FastEl(1), base);
    taiMember::GetImage_impl(rval->data_el.FastEl(1), base);

    bool is_on = false; // defaults here make it editable in test chain below
    bool val_is_eq = false;
    //note: we don't care if processed or not -- flag defaults make it editable
    CheckProcessCondMembMeth("CONDEDIT", mbr, base, is_on, val_is_eq);
    if (is_on) {
      if (val_is_eq)
	rval->GetImage(0);	// editable
      else
	rval->GetImage(1);	// not editable
    } else {
      if (val_is_eq)
	rval->GetImage(1);	// not editable
      else
	rval->GetImage(0);	// editable
    }
  } else {
    rval->GetImage(0);		// always editable
  }
  GetOrigVal(dat, base);
}

void taiCondEditMember::GetMbrValue(taiData* dat, void* base, bool& first_diff) {
// note: these guys are so hairy and complicated, we put these tests in!
  taiDataDeck* rval = qobject_cast<taiDataDeck*>(dat);
  if (!rval) {
    taMisc::Error("taiCondEditMember::GetMbrValue: expect taiDataDeck for mbr",
    mbr->name, "but was:",
      dat->metaObject()->className());
    return;
  }
  if (m_sub_types) {
    sub_types()->GetMbrValue(rval->data_el.FastEl(0), base, first_diff);
  } else {
    taiMember::GetMbrValue(rval->data_el.FastEl(0), base, first_diff);
  }
  //note: of course never need to get the RO guy...
}
*/

/////////////////////////////
//    taiTDefaultMember  //
/////////////////////////////

int taiTDefaultMember::BidForMember(MemberDef*, TypeDef*) {
// TD_Default member does not bid, it is only applied in special cases.
  return (0);
}

taiData* taiTDefaultMember::GetDataRep(IDataHost* host_,
  taiData* par, QWidget* gui_parent_, int flags_)
{
  taiPlusToggle* rval = new taiPlusToggle(typ, host_, par, gui_parent_, flags_);
  rval->InitLayout();
  taiData* rdat;
  if (m_sub_types)
    rdat = sub_types()->GetDataRep(host_, rval, rval->GetRep(), NULL, flags_);
  else
    rdat = taiMember::GetDataRep_impl(host_, rval, rval->GetRep(), flags_);
  rval->data = rdat;
  rval->AddChildWidget(rdat->GetRep());
  rval->EndLayout();
  return rval;
}

void taiTDefaultMember::GetImage(taiData* dat, const void* base) {
  QCAST_MBR_SAFE_EXIT(taiPlusToggle*, rval, dat)
  if (m_sub_types) {
    sub_types()->GetImage(rval->data, base);
  } else {
    taiMember::GetImage_impl(rval->data, base);
  }
  taBase_List* gp = typ->defaults;
  tpdflt = NULL;
  if (gp != NULL) {
    for (int i = 0; i < gp->size; ++i) {
      TypeDefault* td = (TypeDefault*)gp->FastEl(i);
      if (td->token == (TAPtr)base) {
	tpdflt = td;
	break;
      }
    }
  }
  if (tpdflt != NULL)
    rval->GetImage(tpdflt->GetActive(mbr->idx));
  GetOrigVal(dat, base);
}

void taiTDefaultMember::GetMbrValue(taiData* dat, void* base, bool& first_diff) {
  //note: we don't call the inherited, or use the impls
  QCAST_MBR_SAFE_EXIT(taiPlusToggle*, rval, dat)
  if (m_sub_types) {
    sub_types()->GetMbrValue(rval->data, base, first_diff);
  } else {
    taiMember::GetMbrValue(rval->data, base,first_diff);
  }
  if (tpdflt != NULL)		// gotten by prev GetImage
    tpdflt->SetActive(mbr->idx, rval->GetValue());
//nn  CmpOrigVal(dat, base, first_diff);
}



//////////////////////////
// 	taiMethod	//
//////////////////////////

void taiMethod::AddMethod(MethodDef* md) {
  taiMethod* cur_im = md->im;
  taiMethod** ptr_to_im = (taiMethod **) &(md->im);
  while(cur_im != NULL) {
    if(bid < cur_im->bid) {
      ptr_to_im = cur_im->addr_sub_types();
      cur_im = cur_im->sub_types();
    }
    else {
      m_sub_types = cur_im;
      cur_im = NULL;
    }
  }
  *ptr_to_im = this;
}

taiMethodData* taiMethod::GetMethodRep(void* base, IDataHost* host_, taiData* par, 
  QWidget* gui_parent_) 
{
  return GetMethodRep_impl(base, host_, par, gui_parent_, 0); //TODO: may need to do more with the flags
}


/////////////////////////////
//   taiButtonMethod     //
/////////////////////////////

int taiButtonMethod::BidForMethod(MethodDef* md, TypeDef* td) {
  if(md->HasOption("BUTTON"))
    return (taiMethod::BidForMethod(md,td) + 1);
  return 0;
}

taiMethodData* taiButtonMethod::GetMethodRep_impl(void* base, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  taiMethButton* rval = new taiMethButton(base, meth, typ, host_, par, gui_parent_, flags_);
  return rval;
}


/////////////////////////////
//     taiMenuMethod     //
/////////////////////////////

int taiMenuMethod::BidForMethod(MethodDef* md, TypeDef* td) {
  if ((md->HasOption("MENU")) || (md->HasOption("MENU_BUTTON")))
    return (taiMethod::BidForMethod(md,td) + 1);
  return 0;
}

taiMethodData* taiMenuMethod::GetMethodRep_impl(void* base, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  taiMethMenu* rval = new taiMethMenu(base, meth, typ, host_, par, gui_parent_, flags_);
  return rval;
}

//////////////////////////
// 	taiArgType	//
//////////////////////////

taiArgType::taiArgType(int aidx, TypeDef* argt, MethodDef* mb, TypeDef* td)
: taiType(td) {
  meth = mb;
  arg_idx = aidx;
  arg_typ = argt;
  err_flag = false;

  arg_base = NULL;
  arg_val = NULL;
  use_it = NULL;
  obj_inst = NULL;
}

taiArgType::taiArgType() {
  meth = NULL;
  arg_idx = -1;
  arg_typ = NULL;
  err_flag = false;

  arg_base = NULL;
  arg_val = NULL;
  use_it = NULL;
  obj_inst = NULL;
}

taiArgType::~taiArgType() {
  if (use_it != NULL) {
    delete use_it;
    use_it = NULL;
  }
  if (obj_inst != NULL) {
    delete obj_inst;
    obj_inst = NULL;
  }
}

void taiArgType::GetImage_impl(taiData* dat, const void* base) {
  if (arg_base == NULL)  return;

  if (arg_val && GetHasOption("ARG_VAL_FM_FUN")) {
    Variant val = ((taBase*)base)->GetGuiArgVal(meth->name, arg_idx);
    if(val != _nilVariant) {
      *arg_val = val;		// use css conversion code!
    }
  }

  if(use_it != NULL)
    use_it->GetImage(dat, arg_base);
  else
    arg_typ->it->GetImage(dat, arg_base);
}

void taiArgType::GetValue_impl(taiData* dat, void*) {
  if (arg_base == NULL) return;

  if (use_it)
    use_it->GetValue(dat, arg_base);
  else
    arg_typ->it->GetValue(dat, arg_base);
}

taiData* taiArgType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  if (arg_base == NULL) return NULL;

  if (use_it)
    return use_it->GetDataRep(host_, par, gui_parent_, NULL, flags_);
  else
    return arg_typ->it->GetDataRep(host_, par, gui_parent_, NULL, flags_);
}

cssEl* taiArgType::GetElFromArg(const char* nm, void*) {
  if(arg_typ->ptr == 0) {
    /* type notes:
      explicitly signed/unsigned chars are treated as numbers, whereas char is a char
      current gui stuff can't handle uints well, so we lump them with variants
    */
    if (arg_typ->DerivesFrom(TA_int)  ||
      arg_typ->DerivesFrom(TA_short) || arg_typ->DerivesFrom(TA_unsigned_short) ||
      arg_typ->DerivesFrom(TA_signed_char) || arg_typ->DerivesFrom(TA_unsigned_char) 
    ) {
      arg_typ = &TA_int;
      arg_val = new cssInt(0, nm);
      arg_base = (void*)&(((cssInt*)arg_val)->val);
      return arg_val;
    } else if (arg_typ->DerivesFrom(TA_int64_t) || arg_typ->DerivesFrom(TA_uint64_t)
      || arg_typ->DerivesFrom(TA_unsigned_int)
    ) {
      arg_typ = &TA_Variant;
      arg_val = new cssVariant(Variant(0LL), nm);
      arg_base = (void*)&(((cssVariant*)arg_val)->val);
      use_it = new taiType(arg_typ); // note: only use the vanilla string field gui, not the Variant gui
      return arg_val;
    } else if (arg_typ->DerivesFrom(TA_bool)) {
      arg_val = new cssBool(false, nm);
      arg_base = (void*)&(((cssBool*)arg_val)->val);
      use_it = new taiBoolType(arg_typ); // make an it for it...
      return arg_val;
    } else if (arg_typ->DerivesFrom(TA_char)) {
      arg_typ = &TA_char;
      arg_val = new cssChar(0, nm);
      arg_base = (void*)&(((cssChar*)arg_val)->val);
      return arg_val;
    } else if (arg_typ->DerivesFrom(TA_float) || arg_typ->DerivesFrom(TA_double)) {
      arg_typ = &TA_double;
      arg_val = new cssReal(0, nm);
      arg_base = (void*)&(((cssReal*)arg_val)->val);
      return arg_val;
    } else if (arg_typ->DerivesFrom(TA_taString)) {
      arg_typ = &TA_taString;
      arg_val = new cssString("", nm);
      arg_base = (void*)&(((cssString*)arg_val)->val);
      return arg_val;
    } else if (arg_typ->DerivesFrom(TA_Variant)) {
      arg_typ = &TA_Variant;
      arg_val = new cssVariant(_nilVariant, nm);
      arg_base = (void*)&(((cssVariant*)arg_val)->val);
      return arg_val;
    } else if (arg_typ->DerivesFrom(TA_taBase)) {
      arg_typ = arg_typ->GetNonRefType()->GetNonConstType();
      if(arg_typ->GetInstance() == NULL) return NULL;
      obj_inst = ((TAPtr)arg_typ->GetInstance())->MakeToken();
      arg_val = new cssTA_Base(obj_inst, 1, arg_typ, nm);
      arg_base = obj_inst;
      return arg_val;
    } else if (arg_typ->DerivesFormal(TA_enum)) {
      arg_val = new cssEnum(0, nm);
      arg_base = (void*)&(((cssEnum*)arg_val)->val);
      return arg_val;
    }
    return NULL;
  }
  // ptr > 0 (probably 1)

  if(arg_typ->DerivesFrom(TA_char)) {
    arg_typ = &TA_taString;
    arg_val = new cssString("", nm);
    arg_base = (void*)&(((cssString*)arg_val)->val);
    return arg_val;
  }
  else if(arg_typ->DerivesFrom(TA_taBase)) {
    TypeDef* npt = arg_typ->GetNonRefType()->GetNonConstType()->GetNonPtrType();
    arg_val = new cssTA_Base(NULL, 1, npt, nm);
    arg_base = (void*)&(((cssTA*)arg_val)->ptr);
    return arg_val;
  }

  TypeDef* npt = arg_typ->GetNonRefType()->GetNonConstType()->GetNonPtrType();
  arg_val = new cssTA(NULL, 1, npt, nm);
  arg_base = (void*)&(((cssTA*)arg_val)->ptr);
  return arg_val;
}

bool taiArgType::GetHasOption(const String& opt, MethodDef* md, int aidx) {
  if(!md) md = meth;
  if(!md) return false;
  if(aidx < 0) aidx = arg_idx;
  String mb_nm = md->OptionAfter(opt + "_");
  if(!mb_nm.empty()) {
    if(isdigit(mb_nm.firstchar())) { // arg position indicator
      int ai = (int)String((char)mb_nm.firstchar());
      if(ai == aidx) return true;
      else return false;
    }
  }
  return md->HasOption(opt);
}

String taiArgType::GetOptionAfter(const String& opt, MethodDef* md, int aidx) {
  if(!md) md = meth;
  if(!md) return _nilString;
  if(aidx < 0) aidx = arg_idx;
  String mb_nm = md->OptionAfter(opt);
  if(!mb_nm.empty()) {
    if(isdigit(mb_nm.firstchar()) && (mb_nm[1] == '_')) { // arg position indicator
      int ai = (int)String((char)mb_nm.firstchar());
      if(ai == aidx) mb_nm = mb_nm.after(1); // use it
      else mb_nm = _nilString;			  // bail
    }
  }
  return mb_nm;
}

//////////////////////////////////
//       taiStreamArgType     //
//////////////////////////////////

void taiStreamArgType::Initialize() {
  gf = NULL;
}

void taiStreamArgType::Destroy() {
  if (gf) {
    gf->Close();
    taRefN::unRefDone(gf);
    gf = NULL;
  }
}

int taiStreamArgType::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if(argt->InheritsFrom(TA_ios))
    return taiArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* taiStreamArgType::GetElFromArg(const char* nm, void*) {
  // arg_val is for the function
  arg_val = new cssIOS(NULL, 1, arg_typ, nm);
  arg_base = (void*)&(((cssTA*)arg_val)->ptr);
  return arg_val;
}

taiData* taiStreamArgType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  if (!gf) {
    // we get and initialize the filer once
    gf = taBase::StatGetFiler(meth);
    taRefN::Ref(gf);
  }
  if (arg_typ->InheritsFrom(TA_istream))
    return new taiFileButton(NULL, host_, par, gui_parent_, flags_, true);
  else if(arg_typ->InheritsFrom(TA_ostream))
    return new taiFileButton(NULL, host_, par, gui_parent_, flags_, false, true);
  return new taiFileButton(NULL, host_, par, gui_parent_, flags_);
}

void taiStreamArgType::GetImage_impl(taiData* dat, const void* base){
  if (arg_base == NULL)
    return;
  taiFileButton* fbut = (taiFileButton*) dat;
  fbut->SetFiler(gf);
  fbut->GetImage();
}

void taiStreamArgType::GetValue_impl(taiData* dat, void*) {
  if (arg_base == NULL)
    return;
  GetValueFromGF();
}

void taiStreamArgType::GetValueFromGF() {
  if (arg_typ->InheritsFrom(TA_fstream)) {
    if (gf->fstrm == NULL) {
      *((void**)arg_base) = NULL;
      err_flag = true;		// error-value not set..
      return;
    }
    *((fstream**)arg_base) = gf->fstrm;
    return;
  }
  if (arg_typ->InheritsFrom(TA_istream)) {
    if(gf->istrm == NULL) {
      *((void**)arg_base) = NULL;
      err_flag = true;		// error-value not set..
      return;
    }
    *((istream**)arg_base) = gf->istrm;
    return;
  }
  if(arg_typ->InheritsFrom(TA_ostream)) {
    if (gf->ostrm == NULL) {
      *((void**)arg_base) = NULL;
      err_flag = true;		// error-value not set..
      return;
    }
    *((ostream**)arg_base) = gf->ostrm;
    return;
  }
}


//////////////////////////////////////////
//       taiTokenPtrArgType		//
//////////////////////////////////////////

int taiTokenPtrArgType::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if (td->InheritsFrom(TA_taBase) &&
     (argt->ptr == 1) && argt->DerivesFrom(TA_taBase))
    return taiArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* taiTokenPtrArgType::GetElFromArg(const char* nm, void*) {
  // arg_val is for the function
  TypeDef* npt = arg_typ->GetNonRefType()->GetNonConstType()->GetNonPtrType();
  arg_val = new cssTA_Base(NULL, 1, npt, nm);
  arg_base = (void*)&(((cssTA*)arg_val)->ptr);
  return arg_val;
}

taiData* taiTokenPtrArgType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  TypeDef* npt = arg_typ->GetNonRefType()->GetNonConstType()->GetNonPtrType();
  int token_flags = 0;
  if (GetHasOption("NULL_OK"))
    token_flags |= taiData::flgNullOk;
  if (GetHasOption("EDIT_OK"))
    token_flags |= taiData::flgEditOk;
  taiTokenPtrButton* rval = new taiTokenPtrButton(npt, host_, par, gui_parent_, token_flags);
  return rval;
//   taiToken* rval = new taiToken(taiMenu::buttonmenu, taiMisc::fonSmall, npt, host_, par, gui_parent_,
// 	token_flags);
  return rval;
}

void taiTokenPtrArgType::GetImage_impl(taiData* dat, const void* base){
  if(arg_base == NULL)
    return;
  TypeDef* npt = arg_typ->GetNonRefType()->GetNonConstType()->GetNonPtrType();
  String mb_nm = GetOptionAfter("TYPE_ON_");
  if(!mb_nm.empty()) {
    if(mb_nm == "this") {
      npt = ((TAPtr)base)->GetTypeDef(); // use object type
    }
    else {
      MemberDef* md = typ->members.FindName(mb_nm);
      if(md != NULL) {
	TypeDef* mbr_typ = (TypeDef*)*((void**)md->GetOff(base)); // set according to value of this member
	if(mbr_typ->InheritsFrom(npt) || npt->InheritsFrom(mbr_typ))
	  npt = mbr_typ;		// make sure this applies to this argument..
      }
    }
  }
  else {
    mb_nm = GetOptionAfter("TYPE_");
    if(!mb_nm.empty()) {	// check again..
      TypeDef* tmptd = taMisc::types.FindName(mb_nm);
      if(tmptd) npt = tmptd;
    }
  }
  taiTokenPtrButton* rval = (taiTokenPtrButton*)dat;
  //  taiToken* rval = (taiToken*)dat;
  TAPtr scope = NULL;
  if(GetHasOption("NO_SCOPE"))
    scope = NULL;
  else if((rval->host != NULL) && (rval->host->GetBaseTypeDef() != NULL) &&
	  (rval->host->GetBaseTypeDef()->InheritsFrom(TA_taBase)))
    scope = (TAPtr)(rval->host)->Base();
  else
    scope = (TAPtr)base;
  TypeDef* scope_type = NULL;
  String sctyp = GetOptionAfter("SCOPE_");
  if(!sctyp.empty()) {
    scope_type = taMisc::types.FindName(sctyp);
  }
  String nulltxt = GetOptionAfter("NULL_TEXT_");
  if(nulltxt.nonempty()) {
    taMisc::SpaceLabel(nulltxt);
    rval->setNullText(nulltxt);
  }
  if (GetHasOption("ARG_VAL_FM_FUN")) {
    Variant val = ((taBase*)base)->GetGuiArgVal(meth->name, arg_idx);
    if(val != _nilVariant) {
      *((TAPtr*)arg_base) = val.toBase();
    }
  }
  rval->GetImage(*((TAPtr*)arg_base), npt, scope, scope_type);
}

void taiTokenPtrArgType::GetValue_impl(taiData* dat, void*) {
  if(arg_base == NULL)
    return;
  taiTokenPtrButton* rval = (taiTokenPtrButton*)dat;
//   taiToken* rval = (taiToken*)dat;
// since it is an arg type, its a ptr to a css ptr, don't set it..
//   if(npt->DerivesFrom(TA_taBase))
//     taBase::SetPointer((TAPtr*)arg_base, (TAPtr)rval->GetValue());
//   else
  *((void**)arg_base) = rval->GetValue();
}

///////////////////////////////////
//       taiTypePtrArgType     //
///////////////////////////////////

int taiTypePtrArgType::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if (argt->DerivesFrom(TA_TypeDef) && (argt->ptr == 1))
    return taiArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}


cssEl* taiTypePtrArgType::GetElFromArg(const char* nm, void* base) {
  String mb_nm = GetOptionAfter("TYPE_ON_");
  if (mb_nm != "") {
    TypeDef* tpdf = NULL;
    if (mb_nm == "this") {
      tpdf = typ;
      if (typ->InheritsFrom(&TA_taBase) && (base != NULL))
	tpdf = ((TAPtr)base)->GetTypeDef();
    } else {
      MemberDef* md = typ->members.FindName(mb_nm);
      if ((md != NULL) && (md->type == &TA_TypeDef_ptr)) {
	tpdf = *(TypeDef**)(md->GetOff(base));
      }
    }
    if(tpdf) {
      arg_val = new cssTypeDef(tpdf, 1, &TA_TypeDef, nm);
      arg_base = (void*)&(((cssTA*)arg_val)->ptr);
      return arg_val;
    }
  } else {
    mb_nm = GetOptionAfter("TYPE_");
    if(mb_nm != "") {
      TypeDef* tpdf;
      if (mb_nm == "this") {
	tpdf = typ;
	if (typ->InheritsFrom(&TA_taBase) && (base != NULL))
	  tpdf = ((TAPtr)base)->GetTypeDef();
      } else {
	tpdf = taMisc::types.FindName(mb_nm);
      }
      if (tpdf == NULL)
	tpdf = &TA_taBase;
      arg_val = new cssTypeDef(tpdf, 1, &TA_TypeDef, nm);
      arg_base = (void*)&(((cssTA*)arg_val)->ptr);
      return arg_val;
    }
  }
  arg_val = new cssTypeDef(&TA_taBase, 1, &TA_TypeDef, nm);
  arg_base = (void*)&(((cssTA*)arg_val)->ptr);
  return arg_val;
}

taiData* taiTypePtrArgType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  int flags = flags_;
  if (GetHasOption("NULL_OK"))
    flags |= taiData::flgNullOk;
  
  TypeDef* init_typ = &TA_taBase;
  if (*((TypeDef**)arg_base) != NULL)
    init_typ = *((TypeDef**)arg_base);
  taiTypeDefButton* rval = new taiTypeDefButton(init_typ, host_, par,
    gui_parent_, flags);
  return rval;
}

void taiTypePtrArgType::GetImage_impl(taiData* dat, const void* base) {
  if (arg_base == NULL)
    return;
  taiTypeDefButton* rval = (taiTypeDefButton*)dat;
  String nulltxt = GetOptionAfter("NULL_TEXT_");
  if(nulltxt.nonempty()) {
    taMisc::SpaceLabel(nulltxt);
    rval->setNullText(nulltxt);
  }
  if (GetHasOption("ARG_VAL_FM_FUN")) {
    Variant val = ((taBase*)base)->GetGuiArgVal(meth->name, arg_idx);
    if(val != _nilVariant) {// must be a string..
      TypeDef* tdlkup = taMisc::types.FindName(val.toString());
      *((TypeDef**)arg_base) = tdlkup;
    }
  }
  TypeDef* typ_ = (TypeDef*)*((void**)arg_base);
  rval->GetImage(typ_, typ_);
}

void taiTypePtrArgType::GetValue_impl(taiData* dat, void*) {
  if (arg_base == NULL)
    return;
  taiTypeDefButton* rval = (taiTypeDefButton*)dat;
  *((void**)arg_base) = rval->GetValue();
}


///////////////////////////////////
//       taiMemberPtrArgType     //
///////////////////////////////////

int taiMemberPtrArgType::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if (argt->DerivesFrom(TA_MemberDef) && (argt->ptr == 1))
    return taiArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}


cssEl* taiMemberPtrArgType::GetElFromArg(const char* nm, void*) {
  arg_val = new cssMemberDef(NULL, 1, &TA_MemberDef, nm);
  arg_base = (void*)&(((cssTA*)arg_val)->ptr);
  return arg_val;
}

taiData* taiMemberPtrArgType::GetDataRep_impl(IDataHost* host_, taiData* par, 
  QWidget* gui_parent_, int flags_) 
{
  taiMemberDefButton* rval = new taiMemberDefButton(typ, host_, par, gui_parent_, flags_);
  return rval;
}

void taiMemberPtrArgType::GetImage_impl(taiData* dat, const void* base) {
  if(arg_base == NULL)
    return;
  taiMemberDefButton* rval = (taiMemberDefButton*)dat;
  MemberDef* md = (MemberDef*)*((void**)arg_base);
  rval->GetImage(md, typ); 
}

void taiMemberPtrArgType::GetValue_impl(taiData* dat, void*) {
  if (arg_base == NULL)
    return;
  taiMemberDefButton* rval = (taiMemberDefButton*)dat;
  *((MemberDef**)arg_base) = rval->GetValue();
}


///////////////////////////////////
//       taiMethodPtrArgType     //
///////////////////////////////////

int taiMethodPtrArgType::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if(argt->DerivesFrom(TA_MethodDef) && (argt->ptr == 1))
    return taiArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}


cssEl* taiMethodPtrArgType::GetElFromArg(const char* nm, void*) {
  arg_val = new cssMethodDef(NULL, 1, &TA_MethodDef, nm);
  arg_base = (void*)&(((cssTA*)arg_val)->ptr);
  return arg_val;
}

taiData* taiMethodPtrArgType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  MethodDef* init_md = typ->methods.FindName("Load");
  if (*((MethodDef**)arg_base) != NULL)
    init_md = *((MethodDef**)arg_base);
  taiMethodDefMenu* rval = new taiMethodDefMenu(taiMenu::buttonmenu, taiMisc::fonSmall,
	init_md, NULL, typ, host_, par, gui_parent_, flags_);
  rval->GetMenu();
  return rval;
}

void taiMethodPtrArgType::GetImage_impl(taiData* dat, const void*) {
  if(arg_base == NULL)
    return;
  taiMethodDefMenu* rval = (taiMethodDefMenu*)dat;
  rval->md = (MethodDef*)*((void**)arg_base);
  rval->menubase = typ;
  rval->ta_actions->Reset();
  MethodSpace* mbs = &(typ->methods);
  for (int i = 0; i < mbs->size; ++i){
    MethodDef* mbd = mbs->FastEl(i);
    if (mbd->im == NULL) continue;
    if ((mbd->name == "Close") || (mbd->name == "DuplicateMe") || (mbd->name == "ChangeMyType")
       || (mbd->name == "SelectForEdit") || (mbd->name == "SelectFunForEdit")
       || (mbd->name == "Help"))
      continue;
    rval->ta_actions->AddItem(mbd->GetLabel(), mbd);
  }
  MethodDef* initmd = (MethodDef*)*((void**)arg_base);
  if ((initmd != NULL) && typ->InheritsFrom(initmd->GetOwnerType()))
    rval->ta_actions->GetImageByData(Variant((void*)initmd));
  else
    rval->ta_actions->GetImageByIndex(0);	// just get first on list
}

void taiMethodPtrArgType::GetValue_impl(taiData* dat, void*) {
  if(arg_base == NULL)
    return;
  taiMethodDefMenu* rval = (taiMethodDefMenu*)dat;
  *((void**)arg_base) = rval->GetValue();
}




//////////////////////////////
//    taiDefaultEditDataHost  //
//////////////////////////////

// this special edit is for defualt instances
class taiDefaultEditDataHost : public taiEditDataHost {
INHERITED(taiEditDataHost)
public:
  override void	GetValue();

  MemberSpace   mspace;		// special copy of the mspace (added toggles)

//obs:  taiDefaultEditDataHost(TypeDef* tp, void * base);
  taiDefaultEditDataHost(void* base, TypeDef* typ_, bool read_only_ = false,
  	bool modal_ = false, QObject* parent = 0);
  ~taiDefaultEditDataHost();
protected:
  override void		Enum_Members(); // called by Constr_impl to fill memb_el[]
private:
  explicit taiDefaultEditDataHost(taiDefaultEditDataHost&)	{ }; // avoid copy constr bug
};

taiDefaultEditDataHost::taiDefaultEditDataHost(void* base, TypeDef* typ_, bool read_only_,
  	bool modal_, QObject* parent)
: taiEditDataHost(base, typ_, read_only_, modal_, parent)
{
}

taiDefaultEditDataHost::~taiDefaultEditDataHost(){
  for (int i = mspace.size - 1; i >= 0; --i) {
    MemberDef* md = mspace.FastEl(i);
    if (md->im)
      delete md->im;
    md->im = NULL;
  }
  mspace.Reset();
}

void taiDefaultEditDataHost::Enum_Members() {
  for (int i = 0; i < typ->members.size; ++i){
    MemberDef* md = new MemberDef(*(typ->members.FastEl(i)));
    mspace.Add(md);
    md->im = typ->members.FastEl(i)->im;	// set this here
    if (md->im != NULL) {
      taiTDefaultMember* tdm = new taiTDefaultMember(md, typ);
      tdm->bid = md->im->bid + 1;
      tdm->AddMember(md);
    }
    memb_el(0).Add(md);
  }
}

void taiDefaultEditDataHost::GetValue() {
  inherited::GetValue();
  if (typ->InheritsFrom(TA_taBase)) {
    TAPtr rbase = (TAPtr)cur_base;
    taBase_List* gp = typ->defaults;
    TypeDefault* tpdflt = NULL;
    if (gp != NULL) {
      for (int i = 0; i < gp->size; ++i) {
	TypeDefault* td = (TypeDefault*)gp->FastEl(i);
	if (td->token == rbase) {
	  tpdflt = td;
	  break;
	}
      }
    }
    if (tpdflt != NULL)
      tpdflt->UpdateToNameValue();
  }
}


////////////////////////////////
//  taiDefaultEdit    //
////////////////////////////////

taiEditDataHost* taiDefaultEdit::CreateDataHost(void* base, bool readonly) {
  return new taiDefaultEditDataHost(base, typ, readonly);
}


/*OBS
//////////////////////
// taiROListMember //
/////////////////////

int taiROListMember::BidForMember(MemberDef* md, TypeDef* td) {
  if ((md->HasOption("READ_ONLY") || md->HasOption("IV_READ_ONLY")) &&
     (td->InheritsFrom(TA_taList_impl)))
    return (taiROMember::BidForMember(md,td) + 1);
  return 0;
}

taiData* taiROListMember::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  gpiListEditButton *rval = new gpiListEditButton(NULL, typ, host_, par, gui_parent_, flags_);
  rval->read_only = true;
  return rval;
}

void taiROListMember::GetImage_impl(taiData* dat, const void* base){
  gpiListEditButton *rval = (gpiListEditButton*)dat;
  rval->GetImage_impl(base);
}

void taiROListMember::GetMbrValue(taiData*, void*, bool&) {
}


//////////////////////
// taiROGroupMember //
/////////////////////

int taiROGroupMember::BidForMember(MemberDef* md, TypeDef* td) {
  if ((md->HasOption("READ_ONLY") || md->HasOption("IV_READ_ONLY")) &&
     (td->InheritsFrom(TA_taGroup_impl)))
    return (taiROMember::BidForMember(md,td) + 1);
  return 0;
}

taiData* taiROGroupMember::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  gpiGroupEditButton *rval = new gpiGroupEditButton(NULL, typ, host_, par, gui_parent_, flags_);
  rval->read_only = true;
  return rval;
}

void taiROGroupMember::GetImage_impl(taiData* dat, const void* base){
  gpiGroupEditButton *rval = (gpiGroupEditButton*)dat;
  rval->GetImage_impl(base);
}

void taiROGroupMember::GetMbrValue(taiData*, void*, bool&) {
}



//////////////////////
// taiROArrayMember //
/////////////////////

int taiROArrayMember::BidForMember(MemberDef* md, TypeDef* td) {
  if ((md->HasOption("READ_ONLY") || md->HasOption("IV_READ_ONLY")) &&
     (td->InheritsFrom(TA_taArray)))
    return (taiROMember::BidForMember(md,td) + 1);
  return 0;
}

taiData* taiROArrayMember::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  gpiArrayEditButton *rval = new gpiArrayEditButton(NULL, typ, host_, par, gui_parent_, flags_);
  rval->read_only = true;
  return rval;
}

void taiROArrayMember::GetImage_impl(taiData* dat, const void* base){
  gpiArrayEditButton *rval = (gpiArrayEditButton*)dat;
  rval->GetImage_impl(base);
}

void taiROArrayMember::GetMbrValue(taiData*, void*, bool&) {
}

*/
//////////////////////////////////
// 	gpiDefaultEl		//
//////////////////////////////////

int gpiDefaultEl::BidForMember(MemberDef* md, TypeDef* td) {
  if ((md->name == "el_def") && (td->InheritsFrom(TA_taList_impl)))
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

taiData* gpiDefaultEl::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  gpiListEls *rval = new gpiListEls(taiMenu::buttonmenu, taiMisc::fonSmall,
	NULL, typ, host_, par, gui_parent_, (flags_ | taiData::flgNullOk | taiData::flgNoList));
  return rval;
}

void gpiDefaultEl::GetImage_impl(taiData* dat, const void* base) {
  taList_impl* tl = (taList_impl*)base;
  TAPtr tmp_ptr = tl->DefaultEl_();
  gpiListEls* rval = (gpiListEls*)dat;
  rval->GetImage((TAGPtr)base, tmp_ptr);
  GetOrigVal(dat, base);
}

void gpiDefaultEl::GetMbrValue(taiData* dat, void* base, bool& first_diff) {
  taList_impl* tl = (taList_impl*)base;
  gpiListEls* rval = (gpiListEls*)dat;
  TAPtr tmp_ptr = rval->GetValue();
  tl->SetDefaultEl(tmp_ptr);
  CmpOrigVal(dat, base, first_diff);
}


//////////////////////////////////
// 	gpiLinkGP		//
//////////////////////////////////

int gpiLinkGP::BidForMember(MemberDef* md, TypeDef* td) {
  if ((md->type->InheritsFrom(TA_taGroup_impl)) &&
     (md->HasOption("LINK_GROUP")))
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

taiData* gpiLinkGP::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  gpiLinkEditButton* rval = new gpiLinkEditButton(NULL, mbr->type, host_, par, gui_parent_, flags_);
  return rval;
}

void gpiLinkGP::GetImage_impl(taiData* dat, const void* base) {
  gpiLinkEditButton* rval = (gpiLinkEditButton*)dat;
  rval->GetImage_(mbr->GetOff(base));
}

void gpiLinkGP::GetMbrValue(taiData*, void*, bool&) {
}

//////////////////////////////////
// 	gpiLinkList		//
//////////////////////////////////

int gpiLinkList::BidForMember(MemberDef* md, TypeDef* td) {
  if ((md->type->InheritsFrom(TA_taList_impl)) &&
     (md->HasOption("LINK_GROUP")))
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

taiData* gpiLinkList::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  gpiListLinkEditButton* rval = new gpiListLinkEditButton(NULL, mbr->type, host_, par, gui_parent_, flags_);
  return rval;
}

void gpiLinkList::GetImage_impl(taiData* dat, const void* base) {
  gpiListLinkEditButton* rval = (gpiListLinkEditButton*)dat;
  rval->GetImage_(mbr->GetOff(base));
}

void gpiLinkList::GetMbrValue(taiData*, void*, bool&) {
}


//////////////////////////////////
// 	gpiFromGpTokenPtrMember	//
//////////////////////////////////

int gpiFromGpTokenPtrMember::BidForMember(MemberDef* md, TypeDef* td) {
  if (td->InheritsFrom(TA_taBase) && (md->type->ptr == 1)
      && md->type->DerivesFrom(TA_taBase) && md->OptionAfter("FROM_GROUP_").nonempty())
    return taiTokenPtrMember::BidForMember(md,td)+1;
  return 0;
}

taiData* gpiFromGpTokenPtrMember::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  MemberDef* from_md = GetFromMd();
  if(from_md == NULL)	return NULL;
  int new_flags = flags_;
  if(!mbr->HasOption("NO_NULL"))
    new_flags |= taiData::flgNullOk;
  if(!mbr->HasOption("NO_EDIT"))
    new_flags |= taiData::flgEditOk;
  if(!mbr->HasOption("GROUP_OPT_OK"))
    new_flags |= taiData::flgNoList;

  if (mbr->type->DerivesFrom(&TA_taGroup_impl))
    return new gpiSubGroups(taiMenu::buttonmenu, taiMisc::fonSmall, NULL, typ, host_, par, gui_parent_, new_flags);
  else if (from_md->type->DerivesFrom(TA_taGroup_impl))
    return new gpiGroupEls(taiMenu::buttonmenu, taiMisc::fonSmall, NULL,
		typ, host_, par, gui_parent_, (new_flags | taiData::flgNoInGroup));
  else
    return new gpiListEls(taiMenu::buttonmenu, taiMisc::fonSmall, NULL,
		typ, host_, par, gui_parent_, new_flags);
}

void gpiFromGpTokenPtrMember::GetImage_impl(taiData* dat, const void* base) {
  MemberDef* from_md = GetFromMd();
  if(from_md == NULL)	return;
  if (mbr->type->DerivesFrom(TA_taGroup_impl)) {
    gpiSubGroups* rval = (gpiSubGroups*)dat;
    TAGPtr lst = (TAGPtr)GetList(from_md, base);
    rval->GetImage(lst, *((TAGPtr*)mbr->GetOff(base)));
  } else {
    gpiListEls* rval = (gpiListEls*)dat;
    TABLPtr lst = GetList(from_md, base);
    rval->GetImage(lst, *((TAPtr*)mbr->GetOff(base)));
  }
  GetOrigVal(dat, base);
}

void gpiFromGpTokenPtrMember::GetMbrValue(taiData* dat, void* base, bool& first_diff) {
  if (mbr->type->DerivesFrom(&TA_taGroup_impl)) {
    gpiSubGroups* rval = (gpiSubGroups*)dat;
    taBase::SetPointer((TAPtr*)mbr->GetOff(base), (TAPtr)rval->GetValue());
  } else {
    gpiListEls* rval = (gpiListEls*)dat;
    taBase::SetPointer((TAPtr*)mbr->GetOff(base), (TAPtr)rval->GetValue());
  }
  CmpOrigVal(dat, base, first_diff);
}

MemberDef* gpiFromGpTokenPtrMember::GetFromMd() {
  String mb_nm = mbr->OptionAfter("FROM_GROUP_");
  MemberDef* from_md = NULL;
  if(mb_nm != "")
    from_md = typ->members.FindName(mb_nm);
  return from_md;
}

TABLPtr	gpiFromGpTokenPtrMember::GetList(MemberDef* from_md, const void* base) {
  if (from_md == NULL)
    return NULL;
  if(from_md->type->ptr == 1)
    return *((TABLPtr*)from_md->GetOff(base));
  else
    return (TABLPtr)from_md->GetOff(base);
}


//////////////////////////////////
//        gpiTAPtrArgType     //
//////////////////////////////////

int gpiTAPtrArgType::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if (td->InheritsFrom(TA_taList_impl) &&
     (argt->ptr == 1) && argt->DerivesFrom(TA_taBase))
    return taiTokenPtrArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* gpiTAPtrArgType::GetElFromArg(const char* nm, void* base) {
  TABLPtr lst = (TABLPtr)base;
  if ((lst != NULL) &&
     (arg_typ->DerivesFrom(lst->el_base) || lst->el_base->DerivesFrom(arg_typ->GetNonPtrType()))) {
    String ptrnm = lst->el_base->name + "_ptr";
    TypeDef* ntd = taMisc::types.FindName(ptrnm);
    if (ntd != NULL)
      arg_typ = ntd;	// search in el_base (if args are compatible)
  }
  return taiTokenPtrArgType::GetElFromArg(nm,base);
}


//////////////////////////////////
//        gpiInObjArgType     //
//////////////////////////////////

int gpiInObjArgType::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if (td->InheritsFrom(TA_taList_impl) &&
     (argt->ptr == 1) && argt->DerivesFrom(TA_taBase) && (md->HasOption("ARG_ON_OBJ")))
    return gpiTAPtrArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* gpiInObjArgType::GetElFromArg(const char* nm, void* base) {
  TABLPtr lst = (TABLPtr)base;
  TypeDef* npt = arg_typ->GetNonRefType()->GetNonConstType()->GetNonPtrType();
  if (lst != NULL)
    arg_val = new cssTA_Base(lst->DefaultEl_(), 1, npt, nm);
  else
    arg_val = new cssTA_Base(NULL, 1, npt, nm);

  arg_base = (void*)&(((cssTA_Base*)arg_val)->ptr);
  return arg_val;
}

taiData* gpiInObjArgType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  int new_flags = (flags_ & ~taiData::flgNoList); //note: exclude flgNoList
  if (GetHasOption("NULL_OK"))
    new_flags |= taiData::flgNullOk;
/*nn  if (GetHasOption("EDIT_OK"))
    new_flags |= taiData::flgEditOk; */
  if (typ->InheritsFrom(TA_taGroup_impl))
    return new taiGroupElsButton(typ, host_, par, gui_parent_,
      (new_flags | taiData::flgNoInGroup));
  else
    return new taiListElsButton(typ, host_, par, gui_parent_, new_flags);
}

void gpiInObjArgType::GetImage_impl(taiData* dat, const void* base) {
  if (arg_base == NULL)
    return;
  if (GetHasOption("ARG_VAL_FM_FUN")) {
    Variant val = ((taBase*)base)->GetGuiArgVal(meth->name, arg_idx);
    if(val != _nilVariant) {
      *((TAPtr*)arg_base) = val.toBase();
    }
  }
  if (typ->InheritsFrom(TA_taGroup_impl)) {
    taiGroupElsButton* els = (taiGroupElsButton*)dat;
    els->GetImage((taGroup_impl*)base, *((TAPtr*)arg_base));
  } else {
    taiListElsButton* els = (taiListElsButton*)dat;
    els->GetImage((TABLPtr)base, *((TAPtr*)arg_base));
  }
}

void gpiInObjArgType::GetValue_impl(taiData* dat, void*) {
  if (arg_base == NULL)
    return;
  //note: GetValue is not modal
  taiListElsButtonBase* els = (taiListElsButtonBase*)dat;
  *((TAPtr*)arg_base) = els->GetValue();
}

//////////////////////////////////
//        gpiFromGpArgType    //
//////////////////////////////////

int gpiFromGpArgType::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if ((argt->ptr != 1) || !argt->DerivesFrom(TA_taBase))
    return 0;
  String fmgp = GetOptionAfter("FROM_GROUP_", md, aidx);
  if (fmgp.empty()) return 0;
  return taiTokenPtrArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* gpiFromGpArgType::GetElFromArg(const char* nm, void* base) {
  MemberDef* from_md = GetFromMd();
  if(from_md == NULL)	return NULL;
  TABLPtr lst = GetList(from_md, base);
  TypeDef* npt = arg_typ->GetNonRefType()->GetNonConstType()->GetNonPtrType();
  if (lst != NULL)
    arg_val = new cssTA_Base(lst->DefaultEl_(), 1, npt, nm);
  else
    arg_val = new cssTA_Base(NULL, 1, npt, nm);

  arg_base = (void*)&(((cssTA_Base*)arg_val)->ptr);
  return arg_val;
}

taiData* gpiFromGpArgType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  MemberDef* from_md = GetFromMd();
  if(from_md == NULL)	return NULL;
  int new_flags = flags_;
  if (GetHasOption("NULL_OK"))
    new_flags |= taiData::flgNullOk;
  if (GetHasOption("EDIT_OK"))
    new_flags |= taiData::flgEditOk;

  if (GetHasOption("NO_GROUP_OPT"))
    new_flags |= taiData::flgNoGroup; //aka flagNoList

  if (from_md->type->DerivesFrom(TA_taGroup_impl))
     return new taiGroupElsButton(typ, host_, par, gui_parent_,
				  (new_flags | taiData::flgNoInGroup));
  else
    return new taiListElsButton(typ, host_, par, gui_parent_, new_flags);
}

void gpiFromGpArgType::GetImage_impl(taiData* dat, const void* base) {
  if (arg_base == NULL)  return;
  if (GetHasOption("ARG_VAL_FM_FUN")) {
    Variant val = ((taBase*)base)->GetGuiArgVal(meth->name, arg_idx);
    if(val != _nilVariant) {
      *((TAPtr*)arg_base) = val.toBase();
    }
  }
  MemberDef* from_md = GetFromMd();
  if (from_md == NULL)	return;
  TABLPtr lst = GetList(from_md, base);
  if (typ->InheritsFrom(TA_taGroup_impl)) {
    taiGroupElsButton* els = (taiGroupElsButton*)dat;
    els->GetImage((taGroup_impl*)lst, *((TAPtr*)arg_base));
  } else {
    taiListElsButton* els = (taiListElsButton*)dat;
    els->GetImage((TABLPtr)lst, *((TAPtr*)arg_base));
  }
}

void gpiFromGpArgType::GetValue_impl(taiData* dat, void*) {
  if (arg_base == NULL)
    return;
  taiListElsButtonBase* els = (taiListElsButtonBase*)dat;
  *((TAPtr*)arg_base) = els->GetValue();
}

MemberDef* gpiFromGpArgType::GetFromMd() {
  MemberDef* from_md = NULL;
  String mb_nm = GetOptionAfter("FROM_GROUP_");
  if (!mb_nm.empty()) {
    from_md = typ->members.FindName(mb_nm);
  }
  return from_md;
}

TABLPtr	gpiFromGpArgType::GetList(MemberDef* from_md, const void* base) {
  if (from_md == NULL)
    return NULL;
  if(from_md->type->ptr == 1)
    return *((TABLPtr*)from_md->GetOff(base));
  else
    return (TABLPtr)from_md->GetOff(base);
}


//////////////////////////////////
// 	gpiListEdit		//
//////////////////////////////////

int gpiListEdit::BidForEdit(TypeDef* td) {
  if (td->InheritsFrom(TA_taList_impl))
    return taiEdit::BidForEdit(td)+1;
  return 0;
}

taiEditDataHost* gpiListEdit::CreateDataHost(void* base, bool readonly) {
  // compact is either specified explicitly, 
  // or we must use it if the base_type of the list requires inline
  bool use_compact = false; 
  if (!typ->HasOption("NO_CHILDREN_INLINE")) {
    use_compact = typ->HasOption("CHILDREN_INLINE");
    if (!use_compact && base) { // try checking base type
      taList_impl* lst = (taList_impl*)base;
      use_compact = lst->el_typ->it->requiresInline();
    }
  }
  if (use_compact)
    return new gpiCompactListDataHost(base, typ, readonly);
  else 
    return new gpiListDataHost(base, typ, readonly);
}

//////////////////////////////////
// 	gpiArrayEdit		//
//////////////////////////////////

int gpiArrayEdit::BidForEdit(TypeDef* td){
  if (td->InheritsFrom(TA_taArray))
    return (taiEdit::BidForType(td) +1);
  return 0;
}

taiEditDataHost* gpiArrayEdit::CreateDataHost(void* base, bool readonly) {
  return new gpiArrayEditDataHost(base, typ, readonly);
}

//////////////////////////
// 	SArgEdit	//
//////////////////////////

int SArgEdit::BidForEdit(TypeDef* td){
  if(td->InheritsFrom(TA_SArg_Array))
    return (gpiArrayEdit::BidForType(td) +1);
  return 0;
}

taiEditDataHost* SArgEdit::CreateDataHost(void* base, bool readonly) {
  return new SArgEditDataHost(base, typ, readonly);
}

//////////////////////////////////
//	gpiSelectEdit		//
//////////////////////////////////

int gpiSelectEdit::BidForEdit(TypeDef* td) {
  if (td->InheritsFrom(TA_SelectEdit))
    return taiEdit::BidForEdit(td)+1;
  return 0;
}

taiEditDataHost* gpiSelectEdit::CreateDataHost(void* base, bool readonly) {
  return new gpiSelectEditDataHost(base, typ, readonly);
}


//////////////////////////
//   taiViewType	//
//////////////////////////

taiDataLink* taiViewType::StatGetDataLink(void* el, TypeDef* el_typ) {
  if (!el || !el_typ) return NULL; // subclass will have to grok

  // handle ptrs by derefing the type and the el
  if (el_typ->ptr > 0) {
    int ptr = el_typ->ptr; // need to deref this many times
    el_typ = el_typ->GetNonPtrType(); // gets base type in one step
    while (el && ptr) {
      el = *((void**)el);
      --ptr;
    }
  }

  if (!el || !el_typ) return NULL;
  if (!el_typ->iv) return NULL;
  taiDataLink* rval = NULL;
  taiViewType* tiv = el_typ->iv;
  rval = tiv->GetDataLink(el, el_typ);
  return rval; //NULL if not taBase
}


void taiViewType::Initialize() {
  m_dp = NULL;
  m_dps = NULL;
  m_need_set = false;
}

void taiViewType::AddView(TypeDef* td) {
  taiViewType* cur_iv = td->iv;		// the current it
  taiViewType** ptr_to_iv = &(td->iv);	// were to put one
  while (cur_iv != NULL) {
    if (bid < cur_iv->bid) {		// we are lower than current
      ptr_to_iv = cur_iv->addr_sub_types();	// put us on its sub_types
      cur_iv = cur_iv->sub_types();	// and compare to current sub_type
    } else {
      m_sub_types = cur_iv;		// we are higher, current is our sub
      cur_iv = NULL;			// and we are done
    }
  }
  *ptr_to_iv = this;			// put us here
}

iDataPanel* taiViewType::CreateDataPanel(taiDataLink* dl_) {
  m_dp = NULL;
  m_dps = NULL;
  m_need_set = false;
  CreateDataPanel_impl(dl_);
  if (m_dps) {
      m_dps->AllSubPanelsAdded();
      m_dps->setCurrentPanelId(0);
      return m_dps;
  } else return m_dp;
}

void taiViewType::DataPanelCreated(iDataPanelFrame* dp) {
  // we will need to create a set if > 1 panel, or 1st-only uses minibar
  m_need_set = m_need_set || (m_dp != NULL) || dp->hasMinibarCtrls();
  if (!m_dp) {
    m_dp = dp;
    if (!needSet()) return; // only applies to first guy
  }
  // using or need a set
  if (!m_dps) {
//     // note: use first link for set, in case, ex. this is a Doc panel, which is nonstandard
    m_dps = new iDataPanelSet(m_dp->link());
    m_dps->AddSubPanel(m_dp);
    // if adding first because of minibar, don't add again
    if (m_dp == dp) return;
  }
  m_dps->AddSubPanel(dp);
}



//////////////////////////
//   tabViewType 	//
//////////////////////////

int tabViewType::BidForView(TypeDef* td) {
  return 0; //no taBase implementation for now
}

taiDataLink* tabViewType::CreateDataLink_impl(taBase* data_) {
  return NULL; //no taBase implementation for now
}

iDataPanel* tabViewType::CreateDataPanel(taiDataLink* dl_) {
  iDataPanel* rval = inherited::CreateDataPanel(dl_);
  // if more than one panel, then move the edit menu and methods to outside panelset
  if (m_dps) {
    taiEditDataHost* edh = edit_panel->editDataHost();
    // move the menu -- note: QMenu on linux/win, QToolBar on mac
    m_dps->SetMenu(edh->menu->GetRep());
    m_dps->SetMethodBox(edh->frmMethButtons);
    edh->frmMethButtons->setVisible(edh->showMethButtons());
  }
  return rval;
}

void tabViewType::CreateDataPanel_impl(taiDataLink* dl)
{
  //NOTE: tabListViewType calls this directly to get the property panel --
  // if this method is changed substantially, make sure to check tabListViewType, and change
  // if necessary
  TypeDef* td = dl->GetDataTypeDef();
  taiEdit* taie = td->ie;
  //TODO: need to determine read_only
  //note: we cache this panel for the menu/method box fixup step
  // note: prob don't need this color nonsense, could just use defs, which does the same thing
  bool ok;
  const iColor bg = GetEditColorInherit(dl, ok);
  if (ok)
    edit_panel = taie->EditNewPanel(dl, dl->data(), false, bg);
  else 
    edit_panel = taie->EditNewPanel(dl, dl->data(), false);
  DataPanelCreated(edit_panel);
}

taiDataLink* tabViewType::GetDataLink(void* data_, TypeDef* el_typ) {
  //NOTE: replaced in tabOViewType with an optimized version
  taBase* data = (taBase*)data_;
  taDataLink* dl = data->data_link();
  if (dl) return (taiDataLink*)dl;
  else return CreateDataLink_impl(data);
}

const iColor tabViewType::GetEditColorInherit(taiDataLink* dl, bool& ok) const {
  return ((taBase*)dl->data())->GetEditColorInherit(ok);
}


//////////////////////////
//   tabOViewType 	//
//////////////////////////

int tabOViewType::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_taOBase))
    return (inherited::BidForView(td) +1);
  return 0;
}

void tabOViewType::CheckUpdateDataPanelSet(iDataPanelSet* pan) {
  // TODO (4.1): check for new UserData
  // TODO: check for a new DocLink
  // if we have UserData, make a panel for it
  taiDataLink* dl = pan->link();
  if (!dl) return;
  taOBase* tab = dynamic_cast<taOBase*>(dl->taData());
  if (!tab) return; // shouldn't happen
  UserDataItem_List* udl = tab->GetUserDataList(false); // no force
  if (udl) { // note: if not, can't have DocLink either...
    /* TODO: 4.0release or 4.1: so make the panel
    iUserDataPanel* udp = new iUserDataPanel((taiDataLink*)udl->GetDataLink());
    DataPanelCreated(udp);*/
    // if we have a DocLink, make a panel for it
    // if the content changes or it gets deleted, the panel updates
    // if we set a new Doc or null it out, we get a USER_DATA_UPDATED notify, and reset it
    taDoc* doc = tab->GetDocLink();
    // get an existing DocPanel, if any -- don't force yet...
    int start_idx = 0;
    iDocDataPanel* dp = (iDocDataPanel*)pan->GetDataPanelOfType(&TA_iDocDataPanel, start_idx);
    if (doc) {
      if (!dp) { // need to create one
        dp = new iDocDataPanel; // note special case: no ctor link
        DataPanelCreated(dp);
      }
      dp->setDoc(doc); // sets link
    } else {
      if (dp) {
        dp->setDoc(NULL);
      }
    }
  }
}

taiDataLink* tabOViewType::CreateDataLink_impl(taBase* data_) {
  return new tabODataLink((taOBase*)data_);
}

void tabOViewType::CreateDataPanel_impl(taiDataLink* dl_)
{
  // if we have defchildren, make a list panel
  // notes:
  // 1. taList will never have defchildren, so it will make the panel for itself
  // 2. for taOBase guys, the list goes second, since the obj is germane
  inherited::CreateDataPanel_impl(dl_);
  if (typ && typ->OptionAfter("DEF_CHILD_").nonempty()) {
    String custom_name = typ->OptionAfter("DEF_CHILDNAME_"); // optional
    custom_name.gsub("_", " ");
    dl_ = dynamic_cast<tabODataLink*>(dl_)->listLink();
    iListDataPanel* bldp = new iListDataPanel(dl_, custom_name);
    DataPanelCreated(bldp);
  }
  // if we have UserData, make a panel for it
  taOBase* tab = dynamic_cast<taOBase*>(dl_->taData());
  if (!tab) return; // shouldn't happen
  UserDataItem_List* udl = tab->GetUserDataList(false); // no force
  if (udl) { // note: if not, can't have DocLink either...
    /* TODO: 4.0release or 4.1: so make the panel
    iUserDataPanel* udp = new iUserDataPanel((taiDataLink*)udl->GetDataLink());
    DataPanelCreated(udp);*/
    // if we have a DocLink, make a panel for it
    // if the content changes or it gets deleted, the panel updates
    // if we set a new Doc or null it out, we get a USER_DATA_UPDATED notify, and reset it
    taDoc* doc = tab->GetDocLink();
    if (doc) {
      iDocDataPanel* dp = new iDocDataPanel; // note special case: no ctor link
      DataPanelCreated(dp);
      dp->setDoc(doc); // sets link
    }
  }
}


taiDataLink* tabOViewType::GetDataLink(void* data_, TypeDef* el_typ) {
  taOBase* data = (taOBase*)data_;
  taDataLink* dl = *(data->addr_data_link());
  if (dl) return (taiDataLink*)dl;
  else return CreateDataLink_impl(data);
}


//////////////////////////
//   tabListViewType 	//
//////////////////////////

int tabListViewType::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_taList_impl))
    return (inherited::BidForView(td) +1);
  return 0;
}

taiDataLink* tabListViewType::CreateDataLink_impl(taBase* data_) {
  return new tabListDataLink((taList_impl*)data_);
}

void tabListViewType::CreateDataPanel_impl(taiDataLink* dl_)
{
  // we create ours first, because it should be the default
  iListDataPanel* bldp = new iListDataPanel(dl_);
  DataPanelCreated(bldp);
  inherited::CreateDataPanel_impl(dl_);
}


//////////////////////////
//   tabGroupViewType 	//
//////////////////////////

int tabGroupViewType::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_taGroup_impl))
    return (inherited::BidForView(td) +1);
  return 0;
}

taiDataLink* tabGroupViewType::CreateDataLink_impl(taBase* data_) {
  return new tabGroupDataLink((taGroup_impl*)data_);
}



void taiTypeBase::InitializeTypes(bool gui) {
  int i,j,k;
  TypeDef* td;

  TypeSpace i_type_space;
  TypeSpace v_type_space;
  TypeSpace i_memb_space;
  TypeSpace i_meth_space;
  TypeSpace i_edit_space;

  for (i=0; i < taMisc::types.size; ++i) {
    td = taMisc::types.FastEl(i);

    // generate a list of all the qt types
    if (gui && td->InheritsFrom(TA_taiType) && (td->instance != NULL)
       && !(td->InheritsFrom(TA_taiMember) || td->InheritsFrom(TA_taiMethod) ||
	    td->InheritsFrom(TA_taiArgType) || td->InheritsFrom(TA_taiEdit)))
      i_type_space.Link(td);


    // generate a list of all the view types
    if (td->InheritsFrom(TA_taiViewType) && (td->instance != NULL))
      v_type_space.Link(td);

    // generate a list of all the member_i types
    if (gui && td->InheritsFrom(TA_taiMember) && (td->instance != NULL))
      i_memb_space.Link(td);

    // generate a list of all the method_i types
    if (gui && td->InheritsFrom(TA_taiMethod) && (td->instance != NULL))
      i_meth_space.Link(td);

    // generate a list of all the method arg types to be used later
    if (gui && td->InheritsFrom(TA_taiArgType) && (td->instance != NULL))
      taiMisc::arg_types.Link(td);

    // generate a list of all the ie types (edit dialogs)
    if (gui && td->InheritsFrom(TA_taiEdit) && (td->instance != NULL))
      i_edit_space.Link(td);
  }

  if (gui && (i_type_space.size == 0))
    taMisc::Error("taiInit: warning: no taiType's found with instance != NULL");
  if (gui && (i_memb_space.size == 0))
    taMisc::Error("taiInit: warning: no taiMembers's found with instance != NULL");
  if (gui && (i_edit_space.size == 0))
    taMisc::Error("taiInit: warning: no taiEdit's found with instance != NULL");

  // go through all the types and assign the highest bid for
  //   the it, iv, and ie
  int bid;

  for (i=0; i < taMisc::types.size; ++i){
    td = taMisc::types.FastEl(i);
    if (gui) for (j=0; j <i_type_space.size; ++j) {
      taiType* tit_i = (taiType*) i_type_space.FastEl(j)->GetInstance();
      if ((bid = tit_i->BidForType(td)) > 0) {
	taiType* tit = (taiType*)tit_i->TypInst(td); // make one
	tit->bid = bid;
	tit->AddToType(td);		// add it
      }
    }

    for (j=0; j < v_type_space.size; ++j) {
      taiViewType* tit_v = (taiViewType*) v_type_space.FastEl(j)->GetInstance();
      if ((bid = tit_v->BidForView(td)) > 0) {
	taiViewType* tit = (taiViewType*)tit_v->TypInst(td); // make one
	tit->bid = bid;
	tit->AddView(td);		// add it
      }
    }

    if (gui) for (j=0; j < i_edit_space.size; ++j) {
      taiEdit* tie_i = (taiEdit*) i_edit_space.FastEl(j)->GetInstance();
      if ((bid = tie_i->BidForEdit(td)) > 0) {
	taiEdit* tie = (taiEdit*)tie_i->TypInst(td);
	tie->bid = bid;
	tie->AddEdit(td);
      }
    }

    // go though all the types and find the ones that are classes
    // for each class type go through the members and assign
    // the highest bid for the member's it (may be based on opts field)
    // and do the enum types since they are not global and only on members

    if (gui && td->InheritsFormal(TA_class)) {
      for (j=0; j < td->members.size; ++j) {
	MemberDef* md = td->members.FastEl(j);
	if (md->owner->owner != td) continue; // if we do not own this mdef, skip
	for (k=0; k < i_memb_space.size; ++k){
	  taiMember* tim_i = (taiMember*) i_memb_space.FastEl(k)->GetInstance();
	  if ((bid = tim_i->BidForMember(md,td)) > 0 ) {
	    taiMember* tim = tim_i->MembInst(md,td);
	    tim->bid = bid;
	    tim->AddMember(md);
	  }
	}
      }

      for(j=0; j < td->sub_types.size; ++j){
	TypeDef* subt = td->sub_types.FastEl(j);
	for(k=0; k < i_type_space.size; ++k) {
	  taiType* tit_i = (taiType*) i_type_space.FastEl(k)->GetInstance();
	  if ((bid = tit_i->BidForType(subt)) > 0) {
	    taiType* tit = (taiType*)tit_i->TypInst(subt); // make one
	    tit->bid = bid;
	    tit->AddToType(subt);		// add it
	  }
	}
      }

      // only assign method im's to those methods that do better than the default
      // (which has a value of 0).  Thus, most methods don't generate a new object here

      for (j=0; j < td->methods.size; ++j) {
	MethodDef* md = td->methods.FastEl(j);
	if (md->owner->owner != td) continue; // if we do not own this mdef, skip
	for (k=0; k < i_meth_space.size; ++k){
	  taiMethod* tim_i = (taiMethod*) i_meth_space.FastEl(k)->GetInstance();
	  if ((bid = tim_i->BidForMethod(md,td)) > 0 ) {
	    taiMethod* tim = (taiMethod*)tim_i->MethInst(md,td);
	    tim->bid = bid;
	    tim->AddMethod(md);
	  }
	}
      }

    } // td->InheritsFormal(TA_class)

  } // for each Type
}

