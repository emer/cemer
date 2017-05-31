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

#include "iInterceptor.h"

//
// Author: Kuba Ober <kuba@mareimbrium.org>
// Downloaded from: http://www.ibib.waw.pl/~winnie
//
// License: Public domain
//

#ifdef TA_OS_WIN
# include <io.h>
#else
# include <unistd.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <cassert>

#include <qtextstream.h>
#include <qsocketnotifier.h>

using namespace std;

iInterceptor::iInterceptor(QObject * parent) :
QObject(parent),
    m_stream(0),
    m_notifier(0)
{
  m_origFd = 0;
  m_origFdCopy = 0;
  m_pipeFd[0] = 0;
  m_pipeFd[1] = 0;
}

iInterceptor::~iInterceptor()
{
    finish();
}

void iInterceptor::initialize(int outFd)
{           
    if (m_notifier != 0) finish();

    m_origFd = outFd;
   
    // m_pipefd[0] is the read-end of the pipe and m_pipefd[1] is the write-end.

    // Open a new pipe. We will pipe output file descriptor through the new pipe and
    // read it with a QTextStream object, using a QSocketNotifier to tell us when
    // something is waiting in the pipe.
#ifdef TA_OS_WIN
    int rc = ::_pipe(m_pipeFd, 1024, _O_TEXT);
#else
    int rc = ::pipe(m_pipeFd);
#endif
    assert (rc >= 0);

    // Save the original output descriptor.
    m_origFdCopy = ::dup(m_origFd);
    assert(m_origFdCopy >= 0);
   
    // Make the ouput descriptor a copy of pipe's write end
    rc = ::dup2(m_pipeFd[1], m_origFd);
    assert (rc >= 0);
    ::close(m_pipeFd[1]); // close the write end of the pipe descriptor, it's redundant now

    // Open the pipe's read end non-blocking so that we can reliably get the data
#ifndef TA_OS_WIN // ???? 
    rc = ::fcntl(m_pipeFd[0], F_GETFL);
    assert(rc != -1);
    rc = ::fcntl(m_pipeFd[0], F_SETFL, rc | O_NONBLOCK); // otherwise atEnd() will block!
    assert(rc != -1);
#endif
    FILE * f = fdopen(m_pipeFd[0], "r");
    assert(f != 0);

    if (m_stream != 0) delete m_stream;
    if (m_notifier != 0) delete m_notifier;
    m_stream = new QTextStream(f);
    m_notifier = new QSocketNotifier(m_pipeFd[0], QSocketNotifier::Read);
    QObject::connect(m_notifier, SIGNAL(activated(int)), SLOT(received()));
}

void iInterceptor::received()
{
    emit received(m_stream);
}

void iInterceptor::finish()
{
    if (m_notifier == 0) return;
   
    std::cout.flush(); // flush standard output cout file descriptor
    ::fflush(NULL);    // flush all file buffers
#ifndef TA_OS_WIN // ???? 
    ::fsync(1);        // syncronize standard output buffers -- may be unnessessery
#endif
    received();        // process whatever data is left there
   
    // Restore original state
    delete m_notifier;
    delete m_stream;
    m_notifier = 0;
    m_stream = 0;
   
    ::dup2(m_origFdCopy, m_origFd); // restore the output descriptor
    ::close(m_origFdCopy); // close the copy as it's redundant now
    ::close(m_pipeFd[0]);  // close the reading end of the pipe
}
