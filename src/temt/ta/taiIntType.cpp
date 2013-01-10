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

#include "taiIntType.h"

int taiIntType::BidForType(TypeDef* td){
//NOTE: we can't properly handle uints, so we don't bid for them
// we left the handler code in the other routines, in case we implement them
  // we handle all numeric int types < 32 bits but NOT uint/ulong
  if (td->DerivesFrom(&TA_int)
      || td->DerivesFrom(&TA_short)
      || td->DerivesFrom(&TA_unsigned_short)
      || td->DerivesFrom(&TA_signed_char)
      || td->DerivesFrom(&TA_unsigned_char))
  {
    return (taiType::BidForType(td) +1);
  }
  else if (td->DerivesFrom(&TA_QAtomicInt))
  {
    // needs higher bid to overcome class
    return (taiType::BidForType(td) +10);
  }
  return 0;
}

taiData* taiIntType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_, MemberDef* mbr) {
//TODO: the taiIncrField control can only handle int values, so can't handle uint range
// should either replace with a DoubleSpin, or longlongspin
  taiIncrField* rval = new taiIncrField(typ, host_, par, gui_parent_, flags_);
  // put limits on values -- start w/ explicit ones, them limit them by datatype
  int min = INT_MIN;
  if (mbr && mbr->HasOption("POS_ONLY")) // do this one first, then max of min
    min = 0;
  String val;
  if (mbr) val = mbr->OptionAfter("MIN_");
  int it;
  // we are very conservative, and make sure the value can be converted
  if (val.nonempty() && val.isInt()) {
    it = val.toInt();
    min = MAX(min, it);
  }
  int max = INT_MAX;
  if (mbr) val = mbr->OptionAfter("MAX_");
  if (val.nonempty() && val.isInt()) {
    it = val.toInt();
    max = MIN(max, it);
  }
  if (typ->DerivesFrom(&TA_int) || typ->DerivesFrom(&TA_QAtomicInt)) {
    min = MAX(min, INT_MIN);
    max = MIN(max, INT_MAX);
//  }
//  else if (typ->DerivesFrom(&TA_unsigned_int)) {
//    min = 0;  max = INT_MAX;//NOTE: does not cover entire uint range
  }
  else if (typ->DerivesFrom(&TA_short)) {
    min = MAX(min, SHRT_MIN);
    max = MIN(max, SHRT_MAX);
  }
  else if (typ->DerivesFrom(&TA_unsigned_short)) {
    min = MAX(min, 0);
    max = MIN(max, USHRT_MAX);
  }
  else if (typ->DerivesFrom(&TA_signed_char)) {
    min = MAX(min, SCHAR_MIN);
    max = MIN(max, SCHAR_MAX);
  }
  else { //if typ->DerivesFrom(&TA_unsigned_char)
    min = MAX(min, 0);
    max = MIN(max, UCHAR_MAX);
  }
  rval->setMinimum(min);
  rval->setMaximum(max);
  return rval;
}

void taiIntType::GetImage_impl(taiData* dat, const void* base) {
  int val = 0;
  if (typ->DerivesFrom(&TA_int)) {
    val = *((int*)base);
  }
  else if (typ->DerivesFrom(&TA_unsigned_int)) {
    val = (int)*((uint*)base); //NOTE: overflow issue
  }
  else if (typ->DerivesFrom(&TA_short)) {
    val = (int)*((short*)base);
  }
  else if (typ->DerivesFrom(&TA_unsigned_short)) {
    val = (int)*((unsigned short*)base);
  }
  else if (typ->DerivesFrom(&TA_signed_char)) {
    val = (int)*((signed char*)base);
  }
  else if (typ->DerivesFrom(&TA_unsigned_char)) {
    val = (int)*((unsigned char*)base);
  }
  else if (typ->DerivesFrom(&TA_taAtomicInt)) {
    val = (int)*((taAtomicInt*)base);
  }
  else {
    // should never happen
  }

  taiIncrField* rval = (taiIncrField*)dat;
  rval->GetImage(val);
}

void taiIntType::GetValue_impl(taiData* dat, void* base) {
  taiIncrField* rval = (taiIncrField*)dat;
  int val = rval->GetValue();
  if (typ->DerivesFrom(&TA_int)) {
    *((int*)base) = val;
  }
  else if (typ->DerivesFrom(&TA_unsigned_int)) {
   *((uint*)base) = (uint)val; //NOTE: range issue
  }
  else if (typ->DerivesFrom(&TA_short)) {
    *((short*)base) = (short)val;
  }
  else if (typ->DerivesFrom(&TA_unsigned_short)) {
    *((unsigned short*)base) = (unsigned short)val;
  }
  else if (typ->DerivesFrom(&TA_signed_char)) {
    *((signed char*)base) = (signed char)val;
  }
  else if (typ->DerivesFrom(&TA_unsigned_char)) {
    *((unsigned char*)base) = (unsigned char)val;
  }
  else if (typ->DerivesFrom(&TA_QAtomicInt)) {
    *((QAtomicInt*)base) = val;
  }
  else {
    // should never happen
  }
}
