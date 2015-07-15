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

#include "taHashTable.h"

///////////////////////////
//        Hash Table     //
///////////////////////////

// some of the following code (the hashing function and the prime number list)
// were taken from the COOL object library package, which has the following
// copyright notice:
//
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.

int taHashTable::n_bucket_primes[] =
                     {3, 7, 13, 19, 29, 41, 53, 67, 83, 97, 113, 137,
                      163, 191, 223, 263, 307, 349, 401, 461, 521,
                      653, 719, 773, 839, 911, 983, 1049, 1123, 1201,
                      1279, 1367, 1459, 1549, 1657, 1759, 1861, 1973,
                      2081, 2179, 2281, 2383, 2503, 2617, 2729, 2843,
                      2963, 3089, 3203, 3323, 3449, 3571, 3697, 3833,
                      3967, 4099, 4241, 4391, 4549, 4703, 4861, 5011,
                      5171, 5333, 5483, 5669, 5839, 6029, 6197, 6361,
                      6547, 6761, 6961, 7177, 7393, 7517, 7727, 7951,
                      8101, 8209, 16411, 32771, 65537, 131301, 262147,
                      524287};

int taHashTable::n_primes = 86;

void taHashTable::InitList_() {
  Alloc(3);
}

void taHashTable::AddHash(taHashVal hash, int val, const String& str) {
  if (size == 0) return;                // this shouldn't happen, but justin case..
  int buck_no = (int)(hash % size);
  taHashBucket* bucket = FastEl(buck_no);
  if(!bucket) {
    bucket = new taHashBucket();
    ReplaceIdx(buck_no, bucket);
  }
  bucket->Add(new taHashEl(hash, val, str));
  bucket_max = MAX(bucket_max, bucket->size);
}

bool taHashTable::Alloc(int sz) {
  Reset();                      // get rid of any existing ones
  bucket_max = 0;
  int act_sz = 0;
  int cnt = 0;
  while((cnt < n_primes) && (act_sz < sz))      act_sz = n_bucket_primes[cnt++];
  if(!taPtrList<taHashBucket>::Alloc(act_sz)) return false;
  int i;
  for(i=0; i<act_sz; i++)       // initialize with nulls
    AddOnly_(NULL);
  return true;
}

int taHashBucket::FindBucketIndex(taHashVal hash, const String& str) const {
  for(int i=0; i < size; i++) {
    taHashEl* hel = FastEl(i);
    if(hel->hash_code == hash && hel->hashed_str == str) { // nil strs always match..
      return i;
    }
  }
  return -1;
}

int taHashBucket::FindHashVal(taHashVal hash, const String& str) const {
  for(int i=0; i < size; i++) {
    taHashEl* hel = FastEl(i);
    if(hel->hash_code == hash && hel->hashed_str == str) { // nil strs always match..
      return FastEl(i)->value;
    }
  }
  return -1;
}

int taHashTable::FindHashVal(taHashVal hash, const String& str) const {
  if(size == 0) return -1;
  int buck_no = (int)(hash % size);
  taHashBucket* bucket = FastEl(buck_no);
  if(bucket == NULL) return -1;
  return bucket->FindHashVal(hash, str);
}

bool taHashTable::UpdateHashVal(taHashVal hash, int val, const String& str) {
  if(size == 0) return false;
  int buck_no = (int)(hash % size);
  taHashBucket* bucket = FastEl(buck_no);
  if(bucket == NULL) return false;
  int idx = bucket->FindBucketIndex(hash, str);
  if(idx < 0)    return false;
  bucket->FastEl(idx)->value = val;
  return true;
}

bool taHashTable::RemoveHash(taHashVal hash, const String& str) {
  if(size == 0) return false;
  int buck_no = (int)(hash % size);
  taHashBucket* bucket = FastEl(buck_no);
  if(bucket == NULL) return false;
  int idx = bucket->FindBucketIndex(hash, str);
  if(idx == -1)
    return false;
  return bucket->RemoveIdx(idx);
}

void taHashTable::RemoveAll() {
  taPtrList<taHashBucket>::RemoveAll();
  bucket_max = 0;
}
