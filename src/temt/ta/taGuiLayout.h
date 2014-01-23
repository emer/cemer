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

#ifndef taGuiLayout_h
#define taGuiLayout_h 1

// parent includes:
#include <taNBase>

// member includes:
#ifndef __MAKETA__
#include <QPointer>
#endif

// declare all other types mentioned but not required to include:
class QBoxLayout; //

taTypeDef_Of(taGuiLayout);

class TA_API taGuiLayout : public taNBase {
  // ##CAT_Gui manages a qt layout for the taGui system
INHERITED(taNBase) // declares 'inherited' keyword for safer base-class references
public:
#ifndef __MAKETA__
  QPointer<QBoxLayout>  layout; // the layout
#endif

  TA_SIMPLE_BASEFUNS(taGuiLayout);
// protected:
//   void              UpdateAfterEdit_impl() CPP11_OVERRIDE;
private:
  void  Initialize()    { };
  void  Destroy()       { };
};

#endif // taGuiLayout_h
