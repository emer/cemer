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

#ifndef Tokenizer_h
#define Tokenizer_h 1

// parent includes:
#include "ta_def.h"

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:

class TA_API Tokenizer {
  // #IGNORE this is a basic class for parsing a stream of input into alpha/numeric categories. written by Dan Cer
protected:
  std::istream       *ifstrm;        // the input stream
  char          last_char;      // last character read
  char          get_char();
  void          unget_char();
public:
  enum Token_Type {
    Alpha,
    Numeric,
    Other
  };

  int           line_num;       // line number within the input file
  Token_Type    token_type;     // Type of the last token
  String        token;          // the string value of the token

  virtual bool  next();         // get next element: main interface
  virtual void  error_msg(const char *msg_part1, const char *msg_part2 = "");
  virtual bool  expect(const char *token_str, const char *err_msg);

  Tokenizer(std::istream &ifstrm);
  virtual ~Tokenizer();
};

#endif // Tokenizer_h
