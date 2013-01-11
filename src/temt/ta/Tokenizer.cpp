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

#include "Tokenizer.h"

using namespace std;

Tokenizer::Tokenizer(std::istream &ifstrm) {
  this->ifstrm = &ifstrm;
  if (!ifstrm) 
    taMisc::Warning("(Tokenizer::Tokenizer): bad ifstream");
  line_num = 1;
}

Tokenizer::~Tokenizer() {
}

void Tokenizer::error_msg(const char *msg_part1, const char *msg_part2) {
  taMisc::Warning("*** Tokenizer: Parse error on line: ", String(line_num),
		  msg_part1, msg_part2);
}

bool Tokenizer::expect(const char *token_str, const char *err_msg) {
  next();
  if (token != String(token_str)) {
    error_msg(err_msg);
    return false;
  }
  return true;
}

char Tokenizer::get_char() {
  last_char = ifstrm->get();
  if (last_char == '\n') line_num++;
  return last_char;
}

void Tokenizer::unget_char() {
  if (last_char == '\n') line_num--;
  ifstrm->unget();
}

bool Tokenizer::next() {
  token = "";
  bool in_comment = false;
  char c;

  if (!*ifstrm) {
    taMisc::Warning("(Tokenizer::get_next): bad ifstream");
    return false;
  }

  // Skip white space & comments
  do {
    if (in_comment) {
      do {
	c = get_char();
      } while ((c != '\n') && !ifstrm->eof());
      if ((c != '\n') && ifstrm->eof()) return false;
    }
    in_comment = false;

    do {
      c = get_char();
    } while (isspace(c) && !ifstrm->eof());
    if (!isspace(c) && ifstrm->eof()) return false;

    if (c == '#') in_comment = true;
  } while (in_comment && !ifstrm->eof());

  unget_char();

  // Extract alpha tokens
  if (isalpha(c) || (c == '_')) {
    token_type = Alpha;
    do {
      c = get_char();
      if (isalpha(c) || isdigit(c) || c == '_') {
	token += c;
      }
    } while (isalpha(c) || isdigit(c) || c == '_');
    unget_char();
    return true;
  }

  // Extract numeric tokens
  if (isdigit(c)) {
    int hit_period = 0;
    token_type = Numeric;
    do {
      c = get_char();
      if (isdigit(c)) {
	token += c;
      } else if (c == '.')  {
	if (!hit_period) token += c;
	hit_period++;
      }
    } while ((isdigit(c) || c == '.') && (hit_period < 2));
    unget_char();
    return true;
  }

  // Extract all else as: 1 char == 1 token
  token_type = Other;
  c = get_char();
  token += c;
  return true;
}

