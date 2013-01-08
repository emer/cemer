// Carnegie Mellon University, Princeton University.
// Copyright, 1995-2007, Regents of the University of Colorado,
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


// ta_type.cc: Type Access Main code

#include "ta_type.h"
#include "ta_variant.h"

#ifndef NO_TA_BASE
# define CMAKE_DEPENDENCY_HACK(a) #a
#  include CMAKE_DEPENDENCY_HACK(svnrev.h)
# undef CMAKE_DEPENDENCY_HACK

# include "ta_group.h"
# include "ta_dump.h"
# include "ta_project.h" // for taRootBase
# include "ta_program.h"
# include "colorscale.h"
# include "ta_thread.h"
# ifdef DMEM_COMPILE
#   include "ta_dmem.h"
# endif
# include "ta_TA_type_WRAPPER.h"
# include <QDir>
# include <QFileInfo>
# include <QCoreApplication>
# include <QTimer>
# include <QDateTime>
# include <QNetworkInterface>
# include <QNetworkAddressEntry>
# include <QHostAddress>
# include <QList>
# include "css_machine.h"       // for setting error code in taMisc::Error
# ifdef TA_GUI
#   include "ta_qtdata.h"
#   include "ta_qttype.h"
#   include <QMainWindow>
#   include "igeometry.h"
# endif // TA_GUI
#endif // NO_TA_BASE


#include <sstream>              // for FormatValue
#include <ctime>
#include <ctype.h>
#include <stdio.h>
#include <signal.h>
#ifndef TA_OS_WIN
# include <sys/time.h>
# include <sys/times.h>
#endif




#endif // NO_TA_BASE


