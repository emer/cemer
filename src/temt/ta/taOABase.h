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

#ifndef taOABase_h
#define taOABase_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:
class taBaseAdapter; // 


class TA_API taOABase : public taOBase {
  // #NO_TOKENS #NO_UPDATE_AFTER owned base class with QObject adapter for signals/slots
INHERITED(taOBase)
friend class taBaseAdapter;
public:
#ifdef TA_USE_QT
  taBaseAdapter*        adapter; // #IGNORE
  void                  SetAdapter(taBaseAdapter* adapter_);
  void  Initialize()    {adapter = NULL;}
#else
  void  Initialize()    {}
#endif
  void  CutLinks();
  TA_BASEFUNS_NOCOPY(taOABase); //

private:
  void  Destroy() {CutLinks();}
};

#ifdef TA_USE_QT
/*
 * taBaseAdapter enables a taOBase object to handle Qt events, via a
 * proxy(taBaseAdapter)/stub(taBase) approach. Note that dual-parenting a taBase object
 * with QObject is not workable, because QObject must come first, and then all the (taBase*)(void*)
 * casts in the system break...

 * To use, subclass taBaseAdapter when events need to be handled. Create the instance in
 * the Initialize() call and set with SetAdapter. The adapter object does not participate
 * in copying/cloning/etc. (it has no state information).

 * Since classes can have subclasses, there may be successive calls to Initialize with subclasses
 * of an adapter. The adapters will be chained, with the new adapter becoming the QObject parent of
 * the previous adapter. Therefore, when the current adapter is destroyed, child adapters will also
 * get destroyed. It will not matter whether a base class hooks to its own adapter, or to a subclass
 * or to a combination.
*/

class TA_API taBaseAdapter: public QObject {
  // ##IGNORE QObject for attaching events/signals for its taBase owner
friend class taOABase;
public:
  taBaseAdapter(taOABase* owner_): QObject(NULL) {owner = owner_;}
  ~taBaseAdapter();
protected:
  taOABase* owner; // #IGNORE
};
#endif // TA_USE_QT

#endif // taOABase_h
