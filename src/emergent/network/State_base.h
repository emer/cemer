// Copyright 2017-2018, Regents of the University of Colorado,
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

// TA-like base classes for State implementation code
// included in appropriate specific context-setting code: State_core.h, cuda.h

#ifdef __MAKETA__
#pragma maketa_file_is_target NetworkState
#endif

// these are all the base State classes

class STATE_CLASS(NetworkState); //
class STATE_CLASS(LayerState); //
class STATE_CLASS(PrjnState); //
class STATE_CLASS(UnGpState); //
class STATE_CLASS(UnitState); //
class STATE_CLASS(ConState); // 

// and corresponding spec core classes that contain all the algorithms

class STATE_CLASS(LayerSpec); //
class STATE_CLASS(ProjectionSpec); //
class STATE_CLASS(UnitSpec); //
class STATE_CLASS(ConSpec); //

class STATE_CLASS(taBase) {
  // ##NO_TOKENS ##INLINE base class for core
public:
};

class STATE_CLASS(taOBase) : public STATE_CLASS(taBase) {
  // ##NO_TOKENS ##INLINE base class with owner for core
public:
};

class STATE_CLASS(taNBase) : public STATE_CLASS(taOBase) {
  // ##NO_TOKENS ##INLINE base class with owner and name for core
public:
};

class STATE_CLASS(BaseSpec) : public STATE_CLASS(taNBase) {
  // ##NO_TOKENS ##INLINE base class for all State versions of specs
public:
};

class STATE_CLASS(SpecMemberBase) : public STATE_CLASS(taNBase) {
  // ##NO_TOKENS ##INLINE base class for all core versions of spec members
public:
};

// see also Network_mbrs.h for taBase objects that are used in State code --
// must put them in there so the TA* objects get defined properly.
// Network is the most widely included so best place to put them
