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

#ifndef taXmlStreamReader_h
#define taXmlStreamReader_h 1

// parent includes:
#include <taNBase>

// member includes:
#ifndef __MAKETA__
  #include <QXmlStreamReader>
  #include <QFile>
#else
  class QXmlStreamReader; // #IGNORE
  class QFile; // #IGNORE
#endif

// declare all other types mentioned but not required to include:

taTypeDef_Of(taXmlStreamReader);

class TA_API taXmlStreamReader : public taNBase {
  // wrapper for parsing xml files with Qt API -- Must call SetFile first!
  INHERITED(taNBase)
public:
#ifndef __MAKETA__
  
#if (QT_VERSION >= 0x040600)
  bool             qReadNextStartElement() { return reader.readNextStartElement(); }
  void             qSkipCurrentElement() { reader.skipCurrentElement(); }
  void             qReadNext() { reader.readNext(); }
  QString          qReadElementText() { return reader.readElementText(); }
  QString          qName() { return reader.name().toString(); }
  void             qClear() { reader.clear(); }
#endif  // QT_VERSION >= 0x040600
  
#endif // __MAKETA__
  
  bool              SetFile(const String& filename);  // Must call first!
  bool              ReadNextStartElement();  // See Qt documentation for QXmlStreamReader
  void              ReadNext();  // See Qt documentation for QXmlStreamReader
  void              SkipCurrentElement();  // See Qt documentation for QXmlStreamReader
  String            ReadElementText();  // See Qt documentation for QXmlStreamReader
  String            GetNameValue();  // See Qt documentation for QXmlStreamReader
  void              Clear();  // removes device associated with the reader and clears internal state
  void              CloseFile(); // !! Do this when done
  
  TA_SIMPLE_BASEFUNS(taXmlStreamReader);
  
protected:
  QXmlStreamReader             reader;
  QFile                        file;
  
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // taXmlStreamReader_h
