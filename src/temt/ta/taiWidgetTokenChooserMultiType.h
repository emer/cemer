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

#ifndef taiWidgetTokenChooserMultiType_h
#define taiWidgetTokenChooserMultiType_h 1

// parent includes:
#include <taiWidgetItemChooser>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taiWidgetTokenChooserMultiType : public taiWidgetItemChooser {
  // for tokens of taBase objects of multiple types
  INHERITED(taiWidgetItemChooser)
public:
  TypeSpace             type_list; // #LINK_GROUP set of types to generate tokens for -- must be set manually after construction and before GetImage etc -- be sure to only do Link here..

  inline taBase*        token() const {return (taBase*)m_sel;}
  int          columnCount(int view) const CPP11_OVERRIDE;
  const String headerText(int index, int view) const CPP11_OVERRIDE;
  int          viewCount() const CPP11_OVERRIDE; // n = size of type_list + 1
  const String viewText(int index) const CPP11_OVERRIDE;

  void         GetImage(void* cur_sel, TypeDef* targ_typ) CPP11_OVERRIDE;
  virtual void          GetImageScoped(taBase* ths, TypeDef* targ_typ, taBase* scope = NULL,
        TypeDef* scope_type = NULL);
  // get image, using the new type and scope params supplied
  virtual taBase*       GetValue() {return token();}

  void                  BuildChooser(iDialogItemChooser* ic, int view = 0); // override
//TODO  void           btnHelp_clicked() CPP11_OVERRIDE;

  void         EditPanel() CPP11_OVERRIDE;
  void         EditDialog() CPP11_OVERRIDE;

  taiWidgetTokenChooserMultiType(TypeDef* typ_, IWidgetHost* host,
                             taiWidget* par, QWidget* gui_parent_, int flags_ = 0,
                             const String& flt_start_txt = "");
protected:
  taSmartRef            scope_ref;      // reference object for scoping, default is none
  TypeDef*              scope_typ;      // type of scope to use (NULL = default)

  const String          itemTag() const {return "Token: ";}
  const String          labelNameNonNull() const;

  int                   BuildChooser_0(iDialogItemChooser* ic, TypeDef* top_typ,
    QTreeWidgetItem* top_item); // we use this recursively
  virtual bool          ShowToken(taBase* obj) const;
};

#endif // taiWidgetTokenChooserMultiType_h
