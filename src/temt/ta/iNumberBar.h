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

#ifndef iNumberBar_h
#define iNumberBar_h 1

#include "ta_def.h"

#include <QWidget>
#include <QPixmap>

class iTextEdit;

/**
 * @internal Used to display the numbers.
 */
class iNumberBar : public QWidget {
  Q_OBJECT
public:
  enum LineFlags {		// flags stored in line_flags for each line
    LF_NONE 	= 0,		// no flag here
    LF_BREAK	= 0x0001,	// breakpoint at this line
    LF_ERROR	= 0x0002,	// there is an error at this line
    LF_WARNING 	= 0x0004,	// there is a warning at this line
  };

  iNumberBar( QWidget *parent, bool enable_icons = false );
  ~iNumberBar();

  void setTextEdit( iTextEdit *edit );
  void paintEvent( QPaintEvent *ev ) override;

  virtual void	setLineFlags(int lineno, int flags);
  // set line flags for given line
  virtual int	lineFlags(int lineno) const;
  // get line flags for given line

  virtual void	clearAllLineFlags();
  // reset all line flags
  virtual void 	setCurrentLine(int lineno, bool recenter = true);
  // set line that is current executing -- if recenter then center text view on the line if it is different than the previous setting
  virtual void	centerOnLine(int lineno);
  // center text on given line number

  virtual int	lineNumberFmPos(float y_pos);
  // get line number from position relative to y position within this widget
  virtual QRectF rectFromLineNumber(int lineno);
  // get visible rectangle of the entire numberbar entry for given line number, including number and icons

signals:
  void	lineFlagsUpdated(int lineno, int flags);
  // a gui action resulted in the flags for given line being updated
  void  viewSource(int lineno);
  // signal to view source for given line number (e.g., what generated this line of code)

public slots:
  void	setBreakpointSlot();
  // set breakpoint at cur_lineno
  void	clearBreakpointSlot();
  // clear breakpoint at cur_lineno
  void	viewSourceSlot();
  // view source at cur_lineno

protected:
  bool event( QEvent *ev ) override;
//   void mousePressEvent (QMouseEvent * event) override;
  void contextMenuEvent ( QContextMenuEvent * event ) override;

  QVector<int>	line_flags;	// flags for each line of code
  int	cur_lineno;		// current line number -- for gui interactions
  int	text_width;		// width of the text display
  int	text_height;		// height of the text display
  int	icon_width;		// width of the extra icon
  int	total_width;		// total width of bar
  bool	icons_enabled;
  iTextEdit *edit;
  QPixmap stopMarker;
  QPixmap currentMarker;
  QPixmap bugMarker;
  int 	currentLine;		// current line -- where the code execution is now
};


#endif // iNumberBar_h
