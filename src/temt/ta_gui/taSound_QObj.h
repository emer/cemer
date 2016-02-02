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

#ifndef taSound_QObj_h
#define taSound_QObj_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QObject>
#endif

// member includes:
#include <QAudio>

// declare all other types mentioned but not required to include:
class taSound;
class QAudioDecoder;
class QAudioFormat;
class QString;
class QAudioOutput;
class QBuffer;
class QByteArray;


class TA_API taSound_QObj : public QObject {
 // #IGNORE this is a helper QObject that handles signals/slots
Q_OBJECT
friend class taSound;
public:

  bool  LoadSound(const QString& fname);
  bool  PlaySound(const QString& device_name);

  ~taSound_QObj();
                                             
public slots:
  void           LoadFinished();
  void           LoadError();
  void           PlayStateChanged(QAudio::State new_state);

protected:
  taSound*      sound;
  QAudioDecoder* decoder;
  QAudioOutput*  output;
  QByteArray    out_bary;
  QBuffer*      out_buff;
  bool          done_loading;

  taSound_QObj(taSound* snd);
};

#endif // taSound_QObj_h
