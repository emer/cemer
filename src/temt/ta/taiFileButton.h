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

#ifndef taiFileButton_h
#define taiFileButton_h 1

// parent includes:
#include <taiButtonMenu>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiFileButton : public taiButtonMenu {
  Q_OBJECT
public:
  bool                  read_only;      // only reading streams is an option
  bool                  write_only;     // only writing streams are available

  virtual void          SetFiler(taFiler* gf_); // you can use your own filer if you want; ref counted
  virtual void          GetImage();
  taFiler*              GetFiler() {return gf;} //NOTE: make sure you ref it properly!

  virtual void          GetGetFile();   // make sure we have a getfile..

  taiFileButton(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_,
      int flags_, bool rd_only = false, bool wrt_only = false);
  ~taiFileButton();

public slots:
  virtual void          Open();         // callback
  virtual void          Save();         // callback
  virtual void          SaveAs();       // callback
  virtual void          Append();       // callback
  virtual void          Close();        // callback
  virtual void          Edit();         // callback
protected:
  taFiler*              gf;
};

#endif // taiFileButton_h
