// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "ImageNetUtils.h"

#include <taMisc>
#include <taVector2i>
#include <taVector3i>

#include <QFile>
#include <QString>

void ImageNetUtils::GetImageSize(const String& filename, taVector3i& width_height_depth) {

#if (QT_VERSION >= 0x040600)
  QFile file(filename);
  if(!file.open(QFile::ReadOnly | QFile::Text)){
    String msg("ImageNetUtils::OpenReader -- could not open file " + filename);
    taMisc::Error(msg);
  }
  
  QXmlStreamReader reader(&file);
  
  int width = 0;
  int height = 0;
  int depth = 0;
  bool ok;
  
  if (reader.readNextStartElement()) {
    if (reader.name() == "annotation") {
      while(reader.readNextStartElement()) {
        if(reader.name() == "size") {
          while(reader.readNextStartElement()) {
            if(reader.name() == "width") {
              QString s = reader.readElementText();
              width = s.toInt(&ok, 10);
            }
            else if(reader.name() == "height") {
              QString s = reader.readElementText();
              height = s.toInt(&ok, 10);
            }
            else if(reader.name() == "depth") {
              QString s = reader.readElementText();
              depth = s.toInt(&ok, 10);
            }
            else {
              reader.skipCurrentElement();
            }
          }
        }
        else {
          reader.skipCurrentElement();
        }
      }
    }
    else {
      taMisc::Error("ImageNetUtils::GetSize -- start element is not \"annotation\" " );
      return;
    }
    width_height_depth.SetXYZ(width, height, depth);
  }
#endif
}

void ImageNetUtils::GetBoundingBox(const String& filename, taVector2i& top_left, taVector2i& bottom_right) {

#if (QT_VERSION >= 0x040600)
  QFile file(filename);
  if(!file.open(QFile::ReadOnly | QFile::Text)){
    String msg("ImageNetUtils::OpenReader -- could not open file " + filename);
    taMisc::Error(msg);
  }
  
  QXmlStreamReader reader(&file);
  
  int x_min = -1;
  int y_min = -1;
  int x_max = -1;
  int y_max = -1;
  bool ok;
  
  if (reader.readNextStartElement()) {
    if (reader.name() == "annotation") {
      while(reader.readNextStartElement()) {
        if(reader.name() == "object") {
          while(reader.readNextStartElement()) {
            if(reader.name() == "bndbox") {
              while(reader.readNextStartElement()) {
                if(reader.name() == "xmin") {
                  QString s = reader.readElementText();
                  x_min = s.toInt(&ok, 10);
                }
                else if(reader.name() == "ymin") {
                  QString s = reader.readElementText();
                  y_min = s.toInt(&ok, 10);
                }
                else if(reader.name() == "xmax") {
                  QString s = reader.readElementText();
                  x_max = s.toInt(&ok, 10);
                }
                else if(reader.name() == "ymax") {
                  QString s = reader.readElementText();
                  y_max = s.toInt(&ok, 10);
                }
                else {
                  reader.skipCurrentElement();
                }
              }
              // We only return the first bounding box
              if (x_min > -1 && x_max > -1 && y_min > -1 && y_max > -1) {
                top_left.SetXY(x_min, y_min);
                bottom_right.SetXY(x_max, y_max);
                return;
              }
            }
            else {
              reader.skipCurrentElement();
            }
          }
        }
        else {
          reader.skipCurrentElement();
        }
      }
    }
    else {
      taMisc::Error("ImageNetUtils::GetSize -- start element is not \"annotation\" " );
      return;
    }
  }
#endif
}

String ImageNetUtils::GetPrimaryCategory(const String& filename) {  
  String category;
  
#if (QT_VERSION >= 0x040600)
  QFile file(filename);
  if(!file.open(QFile::ReadOnly | QFile::Text)){
    String msg("ImageNetUtils::OpenReader -- could not open file " + filename);
    taMisc::Error(msg);
  }
  
  QXmlStreamReader reader(&file);
  
  bool ok;
  
  if (reader.readNextStartElement()) {
    if (reader.name() == "annotation") {
      while(reader.readNextStartElement()) {
        if(reader.name() == "object") {
          while(reader.readNextStartElement()) {
            if(reader.name() == "name") {
              QString s = reader.readElementText();
              category = s;
              return category;
            }
            else {
              reader.skipCurrentElement();
            }
          }
        }
        else {
          reader.skipCurrentElement();
        }
      }
    }
    else {
      taMisc::Error("ImageNetUtils::GetSize -- start element is not \"annotation\" " );
    }
  }
#endif
  return category;
}
