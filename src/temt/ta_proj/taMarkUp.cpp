// Copyright 2017, Regents of the University of Colorado,
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

#include "taMarkUp.h"

#include <DataTable>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(taMarkUp);

String taMarkUp::Bold(Format fmt, const String& text) {
  switch(fmt) {
  case HTML:
    return "<b>" + text + "</b>";
  case MEDIAWIKI:
    return "'''" + text + "'''";
  case MARKDOWN:
    return "**" + text + "**";
  case LATEX:
    return "\\textbf{" + text + "}";
  }
  return _nilString;
}

String taMarkUp::Italics(Format fmt, const String& text) {
  switch(fmt) {
  case HTML:
    return "<i>" + text + "</i>";
  case MEDIAWIKI:
    return "''" + text + "''";
  case MARKDOWN:
    return "*" + text + "*";
  case LATEX:
    return "\\textit{" + text + "}";
  }
  return _nilString;
}

String taMarkUp::Code(Format fmt, const String& text) {
  switch(fmt) {
  case HTML:
  case MEDIAWIKI:
    return "<code>" + text + "</code>";
  case MARKDOWN:
    return "`" + text + "`";
  case LATEX:
    return "\\texttt{" + text + "}";
  }
  return _nilString;
}

String taMarkUp::Strike(Format fmt, const String& text) {
  switch(fmt) {
  case HTML:
  case MEDIAWIKI:
    return "<s>" + text + "</s>";
  case MARKDOWN:
    return "~~" + text + "~~";
  case LATEX:
    return "\\sout{" + text + "}";
  }
  return _nilString;
}


String taMarkUp::TableStart(Format fmt) {
  switch(fmt) {
  case HTML:
    return "<table>\n";
  case MEDIAWIKI:
    return "{| class=\"wikitable\"\n";
  case MARKDOWN:
    return "";
  case LATEX:
    return "\\begin{tabular}{ll} % todo: fix to number of cols\n";
  }
  return _nilString;
}

String taMarkUp::TableHeaderStart(Format fmt, int& n_cols) {
  n_cols = 0;
  switch(fmt) {
  case HTML:
    return "<tr>\n";
  case MEDIAWIKI:
    return "|-\n";
  case MARKDOWN:
    return "";
  case LATEX:
    return "\\hline\n";
  }
  return _nilString;
}

String taMarkUp::TableHeader(Format fmt, const String& header, int& n_cols) {
  n_cols++;
  switch(fmt) {
  case HTML:
    return "  <th>" + header + "</th>\n";
  case MEDIAWIKI: {
    if(n_cols == 1)
      return "! " + header;
    else
      return " !! " + header;
  }
  case MARKDOWN:
    return "| " + header + " ";
  case LATEX: {
    if(n_cols == 1)
      return header;
    else
      return " & " + header;
  }
  }
  return _nilString;
}

String taMarkUp::TableHeaderEnd(Format fmt, int n_cols) {
  switch(fmt) {
  case HTML:
    return "</tr>\n";
  case MEDIAWIKI:
    return "\n";
  case MARKDOWN: {
    String rval = "|\n";
    for(int i=0; i<n_cols; i++) {
      rval << "| --- ";
    }
    rval << "|\n";
    return rval;
  }
  case LATEX:
    return "\\\\\n\\hline";
  }
  return _nilString;
}

String taMarkUp::TableRowStart(Format fmt, int& col_no) {
  col_no = 0;
  switch(fmt) {
  case HTML:
    return "<tr>\n";
  case MEDIAWIKI:
    return "|-\n";
  case MARKDOWN:
    return "";
  case LATEX:
    return "";
  }
  return _nilString;
}

String taMarkUp::TableCell(Format fmt, const String& cell, int& col_no) {
  col_no++;
  switch(fmt) {
  case HTML:
    return "  <td>" + cell + "</td>\n";
  case MEDIAWIKI: {
    if(col_no == 1)
      return "| " + cell;
    else
      return " || " + cell;
  }
  case MARKDOWN:
    return "| " + cell + " ";
  case LATEX: {
    if(col_no == 1)
      return cell;
    else
      return " & " + cell;
  }
  }
  return _nilString;
}

String taMarkUp::TableRowEnd(Format fmt, int col_no) {
  switch(fmt) {
  case HTML:
    return "</tr>\n";
  case MEDIAWIKI:
    return "\n";
  case MARKDOWN:
    return "|\n";
  case LATEX:
    return "\\\\\n";
  }
  return _nilString;
}

String taMarkUp::TableEnd(Format fmt) {
  switch(fmt) {
  case HTML:
    return "</table>\n";
  case MEDIAWIKI:
    return "|}\n";
  case MARKDOWN:
    return "\n";
  case LATEX:
    return "\\hline\n\\end{tabular}\n";
  }
  return _nilString;
}

String taMarkUp::DataTableTable(const DataTable& data, Format fmt) {
  String rval;
  int n_cols = 0;
  rval << TableStart(fmt) << TableHeaderStart(fmt, n_cols);
  for(int i=0; i<data.data.size; i++) {
    rval << TableHeader(fmt, data.data[i]->name, n_cols);
  }
  rval << TableHeaderEnd(fmt, n_cols);
  for(int r=0; r<data.rows; r++) {
    int col_no = 0;
    rval << TableRowStart(fmt, col_no);
    for(int i=0; i<data.data.size; i++) {
      rval << TableCell(fmt, data.GetValAsString(r,i), col_no);
    }
    rval << TableRowEnd(fmt, col_no);
  }
  rval << TableEnd(fmt);
  return rval;
}

String taMarkUp::ListStart(Format fmt, int& indent, bool number) {
  String rval;
  taMisc::IndentString(rval, indent);
  indent++;
  switch(fmt) {
  case HTML:
    if(number)
      rval << "<ol>\n";
    else
      rval << "<ul>\n";
    break;
  case MEDIAWIKI:
    return "";
    break;
  case MARKDOWN:
    return "";
    break;
  case LATEX:
    if(number)
      rval << "\\begin{enumerate}\n";
    else
      rval << "\\begin{itemize}\n";
    break;
  }
  return rval;
}

String taMarkUp::ListItem(Format fmt, int indent, bool number) {
  String rval;
  switch(fmt) {
  case HTML:
    taMisc::IndentString(rval, indent);
    rval << "<li> ";
    break;
  case MEDIAWIKI:
    if(number)
      rval << String(indent, 0, '#') << " ";
    else
      rval << String(indent, 0, '*') << " ";
    break;
  case MARKDOWN:
    taMisc::IndentString(rval, indent);
    if(number)
      rval << "1. ";
    else {
      if(indent == 1)
        rval << "* ";
      else
        rval << "+ ";
    }
    break;
  case LATEX:
    taMisc::IndentString(rval, indent);
    rval << "\\item ";
    break;
  }
  return rval;
}

String taMarkUp::ListEnd(Format fmt, int& indent, bool number) {
  indent--;
  String rval;
  taMisc::IndentString(rval, indent);
  switch(fmt) {
  case HTML:
    if(number)
      rval << "</ol>\n";
    else
      rval << "</ul>\n";
    break;
  case MEDIAWIKI:
    return "";
    break;
  case MARKDOWN:
    return "";
    break;
  case LATEX:
    if(number)
      rval << "\\end{enumerate}\n";
    else
      rval << "\\end{itemize}\n";
    break;
  }
  return rval;
}
