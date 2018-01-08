// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#ifndef iDialog_h
#define iDialog_h 1

#include "ta_def.h"

#include <QDialog>

class TA_API iDialog : public QDialog
{
  Q_OBJECT

public:
  explicit iDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);

  int exec() override;

protected:
  void keyPressEvent(QKeyEvent* key_event) override;
};

#endif // iDialog_h
