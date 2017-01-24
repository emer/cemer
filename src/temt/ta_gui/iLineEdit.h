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

#ifndef iLineEdit_h
#define iLineEdit_h 1

#include "ta_def.h"

#include <QLineEdit>
#include <iCodeCompleter>

// declare all other types mentioned but not required to include:
class String_Array; //

class TA_API iLineEdit: public QLineEdit {
  Q_OBJECT
INHERITED(QLineEdit)
public:
  enum CompleterType {		//
    NO_COMPLETER,			//
    INLINE_CODE_COMPLETER,		//
    DIALOG_FIELD_CODE_COMPLETER,		//
  };

  int           init_start_pos;            // initial starting position for editing -- -1 = end, 0 = start
  bool          init_start_kill;           // at start of editing, kill contents
  bool          completion_enabled;
  int           cursor_position_from_end;  // cursor position for start of completion - only set during completion

  iLineEdit(QWidget* parent = 0, CompleterType completer_type = NO_COMPLETER);
  iLineEdit(const char* text, QWidget* parent, CompleterType completer_type = NO_COMPLETER); //note: can't have defaults, ambiguity

  inline int	  charWidth() {return mchar_width;}
  void		      setCharWidth(int num); // sets width to accommodate num chars of
  inline int	  minCharWidth() {return mmin_char_width;}
  void		      setMinCharWidth(int num); // sets aprox min width to accommodate num chars of average text in current font; 0=no restriction; limited to 128
  virtual void	clearExtSelection();	   // clear extended selection mode and also clear any existing selection
  virtual void	emitReturnPressed();	   // emit this signal
  iCodeCompleter* GetCompleter() { return completer; }
    
#ifndef __MAKETA__
signals:
  void          focusChanged(bool got_focus);
  void          lookupKeyPressed(iLineEdit* le);
  // use this as hook for looking up information based on current text etc (completion) -- bound to Ctrl-L
  void          characterEntered(iLineEdit* le); // use this as hook for looking up information based on current text etc (completion) -- when using the Completer feature rather than lookup feature
  void          completed(QModelIndex index); // emit this signal when the use makes a completion selection
#endif

public slots:
  void          setReadOnly(bool value);
  void          editInEditor(); // edit contents in modal dialog
  void          doLookup();     // what we do when the lookup key is pressed
  void          DoCompletion(bool extend); // what we do when the code completion key combo is pressed
  void          CompletionDone(); // On return (selection of completion)
  void          setText(const QString &str);  // "override" so we can fix cursor position after completion
  
protected slots:

protected:
  int               mmin_char_width; // note: we limit to 128
  int               mchar_width; // note: we limit to 128
  bool              ext_select_on;	   // toggled by Ctrl+space -- extends selection with keyboard movement
  iCodeCompleter*   completer; //
  
  void              focusInEvent(QFocusEvent* e) override;
  void              focusOutEvent(QFocusEvent* e) override;
  void              keyPressEvent(QKeyEvent* e) override;
  void              wheelEvent(QWheelEvent * e) override;
  bool              event(QEvent * e) override;
  bool              eventFilter(QObject* obj, QEvent * e) override;
  virtual bool      IsDelimter(char a_char);

private:
  void		      init(CompleterType completer_type = NO_COMPLETER);
};

#endif // iLineEdit_h
