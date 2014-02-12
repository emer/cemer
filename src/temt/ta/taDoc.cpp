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

#include "taDoc.h"

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(taDoc);


const String taDoc::init_text(
"<html>\n"
"<head></head>\n"
"<body>\n"
"== Enter Title Here ==\n"
"</body>\n"
"</html>\n");

void taDoc::Initialize() {
  auto_open = false;
  web_doc = false;
  url = "local";
  full_url = "local";
  text_size = 1.0f;
  if (!taMisc::is_loading && !taMisc::is_duplicating)
    text = init_text;
}

void taDoc::UpdateText() {
  html_text = WikiParse(text);
}

void taDoc::SetURL(const String& new_url) {
  full_url = new_url;
  if(wiki.nonempty()) {
    String base_url = taMisc::GetWikiURL(wiki, true); // index.php
    if(new_url.startsWith(base_url))
      url = new_url.after(base_url);
    else {
      wiki = _nilString;
      url = url;
    }
  }
  else {
    bool got_one = false;
    for(int i=0;i<taMisc::wikis.size; i++) {
      String wiknm = taMisc::wikis[i].name;
      String base_url = taMisc::GetWikiURL(wiknm, true); // index.php
      if(new_url.startsWith(base_url)) {
        wiki = wiknm;
        url = new_url.after(base_url);
        got_one = true;
        break;
      }
    }
    if(!got_one) {
      wiki = _nilString;
      url = new_url;
    }
  }
  UpdateAfterEdit();
}

String taDoc::GetURL() {
  if(wiki.nonempty()) {
    String wiki_url = taMisc::GetWikiURL(wiki, true); // true = add index.php
    if(TestWarning(wiki_url.empty(), "GetURL", "wiki named:", wiki,
                   "not found in global preferences/options under wiki_url settings -- using full url backup instead.")) {
      wiki = _nilString;
      url = full_url;
      return url;
    }
    return wiki_url + url;
  }
  url = taMisc::FixURL(url);
  return url;
}

void taDoc::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(url.empty() || url == "local")
    web_doc = false;
  else
    web_doc = true;
  if(web_doc) {
    full_url = GetURL();
  }
  else {
    // only do this if not a web doc -- otherwise it saves web page directly to html_text and can display that when offline..
    UpdateText();
  }
}

static String wiki_parse_str_between(const String& cl, const String& sts, const String& eds) {
  if(cl.freq(sts) == 1 && cl.freq(eds) == 1) {
    String btwn = cl.after(sts);
    btwn = btwn.before(eds);
    return btwn;
  }
  return _nilString;
}

static bool wiki_parse_check_seq(const String& cl, int cur_pos, char trg, char trg1 = '\0',
                                 char trg2 = '\0', char trg3 = '\0') {
  int ln = cl.length();
  if(cl[cur_pos] != trg) return false;
  if(trg1 == '\0') return true;
  if(cur_pos+1 >= ln || cl[cur_pos+1] != trg1) return false;
  if(trg2 == '\0') return true;
  if(cur_pos+2 >= ln || cl[cur_pos+2] != trg2) return false;
  if(trg3 == '\0') return true;
  if(cur_pos+3 >= ln || cl[cur_pos+3] != trg3) return false;
  return true;
}

static int wiki_parse_find_term(const String& cl, int cur_pos, char trg, char trg1 = '\0',
                                 char trg2 = '\0', char trg3 = '\0') {
  int ln = cl.length();
  for(int i=cur_pos; i<ln; i++) {
    if(cl[i] != trg) continue;
    if(trg1 == '\0') return i;
    if(i+1 >= ln || cl[i+1] != trg1) continue;
    if(trg2 == '\0') return i+1;
    if(i+2 >= ln || cl[i+2] != trg2) continue;
    if(trg3 == '\0') return i+2;
    if(i+3 >= ln || cl[i+3] != trg3) continue;
    return i+3;
  }
  return -1;
}

