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

#include "taSound_QObj.h"

#if (QT_VERSION >= 0x050000)

#include <taSound>
#include <QAudioDecoder>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QBuffer>
#include <QByteArray>

#include <taMisc>

taSound_QObj::taSound_QObj(taSound* snd) {
  sound = snd;
  decoder = NULL;
  output = NULL;
  out_buff = NULL;
  done_loading = false;
}

taSound_QObj::~taSound_QObj() {
  // these are parented to us so they die with us.
  // if(decoder) delete decoder;
  // if(output) delete output;
  sound = NULL;
  decoder = NULL;
  output = NULL;
}


bool taSound_QObj::LoadSound(const QString& fname) {
  if(decoder) {
    delete decoder;
  }
  decoder = new QAudioDecoder(this);
  decoder->setSourceFilename(fname);

  connect(decoder, SIGNAL(finished()), this, SLOT(LoadFinished()));

  connect(decoder, SIGNAL(error(QAudioDecoder::Error)), this, SLOT(LoadError()));

  done_loading = false;
  
  decoder->start();

  while(!done_loading) {
    taMisc::RunPending();
  }
  return sound->q_buf.isValid(); // todo: could have local err flag too
}

void taSound_QObj::LoadFinished() {
  if(!decoder->bufferAvailable()) {
    LoadError();
    return;
  }
  sound->q_buf = decoder->read();
  delete decoder;
  decoder = NULL;
  done_loading = true;
}

void taSound_QObj::LoadError() {
  taMisc::Error("Sound file not loadable for sound:", sound->name,
                "file:", decoder->sourceFilename().toLatin1(), "err msg:",
                decoder->errorString().toLatin1());
  delete decoder;
  decoder = NULL;
  done_loading = true;
}

bool taSound_QObj::PlaySound(const QString& device_name) {
  if(output) {
    taMisc::Error("PlaySound for sound:", sound->name, "sound is currently still being played");
    return false;
  }
  if(device_name.isEmpty()) {
    output = new QAudioOutput(sound->q_buf.format(), this);
  }
  else {
    QList<QAudioDeviceInfo> devs = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    QAudioDeviceInfo ad;
    for(int i=0; i<devs.count(); i++) {
      ad = devs[i];
      if(ad.deviceName() == device_name)
        break;
    }
    output = new QAudioOutput(ad, sound->q_buf.format(), this);
  }
  out_bary = QByteArray::fromRawData((const char*)sound->q_buf.constData(),
                                            sound->q_buf.byteCount());
  out_buff = new QBuffer(&out_bary, output);
  out_buff->open(QIODevice::ReadOnly);

  connect(output, SIGNAL(stateChanged(QAudio::State)), this,
          SLOT(PlayStateChanged(QAudio::State)));
  output->start(out_buff);
  return true;
}

void taSound_QObj::PlayStateChanged(QAudio::State newState) {
  switch (newState) {
  case QAudio::IdleState:
    // Finished playing (no more data)
    output->stop();
    out_buff->close();
    delete output;
    output = NULL;
    // taMisc::Info("sound done");
    break;

  case QAudio::StoppedState:
    // Stopped for other reasons
    if (output->error() != QAudio::NoError) {
      taMisc::Error("Error playing sound:", sound->name, "Code:",
                    String(output->error()));
    }
    break;

  default:
    // ... other cases as appropriate
    break;
  }
}

#else  // (QT_VERSION >= 0x050000)

taSound_QObj::taSound_QObj(taSound* snd) {
}

#endif // (QT_VERSION >= 0x050000)
