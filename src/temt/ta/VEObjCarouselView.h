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

#ifndef VEObjCarouselView_h
#define VEObjCarouselView_h 1

// parent includes:
#include <VEBodyView>

// member includes:

// declare all other types mentioned but not required to include:
class VEObjCarousel; // 


taTypeDef_Of(VEObjCarouselView);

class TA_API VEObjCarouselView: public VEBodyView {
  // view of VEObjCarousel
INHERITED(VEBodyView)
friend class VEWorldView;
public:
  VEObjCarousel*	ObjCarousel() const { return (VEObjCarousel*)data();}
  virtual void		SetObjCarousel(VEObjCarousel* ob);

  TA_BASEFUNS_NOCOPY(VEObjCarouselView);

protected:
  void	Initialize();
  void	Destroy();

  void		Render_pre() CPP11_OVERRIDE;
  void		Render_impl() CPP11_OVERRIDE;
};

#endif // VEObjCarouselView_h
