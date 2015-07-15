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

#ifndef taHashTable_h
#define taHashTable_h 1

// parent includes:
#include <taHashBucket>
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taHashTable);

class TA_API taHashTable : public taPtrList<taHashBucket> {
  // table has a number of buckets, each with some hash values
protected:
  void  El_Done_(void* it)      { delete (taHashBucket*)it; }
public:
  static int    n_bucket_primes[]; // prime numbers for number of buckets
  static int    n_primes;        // number of prime numbers (86)

  int           bucket_max;     // maximum size of any bucket
  KeyType       key_type;       // type of key to use by default (name/string or pointer)

  // allocate in prime-number increments
  bool Alloc(int sz) override;
  void RemoveAll() override;

  virtual int   FindHashVal(taHashVal hash, const String& str = _nilString) const;
  // find value associated with given hash code (-1 if not found)
  virtual int   FindHashValString(const String& str) const
  { return FindHashVal(taHashEl::HashCode_String(str), str); }
  // find value associated with given string (-1 if not found)
  virtual int   FindHashValPtr(const void* ptr) const
  { return FindHashVal(taHashEl::HashCode_Ptr(ptr)); }
  // find value associated with given pointer (-1 if not found)

  virtual void  AddHash(taHashVal hash, int val, const String& str = _nilString);
  // add a new item to the hash table
  virtual void  AddHashString(const String& str, int val)
  { AddHash(taHashEl::HashCode_String(str), val, str); }
  // add a new string item to the hash table
  virtual void  AddHashPtr(const void* ptr, int val)
  { AddHash(taHashEl::HashCode_Ptr(ptr), val); }
  // add a new pointer item to the hash table

  virtual bool  RemoveHash(taHashVal hash, const String& str = _nilString);
  // remove given hash code from table
  virtual bool  RemoveHashString(const String& str)
  { return RemoveHash(taHashEl::HashCode_String(str), str); }
  // remove given string from table
  virtual bool  RemoveHashPtr(const void* ptr)
  { return RemoveHash(taHashEl::HashCode_Ptr(ptr)); }
  // remove given pointer from table

  virtual bool  UpdateHashVal(taHashVal hash, int val, const String& str = _nilString);
  // update value associated with hash item
  virtual bool  UpdateHashValString(const String& str, int val)
  { return UpdateHashVal(taHashEl::HashCode_String(str), val, str); }
  // update value associated with string item
  virtual bool  UpdateHasValPtr(const void* ptr, int val)
  { return UpdateHashVal(taHashEl::HashCode_Ptr(ptr), val); }
  // update value associated with ptr item

  void          InitList_();
  taHashTable()                 { key_type = KT_NAME; InitList_(); }
  taHashTable(const taHashTable& cp)    { key_type = cp.key_type; InitList_(); Duplicate(cp); }
  ~taHashTable()                { Reset(); }
};

#endif // taHashTable_h
