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

#ifndef taDoc_h
#define taDoc_h 1

// parent includes:
#include <taNBase>

// smartptr, ref includes
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taDoc);

class TA_API taDoc : public taNBase {
  // ##CAT_Docs document for providing information on projects and other objects
INHERITED(taNBase)
public:
  String        desc;           // #EDIT_DIALOG #CAT_taBase description of this documentation: what is this doc about
  bool          auto_open;      // open this document upon startup
  bool          web_doc;        // #READ_ONLY this document lives on the web, at the following URL, instead of being local text saved in the project -- the most recently viewed version of the document is cached into the local text, and is rendered if it is not possible to connect to the internet -- this flag is automatically updated based on the url field
  String        wiki;           // name of a wiki, as specified in global preferences, where this object should be stored -- this is used to lookup the wiki name -- if blank then url must be a full URL path
  String        url;            // a URL location for this document -- if blank or "local" then text field is used as document text -- otherwise if wiki name is set, then this is relative to that wiki, as wiki_url/index.php/Projects/url, otherwise it is a full URL path to a valid location
  String        full_url;       // #HIDDEN full URL location for this document -- always updated and used as a backup if loaded in a context where the wiki name is not recognized
  float         text_size;      // #DEF_1 multiplier for text size on page (zoom factor) -- 1.0 is default -- this multiplies the global preferences/options font_size / 12.0 factor (i.e., 12.0 is assumed to be a normal baseline font size), to arrive at the overall font size scaling factor

  String        text;           // #HIDDEN #EDIT_DIALOG the text of the document for local docs (in html/mediawiki format)
  String        html_text;      // #READ_ONLY #HIDDEN #EDIT_DIALOG wiki conversion of html text -- use this for actual display

  static String         WikiParse(const String& in_str);
  // convert very basic wiki syntax to html format -- == headers ==, * bulleted lists, [[ ]] links, etc

  virtual void          UpdateText();
  // update the html_text from the user-entered text with wiki syntax by calling WikiParse

  virtual void          SetURL(const String& new_url);
  // set a new url -- will parse into wiki and url if possible
  virtual String        GetURL();
  // get a full url based on url specified plus any absolute refs needed

  String       GetTypeDecoKey() const override { return "Doc"; }

  TA_BASEFUNS(taDoc);
protected:
  void         UpdateAfterEdit_impl() override;
  static const String   init_text; // ##IGNORE initial text

private:
  SIMPLE_COPY(taDoc);
  void  Initialize();
  void  Destroy() { }
};

TA_SMART_PTRS(taDoc);

#endif // taDoc_h
