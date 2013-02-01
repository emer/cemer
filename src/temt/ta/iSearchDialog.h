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

#ifndef iSearchDialog_h
#define iSearchDialog_h 1

// parent includes:
#include <ISigLinkClient>
#ifndef __MAKETA__
#include <QDialog>
#endif

// member includes:
#ifndef __MAKETA__
#include <QTime>
#endif
#include <DataTable>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class taiSigLink; // 
class taSigLink; // 
class taiWidgetBitBox; //
class iMainWindowViewer; //
class QVBoxLayout; // 
class iLineEdit; //
class QAbstractButton; //
class iTextBrowser; //
class QStatusBar; //


TypeDef_Of(iSearchDialog);

class TA_API iSearchDialog: public QDialog, public virtual ISigLinkClient {
//   search a project (or more)
INHERITED(QDialog)
  Q_OBJECT
public:
#ifndef __MAKETA__
  enum Roles { // extra roles, for additional data, etc.
    ObjUrlRole = Qt::UserRole + 1, // Url stored in this
//    ObjCatRole  // for object category string, whether shown or not
  };
#endif

  enum SearchOptions { // #BITS
    SO_OBJ_NAME         = 0x0001, // #LABEL_object_name the name given to the object by the user
    SO_OBJ_TYPE         = 0x0002, // #LABEL_object_type the type name
    SO_OBJ_DESC         = 0x0004, // #LABEL_object_desc object description (where applicable), including full display name
    SO_MEMB_NAME        = 0x0008, // #LABEL_member_name the name of the member of an object
    SO_MEMB_VAL         = 0x0010, // #LABEL_member_value searches in the values of members, especially strings
    SO_ALL_MEMBS        = 0x0020, // #LABEL_all_members searches in all members, otherwise excludes the members that are not typicaly shown according to current settings
    SO_TYPE_DESC        = 0x0040, // #LABEL_type_desc searches in the type description information, including the description of the type of the object, and the description of the member (all of this information is hard-coded into the program)
    SO_MATCH_CASE       = 0x0080, // #LABEL_match_case match the case of the search string -- default is to ignore case
#ifndef __MAKETA__
    SO_DEF_OPTIONS      = SO_OBJ_NAME | SO_OBJ_TYPE | SO_OBJ_DESC | SO_MEMB_NAME | SO_MEMB_VAL
#endif
  };

#ifndef __MAKETA__ // needed to allow us to get SearchOptions
  static const int      col_row = 0; // note: cols must be created in this order!
  static const int      col_level = 1;
  static const int      col_headline = 2;
  static const int      col_href = 3;
  static const int      col_desc = 4;
  static const int      col_hits = 5;
  static const int      col_relev = 6;
  static const int      col_path = 7;
  static const int      num_cols = col_path + 1;

//static const int      num_vis_cols = 3;
//static const char*    vis_col_names[] = {"nest", "item", "hits"};


  static iSearchDialog* New(int ft = 0, iMainWindowViewer* par_window_ = NULL);

  QVBoxLayout*          layOuter;
  taiWidgetBitBox*              bbOptions;
  iLineEdit*                search;
  QAbstractButton*          btnGo;
  QAbstractButton*          btnStop;
  iTextBrowser*           results;      // list of result items
  QStatusBar*             status_bar;
  QTime                   proc_events_timer;

  int                   options() const {return m_options;}
  void                  setRoot(taiSigLink* root, bool update_gui = true); // set or reset the root and window used for the search; sets caption and clears
  void                  setSearchStr(const String& srch_str);
  // set the search string to given value (erases any that might be there already -- if non-empty, starts the search going too!
  String                searchStr() const;
  // gets the current value of the search string

  bool                  stop() const; // allow event loop, then check stop
  bool                  setFirstSort(int col); // set first sort column, pushes others down; true if order changed
  inline const String_PArray& targets() const {return m_targets;} // ref for max speed; only use in search
  inline const String_PArray& kickers() const {return m_kickers;} // ref for max speed; only use in search

  void                  FindNext(); // really only applicable if already have results

  void                  Reset(); // clears items and window, esp for when link detaches
  void                  Search(); // search, based on search line
// following are all rendering apis
  void                  Start(); // resets everything, and starts new results page
  void                  StartSection(const String& sec_name);
  void                  EndSection(); // end the current section
  void                  AddItem(const String& headline,
    const String& href, const String& desc, const String& hits,
    const String& path_long,int level = 0, int relev = 0);
  void                  End(); // end all and display results

public: // ISigLinkClient interface
  override void*        This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_iSearchDialog;}
  override bool         ignoreSigEmit() const {return true;}
  override void         SigLinkRecv(taSigLink*, int sls, void* op1, void* op2) {}
  override void         SigLinkDestroying(taSigLink* dl);


protected:
  static const int      num_sorts = 3;

  int                   m_options; // any of the option values
  String                m_caption; // base portion of caption
  String                src;
  String                root_path;
  int                   m_changing;
  bool                  m_stop;
  String_PArray         m_targets;
  String_PArray         m_kickers;
  DataTable             m_items;
  int                   m_row; // row num, for baking into results table
  int                   m_sorts[num_sorts];

  virtual void          Constr();
   // does constr, called in static, so can extend
  void                  RootSet(taiSigLink* root); // called when root changes
  void                  ParseSearchString();
  void                  Render();
  void                  RenderItem(int level, const String& headline,
           const String& href, const String& desc, const String& hits,
           const String& path, int relev);

  override void         closeEvent(QCloseEvent * e);

  iSearchDialog(iMainWindowViewer* par_window_);
  ~iSearchDialog();

protected slots:
  void                  go_clicked();
  void                  stop_clicked();
  void                  results_setSourceRequest(iTextBrowser* src,
    const QUrl& url, bool& cancel);

private:
  void          init(); // called by constructors
#endif // !__MAKETA__
};

#endif // iSearchDialog_h
