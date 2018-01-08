// Co2018ght 2017-2017, Regents of the University of Colorado,
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

#ifndef taMarkUp_h
#define taMarkUp_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:
class DataTable; //

taTypeDef_Of(taMarkUp);

class TA_API taMarkUp : public taNBase {
  // #STEM_BASE ##CAT_Text ##STATIC_COMPLETION functions for generating markup-formatted text in various standard formats
INHERITED(taNBase)
public:
  enum Format {
    MARKDOWN,                   // GFM git flavored Markdown format
    HTML,                       // basic HTML format tags 
    MEDIAWIKI,                  // MediaWiki format, as used on WikiPedia
    LATEX,                      // LaTeX format
  };

  static String Escape(Format fmt, const String& text);
  // #CAT_Style apply format-specific escaping of control sequences so things should render properly -- used automtically in any of the code below that takes an arg, but can be called separately when generating markup output
 
  static String Bold(Format fmt, const String& text);
  // #CAT_Style format the given text in bold
  static String Italics(Format fmt, const String& text);
  // #CAT_Style format the given text in italics
  static String Code(Format fmt, const String& text);
  // #CAT_Style format the given text in code / preformatted
  static String Strike(Format fmt, const String& text);
  // #CAT_Style format the given text in strike-through
 
  static String TableStart(Format fmt);
  // #CAT_Table code for start of overall table in given format -- initializes a column counter
  static String TableHeaderStart(Format fmt, int& n_cols);
  // #CAT_Table code for start of table header row in given format
  static String TableHeader(Format fmt, const String& header, int& n_cols);
  // #CAT_Table code for a single header
  static String TableHeaderEnd(Format fmt, int n_cols);
  // #CAT_Table code for end of table header row in given format
  static String TableRowStart(Format fmt, int& col_no);
  // #CAT_Table code for start of regular table row in given format, initializing a column number counter
  static String TableCell(Format fmt, const String& cell, int& col_no);
  // #CAT_Table code for a single cell, incrementing a column number counter
  static String TableRowEnd(Format fmt, int col_no);
  // #CAT_Table code for end of regular table row in given format
  static String TableEnd(Format fmt);
  // #CAT_Table code for end of overall table in given format

  static String DataTableTable(const DataTable& data, Format fmt);
  // #CAT_Table generate a text table for given data table

  static String ListStart(Format fmt, int& indent, bool number = false);
  // #CAT_List code for start of list in given format -- increments an indent counter for nested lists -- if number then uses an enumerated format -- otherwise just a bullet-style list
  static String ListItem(Format fmt, int indent, bool number = false);
  // #CAT_List code for a given list item at given level of indenting -- does NOT add the item in case you want to do that in a separate way
  static String ListEnd(Format fmt, int& indent, bool number = false);
  // #CAT_List code for end of list in given format -- decrements an indent counter for nested lists -- if number then uses an enumerated format -- otherwise just a bullet-style list
  
  TA_SIMPLE_BASEFUNS(taMarkUp);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // taMarkUp_h
