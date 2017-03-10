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

#ifndef taXmlStreamReader_h
#define taXmlStreamReader_h 1

// parent includes:
#include <taNBase>

// member includes:

// member includes:
#ifndef __MAKETA__
# include <QXmlStreamReader>
#else
class QXmlStreamReader; // #IGNORE
#endif

// declare all other types mentioned but not required to include:

taTypeDef_Of(taXmlStreamReader);

class TA_API taXmlStreamReader : public taNBase {
  // wrapper for parsing xml files with Qt API
INHERITED(taNBase)
public:
#ifndef __MAKETA__
  QXmlStreamReader    reader;
  #if (QT_VERSION >= 0x040600)
    bool                qReadNextStartElement() { return reader.readNextStartElement(); }
    void                qSkipCurrentElement() { reader.skipCurrentElement(); }
    QString             qReadElementText() { return reader.readElementText(); }
    QStringRef          qName() { return reader.name(); }
  #endif
#endif
  
  String            filename;
  
  void              SetFile(const String& filename);
  bool              ReadNextStartElement();
  void              SkipCurrentElement();
  String            ReadElementText();
  String            GetNameValue();
  
  TA_SIMPLE_BASEFUNS(taXmlStreamReader);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // taXmlStreamReader_h
