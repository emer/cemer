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

#ifndef IMultiSigLinkClient_h
#define IMultiSigLinkClient_h 1

// parent includes:
#include <ISigLinkClient>

// member includes:
#include <taPtrList>
#include <taSigLink>

// declare all other types mentioned but not required to include:


taTypeDef_Of(IMultiSigLinkClient);

class TA_API IMultiSigLinkClient : public virtual ISigLinkClient {//#NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE
public:

  IMultiSigLinkClient() {}
  ~IMultiSigLinkClient();
protected:
  taPtrList<taSigLink> dls;
  bool         AddSigLink(taSigLink* dl) CPP11_OVERRIDE;
  bool         RemoveSigLink(taSigLink* dl) CPP11_OVERRIDE;
};

#endif // IMultiSigLinkClient_h
