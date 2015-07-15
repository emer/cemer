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

#ifndef taHashBucket_h
#define taHashBucket_h 1

// parent includes:
#include <taHashEl>
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taHashBucket);

class TA_API taHashBucket : public taPtrList<taHashEl> {
  // holds a set of hash table entries that all have the same hash_code modulo value
protected:
  void  El_Done_(void* it)      { delete (taHashEl*)it; }
public:
  virtual int   FindBucketIndex(taHashVal hash, const String& str = _nilString) const;
  // find index of item in the bucket with given hash code, and string if using
  virtual int   FindHashVal(taHashVal hash, const String& str = _nilString) const;
  // find hash value associated with given hash code, and string if using

  ~taHashBucket()               { Reset(); }
};

#endif // taHashBucket_h