String taDoc::WikiParse(const String& in_str) {
  String rval;
  String rest = in_str;
  bool bullet1 = false;
  bool bullet2 = false;
  bool num1 = false;
  bool num2 = false;
  while(rest.contains("\n")) {
    String cl = rest.before("\n");
    rest = rest.after("\n");

    // need bullet first because it sets context with <ul> </ul>
    if(cl.startsWith("* ")) {
      if(bullet2) { cl = "</ul> <li> " + cl.after("* "); bullet2 = false; }
      else if(!bullet1) cl = "<ul><li> " + cl.after("* ");
      else cl = "<li> " + cl.after("* ");
      bullet1 = true;
    }
    else if(cl.startsWith("** ")) {
      if(!bullet2) cl = "<ul><li> " + cl.after("** ");
      else cl = "<li> " + cl.after("** ");
      bullet2 = true;
    }
    else if(cl.startsWith(":* ")) {
      if(!bullet2) cl = "<ul><li> " + cl.after(":* ");
      else cl = "<li> " + cl.after(":* ");
      bullet2 = true;
    }
    else if(cl.startsWith("# ")) {
      if(num2) { cl = "</ol> <li> " + cl.after("# "); num2 = false; }
      else if(!num1) cl = "<ol><li> " + cl.after("# ");
      else cl = "<li> " + cl.after("# ");
      num1 = true;
    }
    else if(cl.startsWith("## ")) {
      if(!num2) cl = "<ol><li> " + cl.after("## ");
      else cl = "<li> " + cl.after("## ");
      num2 = true;
    }
    else if(cl.startsWith(":# ")) {
      if(!num2) cl = "<ol><li> " + cl.after(":# ");
      else cl = "<li> " + cl.after(":# ");
      num2 = true;
    }
    else {
      if(bullet2) { cl += "</ul>"; bullet2 = false; }
      if(bullet1) { cl += "</ul>"; bullet1 = false; }

      if(num2) { cl += "</ol>"; num2 = false; }
      if(num1) { cl += "</ol>"; num1 = false; }
    }

    if(cl.empty()) {            // make a <P> for blank lines..
      rval += "<P>\n";
      continue;
    }

    // headers
    if(wiki_parse_str_between(cl, "==== ", " ====").nonempty()) {
      cl.gsub("==== ", "<h4> ");
      cl.gsub(" ====", " </h4>");
    }
    else if(wiki_parse_str_between(cl, "=== ", " ===").nonempty()) {
      cl.gsub("=== ", "<h3> ");
      cl.gsub(" ===", " </h3>");
    }
    else if(wiki_parse_str_between(cl, "== ", " ==").nonempty()) {
      cl.gsub("== ", "<h2> ");
      cl.gsub(" ==", " </h2>");
    }
    else if(wiki_parse_str_between(cl, "= ", " =").nonempty()) {
      cl.gsub("= ", "<h1> ");
      cl.gsub(" =", " </h1>");
    }

    // now process remainder of string looking for various formatting things
    int cur_pos = 0;

    while(cur_pos < cl.length()) {
      if(wiki_parse_check_seq(cl, cur_pos, '[', '[')) {
        int epos = wiki_parse_find_term(cl, cur_pos+2, ']', ']');
        if(epos > cur_pos+2) {
          String href = cl.at(cur_pos+2, epos-cur_pos-3); // 2 * delim -1
          bool ta_tag = false;
          if(href.startsWith('.')) {
            ta_tag = true;
            href = "ta:" + href;
          }
          String tag = href;
          if(tag.contains('|')) {
            href = href.before('|');
            tag = tag.after('|');
          }
          else if(ta_tag) {
            if(tag.contains("()")) {
              String fnm = tag.after('.',-1);
              tag = tag.before('.',-1);
              if(tag.contains('.')) { // should!
                tag = tag.after('.',-1);
              }
              tag += "." + fnm;
            }
            else {
              tag = tag.after('.',-1);
            }
          }
          cl = cl.before(cur_pos) + "<a href=\"" + href + "\">" + tag + "</a>" + cl.after(epos);
          cur_pos = epos+1;
          continue;
        }
      }
      if(wiki_parse_check_seq(cl, cur_pos, '\'', '\'', '\'')) { // bold
        int epos = wiki_parse_find_term(cl, cur_pos+3, '\'', '\'', '\'');
        if(epos > cur_pos+3) {
          String bld = cl.at(cur_pos+3, epos-cur_pos-5); // 2 * delim -1
          cl = cl.before(cur_pos) + " <b>" + bld + "</b> " + cl.after(epos);
          cur_pos = epos+1;
          continue;
        }
      }
      if(wiki_parse_check_seq(cl, cur_pos, '\'', '\'')) { // emph
        int epos = wiki_parse_find_term(cl, cur_pos+2, '\'', '\'');
        if(epos > cur_pos+2) {
          String bld = cl.at(cur_pos+2, epos-cur_pos-3); // 2 * delim -1
          cl = cl.before(cur_pos) + " <i>" + bld + "</i> " + cl.after(epos);
          cur_pos = epos+1;
          continue;
        }
      }
      cur_pos++;
    }

    rval += cl + "\n";
  }
  return rval;
}

