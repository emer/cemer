// Copyright, 1995-2007, Regents of the University of Colorado,
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


#include "ta_qtviewer.h"

#include "ta_qt.h"
#include "ta_qtdata.h"
#include "ta_qtdialog.h"
#include "ta_qttype.h"
#include "ta_qtclipdata.h"
#include "ta_script.h"
#include "ta_project.h"
#include "ta_qtclassbrowse.h"
#include "ta_qtgroup.h"

#include "css_qt.h"
#include "css_qtdialog.h"
#include "css_machine.h"
#include "css_basic_types.h"

#include "icolor.h"
#include "inetworkaccessmanager.h"

#include <QAction>
#include <QApplication>
#include <qbuttongroup.h>
#include <qclipboard.h>
#include <QDesktopServices>
#include <qdialog.h>
#include <qevent.h>
#include <QButtonGroup>
#include <QFileInfo>
#include <QHeaderView>
#include <qimage.h>
#include <QLayout>
#include <qmenubar.h>
#include <QList>
#include <QMenu>
#include <QNetworkReply>	
#include <QScrollArea>
#include <QScrollBar>
#include <QStackedWidget>
#include <QStackedLayout>
#include <QStatusBar>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTimer>
#include <QToolBar>
#include <QToolBox>
#include <QToolTip>
#include <QTreeWidget>
#include <qvariant.h>
#include <QVBoxLayout>
#include <qwhatsthis.h>
#include <QWebFrame>
#include <QWebPage>
#include <QWebView>
#include <QProgressBar>

#include "itextbrowser.h"
#include "itextedit.h"
#include "iscrollarea.h"

using namespace Qt;
/*TODO: nuke
static const unsigned char image0_data[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0x74, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xed, 0xd5, 0xc1, 0x09, 0xc0,
    0x20, 0x0c, 0x05, 0xd0, 0x6f, 0xe9, 0x36, 0x81, 0x2c, 0x10, 0xb2, 0xff,
    0xdd, 0x85, 0xd2, 0x53, 0x85, 0xb6, 0xa9, 0x91, 0x48, 0x0f, 0x05, 0x3f,
    0x08, 0x1a, 0xf0, 0x29, 0x12, 0x10, 0xf8, 0x28, 0xc5, 0xa9, 0xd9, 0xc4,
    0xde, 0x96, 0xcd, 0x2b, 0x9a, 0xd9, 0xeb, 0x00, 0x00, 0x66, 0x0e, 0x2f,
    0xe0, 0xc2, 0x51, 0x98, 0x39, 0xc4, 0xf7, 0x0c, 0x4c, 0x44, 0x6d, 0x5e,
    0x6b, 0x35, 0x38, 0xcf, 0x92, 0x82, 0x45, 0xe4, 0xb2, 0xf6, 0xf0, 0x14,
    0xac, 0xaa, 0x8f, 0xda, 0x1d, 0x4f, 0xc1, 0xa5, 0x74, 0x1b, 0x22, 0x07,
    0x9f, 0x9d, 0x11, 0x1d, 0x96, 0xea, 0x8a, 0x91, 0x2c, 0x78, 0xc1, 0x0b,
    0xee, 0x64, 0xe6, 0x07, 0x19, 0xf5, 0x7e, 0x92, 0x03, 0xad, 0x45, 0x2a,
    0x04, 0x03, 0x18, 0xf5, 0xf7, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e,
    0x44, 0xae, 0x42, 0x60, 0x82
};

static const unsigned char image1_data[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0x99, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xed, 0x94, 0x41, 0x0e, 0x85,
    0x20, 0x0c, 0x44, 0x5f, 0x89, 0xc7, 0x36, 0x7f, 0x61, 0xbc, 0x77, 0x5d,
    0x28, 0x48, 0xa4, 0x28, 0x60, 0xff, 0xce, 0xd9, 0x54, 0x8b, 0xbe, 0x8e,
    0x13, 0x04, 0x3e, 0x1d, 0x92, 0x81, 0x77, 0xf4, 0x81, 0xa1, 0x23, 0xdc,
    0x2b, 0x34, 0xf6, 0xf4, 0x7a, 0x3d, 0xe2, 0xb8, 0x65, 0xa8, 0x84, 0x3f,
    0x40, 0x01, 0x98, 0x2a, 0x0b, 0x3d, 0x5f, 0x62, 0xc5, 0x83, 0x00, 0xaa,
    0x1a, 0xd7, 0x05, 0x50, 0x44, 0x9a, 0xb9, 0xd5, 0x07, 0xa7, 0x73, 0xa8,
    0xa4, 0xba, 0x4f, 0x92, 0xa2, 0xdf, 0x33, 0x3c, 0x64, 0xc6, 0x3b, 0xeb,
    0xbd, 0x82, 0xe5, 0xb8, 0xad, 0xde, 0xcb, 0xcc, 0x78, 0x20, 0xeb, 0x42,
    0x66, 0xc6, 0x39, 0x74, 0x5d, 0xfa, 0x80, 0xf3, 0x6f, 0xaf, 0x66, 0xc6,
    0x6f, 0xa1, 0x9c, 0x3f, 0x88, 0x2f, 0xb4, 0x70, 0xec, 0x05, 0xcd, 0xc0,
    0xbe, 0xd0, 0x78, 0x93, 0xf6, 0x8e, 0x17, 0x14, 0x92, 0x63, 0x5f, 0x68,
    0x6c, 0x3e, 0xef, 0xf6, 0xba, 0x3c, 0x8f, 0xdd, 0x36, 0x6d, 0xc4, 0xc0,
    0x45, 0x2c, 0xf1, 0x53, 0xa9, 0x59, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45,
    0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static const unsigned char image2_data[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0xa0, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xd5, 0x95, 0x4d, 0x0a, 0x80,
    0x20, 0x10, 0x85, 0x9f, 0xd1, 0x46, 0x68, 0xe1, 0x8d, 0xe6, 0x62, 0xd2,
    0x22, 0xbc, 0x98, 0x37, 0x6a, 0x21, 0xb4, 0xac, 0x45, 0x19, 0x92, 0xc6,
    0x64, 0x69, 0xe0, 0xb7, 0xf1, 0x87, 0xf1, 0xf1, 0x1c, 0x47, 0x05, 0x2a,
    0x21, 0x8e, 0x76, 0x2d, 0xad, 0xdb, 0xfb, 0x9e, 0x99, 0xf6, 0x56, 0x8f,
    0x80, 0xb5, 0x36, 0x4b, 0x85, 0x88, 0xce, 0x35, 0x44, 0x04, 0x00, 0xe8,
    0x0a, 0x39, 0x8c, 0xe8, 0xf9, 0x90, 0x34, 0xd2, 0x29, 0x2c, 0xc3, 0x7c,
    0x8e, 0xbd, 0x53, 0x0f, 0xeb, 0x58, 0x3a, 0x05, 0xe9, 0x54, 0x34, 0x1f,
    0x8a, 0x02, 0x7b, 0x2a, 0x7d, 0x3a, 0x1f, 0x09, 0xbf, 0x85, 0x4d, 0xc5,
    0xd5, 0xd9, 0x53, 0xaa, 0x39, 0x6e, 0x4f, 0x38, 0xca, 0xb1, 0x99, 0xe2,
    0xd2, 0xe1, 0x08, 0xab, 0xe1, 0x56, 0xf8, 0x2e, 0x30, 0x97, 0x7f, 0xcb,
    0x4d, 0x8f, 0xf9, 0x42, 0xd7, 0x5d, 0xbe, 0xbe, 0xd2, 0xe1, 0x43, 0x95,
    0x3a, 0x93, 0xf6, 0xca, 0xad, 0x3d, 0x61, 0x11, 0xf4, 0x4b, 0x7d, 0x4f,
    0x82, 0x0f, 0xf9, 0xc0, 0x06, 0x9b, 0xb5, 0x1e, 0xcd, 0x12, 0x20, 0x66,
    0x74, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60,
    0x82
};

static const unsigned char image3_data[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x02,
    0x9c, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0x8d, 0x95, 0xad, 0x76, 0xdb,
    0x40, 0x10, 0x85, 0x3f, 0xf7, 0x18, 0xcc, 0x32, 0x89, 0xd9, 0x50, 0xd0,
    0x61, 0x36, 0x34, 0x4c, 0x98, 0xc3, 0x62, 0x96, 0x40, 0x87, 0x25, 0x6f,
    0x50, 0x3f, 0x42, 0x61, 0x61, 0x02, 0x1b, 0xe6, 0xb2, 0x84, 0x25, 0x50,
    0x61, 0x2e, 0x8b, 0xe1, 0x42, 0x99, 0x49, 0x6c, 0x86, 0x6d, 0xc1, 0x4a,
    0xb2, 0xfc, 0x77, 0xda, 0x21, 0x92, 0x66, 0x57, 0x77, 0xee, 0xdc, 0x3b,
    0x5a, 0xf5, 0x38, 0x13, 0xaf, 0xaf, 0xaf, 0x41, 0x44, 0x48, 0xd3, 0x74,
    0x2f, 0x6f, 0x66, 0x00, 0xa8, 0x2a, 0x00, 0x55, 0x55, 0x91, 0x24, 0x09,
    0x57, 0x57, 0x57, 0xbd, 0xee, 0xbe, 0xfe, 0x39, 0x60, 0x11, 0x61, 0x32,
    0x99, 0xb4, 0x40, 0x87, 0x6b, 0x4d, 0x94, 0x65, 0x89, 0xf7, 0xfe, 0x68,
    0xcf, 0x59, 0x60, 0x80, 0xcd, 0x66, 0x73, 0x04, 0x76, 0x58, 0x48, 0x55,
    0x71, 0xce, 0xfd, 0x3f, 0xf0, 0x29, 0x00, 0x33, 0x3b, 0x2a, 0x70, 0xaa,
    0x23, 0x80, 0x6f, 0xa7, 0x92, 0x79, 0x9e, 0x07, 0x33, 0x6b, 0x99, 0x38,
    0xe7, 0x70, 0xce, 0xed, 0xe9, 0xdd, 0xe8, 0x2f, 0x22, 0x47, 0xfa, 0x9e,
    0x65, 0xac, 0xaa, 0x24, 0x49, 0x42, 0x59, 0x96, 0x88, 0x48, 0x6b, 0x54,
    0x37, 0x4e, 0xb5, 0xff, 0x4f, 0xc6, 0x10, 0x5b, 0x3c, 0x9c, 0x88, 0x2e,
    0x68, 0x53, 0xec, 0x9c, 0x14, 0x27, 0x19, 0x37, 0x6c, 0x4e, 0x31, 0xed,
    0xe6, 0x55, 0x75, 0x6f, 0x42, 0xba, 0x71, 0xa4, 0x0d, 0xc0, 0x6a, 0xb5,
    0x0a, 0x59, 0x96, 0x31, 0x1c, 0x0e, 0xcf, 0x82, 0x37, 0x46, 0x7e, 0x7e,
    0x7e, 0x02, 0x20, 0x92, 0x30, 0x9f, 0x5f, 0xb7, 0x78, 0x7b, 0x8c, 0xdf,
    0xdf, 0xdf, 0x83, 0xf7, 0x9e, 0xfc, 0x23, 0x47, 0x66, 0x82, 0x88, 0xb4,
    0x00, 0x87, 0xd7, 0x86, 0x69, 0x59, 0x94, 0xe4, 0x79, 0xce, 0xb6, 0xda,
    0xf2, 0xf0, 0xf0, 0x10, 0x66, 0xb3, 0x19, 0xd7, 0xd7, 0xd7, 0xbd, 0x5e,
    0x17, 0x74, 0xb3, 0xf1, 0x54, 0xc5, 0x16, 0x35, 0x80, 0xd3, 0x4c, 0x01,
    0x9c, 0xa4, 0x08, 0x02, 0x0e, 0x7c, 0xe1, 0x59, 0xaf, 0xff, 0xb0, 0xdd,
    0x16, 0xa8, 0x1a, 0x17, 0x17, 0x19, 0x8b, 0xc5, 0x22, 0x4a, 0xd1, 0x30,
    0xbd, 0x9c, 0x5e, 0xe2, 0xd2, 0x14, 0x55, 0x03, 0x53, 0x8e, 0x6c, 0x31,
    0x03, 0x84, 0x9c, 0x4f, 0x3e, 0x78, 0x65, 0x6a, 0x53, 0xd2, 0xaf, 0x94,
    0xe7, 0x97, 0x67, 0xfc, 0x57, 0xfc, 0xfa, 0xd4, 0x94, 0x6c, 0x74, 0x11,
    0x41, 0x9f, 0x9e, 0x7e, 0x85, 0xb2, 0x28, 0xc3, 0xff, 0xc4, 0x57, 0xf8,
    0x0a, 0xa3, 0x30, 0x0a, 0x12, 0x24, 0x8c, 0xc2, 0x28, 0xac, 0xd7, 0xeb,
    0xf0, 0xe3, 0xfb, 0xcf, 0x30, 0x1e, 0x8f, 0xc3, 0x60, 0x90, 0x85, 0x24,
    0x49, 0x42, 0x36, 0xc8, 0x42, 0xbf, 0xda, 0x56, 0xdc, 0xdd, 0xdd, 0x9c,
    0x75, 0xf7, 0x30, 0x52, 0x52, 0x2e, 0x99, 0x92, 0x23, 0xcc, 0x98, 0x31,
    0x1e, 0x8f, 0x49, 0x64, 0x48, 0x69, 0x05, 0xcf, 0xbf, 0x5e, 0xa8, 0xaa,
    0x8a, 0x74, 0x90, 0xd2, 0x37, 0xc0, 0xfb, 0x22, 0xce, 0xa3, 0x19, 0x88,
    0x10, 0x6b, 0x48, 0xed, 0x36, 0x38, 0x5c, 0x54, 0xdc, 0x14, 0xc4, 0xf1,
    0x60, 0xdf, 0xb9, 0xc1, 0x33, 0xb4, 0x21, 0x7f, 0xd8, 0x80, 0x19, 0xe9,
    0x70, 0x18, 0xd7, 0x6b, 0x77, 0xfa, 0x65, 0x51, 0xe0, 0x45, 0xa2, 0x9e,
    0x66, 0xb4, 0xbe, 0x39, 0x88, 0x2e, 0xd6, 0x9d, 0x38, 0x03, 0x15, 0x20,
    0xe6, 0x04, 0xf0, 0xb6, 0xc5, 0x88, 0x67, 0x88, 0xdf, 0x6c, 0x5a, 0x4f,
    0x1c, 0xf5, 0xb8, 0x35, 0x09, 0x6b, 0x00, 0xb1, 0x76, 0x28, 0x14, 0x8b,
    0x35, 0x74, 0x6f, 0x67, 0x3b, 0x39, 0xd2, 0x78, 0xda, 0x09, 0x45, 0xe9,
    0x23, 0x60, 0x65, 0xe7, 0x05, 0xad, 0xc9, 0x76, 0x37, 0x1a, 0x20, 0x0a,
    0x76, 0xb8, 0xe2, 0x30, 0x2b, 0xa9, 0xfb, 0x6c, 0x7a, 0x63, 0x32, 0x99,
    0xf2, 0x0d, 0xeb, 0xb0, 0x6c, 0xc9, 0x6a, 0x7c, 0xb4, 0xfa, 0xba, 0x07,
    0xea, 0x9a, 0x6d, 0x35, 0x68, 0x0d, 0x58, 0xcb, 0x39, 0x18, 0x0c, 0x58,
    0x2c, 0xee, 0x22, 0x63, 0xef, 0x7d, 0x63, 0x15, 0x88, 0x41, 0x25, 0x40,
    0x15, 0x9d, 0x33, 0x8b, 0x30, 0xd2, 0xb0, 0xb2, 0x1d, 0x18, 0x3b, 0xcd,
    0x31, 0x43, 0x04, 0x96, 0xcb, 0x25, 0xf3, 0xf9, 0xbc, 0xd7, 0xcf, 0xb2,
    0x8c, 0x8f, 0xb7, 0x0f, 0x7e, 0xbf, 0xbd, 0xa1, 0x6a, 0xc4, 0xf3, 0x47,
    0xd8, 0x1b, 0x3e, 0xe9, 0x3c, 0xcb, 0x0e, 0xb2, 0xed, 0xb3, 0x9e, 0xa6,
    0xe5, 0x72, 0xc9, 0xe3, 0xe3, 0x63, 0x0f, 0x3a, 0x87, 0xd0, 0x6a, 0xb5,
    0x0a, 0xab, 0xd5, 0x1b, 0xdb, 0xfa, 0xff, 0xa5, 0x68, 0x6d, 0xca, 0xce,
    0x99, 0xdd, 0x5f, 0x03, 0x54, 0xcb, 0x78, 0x5f, 0x19, 0x93, 0xe9, 0x84,
    0xdb, 0xdb, 0x5b, 0xee, 0xef, 0xef, 0x5b, 0xbc, 0xbf, 0xd1, 0xf6, 0x9e,
    0x0c, 0x0f, 0x84, 0x48, 0x03, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e,
    0x44, 0xae, 0x42, 0x60, 0x82
};

static const unsigned char image4_data[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0x73, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xed, 0x92, 0xc1, 0x0e, 0x80,
    0x20, 0x08, 0x40, 0xb1, 0xef, 0x66, 0x1d, 0x1c, 0xff, 0x4d, 0x87, 0x6a,
    0xa8, 0x88, 0xa1, 0x76, 0x69, 0xf9, 0x36, 0x0f, 0x28, 0x3e, 0xd8, 0x00,
    0x60, 0xf1, 0x59, 0x42, 0x5f, 0x3a, 0x71, 0xf5, 0x36, 0x02, 0xe0, 0x8e,
    0x99, 0x2b, 0x09, 0x88, 0x01, 0xd0, 0x28, 0x54, 0x17, 0x6a, 0xe4, 0x7f,
    0x21, 0xce, 0x1f, 0xb5, 0xb0, 0x5d, 0x38, 0xed, 0xdc, 0x90, 0x60, 0xd0,
    0xf1, 0x13, 0x79, 0x63, 0x5b, 0x3b, 0xc9, 0x2b, 0xd5, 0x18, 0xe2, 0x39,
    0xa9, 0x43, 0xec, 0x1d, 0x5a, 0xb7, 0x78, 0x5c, 0xee, 0x10, 0x7b, 0xe4,
    0xb2, 0x15, 0xaf, 0x40, 0x91, 0xf8, 0x94, 0xde, 0x47, 0x18, 0x1e, 0xce,
    0xa5, 0x9e, 0xde, 0x9e, 0xc5, 0x9f, 0x38, 0x00, 0x62, 0xac, 0x28, 0xb1,
    0x0f, 0xe7, 0x4f, 0xef, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44,
    0xae, 0x42, 0x60, 0x82
};

static const unsigned char image5_data[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0x74, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xed, 0x92, 0xc1, 0x0a, 0xc0,
    0x20, 0x08, 0x40, 0x6d, 0xdf, 0x2d, 0x3b, 0x84, 0xff, 0xed, 0x0e, 0xa3,
    0x58, 0x6a, 0x26, 0xd1, 0x65, 0xe0, 0x83, 0x0e, 0xa5, 0x3e, 0x85, 0x04,
    0x48, 0x7e, 0x4b, 0x91, 0x0f, 0x54, 0x89, 0xf1, 0x9e, 0xa5, 0xa3, 0xca,
    0x0f, 0x8a, 0x89, 0x63, 0x65, 0xb3, 0x06, 0xc4, 0x2d, 0xd6, 0x13, 0xc6,
    0x49, 0xbd, 0xc2, 0x59, 0x83, 0x16, 0x13, 0x62, 0x19, 0xf0, 0xf9, 0x36,
    0xc0, 0xa2, 0xef, 0x00, 0xd7, 0x5a, 0x62, 0x61, 0x4d, 0x3a, 0xb2, 0x29,
    0x96, 0xf2, 0xa3, 0x62, 0xff, 0xa3, 0x37, 0xc5, 0xeb, 0xed, 0xe9, 0x62,
    0xaa, 0xd1, 0xa2, 0xe8, 0x4a, 0xaa, 0xa2, 0xf7, 0x50, 0xdd, 0x12, 0x74,
    0x8c, 0x0f, 0xd0, 0xab, 0x93, 0x24, 0x67, 0x78, 0x00, 0x59, 0x6e, 0x28,
    0xb1, 0xbb, 0x69, 0xe3, 0x51, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e,
    0x44, 0xae, 0x42, 0x60, 0x82
};

static const unsigned char image6_data[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0x82, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xcd, 0xd3, 0x41, 0x12, 0x80,
    0x20, 0x08, 0x05, 0x50, 0xe8, 0xe0, 0x2e, 0xbc, 0x38, 0xad, 0x32, 0x73,
    0x50, 0x3e, 0x48, 0x53, 0x7f, 0xe3, 0xe4, 0x8c, 0x4f, 0x24, 0x25, 0xfa,
    0x28, 0xe2, 0x9c, 0x6f, 0x39, 0x92, 0x0b, 0xf9, 0x27, 0x6c, 0xb6, 0x01,
    0x85, 0x35, 0x88, 0x77, 0x61, 0x13, 0x88, 0xc2, 0x57, 0x64, 0x18, 0xcd,
    0xa0, 0x15, 0xf5, 0x20, 0xb4, 0xe6, 0xb5, 0x5b, 0xe1, 0x09, 0xdc, 0x06,
    0x22, 0xb8, 0xe2, 0x2a, 0xcf, 0x31, 0x05, 0x6e, 0x18, 0xdf, 0xdf, 0xf8,
    0x06, 0x06, 0xaa, 0x55, 0x1c, 0xc6, 0x35, 0x64, 0xc4, 0xdc, 0xf8, 0x0c,
    0xd0, 0x20, 0x1d, 0x57, 0x7a, 0x5c, 0x85, 0xa8, 0x84, 0x5f, 0xdc, 0x02,
    0x5e, 0xa5, 0x30, 0x7a, 0xfc, 0xcd, 0x07, 0xe2, 0x3a, 0x1d, 0xf2, 0x83,
    0xec, 0x2b, 0x37, 0xd9, 0xad, 0x5f, 0xb4, 0xdf, 0xef, 0xd4, 0x9c, 0xfb,
    0xf7, 0x2f, 0xac, 0xb6, 0x51, 0xb8, 0x9d, 0x00, 0x00, 0x00, 0x00, 0x49,
    0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static const unsigned char image7_data[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0xbf, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xd5, 0x93, 0x41, 0x0a, 0x83,
    0x30, 0x10, 0x45, 0xdf, 0x48, 0x8e, 0xe5, 0x1d, 0xbc, 0x8c, 0x3b, 0xa9,
    0x8b, 0xf4, 0x6a, 0x9e, 0xab, 0xd3, 0x85, 0x35, 0x0d, 0x26, 0x63, 0x62,
    0x44, 0x4a, 0x3f, 0x0c, 0x42, 0x66, 0xfc, 0xf3, 0xf8, 0x24, 0xf0, 0x6f,
    0x12, 0x40, 0x2b, 0x66, 0xda, 0x8c, 0x55, 0xf3, 0xde, 0x22, 0x12, 0xcf,
    0x9d, 0x92, 0xcb, 0x98, 0xc0, 0xba, 0x2d, 0x7c, 0x45, 0x44, 0xcf, 0x9a,
    0x07, 0x63, 0x8b, 0xba, 0xd5, 0x3c, 0x44, 0x91, 0x23, 0x5e, 0xcf, 0x7c,
    0xc1, 0x62, 0x36, 0x97, 0xa9, 0x25, 0x40, 0xc1, 0x1f, 0xf4, 0xfd, 0xa7,
    0x52, 0x75, 0x01, 0x5d, 0x24, 0xa9, 0x38, 0x9e, 0x7d, 0x6f, 0x53, 0xdf,
    0x4f, 0xe4, 0xcc, 0xab, 0x32, 0x3e, 0xea, 0x0f, 0x03, 0xc0, 0xc4, 0xb2,
    0xa0, 0x71, 0x2c, 0xe6, 0xad, 0xd8, 0x9b, 0x59, 0xb7, 0x66, 0x1c, 0x3b,
    0xe0, 0x95, 0x98, 0x5f, 0x26, 0x16, 0x79, 0xee, 0x4e, 0xbc, 0xc2, 0x2c,
    0x97, 0x88, 0x55, 0x1f, 0xe6, 0xa2, 0xcb, 0xc4, 0x96, 0x9a, 0x89, 0x4b,
    0xcb, 0x6f, 0x23, 0xee, 0x36, 0x1a, 0xab, 0x62, 0xe2, 0x52, 0xc5, 0x72,
    0x94, 0xdf, 0xbf, 0xb6, 0x10, 0xbb, 0xf2, 0xc8, 0x97, 0xb8, 0xa4, 0x6c,
    0xc6, 0x67, 0x7e, 0xaa, 0x51, 0x95, 0x71, 0xfa, 0x08, 0x7e, 0xa8, 0x37,
    0x62, 0xda, 0x9a, 0xba, 0x45, 0xb6, 0x37, 0x52, 0x00, 0x00, 0x00, 0x00,
    0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

static const unsigned char image8_data[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0xd5, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xc5, 0x95, 0x5d, 0x0a, 0x84,
    0x30, 0x0c, 0x84, 0x27, 0xe2, 0xa9, 0x0a, 0x9e, 0x6c, 0x8b, 0x0f, 0x4b,
    0x3d, 0xd9, 0x82, 0xd7, 0xca, 0x3e, 0x58, 0xd7, 0xfe, 0x4c, 0xd0, 0xba,
    0x5d, 0x76, 0x40, 0x02, 0x4d, 0xf2, 0x65, 0xda, 0x0a, 0x05, 0x7e, 0x24,
    0x39, 0xc9, 0xeb, 0x8d, 0x9e, 0xaa, 0x88, 0x41, 0xa0, 0xc9, 0xaa, 0xd8,
    0xc8, 0x2a, 0xb3, 0x2f, 0x9c, 0x42, 0x5b, 0xe1, 0xe3, 0x0e, 0x0d, 0xcf,
    0x00, 0xc0, 0x03, 0x08, 0xf0, 0xb3, 0xa7, 0xa0, 0x74, 0x10, 0xa9, 0xd7,
    0x14, 0x2e, 0x00, 0xb4, 0x2c, 0x5a, 0x5f, 0xab, 0x69, 0x6b, 0x97, 0x9b,
    0x1c, 0x83, 0x7f, 0xc0, 0xc3, 0x16, 0xb6, 0xe4, 0x16, 0x5b, 0x64, 0xf7,
    0x8d, 0x71, 0x63, 0x59, 0x91, 0x9b, 0xdc, 0x45, 0x70, 0xde, 0x47, 0xc0,
    0x47, 0x32, 0xdd, 0x5e, 0x5b, 0xcc, 0x35, 0xf0, 0xc9, 0x77, 0x62, 0xae,
    0x78, 0x79, 0x36, 0xdc, 0xcf, 0x75, 0x13, 0x57, 0x7e, 0x79, 0xf4, 0x8c,
    0x4b, 0x27, 0xaa, 0x0f, 0x13, 0x27, 0xb2, 0x40, 0xf5, 0x11, 0x7f, 0xcb,
    0xe3, 0x48, 0xaa, 0x33, 0xb6, 0xe0, 0x22, 0x4b, 0x05, 0x4d, 0x07, 0x46,
    0xb8, 0x02, 0x5e, 0x2e, 0x3b, 0x3e, 0x73, 0xcd, 0xe0, 0xdd, 0x1c, 0x97,
    0xf0, 0x2e, 0x8e, 0xd9, 0xd0, 0xaf, 0x1d, 0xb3, 0x81, 0x22, 0x4b, 0xdf,
    0x33, 0xee, 0xe6, 0x98, 0xa9, 0x34, 0xa0, 0xf6, 0x17, 0xb4, 0x55, 0x40,
    0xd0, 0x0b, 0xcf, 0x4c, 0xa0, 0x8f, 0xc0, 0xdf, 0xf4, 0x06, 0xe3, 0x25,
    0xc1, 0x98, 0xb1, 0x6b, 0x67, 0xfb, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45,
    0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};
*/


/* Viewing/Browsing hierarchies

  CLASS HIERARCHY

  (taDataViewer)
    DataViewer -- (abstract) main taBase persistable object for controlling viewing windows
      DataBrowser [ta_qtbrowse.h] -- controller for tree-based data browser
      T3Viewer [t3viewer.h] --  controller for 3d-based data navigator

  (QMainWindow)
    iMainWindowViewer -- (abstract) main window class that can be used by DataViewer classes; manages menus,
          docking toolbars
      iTabViewer -- (abstract) subclass that handles tabbed data panels
        iDataBrowser -- window for tree-based browsing
        iT3Viewer -- window for Inventor-based browsing

  (QFrame, IDataLinkClient)
    iDataPanel -- basic panel that can appear in tabbed viewers -- note that panels are not tied
          to any particular viewing subsystem, so can appear i.e., in browsing and 3d viewing
      iDataPanelSet -- contains any number of other types of panels, with a way to activate each
      iBrListDataPanel -- contains a list view of the item selected in the browse tree
      EditPanel -- property list -- based on the v3.2 EditDialog


  (QTreeWidgetItem, IDataLinkClient)
    BrListViewItem  -- (abstract) nodes in either the tree view (left side), or list views (right panel)
      taiTreeDataNode -- (abstract) nodes in the tree view (left side)
        tabTreeDataNode -- nodes for taBase data items
          tabListTreeDataNode -- nodes for taList data items
            tabGroupTreeDataNode -- nodes for taGroup data items
      taiListDataNode -- nodes in the list views (right panel)

  (QTreeWidget)
    iListView [implementation class, ta_qtbrowse.cc] -- light subclass of the Qt ListView

  (QWidget)
    iTabView -- pane with tabs, for showing panels; a iMainWindowViewer can split-nest/unsplit-unnest these
        ad infinitum

  (QTabBar)
    iTabBar -- tab bar in iTabView right hand side

  (QTab)
    iTab -- tabs in the iTabBar -- has a reference to its panel



  OWNERSHIP

  DataViewer (1)
    iMainWindowViewer (0:1)

  DataBrowser (1)
    iDataBrowser (0:1)
      iListView (1) -- on right hand side (tree view)
      iTabView (1:M) -- on left, recursively nested hor or ver, 2 per splitter
        iTabBar (1)
          iTab (1:M) -- always at least one tab (even if empty), user can add new tabs
            (iDataPanel) (0:1) -- contains ref to one of the TabView's panels
        iDataPanel (0:M) -- any number of data panels, in a widget stack


  CREATION SEQUENCES

  1. Interactively (by user)

    * create a DataViewer dv of the correct subclass
    * call dv->OpenWindow()
    * creates the correct iMainWindowViewer window class, and all its various subcomponents,
      such as list views, toolbars, etc.

  2. Loading from file (ex. when loading a project)

    * create the DataViewer dv of the correct subclass, during load
    * put dv on the winbMisc::unopened_windows list
    * when loading is complete, in the event loop, the u_w list is processed
    * for each u_w list item dv, call dv->OpenWindow()


  DESTRUCTION SEQUENCES

  1. Window Close -- the user closes the window  (but can be stopped by unsaved change prompts)

    for each iTabView tv
      for each iDataPanel dp
        call dp->Closing, with option to cancel (ex. if unsaved changes, and user responds "Cancel")

  2. Data Item deletion -- a data item being viewed is deleted

    TreeNodes: simply delete the node; this deletes all subnodes, and removes from display

    Panels: delete every iTab with which the panel is associated (except leave at least one tab)



*/

/* Clipboard Handling

  A subcontrol that wants to control clipboard handling must provide a signal slot interface,
  as follows; the slots/signals can have any names (* is optional):

  Slots:
    void EditAction(int); // called when user requests the indicated edit action, 
      ex. via menu or accel key
    void GetEditActions(int&); // called to get current valid edit actions
    *void SetActionsEnabled(TBD); // enables/disables any actions
  Signals:
    *void UpdateUi(); // control can call if something changed, to update the ui -- calls back its
       GetEditAction and SetActionsEnabled slot functions


  When a clipboard-enabled control (ex. the data browser tree) gets the focus, it should call:

    SetClipboardHandler(QWidget* handler_obj, ...) [see object below for api details]

  If a control knows that it is no longer active,  it should call.

    SetClipboardHandler(NULL)

  NOTE: however you can't call the above just because you lose focus, because this happens
    normally. ex. an edit control has focus, then user clicks on a toolbar button -- the
    edit control loses focus. Instead, the mechanism used is that basically unless something
    actively grabs the handler, it is left with the previous value. The taiData objects
    have a base mechanism built in so that when an implementation control gets focus, it
    unregisters clipboard handling, by default. Therefore, only taiData controls that implement
    it will actually get the focus, but simply clicking away on a toolbar button won't
    dehandle.

*/

/* Selection Handling

  Selection handling is the system that handles user selection of items in the gui,
  and the corresponding changes that take place. For example, the user selects an
  item in the tree, which causes its corresponding panel to display. Or, the user
  selects a 3D item in the T3 viewer. Some areas permit multiple items to be selected;
  examples are T3 and ListViews.
  The system does not make a distinction between "current" and "selected" (ex. as is made
    by Qt in its ListView).
    
  A selection handler implies being a Clipboard Handler -- the ISelectableHost i/f
  provides an implementation ("ActionsEnabled" and "UpdateUi" are not needed.)
  
  Selections affect:
    gui -- things like panels change when selections change
    clip -- legal clipboard operations change, depending on selection
    actions -- selected items have actions, and multiple selections
        have dynamic actions, such as adding a projection between layers
    context menus -- these need access to selection
    
  Selection Sources/Sinks
  
  A gui item can be a selection source, sink, or both. A selection Source is something
  the user can work in to make selections. 
  Sources:
    * tree nodes (browser, list views, specials, such as prog editor)
    * T3 viewer
  Sinks:
    * panel tabs (note: modal)
    * main window -- updates the Actions menu and actions
  How/When things become sinks is modal -- if the user is in the tree browser, then
  the panels should update; but if the user is in the prog editor, that containing
  panel should NOT update as the user clicks around on the items; however, the user
  still expects the clip menu items to be valid.
  

  Selection handling involves the following elements:
  
  Interfaces:
    ISelectableHost -- interface implemented by an object that supports selectable items
    ISelectable -- interface of an item that can be selected in the gui
  
  Main Menu:
    Edit/clip menu -- enabling of clip items is based on selection
    Actions menu -- contains the actions available, based on selection
    
    
  ISelectableHost i/f -- for sources of selection
    enum NotifyOp: GotFocus, SelectionChanged, Destroying
    abstract public virtuals you must implement:
      QWidget* widget() -- provides access to the implementers widget (for signals/slots etc.)
      bool hasMultiSelect() -- true if supports multi select
    abstract protected virtuals you must implement:
      ApplySelectedItems_impl -- called when force=true for changes, force gui to be selItems
      
    implemented virtuals, possible (but not usual) to extend:
      selItems() -- list of selected items
      dynActions() -- Action list, of current dynamic actions available
      dynMethods() -- list of current dynamic methods available
      (the list accessors are virtual, which supports possible JustInTime filling
       of the lists prior to returning them)
      SelectionChanging(bool) -- bracketing batch changes, so only one notify
      ClearSelectedItems() -- forced clearing
      AddSelectedItem(item) -- add the item
      RemoveSelectedItem(item) -- remove the item
      FillContextMenu(taiActions* menu) -- fill the context menu; host can extend
        to put items before or after as well
    convenience members (non virtual)
      handlerObj() -- this is provided so client can connect to us as ClipHandler
      selItem() -- convenience, first selected guy (or NULL if none)
      setSelItem(item, force) -- convenience, for prog setting of single select
      AddDynMenuItems(taiMenu) -- add the dynamic guys to the given menu
    ClipHandler methods:
      EditEnabled(int&) -- return enabled flags
      EditAction(int) -- perform the action
    connection methods:
      Connect_SelectableHostItemRemovingSlot(QObject* src_obj, 
        const char* src_signal, bool disconnect = false) -- connects (or disconnects)
         an optional ItemRemoving notification
      Connect_SelectableHostNotifySignal(QObject* sink_obj,
        const char* sink_signal, bool disconnect = false) -- connects (or disconnects)
         a sink (ex iFrame) to the notify signal raised when sel changes (or 
         gets focus, etc.)
    protected:
      handler -- the impl widget object for signals/slots
      (list members)
      Emit_NotifySignal(op) -- called internally, and also by imlementer when gets focus
      DoDynAction(int) -- called by the slot on helper
      ApplySelectedItems_impl -- called when force=true for changes, force gui to be selItems
      UpdateActions() -- implementation function that updates the dyn methods/actions
        (can be overridden/extended if necessary)
      
    SelectableHostHelper -- this is a helper QObject that handles signals/slots
      ISelectableHost* host -- the owning host
      Emit_NotifySignal(op) -- called to emit the signal
    signals:
      SelectableHostNotifySignal(ISelectableHost* src, op);
    slots:
      DynActionSlot(int) -- callback for dynamic actions
      ItemRemovingSlot(item)
      EditEnabled(int&) -- callback for when we are ClipHandler
      EditAction(int) --  callback for when we are ClipHandler
    
    Client of ISelectableHost:
      members:
        ISelectableHost* cur_host -- the client should keep track of the host
          with focus, so it can ignore other hosts; it should also check for the
          Delete op, and delete this if so
    
      slots: 
        SelectableHostNotify(ISelectableHost* src, op) (name can be anything)
          this is the slot called by the ISelectableHost
      
  Notes:
    * whenever the Actions guys gets cleared for any reason, the actions are deleted,
      and any menu guys that have been created will also be deleted
  Source Changes:
    Source changes (the guy controlling the selectable clients) typically only happen
    explicitly, when that guy gets focus. In that case, it raises a Notify signal
    with GotFocus op, to enable clients to start tracking changes only from that
    guy -- until an explicit focus change happens again, they will typically
    ignore Notifies from other sources.

      
  What the implementing class must do:
    * provide the implementations for the trivial abstract virtuals, like widget()
    * provide an implementation for ApplySelectedItems_impl, which has to take
      the list and then force the gui to reflect the selection in the list
    * when you detect that selection changes in the gui, call the non-force
      member funcs, ex. setSelItem (for one guy), 
      or: SelChanging(t) ... changes .. SelChanging(f) (for multiple guys)
      -- the non-force version won't callback your own ApplySel.. guy
    * detect when your control *receives* focus, and call Emit_Notify(ReceivedFocus)
      this will force the host/clients to configure themselves for our selections 
  
    
  What the client class should do:
    * it should connect its Notify slot to the ISelectableHost via
      Connect_SelectableHostNotifySignal
    * if it connects to more than one source, it should have a member
      to keep track of the one with focus -- if so, it should ignore
      signals from any non-focus guys, update the focus guy, and delete
      it if the delete op is received
    * if it is the main window, when it gets the GotFocus op, 
      it should set the src as the cliphandler by calling:
        SetClipboardHandler(src_host->handlerObj(), 
          SLOT(EditEnabled(int&)), SLOT(EditAction(int)) )
      Note: the cliphandler can disengage independently of ISH
        still being the selection handler
    * when it acts on a valid SelectionChanged, it can update
      its action menus
    * clients must NEVER keep references to the sellists etc. outside
      the signal handler -- if they need to reference them, they must
      do so through the instance pointer they cache (which gets a signal
      on deletion, so client doesn't have to independently connect
      a delete notify signal)
  
  iFrame:
    
    slots:
      SelectableHostNotifySlot_Internal_ -- connects guys nested below us; lets us trap
      SelectableHostNotifySlot_External -- from external guys (forwarded from main window)
      
    signals:
      SelectableHostNotifySignal -- forwarder, from all internal guys
    
*/

//////////////////////////
//   IDataHost	 	//
//////////////////////////

//from ta_qtdata_def.h -- dependencies are here, so makes sense to impl here
void IDataHost::SetItemAsHandler(taiData* item, bool set_it) {
  iMainWindowViewer* dv = viewerWindow();
  if (!dv) return;
  //TODO: we really should check to make sure our class expresses these,
  // and not include the last two if not expressed
  if (set_it) {
    dv->SetClipboardHandler(item,
      SLOT(this_GetEditActionsEnabled(int&)),
      SLOT(this_EditAction(int)),
      SLOT(this_SetActionsEnabled()),
      SIGNAL(UpdateUi()) );
  } else {
    dv->SetClipboardHandler(NULL);
  }
}


//////////////////////////
//   taiDataLink 	//
//////////////////////////

String taiDataLink::AnonymousItemName(const String& type_name, int index) {
  // makes name in form [index]:TypeName
  String rval = "[" + String(index) + "]:" + type_name;
  return rval;
}

taiDataLink::taiDataLink(void* data_, taDataLink* &link_ref_)
:inherited(data_, link_ref_)
{
  qobj = NULL;
}

taiDataLink::~taiDataLink() {
  if (qobj) {delete qobj; qobj = NULL;}
}

void taiDataLink::Assert_QObj() {
  if (!qobj) {
    qobj = new DataLink_QObj(this);
  }
}

iDataPanel* taiDataLink::CreateDataPanel() {
  return CreateDataPanel_impl();
}

iDataPanel* taiDataLink::CreateDataPanel_impl() {
  iDataPanel* rval = NULL;
  TypeDef* typ = GetDataTypeDef();
  //typ can be null for non-taBase classes
  if ((typ == NULL) || (typ->iv == NULL)) return NULL;
  taiViewType* tiv = typ->iv;
  rval = tiv->CreateDataPanel(this);
  return rval;
}


taiTreeDataNode* taiDataLink::CreateTreeDataNode(MemberDef* md, taiTreeDataNode* parent,
  taiTreeDataNode* after, const String& node_name, int dn_flags)
{
  return CreateTreeDataNode(md, parent, NULL, after, node_name, dn_flags);
}

taiTreeDataNode* taiDataLink::CreateTreeDataNode(MemberDef* md, iTreeView* parent,
  taiTreeDataNode* after, const String& node_name, int dn_flags)
{
  return CreateTreeDataNode(md, NULL, parent, after, node_name, dn_flags);
}

taiTreeDataNode* taiDataLink::CreateTreeDataNode(MemberDef* md, taiTreeDataNode* nodePar,
  iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags)
{
  if (HasChildItems()) {
    //note: the list item automatically enables lazy children
    dn_flags |= iTreeViewItem::DNF_LAZY_CHILDREN;
  }
  taiTreeDataNode* rval = CreateTreeDataNode_impl(md, nodePar, tvPar, after, node_name, dn_flags);
  if (rval) {
    rval->DecorateDataNode();
  }
  return rval;
}

void taiDataLink::FillContextMenu(taiActions* menu) {
  FillContextMenu_impl(menu);
}


//////////////////////////////////
// 	tabDataLink		//
//////////////////////////////////

tabDataLink::tabDataLink(taBase* data_, taDataLink* &link_ref_)
:inherited((void*)data_, link_ref_)
{
}

void tabDataLink::ChildQueryEditActions_impl(const MemberDef* par_md, taiDataLink* child,
  taiMimeSource* ms,  int& allowed, int& forbidden)
{
  if (child) {
    TypeDef* td = child->GetDataTypeDef();
    if (td && td->InheritsFrom(&TA_taBase)) {
      data()->ChildQueryEditActions(par_md, (taBase*)child->data(), ms, allowed, forbidden);
      return;
    }
  } else
    data()->ChildQueryEditActions(par_md, NULL, ms, allowed, forbidden);
}

int tabDataLink::ChildEditAction_impl(const MemberDef* par_md, taiDataLink* child,
  taiMimeSource* ms, int ea)
{
  int rval = 0;
  if (child) {
    TypeDef* td = child->GetDataTypeDef();
    if (td && td->InheritsFrom(&TA_taBase)) {
      int rval = data()->ChildEditAction(par_md, (taBase*)child->data(), ms, ea);
      if (rval == 0)
        child->EditAction_impl(ms, ea);
      return rval;
    }
  } else
    rval = data()->ChildEditAction(par_md, NULL, ms, ea);
  return rval;
}

taiTreeDataNode* tabDataLink::CreateTreeDataNode_impl(MemberDef* md, taiTreeDataNode* nodePar,
  iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags)
{
  taiTreeDataNode* rval = NULL;
  if (nodePar)
    rval = new tabTreeDataNode(this, md, nodePar, after, node_name, dn_flags);
  else
    rval = new tabTreeDataNode(this, md, tvPar, after, node_name, dn_flags);
  return rval;
}

String tabDataLink::GetColText(const KeyString& key, int itm_idx) const {
  return data()->GetColText(key, itm_idx);
}

const QVariant tabDataLink::GetColData(const KeyString& key, int role) const {
  return data()->GetColData(key, role);
}

String tabDataLink::GetTypeDecoKey() const {
  return data()->GetTypeDecoKey();
}

String tabDataLink::GetStateDecoKey() const {
  return data()->GetStateDecoKey();
}

bool tabDataLink::isEnabled() const {
  return data()->GetEnabled();
}

void tabDataLink::QueryEditActions_impl(taiMimeSource* ms, int& allowed, int& forbidden) {
  data()->QueryEditActions(ms, allowed, forbidden);
}

int tabDataLink::EditAction_impl(taiMimeSource* ms, int ea) {
  return data()->EditAction(ms, ea);
}

taiDataLink* tabDataLink::ownLink() const {
  if (!m_data) return NULL;
  taBase* own = ((taBase*)m_data)->GetOwner();
  return (own) ? (taiDataLink*)own->GetDataLink() : NULL;
} 

/*
void tabDataLink::fileClose() {
  data()->Close();
}

void tabDataLink::fileOpen() {
  data()->CallFun("Load");
}

void tabDataLink::fileSave() {
  data()->CallFun("Save");
}

void tabDataLink::fileSaveAs() {
  data()->CallFun("SaveAs");
}*/

/* Context Menus are filled in the following order (with indicated separators)
------
Dynamic actions
------
Standard Edit menu items (Cut, Copy, etc.)
------
#MENU_CONTEXT directive methods (tend to be base-ish methods that often appear)
------
#BUTTON directive methods (key operations for the item)
#MENU_BUTTON directive methods (each button gets a submenu)
------
Normal submenus (ex. Object Edit, etc.)
------
Browser/gui framework items 
------

*/
void tabDataLink::FillContextMenu_impl(taiActions* menu) {
  inherited::FillContextMenu_impl(menu);

  TypeDef* typ = GetDataTypeDef();
  if (typ == NULL)  return;

  // add all the #MENU_CONTEXT items first, so they always appear before the nested submenus
  int cnt = 0;
  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    if (!(md->HasOption("MENU_CONTEXT"))) continue;
    if (!md->ShowMethod()) continue;
    if (md->im == NULL)  continue;
    taiMethodData* mth_rep = md->im->GetMenuMethodRep(data(), NULL, NULL, NULL);
    if (mth_rep == NULL)  continue;
    if (cnt == 0) menu->AddSep();
    mth_rep->AddToMenu(menu); 
    ++cnt;
  }

  // add the BUTTON and MENU_BUTTON items
  cnt = 0;
  taiMenu_List ta_menus;
  taiActions* cur_menu = NULL;
  String men_nm = "Misc";  //see note in taiEditDataHost::SetCurMenuButton
  String on_nm;
  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    // we always have to extract the MENU_ON because it has side-effects
    on_nm = md->OptionAfter("MENU_ON_");
    if (on_nm.nonempty()) men_nm = on_nm;
    //check skip conditions
    if (md->HasOption("NO_MENU_CONTEXT") || md->HasOption("MENU_CONTEXT") ||
        !(md->HasOption("BUTTON") || md->HasOption("MENU_BUTTON")) )
        continue;
    if (!md->ShowMethod()) continue;
    if (md->im == NULL)  continue;
    //note: we request the Menu guy because we put it in menu even if BUTTON
    taiMethodData* mth_rep = md->im->GetMenuMethodRep(data(), NULL, NULL, NULL);
    if (mth_rep == NULL)  continue;
    if (cnt == 0) menu->AddSep();

    //note: both are allowed, but we give priority to BUTTON
    if (md->HasOption("BUTTON")) {
      mth_rep->AddToMenu(menu); 
    } else { // has to be "MENU_BUTTON"
      // create the submenus when needed, and locate -- default is last created one
      if (!(cur_menu = ta_menus.FindName(men_nm))) {
        cur_menu = menu->AddSubMenu(men_nm);
        ta_menus.Add(cur_menu);
      }
      mth_rep->AddToMenu(cur_menu);
    }
    ++cnt;
  } // end BUTTON and MENU_BUTTON loop

  // now, finally, add the normal submenu items, ex. Object, Edit, etc.
  ta_menus.Reset();
  cur_menu = NULL;
  cnt = 0;
  men_nm = "Actions"; // default until/unless explicit
  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    // we always have to extract the MENU_ON because it has side-effects
    on_nm = md->OptionAfter("MENU_ON_");
    if (on_nm.nonempty()) men_nm = on_nm;
    // skip the ones we already put up
    if ((!md->HasOption("MENU")) || (md->HasOption("NO_MENU_CONTEXT") ||
      md->HasOption("MENU_CONTEXT") ||
      md->HasOption("BUTTON") || md->HasOption("MENU_BUTTON")) )
        continue;
    // standard test
    if (!md->ShowMethod()) continue;
    if (md->im == NULL)  continue;
    taiMethodData* mth_rep = md->im->GetMenuMethodRep(data(), NULL, NULL, NULL);
    if (mth_rep == NULL)  continue;
    if (cnt == 0) menu->AddSep();
    // create the submenus when needed, and locate -- default is last created one
    if (!(cur_menu = ta_menus.FindName(men_nm))) {
      cur_menu = menu->AddSubMenu(men_nm);
      ta_menus.Add(cur_menu);
    }
    mth_rep->AddToMenu(cur_menu);
    ++cnt;
  } // end normal submenu items

}

TypeDef* tabDataLink::GetDataTypeDef() const {
  return data()->GetTypeDef();
}

bool tabDataLink::GetIcon(int bmf, int& flags_supported, QIcon& ic) {
  const QPixmap* pm = data()->GetDataNodeBitmap(bmf, flags_supported);
  if (pm) {
    QIcon tmp(*pm);
    ic = tmp;
    return true;
  } else return false;
}

MemberDef* tabODataLink::GetDataMemberDef() const {
  if (!m_data) return NULL;
  taBase* owner = data()->GetOwner();
  if (owner) return owner->FindMember(data());
  else return NULL;
}

String tabDataLink::GetName() const {
  return data()->GetName();
}

String tabDataLink::GetDisplayName() const {
  MemberDef* md = GetDataMemberDef();
  if (md) return md->name;
  else    return data()->GetDisplayName();
}

bool tabDataLink::HasChildItems() {
  // we only search up until we can say yes...
  MemberSpace* ms = &GetDataTypeDef()->members;
  for (int i = 0; i < ms->size; ++ i) {
    MemberDef* md = ms->FastEl(i);
    if (ShowMember(md, TypeItem::SC_TREE)) return true;
  }
  return false;
}

int tabDataLink::checkConfigFlags() const {
  return (data()->baseFlags() & taBase::INVALID_MASK);
}

bool IsHit(const String_PArray& targs, const String_PArray& kicks, String& p) {
  for (int i = 0; i < kicks.size; ++i) {
    if (p.contains_ci(kicks[i])) return false;
  }
  String targ;
  String px = p; 
  // px: highlighted version; we progressively put in placeholders for the
  // font start/end tags, which will split up partial overlapping strings
  // so we don't generate invalid html (some composite searches won't get full
  // highlighting)
  bool rval = false;
  for (int i = 0; i < targs.size; ++i) {
    targ = targs[i];
    int pos = p.index_ci(targ);
    if (pos >= 0) {
      rval = true;
      pos = px.index_ci(targ);
      if (pos >= 0) { // can still highlight in already highlighted version
        px = px.left(pos) + "~~~~" +
          px.at(pos, targ.length()) + "```" +
          px.from(pos + targ.length());
      }
    }
  }
  if (rval) {
    px.gsub("~~~~", "<font color=red>");
    px.gsub("```", "</font>");
    p = px;
  }
  return rval;
}

void AddHit(int item_type, const String& probedx, String& hits) 
{
  if (hits.nonempty()) hits += "<br>";
  switch (item_type) {
  case iSearchDialog::SO_OBJ_NAME: hits += "name:"; break;
  case iSearchDialog::SO_OBJ_TYPE: hits += "type:"; break;
  case iSearchDialog::SO_OBJ_DESC: hits += "desc:"; break;
  case iSearchDialog::SO_MEMB_NAME: hits += "memb name:"; break;
  case iSearchDialog::SO_MEMB_VAL: hits += "memb val:"; break;
  default: break; // huh?
  }
  hits += probedx;
}

void tabDataLink::SearchStat(taBase* tab, iSearchDialog* sd, int level) {
  if (sd->stop()) return; // user hit stop
  const String_PArray& targs = sd->targets();
  const String_PArray& kicks = sd->kickers();
  String probed;
  String hits; // cumulative hits guy
  int n = 0; // hits counter, to know to call Add
  
  // NAME
  int item_type = iSearchDialog::SO_OBJ_NAME;
  if (sd->options() & item_type) {
    probed = tab->GetName();
    if (IsHit(targs, kicks, probed)) 
      {++n; AddHit(item_type, probed, hits);}
  }
  
  // TYPE
  item_type = iSearchDialog::SO_OBJ_TYPE;
  if (sd->options() & item_type) {
    probed = tab->GetTypeDef()->name;
    if (IsHit(targs, kicks, probed)) {
      ++n; AddHit(item_type, probed, hits);
    }
    else {
      if(sd->options() & iSearchDialog::SO_TYPE_DESC) {
	probed = tab->GetTypeDef()->desc;
	if (IsHit(targs, kicks, probed)) {
	  ++n; AddHit(item_type, probed, hits);
	}
      }
    }
  }
  
  // DESC
  item_type = iSearchDialog::SO_OBJ_DESC;
  if (sd->options() & item_type) {
    probed = tab->GetColText(taBase::key_desc);
    if (IsHit(targs, kicks, probed)) {
      ++n; AddHit(item_type, probed, hits);
    }
    else {
      probed = tab->GetDisplayName(); // include display name
      if (IsHit(targs, kicks, probed)) {
	++n; AddHit(item_type, probed, hits);
      }
    }
  }
  
  TypeDef* td = tab->GetTypeDef();
  // MEMB NAME (note: NO_SEARCH not applicable to name search)
  item_type = iSearchDialog::SO_MEMB_NAME;
  if (sd->options() & item_type) {
    for(int m=0;m<td->members.size;m++) {
      MemberDef* md = td->members[m];
      if (!(sd->options() & iSearchDialog::SO_ALL_MEMBS) && !md->ShowMember()) continue;
      probed = md->name;
      if (IsHit(targs, kicks, probed)) {
	++n; AddHit(item_type, probed, hits);
      }
      else {
	if(sd->options() & iSearchDialog::SO_TYPE_DESC) {
	  probed = md->desc;
	  if (IsHit(targs, kicks, probed)) {
	    ++n; AddHit(item_type, probed, hits);
	  }
	}
      }
    }
  }
  
  
  // MEMB VALUE
  
  // first pass: "value" members: 
  // non-ptrs, non-owned taBase values, owned no-browse taBase values
  item_type = iSearchDialog::SO_MEMB_VAL;
  if (sd->options() & item_type) {
    for(int m=0;m<td->members.size;m++) {
      MemberDef* md = td->members[m];
      if (!(sd->options() & iSearchDialog::SO_ALL_MEMBS) && !md->ShowMember()) continue;
      if (md->is_static) continue;
      if (md->HasOption("NO_SEARCH")) continue;
      if (md->type->ptr == 0) {
        // a list or greater is never a "value"
        if (md->type->InheritsFrom(TA_taList_impl)) continue;
        if (md->type->InheritsFrom(TA_taBase)) {
          taBase* obj = (taBase*)md->GetOff(tab);
          taBase* own = obj->GetOwner(); //note: embedded obj must be owned by par
          // non-owned values can't be browsed, and must be handled inline, below
          if (own) {
            // if owned, could browsable child -- we do that as recursive
            if (md->ShowMember(taMisc::USE_SHOW_GUI_DEF,
              TypeItem::SC_TREE)) continue;
          }
        } 
        // have to force getting an inline value, since default is often the path
        probed = md->type->GetValStr(md->GetOff(tab), tab, md,
				     TypeDef::SC_DEFAULT, true); // force_inline
        if (IsHit(targs, kicks, probed)) 
          {++n; AddHit(item_type, probed, hits);}
      }
      else if(md->type->ptr == 1) {
	// if a pointer, treat it as a value and go for it!
        probed = md->type->GetValStr(md->GetOff(tab), tab, md,
				     TypeDef::SC_DEFAULT, true); // force_inline
        if (IsHit(targs, kicks, probed)) 
          {++n; AddHit(item_type, probed, hits);}
      }
    }
  }
  // end of this guy, so if he hit, then output
  if (n > 0) {
    String headline = tab->GetColText(taBase::key_disp_name) +
        " (" + tab->GetTypeDef()->name + ")";
    String href = "ta:" + tab->GetPath();
    String desc = tab->GetColText(taBase::key_desc);
    sd->AddItem(headline, href, desc, hits, level);
  }
   
  
  String def_child = td->OptionAfter("DEF_CHILD_");
  // browsable taBase members
  // second pass: recurse
  for(int m=0; m<td->members.size;m++) {
    if (sd->stop()) return; // user hit stop
    MemberDef* md = td->members[m];
    if (!(sd->options() & iSearchDialog::SO_ALL_MEMBS) && !md->ShowMember()) {
      // def children are excluded from show, but should not be from search!!
      if (md->name != def_child)
	continue;
    }
    if (md->is_static) continue;
    if (md->HasOption("NO_SEARCH")) continue;
    
    taBase* chld = NULL;
    // we are only handling owned browsable taBase guys here
    if ((md->type->ptr > 1) || !md->type->InheritsFrom(TA_taBase) )
      continue;
    // if guy is not a list or greater, must be browsable taBase
    if (!md->type->InheritsFrom(TA_taList_impl)) {
      if (!(sd->options() & iSearchDialog::SO_ALL_MEMBS) && !md->ShowMember(taMisc::USE_SHOW_GUI_DEF,
          TypeItem::SC_TREE)) continue;
    }      
    
    if (md->type->ptr == 0) {
	chld = static_cast<taBase*>(md->GetOff(tab));
    } else { // must be == 1
      taBase** pchld = static_cast<taBase**>(md->GetOff(tab));
      if (!pchld || !(chld = *pchld)) continue;
      if (chld->GetOwner() != tab) continue;
    }
    // note: chld will have a value by here
    tabDataLink::SearchStat(chld, sd, level+1);
  }
  
  // only for Lists:
  if (td->InheritsFrom(&TA_taList_impl)) {
    taList_impl* tal = static_cast<taList_impl*>(tab);
    for(int i=0; i<tal->size; i++) {
      if (sd->stop()) return; // user hit stop
      taBase* itm = (taBase*)tal->FastEl_(i);
      if(!itm) continue;
       // for guys we own (not links; prevents loops)
      if (itm->GetOwner() != tab) continue;
      tabDataLink::SearchStat(itm, sd, level+1);
    }
    
    // only for Groups:
    if (td->InheritsFrom(&TA_taGroup_impl)) {
      taGroup_impl* tag = static_cast<taGroup_impl*>(tab);
      for(int i=0; i < tag->gp.size; i++) {
        if (sd->stop()) return; // user hit stop
        taGroup_impl* gp = tag->gp.FastEl(i);
        if(!gp) continue;
        tabDataLink::SearchStat(gp, sd, level+1);
      }
    }
  }  
}


void tabDataLink::Search(iSearchDialog* dlg) {
  tabDataLink::SearchStat(data(), dlg);
}

bool tabDataLink::ShowMember(MemberDef* md, TypeItem::ShowContext show_context) const {
  TypeDef* td = md->type;
  if (td == NULL) return false; // shouldn't happen...
  // should just be able to completely delegate to the memberdef...
  
  return md->ShowMember(taMisc::USE_SHOW_GUI_DEF, show_context);
/*obs
  // show: normally visible lists; items marked BROWSE
  if (md->HasOption("NO_BROWSE")) return false;
  return ((td->InheritsFrom(&TA_taList_impl) && md->ShowMember(taMisc::NORM_MEMBS))
    || md->HasOption("BROWSE")); */
}


//////////////////////////
//   tabODataLink	//
//////////////////////////

tabODataLink::tabODataLink(taOBase* data_)
:inherited((taBase*)data_, *(data_->addr_data_link()))
{
}

String tabODataLink::ChildGetColText(taDataLink* child, const KeyString& key, int itm_idx) const
{
  if (!list()) return inherited::ChildGetColText(child, key, itm_idx);
  return list()->ChildGetColText(child->data(), child->GetDataTypeDef(), key, itm_idx);
}

taiTreeDataNode* tabODataLink::CreateTreeDataNode_impl(MemberDef* md, taiTreeDataNode* nodePar,
  iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags)
{
  if (!list()) return inherited::CreateTreeDataNode_impl(md, nodePar,
    tvPar, after, node_name, dn_flags);
  taiTreeDataNode* rval = NULL;
  if (nodePar)
    rval = new tabDefChildTreeDataNode(this, md, nodePar, after, node_name, dn_flags);
  else
    rval = new tabDefChildTreeDataNode(this, md, tvPar, after, node_name, dn_flags);
  return rval;
}


String tabODataLink::GetColHeading(const KeyString& key) const {
  if (!list()) return inherited::GetColHeading(key);
  return list()->GetColHeading(key);
}

const KeyString tabODataLink::GetListColKey(int col) const {
  if (!list()) return inherited::GetListColKey(col);
  return list()->GetListColKey(col);
}

taiDataLink* tabODataLink::GetListChild(int itm_idx) {
  if (!list()) return inherited::GetListChild(itm_idx);
  if ((itm_idx < 0) || (itm_idx >= list()->size))
    return NULL;
  TypeDef* typ;
  void* el = list()->GetTA_Element(itm_idx, typ); // gets the item, and its TypeDef
  if (typ == NULL) return NULL; 
  // if we get a taBase item, the type might only be the base type, not the derived type of the item
  // so we cast the item, and then grab the exact type right from the item
  if (typ->InheritsFrom(&TA_taBase)) {
      typ = ((taBase*)el)->GetTypeDef();
  }
  // get the link
  taiDataLink* dl = taiViewType::StatGetDataLink(el, typ);
  return dl;
}

taiDataLink* tabODataLink::GetListChild(void* el) {
  if (!list()) return inherited::GetListChild(el);
  if (!el) return NULL;
  TypeDef* typ = list()->El_GetType_(el);
  if (typ == NULL) return NULL;
  // if we get a taBase item, the type might only be the base type, not the derived type of the item
  // so we cast the item, and then grab the exact type right from the item
  if (typ->InheritsFrom(&TA_taBase)) {
      typ = ((taBase*)el)->GetTypeDef();
  }
  // get the link
  taiDataLink* dl = taiViewType::StatGetDataLink(el, typ);
  return dl;
}

int tabODataLink::NumListCols() const {
  if (!list()) return inherited::NumListCols();
  return list()->NumListCols();
}


//////////////////////////////////
//   tabListDataLink		//
//////////////////////////////////

tabListDataLink::tabListDataLink(taList_impl* data_)
:inherited((taOBase*)data_)
{
}

taiTreeDataNode* tabListDataLink::CreateTreeDataNode_impl(MemberDef* md, taiTreeDataNode* nodePar,
  iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags)
{
  taiTreeDataNode* rval = NULL;
  if (nodePar)
    rval = new tabListTreeDataNode(this, md, nodePar, after, node_name, dn_flags);
  else
    rval = new tabListTreeDataNode(this, md, tvPar, after, node_name, dn_flags);
  return rval;
}


//////////////////////////////////
// 	tabGroupDataLink	//
//////////////////////////////////


tabGroupDataLink::tabGroupDataLink(taGroup_impl* data_)
:inherited((taList_impl*)data_)
{
}

taiTreeDataNode* tabGroupDataLink::CreateTreeDataNode_impl(MemberDef* md, taiTreeDataNode* nodePar,
  iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags)
{
  taiTreeDataNode* rval = NULL;
  if (nodePar)
    rval = new tabGroupTreeDataNode(this, md, nodePar, after, node_name, dn_flags);
  else
    rval = new tabGroupTreeDataNode(this, md, tvPar, after, node_name, dn_flags);
  return rval;
}


//////////////////////////////////
//  IDataViewWidget		//
//////////////////////////////////

IDataViewWidget::IDataViewWidget(DataViewer* viewer_)
{
  m_viewer = viewer_;
  m_window = NULL;
  // note: caller will still do a virtual Constr() on us after new
}

IDataViewWidget::~IDataViewWidget() {
  //note: the viewer may already have deleted, so it will have nulled its ref here
  if (m_viewer) {
    m_viewer->WidgetDeleting();
    m_viewer = NULL;
  }
}

void IDataViewWidget::Close() {
  if (m_viewer) {
    CancelOp cancel_op = CO_NOT_CANCELLABLE; // the Close call is unconditional
    OnClosing_impl(cancel_op);
  }
  if (m_viewer) {
    m_viewer->WidgetDeleting();
    m_viewer = NULL;
  }
//TEMP  delete this;
widget()->deleteLater();
  //NO CODE AFTER THIS POINT -- WE ARE DELETED
}

void IDataViewWidget::closeEvent_Handler(QCloseEvent* e,
    CancelOp def_cancel_op)
{
  OnClosing_impl(def_cancel_op); // param is op&
  if (def_cancel_op == CO_CANCEL)
    e->ignore();
  else // proceed or not cancellable
    e->accept();
}
 
void IDataViewWidget::OnClosing_impl(CancelOp& cancel_op) {
  if (m_viewer) {
    m_viewer->WindowClosing(cancel_op);
  }
}

void IDataViewWidget::Refresh() {
  taMisc::Busy(true);
  Refresh_impl();
  taMisc::Busy(false);
}

iMainWindowViewer* IDataViewWidget::viewerWindow() const {
  //note: an owner might have simply set this on creation
  if (!m_window) {
    //note: ok to cast away constness
    QWidget* par = const_cast<IDataViewWidget*>(this)->widget(); // we start here, so MainViewer itself returns itself
    do {
      m_window = qobject_cast<iMainWindowViewer*>(par);
      if (m_window) break;
    } while ((par = par->parentWidget()));
  }
  return m_window; // could still be null if not found
}

//////////////////////////
//   ISelectable	//
//////////////////////////

ISelectable::~ISelectable() {
  ISelectableHost::ItemDeleting(this);
}

QObject* ISelectable::clipHandlerObj() const {
  ISelectableHost* host_ = host();
  return (host_) ? host_->clipHandlerObj() : NULL;
}

taiDataLink* ISelectable::clipParLink(GuiContext sh_typ) const {
  return par_link();
}

// called from ui to handle drops
void ISelectable::DropHandler(const QMimeData* mime, const QPoint& pos, 
    int mods, int where) 
{
//Note: on Mac, "Ctrl" and test bits always refer to Command key (not Ctrl key)
  taiMimeSource* ms = taiMimeSource::New(mime);
  ISelectableHost* host_ = host(); //cache
  
  // set for the menu callbacks
  host_->ctxt_ms = ms;
  host_->ctxt_item = this;
  
  int ea = QueryEditActions_(ms, GC_DEFAULT);
  int key_mods = mods & (Qt::ShiftModifier | Qt::ControlModifier |
    Qt::AltModifier);
  // only honor if user has chosen 1 and only 1 mod
  // and its shortcut ops is ambiguous and available
  if (key_mods == Qt::ShiftModifier) { // Move
    if ((ea & taiClipData::EA_DROP_MOVE2) == taiClipData::EA_DROP_MOVE2) 
      goto show_menu;
    host_->helperObj()->DropEditAction(ea & taiClipData::EA_DROP_MOVE2); // is only one or the other
    goto exit;
  } else if (key_mods == Qt::ControlModifier) { // Copy
    if ((ea & taiClipData::EA_DROP_COPY2) == taiClipData::EA_DROP_COPY2) 
      goto show_menu;
    host_->helperObj()->DropEditAction(ea & taiClipData::EA_DROP_COPY2); // is only one or the other
    goto exit;
  } else if (key_mods == Qt::AltModifier) { // Link
    if ((ea & taiClipData::EA_DROP_LINK2) == taiClipData::EA_DROP_LINK2) 
      goto show_menu;
    host_->helperObj()->DropEditAction(ea & taiClipData::EA_DROP_LINK2); // is only one or the other
    goto exit;
  }
  
     
  // always show menu, for consistency
  // all following implicitly use the GC_DEFAULT code
show_menu: 
  { // block for jump
  taiMenu* menu = new taiMenu(widget(), taiMenu::normal, 0);
  QAction* act = NULL;
  // when dropping "at end", we force the term "here" instead of "into"
  // but we still use the INTO semantics
  bool force_here = (where == iTreeWidgetItem::WI_AT_END);
  String IntoOrHere = force_here ? "Here" : "Into";
// NOTE: "Xxx+" shortcuts are dummies, to hint the shortcircuit drop key
// if has both variants of an op, no shortcuts and both, else yes
// NOTE: for the "between/end" cases, we can't allow the "Into" (ie,
//  typically for a group) because the situation is ambiguous, and
// the result is not what might be expected, since it gets put into
// the next item, not as a child of the previous item
  if ((ea & taiClipData::EA_DROP_MOVE2) == taiClipData::EA_DROP_MOVE2) {
    act = menu->AddItem("Move Here", taiAction::int_act,
      host_->helperObj(),  SLOT(DropEditAction(int)), 
      taiClipData::EA_DROP_MOVE, QKeySequence());
    if (where == iTreeWidgetItem::WI_ON) {
      act = menu->AddItem("Move Into", taiAction::int_act,
        host_->helperObj(),  SLOT(DropEditAction(int)), 
        taiClipData::EA_DROP_MOVE_INTO, QKeySequence());
    }
  } else {
    if (ea & taiClipData::EA_DROP_MOVE)
      act = menu->AddItem("&Move Here", taiAction::int_act,
        host_->helperObj(),  SLOT(DropEditAction(int)), 
        taiClipData::EA_DROP_MOVE, QKeySequence("Shift+"));
    else if (ea & taiClipData::EA_DROP_MOVE_INTO)
      act = menu->AddItem("&Move "+IntoOrHere, taiAction::int_act,
        host_->helperObj(),  SLOT(DropEditAction(int)), 
        taiClipData::EA_DROP_MOVE_INTO, QKeySequence("Shift+"));
  }
  
  act = NULL;
  if ((ea & taiClipData::EA_DROP_COPY2) == taiClipData::EA_DROP_COPY2) {
    act = menu->AddItem("Copy Here", taiAction::int_act,
      host_->helperObj(),  SLOT(DropEditAction(int)),
      taiClipData::EA_DROP_COPY, QKeySequence());
    if (where == iTreeWidgetItem::WI_ON) {
      act = menu->AddItem("Copy Into", taiAction::int_act,
        host_->helperObj(),  SLOT(DropEditAction(int)),
        taiClipData::EA_DROP_COPY_INTO, QKeySequence());
    }
  } else {
    if (ea & taiClipData::EA_DROP_COPY)
      act = menu->AddItem("&Copy Here", taiAction::int_act,
        host_->helperObj(),  SLOT(DropEditAction(int)),
        taiClipData::EA_DROP_COPY, QKeySequence("Ctrl+"));
    else if (ea & taiClipData::EA_DROP_COPY_INTO)
      act = menu->AddItem("&Copy "+IntoOrHere, taiAction::int_act,
        host_->helperObj(),  SLOT(DropEditAction(int)),
        taiClipData::EA_DROP_COPY_INTO, QKeySequence("Ctrl+"));
  }
  
  act = NULL;
  // Assign only applicable for "On" drops
  if ((where == iTreeWidgetItem::WI_ON) && 
     (ea & taiClipData::EA_DROP_ASSIGN)) 
  {
    act = menu->AddItem("Assign To", taiAction::int_act,
      host_->helperObj(),  SLOT(DropEditAction(int)),
      taiClipData::EA_DROP_ASSIGN, QKeySequence());
  }   
     
  act = NULL;
  if ((ea & taiClipData::EA_DROP_LINK2) == taiClipData::EA_DROP_LINK2) {
    act = menu->AddItem("Link Here", taiAction::int_act,
      host_->helperObj(),  SLOT(DropEditAction(int)),
      taiClipData::EA_DROP_LINK, QKeySequence());
    if (where == iTreeWidgetItem::WI_ON) {
      act = menu->AddItem("Link Into", taiAction::int_act,
        host_->helperObj(),  SLOT(DropEditAction(int)),
        taiClipData::EA_DROP_LINK_INTO, QKeySequence());
    }
  } else {
    if (ea & taiClipData::EA_DROP_LINK)
      act = menu->AddItem("&Link Here", taiAction::int_act,
        host_->helperObj(),  SLOT(DropEditAction(int)),
        taiClipData::EA_DROP_LINK, QKeySequence("Alt+"));
    else if (ea & taiClipData::EA_DROP_LINK_INTO)
      act = menu->AddItem("&Link "+IntoOrHere, taiAction::int_act,
        host_->helperObj(),  SLOT(DropEditAction(int)),
        taiClipData::EA_DROP_LINK_INTO, QKeySequence("Alt+"));
  }

  // if any appropriate drop actions, then add them!
  menu->AddSep();
  host_->UpdateMethodsActionsForDrop();
  host_->AddDynActions(menu, 0);
  
  menu->AddSep();
  act = menu->AddItem("C&ancel", -1);
  act->setShortcut(QKeySequence("Esc"));

  // get current mouse position
  QPoint men_pos = widget()->mapToGlobal(pos);
  
  act = menu->menu()->exec(men_pos);
  //menu->deleteLater();
  delete menu;
  } // block for jump
exit:
  host_->ctxt_ms = NULL;
  host_->ctxt_item = NULL;
  delete ms;
}

taiDataLink* ISelectable::effLink(GuiContext sh_typ) const {
  if (sh_typ == GC_DEFAULT)
    sh_typ = shType();
  if (sh_typ <= GC_DUAL_DEF_DATA) return link();
  return viewLink();
}
      
void ISelectable::FillContextMenu(ISelectable_PtrList& sel_items,
  taiActions* menu, GuiContext sh_typ) 
{
  FillContextMenu_impl(menu, sh_typ);
  int allowed = QueryEditActions_(sel_items, sh_typ);
  FillContextMenu_EditItems_impl(menu, allowed, sh_typ);
  if (sel_items.size == 1) {
    taiDataLink* link = this->effLink(sh_typ);
    if (link) link->FillContextMenu(menu);
  }
}

void ISelectable::FillContextMenu_EditItems_impl(taiActions* menu,
  int ea, GuiContext sh_typ)
{
  if (ea == 0) return;
  if (menu->count() > 0)
    menu->AddSep();
//  cut copy paste link delete
  taiAction* mel;
  if (ea & taiClipData::EA_CUT) {
    mel = menu->AddItem("Cu&t", taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_CUT;
    mel->setData(sh_typ);
  }
  if (ea & taiClipData::EA_COPY) {
    mel = menu->AddItem("&Copy", taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_COPY;
    mel->setData(sh_typ);
  }
  if (ea & taiClipData::EA_DUPE) {
    mel = menu->AddItem("Duplicate  (Ctrl+M)", taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_DUPE;
    mel->setData(sh_typ);
  }
  
  // Paste and Link guys are slightly complicated, because we can have 
  // OP/OP_INTO variants, so we can't have shortcuts with both
  int paste_cnt = 0;
  if (ea & taiClipData::EA_PASTE) ++paste_cnt;
  if (ea & taiClipData::EA_PASTE_INTO) ++paste_cnt;
  if (ea & taiClipData::EA_PASTE_ASSIGN) ++paste_cnt;
  if (ea & taiClipData::EA_PASTE_APPEND) ++paste_cnt;
  String txt;
  if (ea & taiClipData::EA_PASTE) {
    if (paste_cnt > 1) txt = "Paste"; else txt = "&Paste";
    mel = menu->AddItem(txt, taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_PASTE;
    mel->setData(sh_typ);
  } 
  if (ea & taiClipData::EA_PASTE_INTO) {
    if (paste_cnt > 1) txt = "Paste Into"; else txt = "&Paste Into";
    mel = menu->AddItem(txt, taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_PASTE_INTO;
    mel->setData(sh_typ);
  } 
  if (ea & taiClipData::EA_PASTE_ASSIGN) {
    if (paste_cnt > 1) txt = "Paste Assign"; else txt = "&Paste Assign";
    mel = menu->AddItem(txt, taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_PASTE_ASSIGN;
    mel->setData(sh_typ);
  } 
  if (ea & taiClipData::EA_PASTE_APPEND) {
    if (paste_cnt > 1) txt = "Paste Append"; else txt = "&Paste Append";
    mel = menu->AddItem(txt, taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_PASTE_APPEND;
    mel->setData(sh_typ);
  }
  
  if (ea & taiClipData::EA_DELETE) {
    mel = menu->AddItem("&Delete  (Ctrl+D)", taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_DELETE;
    mel->setData(sh_typ);
  }
  if ((ea & taiClipData::EA_LINK) == taiClipData::EA_LINK) {
    mel = menu->AddItem("Link", taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_LINK;
    mel->setData(sh_typ);
    mel = menu->AddItem("Link Into", taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_LINK_INTO;
    mel->setData(sh_typ);
  } 
  else if (ea & taiClipData::EA_LINK) {
    mel = menu->AddItem("&Link", taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_LINK;
    mel->setData(sh_typ);
  } 
  else if (ea & taiClipData::EA_LINK_INTO) {
    mel = menu->AddItem("&Link Into", taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_LINK_INTO;
    mel->setData(sh_typ);
  } 
  
  if (ea & taiClipData::EA_UNLINK) {
    mel = menu->AddItem("&Unlink", taiMenu::use_default,
        taiAction::men_act, clipHandlerObj(), ISelectableHost::edit_menu_action_slot, this);
    mel->usr_data = taiClipData::EA_UNLINK;
    mel->setData(sh_typ);
  }
  taiDataLink* link = this->effLink(sh_typ);
  if (link) link->FillContextMenu_EditItems(menu, ea);
}

taiClipData* ISelectable::GetClipData(const ISelectable_PtrList& sel_items,
  int src_edit_action, bool for_drag, GuiContext sh_typ) const
{
  if (sel_items.size <= 1) { 
    return GetClipDataSingle(src_edit_action, for_drag, sh_typ);
  } else { 
    return GetClipDataMulti(sel_items,src_edit_action, for_drag, sh_typ);
  }
}

void ISelectable::GetContextCaptions(String& view_cap, String& obj_cap) {
  taBase* tab = taData(GC_DUAL_DEF_VIEW);
  if (tab) view_cap = tab->GetTypeName();
  tab = taData(GC_DUAL_DEF_DATA);
  if (tab) obj_cap = tab->GetTypeName();
}

TypeDef* ISelectable::GetEffDataTypeDef(GuiContext sh_typ) const {
  taDataLink* link_ = (taDataLink*)effLink(sh_typ);
  return (link_) ? link_->GetDataTypeDef() : NULL;
}

taiDataLink* ISelectable::own_link(GuiContext sh_typ) const {
  taiDataLink* link = this->effLink(sh_typ);
  return (link) ? link->ownLink() : NULL;
}

MemberDef* ISelectable::par_md() const {
  // to get the par_md, we have to go up the grandparent, and ask for member addr
  MemberDef* rval = NULL;
  taBase* par_tab = NULL; //note: still only got the guy, not par
  taBase* gpar_tab = NULL;
  taiDataLink* link = this->effLink();
  if (!link) goto exit;
  par_tab = link->taData(); //note: still only got the guy, not par
  if (!par_tab) goto exit;
  par_tab = par_tab->GetOwner(); // now we have par
  if (!par_tab) goto exit;
  gpar_tab = par_tab->GetOwner();
  if (!gpar_tab) goto exit;
  rval = gpar_tab->FindMemberPtr(par_tab);
exit:
   return rval;
}

taiDataLink* ISelectable::par_link() const {
  ISelectable* par = this->par();
  if (par) return par->link();
  else     return NULL;
}

// called from Ui for cut/paste etc. -- not called for drag/drop ops
int ISelectable::EditAction_(ISelectable_PtrList& sel_items, int ea,
  GuiContext sh_typ) 
{
  taiMimeSource* ms = NULL;
  taiClipData* cd = NULL;
  int rval = taiClipData::ER_IGNORED; //not really used, but 0 is ignored, 1 is done, -1 is forbidden, -2 is error
  
  // get the appropriate data, either clipboard data, or item data, depending on op
  if  (ea & (taiClipData::EA_SRC_OPS)) { // no clipboard data
    // we handle cut and copy
    if ((ea & (taiClipData::EA_CUT | taiClipData::EA_COPY))) { // copy-like op, get item data
      cd = GetClipData(sel_items, taiClipData::ClipOpToSrcCode(ea), false, sh_typ);
      // note that a Cut is a Copy, possibly followed later by a xxx_data_taken command, if client pastes it
      QApplication::clipboard()->setMimeData(cd, QClipboard::Clipboard);
      cd = NULL; // clipboard now owns it
      rval = taiClipData::ER_OK;
    } else { // other ops, like Duplicate, Clear or Unlink
      for (int i = 0; i < sel_items.size; ++i) {
        ISelectable* is = sel_items.SafeEl(i);
        if (!is) continue;
        int trval = is->EditActionS_impl_(ea, GC_DEFAULT);
        if (trval == 0) continue;
        rval = trval;
        if (rval < 0) break; // forbidden or error
      }
    }
  } else { // paste-like op, get item data
    // confirm only 1 item selected for dst op -- Error is diagnostic, not operational
    if (sel_items.size > 1) {
      taMisc::Error("Paste-like clip operations only allowed for a single dest item");
    } else {
      ms = taiMimeSource::New(QApplication::clipboard()->mimeData(QClipboard::Clipboard));
      rval = EditActionD_impl_(ms, ea, sh_typ);
    }
  }
  if (ms) delete ms;
  if (cd) delete cd;
  return rval;
}

int ISelectable::QueryEditActions_(taiMimeSource* ms, GuiContext sh_typ) const {
  int allowed = 0;
  int forbidden = 0;
  // if src is readonly, then forbid certain dst ops
  if (ms->srcAction() & taiClipData::EA_SRC_READONLY)
    forbidden |= taiClipData::EA_FORB_ON_SRC_READONLY;
  QueryEditActionsD_impl_(ms, allowed, forbidden, sh_typ);
  return (allowed & (~forbidden));
}

int ISelectable::QueryEditActions_(const ISelectable_PtrList& sel_items,
  GuiContext sh_typ) const
{
  int allowed = 0;
  int forbidden = 0;
  if (sel_items.size <= 1) { // single select
    QueryEditActionsS_impl_(allowed, forbidden, sh_typ);
    taiMimeSource* ms = taiMimeSource::New(
      QApplication::clipboard()->mimeData(QClipboard::Clipboard));
    QueryEditActionsD_impl_(ms, allowed, forbidden, sh_typ);
    delete ms;
  } else { // multi select -- no dst ops allowed
    int allowed_accum = 0; // add allowed to this guy
    int allowed_knockout = -1; // subtract allowed from this guy
    for (int i = 0; i < sel_items.size; ++i) {
      ISelectable* is = sel_items.SafeEl(i);
      if (is) {
        int item_allowed = 0;
        is->QueryEditActionsS_impl_(item_allowed, forbidden, GC_DEFAULT);
        allowed_accum |= item_allowed;
        allowed_knockout &= item_allowed;
      }
    }
    allowed |= (allowed_accum & allowed_knockout);
  }
  return (allowed & (~forbidden));
}

taBase* ISelectable::taData(GuiContext sh_typ) const {
  taiDataLink* link = this->effLink(sh_typ);
  if (link)
    return link->taData();
  else return NULL;
}

QWidget* ISelectable::widget() const {
  ISelectableHost* host_ = host();
  return (host_) ? host_->widget() : NULL;
}


//////////////////////////////////
//   IObjectSelectable		//
//////////////////////////////////

taiClipData* IObjectSelectable::GetClipDataSingle(int src_edit_action,
  bool for_drag, GuiContext sh_typ) const
{
  // if it is taBase, we can make an object
  taBase* obj = this->taData(sh_typ);
  if (!obj) return NULL;
  
  taiObjectMimeFactory* mf = taiObjectMimeFactory::instance();
  taiClipData* rval = new taiClipData(src_edit_action);
  mf->AddSingleObject(rval, obj);
  return rval;
}

taiClipData* IObjectSelectable::GetClipDataMulti(const ISelectable_PtrList& sel_items, 
    int src_edit_action, bool for_drag, GuiContext sh_typ) const
{
  taiClipData* rval = NULL;
  //note: although a bit sleazy, we just do this by optimistically
  // assuming all are taBase (which realistically, they are) 
  taBase_PtrList* bl = new taBase_PtrList;
  bool do_it = true;
  for (int i = 0; i < sel_items.size; ++i) {
    ISelectable* dn = sel_items.FastEl(i);
    taBase* obj = dn->taData();
    if (!obj) {
      do_it = false;
      break;
    }
    bl->Add(obj);
  }
  if (do_it) {
    taiObjectMimeFactory* mf = taiObjectMimeFactory::instance();
    rval = new taiClipData(src_edit_action);
    mf->AddMultiObjects(rval, bl);
  }
  delete bl;
  return rval;
}

int IObjectSelectable::EditActionD_impl_(taiMimeSource* ms,
  int ea, GuiContext sh_typ) 
{//note: follows same logic as the Query
  taiDataLink* pdl = clipParLink(sh_typ);
  //note: called routines must requery for allowed
  taiDataLink* link = this->effLink(sh_typ);
  int rval = taiClipData::ER_IGNORED;
  // we have to individually disambiguate the allowed, because we have
  // to make sure the right list or group guy handles things like PasteInto, etc.
  int allowed = 0;
  int forbidden = 0;
  int eax;
  if (pdl) {
//no    pdl->ChildQueryEditActions_impl(par_md(), link, ms, allowed, forbidden);
    pdl->ChildQueryEditActions_impl(NULL, link, ms, allowed, forbidden);
    eax = ea & (allowed & (~forbidden));
    if (eax)
//no      rval = pdl->ChildEditAction_impl(par_md(), link, ms, eax);
      rval = pdl->ChildEditAction_impl(NULL, link, ms, eax);
  }
  if (link && (rval == taiClipData::ER_IGNORED)) {
    allowed = forbidden = 0;
    link->ChildQueryEditActions_impl(this->md(), NULL, ms, allowed, forbidden);
    eax = ea & (allowed & (~forbidden));
    if (eax)
      rval = link->ChildEditAction_impl(this->md(), NULL, ms, eax);
    
    if (rval == taiClipData::ER_IGNORED) {
      allowed = forbidden = 0;
      link->QueryEditActions_impl(ms, allowed, forbidden); // ex. COPY
      eax = ea & (allowed & (~forbidden));
      if (eax)
        rval = link->EditAction_impl(ms, eax);
    }
  }
  return rval;
}

int IObjectSelectable::EditActionS_impl_(int ea, GuiContext sh_typ) {
//note: follows same logic as the Query
  taiDataLink* pdl = clipParLink(sh_typ);
  //note: called routines must requery for allowed
  taiDataLink* link = this->effLink(sh_typ);
  int rval = taiClipData::ER_IGNORED;
  if (pdl) {
//no    rval = pdl->ChildEditAction_impl(par_md(), link, NULL, ea);
    rval = pdl->ChildEditAction_impl(NULL, link, NULL, ea);
  }
  if (link) {
    //note: item-as-parent does not apply to src context, so we omit it
    if (rval == taiClipData::ER_IGNORED)
      rval = link->EditAction_impl(NULL, ea);
  }
  return rval;
}

void IObjectSelectable::QueryEditActionsD_impl_(taiMimeSource* ms,
  int& allowed, int& forbidden, GuiContext sh_typ) const 
{
  // parent object will generally manage CUT, and DELETE
  // parent object normally passes on to child object
  taiDataLink* pdl = clipParLink(sh_typ);
  taiDataLink* link = this->effLink(sh_typ);
  if (pdl) 
    pdl->ChildQueryEditActions_impl(NULL, link, ms, allowed, forbidden); // ex. DROP of child on another child, to reorder
  if (link) {
    link->ChildQueryEditActions_impl(this->md(), NULL, ms, allowed, forbidden); // ex. DROP of child on parent, to insert as first item
    link->QueryEditActions_impl(ms, allowed, forbidden); // ex. COPY
  }
}

void IObjectSelectable::QueryEditActionsS_impl_(int& allowed, int& forbidden,
  GuiContext sh_typ) const 
{
  // parent object will generally manage CUT, and DELETE
  // parent object normally passes on to child object
  taiDataLink* pdl = clipParLink(sh_typ);
  taiDataLink* link = this->effLink(sh_typ);
  if (pdl) 
    pdl->ChildQueryEditActions_impl(NULL, link, NULL, allowed, forbidden); // ex. CUT of child
  if (link) {
    // note: item-as-parent doesn't apply to src actions, so we omit that
    link->QueryEditActions_impl(NULL, allowed, forbidden); // ex. COPY
  }
}


//////////////////////////////////
//   ISelectable_PtrList	//
//////////////////////////////////

taPtrList_impl* ISelectable_PtrList::insts;

ISelectable_PtrList::ISelectable_PtrList(const ISelectable_PtrList& cp)
:taPtrList<ISelectable>(cp) 
{
  Initialize();
}

void ISelectable_PtrList::Initialize() {
  // add to managed list
  if (!insts) {
    insts = new taPtrList_impl;
  }
  insts->Add_(this);
}

ISelectable_PtrList::~ISelectable_PtrList() {
  // remove from managed list
  if (insts) {
    insts->RemoveEl_(this);
    if (insts->size == 0) {
      delete insts;
      insts = NULL;
    }
  }
}

TypeDef* ISelectable_PtrList::CommonSubtype1N(ISelectable::GuiContext gc_typ) 
{ // greatest common subtype of items 1-N
  if (size == 0) return NULL;
  taiDataLink* link = FastEl(0)->effLink(gc_typ);
  if (!link) return NULL; // gui-only object, no ref
  TypeDef* rval = link->GetDataTypeDef();
  for (int i = 1; (rval && (i < size)); ++i) {
    link = FastEl(i)->effLink(gc_typ);
    if (!link) return NULL; // gui-only, not commensurable
    rval = TypeDef::GetCommonSubtype(rval, link->GetDataTypeDef());
  }
  return rval;
}

TypeDef* ISelectable_PtrList::CommonSubtype2N(ISelectable::GuiContext gc_typ) 
{ // greatest common subtype of items 2-N
  if (size <= 1) return NULL;
  taiDataLink* link = FastEl(1)->effLink(gc_typ);
  if (!link) return NULL; // gui-only object, no ref
  TypeDef* rval = link->GetDataTypeDef();
  for (int i = 2; (rval && (i < size)); ++i) {
    link = FastEl(i)->effLink(gc_typ);
    if (!link) return NULL; // gui-only, not commensurable
    rval = TypeDef::GetCommonSubtype(rval, link->GetDataTypeDef());
  }
  return rval;
}

TypeDef* ISelectable_PtrList::Type1(ISelectable::GuiContext gc_typ) {
  if (size == 0) return NULL;
  else {
    taiDataLink* link = FastEl(0)->effLink(gc_typ);
    if (link) return link->GetDataTypeDef();
    else      return NULL; // gui-only object, no ref
  }
}

//////////////////////////
//   DynMethod_PtrList	//
//////////////////////////

DynMethod_PtrList::~DynMethod_PtrList() {
  Reset();
}

DynMethodDesc* DynMethod_PtrList::AddNew(int dmd_type, MethodDef* md) {
  DynMethodDesc* rval = new DynMethodDesc();
  rval->dmd_type = dmd_type;
  rval->md = md;
  Add(rval);
  return rval;
}

void DynMethod_PtrList::Fill(ISelectable_PtrList& sel_items,
  ISelectable::GuiContext gc_typ) 
{
  if (sel_items.size == 0) return;

  TypeDef* t1n = sel_items.CommonSubtype1N(gc_typ); // greatest common subtype of items 1-N
  if (t1n == NULL) return; // typically for non-taBase types, ex Class browsing
  MethodDef* md;
  // Type_N methods (common to all)
  for (int i = 0; i < t1n->methods.size; ++i) {
    md = t1n->methods.FastEl(i);
    if (!md->HasOption("DYN1")) continue;
    AddNew(Type_1N, md);
  }

  if (sel_items.size == 1) return;
  taiDataLink* link = sel_items.FastEl(0)->effLink(gc_typ);
  if (!link) return; // gui only obj
  TypeDef* t1 = link->GetDataTypeDef(); // type of 1st item
  TypeDef* t2n = sel_items.CommonSubtype2N(gc_typ); // greatest common subtype of items 2-N
  if (!t2n) return;

  TypeDef* arg1_typ;
  // Type_1_2N
  for (int i = 0; i < t1->methods.size; ++i) {
    md = t1->methods.FastEl(i);
    if (!md->HasOption("DYN12N")) continue;
    if (md->arg_types.size == 0) {
      cerr << "Warning: method " << md->name << "should have had an arg1 in class* form.\n" ;
      continue;
    }
    arg1_typ = md->arg_types.FastEl(0);
    // must be a pointer to a class type
    if (arg1_typ->ptr != 1) {
      cerr << "Warning: method " << md->name << "should have had arg1 in class* form.\n" ;
      continue;
    }
    // now get the non-pointer type
    arg1_typ = arg1_typ->GetNonPtrType();
    if (!t2n->InheritsFrom(arg1_typ)) continue;
    AddNew(Type_1_2N, md);
  }

  // Type_2N_1
  for (int i = 0; i < t2n->methods.size; ++i) {
    md = t2n->methods.FastEl(i);
    if ((md->arg_types.size == 0) || !md->HasOption("DYN2N1")) continue;
    arg1_typ = md->arg_types.FastEl(0);
    // must be a pointer to a class type
    if (arg1_typ->ptr != 1) {
      cerr << "Warning: method " << md->name << "should have had class* form.\n" ;
      continue;
    }
    // now get the non-pointer type
    arg1_typ = arg1_typ->GetNonPtrType();
    if (!t1->InheritsFrom(arg1_typ)) continue;
    AddNew(Type_2N_1, md);
  }

}

void DynMethod_PtrList::FillForDrop(const taiMimeSource& ms, 
    ISelectable* drop_item)
{
  taiObjectsMimeItem* mi = ms.objects();
  if (!mi || (mi->count() == 0)) return;
  TypeDef* tms = mi->CommonSubtype(); // greatest common subtype of source object(s)
  TypeDef* tdi = drop_item->GetEffDataTypeDef(); 
  if (!tdi) return;
  
  for (int i = 0; i < tdi->methods.size; ++i) {
    MethodDef* md = tdi->methods.FastEl(i);
    //look for all DROP methods with compatible arg0 type
    if (md->arg_types.size == 0) continue; 
    TypeDef* arg0_typ = md->arg_types.FastEl(0);
    // must be a pointer to a class type
    if (arg0_typ->ptr != 1) {
      continue;
    }
    // meth must be marked for drop
    if (!(md->HasOption("DROPN") ||
      ((mi->count() == 1) && md->HasOption("DROP1")))) continue;
    
    // now get the non-pointer type
    arg0_typ = arg0_typ->GetNonPtrType();
    if (!tms->InheritsFrom(arg0_typ)) continue;
    AddNew(Type_MimeN_N, md);
  }

}

//////////////////////////////////
//   ISelectableHost		//
//////////////////////////////////

void ISelectableHost::ItemDeleting(ISelectable* item) {
  taPtrList_impl* insts = ISelectable_PtrList::insts; // cache for convenience
  if (!insts) return; // note: prob shouldn't happen, if item exists!
  for (int i = insts->size - 1; i > 0; --i) {
    ISelectable_PtrList* list = (ISelectable_PtrList*)insts->FastEl_(i);
    list->RemoveEl_(item);
  }
}

const char* ISelectableHost::edit_enabled_slot = SLOT(EditActionsEnabled(int&));
const char* ISelectableHost::edit_action_slot = SLOT(EditAction(int)); 
const char* ISelectableHost::edit_menu_action_slot = SLOT(EditAction(taiAction*)); 
const char* ISelectableHost::actions_enabled_slot; // currently NULL
const char* ISelectableHost::update_ui_signal; // currently NULL

ISelectableHost::ISelectableHost() {
  m_sel_chg_cnt = 0;
  helper = new SelectableHostHelper(this);
  dyn_idx = 0;
  ctxt_ms = 0;
  ctxt_ms = NULL;
}

ISelectableHost::~ISelectableHost() {
  Emit_NotifySignal(OP_DESTROYING);
  //note: we delete it right now, to force disconnect of all signals/slots
  delete helper;
  helper = NULL;
}

void ISelectableHost::AddSelectedItem(ISelectable* item,  bool forced) {
  sel_items.AddUnique(item); //note: use raw list, because we are building it
  if (m_sel_chg_cnt == 0) // also ignored if in Update
    SelectionChanged(forced);
}

void ISelectableHost::AddDynActions(taiActions* menu, int dyn_list,
  ISelectable::GuiContext gc_typ) 
{
  if (dyn_actions[dyn_list].count() == 0) return;
//nn,at top  menu->AddSep();
  for (int i = 0; i < (int)dyn_actions[dyn_list].count(); ++i) {
    taiAction* act = dyn_actions[dyn_list].FastEl(i);
    act->AddTo(menu);
  }
}

void ISelectableHost::ClearSelectedItems(bool forced) {
  SelectionChanging(true, forced);
  sel_items.Reset(); //note: use raw list, because we are building it
  SelectionChanging(false, forced);
}

QObject* ISelectableHost::clipHandlerObj() const {
  return helper;
} 

void ISelectableHost::Connect_SelectableHostNotifySignal(QObject* sink_obj,
    const char* sink_slot, bool discnct)
{
  static const char* sig_nm = SIGNAL(NotifySignal(ISelectableHost*, int));
  if (discnct)
    QObject::disconnect(helper, sig_nm, sink_obj, sink_slot);
  else
    QObject::connect(helper, sig_nm, sink_obj, sink_slot);
}

void ISelectableHost::Connect_SelectableHostItemRemovingSlot(QObject* src_obj, 
    const char* src_signal, bool discnct)
{
  static const char* slot_nm = SLOT(ItemRemoving(ISelectableItem*));
  if (discnct)
    QObject::disconnect(src_obj, src_signal, helper, slot_nm);
  else
    QObject::connect(src_obj, src_signal, helper, slot_nm);
}

void ISelectableHost::ctxtMenu_destroyed() {
  if (ctxt_ms) {
    delete ctxt_ms;
    ctxt_ms = NULL;
  }
  ctxt_item = NULL;
}

void ISelectableHost::DropEditAction(int ea) {
  ISelectable* ci = ctxt_item; 
  if (!ci) return;
  ci->EditActionD_impl_(ctxt_ms, ea, ISelectable::GC_DEFAULT);
}

void ISelectableHost::EditAction(int ea, 
    ISelectable::GuiContext gc_typ) 
{
  ISelectable* ci = curItem();
  if (!ci) return;
  // delete is a special case
  if (ea & taiClipData::EA_DELETE) {
    if(taMisc::delete_prompts) {
      int chs = taMisc::Choice("Are you sure you want to delete the selected object(s)?",
			       "&No", "&Yes");
      if (chs != 1) return;
    }
    EditAction_Delete(gc_typ);
  } else {
    ISelectable_PtrList items(selItems());
    ci->EditAction_(items, ea, gc_typ);
  }
}

void ISelectableHost::EditAction_Delete(ISelectable::GuiContext gc_typ) {
  const ISelectable_PtrList& items = selItems();
  // first, compile a ref list of all taBase guys
  taBase_RefList ta_items;
  for (int i = 0; i < items.size; ++i) {
    ISelectable* ci = items.SafeEl(i); 
    if (!ci) continue;
    taBase* tab =  ci->taData(gc_typ);// is the effLink data
    if (!tab) continue;
    ta_items.Add(tab);
  }
  // now, just request deletion -- items could go missing
  // if we've selected items that will get deleted by other items 

  taProject* proj = myProject();
  if(proj) {
    taBase* tab = ta_items.Peek();
    if(ta_items.size > 1)
      proj->undo_mgr.SaveUndo(tab, "Delete " + String(ta_items.size) + " items");
    else
      proj->undo_mgr.SaveUndo(tab, "Delete");
  }

  for (int i = ta_items.size - 1; i >= 0; --i) {
    taBase* tab = ta_items.SafeEl(i);
    if (!tab) continue;
    tab->Close(); // if happens immediately, will get removed from list
  }
}

void ISelectableHost::EditActionsEnabled(int& ea) {
  ISelectable* ci = curItem();
  if (!ci) return;
  ISelectable_PtrList items(selItems());
  int rval = ci->QueryEditActions_(items);
  // certain things disallowed if more than one item selected
  if (items.size > 1) {
    rval &= ~(taiClipData::EA_FORB_ON_MUL_SEL);
  }
  ea = rval;
}

void ISelectableHost::Emit_NotifySignal(NotifyOp op) {
  helper->Emit_NotifySignal(op);
}

void ISelectableHost::FillContextMenu(taiActions* menu) {
  if (sel_items.size == 0) return; // shouldn't happen
  QObject::connect(menu, SIGNAL(destroyed()), helper, SLOT(ctxtMenu_destroyed()) );
  ISelectable_PtrList& sel_items = selItems(); 
  ISelectable* item = sel_items.FastEl(0);
  if (sel_items.size == 1) {
    ctxt_ms = taiMimeSource::NewFromClipboard(); // deleted in the destroyed() handler
    ctxt_item = item;
  }
  
  FillContextMenu_pre(sel_items, menu);
  
  ISelectable::GuiContext sh_typ = item->shType();
  // init the dyn context
  dyn_idx = 0;
  
  if (sh_typ == ISelectable::GC_SINGLE_DATA) {
    FillContextMenu_int(sel_items, menu, 0, sh_typ);
  } else { // dual, make submenus
    String view_cap = "View";
    String obj_cap = "Object"; 
    item->GetContextCaptions(view_cap, obj_cap);
    taiMenu* sub = menu->AddSubMenu(view_cap);
    FillContextMenu_int(sel_items, sub, 0, ISelectable::GC_DUAL_DEF_VIEW);
    sub = menu->AddSubMenu(obj_cap);
    FillContextMenu_int(sel_items, sub, 1, ISelectable::GC_DUAL_DEF_DATA);
  }
  
  FillContextMenu_post(sel_items, menu);
  
  // have to delete ms now, because Qt deletes MimeSource from clipboard in event loop
  if (ctxt_ms) {
    delete ctxt_ms;
    ctxt_ms = NULL;
  }
}

void ISelectableHost::FillContextMenu_int(ISelectable_PtrList& sel_items,
  taiActions* menu, int dyn_list, ISelectable::GuiContext sh_typ)
{
  UpdateMethodsActions(dyn_list, sh_typ);
  
  // do the item-mediated portion
  ISelectable* ci = curItem();
  if (ci) {
    // start with dynamic actions
    if (dyn_actions[dyn_list].count() != 0) {
      AddDynActions(menu, dyn_list, sh_typ);
      menu->AddSep();
    }
    ci->FillContextMenu(sel_items, menu, sh_typ);
  }
}

void ISelectableHost::DoDynAction(int idx) {
  // find the right list, and adjust idx to refer to that list
  //const int max_lists = 1; // 2 lists
  int list = 0;
  while (idx > dyn_methods[list].size) {
    idx -= dyn_methods[list].size;
    list++;
  }
  DynMethod_PtrList& dyn_methods = this->dyn_methods[list];
//nn  taiAction_List&    dyn_actions = this->dyn_actions[list]; 
  ISelectable::GuiContext gui_ctxt = dyn_context[list];
  
  //note: we really won't have been called if any items don't have links,
  // but we have code in here to bail anyway if we do (maybe should put warning text?)
  if ((idx < 0) || (idx >= dyn_methods.size)) return; // shouldn't happen
  DynMethodDesc* dmd = dyn_methods.FastEl(idx);
  // NOTE: this function is based on the ta_qtdata:taiMethod::CallFun function
  // NOTE: we can't show the return values from any of the functions

  MethodDef* meth =  dmd->md;
  int use_argc = meth->fun_argc;
  int hide_args = (dmd->dmd_type == DynMethod_PtrList::Type_1N) ? 0 : 1;

  if (meth->stubp == NULL) return;
  // type of calling method's class
  // NOTE: we may look up exact type for each inst later
  TypeDef* typ = meth->owner->owner;
  // we use a smart ref in case anchor obj ever disappears in multi ops
  void* base; // the object calling the method

//TODO:  ApplyBefore();
  String argc_str = meth->OptionAfter("ARGC_");
  if (argc_str != "")
    use_argc = (int)argc_str;
  use_argc = MIN(use_argc, meth->arg_types.size);
  use_argc = MIN(use_argc, meth->arg_names.size);

  // use a copy of selected items list, to avoid issues if items change during these operations
  //NOTE: a bit hacky since adding dynamic drop actions...
  // if we are in the middle of a Drop, then ctxt_ms will have been set
  // otherwise, we have to recreate it here, since Qt deletes MimeSource guys
  // from clipboard automatically in the event loops, so we couldn't cache it
  ISelectable_PtrList sel_items_cp;
  if (dmd->dmd_type == DynMethod_PtrList::Type_MimeN_N) {
    if (!ctxt_item) return; // not supposed to happen
    sel_items_cp.Add(ctxt_item);
  } else {
    sel_items_cp = sel_items;
  }

  // if params to prompt for, do that now
  // NOTE: if it is also CONFIRM, we implicitly use param collection as confirm
  int prompt_argc = use_argc - hide_args;
  cssiArgDialog* arg_dlg = NULL; // if needed 
  cssEl** prompt_params = NULL; // if needed -- points inside arg_dlg
  if( (prompt_argc != 0) || meth->HasOption("CONFIRM")) {
    base = curItem()->effLink(gui_ctxt)->data();
    arg_dlg = new cssiArgDialog(meth, typ, base, use_argc, hide_args);
    if (base && typ->InheritsFrom(TA_taBase)) {
      bool ok;
      iColor bgclr = ((taBase*)base)->GetEditColorInherit(ok);
      if (ok) arg_dlg->setBgColor(bgclr);
    }
    arg_dlg->Constr("", "");
    int ok_can = arg_dlg->Edit(true);	// true = wait for a response
    if (!(ok_can && !arg_dlg->err_flag))
      goto exit;
    // args in dlg are now: arg[0] arg[1] .. hide_args .. use_args
    // need to have [0] [1] be a dummy, since actual args are from [2]
    prompt_params = &(arg_dlg->obj->members->els[hide_args]);
    // make sure argc is now right
    prompt_argc = arg_dlg->obj->members->size-(1+hide_args);
  }
//TODO    GenerateScript();
#ifdef DMEM_COMPILE
    // don't actually run the command when using gui in dmem mode: everything happens via the script!
    if (taMisc::dmem_nprocs == 1)
#endif
    { // block for jumps
      cssEl* rval = NULL; // ignored
      int i;
      ISelectable* itN;
      taiDataLink* link = NULL;
      if (dmd->dmd_type == DynMethod_PtrList::Type_1N) { // same for all
        while (sel_items_cp.size > 0) {
          itN = sel_items_cp.TakeItem(0);
          typ = itN->GetEffDataTypeDef(gui_ctxt);
          link = itN->effLink(gui_ctxt);
          if (!link) continue;
          base = link->data();
          if (!base) continue;
          rval = (*(meth->stubp))(base, prompt_argc, prompt_params);
	  if(rval) {
	    cssEl::Ref(rval);
	    cssEl::unRefDone(rval);
	  }
        }
        goto exit;
      }
      
      // remaining types use more complicated params
      { // for jumps    
      cssEl** param = (cssEl**)calloc(3 + prompt_argc, sizeof(cssEl*));
      param[0] = &cssMisc::Void; // method*
      param[1] = &cssMisc::Void; // this*
      param[2] = new cssCPtr();
      for (i = 2; i <= prompt_argc; ++i)
        param[1+i] = prompt_params[i]; 
        
      const int si_presize = sel_items_cp.size; // size before ops
      switch(dmd->dmd_type) {
      case DynMethod_PtrList::Type_1_2N: { // call 1 with 2:N as a param
        //NOTE: we insist list size doesn't change, i.e. no deletions,
        // else we bail!
        ISelectable* it1 = sel_items_cp.FastEl(0); // the anchor item
        typ = it1->GetEffDataTypeDef(gui_ctxt);
        link = it1->effLink(gui_ctxt);
        if (!link) goto free_mem;
        base = link->data();
        for (int i = 1; 
          (i < sel_items_cp.size) && (sel_items_cp.size == si_presize);
          ++i) 
        {
          itN = sel_items_cp.FastEl(i);
          link = itN->effLink(gui_ctxt); //note: prob can't be null, because we wouldn't get called
          if (!link) continue;
          *param[2] = (void*)link->data();
          rval = (*(meth->stubp))(base, 1 + prompt_argc, param); 
	  if(rval) {
	    cssEl::Ref(rval);
	    cssEl::unRefDone(rval);
	  }
        }
      } break;
      case DynMethod_PtrList::Type_2N_1: { // call 2:N with 1 as param
        ISelectable* it1 = sel_items_cp.FastEl(0);
        typ = it1->GetEffDataTypeDef(gui_ctxt);
        link = it1->effLink(gui_ctxt);
        if (!link) goto free_mem; //note: we prob wouldn't get called if any were null
        *param[2] = (void*)link->data();
        for (int i = 1; 
          (i < sel_items_cp.size) && (sel_items_cp.size == si_presize);
          ++i) 
        {
          itN = sel_items_cp.FastEl(i);
          link = itN->effLink(gui_ctxt);
          if (!link) continue; // prob won't happen
          base = link->data();
          rval = (*(meth->stubp))(base, 1 + prompt_argc, param); // note: "array" of 1 item
	  if(rval) {
	    cssEl::Ref(rval);
	    cssEl::unRefDone(rval);
	  }
        }
      } break;
      case DynMethod_PtrList::Type_MimeN_N: { // call 1:N with ms_objs[1..N] as params
        //NOTE: sel_items actually contains the drop target item
        if (!ctxt_ms) { // ctxt (not drop) mode -- need to make it
          ctxt_ms =  taiMimeSource::NewFromClipboard(); // deleted in the destroyed() handler
        }
        ISelectable* it1 = sel_items_cp.FastEl(0);
        typ = it1->GetEffDataTypeDef(gui_ctxt);
        for (int j = 0; j < ctxt_ms->count(); ++j) {
          ctxt_ms->setIndex(j);
          taBase* obj = ctxt_ms->tabObject();
          if (!obj) continue;
          *param[2] = (void*)obj;
          while (sel_items_cp.size > 0) {
            itN = sel_items_cp.TakeItem(0);
            link = itN->effLink(gui_ctxt);
            if (!link) continue; // prob won't happen, because we wouldn't have been called
            base = link->data();
            rval = (*(meth->stubp))(base, 1 + prompt_argc, param); 
            if (link->isBase())
              ((taBase*)base)->UpdateAfterEdit();
	    if(rval) {
	      cssEl::Ref(rval);
	      cssEl::unRefDone(rval);
	    }
          }
        }
      } break;
      default: break; // compiler food, we handled all cases
      }
free_mem:
      delete param[2];
      free(param);
      } // for jumps     
    }
exit:
  if (arg_dlg) {
    delete arg_dlg;
  }

}

iMainWindowViewer* ISelectableHost::mainWindow() const {
  QWidget* wid = const_cast<ISelectableHost*>(this)->widget();
  iMainWindowViewer* rval = NULL;
  while (wid && !(rval = qobject_cast<iMainWindowViewer*>(wid)))
    wid = wid->parentWidget();
  return rval;
}

taProject* ISelectableHost::myProject() const {
  iMainWindowViewer* imwv = mainWindow();
  if(!imwv) return NULL;
  return imwv->myProject();
}

taProject* ISelectableHost::curProject() const {
  iMainWindowViewer* imwv = mainWindow();
  if(!imwv) return NULL;
  return imwv->curProject();
}

bool ISelectableHost::RemoveSelectedItem(ISelectable* item,  bool forced) {
  bool rval = sel_items.RemoveEl(item); // use raw list, because we are building
  if ((rval || forced) && (m_sel_chg_cnt == 0))
    SelectionChanged(forced);
  return rval;
}

void ISelectableHost::SelectionChanged(bool forced) {
  if (forced) {
    m_sel_chg_cnt = -1;
    UpdateSelectedItems_impl();
    m_sel_chg_cnt = 0;
  }
  dyn_idx = 0;
  UpdateMethodsActions(0);
  Emit_NotifySignal(OP_SELECTION_CHANGED); // note: sent through event loop first
}

void ISelectableHost::SelectionChanging(bool begin, bool forced) {
  // if -ve, we are in the Update routine, so we basically ignore, by counting backwards
  if (begin)
    if (m_sel_chg_cnt < 0) --m_sel_chg_cnt; else ++m_sel_chg_cnt;
  else {
    if (m_sel_chg_cnt < 0) ++m_sel_chg_cnt; else --m_sel_chg_cnt;
    if (m_sel_chg_cnt == 0)
      SelectionChanged(forced);
  }
}

void ISelectableHost::UpdateMethodsActions(int dyn_list, ISelectable::GuiContext gc_typ) {
  dyn_context[dyn_list] = gc_typ;
  // enumerate dynamic methods
  dyn_methods[dyn_list].Reset();
  // if one dst, add the drop actions
  ISelectable_PtrList& sel_items = selItems();
  if (ctxt_ms && ctxt_item) {
    dyn_methods[dyn_list].FillForDrop(*ctxt_ms, ctxt_item);
  }
  dyn_methods[dyn_list].Fill(sel_items, gc_typ);

  // dynamically create actions
  dyn_actions[dyn_list].Reset(); // note: items ref deleted if needed
  for (int i = 0; i < dyn_methods[dyn_list].size; ++i) {
    DynMethodDesc* dmd = dyn_methods[dyn_list].FastEl(i);
    taiAction* act = new taiAction(dyn_idx, dmd->md->GetLabel(), QKeySequence(), dmd->md->name );
    QObject::connect(act, SIGNAL(IntParamAction(int)), helper, SLOT(DynAction(int)));
    dyn_actions[dyn_list].Add(act);
    dyn_idx++;
  }
}

void ISelectableHost::UpdateMethodsActionsForDrop() {
  dyn_context[0] = ISelectable::GC_DEFAULT;
  // enumerate dynamic methods
  dyn_methods[0].Reset();
  dyn_actions[0].Reset(); // note: items ref deleted if needed
  if (!ctxt_ms || !ctxt_item) return;
  dyn_methods[0].FillForDrop(*ctxt_ms, ctxt_item);

  // dynamically create actions
  for (int i = 0; i < dyn_methods[0].size; ++i) {
    DynMethodDesc* dmd = dyn_methods[0].FastEl(i);
    taiAction* act = new taiAction(i, dmd->md->GetLabel(), QKeySequence(), dmd->md->name );
    QObject::connect(act, SIGNAL(IntParamAction(int)), helper, SLOT(DynAction(int)));
    dyn_actions[0].Add(act);
    dyn_idx++;
  }
}

void ISelectableHost::setCurItem(ISelectable* item, bool forced) {
  ISelectable_PtrList& sel_items = selItems(); // in case overridden
  ISelectable* ci = sel_items.SafeEl(0);
  if ((ci == item) && (sel_items.size <= 1) && (!forced)) return;

  SelectionChanging(true, forced);
    ClearSelectedItems(forced);
    if (item)
      AddSelectedItem(item, forced);
  SelectionChanging(false, forced);
}


//////////////////////////////////
//  SelectableHostHelper	//
//////////////////////////////////


void SelectableHostHelper::customEvent(QEvent* ev) {
  switch ((int)ev->type()) {
  case iDataViewer_SelectionChanged_EventType:
    emit NotifySignal(host, ISelectableHost::OP_SELECTION_CHANGED);
    break;
  default:
    inherited::customEvent(ev);
    break;
  }
}

void SelectableHostHelper::EditAction(taiAction* act) {
  int ea = act->usr_data.toInt();
  ISelectable::GuiContext gc = (ISelectable::GuiContext)act->data().toInt();
  host->EditAction(ea, gc);
} 

void SelectableHostHelper::Emit_NotifySignal(ISelectableHost::NotifyOp op) {
  // selection ops need to go through the event loop or things get weird and nasty...
  if (op == ISelectableHost::OP_SELECTION_CHANGED) {
    QEvent* ev = new QEvent((QEvent::Type)iDataViewer_SelectionChanged_EventType);
    QCoreApplication::postEvent(this, ev); // returns immediately
  } else
    emit NotifySignal(host, op);
}



//////////////////////////////////
//  iFrameViewer		//
//////////////////////////////////

iFrameViewer::iFrameViewer(FrameViewer* viewer_, QWidget* parent)
:inherited(parent), IDataViewWidget(viewer_)
{
  Init();
  // note: caller will still do a virtual Constr() on us after new
}

iFrameViewer::~iFrameViewer() {
}

void iFrameViewer::Init() {
  shn_changing = 0;
}

void iFrameViewer::hideEvent(QHideEvent* e) {
  inherited::hideEvent(e);
  Showing(false);
}

void iFrameViewer::showEvent(QShowEvent* e) {
  inherited::showEvent(e);
  Showing(true);
}

void iFrameViewer::Showing(bool showing) {
  iMainWindowViewer* dv = viewerWindow();
  if (!dv) return;
  taiAction* me = dv->frameMenu->FindActionByData((void*)this);
  if (!me) return;
  if (showing == me->isChecked()) return;
  me->setChecked(showing); //note: triggers event
}

void iFrameViewer::SelectableHostNotifySlot_Internal(ISelectableHost* src, int op) {
  ++shn_changing;
    emit SelectableHostNotifySignal(src, op);
  --shn_changing;
}

void iFrameViewer::SelectableHostNotifySlot_External(ISelectableHost* src, int op) {
  if (shn_changing > 0) return; // reflection back down, ignore it
  switch (op) {
  case ISelectableHost::OP_GOT_FOCUS:
  case ISelectableHost::OP_SELECTION_CHANGED:
    SelectionChanged_impl(src);
    break;
  case ISelectableHost::OP_DESTROYING: break;
  default: break; // shouldn't happen
  }
  
}

MainWindowViewer* iFrameViewer::mainWindowViewer() {
  return (m_viewer) ? ((FrameViewer*)m_viewer)->mainWindowViewer() : NULL;
}

//////////////////////////
//   iBrowseViewer 	//
//////////////////////////

iBrowseViewer::iBrowseViewer(BrowseViewer* browser_, QWidget* parent)
:inherited(browser_, parent)
{
  Init();
}

iBrowseViewer::~iBrowseViewer()
{
}

void iBrowseViewer::Init() {
  mnuBrowseNodeDrop_param = -1;
  QVBoxLayout* lay = new QVBoxLayout(this);
  lay->setMargin(0);  lay->setSpacing(0);
  lvwDataTree = new iTreeView(this);
  lay->addWidget(lvwDataTree);
  lvwDataTree->main_window = mainWindowViewer()->widget();
  lvwDataTree->setObjectName("lvwDataTree");
  lvwDataTree->setSortingEnabled(false); // preserve enumeration order of items
  lvwDataTree->setSelectionMode(QAbstractItemView::ExtendedSelection); // multiselect
  lvwDataTree->setDefaultExpandLevels(8); // set fairly deep for ExpandAll
  lvwDataTree->setColumnCount(1);
  lvwDataTree->AddColDataKey(0, "", Qt::StatusTipRole); // show status tip
  lvwDataTree->AddColDataKey(0, "", Qt::ToolTipRole); // and tool tip
  lvwDataTree->header()->hide();
  //enable dnd support
  lvwDataTree->setDragEnabled(true);
  lvwDataTree->setAcceptDrops(true);
  lvwDataTree->setDropIndicatorShown(true);
  lvwDataTree->setHighlightRows(true); // show error objects
/*no! prevents from collapsing in splitters  int mw = (taiM->scrn_s.width() * 3) / 20; // 15% min for tree
  lvwDataTree->resize(mw, lvwDataTree->height()); // 15% min for tree
  lvwDataTree->setMinimumWidth(mw); // 15% min for tree
*/
  connect(lvwDataTree, SIGNAL(FillContextMenuHookPost(ISelectable_PtrList&, taiActions*)),
      this, SLOT(lvwDataTree_FillContextMenuHookPost(ISelectable_PtrList&, taiActions*)) );
  lvwDataTree->Connect_SelectableHostNotifySignal(this, 
    SLOT(SelectableHostNotifySlot_Internal(ISelectableHost*, int)) );
}

void iBrowseViewer::ApplyRoot() {
  void* m_root = root(); //cache
  if (!m_root) return;
  taiDataLink* dl = taiViewType::StatGetDataLink(m_root, root_typ());
  if (!dl) return; // shouldn't happen...

  // by definition, we should always be able to create a new browser on root of a browser
  int dn_flags_ = iTreeViewItem::DNF_CAN_BROWSE;
  
  // we treat root slightly different if it is true root, or is just a subsidiary named item
  // also, we assume this guy is visible, so we don't apply the filter
  taiTreeDataNode* node;
  if (m_root == tabMisc::root)
    node = dl->CreateTreeDataNode(root_md(), lvwDataTree, NULL, "root",
      dn_flags_ | iTreeViewItem::DNF_IS_MEMBER | iTreeViewItem::DNF_NO_UPDATE_NAME);
  else {
    // if root is a member, we use that name, else the obj name
    MemberDef* md = root_md();
    String root_nm;
    if (md) 
      root_nm = md->name;
    else {
      root_nm = dl->GetName();
      dn_flags_ |= iTreeViewItem::DNF_UPDATE_NAME; // change it on data changes
    }
    node = dl->CreateTreeDataNode(md, lvwDataTree, NULL, root_nm, 
      dn_flags_);
  }
  // always show the first items under the root
  node->CreateChildren();
  lvwDataTree->setCurrentItem(node);//setCurItem(node);
  lvwDataTree->setItemExpanded(node, true); // always open root node
}

void iBrowseViewer::lvwDataTree_FillContextMenuHookPost(ISelectable_PtrList& /*sel_items*/,
   taiActions* menu) 
{
//note: nothing
}

void iBrowseViewer::Refresh_impl() {
  lvwDataTree->Refresh();
  inherited::Refresh_impl();
}

void iBrowseViewer::Reset() {
  lvwDataTree->clear();
}


//////////////////////////
//   iTabViewer 	//
//////////////////////////

iTabViewer::iTabViewer(PanelViewer* viewer_, QWidget* parent)
: inherited(viewer_, parent)
{
  Init();
}

/* iTabViewer::iTabViewer(DataViewer* viewer_, QWidget* parent, WFlags fl)
: iMainWindowViewer(viewer_, parent, fl)
{
  init();
} */

iTabViewer::~iTabViewer()
{
//  delete m_tabViews;
//  m_tabViews = NULL;
  if (m_curTabView) {
    delete m_curTabView;
    m_curTabView = NULL;
  }
}

void iTabViewer::Init() {
//  m_tabViews = new  iTabView_PtrList();
  cur_item = NULL;
  QVBoxLayout* lay = new QVBoxLayout(this);
  lay->setMargin(0);  lay->setSpacing(0);
  m_curTabView = new iTabView(this, this);
  lay->addWidget(m_curTabView);
//  m_tabViews->Add(rval);
//nn  m_curTabView->show();
}

void iTabViewer::AddPanel(iDataPanel* panel) {
  tabView()->AddPanel(panel);
}

void iTabViewer::AddPanelNewTab(iDataPanel* panel, bool lock) {
  tabView()->AddPanelNewTab(panel, lock);
}

void iTabViewer::AddTab() {
  tabView()->AddTab();
}

void iTabViewer::CloseTab() {
  tabView()->CloseTab();
}

void iTabViewer::Closing(CancelOp& cancel_op) {
  tabView()->Closing(cancel_op);
}

void iTabViewer::Constr_post() {
/*  for (int i = 0; i < m_tabViews->size; ++i) {
    iTabView* itv = m_tabViews->FastEl(i);
    itv->OnWindowBind(this);
  }*/
  if (m_curTabView) {
    m_curTabView->OnWindowBind(this);
  }
}

void iTabViewer::GetWinState_impl() {
  /*for (int i = 0; i < m_tabViews->size; ++i) {
    iTabView* itv = m_tabViews->FastEl(i);
    itv->GetWinState();
  }*/
  if (m_curTabView) {
    m_curTabView->GetWinState();
  }
}

void iTabViewer::SetWinState_impl() {
  /*for (int i = 0; i < m_tabViews->size; ++i) {
    iTabView* itv = m_tabViews->FastEl(i);
    itv->SetWinState();
  }*/
  if (m_curTabView) {
    m_curTabView->SetWinState();
  }
}

void iTabViewer::Refresh_impl() {
 /* for (int i = 0; i < m_tabViews->size; ++i) {
    iTabView* itv = m_tabViews->FastEl(i);
    itv->Refresh();
  }*/
  if (m_curTabView) {
    m_curTabView->Refresh();
  }
}

void iTabViewer::ResolveChanges_impl(CancelOp& cancel_op) {
  /*for (int i = 0; i < m_tabViews->size; ++i) {
    iTabView* itv = m_tabViews->FastEl(i);
    if (itv) itv->ResolveChanges(cancel_op);
    if (cancel_op == CO_CANCEL) break;
  }
  }*/
  if (m_curTabView) {
    m_curTabView->ResolveChanges(cancel_op);
  }
}

void iTabViewer::SelectionChanged_impl(ISelectableHost* src_host) {
  //TODO: should actually check if old panel=new panel, since theoretically, two different
  // gui items can have the same datalink (i.e., underlying data)
  iDataPanel* new_pn = NULL;
  cur_item = src_host->curItem(); // note: could be NULL
  if (!cur_item) goto end; // no selected item, so no change
  if (m_curTabView) {
    taiDataLink* link = cur_item->effLink();
    if (link) {
      new_pn = m_curTabView->GetDataPanel(link);
      m_curTabView->ShowPanel(new_pn);
    }
  }
end:
  inherited::SelectionChanged_impl(src_host); // prob does nothing
}

void iTabViewer::TabView_Destroying(iTabView* tv) {
  if (m_curTabView != tv) return;
  m_curTabView = NULL;
 /* int idx = m_tabViews->FindEl(tv);
  m_tabViews->RemoveIdx(idx);
  if (m_curTabView != tv) return;
  // focus next, if any, or prev, if any
  if (idx >= m_tabViews->size) --idx;
  TabView_Selected(m_tabViews->PosSafeEl(idx)); // NULL if no more*/
}

void iTabViewer::TabView_Selected(iTabView* tv) {
  if (m_curTabView == tv) return;
  if (m_curTabView) {
    m_curTabView->Activated(false);
  }
  m_curTabView = tv;
  if (m_curTabView) {
    m_curTabView->Activated(true);
  }
}

void iTabViewer::ShowLink(taiDataLink* link, bool not_in_cur) {
  tabView()->ShowLink(link, not_in_cur);
}

void iTabViewer::ShowPanel(iDataPanel* panel) {
  tabView()->ShowPanel(panel);
}

void iTabViewer::UpdateTabNames() { // called by a datalink when a tab name might have changed
  tabView()->UpdateTabNames();
}


//////////////////////////
//  iDockViewer		//
//////////////////////////

iDockViewer::iDockViewer(DockViewer* viewer_, QWidget* parent)
:inherited(parent), IDataViewWidget(viewer_)
{
  setAttribute(Qt::WA_DeleteOnClose, true);
  Init();
}

iDockViewer::~iDockViewer() {
}

void iDockViewer::Init() {
  // set the features
  DockViewer::DockViewerFlags dock_flags = viewer()->dock_flags; // cache
  DockWidgetFeatures dwf = 0;
  if (dock_flags & DockViewer::DV_CLOSABLE)
    dwf |= QDockWidget::DockWidgetClosable;
  if (dock_flags & DockViewer::DV_MOVABLE)
    dwf |= QDockWidget::DockWidgetMovable;
  if (dock_flags & DockViewer::DV_FLOATABLE)
    dwf |= QDockWidget::DockWidgetFloatable;
  setFeatures(dwf);
}

void iDockViewer::closeEvent(QCloseEvent* e) {
   // always closing if force-quitting, docked or we no longer have our mummy
  CancelOp cancel_op = ((taMisc::quitting == taMisc::QF_FORCE_QUIT) ||
    !isFloating() || (!m_viewer)) ? 
    CO_NOT_CANCELLABLE : CO_PROCEED; 
  closeEvent_Handler(e, cancel_op);
}

bool iDockViewer::event(QEvent* ev) {
  bool rval = inherited::event(ev);
  if (ev->type() == QEvent::WindowActivate)
    taiMisc::active_wins.GotFocus_DockWindow(this);
  return rval;
}

void iDockViewer::hideEvent(QHideEvent* e) {
  inherited::hideEvent(e);
  Showing(false);
}

void iDockViewer::showEvent(QShowEvent* e) {
  inherited::showEvent(e);
  Showing(true);
}

void iDockViewer::Showing(bool showing) {
  DockViewer* vw = viewer();
  if (!vw) return; // shouldn't happen
  MainWindowViewer* wvw = (MainWindowViewer*)vw->GetOwner(&TA_MainWindowViewer);
  if (!wvw) return; // normally shouldn't happen for owned docks
  iMainWindowViewer* dv = wvw->viewerWindow();
  if (!dv) return;
  taiAction* me = dv->dockMenu->FindActionByData((void*)this);
  if (!me) return;
  if (showing == me->isChecked()) return;
  me->setChecked(showing); //note: triggers event
}


//////////////////////////
//   iToolBoxDockViewer	//
//////////////////////////

IDataViewWidget* ToolBoxDockViewer::ConstrWidget_impl(QWidget* gui_parent) {
    return new iToolBoxDockViewer(this, gui_parent); // usually parented later
}


iToolBoxDockViewer::iToolBoxDockViewer(ToolBoxDockViewer* viewer_, QWidget* parent)
:inherited(viewer_, parent)
{
  Init();
}

iToolBoxDockViewer::~iToolBoxDockViewer() {
}

void iToolBoxDockViewer::Init() {
  tbx = new QToolBox();
  //note: if we don't set font, tabs seem to have too big a font
  tbx->setFont(taiM->buttonFont(taiMisc::defFontSize));
  setWidget(tbx);
}

int iToolBoxDockViewer::AssertSection(const String& sec_name) {
  int sec = -1;
  for (int i = 0; i < tbx->count(); ++i) {
    if (tbx->itemText(i) == sec_name) {
      sec = i;
      break;
    }
  }
  if (sec < 0) {
    QToolBar* tb = new QToolBar;
    tb->setOrientation(Qt::Vertical);
    sec = tbx->addItem(tb, sec_name);
  }
  return sec;
}

void iToolBoxDockViewer::AddClipToolWidget(int sec, iClipWidgetAction* cwa) {
  QToolBar* w = sectionWidget(sec);
  if (!w) return; // user didn't assert
  // don't leak!
  if (cwa->parent() == NULL) {
    cwa->setParent(this);
  }
  w->addAction(cwa); 
  connect(cwa, SIGNAL(triggered()), cwa, SLOT(copyToClipboard()) ); // ie to self
}

void iToolBoxDockViewer::AddSeparator(int sec) {
  QToolBar* w = sectionWidget(sec);
  if (!w) return; // user didn't assert
  w->addSeparator();
}

void iToolBoxDockViewer::Constr_post() {
  ToolBoxRegistrar_PtrList* list = ToolBoxRegistrar::instances();
  for (int i = 0; i < list->size; ++i) {
    ToolBoxProc proc = list->FastEl(i)->proc;
    proc(this);
  }
}

QToolBar* iToolBoxDockViewer::sectionWidget(int sec) {
  QToolBar* rval = qobject_cast<QToolBar*>(tbx->widget(sec));
  return rval;
}

//////////////////////////
//   iToolBar 		//
//////////////////////////

IDataViewWidget* ToolBar::ConstrWidget_impl(QWidget* gui_parent) {
  if (name == "Application")
    return new iApplicationToolBar(this, gui_parent); // usually parented later
  else
    return new iToolBar(this, gui_parent); // usually parented later
}

iToolBar::iToolBar(ToolBar* viewer_, QWidget* parent)
:inherited(parent), IDataViewWidget(viewer_)
{
  Init();
}

iToolBar::~iToolBar() {
}

void iToolBar::Init() {
  setWindowTitle(viewer()->GetName());
}

void iToolBar::hideEvent(QHideEvent* e) {
  inherited::hideEvent(e);
  Showing(false);
}

void iToolBar::showEvent(QShowEvent* e) {
  inherited::showEvent(e);
  Showing(true);
}

void iToolBar::Showing(bool showing) {
  iMainWindowViewer* dv = viewerWindow();
  if (!dv) return;
  taiAction* me = dv->toolBarMenu->FindActionByData((void*)this);
  if (!me) return;
  if (showing == me->isChecked()) return;
  me->setChecked(showing); //note: triggers event
}


//////////////////////////////////
//  iToolBar_List 		//
//////////////////////////////////

String iToolBar_List::El_GetName_(void* it) const {
  return ((QWidget*)it)->objectName(); 
}


//////////////////////////
//   iApplicationToolBar//
//////////////////////////

void iApplicationToolBar::Constr_post() {
  iMainWindowViewer* win = viewerWindow(); //cache
  iToolBar* tb = this;
  int icon_sz = taiM_->label_height(taiMisc::sizMedium);
  tb->setIconSize(QSize(icon_sz, icon_sz));
  win->historyBackAction->addTo(tb);
  win->historyForwardAction->addTo(tb);
  //TEMP 
  QToolButton* but = qobject_cast<QToolButton*>(tb->widgetForAction(win->historyBackAction));
  if (but) {but->setArrowType(Qt::LeftArrow); but->setText("");}
  but = qobject_cast<QToolButton*>(tb->widgetForAction(win->historyForwardAction));
  if (but) {but->setArrowType(Qt::RightArrow); but->setText("");}
  win->editFindAction->addTo(tb);
  tb->addSeparator();
  win->fileNewAction->addTo(tb);
  win->fileOpenAction->addTo(tb);
  win->fileSaveAction->addTo(tb);
  win->fileSaveAsAction->addTo(tb);
//   win->fileSaveNotesAction->addTo(tb);
  win->fileUpdateChangeLogAction->addTo(tb);
  win->fileCloseAction->addTo(tb);
//   win->filePrintAction->addTo(tb);
  tb->addSeparator();
  win->editUndoAction->addTo(tb);
  win->editRedoAction->addTo(tb);
  tb->addSeparator();
  win->editCutAction->addTo(tb);
  win->editCopyAction->addTo(tb);
  win->editPasteAction->addTo(tb);
  win->editPasteIntoAction->addTo(tb);
  win->editPasteAssignAction->addTo(tb);
  win->editPasteAppendAction->addTo(tb);
  tb->addSeparator();
  win->helpHelpAction->addTo(tb);
}

//////////////////////////
//  iBaseClipWidgetAction //
//////////////////////////

iBaseClipWidgetAction::iBaseClipWidgetAction(taBase* inst_, QObject* parent)
:inherited(parent)
{
  Init(inst_);
}

iBaseClipWidgetAction::iBaseClipWidgetAction(const QIcon & icon_, taBase* inst_,
 QObject* parent)
:inherited(parent)
{
  Init(inst_);
  setIcon(icon_);
}

iBaseClipWidgetAction::iBaseClipWidgetAction(const String& tooltip_, const QIcon & icon_,
    taBase* inst_, QObject* parent)
:inherited(parent)
{
  Init(inst_, tooltip_);
  setIcon(icon_);
}

iBaseClipWidgetAction::iBaseClipWidgetAction(const String& text_,
    taBase* inst_, QObject* parent)
:inherited(parent)
{
  Init(inst_);
  setText(text_);
}

iBaseClipWidgetAction::iBaseClipWidgetAction(const String& tooltip_, const String& text_,
    taBase* inst_, QObject* parent)
:inherited(parent)
{
  Init(inst_, tooltip_);
  setText(text_);
}

  
void iBaseClipWidgetAction::Init(taBase* inst_, String tooltip_) {
  m_inst = inst_;
  if (tooltip_.empty() && inst_) {
    tooltip_ = inst_->GetToolTip(taBase::key_type_desc);
  }
  if (tooltip_.nonempty()) {
    setToolTip(tooltip_);
  }
  if (inst_) {
    String statustip = inst_->statusTip(); // no key
    if (statustip.nonempty())
      setStatusTip(statustip);
  }
}
  		
QMimeData* iBaseClipWidgetAction::mimeData() const {
  taiClipData* rval = NULL;
  if (m_inst) {
    taiDataLink* link = (taiDataLink*)m_inst->GetDataLink();
    if (link) {
      // get readonly clip data -- we don't know if dragging or not, so we always say we are
      taBase* obj = link->taData();
      if (obj) {
        taiObjectMimeFactory* mf = taiObjectMimeFactory::instance();
        rval = new taiClipData( 
          (taiClipData::EA_SRC_COPY | taiClipData::EA_SRC_DRAG | taiClipData::EA_SRC_READONLY));
        mf->AddSingleObject(rval, obj);
      }
    }
  }
  return rval;
}

QStringList iBaseClipWidgetAction::mimeTypes() const {
 //NOTE: for dnd to work, we just permit our own special mime type!!!
  QStringList rval;
  rval.append(taiObjectMimeFactory::tacss_objectdesc);
  return rval;
}


//////////////////////////
//  iBrowseHistory	//
//////////////////////////

iBrowseHistory::iBrowseHistory(QObject* parent) 
:inherited(parent)
{
  max_items = taMisc::num_browse_history;
  if (max_items < 10) max_items = 10; // sanity
  cur_item = -1;
}

iBrowseHistory::~iBrowseHistory() {
  reset();
}

void iBrowseHistory::addItem(taiDataLink* dl) {
  if (items.size >= max_items) {
    taiDataLink* it = items[0];
    items.RemoveIdx(0);
    itemRemoved(it);
  }
  itemAdding(dl);
  items.Add(dl);
  cur_item = items.size - 1;
  doEnabling();
}

void iBrowseHistory::back() {
  if (cur_item <= 0) goto exit;
  --cur_item;
  if (cur_item >= items.size) goto exit;
  ++navigating; {
    taiDataLink* dl = items[cur_item];
    emit select_item(dl);
  } --navigating;
  
exit:
  doEnabling();
}

void iBrowseHistory::forward() {
  if (cur_item >= items.size) goto exit;
  ++cur_item;
  if (cur_item >= items.size) goto exit;
  ++navigating; {
    taiDataLink* dl = items[cur_item];
    emit select_item(dl);
  } --navigating;
exit:
  doEnabling();
}

void iBrowseHistory::doEnabling() {
  bool be = ((cur_item > 0) && (items.size > 1));
  emit back_enabled(be);
  bool fe = ((cur_item < (items.size - 1)) && (items.size > 1));
  emit forward_enabled(fe);
}

void iBrowseHistory::DataLinkDestroying(taDataLink* dl) {
  for (int i = items.size - 1; i >= 0; --i) {
    if (dl == items[i]) {
      if (cur_item > i) -- cur_item;
      else if (cur_item == i) cur_item = items.size; // adjust later
      items.RemoveIdx(i);
    }
  }
  doEnabling();
}

void iBrowseHistory::itemAdding(taiDataLink* dl) {
  for (int i = 0; i < items.size; ++i) {
    if (dl == items[i]) return;
  }
  dl->AddDataClient(this);
}

void iBrowseHistory::itemRemoved(taiDataLink* dl) {
  for (int i = 0; i < items.size; ++i) {
    if (dl == items[i]) return;
  }
  dl->RemoveDataClient(this);
}

void iBrowseHistory::SelectableHostNotifying(ISelectableHost* src_host, int op)
{
  if (navigating) return;
  if (op != ISelectableHost::OP_SELECTION_CHANGED) return;
  iTreeView* tvw = qobject_cast<iTreeView*>(src_host->widget());
  if (!tvw) return;
  iTreeViewItem* tvi = dynamic_cast<iTreeViewItem*>(tvw->curItem());
  if (!tvi) return;
  ItemSelected(tvi);
}

void iBrowseHistory::ItemSelected(iTreeViewItem* tvi) {
  if (navigating) return;
  taiDataLink* dl = tvi->link();
  if (!dl) return;
  addItem(dl);
}


void iBrowseHistory::reset() {
  for (int i = items.size - 1; i >= 0; --i) {
    taiDataLink* dl = items[i];
    items.RemoveIdx(i);
    itemRemoved(dl);
  }
  cur_item = 0;
  //enabled?
}


//////////////////////////
//  iMainWindowViewer	//
//////////////////////////

int iMainWindowViewer::s_next_unique_id;

iMainWindowViewer::iMainWindowViewer(MainWindowViewer* viewer_, QWidget* parent)
: inherited(parent, (Qt::Window |Qt:: WindowSystemMenuHint | 
		     Qt::WindowMinMaxButtonsHint
#if (QT_VERSION >= 0x040500)
		      | Qt::WindowCloseButtonHint
#endif
		      )), IDataViewWidget(viewer_)
{
  Init();
  m_is_root = viewer_->isRoot(); // need to do before Constr
  m_is_proj_browser = viewer_->isProjBrowser(); // need to do before Constr
  m_is_proj_viewer = viewer_->isProjViewer(); // need to do before Constr
  m_unique_id = s_next_unique_id++;
  // note: caller will still do a virtual Constr() on us after new
}

/*obs iMainWindowViewer::iMainWindowViewer(DataViewer* viewer_, QWidget* parent, WFlags flags)
: QMainWindow(parent, NULL, flags)
{
  m_viewer = viewer_;
  init();
} */

iMainWindowViewer::~iMainWindowViewer() {
  taiMisc::active_wins.RemoveEl(this);
//TODO: need to delete menu, but just doing a delete causes an exception (prob because Qt
// has already deleted the menu items
  if (menu) menu->deleteLater();
  menu = NULL;
}

void iMainWindowViewer::Init() {
  setAttribute(Qt::WA_DeleteOnClose);
  //note: only a bare init -- most stuff done in virtual Constr() called after new
  brow_hist = new iBrowseHistory(this);
  menu = NULL;
  body = NULL;
  last_clip_handler = NULL;
  last_sel_server = NULL;
  m_is_root = false;
  m_is_proj_browser = false;
  m_is_proj_viewer = false;
  m_close_proj_now = false; // only ever set once
  
  // allow win to be any size, even bigger than screen -- esp important for
  // multi-monitor situations, so you can size across screens
  // note that we do constrain windows on restore to be on screen, even if
  // their metrics indicated very big -- VERY IMPORTANT FOR MAC 
//  iSize ss = taiM->scrn_s;
//  setMaximumSize(ss.width(), ss.height());

//for some GD reason, we can't get the icon to show by doing this:
//setWindowIcon(QApplication::windowIcon()); // supposed to be automatic, but doesn't seem to be...
// so we curse and swear, and do this:
  QString app_ico_nm = ":/images/" + taMisc::app_name + "_32x32.png";
  QPixmap app_ico(app_ico_nm);
  setWindowIcon(app_ico);

  setFont(taiM->dialogFont(taiM->ctrl_size));

  (void)statusBar(); // creates the status bar
  
  // these are modal, so NULL them for when unused
  historyBackAction = NULL;
  historyForwardAction = NULL;
  fileNewAction = NULL;
  fileOpenAction = NULL;
  fileSaveAction = NULL;
  fileSaveAsAction = NULL;
  fileSaveNotesAction = NULL;
  fileUpdateChangeLogAction = NULL;
  fileSaveAllAction = NULL;
  fileCloseAction = NULL;
  fileCloseWindowAction = NULL;
  fileQuitAction = NULL;
}

taiAction* iMainWindowViewer::AddAction(taiAction* act) {
  actions.Add(act); // refs the act
  // note: don't parent Actions because we manage them manually in our lists
  // note: because Qt only activates acts with shortcuts if visible, we need
  // to add the shorcutted guys to something visible... how about... us!
  if (!act->shortcut().isEmpty())
    this->addAction(act);
  return act;
}

void iMainWindowViewer::AddDockViewer(iDockViewer* dv, Qt::DockWidgetArea in_area) {
  if (!dv) return;
  addDockWidget(in_area, dv);
  // create a menu entry to show/hide it, regardless if visible now
  taiAction* act = dockMenu->AddItem(dv->viewer()->GetName(), taiMenu::toggle,
    taiAction::men_act, this, SLOT(this_DockSelect(taiAction*)), (void*)dv);
  if (dv->isVisible() != act->isChecked())
    act->setChecked(dv->isVisible()); // note: triggers action
  //TODO: maybe need to hook up signals for undocking
}

void iMainWindowViewer::AddFrameViewer(iFrameViewer* fv, int at_index) {
  if (at_index < 0) {
    body->addWidget(fv);
    at_index = body->count() - 1;
  } else
    body->insertWidget(at_index, fv);
  fv->m_window = this;
//TODO: this stretch thing isn't working -- replace with sizing
  body->setStretchFactor(at_index, fv->stretchFactor());
  
  connect(this, SIGNAL(SelectableHostNotifySignal(ISelectableHost*, int)),
    fv, SLOT(SelectableHostNotifySlot_External(ISelectableHost*, int)) );
  connect(fv, SIGNAL(SelectableHostNotifySignal(ISelectableHost*, int)), 
    this, SLOT(SelectableHostNotifySlot(ISelectableHost*, int)) );
    
  taiAction* act = frameMenu->AddItem(fv->viewer()->GetName(), taiMenu::toggle,
    taiAction::men_act, this, SLOT(this_FrameSelect(taiAction*)), (void*)fv);
  
  //TODO: the show decision should probably be elsewhere
  if (fv->viewer()->isVisible())
    fv->viewer()->Show(); // always needed when adding guys to visible
  
  if (fv->viewer()->isVisible() != act->isChecked())
    act->setChecked(fv->isVisible()); // note: triggers action
    
}

// this guy exists because we must always be able to add a panel,
// so if there isn't already a panel viewer, we have to add one
void iMainWindowViewer::AddPanel(iDataPanel* panel, bool new_tab) {
  iTabViewer* itv = GetTabViewer(true);
  if (new_tab) {
    itv->AddPanelNewTab(panel);
  } else {
    itv->AddPanel(panel); // typically for ctrl panels
  }
} 

iTabViewer* iMainWindowViewer::GetTabViewer(bool force) {
  int idx;
  PanelViewer* tv = (PanelViewer*)viewer()->FindFrameByType(&TA_PanelViewer, idx);
  if (!tv) {
    if (!force) return NULL;
    tv = (PanelViewer*)viewer()->AddFrameByType(&TA_PanelViewer);
    tv->Constr(); // parented when added
    AddFrameViewer(tv->widget(), 1); // usually in middle
  }
  return tv->widget();
} 

void iMainWindowViewer::EditItem(taiDataLink* link, bool not_in_cur) {
  iTabViewer* itv = GetTabViewer(true);
  itv->ShowLink(link, not_in_cur);
} 

void iMainWindowViewer::AddToolBar(iToolBar* itb) {
  if (!itb->parent()) 
    itb->setParent(this); // needs parent otherwise will leak
  itb->m_window = this;
  addToolBar(itb); //TODO: should specify area
  // create a menu entry to show/hide it, regardless if visible now
  toolBarMenu->AddItem(itb->objectName(), taiMenu::toggle,
    taiAction::men_act, this, SLOT(this_ToolBarSelect(taiAction*)), (void*)itb);
  // if initially invisible, hide it
  ToolBar* tb = itb->viewer();
  if (!tb) return; // shouldn't happen
  if (!tb->visible) {
    tb->Hide();
  }
//nn  toolbars.append(tb);
}

void iMainWindowViewer::ch_destroyed() {
  last_clip_handler = NULL;
  UpdateUi();
}

void iMainWindowViewer::closeEvent(QCloseEvent* e) {
   // always closing if force-quitting or we no longer have our mummy
  CancelOp cancel_op = ((taMisc::quitting == taMisc::QF_FORCE_QUIT) || (!m_viewer)) ?
     CO_NOT_CANCELLABLE : CO_PROCEED; 
  closeEvent_Handler(e, cancel_op);
  // now, if we are the last proj window, close us!
  if (m_close_proj_now) 
    curProject()->CloseLater();
}

void iMainWindowViewer::moveEvent(QMoveEvent* e) {
  QMainWindow::moveEvent(e);
  taProject* prj = curProject();
  if(!prj) return;
  if(!taMisc::console_win) return;
  QRect r = frameGeometry();
  int nw_top = r.bottom() + 1;
  int nw_ht = taiM->scrn_s.h - nw_top - 64; // leave a fixed amount of space at bottom.
  if(nw_ht < 40) nw_ht = 40;
  taMisc::console_win->resize(r.width(), nw_ht);
  taMisc::console_win->move(r.left(), nw_top);
}

void iMainWindowViewer::resizeEvent(QResizeEvent* e) {
  inherited::resizeEvent(e);
  // use this to check for initializing the hacky frame_s value
  if ((taiM->frame_s.h + taiM->frame_s.w) == 0) {
    QRect r = frameGeometry();
    taiM->frame_s.h = r.height() - height();
    taiM->frame_s.w = r.width() - width();
  }
  taProject* prj = curProject();
  if(!prj) return;
  if(!taMisc::console_win) return;
  QRect r = frameGeometry();
  int nw_top = r.bottom() + 1;
  int nw_ht = taiM->scrn_s.h - nw_top - 64; // leave a fixed amount of space at bottom.
  if(nw_ht < 40) nw_ht = 40;
  taMisc::console_win->resize(r.width(), nw_ht);
  taMisc::console_win->move(r.left(), nw_top);
}

void iMainWindowViewer::showEvent(QShowEvent* e) {
  inherited::showEvent(e);
  MainWindowViewer* db = viewer();
  if (!db) return;
  taBase* data = db->data();
  if(!data) return;
  tabMisc::DelayedFunCall_gui(data, "WindowShowHook"); // make it delayed so window should actuall show first!
}

void iMainWindowViewer::hideEvent(QHideEvent* e) {
  inherited::hideEvent(e);
  MainWindowViewer* db = viewer();
  if (!db) return;
  taBase* data = db->data();
  if(!data) return;
  tabMisc::DelayedFunCall_gui(data, "WindowHideHook");
}

void iMainWindowViewer::Constr_impl() {
  Constr_MainMenu_impl();
  Constr_Menu_impl();
  // we always put the fileclose or quit action at bottom of menu
  fileMenu->insertSeparator();
  
  if (!isRoot()) {
    fileCloseWindowAction->AddTo(fileMenu);
  }
  
  //note: on Mac, this Quit will get moved to app menu (needed on all toplevel wins)
#ifndef TA_OS_MAC
  if (isRoot())
#endif
  {
    fileQuitAction->AddTo(fileMenu);
  }

  body = new iSplitter(); // def is hor
  setCentralWidget(body);
  body->show();

  taiMisc::active_wins.AddUnique(this);
}

void iMainWindowViewer::Constr_MainMenu_impl() {
  if (menu) return;
  // create a taiMenu wrapper around the window's provided menubar
  menu = new taiMenuBar(this, taiMisc::fonBig, menuBar());

  fileMenu = menu->AddSubMenu("F&ile");
  editMenu = menu->AddSubMenu("&Edit");
  viewMenu = menu->AddSubMenu("&View");
  show_menu = menu->AddSubMenu("&Show");
  connect(show_menu->menu(), SIGNAL(aboutToShow()), this, SLOT(showMenu_aboutToShow()) );
  toolsMenu = menu->AddSubMenu("&Tools");
  windowMenu = menu->AddSubMenu("&Window");
  connect(windowMenu->menu(), SIGNAL(aboutToShow()),
    this, SLOT(windowMenu_aboutToShow()) );
  
  helpMenu = menu->AddSubMenu("&Help");;
}

void iMainWindowViewer::Constr_Menu_impl() {
  QString cmd_str = "Ctrl+";

  // forward/back guys -- note: on Win the icons don't show up if Action has text
  historyBackAction = AddAction(new taiAction("Back", QKeySequence(), "historyBackAction" ));
  historyBackAction->setToolTip("Back");
  historyBackAction->setStatusTip(historyBackAction->toolTip());
  connect(historyBackAction, SIGNAL(triggered()), brow_hist, SLOT(back()) );
  connect(brow_hist, SIGNAL(back_enabled(bool)), 
    historyBackAction, SLOT(setEnabled(bool)) );
  historyForwardAction = AddAction(new taiAction("Forward", QKeySequence(), "historyForwardAction" ));
  historyForwardAction->setToolTip("Forward");
  historyForwardAction->setStatusTip(historyForwardAction->toolTip());
  connect(historyForwardAction, SIGNAL(triggered()), brow_hist, SLOT(forward()) );
  connect(brow_hist, SIGNAL(forward_enabled(bool)), 
    historyForwardAction, SLOT(setEnabled(bool)) );
  connect(this, SIGNAL(SelectableHostNotifySignal(ISelectableHost*, int)),
    brow_hist, SLOT(SelectableHostNotifying(ISelectableHost*, int)) );
  connect(brow_hist, SIGNAL(select_item(taiDataLink*)),
    this, SLOT(slot_AssertBrowserItem(taiDataLink*)) );
  // no history, just manually disable
  historyBackAction->setEnabled(false);
  historyForwardAction->setEnabled(false);
  
  // project actions -- we always create them (for menu consistency) but selectively enable
  fileNewAction = AddAction(new taiAction("&New Project...", QKeySequence(), _fileNewAction ));
  fileNewAction->setIcon( QIcon( ":/images/filenew.png") );
  fileOpenAction = AddAction(new taiAction("&Open Project...", QKeySequence(), _fileOpenAction ));
  fileOpenAction->setIcon( QIcon( QPixmap(":/images/fileopen.png") ) );
  
  fileSaveAction = AddAction(new taiAction("&Save Project", QKeySequence(cmd_str+"S"), _fileSaveAction ));
  fileSaveAction->setIcon( QIcon( QPixmap(":/images/filesave.png") ) );
  fileSaveAsAction = AddAction(new taiAction("Save Project &As...", QKeySequence(), _fileSaveAsAction ));
  fileSaveNotesAction = AddAction(new taiAction("Save Note &Changes", QKeySequence(), "fileSaveNotesAction"));
  fileUpdateChangeLogAction = AddAction(new taiAction("&Updt Change Log", QKeySequence(), "fileUpdateChangeLogAction"));
  fileSaveAllAction = AddAction(new taiAction("Save A&ll Projects", QKeySequence(), _fileSaveAsAction ));
  fileCloseAction = AddAction(new taiAction("Close Project", QKeySequence(), "fileCloseAction" ));
  
  fileNewAction->AddTo(fileMenu);
  fileOpenAction->AddTo(fileMenu );
  fileOpenRecentMenu = fileMenu->AddSubMenu("Open &Recent");
  connect(fileOpenRecentMenu->menu(), SIGNAL(aboutToShow() ), 
    this, SLOT( fileOpenRecent_aboutToShow() ) );
  fileSaveAction->AddTo(fileMenu );
  fileSaveAsAction->AddTo(fileMenu);
  fileMenu->insertSeparator();
  fileSaveNotesAction->AddTo(fileMenu );
  fileUpdateChangeLogAction->AddTo(fileMenu );
  fileSaveAllAction->AddTo(fileMenu);
  fileMenu->insertSeparator();
  fileCloseAction->AddTo(fileMenu);
  
  // other actions
  fileOptionsAction = AddAction(new taiAction("&Options...", QKeySequence(), "fileOptionsAction" ));
  
//   filePrintAction = AddAction(new taiAction("&Print...", QKeySequence(), _filePrintAction ));
//   filePrintAction->setIcon( QIcon( QPixmap(":/images/fileprint.png") ) );
  if (!isRoot()) {
    fileCloseWindowAction = AddAction(new taiAction("C&lose Window", QKeySequence(), _fileCloseWindowAction ));
    connect(fileCloseWindowAction, SIGNAL(Action()), this, SLOT(fileCloseWindow()) );
  }
  connect( fileNewAction, SIGNAL( Action() ), this, SLOT( fileNew() ) );
  connect( fileOpenAction, SIGNAL( Action() ), this, SLOT( fileOpen() ) );
  connect( fileSaveAllAction, SIGNAL( Action() ), this, SLOT( fileSaveAll() ) );
  if (isProjShower()) {
    connect( fileSaveAction, SIGNAL( Action() ), this, SLOT( fileSave() ) );
    connect( fileSaveAsAction, SIGNAL( Action() ), this, SLOT( fileSaveAs() ) );
    connect( fileSaveNotesAction, SIGNAL( Action() ), this, SLOT( fileSaveNotes() ) );
    connect( fileUpdateChangeLogAction, SIGNAL( Action() ), this, SLOT( fileUpdateChangeLog() ) );
    connect( fileCloseAction, SIGNAL( Action() ), this, SLOT( fileClose() ) );
    // disable the CloseWindow to help emphasize that Closing will close proj
//no, not needed    fileCloseWindowAction->setEnabled(false);
  } else {
    fileSaveAction->setEnabled(false);
    fileSaveAsAction->setEnabled(false);
    fileSaveNotesAction->setEnabled(false);
    fileUpdateChangeLogAction->setEnabled(false);
    fileCloseAction->setEnabled(false);
  }
#ifndef TA_OS_MAC
  if (isRoot())
#endif
  {
    fileQuitAction = AddAction(new taiAction("&Quit", QKeySequence(cmd_str+"Q"),
      _fileQuitAction ));
    connect(fileQuitAction, SIGNAL(Action()), this, SLOT(fileQuit()) );
  }
  
  editUndoAction = AddAction(new taiAction("&Undo", QKeySequence(cmd_str+"Z"), _editUndoAction ));
  editUndoAction->setIcon( QIcon( QPixmap(":/images/editundo.png") ) );
  editRedoAction = AddAction(new taiAction("&Redo", QKeySequence(cmd_str+"Shift+Z"), _editRedoAction ));
  editRedoAction->setIcon( QIcon( QPixmap(":/images/editredo.png") ) );
  editCutAction = AddAction(new taiAction(taiClipData::EA_CUT, "Cu&t", QKeySequence(cmd_str+"X"), _editCutAction ));
  editCutAction->setIcon( QIcon( QPixmap(":/images/editcut.png") ) );
  editCopyAction = AddAction(new taiAction(taiClipData::EA_COPY, "&Copy", QKeySequence(cmd_str+"C"), _editCopyAction ));
  editCopyAction->setIcon( QIcon( QPixmap(":/images/editcopy.png") ) );
  //note: we twiddle the visibility, shortcuts, and accelerator for the Paste and Link guys
  editDupeAction = AddAction(new taiAction(taiClipData::EA_DUPE, "Duplicate  (Ctrl+M)", QKeySequence(), "editDuplicateAction" ));
  editPasteAction = AddAction(new taiAction(taiClipData::EA_PASTE, "&Paste", QKeySequence(cmd_str+"V"), _editPasteAction ));
  QPixmap editpaste(":/images/editpaste.png");
  editPasteAction->setIcon( QIcon( editpaste ) );
  editPasteIntoAction = AddAction(new taiAction(taiClipData::EA_PASTE_INTO, "&Paste Into", QKeySequence(cmd_str+"V"), "editPasteIntoAction" ));
  editPasteIntoAction->setIcon( QIcon( editpaste ) );
  editPasteAssignAction = AddAction(new taiAction(taiClipData::EA_PASTE_ASSIGN, "&Paste Assign", QKeySequence(cmd_str+"V"), "editPasteAssignAction" ));
  editPasteAssignAction->setIcon( QIcon( editpaste ) );
  editPasteAppendAction = AddAction(new taiAction(taiClipData::EA_PASTE_APPEND, "&Paste Append", QKeySequence(cmd_str+"V"), "editPasteAppendAction" ));
  editPasteAppendAction->setIcon( QIcon( editpaste ) );
  editDeleteAction = AddAction(new taiAction(taiClipData::EA_DELETE, "&Delete", QKeySequence("Shift+D"), _editDeleteAction ));
//  editDeleteAction->setIcon( QIcon( editpaste ) );
  editLinkAction = AddAction(new taiAction(taiClipData::EA_LINK, "&Link", QKeySequence(), _editLinkAction ));
  editLinkIntoAction = AddAction(new taiAction(taiClipData::EA_LINK, "&Link Into", QKeySequence(), "editLinkIntoAction" ));
  editUnlinkAction = AddAction(new taiAction(taiClipData::EA_LINK, "Unlin&k", QKeySequence(), "editUnlinkAction" ));
  editFindAction = AddAction(new taiAction(0, "&Find...", QKeySequence(), "editFindAction"));
  editFindNextAction = AddAction(new taiAction(0, "Find &Next", QKeySequence("F3"), "editFindNextAction"));
  viewRefreshAction = AddAction(new taiAction("&Refresh", QKeySequence("F5"), _viewRefreshAction ));
  viewSaveViewAction = AddAction(new taiAction("&Save View", QKeySequence(), "viewSaveViewAction" ));
  toolsClassBrowseAction = AddAction(new taiAction(0, "Class Browser", QKeySequence(), "toolsClassBrowseAction"));
  toolsTypeBrowseAction = AddAction(new taiAction(0, "&Help Browser", QKeySequence(), "toolsTypeBrowseAction"));
  String s = taMisc::app_name + " Help on the web";
  helpHelpAction = AddAction(new taiAction("&Help", QKeySequence(), _helpHelpAction ));
  helpHelpAction->setToolTip(s);
  helpHelpAction->setStatusTip(s);
  helpAboutAction = AddAction(new taiAction("&About", QKeySequence(), _helpAboutAction ));

//   fileExportMenu = fileMenu->AddSubMenu("Export"); // submenu -- empty and disabled in base
  fileOptionsAction->AddTo( fileMenu );
//   filePrintAction->AddTo( fileMenu );
  editUndoAction->AddTo( editMenu ); 
  editRedoAction->AddTo( editMenu );
  editMenu->insertSeparator();
  editCutAction->AddTo( editMenu );
  editCopyAction->AddTo( editMenu );
  editDupeAction->AddTo( editMenu );
  editPasteAction->AddTo( editMenu );
  editPasteIntoAction->AddTo( editMenu );
  editPasteAssignAction->AddTo( editMenu );
  editPasteAppendAction->AddTo( editMenu );
  editLinkAction->AddTo( editMenu );
  editLinkIntoAction->AddTo( editMenu );
  editUnlinkAction->AddTo( editMenu );
  editDeleteAction->AddTo( editMenu );
  editMenu->insertSeparator();
  editFindAction->AddTo( editMenu );
  editFindNextAction->AddTo( editMenu );
  
  viewRefreshAction->AddTo(viewMenu);
  viewMenu->insertSeparator();
  historyBackAction->AddTo(viewMenu);
  historyForwardAction->AddTo(viewMenu);
  viewMenu->insertSeparator();
  frameMenu = viewMenu->AddSubMenu("Frames");
  toolBarMenu = viewMenu->AddSubMenu("Toolbars");
  dockMenu = viewMenu->AddSubMenu("Dock Windows");
  viewMenu->insertSeparator();
  viewSaveViewAction->AddTo(viewMenu);
  
  // next two items are commands that set the other toggle flags
  show_menu->AddItem("Normal &only", taiMenu::normal, taiAction::men_act,
      this, SLOT(ShowChange(taiAction*)), 0 );
  show_menu->AddItem("&All", taiMenu::normal, taiAction::men_act,
      this, SLOT(ShowChange(taiAction*)), 1 );
  show_menu->AddSep();
  //  toggle flags
  show_menu->AddItem("&Normal", taiMenu::toggle, taiAction::men_act,
      this, SLOT(ShowChange(taiAction*)), 2 );
  show_menu->AddItem("E&xpert", taiMenu::toggle, taiAction::men_act,
      this, SLOT(ShowChange(taiAction*)), 3 );
  show_menu->AddItem("&Hidden", taiMenu::toggle, taiAction::men_act,
      this, SLOT(ShowChange(taiAction*)), 4 );
  //note: correct toggles set dynamically when user drops down menu
 

  toolsClassBrowseAction->AddTo(toolsMenu );
  toolsTypeBrowseAction->AddTo(toolsMenu );
  helpHelpAction->AddTo(helpMenu );
  helpMenu->insertSeparator();
  helpAboutAction->AddTo(helpMenu );

    // signals and slots connections
  connect( fileOptionsAction, SIGNAL( Action() ), this, SLOT( fileOptions() ) );
//   connect( filePrintAction, SIGNAL( activated() ), this, SLOT( filePrint() ) );
  connect( editUndoAction, SIGNAL( Action() ), this, SLOT( editUndo() ) );
  connect( editRedoAction, SIGNAL( Action() ), this, SLOT( editRedo() ) );
  connect( editCutAction, SIGNAL( IntParamAction(int) ), this, SIGNAL(EditAction(int)) );
  connect( editCopyAction, SIGNAL( IntParamAction(int) ), this, SIGNAL(EditAction(int)) );
  connect( editDupeAction, SIGNAL( IntParamAction(int) ), this, SIGNAL(EditAction(int)) );
  connect( editPasteAction, SIGNAL( IntParamAction(int) ), this, SIGNAL(EditAction(int)) );
  connect( editPasteIntoAction, SIGNAL( IntParamAction(int) ), this, SIGNAL(EditAction(int)) );
  connect( editPasteAssignAction, SIGNAL( IntParamAction(int) ), this, SIGNAL(EditAction(int)) );
  connect( editPasteAppendAction, SIGNAL( IntParamAction(int) ), this, SIGNAL(EditAction(int)) );
  connect( editLinkAction, SIGNAL( IntParamAction(int) ), this, SIGNAL(EditAction(int)) );
  connect( editLinkIntoAction, SIGNAL( IntParamAction(int) ), this, SIGNAL(EditAction(int)) );
  connect( editUnlinkAction, SIGNAL( IntParamAction(int) ), this, SIGNAL(EditAction(int)) );
  connect( editDeleteAction, SIGNAL( IntParamAction(int) ), this, SIGNAL(EditAction(int)) );
  connect( editFindAction, SIGNAL( Action() ), this, SLOT(editFind()) );
  connect( editFindNextAction, SIGNAL( Action() ), this, SLOT(editFindNext()) );
  connect( viewRefreshAction, SIGNAL( Action() ), this, SLOT(viewRefresh()) );
  connect( viewSaveViewAction, SIGNAL( Action() ), this, SLOT(viewSaveView()) );
  connect( toolsClassBrowseAction, SIGNAL( triggered() ), 
    this, SLOT( toolsClassBrowser() ) );
  connect( toolsTypeBrowseAction, SIGNAL( triggered() ), 
    this, SLOT( toolsTypeBrowser() ) );
  connect( helpHelpAction, SIGNAL(Action()), this, SLOT(helpHelp()) );
  connect( helpAboutAction, SIGNAL(Action()), this, SLOT(helpAbout()) );
}

taProject* iMainWindowViewer::curProject() const {
  taProject* rval = NULL;
  if (isProjShower()) {
    MainWindowViewer* db = viewer();
    if (!db) return NULL; // shouldn't happen
    rval = dynamic_cast<taProject*>(db->data());
  }
  return rval;
}

taProject* iMainWindowViewer::myProject() const {
  MainWindowViewer* db = viewer();
  if (!db) return NULL; // shouldn't happen
  // first, check if a project browser, if so, then we have our puppy
  taBase* data = db->data();
  if (!data) return NULL;
  taProject* rval = dynamic_cast<taProject*>(data);
  // otherwise, will need to get its parent
  if (!rval) {
    rval = (taProject*)data->GetOwner(&TA_taProject);
  }
  return rval;
}

void iMainWindowViewer::emit_EditAction(int param) {
  emit EditAction(param);
  taiMisc::RunPending();
  UpdateUi();
}

/*obs void iMainWindowViewer::emit_selectionChanged()
{
  emit selectionChanged();
} */

void iMainWindowViewer::Find(taiDataLink* root, const String& find_str) {
  // if an instance doesn't exist, need to make one; we tie it to ourself 
  if (!search_dialog) {
    search_dialog = iSearchDialog::New(0, this);
  }
  iSearchDialog* dlg = search_dialog;
  
  dlg->setRoot(root);
  dlg->show();
  dlg->raise();
  dlg->activateWindow();
  if(find_str.nonempty())
    dlg->setSearchStr(find_str);
}

void iMainWindowViewer::editUndo() {
  taProject* proj = curProject();
  if (!proj) return;
  proj->undo_mgr.Undo();
}

void iMainWindowViewer::editRedo() {
  taProject* proj = curProject();
  if (!proj) return;
  proj->undo_mgr.Redo();
}

void iMainWindowViewer::editFind() {
  // assume root of this window's browser
  taiDataLink* root = NULL;
  
  MainWindowViewer* db = viewer();
  BrowseViewer* bv = NULL;
  int idx;
  if (db) {
    bv = (BrowseViewer*)db->FindFrameByType(&TA_BrowseViewer, idx);
    if (bv) root = bv->rootLink();
  }
  if (!root) root = (taiDataLink*)tabMisc::root->GetDataLink();
  Find(root);
}

void iMainWindowViewer::editFindNext() {
  // if we don't have a find window, just do Find 
  if (search_dialog) {
    search_dialog->FindNext();
  } else {
    editFind();
  }
}

void iMainWindowViewer::fileCloseWindow() {
  close();
}

// note: all the fileXxx for projects are safe -- they don't get enabled
//  unless they are applicable (ex Save only if it is a viewer)
void iMainWindowViewer::fileNew() {
  if (!tabMisc::root) return;
  tabMisc::root->projects.CallFun("New_gui"); // let user choose type
  //should automatically open
}

void iMainWindowViewer::fileOpen() {
  if (!tabMisc::root) return;
  TypeDef* td = &TA_taProject;
// get filename ourself, so we can check if it is already open!
  taFiler* flr = tabMisc::root->projects.GetFiler(td); 
  taRefN::Ref(flr);
//TODO: context path  flr->setFileName(fname);
  if (flr->GetFileName(taFiler::foOpen)) {
    String fname = flr->FileName();
    fileOpenFile(fname);
  }  
  taRefN::unRefDone(flr); 
}

void iMainWindowViewer::fileOpenRecent_aboutToShow() {
  // delete previous
  fileOpenRecentMenu->Reset();
  // populate with current recent
  for (int i = 0; i < tabMisc::root->recent_files.size; ++i) {
    String file = tabMisc::root->recent_files[i];
    //taiAction* item =
    fileOpenRecentMenu->AddItem(file, taiAction::var_act, this, SLOT(fileOpenFile(const Variant&)),
      file);
  }
}

void iMainWindowViewer::fileOpenFile(const Variant& fname_) {
  String fname = fname_.toString();
  // check if already open
  taProject* proj = NULL;
  // canonicalize name, for comparison to open projects
  QFileInfo fi(fname);
  fname = fi.canonicalFilePath();
  for (int i = 0; i < tabMisc::root->projects.size; ++i) {
    proj = tabMisc::root->projects.FastEl(i);
    if (proj->file_name == fname) {
      int chs = taMisc::Choice("That project is already open -- it will be viewed instead",
			       "Ok", "Cancel");
      switch (chs) {
      case 0: break; // break out of switch -- we'll also break out of the loop
      case 1: return;
      }
      break; // break out of loop
    }
    proj = NULL; // in case we fall out of loop
  }
  // if proj has a value, then we should view the existing, else open it
  bool clear_dirty = true; // only for new loads, or old views when not dirty already
  if (proj) {
    if (proj->isDirty()) clear_dirty = false; // don't clear if still dirty
    // this will automatically view it if already open
    proj->AssertDefaultProjectBrowser(true);
  } else {
    taBase* el = NULL;
    tabMisc::root->projects.Load(fname, &el);
    proj = (taProject*)el;
    //note: gets autoviewed in its postload routine
  }
  tabMisc::root->AddRecentFile(fname);
  //note: the viewing will have set us dirty again, after load cleared it
  if (proj && clear_dirty)
    proj->setDirty(false);
}

void iMainWindowViewer::fileQuit() {
  // note: following does a save of prefs
  taiMiscCore::Quit(CO_PROCEED); // not forced, until main closes
}

void iMainWindowViewer::fileSave() {
  taProject* proj = curProject();
  if (!proj) return;
  proj->Save();
}

void iMainWindowViewer::fileSaveAs() {
  taProject* proj = curProject();
  if (!proj) return;
  proj->SaveAs();
}

void iMainWindowViewer::fileSaveNotes() {
  taProject* proj = curProject();
  if (!proj) return;
  proj->SaveNoteChanges();
}

void iMainWindowViewer::fileUpdateChangeLog() {
  taProject* proj = curProject();
  if (!proj) return;
  proj->UpdateChangeLog();
}

void iMainWindowViewer::fileSaveAll() {
  if (!tabMisc::root) return;
  tabMisc::root->SaveAll();
}

void iMainWindowViewer::fileClose() {
  taProject* proj = curProject();
  if (!proj) return;
  // Check for dirty/save
  if (proj->isDirty()) {
    int chs= taMisc::Choice("The project has unsaved changes -- do you want to save before closing it?",
        "&Save", "&Don't Save", "&Cancel");
  
    switch (chs) {
    case 0:
      SaveData(); 
      break;
    case 1:
      break;
    case 2:
      return;
    }
  }
  proj->CloseLater();
}

void iMainWindowViewer::fileOptions() {
  if (!tabMisc::root) return;
  tabMisc::root->Options();
}

iTreeView* iMainWindowViewer::GetMainTreeView() {
  MainWindowViewer* db = viewer();
  if (!db) return NULL;
  int idx;
  BrowseViewer* bv  = (BrowseViewer*)db->FindFrameByType(&TA_BrowseViewer, idx);
  if (!bv) return NULL;
  iBrowseViewer* ibv = bv->widget();
  if (!ibv) return NULL;
  return ibv->lvwDataTree;
}

iTreeView* iMainWindowViewer::GetCurTreeView() {
  if(cur_tree_view) return cur_tree_view;
  return GetMainTreeView();
}

void iMainWindowViewer::FocusCurTreeView() {
  iTreeView* itv = GetCurTreeView();
  if(itv) itv->setFocus();
}

iTreeViewItem* iMainWindowViewer::AssertBrowserItem(taiDataLink* link) {
  iTreeView* itv = GetCurTreeView(); // note: use current
  if(!itv) return NULL;
  // note: waitproc is now insulated against recurrent calls..
  // make sure previous operations are finished
  taiMiscCore::ProcessEvents();
  iTreeViewItem* rval = itv->AssertItem(link);
  if (rval) {
    itv->scrollTo(rval);
    itv->setCurrentItem(rval);
  }
  else if(itv == cur_tree_view) { // try again with main 
    itv = GetMainTreeView();
    rval = itv->AssertItem(link);
    if (rval) {
      itv->scrollTo(rval);
      itv->setCurrentItem(rval);
    }
  }
  // make sure our operations are finished
  taiMiscCore::ProcessEvents();
  return rval;
}

iTreeViewItem* iMainWindowViewer::BrowserExpandAllItem(taiDataLink* link) {
  iTreeView* itv = GetCurTreeView(); // note: use current
  if(!itv) return NULL;
  // note: waitproc is now insulated against recurrent calls..
  // make sure previous operations are finished
  taiMiscCore::ProcessEvents();
  iTreeViewItem* rval = itv->AssertItem(link);
  if (rval) {
    itv->ExpandAllUnder(rval);
  }
  else if(itv == cur_tree_view) { // try again with main 
    itv = GetMainTreeView();
    rval = itv->AssertItem(link);
    if (rval) {
      itv->CollapseAllUnder(rval);
    }
  }
  // make sure our operations are finished
  taiMiscCore::ProcessEvents();
  return rval;
}

iTreeViewItem* iMainWindowViewer::BrowserCollapseAllItem(taiDataLink* link) {
  iTreeView* itv = GetCurTreeView(); // note: use current
  if(!itv) return NULL;
  // make sure previous operations are finished
  taiMiscCore::ProcessEvents();
  iTreeViewItem* rval = itv->AssertItem(link);
  if (rval) {
    itv->CollapseAllUnder(rval);
  }
  else if(itv == cur_tree_view) { // try again with main 
    itv = GetMainTreeView();
    rval = itv->AssertItem(link);
    if (rval) {
      itv->CollapseAllUnder(rval);
    }
  }
  // make sure our operations are finished
  taiMiscCore::ProcessEvents();
  return rval;
}

bool iMainWindowViewer::AssertPanel(taiDataLink* link,
  bool new_tab, bool new_tab_lock)
{
  if (!new_tab) new_tab_lock = false;
  iTabViewer* itv = GetTabViewer(); // should exist
  if (!itv) return false;
  
  iDataPanel* pan = itv->tabView()->GetDataPanel(link);
  if (!pan) return false; // shouldn't happen
  
  if (new_tab) {
    itv->AddPanelNewTab(pan, new_tab_lock);
  } else {
    itv->AddPanel(pan);
  }
  // make sure previous operations are finished
  taiMiscCore::ProcessEvents();
  return true;
}

// this is only for internally-generated ta:// links

void iMainWindowViewer::taUrlHandler(const QUrl& url) {
// URLs are usually suffixed with a "#Xxx" where Xxx is the uniqueId()
// of the window in which is embedded the doc viewer 
// if no WinId (ex. Find) then we use that of the topmost window

  //NOTE: URLs only open in the main project browser for that project
  String path = url.path(); // will only be part before #, if any
  
  // TypeBrowser invocations are dependency-free so we check them first
  if(path.startsWith(".Type.")) {
    iHelpBrowser::StatLoadUrl(String(url.toString()));
    return; 
  }
  
  // we usually embed the uniqueId of the win that had the invoking url
  String win_id_str = String(url.fragment()).after("#");
  int win_id = win_id_str.toInt(); // 0 if empty or not an int
  if (win_id == 0) {
    iMainWindowViewer* top_win = taiMisc::active_wins.Peek_MainWindow();
    if (top_win)
      win_id = top_win->uniqueId();
  }
  
  iMainWindowViewer* idoc_win = NULL; // win from which url was invoked
  if (win_id != 0) { // problem if not found!!!
    idoc_win = taiMisc::active_wins.FindMainWindowById(win_id);
  }
//NOTE: if idoc_win is NULL, then the only really valid thing after this
// is a WIKI or Web url...

  // get the project -- should be able to get from any viewer/browser
  taProject* proj = NULL;
  if (idoc_win) {
    proj = idoc_win->myProject();
  }
  
  // for uniformity and simplicity, we look up the canonical windows
  // and corresponding viewers for the tree/panels and panels/t3 frames
  // note that these are the same for 2-pane
  MainWindowViewer* proj_brow = NULL;
  MainWindowViewer* proj_view = NULL;
  if (proj) {
    proj_brow = proj->GetDefaultProjectBrowser();
    proj_view = proj->GetDefaultProjectViewer();
  }
  iMainWindowViewer* iproj_brow = NULL;
  if (proj_brow)
    iproj_brow = proj_brow->widget();
/*nn  iMainWindowViewer* iproj_view = NULL;
  if (proj_view)
    iproj_view = proj_view->widget(); */

//IMPORTANT NOTE: You *must* check ALL objects for NULL in the following
// cascades, because there are conditions under which it is possible for
// something not to have a value
  if(path.startsWith(".T3Tab.")) {
    String tbnm = path.after(".T3Tab.");
    if(!proj_view || !proj_view->SelectT3ViewTabName(tbnm)) {
      taMisc::Warning("ta: URL -- could not activate 3D View Tab named:", tbnm);
    }      
  }
  else if(path.startsWith(".PanelTab.")) {
    String tbnm = path.after(".PanelTab.");
    if(!proj_brow || !proj_brow->SelectPanelTabName(tbnm)) {
      taMisc::Warning("ta: URL -- could not activate Panel Tab named:", tbnm);
    }
  }
  else if(path.startsWith(".Wiki.")) {
    String new_path = "http://grey.colorado.edu/emergent/index.php/" + path.after(".Wiki.");
    QUrl new_url(new_path);
    QDesktopServices::openUrl(new_url);
  }
  else {
    String fun_call;
    if(path.endsWith("()")) {	// function call!
      fun_call = path.after(".",-1);
      fun_call = fun_call.before("()");
      path = path.before(".",-1);	// get part before last call
    }
    taBase* tab = NULL;
    MemberDef* md;
    if(path.startsWith(".projects")) {
      tab = tabMisc::root->FindFromPath(path, md);
    }
    else {
      if(path.startsWith("."))
	path = path.after(".");
      if (proj)
        tab = proj->FindFromPath(path, md);
    }
    if (!tab) {
      taMisc::Warning("ta: URL",path,"not found as a path to an object!");
      return;
    }
    if(fun_call.nonempty()) {
      tab->CallFun(fun_call);
    }
    else {
      taiDataLink* link = (taiDataLink*)tab->GetDataLink();
      if (!link || !iproj_brow) {
        taMisc::Warning("ta: URL",path,"not found as a path to an object!");
        return;
      }
      //  iTreeViewItem* item = 
      iproj_brow->AssertBrowserItem(link);
    }
  }
}

void iMainWindowViewer::httpUrlHandler(const QUrl& url) {
  // always just use the help browser for any misc links
  iHelpBrowser::StatLoadUrl(url.toString());
//   if(isProjShower()) {
//     taProject* prj = curProject();
//     if(prj) {
//       taDoc* browser = prj->FindMakeDoc("misc_browser", "", url.toString());
//       browser->EditDialog();
//       return;
//     }
//   }
//   else if(isRoot()) {
//     MainWindowViewer* db = viewer();
//     if(db) {
//       taRootBase* rt = dynamic_cast<taRootBase*>(db->data());
//       if(rt) {
// 	taDoc* browser = rt->FindMakeDoc("misc_browser", "", url.toString());
// 	browser->EditDialog();
// 	return;
//       }
//     }
//   }
//   QDesktopServices::openUrl(url);	// fall back on default
}

bool iMainWindowViewer::event(QEvent* ev) {
  bool rval = inherited::event(ev);
  if (ev->type() == QEvent::WindowActivate)
    taiMisc::active_wins.GotFocus_MainWindow(this);
  return rval;
}

int iMainWindowViewer::GetEditActions() {
  int rval = 0;
  emit GetEditActionsEnabled(rval);
  return rval;
}

void iMainWindowViewer::viewSaveView() {
  viewer()->GetWinState();
}

void iMainWindowViewer::windowMenu_aboutToShow() {
  // delete previous
  windowMenu->Reset();
  // populate with current windows
  Widget_List wl;
  taiMisc::GetWindowList(wl);
  for (int i = 0; i < wl.size; ++i) {
    QWidget* wid = wl.FastEl(i);
    String title;
    // for up to first 9, put numeric accelerator
    if (i < 9) {
      title = "&" + String(i + 1) + " ";
    }
    if (wid->isWindow())
      title.cat((String)wid->windowTitle());
/*??    else 
      title.cat((String)wid->text());*/
    //taiAction* item =
    windowMenu->AddItem(title, taiAction::int_act, 
     this, SLOT(windowActivate(int)), i);
  }
}

void iMainWindowViewer::windowActivate(int win) {
  // populate with current windows -- should correspond to the ones enumerated
  Widget_List wl;
  taiMisc::GetWindowList(wl);
  QWidget* wid = wl.PosSafeEl(win);
  if (!wid) return;
  wid->activateWindow();
  wid->raise();
}

void iMainWindowViewer::filePrint() {
}

void iMainWindowViewer::helpHelp() {
  String url = taMisc::web_help_general;
  if (url.nonempty()) {
    QDesktopServices::openUrl(QUrl(url));
  }
}

void iMainWindowViewer::helpAbout() {
  if (tabMisc::root) tabMisc::root->About();
}

void iMainWindowViewer::mnuEditAction(taiAction* mel) {
   // called from context; cast obj to an taiClipData::EditAction
  emit_EditAction(mel->usr_data.toInt());
}

void iMainWindowViewer::SelectableHostNotifying_impl(ISelectableHost* src_host, int op) {
  //TODO: should we do anything else with this ourself????
  emit SelectableHostNotifySignal(src_host, op);
  UpdateUi();
}

void iMainWindowViewer::Refresh_impl() {
  QObject* obj;
  foreach (obj, body->children()) {
    IDataViewWidget* dvw = dynamic_cast<IDataViewWidget*>(obj); // null if not type
    if (dvw) {
      dvw->Refresh();
    } 
  }
}

void iMainWindowViewer::ResolveChanges_impl(CancelOp& cancel_op) {
  if (!isProjShower()) return; // changes only applied for proj showers

  taProject* proj = curProject();
  // only closing last browser is important (easier to check that here than below)
  if(!proj) return;		// does happen!
  MainWindowViewer* mwv = viewer();
  if (!mwv) return; // shouldn't happen
  if (!mwv->isProjBrowser()) return;
  // ok, if this isn't last, then bail
  int cnt = 0; // just bail if 2+
  for (int i = 0; i < proj->viewers.size; i++) {
    mwv = dynamic_cast<MainWindowViewer*>(proj->viewers.FastEl(i));
    if (!mwv || !(mwv->isProjBrowser() && mwv->isMapped())) continue;
    if (++cnt >= 2) return;
  }
  // ok, so here we are at the final project viewer, which will close the project!
  if (isDirty() && !(proj && proj->m_no_save)) {
    bool forced = (cancel_op == CO_NOT_CANCELLABLE);
    int chs;
    if (forced)
      chs= taMisc::Choice("The project has unsaved changes -- do you want to save before closing this window?",
        "&Save", "&Don't Save");
    else 
      chs= taMisc::Choice("The project has unsaved changes -- do you want to save before closing this window?",
        "&Save", "&Don't Save", "&Cancel");

    switch (chs) {
    case 0:
      SaveData(); 
      break;
    case 1:
      if (proj)
        proj->m_no_save = true; // save so we don't prompt again
      break;
    case 2: //only possible if not forced
      cancel_op = CO_CANCEL;
      return;
    }
  }
  // if we make it here,  then we should be closed -- closeEvent will do this
  m_close_proj_now = true;
}

bool iMainWindowViewer::isDirty() const {
  taProject* proj = curProject(); // only if projviwer
  if (proj) return proj->isDirty();
  else return false;
}


void iMainWindowViewer::SaveData() {
  taProject* proj = curProject(); // only if projviwer
   // only impl for projects, because they are only thing we know how to save
  if (proj) 
    proj->Save();
}

void iMainWindowViewer::SelectableHostNotifySlot(ISelectableHost* src_host, int op) {
  switch (op) {
  case ISelectableHost::OP_GOT_FOCUS: {
    // spec says that this automatically also makes the guy the cliphandler
    if (last_sel_server == src_host) break; // nothing to do
    last_sel_server = src_host;
    QObject* handler_obj = src_host->clipHandlerObj();
    SetClipboardHandler(handler_obj, ISelectableHost::edit_enabled_slot,
      ISelectableHost::edit_action_slot);
    SelectableHostNotifying_impl(src_host, op); // break out for clarity
    } break;
  case ISelectableHost::OP_SELECTION_CHANGED: {
    // if no handler, then this guy becomes handler implicitly, otherwise ignore
    if (last_sel_server && (last_sel_server != src_host)) break;
    SelectableHostNotifying_impl(src_host, op); // break out for clarity
    } break;
  case ISelectableHost::OP_DESTROYING: {
    if (last_sel_server == src_host) {
      if (last_clip_handler == src_host->clipHandlerObj()) {
        SetClipboardHandler(NULL); // might as well do this now
      }
      last_sel_server = src_host;
      UpdateUi();
    }
    } break;
  default: break; // shouldn't happen
  }
}

void iMainWindowViewer::SetClipboardHandler(QObject* handler_obj,
  const char* edit_enabled_slot,
  const char* edit_action_slot,
  const char* actions_enabled_slot,
  const char* update_ui_signal)
{
  if (last_clip_handler == handler_obj) return; // nothing to do
  // always disconnect first
  if (last_clip_handler) {
    disconnect(this, SIGNAL(GetEditActionsEnabled(int&)), last_clip_handler, NULL);
    disconnect(this, SIGNAL(EditAction(int)), last_clip_handler, NULL);
    disconnect(this, SIGNAL(SetActionsEnabled()), last_clip_handler, NULL);
    disconnect(last_clip_handler, NULL, this, SLOT(UpdateUi()));
    disconnect(last_clip_handler, SIGNAL(destroyed()), this, SLOT(ch_destroyed()));
  }
  // now connect, if supplied
  if (handler_obj) {
    connect(handler_obj, SIGNAL(destroyed()), this, SLOT(ch_destroyed()));
    connect(this, SIGNAL(GetEditActionsEnabled(int&)), handler_obj, edit_enabled_slot);
    connect(this, SIGNAL(EditAction(int)), handler_obj, edit_action_slot );
    if (actions_enabled_slot)
      connect(this, SIGNAL(SetActionsEnabled()), handler_obj, actions_enabled_slot );
    if (update_ui_signal)
      connect(handler_obj, update_ui_signal, this, SLOT(UpdateUi()) );
/*//TEMP
    taMisc::Warning("SetClipHandler to: type, name", handler_obj->metaObject()->className(),
      handler_obj->objectName());
} else {
    taMisc::Warning("SetClipHandler cleared");
// /TEMP */
  }
  last_clip_handler = handler_obj; // whether NULL or not
  UpdateUi();
}

void iMainWindowViewer::setFrameGeometry(const iRect& r) {
  //NOTE: this may only work before calling show() on X
    this->resize(r.size());   // set size
    this->move(r.topLeft());  // set position
}

void  iMainWindowViewer::setFrameGeometry(int left, int top, int width, int height) {
  setFrameGeometry(iRect(left, top, width, height));
}

void iMainWindowViewer::ShowChange(taiAction* sender) {
  int show = taMisc::show_gui;
  int new_show;
  if (sender->usr_data == 0)
    new_show = taMisc::NORM_MEMBS;
  else if (sender->usr_data == 1)
    new_show = taMisc::ALL_MEMBS;
  else {
    int mask;
    switch (sender->usr_data.toInt()) {
      case 2: mask = taMisc::NO_NORMAL; break;
      case 3: mask = taMisc::NO_EXPERT; break;
      case 4: mask = taMisc::NO_HIDDEN; break;
      default: mask = 0; break; // should never happen
    }
    new_show = sender->isChecked() ? show & ~mask : show | mask;
  }
  if (new_show != taMisc::show_gui) {
    taMisc::show_gui = (taMisc::ShowMembs)new_show;
    viewRefresh();
  }
}

void iMainWindowViewer::showMenu_aboutToShow() {
  int value = taMisc::show_gui;
  if (!show_menu) return;
  //note: nothing to do for the command items
  (*show_menu)[2]->setChecked(!(value & taMisc::NO_NORMAL));
  (*show_menu)[3]->setChecked(!(value & taMisc::NO_EXPERT));
  (*show_menu)[4]->setChecked(!(value & taMisc::NO_HIDDEN));
}

void iMainWindowViewer::this_DockSelect(taiAction* me) {
  iDockViewer* itb = (iDockViewer*)(me->usr_data.toPtr());
  DockViewer* tb = itb->viewer();
  if (!tb) return; // shouldn't happen
  if (me->isChecked()) { //note: check has already been toggled
    tb->Show();
  } else { //need to show
    tb->Hide();
  }
}

void iMainWindowViewer::this_FrameSelect(taiAction* me) {
  iFrameViewer* itb = (iFrameViewer*)(me->usr_data.toPtr());
  FrameViewer* tb = itb->viewer();
  if (!tb) return; // shouldn't happen
  if (me->isChecked()) { //note: check has already been toggled
    tb->Show();
  } else { //need to show
    tb->Hide();
  }
}

void iMainWindowViewer::this_ToolBarSelect(taiAction* me) {
  iToolBar* itb = (iToolBar*)(me->usr_data.toPtr());
  ToolBar* tb = itb->viewer();
  if (!tb) return; // shouldn't happen
  if (me->isChecked()) { //note: check has already been toggled
    tb->Show();
  } else { //need to show
    tb->Hide();
  }
}

void iMainWindowViewer::toolsClassBrowser() {
  MainWindowViewer* brows = MainWindowViewer::NewClassBrowser(&taMisc::types, &TA_TypeSpace);
  if (brows == NULL) return;
  brows->ViewWindow();
}

void iMainWindowViewer::toolsTypeBrowser() {
 // iHelpBrowser* tdd = 
  iHelpBrowser::instance();
}

void iMainWindowViewer::UpdateUi() {
  QString cmd_str = "Ctrl+";

  int ea = GetEditActions();
  // some actions we always show, others we only show if available
  editCutAction->setEnabled(ea & taiClipData::EA_CUT);
  editCopyAction->setEnabled(ea & taiClipData::EA_COPY);
  editDupeAction->setVisible(ea & taiClipData::EA_DUPE);
  // we always show the plainjane Paste (enable or disable)
  // if more than one paste guy is enabled, no shortcuts/accelerators
  int paste_cnt = 0;
  if (ea & taiClipData::EA_PASTE) ++paste_cnt;
  if (ea & taiClipData::EA_PASTE_INTO) ++paste_cnt;
  if (ea & taiClipData::EA_PASTE_ASSIGN) ++paste_cnt;
  if (ea & taiClipData::EA_PASTE_APPEND) ++paste_cnt;
  editPasteAction->setEnabled(ea & taiClipData::EA_PASTE);
  editPasteIntoAction->setVisible(ea & taiClipData::EA_PASTE_INTO);
  editPasteAssignAction->setVisible(ea & taiClipData::EA_PASTE_ASSIGN);
  editPasteAppendAction->setVisible(ea & taiClipData::EA_PASTE_APPEND);
  if (ea & taiClipData::EA_PASTE_INTO) {
    if (paste_cnt > 1) {
      editPasteIntoAction->setText("Paste Into");
      editPasteIntoAction->setShortcut(QKeySequence());
    } else {
      editPasteIntoAction->setText("&Paste Into");
      editPasteIntoAction->setShortcut(QKeySequence(cmd_str+"V"));
    }
  }
  if (ea & taiClipData::EA_PASTE_ASSIGN)  {
    if (paste_cnt > 1) {
      editPasteAssignAction->setText("Paste Assign");
      editPasteAssignAction->setShortcut(QKeySequence());
    } else {
      editPasteAssignAction->setText("&Paste Assign");
      editPasteAssignAction->setShortcut(QKeySequence(cmd_str+"V"));
    }
  }
  if (ea & taiClipData::EA_PASTE_APPEND)  {
    if (paste_cnt > 1) {
      editPasteAppendAction->setText("Paste Append");
      editPasteAppendAction->setShortcut(QKeySequence());
    } else {
      editPasteAppendAction->setText("&Paste Append");
      editPasteAppendAction->setShortcut(QKeySequence(cmd_str+"V"));
    }
  }
  
  editDeleteAction->setEnabled(ea & taiClipData::EA_DELETE);
  
  // linking is currently not really used, so we'll not show by default
  // if we later add more linking capability, we may want to always enable,
  // just to hint user that it is sometimes available
  editLinkAction->setVisible(ea & taiClipData::EA_LINK);
  editLinkIntoAction->setVisible(ea & taiClipData::EA_LINK_INTO);
  
  if ((ea & taiClipData::EA_LINK2) == taiClipData::EA_LINK2) {
    // need to remove accelerators
    editLinkAction->setText("Link");
    editLinkAction->setShortcut(QKeySequence());
    editLinkIntoAction->setText("Link Into");
    editLinkIntoAction->setShortcut(QKeySequence());
  } else { 
    // restore accelerators
    editLinkAction->setText("&Link");
    editLinkAction->setShortcut(QKeySequence());
    editLinkIntoAction->setText("&Link Into");
    editLinkIntoAction->setShortcut(QKeySequence());
  }
  
  editUnlinkAction->setVisible(ea & taiClipData::EA_UNLINK);

  taProject* proj = myProject();
  if(proj) {
    editUndoAction->setEnabled(proj->undo_mgr.UndosAvail() > 0);
    editRedoAction->setEnabled(proj->undo_mgr.RedosAvail() > 0);
  }

  emit SetActionsEnabled();
}

void iMainWindowViewer::windowActivationChange(bool oldActive) {
  if (isActiveWindow()) {
    int idx = taiMisc::active_wins.FindEl(this);
    if (idx < 0) {
      taMisc::Error("iMainWindowViewer::windowActivationChange", "Unexpectedly not in taiMisc::viewer_wins");
    } else {
      if (idx < (taiMisc::active_wins.size - 1)) {
        // move us to the end
        taiMisc::active_wins.MoveIdx(idx, taiMisc::active_wins.size - 1);
      }
    }
  }
  inherited::windowActivationChange(oldActive);
}

//////////////////////////
// 	iTabBar 	//
//////////////////////////

QIcon* iTabBar::tab_icon[iTabBar::TI_LOCKED + 1]; 

void iTabBar::InitClass() {
  // load pushpin icons
  tab_icon[TI_UNPINNED] = new QIcon(":/images/tab_unpushed.png");
  tab_icon[TI_PINNED] = new QIcon(":/images/tab_pushed.png");
  tab_icon[TI_LOCKED] = new QIcon(":/images/tab_locked.png");
}

iTabBar::iTabBar(iTabView* parent_)
:QTabBar((QWidget*)parent_)
{
  defPalette = palette();

//  defBackgroundColor = paletteBackgroundColor();
//  defBaseColor = colorGroup().base();
}

iTabBar::~iTabBar() {
}

int iTabBar::addTab(iDataPanel* pan) {
  // if it is a lock guy, add to end, otherwise insert before lock guys
  int idx = 0;
  if (pan && pan->lockInPlace()) {
    idx = inherited::addTab("");
  } else {
    while (idx < count()) {
      iDataPanel* tpan = panel(idx);
      if (tpan && tpan->lockInPlace()) break; // found 1st guy
      ++idx;
    }
    idx = inherited::insertTab(idx, ""); // idx result is normally the same
  }
  setTabData(idx, QVariant((ta_intptr_t)0)); // set to valid null value
  SetPanel(idx, pan);
  return idx;
}

void iTabBar::contextMenuEvent(QContextMenuEvent * e) {
  // find the tab being clicked, or -1 if none
  int idx = count() - 1;
  while (idx >= 0) {
    if (tabRect(idx).contains(e->pos())) break;
    --idx;
  }
  // put up the context menu
  QMenu* menu = new QMenu(this);
  tabView()->FillTabBarContextMenu(menu, idx);
  menu->exec(QCursor::pos());
  delete menu;
}

void iTabBar::mousePressEvent(QMouseEvent* e) {
  QTabBar::mousePressEvent(e);
  if (tabView()->m_viewer_win)
    tabView()->m_viewer_win->TabView_Selected(tabView());
}

void iTabBar::keyPressEvent(QKeyEvent* e) {
  bool ctrl_pressed = false;
  if(e->modifiers() & Qt::ControlModifier)
    ctrl_pressed = true;
#ifdef TA_OS_MAC
  // ctrl = meta on apple
  if(e->modifiers() & Qt::MetaModifier)
    ctrl_pressed = true;
#endif
  if(ctrl_pressed && ((e->key() == Qt::Key_B) || (e->key() == Qt::Key_F))) {
    // following code from qtabbar.cpp:
    int dx = e->key() == (isRightToLeft() ? Qt::Key_F : Qt::Key_B) ? -1 : 1;
    for (int index = currentIndex() + dx; index >= 0 && index < count(); index += dx) {
      if (isTabEnabled(index)) {
	setCurrentIndex(index);
	break;
      }
    }
  }
  else {
    inherited::keyPressEvent(e);
  }
}

bool iTabBar::focusNextPrevChild(bool next) {
//   cerr << "iTabBar:: focusnext/prv" << endl;
  if(!next) return inherited::focusNextPrevChild(next);

  int idx = currentIndex();
  iDataPanel* tpan = panel(idx);
  if(!tpan) return inherited::focusNextPrevChild(next);
  QWidget* nxt = tpan->firstTabFocusWidget();
  if(!nxt) return inherited::focusNextPrevChild(next);
//   cerr << "focusing on widget of type: " << nxt->metaObject()->className() << endl;
  nxt->setFocus();
  return true;
}

iDataPanel* iTabBar::panel(int idx) {
  QVariant data(tabData(idx)); // returns NULL variant if out of range
  if (data.isNull() || !data.isValid()) return NULL;
  ta_intptr_t rval = data.value<ta_intptr_t>(); //NOTE: if probs in msvc, use the qvariant_cast thingy
  return (iDataPanel*)rval;
  
}

void iTabBar::setTabIcon(int idx, TabIcon ti) {
  QIcon* ico = NULL;
  if (ti != TI_NONE)
    ico = tab_icon[ti];
  if (ico) 
    inherited::setTabIcon(idx, *ico);
  else
    inherited::setTabIcon(idx, QIcon());
}

void iTabBar::SetPanel(int idx, iDataPanel* value, bool force) {
  QVariant data(tabData(idx)); // returns NULL variant if out of range
  iDataPanel* m_panel = NULL;
  if (!data.isNull() && data.isValid())
    m_panel = (iDataPanel*)data.value<ta_intptr_t>(); //NOTE: if probs in msvc, use the qvariant_cast thingy
  
  if ((m_panel == value) && !force) goto set_cur;
  m_panel = value;
  if (m_panel) {
    setTabText(idx, m_panel->TabText());
    setTabIcon(idx, m_panel->tabIcon());
//    m_panel->mtab_cnt++;
    // make sure we show latest data (helps in case there are "holes" in data updating)
    m_panel->UpdatePanel();
  } else {
    setTabText(idx, "");
    setTabIcon(idx, TI_NONE); // no icon
  }
  data = (ta_intptr_t)m_panel;
  setTabData(idx, data);
  
set_cur:
;/*don't autoset  if (currentIndex() != idx)
    setCurrentIndex(idx); */
}


//////////////////////////
//   iDataPanel_PtrList	//
//////////////////////////

void* iDataPanel_PtrList::El_Own_(void* it) {
  if (m_tabView) 
    ((iDataPanel*)it)->setTabView(m_tabView); 
  return it;
}

void iDataPanel_PtrList::El_disOwn_(void* it_) {
  if (m_tabView) {
    iDataPanel* it = (iDataPanel*)it_;
    if (it->tabView() == m_tabView)
      it->setTabView(NULL); 
  }
}


//////////////////////////
//   iTabView		//
//////////////////////////

iTabView::iTabView(QWidget* parent)
:QWidget(parent)
{
  m_viewer_win = NULL;
  Init();
}

iTabView::iTabView(iTabViewer* data_viewer_, QWidget* parent)
:QWidget(parent)
{
  m_viewer_win = data_viewer_;
  Init();
}


iTabView::~iTabView()
{
  panels.Reset(); // removes the refs, so we don't get callbacks
  if (m_viewer_win)
    m_viewer_win->TabView_Destroying(this);
}


void iTabView::Init() {
  panels.m_tabView = this;
  layDetail = new QVBoxLayout(this);
  // no margins/spacing needed
  layDetail->setMargin(0);
  layDetail->setSpacing(0);
  tbPanels = new iTabBar(this);
#if (QT_VERSION >= 0x040200)
  tbPanels->setUsesScrollButtons(true);
  tbPanels->setElideMode(Qt::ElideMiddle/*Qt::ElideNone*/); 
#endif
  layDetail->addWidget(tbPanels);
  wsPanels = new QStackedWidget(this);
  layDetail->addWidget(wsPanels);
  // add a dummy data panel with id=0 to show blank (except dialogs)
  MainWindowViewer* mwv = tabViewerWin()->mainWindowViewer();
  if (!mwv->isDialog()) {
    tbPanels->addTab("");
    connect(tbPanels, SIGNAL(currentChanged(int)),
      this, SLOT(panelSelected(int)) );
  }
}

void iTabView::Activated(bool val) {
  // change our appearance when inactivated
  QFont f(tbPanels->font());
  f.setItalic(!val);
  tbPanels->setFont(f);
}

bool iTabView::ActivatePanel(taiDataLink* dl) {
  for (int i = 0; i < tbPanels->count(); ++i) {
    iDataPanel* panel = tbPanels->panel(i);
    if (!panel) continue;
    if (panel->link() == dl) {
      SetCurrentTab(i);
      return true;
    }
  }
  return false;
}

bool iTabView::AddPanel(iDataPanel* panel) {
  if (!panels.AddUnique(panel)) return false; // refs us on add
  wsPanels->addWidget(panel);
  if (panels.size == 1) wsPanels->setCurrentWidget(panel); // always show first
  iTabViewer* itv = tabViewerWin();
  if (itv) panel->OnWindowBind(itv);
  return true;
}

void iTabView::AddPanelNewTab(iDataPanel* panel, bool lock) {
  AddPanel(panel); //noop if already added
  if (lock) panel->Pin();
  int tab_idx = tbPanels->addTab(panel);
  SetCurrentTab(tab_idx);
}

void iTabView::AddTab(int tab_idx) {
  iDataPanel* pan = NULL;
  if (tab_idx >= 0)
    pan = tbPanels->panel(tab_idx);
  // "AddTab" on a view guy just makes a blank tab
  if (pan && pan->lockInPlace())
    pan = NULL;
  int new_tab_idx = tbPanels->addTab(pan);
  SetCurrentTab(new_tab_idx);
}

void iTabView::CloseTab(int tab) {
  if (tab < 0) return; // huh?
  // don't allow closing last tab for a modified panel
  if (tbPanels->count() > 1) {
    tbPanels->removeTab(tab);
    panelSelected(tbPanels->currentIndex()); // needed to resync proper panel with tab
  } else { // last tab
    panelSelected(-1);
  }
}

void iTabView::Closing(CancelOp& cancel_op) {
  // close all panels
  for (int i = panels.size - 1; i >= 0; --i) {
    iDataPanel* panel = panels.FastEl(i);
    panel->Closing(cancel_op);
    if (cancel_op == CO_CANCEL) return; // can stop now

    RemoveDataPanel(panel); // note: removes from tabs, and deletes tabs
  }
}

iDataPanel* iTabView::curPanel() const {
  iDataPanel* rval = tbPanels->panel(tbPanels->currentIndex());
  return rval; // could be NULL if empty
}

void iTabView::DataPanelDestroying(iDataPanel* panel) {
  RemoveDataPanel(panel);//TODO
}

void iTabView::FillTabBarContextMenu(QMenu* contextMenu, int tab_idx) {
  iDataPanel* dp = tabPanel(tab_idx); // always safe, NULL if no tab
  // note: need to (re)parent the actions; not parented by adding to menu
  taiAction* act = new taiAction(tab_idx, "&Add Tab",  CTRL+Key_T );
  act->connect(taiAction::int_act, this,  SLOT(AddTab(int))); 
  act->setParent(contextMenu);
  contextMenu->addAction(act);
  // only add Close if on a tab
  if (tab_idx >= 0) {
    // always add for consistency, even if on an empty or locked guy
    act = new taiAction(tab_idx, "&Close Tab", QKeySequence());
    act->setParent(contextMenu);
    contextMenu->addAction(act);
    if (dp && dp->lockInPlace()) 
      act->setEnabled(false);
    else 
      act->connect(taiAction::int_act, this,  SLOT(CloseTab(int))); 
  }
  // pinning/unpinning only if not lockInPlace guy
  if (tab_idx < 0) return;
  if (!dp || dp->lockInPlace()) return;
  contextMenu->addSeparator();
  if (dp->pinned()) {
    act = new taiAction("&Unpin",  dp, SLOT(Unpin()), CTRL+Key_P );
  } else {
    act = new taiAction("&Pin in place",  dp, SLOT(Pin()), CTRL+Key_P );
  }//TODO
  act->setParent(contextMenu);
  contextMenu->addAction(act);
}

void iTabView::ShowLink(taiDataLink* link, bool not_in_cur) {
  iDataPanel* pan = GetDataPanel(link);
  ShowPanel(pan, not_in_cur);
}

//TODO
iDataPanel* iTabView::GetDataPanel(taiDataLink* link) {
  iDataPanel* rval;
  for (int i = 0; i < panels.size; ++i) {
    rval = panels.FastEl(i);
    if (rval->link() == link) return rval;
  }

  rval = link->CreateDataPanel();
  if (rval != NULL) {
    AddPanel(rval);
    //note: we don't do a show() here because it automatically raises the panel
//    rval->show(); //needed!
  }
  return rval;
}

void iTabView::GetWinState() {
//TODO: may want to save state of what panels are active
  for (int i = 0; i < panelCount(); ++i) {
    iDataPanel* pn = panel(i);
    if (pn) pn->GetWinState();
  }
}

void iTabView::SetWinState() {
//TODO: may want to get state of what panels are active
  for (int i = 0; i < panelCount(); ++i) {
    iDataPanel* pn = panel(i);
    if (pn) pn->SetWinState();
  }
}

void iTabView::ShowTab(iDataPanel* panel, bool show, bool focus) {
// this is for ctrl panel frames that go visible, to show their ctrl panel tabs
// note that we are assuming for simplicity that we can focus the default or 0th tab
// when removing a tab for a visible ctrl guy
  if (show) {
    // may be there already, prob most recent...
    int tb = -1;
    for (int i = tabCount() - 1; i >= 0; --i) {
      iDataPanel* tpan = tabPanel(i);
      if (tpan == panel) {
        tb = i;
        break;
      }
    }
    if (tb < 0) {
      tb = tbPanels->addTab(panel);
    }
    // focus it
    if (focus) 
      SetCurrentTab(tb);
  } else {
    for (int i = tabCount() - 1; i >= 0; --i) {
      iDataPanel* tpan = tabPanel(i);
      if (tpan == panel) {
        tbPanels->removeTab(i);
        if (tpan->isVisible()) {
          SetCurrentTab(0); // should be the non-ctrl default edit guy...
        }
        return;
      }
    }
  }
}

iDataPanel* iTabView::panel(int pan_idx) {
  return panels.PosSafeEl(pan_idx);
}

int iTabView::panelCount() const {
  return panels.size;
}

void iTabView::panelSelected(int idx) {
  iDataPanel* panel = NULL;
  if (idx >= 0) panel = tbPanels->panel(idx); 
  if (panel) {
    wsPanels->setCurrentWidget(panel);
  } else {
    wsPanels->setCurrentIndex(-1);
  }
  if (m_viewer_win)
    m_viewer_win->TabView_Selected(this);
}

void iTabView::OnWindowBind(iTabViewer* itv) {
  for (int i = 0; i < tbPanels->count(); ++i ) {
    iDataPanel* pan = tbPanels->panel(i);
    if (!pan) continue; // can happen!!!
    pan->OnWindowBind(itv);
  }
}

void iTabView::Refresh() {
  for (int i = panels.size - 1; i >= 0; --i) {
    iDataPanel* panel = panels.FastEl(i);
    panel->UpdatePanel();
  }
  UpdateTabNames();
}

void iTabView::RemoveDataPanel(iDataPanel* panel) {
  // we guard for destructing case by clearing panels, so don't detect it
  if (panels.RemoveEl(panel)) { // Remove unrefs us in panel
    wsPanels->removeWidget(panel); // superfluous, but safe, if panel is destroying
    // remove any associated tabs, except leave last non-locked tab (will get deleted anyway if we are
    // destructing)
    for (int i = tbPanels->count() - 1; i >= 0; --i) {
      iDataPanel* dp = tbPanels->panel(i);
      if (dp == panel) {
        if ((i > 0) || (tbPanels->count() > 1)) {
          tbPanels->removeTab(i);
          // next tab focuses, but doesn't send us an event, so...
          // activate next, unless we removed last
          int fi = i;
          if (fi >= tbPanels->count())
            --fi; 
          SetCurrentTab(fi);
        } else {
          tbPanels->SetPanel(0, NULL); // no panel
        }
      }
    }
  }
}

void iTabView::ResolveChanges(CancelOp& cancel_op) {
  for (int i = panels.size - 1; i >= 0; --i) {
    iDataPanel* panel = panels.FastEl(i);
    panel->ResolveChanges(cancel_op);
    if (cancel_op == CO_CANCEL) return; // can stop now
  }
}

bool iTabView::SetCurrentTab(int tab_idx) {
  if (tab_idx < 0 || tab_idx >= tbPanels->count()) return false;
  iDataPanel* pan = tabPanel(tab_idx);
  if (!pan) return false;
  tbPanels->setCurrentIndex(tab_idx);
  wsPanels->setCurrentWidget(pan);
  pan->show();			// make it visible for sure!
  return true;
}

bool iTabView::SetCurrentTabName(const String& tab_nm) {
  int tab_idx = TabIndexByName(tab_nm);
  if(tab_idx >= 0) {
    return SetCurrentTab(tab_idx);
  }
  return false;
}

void iTabView::ShowPanel(iDataPanel* panel, bool not_in_cur) {
  if (!panel) return;
  //note: panel has typically been added already, but we double check
  
  iDataPanel* cur_pn = curPanel(); //note: can be null
  
  // first, see if we have a tab for guy already -- don't create more than 1 per guy
  if (ActivatePanel(panel->link())) return;
  
  // always create a new tab for lockinplace guys
  if (panel->lockInPlace()) {
    AddPanelNewTab(panel);
    return;
  }
  
  // ok, so we'll either replace cur panel, swap one out, or make a new
  
  // replace curr if allowed and it is not locked, pinned, or dirty+autocommit
  if (!not_in_cur && cur_pn && (!cur_pn->lockInPlace() && !cur_pn->pinned() &&
     (!cur_pn->dirty() || autoCommit()))) 
  {
    bool proceed = true;
    if (cur_pn->dirty()) { // must be autocommit
      CancelOp cancel_op = CO_PROCEED;
      cur_pn->ResolveChanges(cancel_op);
      proceed = (cancel_op == CO_PROCEED);
    }
    if (proceed) {
      tbPanels->SetPanel(tbPanels->currentIndex(), panel);
      wsPanels->setCurrentWidget(panel);
      return;
    }
  }
  
  // try switching to another eligible panel
  for (int i = 0; i < tbPanels->count(); ++i) {
    iDataPanel* pn = tbPanels->panel(i);
    if (pn) {
      if (pn == cur_pn) continue;
      if (pn->lockInPlace() || (pn->dirty() && !autoCommit()) || pn->pinned()) continue;
    }
    bool proceed = true;
    if (pn && pn->dirty()) { // must be autocommit
      CancelOp cancel_op = CO_PROCEED;
      pn->ResolveChanges(cancel_op);
      proceed = (cancel_op == CO_PROCEED);
    }
    if (proceed) {
      tbPanels->SetPanel(i, panel);
      SetCurrentTab(i);
      return;
    }
  }
  
  // no eligible one, so make new
  AddPanelNewTab(panel);
}

int iTabView::tabCount() const {
  return tbPanels->count();
}

iDataPanel* iTabView::tabPanel(int tab_idx) {
  int ct = tabCount();
  if ((tab_idx < 0) || (tab_idx >= ct)) return NULL;
  return tbPanels->panel(tab_idx);
}

int iTabView::TabIndexOfPanel(iDataPanel* panel) const {
  for (int i = 0; i < tbPanels->count(); ++i) {
    if (tbPanels->panel(i) == panel) return i;
  }
  return -1;
}

int iTabView::TabIndexByName(const String& nm) const {
  for (int i = 0; i < tbPanels->count(); ++i) {
    if (tbPanels->panel(i)->TabText() == nm) return i;
  }
  return -1;
}


void iTabView::UpdateTabNames() { // called by a datalink when a tab name might have changed
  for (int i = 0; i < tbPanels->count(); ++i ) {
    iDataPanel* pan = tbPanels->panel(i);
    if (pan == NULL) continue; // shouldn't happen...
    tbPanels->setTabText(i, pan->TabText());
    tbPanels->setTabToolTip(i, pan->TabText()); // esp for when elided
    tbPanels->setTabIcon(i,  pan->tabIcon());
  }
}

void iTabView::UpdateTabName(iDataPanel* pan) { // called by a panel when its tab name may have changed
  for (int i = 0; i < tbPanels->count(); ++i ) {
    iDataPanel* pani = tbPanels->panel(i);
    if (pan != pani) continue; // shouldn't happen...
    tbPanels->setTabText(i, pan->TabText());
    tbPanels->setTabToolTip(i, pan->TabText()); // esp for when elided
    tbPanels->setTabIcon(i,  pan->tabIcon());
  }
}

void iTabView::keyPressEvent(QKeyEvent* e) {
  bool ctrl_pressed = false;
  if(e->modifiers() & Qt::ControlModifier)
    ctrl_pressed = true;
#ifdef TA_OS_MAC
  // ctrl = meta on apple
  if(e->modifiers() & Qt::MetaModifier)
    ctrl_pressed = true;
#endif
  if(ctrl_pressed) {
    if(e->key() == Qt::Key_P) {
      iDataPanel* dp = curPanel();
      if(dp) {
	if(dp->pinned())
	  dp->Unpin();
	else
	  dp->Pin();
      }
      e->accept();
      return;
    }
    else if(e->key() == Qt::Key_T) {
      AddTab(tbPanels->currentIndex());
      e->accept();
      return;
    }
    // I don't think it is useful to have a command for this -- too obscure and dangerous
//     else if(e->key() == Qt::Key_D) {
//       CloseTab(tbPanels->currentIndex());
//       e->accept();
//       return;
//     }
  }
  QWidget::keyPressEvent(e);
}



//////////////////////////
//   iTabView_PtrList	//
//////////////////////////


iTabView_PtrList::~iTabView_PtrList()
{
  for (int i = 0; i < size; ++i) {
    iTabView* tv = FastEl(i);
    tv->m_viewer_win = NULL; // prevents callback during destruction
  }
}

void iTabView_PtrList::DataPanelDestroying(iDataPanel* panel) {
  for (int i = 0; i < size; ++i) {
    iTabView* tv = FastEl(i);
    tv->DataPanelDestroying(panel);
  }
}

//////////////////////////
// 	iDataPanel 	//
//////////////////////////

iDataPanel::iDataPanel(taiDataLink* dl_)
:QFrame(NULL)
{
  m_tabView = NULL; // set when added to tabview; remains NULL if in a panelset
  m_pinned = false;
  m_rendered = false;
  m_update_on_show = true; // legacy default for most panels
  m_update_req = true;
  show_req = false;
  setFrameStyle(NoFrame | Plain);
  scr = new QScrollArea(this);
  scr->setWidgetResizable(true);
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(0);
  layOuter->setSpacing(2); //def
  layOuter->addWidget(scr, 1);

  if (dl_) //note: most classes always pass a link, but ex DocDataPanel doesn't
    dl_->AddDataClient(this); // sets our m_link variable
}

iDataPanel::~iDataPanel() {
  //note: m_tabView should be NULL during general destruction
  if (m_tabView)
    m_tabView->DataPanelDestroying(this);
}

QWidget* iDataPanel::centralWidget() const {
  return scr->widget();
}

void iDataPanel::customEvent(QEvent* ev_) {
  // we return early if we don't accept, otherwise fall through to accept
  bool focus = false;
  switch ((int)ev_->type()) {
  case CET_SHOW_PANEL_FOCUS: 
    focus = true; // FALL THROUGH
  case CET_SHOW_PANEL: {
    if (show_req) {
      FrameShowing_Async(focus);
      show_req = false;
    }
  } break;
  default: inherited(ev_); 
    return; // don't accept
  }
  ev_->accept();
}


void iDataPanel::DataChanged_impl(int dcr, void* op1, void* op2) {
  if (dcr <= DCR_ITEM_UPDATED_ND) {
    if (tabView())
      tabView()->UpdateTabName(this); //in case changed
  }
}

void iDataPanel::FrameShowing(bool showing, bool focus) {
  // note: if caller sets focus on its showing, then this will cause
  // spurious tab focuses on restore window, change desktop, etc.
  if (tabView()) {
    tabView()->ShowTab(this, showing, focus);
  }
}

void iDataPanel::FrameShowing_Async(bool focus) {
  if (tabView()) {
    tabView()->ShowTab(this, true, focus);
  }
  show_req = false;
}

void iDataPanel::GetWinState() {
  GetWinState_impl();
}

void iDataPanel::SetWinState() {
  SetWinState_impl();
}

void iDataPanel::OnWindowBind(iTabViewer* itv) {
  OnWindowBind_impl(itv);
  if (m_rendered)
    SetWinState();
}

void iDataPanel::UpdatePanel() {
  if (!isVisible()) return;
  UpdatePanel_impl();
}

void iDataPanel::UpdatePanel_impl() {
  m_update_req = false;
  if (tabView())
    tabView()->UpdateTabName(this); //in case changed 
}

void iDataPanel::Render() {
  if (!m_rendered) {
    Render_impl();
    SetWinState();
    m_rendered = true;
//NOTE: added 08/16/07 BA to try to solve netctrlpanel not initing right
    InitPanel();
    UpdatePanel();
  }
}

void iDataPanel::ResolveChanges(CancelOp& cancel_op) {
  ResolveChanges_impl(cancel_op);
}

void iDataPanel::setButtonsWidget(QWidget* widg) {
  widg->setParent(this);
  layOuter->addWidget(widg);
}

void iDataPanel::setCentralWidget(QWidget* widg) {
  scr->setWidget(widg);
  widg->show(); 
}

void iDataPanel::setUpdateOnShow(bool value) {
  if (m_update_on_show == value) return;
  m_update_on_show = value; // no action needed... 
}

void iDataPanel::setPinned(bool value) {
  if (m_pinned == value) return;
  m_pinned = value; // no action needed... "pinned is just a state of mind"
  if (tabView())
    tabView()->UpdateTabNames(); //updates the icons
}

void iDataPanel::hideEvent(QHideEvent* ev) {
  // note: we only call the impl, because each guy gets it, so we don't
  // want sets to then invoke this twice
  CancelOp cancel_op = CO_PROCEED;
  if (m_rendered) ResolveChanges_impl(cancel_op);
  inherited::hideEvent(ev);
}

void iDataPanel::showEvent(QShowEvent* ev) {
  inherited::showEvent(ev);
  // note: we only call the impl, because each guy gets it, so we don't
  // want sets to then invoke this twice
  if (m_rendered) {
    if (updateOnShow() || m_update_req)
      UpdatePanel_impl();
  } else            Render();
}

iTabBar::TabIcon iDataPanel::tabIcon() const {
//note: locked will be conveyed by no icon, since the lock icon takes up so much room
  if (lockInPlace()) 
    return iTabBar::TI_NONE; //iTabBar::TI_LOCKED;
  else {
    if (pinned()) return iTabBar::TI_PINNED;
    else return iTabBar::TI_UNPINNED;
  }
}

String iDataPanel::TabText() const {
/*Qt3  ISelectable* ci = tabViewerWin()->curItem();
  if (ci) {
    return ci->view_name();
  } else return _nilString; */
  if (m_link)
    return link()->GetDisplayName();
  else return _nilString;
}


//////////////////////////
//   iDataPanel_PtrList	//
//////////////////////////



//////////////////////////
//   iDataPanelFrame 	//
//////////////////////////

iDataPanelFrame::iDataPanelFrame(taiDataLink* dl_)
:inherited(dl_)
{
  m_dps = NULL;
  m_minibar_ctrls = NULL;
  lay_minibar_ctrls = NULL;
}

iDataPanelFrame::~iDataPanelFrame() {
}

void iDataPanelFrame::AddMinibarWidget(QWidget* ctrl) {
  if (!m_minibar_ctrls) {
    m_minibar_ctrls = new QWidget; // not owned, because panel set inserts
    lay_minibar_ctrls = new QHBoxLayout(m_minibar_ctrls);
    lay_minibar_ctrls->setMargin(0);
    lay_minibar_ctrls->setSpacing(2);
  }
  ctrl->setParent(m_minibar_ctrls);
  lay_minibar_ctrls->addWidget(ctrl, 0, (Qt::AlignRight | Qt::AlignVCenter));
}

void iDataPanelFrame::ClosePanel() {
  CancelOp cancel_op = CO_NOT_CANCELLABLE;
  Closing(cancel_op); // forced, ignore cancel
//TENT -- this could have bad side effects, but receiver should treat us as deleted...
  emit destroyed(this); // signals tab view we are gone
/*  if (m_tabView) // effectively unlink from system
    m_tabView->DataPanelDestroying(this); */
  if (!m_dps) // if in a panelset, we let panelset destroy us
    deleteLater(); // per Qt specs, defer deletions to avoid issues
}

void iDataPanelFrame::DataLinkDestroying(taDataLink*) {
  if (!m_dps) // if in a panelset, we let panelset destroy us
    ClosePanel();
}

void iDataPanelFrame::UpdatePanel() {
  if (!HasChanged()) 
    inherited::UpdatePanel();
}

taiDataLink* iDataPanelFrame::par_link() const {
  if (m_dps) return m_dps->par_link();
  else       return (tabView()) ? tabView()->par_link() : NULL;
}

MemberDef* iDataPanelFrame::par_md() const {
  if (m_dps) return m_dps->par_md();
  else       return (tabView()) ? tabView()->par_md() : NULL;
}

String iDataPanelFrame::TabText() const {
  if (m_dps) return m_dps->TabText();
  else       return inherited::TabText();
}

iTabViewer* iDataPanelFrame::tabViewerWin() const {
  if (m_dps) return m_dps->tabViewerWin();
  else       return (tabView()) ? tabView()->tabViewerWin() : NULL;
}


//////////////////////////
//    iViewPanelFrame 	//
//////////////////////////

iViewPanelFrame::iViewPanelFrame(taDataView* dv_)
:inherited((taiDataLink*)dv_->GetDataLink()) //NOTE: link not created yet during loads
{
  m_dps = NULL;
  vp_flags = 0;
  read_only = false;
  m_dv = dv_;
  updating = 0;
  m_modified = false;
  apply_req = false;
  taDataLink* dl = dv_->GetDataLink();
  dl->AddDataClient(this);
  btnApply = NULL;
  btnRevert = NULL; 
  btnCopyFrom = NULL;
  warn_clobber = false;
}

iViewPanelFrame::~iViewPanelFrame() {
  if (m_dps) {// if in a panelset, and it hasn't null'ed us
    m_dps->PanelDestroying(this); // removes tab
  }
}

void iViewPanelFrame::Apply() {
  if (warn_clobber) {
    int chs = taMisc::Choice("Warning: this object has changed since you started editing -- if you apply now, you will overwrite those changes -- what do you want to do?",
			     "Apply", "Revert", "Cancel");
    if(chs == 1) {
      Revert();
      return;
    }
    if(chs == 2)
      return;
  }
  GetValue();
  GetImage();
  InternalSetModified(false); // superfulous??
}

void iViewPanelFrame::Apply_Async() {
  if (apply_req) return; // already waiting
  QEvent* ev = new QEvent((QEvent::Type)CET_APPLY);
  apply_req = true;
  QCoreApplication::postEvent(this, ev);
}

void iViewPanelFrame::ClosePanel() {
  CancelOp cancel_op = CO_NOT_CANCELLABLE;
  Closing(cancel_op); // forced, ignore cancel
  
  emit destroyed(this); // signals tab view we are gone
/*  if (m_tabView) // effectively unlink from system
    m_tabView->DataPanelDestroying(this); */
  if (m_dps) {// if in a panelset, we let panelset destroy us
    m_dps->PanelDestroying(this); // removes tab
    if (tabView()) // effectively unlink from system
      tabView()->DataPanelDestroying(this);
  } else {
    deleteLater(); // per Qt specs, defer deletions to avoid issues
  }
}

void iViewPanelFrame::Changed() {
  if (updating > 0) return;
  InternalSetModified(true);
}

const iColor iViewPanelFrame::colorOfCurRow() const {
  return this->palette().color(QPalette::Active, QPalette::Background);
}

void iViewPanelFrame::customEvent(QEvent* ev_) {
  // we return early if we don't accept, otherwise fall through to accept
  switch ((int)ev_->type()) {
  case CET_APPLY: {
    if (apply_req) {
      Apply();
      apply_req = false;
    }
  } break;
  default: inherited::customEvent(ev_); 
    return; // don't accept
  }
  ev_->accept();
}

void iViewPanelFrame::DataLinkDestroying(taDataLink*) {
  m_dv = NULL;
  ClosePanel();
}

void iViewPanelFrame::GetValue() {
  ++updating;
  InternalSetModified(false); // do it first, so signals/updates etc. see us nonmodified
  GetValue_impl();
  --updating;
}

void iViewPanelFrame::InitPanel() {
  if (!m_dv) return;
  if (updating) return;
  ++updating;
  InitPanel_impl();
  if (isVisible()) { // no update when hidden!
    UpdatePanel_impl();
  }
  --updating;
}

void iViewPanelFrame::InternalSetModified(bool value) {
  m_modified = value;
  if (!value) warn_clobber = false;
  UpdateButtons();
}

void iViewPanelFrame::MakeButtons(QBoxLayout* lay, QWidget* par) {
  if (vp_flags & VP_USE_BTNS) return;
  if (!lay) return; // bug
  if (!par) par = this;
  
  QHBoxLayout* layButtons = NULL;
  if (lay)
    layButtons = new QHBoxLayout();
  else
    layButtons = new QHBoxLayout(par);
  layButtons->setMargin(0);
  layButtons->setSpacing(0);
  btnCopyFrom = new HiLightButton("&Copy From", par);
  layButtons->addWidget(btnCopyFrom);
  //  layButtons->addSpacing(4);

  layButtons->addStretch();
  btnApply = new HiLightButton("&Apply", par);
  layButtons->addWidget(btnApply);
  layButtons->addSpacing(4);
  
  btnRevert = new HiLightButton("&Revert", par);
  layButtons->addWidget(btnRevert);
  layButtons->addSpacing(4);
  if (lay)
    lay->addLayout(layButtons);
  
  connect(btnApply, SIGNAL(clicked()), this, SLOT(Apply()) );
  connect(btnRevert, SIGNAL(clicked()), this, SLOT(Revert()) );
  connect(btnCopyFrom, SIGNAL(clicked()), this, SLOT(CopyFrom()) );
  
  vp_flags |= VP_USE_BTNS;
  InternalSetModified(false);
}

void iViewPanelFrame::ResolveChanges_impl(CancelOp& cancel_op) {
  // called by root on closing, dialog on closing, etc. etc., when hiding
  if (HasChanged()) {
    Apply();
  }
}
 
void iViewPanelFrame::Revert() {
  GetImage();
  InternalSetModified(false);
}

void iViewPanelFrame::CopyFrom() {
  CopyFrom_impl();
  UpdatePanel();
  if(Base() && Base()->InheritsFrom(&TA_taDataView)) {
    taDataView* dvm = (taDataView*)Base();
    dvm->Render();
  }
}

void iViewPanelFrame::UpdatePanel() {
  if (!m_dv) return;
  if (updating) return;
  if (!isVisible()) return; // no update when hidden!
  ++updating;
  UpdatePanel_impl();
  --updating;
}

String iViewPanelFrame::TabText() const {
  if (m_dv) return m_dv->GetLabel();
  else      return inherited::TabText();
}

iTabViewer* iViewPanelFrame::tabViewerWin() const {
  return (tabView()) ? tabView()->tabViewerWin() : NULL;
}

void iViewPanelFrame::UpdateButtons() {
  if (!(vp_flags & VP_USE_BTNS)) return;
  if (Root() && m_modified) {
    btnApply->setEnabled(true);
    btnRevert->setEnabled(true);
  } else {
    btnApply->setEnabled(false);
    btnRevert->setEnabled(false);
  }
}

void iViewPanelFrame::keyPressEvent(QKeyEvent* e) {
  bool ctrl_pressed = false;
  if(e->modifiers() & Qt::ControlModifier)
    ctrl_pressed = true;
#ifdef TA_OS_MAC
  // ctrl = meta on apple
  if(e->modifiers() & Qt::MetaModifier)
    ctrl_pressed = true;
#endif

  if(ctrl_pressed && ((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter))) {
    Apply();			// do it!
  }
  if(e->key() == Qt::Key_Escape) {
    Revert();			// do it!
  }
}

//////////////////////////
//    iDataPanelSet 	//
//////////////////////////

iDataPanelSetBase::iDataPanelSetBase(taiDataLink* link_)
:inherited(link_)
{
  cur_panel_id = -1;
  widg = new QWidget();
  layDetail = new QVBoxLayout(widg); 
  // don't introduce any new margin
  layDetail->setMargin(0);
  layDetail->setSpacing(0);
  wsSubPanels = new QStackedWidget(widg); // add to lay by desc
  
  setCentralWidget(widg);
}

iDataPanelSetBase::~iDataPanelSetBase() {
  for (int i = panels.size - 1; i >= 0 ; --i) {
    iDataPanel* pn = panels.FastEl(i);
    pn->setTabView(NULL);
    pn->ClearDataPanelSet();
  }
}

void iDataPanelSetBase::Closing(CancelOp& cancel_op) {
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    pn->Closing(cancel_op);
    if (cancel_op == CO_CANCEL) return;
  }
}

void iDataPanelSetBase::ClosePanel() {
  for (int i = panels.size - 1; i >= 0 ; --i) {
    iDataPanel* pn = panels.FastEl(i);
    pn->ClosePanel();
  }
  if (tabView()) // effectively unlink from system
    tabView()->DataPanelDestroying(this);
  deleteLater();
}

void iDataPanelSetBase::DataLinkDestroying(taDataLink* dl) {
  ClosePanel();
} // nothing for us; subpanels handle

void iDataPanelSetBase::UpdatePanel() {
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    pn->UpdatePanel();
  }
}

const iColor iDataPanelSetBase::GetTabColor(bool selected, bool& ok) const {
  iDataPanel* pn = curPanel();
  if (pn) return pn->GetTabColor(selected, ok);
  else    return inherited::GetTabColor(selected, ok);
}

iDataPanel* iDataPanelSetBase::GetDataPanelOfType(TypeDef* typ, int& idx) {
  while ((idx >= 0) && (idx < panels.size)) {
    iDataPanel* rval = panels.FastEl(idx);
    idx++; // before returning val
    if (rval->GetTypeDef()->InheritsFrom(typ))
      return rval;
  }
  return NULL;
}

void iDataPanelSetBase::GetWinState() {
  GetWinState_impl();
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    if (pn) pn->GetWinState();
  }
}

void iDataPanelSetBase::SetWinState() {
  SetWinState_impl();
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    if (pn) pn->SetWinState();
  }
}

bool iDataPanelSetBase::HasChanged() {
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    if (pn->HasChanged_impl()) return true;
  }
  return false;
}

void iDataPanelSetBase::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    pn->OnWindowBind(itv);
  }
}

void iDataPanelSetBase::removeChild(QObject* obj) {
  panels.RemoveEl_(obj); // harmless if not a panel
  if (obj)
    obj->setParent(0);
}

void iDataPanelSetBase::ResolveChanges(CancelOp& cancel_op) {
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    pn->ResolveChanges(cancel_op);
    if (cancel_op == CO_CANCEL) return;
  }
  inherited::ResolveChanges(cancel_op); // calls our own impl
}

void iDataPanelSetBase::setCurrentPanelId(int id) {
  if (cur_panel_id == id) return;
  cur_panel_id = id; // set this early, to prevent unexpected calls from the signal
  setCurrentPanelId_impl(id);
}

void iDataPanelSetBase::setTabView(iTabView* value) {
  inherited::setTabView(value);
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    pn->setTabView(value);
  }
}

//////////////////////////
//    iDataPanelSet 	//
//////////////////////////

iDataPanelSetButton::iDataPanelSetButton(QWidget* parent) : QToolButton(parent) {
  m_datapanelset = NULL;
  m_idx = -1;
}

void iDataPanelSetButton::keyPressEvent(QKeyEvent* e) {
  if(!m_datapanelset || m_idx < 0) { inherited::keyPressEvent(e); return; }

  bool ctrl_pressed = false;
  if(e->modifiers() & Qt::ControlModifier)
    ctrl_pressed = true;
#ifdef TA_OS_MAC
  // ctrl = meta on apple
  if(e->modifiers() & Qt::MetaModifier)
    ctrl_pressed = true;
#endif

  // arrow/ctrl-key nav to adjacent buttons
  if((e->key() == Qt::Key_Left) || (ctrl_pressed && e->key() == Qt::Key_B)) {
    int prv_idx = m_idx -1;
    if(prv_idx < 0) prv_idx = m_datapanelset->panels.size-1;
    if(prv_idx == m_idx) { e->accept(); return; }
    QAbstractButton* pbut = m_datapanelset->buttons->button(prv_idx);
    if(pbut) {
      pbut->click();
      pbut->setFocus();
    }
    e->accept();
    return;
  }
  else if((e->key() == Qt::Key_Right) || (ctrl_pressed && e->key() == Qt::Key_F)) {
    int nxt_idx = m_idx +1;
    if(nxt_idx >= m_datapanelset->panels.size) nxt_idx = 0;
    if(nxt_idx == m_idx) { e->accept(); return; }
    QAbstractButton* pbut = m_datapanelset->buttons->button(nxt_idx);
    if(pbut) {
      pbut->click();
      pbut->setFocus();
    }
    e->accept();
    return;
  }
  inherited::keyPressEvent(e);
}

bool iDataPanelSetButton::focusNextPrevChild(bool next) {
  if(!m_datapanelset || m_idx < 0) return inherited::focusNextPrevChild(next);
  if(next) {
    iDataPanel* tpan = m_datapanelset->panels.SafeEl(m_idx); // my panel
    if(!tpan) return inherited::focusNextPrevChild(next);
    QWidget* nxt = tpan->firstTabFocusWidget();
    if(!nxt) return inherited::focusNextPrevChild(next);
    nxt->setFocus();
  }
  else { // prev
    iTabView* itv = m_datapanelset->tabView(); // tab view
    if(!itv) return inherited::focusNextPrevChild(next);
    itv->tbPanels->setFocus();	// focus back on overall tab bar
  }
  return true;
}

/////////////////////////////////////////

iDataPanelSet::iDataPanelSet(taiDataLink* link_)
:inherited(link_)
{
  layMinibar = NULL;
  frmButtons = new QFrame(widg);
  frmButtons->setFrameShape(QFrame::Box);
  frmButtons->setFrameShadow(QFrame::Sunken);
  layButtons = new QHBoxLayout(frmButtons);
  layButtons->setMargin(2);
  layButtons->setSpacing(taiM->hspc_c);
  // note: because we can add btns dynamically, we insert them, so put final
  // spacing here
  layButtons->addSpacing(3); // in case of minibar
  layButtons->addStretch(); // before mb
  layDetail->addWidget(frmButtons);
  buttons = new QButtonGroup(frmButtons); // note: not a widget, not visible
  buttons->setExclusive(true); // this is the default anyway

  layDetail->addWidget(wsSubPanels, 1);

  connect(buttons, SIGNAL(buttonClicked(int)), this, SLOT(setCurrentPanelId(int)));
}

iDataPanelSet::~iDataPanelSet() {
}

void iDataPanelSet::AddSubPanel(iDataPanelFrame* pn) {
  pn->m_dps = this;
  panels.Add(pn);
  int id = panels.size - 1;
  wsSubPanels->addWidget(pn);
  iDataPanelSetButton* but = new iDataPanelSetButton(frmButtons);
  but->m_datapanelset = this;
  but->m_idx = id;
  but->setMaximumHeight(taiM->button_height(taiMisc::sizSmall));
  but->setFont(taiM->buttonFont(taiMisc::sizSmall));
  // first visible button should be down
  if (id == 0) but->setDown(true); // first button should be down
  but->setCheckable(true);
  but->setText(pn->panel_type());
  buttons->addButton(but, id);
  // layout position is same as button (no preceding layouts/wids)
  layButtons->insertWidget(id, but);
  but->show();
  pn->AddedToPanelSet();
  iTabViewer* itv = tabViewerWin();
  if (itv) pn->OnWindowBind(itv);
}

void iDataPanelSet::AddSubPanelDynamic(iDataPanelFrame* pn) {
  AddSubPanel(pn);
  // ok, now the tricky part -- esp if we have a minibar but others didn't
  if (pn->minibarCtrls()) {
    if (layMinibar) { 
      // already created, so just add in our ctrl or dummy
      AddMinibarCtrl(pn);
    } else {
      // none, so need to make the whole bar and add all dummies
      AllSubPanelsAdded();
    }
  } else { // no mb for us, but add dummy if we had one
    if (layMinibar) {
      AddMinibarCtrl(pn); // we know we don't have one, but routine just does dummy
    }
  }
}

void iDataPanelSet::AddMinibar() {
  QFrame* fr = new QFrame(frmButtons);
  fr->setFrameStyle(QFrame::VLine | QFrame::Sunken);
  int idx = layButtons->count() - 2; // insert before stretch
  layButtons->insertWidget(idx, fr);
  layMinibar = new QStackedLayout;
  layButtons->addLayout(layMinibar);
}

void iDataPanelSet::AddMinibarCtrl(iDataPanelFrame* pn) {
  QWidget* ctrl = (pn) ? pn->minibarCtrls() : NULL;
  // note: easiest is just to create a null widg for missing guys
  if (ctrl) {
    ctrl->setParent(frmButtons);
  } else {
    ctrl = new QWidget(frmButtons);
  }
  layMinibar->addWidget(ctrl); // no final spacing etc. so same whether static or dyn
}

void iDataPanelSet::AddMinibarCtrls() {
  for (int i = 0; i < panels.size; ++i) {
    iDataPanelFrame* pn = qobject_cast<iDataPanelFrame*>(panels.FastEl(i));
    AddMinibarCtrl(pn);
  }
}

void iDataPanelSet::AllSubPanelsAdded() {
  // note: called after all static panels added, and if
  // we add a dynamic that needs minibar and not made yet
  // check if any need the minibar, if yes, initialize it and break
  for (int i = 0; i < panels.size; ++i) {
    iDataPanelFrame* pn = qobject_cast<iDataPanelFrame*>(panels.FastEl(i));
    if (!pn) continue; // should always be
    if (pn->minibarCtrls()) {
      AddMinibar();
      break;
    };
  }
  if (layMinibar) {
    AddMinibarCtrls();
  }
}

void iDataPanelSet::DataChanged_impl(int dcr, void* op1, void* op2) {
  inherited::DataChanged_impl(dcr, op1, op2);
  // if UDC then we need to invoke the dyn panel update procedure on the tai guy
  if (dcr == DCR_USER_DATA_UPDATED) {
    TypeDef* typ = link()->GetDataTypeDef();
    if (typ && typ->iv) {
      typ->iv->CheckUpdateDataPanelSet(this);
    }
  }
}

void iDataPanelSet::SetMenu(QWidget* menu) {
  menu->setParent(centralWidget());
  layDetail->insertWidget(0, menu);
}

void iDataPanelSet::SetMethodBox(QWidget* meths) {
  meths->setParent(centralWidget());
  layDetail->addWidget(meths);
}

void iDataPanelSet::setCurrentPanelId_impl(int id) {
  iDataPanel* pn = panels.PosSafeEl(id);
  if (!pn) return; //shouldn't happen
  wsSubPanels->setCurrentWidget(pn);
  QAbstractButton* but = buttons->button(id);
  // for when called programmatically:
  if (but) // should always have a value
    but->setDown(true);
  //note: very annoying, but autoexclusive was not working, so we had to do it manually
  QAbstractButton* but2;
  foreach(but2, buttons->buttons()) {
    if (but2 != but) but2->setDown(false);
  }
  if (layMinibar) {
    layMinibar->setCurrentIndex(id);
  }
  //TODO: maybe something to change tab color
}

void iDataPanelSet::setPanelAvailable(iDataPanel* pn) {
}

QWidget* iDataPanelSet::firstTabFocusWidget() {
  if(panels.size == 0) return NULL;
  if(panels.size == 1) return panels[0]->firstTabFocusWidget(); // skip buttons

  // focus on the current button
  QAbstractButton* but = buttons->button(cur_panel_id);
  if(but) {
    return but;
  }
  return NULL;
}


//////////////////////////
//    iViewPanelSet 	//
//////////////////////////

iViewPanelSet::iViewPanelSet(taiDataLink* link_)
:inherited(link_)
{
  layDetail->addWidget(wsSubPanels, 1);
  tbSubPanels = new QTabBar(widg);
  tbSubPanels->setShape(QTabBar::TriangularSouth);
  layDetail->addWidget(tbSubPanels);

  connect(tbSubPanels, SIGNAL(currentChanged(int)), this, SLOT(setCurrentPanelId(int)));
}

iViewPanelSet::~iViewPanelSet() {
//  panels.Reset(); // don't need/want to find any when child panels deleting
}

void iViewPanelSet::AddSubPanel(iViewPanelFrame* pn) {
  pn->m_dps = this;
  panels.Add(pn);
  wsSubPanels->addWidget(pn);
  tbSubPanels->addTab(pn->TabText());
  
  pn->AddedToPanelSet();
  iTabViewer* itv = tabViewerWin();
  if (itv) pn->OnWindowBind(itv);
}

void iViewPanelSet::PanelDestroying(iViewPanelFrame* pn) {
  int id = panels.FindEl(pn);
  if (id < 0) return;
  panels.RemoveIdx(id); // do 1st in case the gui gets triggered by:
  tbSubPanels->removeTab(id);
  pn->m_dps = NULL; // cut the link
}

void iViewPanelSet::setCurrentPanelId_impl(int id) {
  iDataPanel* pn = panels.PosSafeEl(id);
  if (!pn) return; //shouldn't happen
  wsSubPanels->setCurrentWidget(pn);
  tbSubPanels->setCurrentIndex(id);
}

void iViewPanelSet::UpdatePanel() {
  inherited::UpdatePanel();
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    tbSubPanels->setTabText(i, pn->TabText());
  }
}


//////////////////////////
//    iListDataPanel 	//
//////////////////////////

iListDataPanel::iListDataPanel(taiDataLink* dl_, const String& custom_name_)
:inherited(dl_)
{
  m_custom_name = custom_name_; // optional
  // note: just autoexpands the first fill, user must adjust after that
  list = new iTreeView(this, iTreeView::TV_AUTO_EXPAND);
  setCentralWidget(list);
  list->setSelectionMode(QTreeWidget::ExtendedSelection);
  list->setSortingEnabled(true);
  //enable dnd support, at least as source
  list->setDragEnabled(true);
//  list->setAcceptDrops(true);
//  list->setDropIndicatorShown(true);
  connect(list, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), 
    this, SLOT(list_itemDoubleClicked(QTreeWidgetItem*, int)) );
  ConfigHeader();
  // set default sort order to ASC -- don't know why def is DESC!
  list->sortItems(0, Qt::AscendingOrder);
  FillList();
}

iListDataPanel::~iListDataPanel() {
}

QWidget* iListDataPanel::firstTabFocusWidget() {
  return list;
}

void iListDataPanel::ClearList() {
  list->clear();
}

void iListDataPanel::ConfigHeader() {
  // set up number of cols, based on link, ok to repeat this
  list->setColumnCount(link()->NumListCols() + 1);
  QTreeWidgetItem* hdr = list->headerItem();
  hdr->setText(0, "#"); //note: we don't need a key, because we manage the text ourself
  for (int i = 0; i < link()->NumListCols(); ++i) {
    int hdr_idx = i + 1;
    KeyString key = link()->GetListColKey(i);
    hdr->setText(hdr_idx, link()->GetColHeading(key));
    hdr->setData(hdr_idx, iTreeView::ColKeyRole, key);
  }
}

void iListDataPanel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  if (dcr == DCR_LIST_ITEM_REMOVE) {
    // index will now be invalid for followers
    RenumberList();
  }
  // we handle the cases separately, since just refilling the list
  else if (dcr == DCR_LIST_ITEM_INSERT) {
    // insert at end, regardless of sort order
    taiDataLink* item = link()->GetListChild(op1_);
    if (!item) {
      taMisc::Warning("iListDataPanel::DataChanged_impl: unexpected could not find new list item");
      return;
    }
    taiListDataNode* last_child = dynamic_cast<taiListDataNode*>(
      list->item(list->itemCount() - 1));
    /*taiListDataNode* dn = */new taiListDataNode(-1, this, item, list,
      last_child, (iTreeViewItem::DNF_CAN_DRAG)); // numbered/decorated in Renumber call
    RenumberList();
  }
  // note: remember, we already handled insert and remove
  else if ((dcr >= DCR_LIST_MIN) && (dcr <= DCR_LIST_MAX)) {
    // for other list ops, esp sort or move, just reorder whole list (easy, harmless)
    RenumberList();
  }
  else if (dcr == DCR_STRUCT_UPDATE_END) {
    ConfigHeader();
  }
}

void iListDataPanel::FillList() {
  if (!link()) return; // zombie
  taiListDataNode* last_child = NULL;
  int i = 0;
  while (true) { // break when NULL child encountered
    taiDataLink* child = link()->GetListChild(i);
    if (!child) break;
    taiListDataNode* dn = new taiListDataNode(i, this, child, list,
      last_child, (iTreeViewItem::DNF_CAN_DRAG));
    dn->DecorateDataNode(); // fills in remaining columns
    last_child = dn;
    ++i;
  }
}

void iListDataPanel::RenumberList() {
  int i = 0;
  // mark all the items first, because this is the easiest, safest way/place
  // to remove items that are stale, ex. moved from our list to another list
  for (int j = 0; j < list->itemCount(); ++j) {
    taiListDataNode* dn = dynamic_cast<taiListDataNode*>(
      list->item(j));
    dn->num = -1;
  }
  
  // we have to iterate in proper link order, then find child, since items maybe
  // be sorted by some other column now
  for (taiDataLink* child; (child = link()->GetListChild(i)); ++i) { //iterate until no more
    if (!child) break;
    // find the item for the link
    for (int j = 0; j < list->itemCount(); ++j) {
      taiListDataNode* dn = dynamic_cast<taiListDataNode*>(
        list->item(j));
      if (dn && (dn->link() == child)) {
        dn->num = i;
        dn->DecorateDataNode(); // fills in remaining columns
        break;
      }
    }
  }
  
  // now delete stales -- note: an item that is deleting would have deleted
  // its node, but doing so now is harmless
  for (int j = list->itemCount() - 1; j >=0; --j) {
    taiListDataNode* dn = dynamic_cast<taiListDataNode*>(
      list->item(j));
    if (dn && (dn->num == -1)) delete dn;
  }
}

void iListDataPanel::list_itemDoubleClicked(QTreeWidgetItem* item_, int /*col*/) {
  taiListDataNode* item = dynamic_cast<taiListDataNode*>(item_);
  if (!item) return;
  taBase* ta = item->taData(); // null if n/a
  if (ta) {
//     tabMisc::DelayedFunCall_gui(ta, "BrowserSelectMe");
//    ta->BrowserSelectMe();
    // neither of the above actually update the panel view to new item -- presumably because
    // it is still in use or something..
    //    ta->EditPanel(true, false);	// new non-pinned panel -- leads to a proliferation
    // of panels and doesn't make a lot of sense.
    ta->EditDialog();		// pop up the edit dialog -- not favored, but probably
    				// the best thing for this situation
  }
}

void iListDataPanel::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  // connect the list up to the panel
  list->Connect_SelectableHostNotifySignal(itv,
    SLOT(SelectableHostNotifySlot_Internal(ISelectableHost*, int)) );
}

String iListDataPanel::panel_type() const {
  static String str("List View");
  if (m_custom_name.nonempty())
    return m_custom_name;
  else return str;
}

void iListDataPanel::UpdatePanel_impl() {
  //NOTE: when reshowing from invisible, we need to do this full refresh
  //obs list->Refresh();
  ClearList();
  FillList();
  inherited::UpdatePanel_impl();
}


//////////////////////////
//    iTextDataPanel 	//
//////////////////////////

iTextDataPanel::iTextDataPanel(taiDataLink* dl_)
:inherited(dl_)
{
  txtText = new iTextEdit(this);
  setCentralWidget(txtText);
  // default is ro
  setReadOnly(true);
  connect(txtText, SIGNAL(copyAvailable(bool)),
      this, SLOT(textText_copyAvailable(bool)) );
}

iTextDataPanel::~iTextDataPanel() {
}

QWidget* iTextDataPanel::firstTabFocusWidget() {
  return txtText;
}

void iTextDataPanel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
//TODO:  if (dcr <= DCR_ITEM_UPDATED_ND) ;
  //get updated text
}

int iTextDataPanel::EditAction(int ea) {
  int rval = 0;
//todo
  return rval;
}


int iTextDataPanel::GetEditActions() {
  int rval = 0;
  QTextCursor tc(txtText->textCursor());
  if (tc.hasSelection())
    rval |= taiClipData::EA_COPY;
//TODO: more, if not readonly
  return rval;
}

bool iTextDataPanel::readOnly() {
  return txtText->isReadOnly();
}

void iTextDataPanel::setReadOnly(bool value) {
  txtText->setReadOnly(value);
}

void iTextDataPanel::setText(const String& value) {
  txtText->setPlainText(value);
}

  
void iTextDataPanel::textText_copyAvailable (bool) {
  viewerWindow()->UpdateUi();
}

String iTextDataPanel::panel_type() const {
  static String str("Text View");
  return str;
}


//////////////////////////
//    iDocDataPanel 	//
//////////////////////////

iDocDataPanel::iDocDataPanel()
:inherited(NULL) // usual case: we dynamically set the link, via setDoc
{
  is_loading = false;

  wb_widg = new QWidget();
  wb_box = new QVBoxLayout(wb_widg);
  wb_box->setMargin(0); wb_box->setSpacing(2);

  int font_spec = taiMisc::fonMedium;

  url_box = new QHBoxLayout();    wb_box->addLayout(url_box);

  bak_but = new QToolButton(wb_widg);
  bak_but->setArrowType(Qt::LeftArrow);
  bak_but->setToolTip("Go backward one step in browsing history");
  url_box->addWidget(bak_but);
  fwd_but = new QToolButton(wb_widg);
  fwd_but->setArrowType(Qt::RightArrow);
  fwd_but->setToolTip("Go forward one step in browsing history");
  url_box->addWidget(fwd_but);
  url_box->addSpacing(taiM->hsep_c);
  go_but = new QPushButton("Go", wb_widg);
  go_but->setToolTip("Go to currently specified URL (can also just press enter in URL field)");
  url_box->addWidget(go_but);
  url_box->addSpacing(taiM->hsep_c);

  wiki_label = taiM->NewLabel("wiki:", wb_widg, font_spec);
  wiki_label->setToolTip("name of a wiki, as specified in global preferences, where this object should be stored -- this is used to lookup the wiki name -- if blank then url must be a full URL path");
  url_box->addWidget(wiki_label);
  url_box->addSpacing(taiM->hsep_c);
  wiki_edit = new iLineEdit(wb_widg);
  wiki_edit->setCharWidth(12);	// make this guy shorter
  url_box->addWidget(wiki_edit);
  url_box->addSpacing(taiM->hsep_c);

  url_label = taiM->NewLabel("URL:", wb_widg, font_spec);
  url_label->setToolTip("a URL location for this document -- if blank or 'local' then text field is used as document text -- otherwise if wiki name is set, then this is relative to that wiki, as wiki_url/index.php/Projects/url, otherwise it is a full URL path to a valid location");
  url_box->addWidget(url_label);
  url_box->addSpacing(taiM->hsep_c);
  url_edit = new iLineEdit(wb_widg);
  url_box->addWidget(url_edit);
  url_box->addSpacing(taiM->hsep_c);

  prog_bar = new QProgressBar(wb_widg);
  prog_bar->setRange(0, 100);
  prog_bar->setMaximumWidth(30);
  url_box->addWidget(prog_bar);

  seturl_but = new QPushButton("Set URL", wb_widg);
  url_box->addWidget(seturl_but);

  webview = new QWebView(wb_widg);
  wb_box->addWidget(webview);

  webview->page()->setNetworkAccessManager(taiMisc::net_access_mgr);
  webview->page()->setForwardUnsupportedContent(true);

  setCentralWidget(wb_widg);

  webview->installEventFilter(this); // translate keys..

  connect(go_but, SIGNAL(pressed()), this, SLOT(doc_goPressed()) );
  connect(bak_but, SIGNAL(pressed()), this, SLOT(doc_bakPressed()) );
  connect(fwd_but, SIGNAL(pressed()), this, SLOT(doc_fwdPressed()) );
  connect(url_edit, SIGNAL(returnPressed()), this, SLOT(doc_goPressed()) );
  connect(wiki_edit, SIGNAL(returnPressed()), this, SLOT(doc_goPressed()) );
  connect(seturl_but, SIGNAL(pressed()), this, SLOT(doc_seturlPressed()) );

  connect(webview, SIGNAL(loadProgress(int)), prog_bar, SLOT(setValue(int)) );
  connect(webview, SIGNAL(loadStarted()), this, SLOT(doc_loadStarted()) );
  connect(webview, SIGNAL(loadFinished(bool)), this, SLOT(doc_loadFinished(bool)) );

  webview->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  connect(webview->page(), SIGNAL(linkClicked(const QUrl&)),
	  this, SLOT(doc_linkClicked(const QUrl&)) );

//TODO  connect(txtText, SIGNAL(copyAvailable(bool)),
//      this, SLOT(textText_copyAvailable(bool)) );
  m_doc = NULL; // changed via setDoc -- if diff, we change our dl
}

iDocDataPanel::~iDocDataPanel() {
  taiMisc::net_access_mgr->setMainWindow(NULL); // not us anymore -- someone else will set..
  m_doc = NULL;
}

QWidget* iDocDataPanel::firstTabFocusWidget() {
  return url_edit;
}

void iDocDataPanel::doc_linkClicked(const QUrl& url) {
  String path = url.toString();
  //  cerr << "path: " << path << endl;
  bool ta_path = false;
  QUrl new_url(url);
  if(path.startsWith("ta:") || path.startsWith("."))
    ta_path = true;
  if(!ta_path && url.hasQueryItem("title")) { // wiki versions of our action urls get translated into queries with title=...
    String qry = url.queryItemValue("title");
    if(qry.startsWith("ta:") || qry.startsWith(".")) {
      if(!qry.startsWith("ta:"))
	qry = "ta:"+qry;	// rectify
      new_url.setUrl(qry);	// start from there.
      ta_path = true;
    }
  }
  if(!ta_path) {
    // a standard path:
    webview->load(url);	
    // todo: we could make a note of this somewhere, but key idea is that our URL
    // is fixed!!!
    return;
  }

  // handle it internally
  if (!new_url.hasFragment()) {
    if (viewerWindow())
      new_url.setFragment("#" + QString::number(viewerWindow()->uniqueId()));
  }
  // goes to: iMainWindowViewer::taUrlHandler  in ta_qtviewer.cpp
  QDesktopServices::openUrl(new_url);
}

void iDocDataPanel::doc_loadStarted() {
  is_loading = true;
  go_but->setText("X");
  prog_bar->reset();
}

void iDocDataPanel::doc_loadFinished(bool ok) {
  is_loading = false;
  go_but->setText("Go");
  if(!ok) return;
  taDoc* doc_ = this->doc();
  if(!doc_) return;
  if(!webview) return;
  QWebFrame* mnfrm = webview->page()->mainFrame();
  if(!mnfrm) return;
  doc_->html_text = mnfrm->toHtml(); // harvest it!
}

void iDocDataPanel::doc_goPressed() {
  taDoc* doc_ = this->doc();
  if(!doc_) return;
  if(!url_edit) return;
  if(is_loading) {
    // now means stop!
    if(webview) webview->stop();
  }
  else {
    doc_->wiki = wiki_edit->text();
    doc_->url = url_edit->text();
    doc_->UpdateAfterEdit();	// this will drive all the updating, including toggle from local etc
    UpdatePanel();		// also update us..
  }
}

void iDocDataPanel::doc_bakPressed() {
  // todo: could trap things here too
  webview->back();
}

void iDocDataPanel::doc_fwdPressed() {
  // todo: could trap things here too
  webview->forward();
}

void iDocDataPanel::doc_seturlPressed() {
  taDoc* doc_ = this->doc();
  if(!doc_) return;

  String url = webview->url().toString();
  doc_->SetURL(url);
}

bool iDocDataPanel::ignoreDataChanged() const {
  return false;			
  //  return !isVisible(); -- this doesn't seem to be giving accurate results!!!
}

void iDocDataPanel::DataLinkDestroying(taDataLink* dl) {
  setDoc(NULL);
}

void iDocDataPanel::UpdatePanel_impl() {
  inherited::UpdatePanel_impl();
  taDoc* doc_ = this->doc();
  if(!doc_) return;

  taiMisc::net_access_mgr->setMainWindow(viewerWindow());

  wiki_edit->setText(doc_->wiki);
  url_edit->setText(doc_->url);

  webview->setTextSizeMultiplier(doc_->text_size * ((float)taMisc::font_size / 12.0f));

  if(doc_->web_doc && taMisc::InternetConnected()) {
    String cur_url = webview->url().toString();
    String nw_url = doc_->GetURL();
    if(cur_url != nw_url)
      webview->load(QUrl(nw_url));
//     url_edit->setEnabled(true);
    fwd_but->setEnabled(true);
    bak_but->setEnabled(true);
//     go_but->setEnabled(true);
    seturl_but->setEnabled(true);
  }
  else {
    webview->setHtml(doc_->html_text);
    fwd_but->setEnabled(false);
    bak_but->setEnabled(false);
    seturl_but->setEnabled(false);
  }
}

void iDocDataPanel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  if (dcr <= DCR_ITEM_UPDATED_ND) {
    this->m_update_req = true; // so we update next time we show
    UpdatePanel();
  }
}

/* todo int iDocDataPanel::EditAction(int ea) {
  int rval = 0;
//todo
  return rval;
}


int iDocDataPanel::GetEditActions() {
  int rval = 0;
  QTextCursor tc(txtText->textCursor());
  if (tc.hasSelection())
    rval |= taiClipData::EA_COPY;
//TODO: more, if not readonly
  return rval;
} */

void iDocDataPanel::setDoc(taDoc* doc) {
  if (m_doc == doc) return;
  // if the doc is NULL, or different (regardless of NULL->val or val1->val2)
  // we will necessarily have to change links, so we always revoke link
  if (m_link) {
    m_link->RemoveDataClient(this);
  }
  
  m_doc = doc;
  if (doc) {
    taDataLink* dl = doc->GetDataLink();
    if (!dl) return; // shouldn't happen!
    dl->AddDataClient(this);
    UpdatePanel();
  } else {
    webview->setHtml("(no doc set)");
  }
}

bool iDocDataPanel::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() != QEvent::KeyPress) {
    return QObject::eventFilter(obj, event);
  }

  QKeyEvent* e = static_cast<QKeyEvent *>(event);
  bool ctrl_pressed = false;
  if(e->modifiers() & Qt::ControlModifier)
    ctrl_pressed = true;
#ifdef TA_OS_MAC
  // ctrl = meta on apple
  if(e->modifiers() & Qt::MetaModifier)
    ctrl_pressed = true;
#endif
  // emacs keys!!
  if(ctrl_pressed) {
    QCoreApplication* app = QCoreApplication::instance();
    if(e->key() == Qt::Key_P) {
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier));
      return true;		// we absorb this event
    }
    else if(e->key() == Qt::Key_N) {
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier));
      return true;		// we absorb this event
    }
    else if(e->key() == Qt::Key_A) {
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::ControlModifier));
      return true;		// we absorb this event
    }
    else if(e->key() == Qt::Key_E) {
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::ControlModifier));
      return true;		// we absorb this event
    }
    else if(e->key() == Qt::Key_F) {
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier));
      return true;		// we absorb this event
    }
    else if(e->key() == Qt::Key_B) {
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier));
      return true;		// we absorb this event
    }
    else if(e->key() == Qt::Key_D) {
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier));
      return true;		// we absorb this event
    }
    else if(e->key() == Qt::Key_K) {
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Clear, Qt::NoModifier));
      return true;		// we absorb this event
    }
    else if(e->key() == Qt::Key_Y) {
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier));
      return true;		// we absorb this event
    }
    else if(e->key() == Qt::Key_W) {
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_X, Qt::ControlModifier));
      return true;		// we absorb this event
    }
    else if(e->key() == Qt::Key_Slash) {
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
      return true;		// we absorb this event
    }
    else if(e->key() == Qt::Key_Minus) {
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier));
      return true;		// we absorb this event
    }
  }
  return QObject::eventFilter(obj, event);
}
  
/*void iDocDataPanel::br_copyAvailable (bool) {
  viewerWindow()->UpdateUi();
}*/




//////////////////////////
//    iTreeView 	//
//////////////////////////

const String iTreeView::opt_treefilt("TREEFILT_");


void iTreeView::FillTypedList(const QList<QTreeWidgetItem*>& items,
  ISelectable_PtrList& list)
{
  for (int i = 0; i < items.count(); ++i) {
    ISelectable* tvi = dynamic_cast<iTreeViewItem*>(items.at(i));
    if (tvi)
      list.Add(tvi);
  }
}

/*  TEMP: good colors for red highlighting of errors
  QColor h_base(0xFF, 0x99, 0x99); // pale dull red
  QColor h_rev(0x99, 0x33, 0x33); // dark dull red
*/
iTreeView::iTreeView(QWidget* parent, int tv_flags_)
:inherited(parent)
{
  focus_next_widget = NULL;
  focus_prev_widget = NULL;
  main_window = NULL;

  tv_flags = tv_flags_;
  m_filters = NULL; // only created if needed
  m_def_exp_levels = 2; // works well for most contexts
  m_show = (taMisc::ShowMembs)(taMisc::USE_SHOW_GUI_DEF | taMisc::show_gui);
  m_decorate_enabled = true;
  italic_font = NULL; 
  in_mouse_press = 0;
  setIndentation(taMisc::tree_indent);
  // set default 'invalid' highlight colors, but don't enable highlighting by default
  setHighlightColor(1, 
    QColor(0xFF, 0x99, 0x99),  // pale dull red
    QColor(0x99, 0x33, 0x33) // dark dull red
  );
  setHighlightColor(2, 
    QColor(0xFF, 0xFF, 0x99),  // pale dull yellow
    QColor(0x99, 0x99, 0x33) // dark dull yellow
  );
  setHighlightColor(3, 		// disabled
    QColor(0xa0, 0xa0, 0xa0),  // light grey
    QColor(0x80, 0x80, 0x80) // medium grey
  );

  connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
    this, SLOT(this_currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)) );
//  connect(this, SIGNAL(itemSelectionChanged()),
//    this, SLOT(this_itemSelectionChanged()) );
  //note: can't use "activate" because that is only for ex. double-clicking the item
  // can't use "pressed" because that solves problem, but then screws up drags, etc.
  connect(this, SIGNAL(clicked(const QModelIndex&)),
    this, SLOT(this_itemSelectionChanged()) );
  connect(this, SIGNAL(contextMenuRequested(QTreeWidgetItem*, const QPoint&, int)),
    this, SLOT(this_contextMenuRequested(QTreeWidgetItem*, const QPoint&, int)) );
}

iTreeView::~iTreeView() {
  if (m_filters) {
    delete m_filters;
    m_filters = NULL;
  }
  if (italic_font) {
    delete italic_font;
    italic_font = NULL;
  }
}

QMap_qstr_qvar iTreeView::colDataKeys(int col) const {
  QMap_qstr_qvar map;
  if ((col >= 0) || (col < columnCount())) {
    QVariant vmap = headerItem()->data(col, ColDataRole);
    if (vmap.canConvert(QVariant::Map)) {
      map = vmap.toMap();
    }
  }
  return map;
}

void iTreeView::AddColDataKey(int col, const KeyString& key, int role) {
//NOTE: the role is the map key
  if ((col < 0) || (col >= columnCount())) return;
  QMap_qstr_qvar map;
  // fetch existing map, if any
  QVariant vmap = headerItem()->data(col, ColDataRole);
  if (vmap.canConvert(QVariant::Map)) {
    map = vmap.toMap();
  }
  // note: ok to call multiple times; only sets once
  map[QString::number(role)] = key;
  headerItem()->setData(col, ColDataRole, map);
}

bool iTreeView::RemoveColDataKey(int col, const KeyString& key, int role) {
  if ((col < 0) || (col >= columnCount())) return false;
  // fetch existing map, if any
  QVariant vmap = headerItem()->data(col, ColDataRole);
  // if no map at all, then the key itself is definitely not set
  if (!vmap.canConvert(QVariant::Map)) return false;
  
  QMap_qstr_qvar map(vmap.toMap());
  bool rval = (map.remove(QString::number(role)) > 0);
  if (rval) // only need to re-set if it was actually removed
    headerItem()->setData(col, ColDataRole, map);
  return rval;
}

void iTreeView::ClearColDataKeys(int col) {
  if ((col < 0) || (col >= columnCount())) return;
  // fetch existing map, if any
  QVariant vmap = headerItem()->data(col, ColDataRole);
  if (!vmap.canConvert(QVariant::Map)) return;
  
  headerItem()->setData(col, ColDataRole, QVariant());
}

void iTreeView::AddFilter(const String& value) {
  if (!m_filters) {
    m_filters = new String_PArray;
  }
  m_filters->AddUnique(value);
}

iTreeViewItem* iTreeView::AssertItem(taiDataLink* link, bool super) {
  // first, check if already an item in our tree
  taDataLinkItr itr;
  iTreeViewItem* el;
  FOR_DLC_EL_OF_TYPE(iTreeViewItem, el, link, itr) {
    if (el->treeWidget() == this) {
      return el;
    }
  } 
  if (!super) return NULL; // when we are called by ourself
  // failed, so try to assert the owner
  taiDataLink* own_link = link->ownLink();
  if (!own_link) return NULL;
  iTreeViewItem* own_el = AssertItem(own_link);
  // note: don't bale if no own_el, because could be a defchild parent
  // then try making sure owner's children asserted
  if (own_el) { // && own_el->lazyChildren()) {
    own_el->CreateChildren();
    own_el->setExpanded(true);
    taiMisc::ProcessEvents();
  }
  // and try again, but not supercursively of course!
  return AssertItem(link, false);
}

void iTreeView::CollapseAll() {
  for (int i = topLevelItemCount() - 1; i >= 0; --i) {
    iTreeViewItem* node = dynamic_cast<iTreeViewItem*>(topLevelItem(i));
    if (node) 
      CollapseAllUnder(node);
  }
}

void iTreeView::CollapseAllUnder(iTreeViewItem* item) {
  //note: iterator didn't work, because it collapsed the siblings too!
  if (!item) return;
  taMisc::Busy(true); //note: should have ample count capacity for levels
  // first, the children (if any)...
  for (int i = item->childCount() - 1; i >=0 ; --i) {
    iTreeViewItem* node = dynamic_cast<iTreeViewItem*>(item->child(i));
    if (node)
      CollapseAllUnder(node);
  }
  // then ourself
  setItemExpanded(item, false); 
  taMisc::Busy(false);
} 

void iTreeView::CollapseAllUnderInt(void* item) {
  CollapseAllUnder((iTreeViewItem*)item);
} 

void iTreeView::InsertEl() {
  ISelectable* si = curItem();
  if(!si || !si->link()) return;		// nothing selected
  taBase* sb = si->link()->taData();
  if(!sb) return;
  taList_impl* sbo = NULL;
  if(sb->InheritsFrom(&TA_taList_impl)) {
    sbo = (taList_impl*)sb;
  }
  else {
    sbo = GET_OWNER(sb, taList_impl);
  }
  if(!sbo) return;
  taiTypeDefButton* typlkup =
    new taiTypeDefButton(sbo->el_base, NULL, NULL, NULL, taiData::flgAutoApply);
  TypeDef* td = sbo->el_typ;
  typlkup->GetImage(td, sbo->el_base);
  bool okc = typlkup->OpenChooser();
  td = typlkup->td();
  if(okc && td) {
    taProject* proj = myProject();
    if(proj) {
      proj->undo_mgr.SaveUndo(sbo, "InsertEl"); // global save
    }
    taBase* nwi = taBase::MakeToken(td);
    if(nwi) {
      int idx = 0;
      if(sbo == sb) {		// it is the list
	idx = sbo->size;
      }
      else {
	idx = sbo->FindEl(sb) + 1;
      }
      if(idx < 0) idx = 0;
      if(idx > sbo->size) idx = sbo->size;
      sbo->Insert(nwi, idx);
      tabMisc::DelayedFunCall_gui(nwi, "BrowserExpandAll");
      tabMisc::DelayedFunCall_gui(nwi, "BrowserSelectMe");
    }
  }
  delete typlkup;
}

const KeyString iTreeView::colKey(int col) const {
  if ((col < 0) || (col >= columnCount())) return _nilKeyString;
  KeyString rval = (headerItem()->data(col, ColKeyRole)).toString();
  return rval;
}

bool iTreeView::doubleClickExpandsAll() const {
  return (taMisc::viewer_options & taMisc::VO_DOUBLE_CLICK_EXP_ALL);
}

void iTreeView::ExpandAll(int max_levels) {
  ExpandAll_impl(max_levels, false);
}

void iTreeView::ExpandAll_impl(int max_levels, bool use_custom_filt) {
  //NOTE: we can't user iterators for expanding, because we add/remove items which 
  // crashes the iterator
  taMisc::Busy(true);
  for (int i = 0; i < topLevelItemCount(); ++i) {
    iTreeViewItem* node = dynamic_cast<iTreeViewItem*>(topLevelItem(i));
    if (!node) continue;
    ExpandItem_impl(node, 0, max_levels, use_custom_filt);
  }
//TODO: need to rejig resizing
  if (header()->isVisible() && (header()->count() > 1)) {
    resizeColumnsToContents();
  }
  taMisc::Busy(false);
}

void iTreeView::ExpandItem_impl(iTreeViewItem* item, int level,
  int max_levels, int exp_flags) 
{
  if (!item) return;
  if (isItemHidden(item)) return;

  // special check for guys that should not be auto-expaneded at all!  may need to do this in child below.
  taBase* tab = item->link()->taData();
  if(tab && tab->HasOption("NO_EXPAND_ALL")) return;

  bool expand = true; 
  if (!(exp_flags & EF_EXPAND_DISABLED)) {
    if (!item->link()->isEnabled())
      expand = false;
  }
  if (expand && (exp_flags & EF_CUSTOM_FILTER)) {
    emit CustomExpandFilter(item, level, expand);
  }
  if (expand) {
    // first expand the guy...
    if (!isItemExpanded(item)) { // ok, eligible...
//       if (item->lazyChildren() || (item->childCount() > 0)) {
        setItemExpanded(item, true); // should trigger CreateChildren for lazy
//       }
    }
    // check if we've expanded deeply enough 
    // (works for finite (>=1) and infinite (<0) cases)
    if (max_levels == 0) return;
  
    if (level >= 0) ++level;
    // and expand item's children -- lazy children should be created by now
    for (int i = 0; i < item->childCount(); ++i) {
      iTreeViewItem* child = dynamic_cast<iTreeViewItem*>(item->child(i));
      if (child)
        ExpandItem_impl(child, level, max_levels - 1, exp_flags);
    }
  } else {
    // note: following test not needed for 1st time, but is
    // needed for subsequent ExpandDefault
    if (isItemExpanded(item)) {
      setItemExpanded(item, false);
    }
  }
}


void iTreeView::ExpandAllUnder(iTreeViewItem* item, int max_levels) 
{
  if (!item) return;
  taMisc::Busy(true);
  ExpandItem_impl(item, -1, max_levels, EF_EXPAND_DISABLED);
  if (header()->isVisible() && (header()->count() > 1)) {
    resizeColumnsToContents();
  }
  taMisc::Busy(false);
} 

void iTreeView::ExpandAllUnderInt(void* item) {
  ExpandAllUnder((iTreeViewItem*)item);
}

void iTreeView::ExpandDefault() {
  int exp_flags = EF_DEFAULT;
  if (useCustomExpand()) exp_flags |= EF_CUSTOM_FILTER;
  if (tv_flags & TV_EXPAND_DISABLED) exp_flags |= EF_EXPAND_DISABLED;
  ExpandAll_impl(m_def_exp_levels, exp_flags);
}

void iTreeView::focusInEvent(QFocusEvent* ev) {
  inherited::focusInEvent(ev); // prob does nothing
  if(main_window) {
    main_window->cur_tree_view = this; // always overwrite with current
//     cerr << "focus itv: " << this << endl;
  }
  Emit_GotFocusSignal();
}

void iTreeView::GetSelectedItems(ISelectable_PtrList& lst) {
  QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::Selected);
  QTreeWidgetItem* item;
  while ( (item = *it) ) {
  ISelectable* si = dynamic_cast<ISelectable*>(item);
    if (si)
      lst.Add(si);
    ++it;
  }
}

bool iTreeView::HasFilter(TypeItem* ti) const {
  if (m_filters) for (int i = 0; i < m_filters->size; ++i) {
    if (ti->HasOptionAfter(opt_treefilt, m_filters->FastEl(i)))
      return true; 
  }
  return false;
}

bool iTreeView::hasMultiSelect() const {
  QAbstractItemView::SelectionMode sm = selectionMode();
  return ((sm == ContiguousSelection) ||
    (sm == ExtendedSelection) ||
    (sm == MultiSelection));
}

QFont& iTreeView::italicFont() const {
  if (!italic_font) {
    italic_font = new QFont(font());
    italic_font->setItalic(true);
  }
  return *italic_font;
}

iTreeViewItem* iTreeView::item(int i) const {
  if ((i < 0) || (i >= topLevelItemCount()))
    return NULL;
  return dynamic_cast<iTreeViewItem*>(topLevelItem(i));
}

void iTreeView::ItemDestroyingCb(iTreeViewItem* item) {
  RemoveSelectedItem((ISelectable*)item, false); // not forced, because it is gui
}

QMimeData* iTreeView::mimeData(const QList<QTreeWidgetItem*> items) const {
//NOTE: in Qt4, we no longer know if we are starting a drag operation
  if (items.count() == 0) return NULL; // according to Qt spec
  else if (items.count() == 1) {
    iTreeViewItem* tvi = dynamic_cast<iTreeViewItem*>(items.at(0));
    if (!tvi) return NULL; // prob shouldn't happen
    return tvi->GetClipDataSingle(taiClipData::EA_SRC_OPS, false);
  } else { // multi case
    ISelectable_PtrList list;
    FillTypedList(items, list);
    if (list.size == 0) return NULL; // not likely
    return list.FastEl(0)->GetClipData(list, taiClipData::EA_SRC_OPS, false);
  }
}

QStringList iTreeView::mimeTypes () const {
 //NOTE: for dnd to work, we just permit our own special mime type!!!
  QStringList rval;
  rval.append(taiObjectMimeFactory::tacss_objectdesc);
  return rval;
}

void iTreeView::keyPressEvent(QKeyEvent* e) {
  taProject* proj = myProject();
  bool ctrl_pressed = false;
  if(e->modifiers() & Qt::ControlModifier)
    ctrl_pressed = true;
#ifdef TA_OS_MAC
  // ctrl = meta on apple
  if(e->modifiers() & Qt::MetaModifier)
    ctrl_pressed = true;
#endif
  if(ctrl_pressed) {
    if(e->key() == Qt::Key_I) {
      InsertEl();
      e->accept();
      return;
    }
    if(e->key() == Qt::Key_M) {
      ISelectable* si = curItem();
      if(si && si->link()) {
	taBase* sb = si->link()->taData();
	if(sb) {
	  if(proj) {
	    proj->undo_mgr.SaveUndo(sb, "Duplicate"); // global save
	  }
	  sb->DuplicateMe();
	}
      }
      e->accept();
      return;
    }
  }
  if((ctrl_pressed && e->key() == Qt::Key_W) ||
     (ctrl_pressed && e->key() == Qt::Key_D) || (e->key() == Qt::Key_Delete)
     || (e->key() == Qt::Key_Backspace)) {
    ISelectable* si = curItem();
    if(si && si->link()) {
      taBase* sb = si->link()->taData();
      if(sb) {
	if(proj) {
	  proj->undo_mgr.SaveUndo(sb, "Delete"); // global save
	}
	sb->CloseLater();	// todo: needs to check if a member or not -- only nuke if !mbr
      }
    }
    e->accept();
    return;
  }
  if((e->modifiers() & Qt::AltModifier) && e->key() == Qt::Key_F) {
    ISelectable* si = curItem();
    if(si && si->link()) {
      taiDataLink* link = si->link();
      iMainWindowViewer* imw = mainWindow();
      if(imw) imw->Find(link);
    }
    e->accept();
    return;
  }
  inherited::keyPressEvent(e);
}

bool iTreeView::focusNextPrevChild(bool next) {
  if(next && focus_next_widget) {
    focus_next_widget->setFocus();
    return true;
  }
  if(!next && focus_prev_widget) {
    focus_prev_widget->setFocus();
    return true;
  }
  return inherited::focusNextPrevChild(next);
}

void iTreeView::mnuFindFromHere(taiAction* mel) {
  iMainWindowViewer* imw = mainWindow();
  if (!imw) return;
  iTreeViewItem* node = (iTreeViewItem*)(mel->usr_data.toPtr());
  taiDataLink* dl = node->link();
  imw->Find(dl);
}

void iTreeView::mouseDoubleClickEvent(QMouseEvent* event) {
  if (!doubleClickExpandsAll()) {
    // just does default stuff, which includes single level exp/collapse
    inherited::mouseDoubleClickEvent(event);
    return;
  }
  // NOTE: we replace all the default behavior with our custom exp/coll all shtick
  QModelIndex index = indexAt(event->pos());
  if (!index.isValid()) {
    inherited::mouseDoubleClickEvent(event); // does some other stuff, prob should keep
    return;
  }
  QTreeWidgetItem* item_ = itemFromIndex(index);
  iTreeViewItem* item = dynamic_cast<iTreeViewItem*>(item_);
  if (!item) return; 
  if (item->isExpanded())
    CollapseAllUnder(item);
  else
    ExpandAllUnder(item);
  
  emit itemDoubleClicked(item_, index.column()); // still need to emit the signal for other consumers!
  // i.e., the iListDataPanel
}

void iTreeView::mousePressEvent(QMouseEvent* event) {
  ++in_mouse_press;
  inherited::mousePressEvent(event); 
 --in_mouse_press; 
}

bool iTreeView::useCustomExpand() const {
  return (receivers(SIGNAL(CustomExpandFilter(iTreeViewItem*, int, bool&))) > 0);
}

void iTreeView::setColKey(int col, const KeyString& key) { 
  if ((col < 0) || (col >= columnCount())) return;
  headerItem()->setData(col, ColKeyRole, key);
}

void iTreeView::setHeaderText(int col, const String& value) { 
  headerItem()->setText(col, value);
}

int iTreeView::maxColChars(int col) { 
  if ((col < 0) || (col >= columnCount())) return -1;
  QVariant v = headerItem()->data(col, MaxColCharsRole);
  if (v.isNull()) return -1;
  else return v.toInt();
}

void iTreeView::setMaxColChars(int col, int value) { 
  if ((col < 0) || (col >= columnCount())) return;
  headerItem()->setData(col, MaxColCharsRole, value);
}

int iTreeView::colFormat(int col) { 
  if ((col < 0) || (col >= columnCount())) return 0;
  QVariant v = headerItem()->data(col, ColFormatRole);
  if (v.isNull()) return 0;
  else return v.toInt();
}

void iTreeView::scrollTo(QTreeWidgetItem* item, ScrollHint hint) {
  if (!item) return;
  inherited::scrollTo(indexFromItem(item), hint);
}

void iTreeView::setColFormat(int col, int value) { 
  if ((col < 0) || (col >= columnCount())) return;
  headerItem()->setData(col, ColFormatRole, value);
}

void iTreeView::setDecorateEnabled(bool value) {
  if (m_decorate_enabled == value) return;
  m_decorate_enabled = value;
} 

void iTreeView::setShow(taMisc::ShowMembs value) {
  if (m_show == value) return;
  Show_impl();
} 

void iTreeView::setTvFlags(int value) {
  if (tv_flags == value) return;
  tv_flags = value;
  //nothing to do yet
}

void iTreeView::Refresh_impl() {
  //note: very similar to Show_impl
  QTreeWidgetItemIterator it(this);
  QTreeWidgetItem* item_;
  while ( (item_ = *it) ) {
    iTreeViewItem* item = dynamic_cast<iTreeViewItem*>(item_);
    if (item) {
      bool hide_it = !ShowNode(item);
      bool is_hid = isItemHidden(item);
      if (hide_it != is_hid) {
        setItemHidden(item, hide_it);
      }
      // always refresh visible guys
      if (!hide_it) {
        // simulate update notification
        item->DataChanged(DCR_ITEM_UPDATED, NULL, NULL);
      }
    }
    ++it;
  }
}

void iTreeView::Show_impl() {
  //note: very similar to Refresh_impl
  QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::All);
  QTreeWidgetItem* item_;
  while ( (item_ = *it) ) {
    //note: always show QTreeWidgetItem, since we don't know what to do with them
    iTreeViewItem* item = dynamic_cast<iTreeViewItem*>(item_);
    if (item) {
      bool hide_it = !ShowNode(item);
      bool is_hid = isItemHidden(item);
      if (hide_it != is_hid) {
        setItemHidden(item, hide_it);
        // if we are making shown a hidden item, we also refresh it for safety
        if (!hide_it) {
          // simulate update notification
          item->DataChanged(DCR_ITEM_UPDATED, NULL, NULL);
        }
      }
    } else {
      setItemHidden(item_, false);
    }
    ++it;
  }
}

void iTreeView::showEvent(QShowEvent* ev) {
  inherited::showEvent(ev);
  if ((tv_flags & TV_AUTO_EXPAND) && (!(tv_flags & TV_AUTO_EXPANDED))) {
    QTimer::singleShot(150, this, SLOT(ExpandDefault()) );
    tv_flags = (TreeViewFlags)(tv_flags | TV_AUTO_EXPANDED);
  }
  
}

bool iTreeView::ShowNode(iTreeViewItem* item) const {
  if (!item) return false; // bad caller!
  return item->ShowNode(show(), m_show_context);
}

void iTreeView::FillContextMenu_pre(ISelectable_PtrList& sel_items, taiActions* menu) {
  emit FillContextMenuHookPre(sel_items, menu);
} 

void iTreeView::this_contextMenuRequested(QTreeWidgetItem* item, const QPoint & pos,
 int col ) {
  taiMenu* menu = new taiMenu(this, taiMenu::normal, taiMisc::fonSmall);
  // note: we must force the sel_item to be the item, otherwise we frequently
  // are refering to the wrong item (not what user right clicked on)
  // this seems to be the easiest way:
  this_itemSelectionChanged();  
  FillContextMenu(menu);
  if (menu->count() > 0) { //only show if any items!
    menu->exec(pos);
  }
  delete menu;
}

void iTreeView::FillContextMenu_post(ISelectable_PtrList& sel_items, taiActions* menu) {
  menu->AddSep();
  taiMenu* men_exp = menu->AddSubMenu("Expand/Collapse");
  men_exp->AddItem("Expand Default", taiMenu::normal, taiAction::action,
    this, SLOT(ExpandDefault()) );
  men_exp->AddItem("Expand All", taiMenu::normal, taiAction::action,
    this, SLOT(ExpandAll()) );
  men_exp->AddItem("Collapse All", taiMenu::normal, taiAction::action,
    this, SLOT(CollapseAll()) );
  if (sel_items.size == 1) {
    ISelectable* si = sel_items.FastEl(0);
    if (si && si->GetTypeDef()->InheritsFrom(&TA_iTreeViewItem)) {
      void* nd = si->This(); // don't need to detype, because we pass as void anyway
      men_exp->AddItem("Expand All From Here", taiMenu::normal, taiAction::ptr_act,
        this, SLOT(ExpandAllUnderInt(void*)), (void*)nd );
      men_exp->AddItem("Collapse All From Here", taiMenu::normal, taiAction::ptr_act,
        this, SLOT(CollapseAllUnderInt(void*)), (void*)nd );
    }
  }
  
  emit FillContextMenuHookPost(sel_items, menu);
} 

//NOTE: this is a widget-level guy that just forwards to our signal --
// it presumably is ALSO emitted in addition to itemSelectionChanged
void iTreeView::this_currentItemChanged(QTreeWidgetItem* curr, QTreeWidgetItem* prev) {
  iTreeViewItem* it = dynamic_cast<iTreeViewItem*>(curr); //note: we want null if curr is not itvi
//NOTE: the default QAbstractItemView guy doesn't seem to handle the statustip
// very well == it barely gets activated, only if you click an item then drag a bit --
// so we are doing it manually here
  if (it) {
    QString statustip = it->data(0, Qt::StatusTipRole).toString();
    if (parent() && !statustip.isEmpty()) {
      QStatusTipEvent tip(statustip);
      QApplication::sendEvent(parent(), &tip);
    }
  }
  // needed esp. when we call setCurrentItem(x)
  // except skip in the mouse-down routine (we get in the clicked, via mouse up)
  if (!in_mouse_press) {
    this_itemSelectionChanged();
  }
}

void iTreeView::this_itemSelectionChanged() {
  if (selectionChanging()) return; // ignore
  SelectionChanging(true, false); // not forced, because gui-driven
    sel_items.Reset();
    GetSelectedItems(sel_items);
  SelectionChanging(false, false);
}

void iTreeView::UpdateSelectedItems_impl() {
  //note: we are already guarded against spurious gui updates
  // our approach is to copy the list, then iterate all currently selected items, 
  // removing those from selection not in the list or removing from list if in
  // selection already; then we select any that remain in the list
  ISelectable_PtrList sel_items = selItems(); // copies
  QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::Selected);
  QTreeWidgetItem* item;
  int lst_idx;
  while ( (item = *it) ) {
    ISelectable* si = dynamic_cast<ISelectable*>(item);
    if (si) {
      if ((lst_idx = sel_items.FindEl(si)) >= 0) 
        sel_items.RemoveIdx(lst_idx);
      else setItemSelected(item, false); // hope this is ok while iterating!!!!
    }
    ++it;
  }
  // now, select any remaining
  for (int lst_idx = 0; lst_idx < sel_items.size; ++lst_idx) {
    ISelectable* si = sel_items.FastEl(lst_idx);
    if (si->GetTypeDef()->InheritsFrom(TA_iTreeViewItem)) { // should
      item = (iTreeViewItem*)(si->This());
      if (item) setItemSelected(item, true);
    }
  }
}

//////////////////////////
//    iTreeViewItem 	//
//////////////////////////

class DataNodeDeleter: public QObject { // enables nodes to be put on deferredDelete list
public:
  iTreeViewItem* node;
  DataNodeDeleter(iTreeViewItem* node_): QObject() {node = node_;}
  ~DataNodeDeleter() {delete node;}
};

iTreeViewItem::iTreeViewItem(taiDataLink* link_, MemberDef* md_, iTreeViewItem* node,
  iTreeViewItem* after, const String& tree_name, int dn_flags_)
:inherited(node, after)
{
  init(tree_name, link_, md_, dn_flags_);
}

iTreeViewItem::iTreeViewItem(taiDataLink* link_, MemberDef* md_, iTreeView* parent,
  iTreeViewItem* after, const String& tree_name, int dn_flags_)
:inherited(parent, after)
{
  // assume by default we want to update name if no md, and not told not to
  if (!md_ && !(dn_flags_ & DNF_NO_UPDATE_NAME))
    dn_flags_ |= DNF_UPDATE_NAME;
  init(tree_name, link_, md_, dn_flags_);
}

void iTreeViewItem::init(const String& tree_name, taiDataLink* link_, 
  MemberDef* md_, int dn_flags_) 
{
  m_md = md_;
  dn_flags = dn_flags_;
  link_->AddDataClient(this); // sets link
/*OBS: we really aren't using links
  // links get name italicized
  //TODO: to avoid creating a brand new font for each item, we could
  // get an italicized version from Tree (and everyone would share)
  if (dn_flags & DNF_IS_LINK) {
    QFont fnt(treeView()->font());
    fnt.setItalic(true);
    setData(0, Qt::FontRole, fnt);
  }
*/
  given_name = tree_name;
  setText(0, GetColText(0, tree_name));
  setDragEnabled(dn_flags & DNF_CAN_DRAG);
  setDropEnabled(!(dn_flags & DNF_NO_CAN_DROP));
  // note: lazy children moved to Decorate because uses virtuals
}

iTreeViewItem::~iTreeViewItem() {
  iTreeView* tv = treeView();
  if (tv) {
    tv->ItemDestroyingCb(this);
  }
}

bool iTreeViewItem::canAcceptDrop(const QMimeData* mime) const {
 return true;
/*obs always say yes, since we put up a drop menu, and this decode 
  // would be done all the time
  taiMimeSource* ms = taiMimeSource::New(mime);
  int ea = QueryEditActions_(ms);
  bool rval = (ea & taiClipData::EA_DROP_OPS);
  delete ms;
  return rval;*/
}

/* nnint iTreeViewItem::compare (QTreeWidgetItem* item, int col, bool ascending) const {
  // if we have a visual parent, delegate to its data link, otherwise just do the default
  iTreeViewItem* par = parent();
  if (par)  {
    int rval = par->link->CompareChildItems(this, (iTreeViewItem*)item);
    if (ascending) return rval;
    else return rval * -1;
  } else
    return QTreeWidgetItem::compare(item, col, ascending);
} */

void iTreeViewItem::CreateChildren() {
  inherited::CreateChildren();
  DecorateDataNode();
}

void iTreeViewItem::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  if (dcr != DCR_ITEM_UPDATED) return;
  if (this->dn_flags & iTreeViewItem::DNF_UPDATE_NAME) {
    taiTreeDataNode* par_nd = (taiTreeDataNode*)this->parent();
    if (par_nd) {
      par_nd->UpdateChildNames();
    } else { // a root node or list node -- just force our name to something sensible...
      String nm = link()->GetName();
      if (nm.empty())
        nm = "(" + link()->GetDataTypeDef()->name + ")";
      setName(nm); // col0, except list nodes
    }
  }
  DecorateDataNode();
}

void iTreeViewItem::DataLinkDestroying(taDataLink*) {
  delete this;
}

void iTreeViewItem::DecorateDataNode() {
  // note: first do lazy children, which was previously in ctor but can no longer
  // be because it uses virtual calls -- this is safe and conservative if children
  // are already made
  if (dn_flags & DNF_LAZY_CHILDREN) {
    enableLazyChildren();
  }

  int bmf = 0;
  int dn_flags_supported = 0;
  taiDataLink* link = this->link(); // local cache
  QIcon ic;
  if (isExpanded()) bmf |= NBF_FOLDER_OPEN;
  bool has_ic = link->GetIcon(bmf, dn_flags_supported, ic);
  //TODO (or in GetIcon somewhere) add link iconlet and any other appropriate mods
  if (has_ic)
    setIcon(0, ic);
  // fill out remaining col text and data according to key
  iTreeView* tv = treeView();
  if (!tv) return; //shouldn't happen
  // we only fiddle the font if item disabled or previously disabled
  // (otherwise, we'd be superfluously setting a Font into each item!)
  //  bool set_font = (!item_enabled);
  bool set_font = false;
  for (int i = 0; i < tv->columnCount(); ++i) {
    if (i == 0) {
      if (!set_font)
        set_font = data(0, Qt::FontRole).isValid();
      setText(0, GetColText(0, given_name));
    }
    else {
      setText(i, GetColText(i));
    }
    // font -- just for reference -- not used
//     if (set_font) {
//       if (item_enabled) {
//         // setting the font to nil causes the itemdelegate guy to use default
//         setData(i, Qt::FontRole, QVariant());
//       } else {
//         setData(i, Qt::FontRole, QVariant(tv->italicFont()));
//       }
//     }
    // then, col data, if any (empty map, otherwise)
    QMap_qstr_qvar map = tv->colDataKeys(i);
    if (!map.isEmpty()) {
      KeyString key;
      QMap_qstr_qvar::const_iterator itr;
      for (itr = map.constBegin(); itr != map.constEnd(); ++itr) {
        // remember, we used roles as map keys, and put our colkey in the value
        bool ok; // helps avoid errors, by making sure the mapkey is an int
        int role = itr.key().toInt(&ok);
        if (ok) {
          key = itr.value().toString();
          setData(i, role, link->GetColData(key, role));
        }
      }
    }
  }
  // if tree is using highlighting, then highlight according to state information
  if (tv->highlightRows()) {
    String dec_key = link->GetStateDecoKey();
    if(dec_key.nonempty()) {
      ViewColor* vc = taMisc::view_colors->FindName(dec_key);
      if(vc) {
	if(vc->use_bg)		// prefer bg color; always set bg so no conflict with type info
	  setBackgroundColor(vc->bg_color.color());
 	else if(vc->use_fg)
 	  setBackgroundColor(vc->fg_color.color());
      }
      else {
	resetBackgroundColor();//setHighlightIndex(0);
      }
    }
    else {
      resetBackgroundColor();//setHighlightIndex(0);
    }
  }
  // if decoration enabled, then decorate away
  if (tv->decorateEnabled() && (taMisc::color_hints & taMisc::CH_BROWSER)) {
    String dec_key = link->GetTypeDecoKey(); // nil if none
    if (dec_key.nonempty()) {
      ViewColor* vc = taMisc::view_colors->FindName(dec_key);
      if(vc) {
	if(vc->use_fg)
	  setTextColor(vc->fg_color.color());
 	else if(vc->use_bg)
 	  setTextColor(vc->bg_color.color()); // always set text, even if bg, so no conflict with state info
      }
    }
  }
}

void iTreeViewItem::dropped(const QMimeData* mime, const QPoint& pos, 
    int key_mods, WhereIndicator where) 
{
  DropHandler(mime, pos, key_mods, where);

}

void iTreeViewItem::QueryEditActionsS_impl_(int& allowed, int& forbidden,
  GuiContext sh_typ) const 
{
  if (dn_flags & DNF_IS_MEMBER) {
    forbidden |= (taiClipData::EA_CUT | taiClipData::EA_DELETE);
  }
  IObjectSelectable::QueryEditActionsS_impl_(allowed, forbidden, sh_typ);
}

void iTreeViewItem::FillContextMenu_impl(taiActions* menu,
  GuiContext sh_typ) {
  //taiAction* mel =
  menu->AddItem("Find from here (Alt+F)...", taiMenu::use_default,
		taiAction::men_act, treeView(), SLOT(mnuFindFromHere(taiAction*)), this);
  // note: this causes it to always search from the first one entered!  need to trap
  // specific keyboard input
//   menu->AddItem("Find from here...", taiAction::men_act, treeView(),
// 		SLOT(mnuFindFromHere(taiAction*)), this, QKeySequence("Alt+F"));
  IObjectSelectable::FillContextMenu_impl(menu, sh_typ);
}

const String iTreeViewItem::GetColText(int col, const String& def) const
{
  iTreeView* tv = treeView();
  taiDataLink* link = this->link(); // local cache
  String rval;
  if (tv && link) {  
    KeyString key = tv->colKey(col);
    if (key.length() > 0) { // no point if no key
      const int max_chars = tv->maxColChars(col); // -1 if no limit
      const int col_format = tv->colFormat(col); // 0 if none
      if (col_format & iTreeView::CF_ELIDE_TO_FIRST_LINE)
        rval = link->GetColText(key).elidedToFirstLine();
      else
        rval = link->GetColText(key).elidedTo(max_chars);
    } else if (col == 0) { // use default name for col 0
      rval = link->GetDisplayName();
    }
  }
  if (rval.empty())
    rval = def;
  return rval;
}

ISelectableHost* iTreeViewItem::host() const {
  iTreeView* tv = treeView();
  return (tv) ? (ISelectableHost*)tv : NULL;
}

void iTreeViewItem::itemExpanded(bool value) {
  inherited::itemExpanded(value); // creates children
  DecorateDataNode();
}

void* iTreeViewItem::linkData() const {
  return (m_link) ? m_link->data() : NULL;
}

void iTreeViewItem::moveChild(int fm_idx, int to_idx) {
  if (fm_idx == to_idx) return; // DOH!
  // if the fm is prior to to, we need to adjust index (for removal)
  if (fm_idx < to_idx) --to_idx;
  QTreeWidgetItem* tak = takeChild(fm_idx);
  insertChild(to_idx, tak); 
  // need to re-assert the hidden, because everything goes visible after the take
  iTreeView* tv = treeView();
  QTreeWidgetItemIterator it(this);
  QTreeWidgetItem* item_;
  while ( (item_ = *it) ) {
    iTreeViewItem* item = dynamic_cast<iTreeViewItem*>(item_);
    if (item) {
      bool hide_it = !tv->ShowNode(item);
      bool is_hid = item->isHidden();
      if (hide_it != is_hid) {
        item->setHidden(hide_it);
      }
    }
    ++it;
  }
}

ISelectable* iTreeViewItem::par() const {
  iTreeViewItem* rval = parent();
  if (rval) return rval;
  else return NULL;
}

iTreeViewItem* iTreeViewItem::parent() const {
  return dynamic_cast<iTreeViewItem*>(inherited::parent());
}

void iTreeViewItem::setName(const String& value) {
  given_name = value;
  this->setText(0, value);
}

bool iTreeViewItem::ShowNode_impl(int show, const String&) const 
{ 
  // if not a member, then we just always show, since it must be a list element,
  // or standalone item whose visibility will be controlled by a parent member somewhere
  if (!m_md) return true;
  //TODO: note, context is ignored for now
  return m_md->ShowMember((taMisc::ShowMembs)show, TypeItem::SC_TREE);
}

void iTreeViewItem::swapChildren(int n1_idx, int n2_idx) {
  // we move higher to lower, then lower is next after, and moved to higher
  if (n1_idx > n2_idx) {int t = n1_idx; n1_idx = n2_idx; n2_idx = t;}
  moveChild(n2_idx, n1_idx);
  moveChild(n1_idx + 1, n2_idx);
}

iTreeView* iTreeViewItem::treeView() const {
  iTreeView* rval = dynamic_cast<iTreeView*>(treeWidget());
  return rval;
}


//////////////////////////////////
// 	taiListDataNode 	//
//////////////////////////////////

taiListDataNode::taiListDataNode(int num_, iListDataPanel* panel_,
   taiDataLink* link_, iTreeView* parent_, taiListDataNode* after, int dn_flags_)
:inherited(link_, NULL, parent_, after, String(num_), (dn_flags_ | DNF_IS_LIST_NODE))
{
  num = num_;
  panel = panel_;
  setData(0, Qt::TextAlignmentRole, Qt::AlignRight);
}

taiListDataNode::~taiListDataNode() {
}


bool taiListDataNode::operator<(const QTreeWidgetItem& item) const
{ //NOTE: it was tried to set display data as an int QVariant, but sorting was still lexographic
  QTreeWidget* tw = treeWidget();
  if (!tw) return false; // shouldn't happen
  int col = tw->sortColumn();
  if (col > 0)
    return inherited::operator<(item);
  else {
    taiListDataNode* ldn = (taiListDataNode*)&item;
    return (num < ldn->num);
  } 
}

void taiListDataNode::DecorateDataNode() {
  inherited::DecorateDataNode();
  setText(0, String(num)); // in case changed via renumber
}

taiDataLink* taiListDataNode::par_link() const {
  // in case we decide to support trees in list views, check for an item parent:
  taiDataLink* rval = inherited::par_link();
  if (rval) return rval;
  if (panel) return panel->link();
  return NULL;
}

void taiListDataNode::setName(const String& value) {
  if (columnCount() >= 2) // s/always be true!
    this->setText(1, value);
}

QString taiListDataNode::text(int col) const {
  if (col > 0)
    return inherited::text(col);
  else
    return QString::number(num);
}


//////////////////////////////////
// 	taiTreeDataNode 	//
//////////////////////////////////

taiTreeDataNode::taiTreeDataNode(taiDataLink* link_, MemberDef* md_, taiTreeDataNode* parent_,
  taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited(link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

taiTreeDataNode::taiTreeDataNode(taiDataLink* link_, MemberDef* md_, iTreeView* parent_,
  taiTreeDataNode* last_child_, const String& tree_name, int dn_flags_)
:inherited(link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void taiTreeDataNode::init(taiDataLink* link_, int dn_flags_) {
  last_child_node = NULL;
  last_member_node = NULL;
}


taiTreeDataNode::~taiTreeDataNode() {
}

void taiTreeDataNode::CreateChildren_impl() {
//NOTE: keep willHaveChildren_impl in sync with this code
  MemberSpace* ms = &(link()->GetDataTypeDef()->members);
  iTreeView* tree = treeView(); //cache
  for (int i = 0; i < ms->size; ++ i) {
    MemberDef* md = ms->FastEl(i);
/*TODO: replace filters, or nuke    // check for member/type-based filter
    if (tree->HasFilter(md->type)) continue;
    if (tree->HasFilter(md)) continue; */
    // we make everything that isn't NO_SHOW, then hide if not visible now
    if (!md->ShowMember(taMisc::ALL_MEMBS, TypeItem::SC_TREE)) continue;
    TypeDef* typ = md->type;
    void* el = md->GetOff(linkData()); //note: GetDataLink automatically derefs typ and el if pointers
    taiDataLink* dl = taiViewType::StatGetDataLink(el, typ);
    //note: we still can't get links for some types, ex. ptrs to NULL
    if (dl) {
      String tree_nm = md->GetLabel();
      last_child_node = dl->CreateTreeDataNode(md, this, last_child_node, tree_nm,
        (iTreeViewItem::DNF_IS_MEMBER));
      if (!tree->ShowNode(last_child_node))
        tree->setItemHidden(last_child_node, true);
    }
  }
  last_member_node = last_child_node; //note: will be NULL if no members issued
}

void taiTreeDataNode::willHaveChildren_impl(bool& will) const {
//NOTE: keep CreateChildren_impl in sync with this code
//NOTE: this typically doesn't execute for listish nodes with children
  MemberSpace* ms = &(link()->GetDataTypeDef()->members);
  for (int i = 0; i < ms->size; ++ i) {
    MemberDef* md = ms->FastEl(i);
    //NOTE: this code is only valid for the **current** view state
    // lazy children would need to be rerun for all nodes if view state changed
    // we make everything that isn't NO_SHOW, then hide if not visible now
    if (!md->ShowMember(taMisc::USE_SHOW_GUI_DEF, TypeItem::SC_TREE)) continue;
    will = true;
    break;
  }
  if (!will)
    inherited::willHaveChildren_impl(will);
}

taiTreeDataNode* taiTreeDataNode::FindChildForData(void* data, int& idx) {
  for (int i = 0; i < childCount(); ++i) {
      taiTreeDataNode* rval = (taiTreeDataNode*)child(i);
      if (rval->link()->data() == data) {
        idx = i;
        return rval;
      }
  }
  idx = -1;
  return NULL;
}

/*nn taiDataLink* taiTreeDataNode::par_link() const {
  taiTreeDataNode* par = parent();
  return (par) ? par->link() : NULL;
} */

/*MemberDef* taiTreeDataNode::par_md() const {
//TODO: this is a fairly broken concept -- i don't even think the clip system
// needs to know about par mds
//TODO: following is the gui one, we should be returning the data one
  taiTreeDataNode* par = parent();
  return (par) ? par->md() : NULL;
} */


//////////////////////////////////
//   tabTreeDataNode 		//
//////////////////////////////////

tabTreeDataNode::tabTreeDataNode(tabDataLink* link_, MemberDef* md_, taiTreeDataNode* parent_,
  taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((taiDataLink*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

tabTreeDataNode::tabTreeDataNode(tabDataLink* link_, MemberDef* md_, iTreeView* parent_,
  taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((taiDataLink*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void tabTreeDataNode::init(tabDataLink* link_, int dn_flags_) {
}

tabTreeDataNode::~tabTreeDataNode()
{
}


//////////////////////////////////
//   tabParTreeDataNode 	//
//////////////////////////////////

tabParTreeDataNode::tabParTreeDataNode(tabODataLink* link_, MemberDef* md_,
  taiTreeDataNode* parent_, taiTreeDataNode* last_child_,
    const String& tree_name, int dn_flags_)
:inherited((tabDataLink*)link_, md_, parent_, last_child_, tree_name, 
  dn_flags_ | DNF_LAZY_CHILDREN)
{
  init(link_, dn_flags_);
}

tabParTreeDataNode::tabParTreeDataNode(tabODataLink* link_, MemberDef* md_, iTreeView* parent_,
  taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((tabDataLink*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void tabParTreeDataNode::init(tabODataLink* link_, int dn_flags_) {
  last_list_items_node = NULL;
}

tabParTreeDataNode::~tabParTreeDataNode()
{
}

void tabParTreeDataNode::AssertLastListItem() {
  void* el = list()->Peek_();
  if (el == NULL) {
    last_list_items_node = last_member_node;
    return;
  }
  int idx;
  last_list_items_node = this->FindChildForData(el, idx);
}

void tabParTreeDataNode::CreateChildren_impl() {
  inherited::CreateChildren_impl();
  String tree_nm;
  taList_impl* list = this->list(); // cache
  for (int i = 0; i < list->size; ++i) {
    taBase* el = (taBase*)list->FastEl_(i);
    if (!el) continue; // generally shouldn't happen
    TypeDef* typ = el->GetTypeDef();
    taiDataLink* dl = taiViewType::StatGetDataLink(el, typ);
    if (!dl) continue; // shouldn't happen... unless null

    tree_nm = dl->GetDisplayName();
    if (tree_nm.empty()) {
      tree_nm = link()->AnonymousItemName(typ->name, i);
    }
    int dn_flags_tmp = DNF_UPDATE_NAME | DNF_CAN_BROWSE | DNF_CAN_DRAG;
    // check if this is a link
    taBase* own = el->GetOwner(); //note: own=NULL generally means <taOBase items
    if (own && (own != list))
      dn_flags_tmp |= DNF_IS_LINK;
    last_child_node = dl->CreateTreeDataNode((MemberDef*)NULL, this, last_child_node, tree_nm, dn_flags_tmp);
  }

  last_list_items_node = last_child_node;
}

void tabParTreeDataNode::willHaveChildren_impl(bool& will) const {
  if (list()->size > 0) will = true;
  if (!will)
    inherited::willHaveChildren_impl(will);
}

taiTreeDataNode* tabParTreeDataNode::CreateListItem(taiTreeDataNode* par_node,
  taiTreeDataNode* after, taBase* el) 
{
  if (!el) return NULL;
  taList_impl* list = this->list(); // cache
  TypeDef* typ = el->GetTypeDef();
  taiDataLink* dl = taiViewType::StatGetDataLink(el, typ);
  if (!dl) return NULL; // shouldn't happen unless null...
  //note: we don't make name because it is updated anyway
  int dn_flags_tmp = DNF_UPDATE_NAME | DNF_CAN_BROWSE | DNF_CAN_DRAG;
  // check if this is a link
  taBase* own = el->GetOwner(); //note: own=NULL generally means <taOBase items
  if (own && (own != list))
    dn_flags_tmp |= DNF_IS_LINK;
  taiTreeDataNode* dn = dl->CreateTreeDataNode((MemberDef*)NULL, 
    par_node, after, _nilString, dn_flags_tmp);
  return dn;
}

void tabParTreeDataNode::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  if (!this->children_created) {
    if ((dcr == DCR_LIST_ITEM_INSERT) || (dcr == DCR_LIST_ITEM_REMOVE))
      UpdateLazyChildren(); // updates
    return;
  }
  int idx;
  switch (dcr) {
  case DCR_LIST_INIT: break;
  case DCR_LIST_ITEM_INSERT: {	// op1=item, op2=item_after, null=at beginning
    taiTreeDataNode* after_node = this->FindChildForData(op2_, idx); //null if not found
    if (!after_node) after_node = last_member_node; // insert, after
    taiTreeDataNode* new_node = CreateListItem(this, after_node, (taBase*)op1_);
    iTreeView* tv = treeView();
//EVIL    tv->expandItem(new_node);
    // only scroll to it if parent is visible
    if (isExpandedLeaf())
      tv->scrollTo(new_node);
  }
    break;
  case DCR_LIST_ITEM_REMOVE: {	// op1=item -- note, item not DisOwned yet, but has been removed from list
    taiTreeDataNode* gone_node = this->FindChildForData(op1_, idx); //null if not found
    if (gone_node) delete gone_node; // goodbye!
  }
    break;
  case DCR_LIST_ITEM_MOVED: {	// op1=item, op2=item_after, null=at beginning
    int fm_idx;
    taiTreeDataNode* moved_node = this->FindChildForData(op1_, fm_idx); //null if not found
    if (!moved_node) break; // shouldn't happen
    int to_idx;
    taiTreeDataNode* after_node = this->FindChildForData(op2_, to_idx); //null if not found
    if (!after_node) to_idx = indexOfChild(last_member_node); // insert, after
    ++to_idx; // after
    moveChild(fm_idx, to_idx);
    // only scroll to it if parent is visible
    if (isExpandedLeaf())
      treeView()->scrollTo(moved_node);
  }
    break;
  case DCR_LIST_ITEMS_SWAP: {	// op1=item1, op2=item2
    int n1_idx, n2_idx;
    taiTreeDataNode* node1 = this->FindChildForData(op1_, n1_idx); //null if not found
    taiTreeDataNode* node2 = this->FindChildForData(op2_, n2_idx); //null if not found
    if ((!node1) || (!node2)) break; // shouldn't happen
    swapChildren(n1_idx, n2_idx); 
  }
    break;
  case DCR_LIST_SORTED: {	// no ops
    int nd_idx; // index of the node
    taList_impl* list = this->list(); // cache
    for (int i = 0; i < list->size; ++i) {
      taBase* tab = (taBase*)list->FastEl_(i);
      FindChildForData(tab, nd_idx);
      if (i == nd_idx) continue; // in right place already
      moveChild(nd_idx, i);
    }
  }  break;
  default: return; // don't update names
  }
  UpdateListNames();
}

void tabParTreeDataNode::UpdateChildNames() {
  inherited::UpdateChildNames();
  UpdateListNames();
}

void tabParTreeDataNode::UpdateListNames() {
  String tree_nm;
  taList_impl* list = this->list(); //cache
  for (int i = 0; i < list->size; ++i) {
    // the subgroups are themselves taGroup items
    TypeDef* typ;
    void* el = list->GetTA_Element(i, typ); // gets the item, and its TypeDef
    if (!typ) continue; //TODO: maybe we should put a marker item in list???
    // if we get a taBase item, the type might only be the base type, not the derived type of the item
    // so we cast the item, and then grab the exact type right from the item
    if (typ->InheritsFrom(&TA_taBase)) {
        typ = ((taBase*)el)->GetTypeDef();
    }
    taiDataLink* dl = taiViewType::StatGetDataLink(el, typ);
    if (!dl) continue; // shouldn't happen unless null...

    tree_nm = dl->GetDisplayName();
    if (tree_nm.empty()) {
      tree_nm = link()->AnonymousItemName(typ->name, i);
    }
    int idx;
    taiTreeDataNode* node1 = this->FindChildForData(el, idx); //null if not found
    if (node1 != NULL)
//TEMP      node1->setText(0, tree_nm);
node1->DecorateDataNode();
  }
}

//////////////////////////////////
//   tabDefChildTreeDataNode 	//
//////////////////////////////////

void tabDefChildRef::DataDataChanged(taDataLink*, int dcr, void* op1, void* op2) {
  if (m_own) m_own->DefChild_DataChanged(dcr, op1, op2);
}

void tabDefChildRef::DataLinkDestroying(taDataLink* dl) {
//note: don't need to do anything, since everything is explicitly owned/nested
}

tabDefChildTreeDataNode::tabDefChildTreeDataNode(tabODataLink* link_, MemberDef* md_,
  taiTreeDataNode* parent_, taiTreeDataNode* last_child_,
  const String& tree_name, int dn_flags_)
:inherited(link_, md_, parent_, last_child_, tree_name, 
  dn_flags_ | DNF_LAZY_CHILDREN)
{
  init(link_, dn_flags_);
}

tabDefChildTreeDataNode::tabDefChildTreeDataNode(tabODataLink* link_, MemberDef* md_,
  iTreeView* parent_, taiTreeDataNode* last_child_,
  const String& tree_name, int dn_flags_)
:inherited(link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void tabDefChildTreeDataNode::init(tabODataLink*, int) {
  m_def_child.Init(this, list());
}

tabDefChildTreeDataNode::~tabDefChildTreeDataNode()
{
}

void tabDefChildTreeDataNode::DefChild_DataChanged(int dcr, void* op1, void* op2) {
  // we only pass on the List notifies
  if ((!(dcr >= DCR_LIST_MIN) && (dcr <= DCR_LIST_MAX))) return;
  DataChanged(dcr, op1, op2);
}


//////////////////////////////////
//   tabListTreeDataNode 	//
//////////////////////////////////

tabListTreeDataNode::tabListTreeDataNode(tabListDataLink* link_, MemberDef* md_,
  taiTreeDataNode* parent_, taiTreeDataNode* last_child_,
  const String& tree_name, int dn_flags_)
:inherited((tabODataLink*)link_, md_, parent_, last_child_, tree_name, 
  dn_flags_ | DNF_LAZY_CHILDREN)
{
  init(link_, dn_flags_);
}

tabListTreeDataNode::tabListTreeDataNode(tabListDataLink* link_, MemberDef* md_,
  iTreeView* parent_, taiTreeDataNode* last_child_,
  const String& tree_name, int dn_flags_)
:inherited((tabODataLink*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void tabListTreeDataNode::init(tabListDataLink*, int) {
}

tabListTreeDataNode::~tabListTreeDataNode()
{
}

//////////////////////////////////
//   taiGroupTreeDataNode 	//
//////////////////////////////////

tabGroupTreeDataNode::tabGroupTreeDataNode(tabGroupDataLink* link_, MemberDef* md_,
  taiTreeDataNode* parent_, taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((tabListDataLink*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

tabGroupTreeDataNode::tabGroupTreeDataNode(tabGroupDataLink* link_, MemberDef* md_, iTreeView* parent_,
  taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((tabListDataLink*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void tabGroupTreeDataNode::init(tabGroupDataLink* link_, int dn_flags_) {
}

tabGroupTreeDataNode::~tabGroupTreeDataNode()
{
}

void tabGroupTreeDataNode::CreateChildren_impl() {
  inherited::CreateChildren_impl();
  String tree_nm;
  for (int i = 0; i < tadata()->gp.size; ++i) {
    // the subgroups are themselves taGroup items
    TypeDef* typ;
    void* el = tadata()->gp.GetTA_Element(i, typ); // gets the item, and its TypeDef
    if (!typ) continue; //TODO: maybe we should put a marker item in list???
    // if we get a taBase item, the type might only be the base type, not the derived type of the item
    // so we cast the item, and then grab the exact type right from the item
    if (typ->InheritsFrom(&TA_taBase)) {
        typ = ((taBase*)el)->GetTypeDef();
    }
    taiDataLink* dl = taiViewType::StatGetDataLink(el, typ);
    if (!dl) continue; // shouldn't happen unless null...

    tree_nm = dl->GetDisplayName();
    if (tree_nm.empty()) {
      tree_nm = "(subgroup " + String(i) + ")";
    } else {
      tree_nm = tree_nm + " subgroup";
    }
    last_child_node = dl->CreateTreeDataNode(NULL, this, last_child_node, tree_nm,
      (iTreeViewItem::DNF_UPDATE_NAME | iTreeViewItem::DNF_CAN_DRAG));
    //TODO: maybe this isn't right -- we really want the root group's md, because that is the only
          // one that has the defs
  }
}

taiTreeDataNode* tabGroupTreeDataNode::CreateSubGroup(taiTreeDataNode* after_node,
  void* el) 
{
  taSubGroup* gp = &tadata()->gp;
  TypeDef* typ = gp->GetElType();
  // the subgroups are themselves taGroup items
  if (!typ) return NULL; //TODO: maybe we should put a marker item in list???
  // if we get a taBase item, the type might only be the base type, not the derived type of the item
  // so we cast the item, and then grab the exact type right from the item
  if (typ->InheritsFrom(&TA_taBase)) {
      typ = ((taBase*)el)->GetTypeDef();
  }
  taiDataLink* dl = taiViewType::StatGetDataLink(el, typ);
  if (!dl) return NULL; // shouldn't happen unless null...

  taiTreeDataNode* dn = dl->CreateTreeDataNode(NULL, this, after_node, "",
    (iTreeViewItem::DNF_UPDATE_NAME | iTreeViewItem::DNF_CAN_DRAG));
     //gets its name in rename
  return dn;
}

void tabGroupTreeDataNode::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  if (!this->children_created) {
    if ((dcr == DCR_GROUP_INSERT) || (dcr == DCR_GROUP_REMOVE))
      UpdateLazyChildren(); // updates
    return;
  }
  AssertLastListItem();
  int idx;
  switch (dcr) {
  case DCR_GROUP_INSERT: {	// op1=item, op2=item_after, null=at beginning
    taiTreeDataNode* after_node = this->FindChildForData(op2_, idx); //null if not found
    if (after_node == NULL) after_node = last_list_items_node; // insert, after lists
    taiTreeDataNode* new_node = CreateSubGroup(after_node, op1_);
    // only scroll to it if parent is visible
    if (isExpandedLeaf())
      treeView()->scrollTo(new_node);
  }
    break;
  case DCR_GROUP_REMOVE: {	// op1=item -- note, item not DisOwned yet, but has been removed from list
    taiTreeDataNode* gone_node = this->FindChildForData(op1_, idx); //null if not found
    if (gone_node) delete gone_node; // goodbye!
  }
    break;
  case DCR_GROUP_MOVED: {	// op1=item, op2=item_after, null=at beginning
    int fm_idx;
    taiTreeDataNode* moved_node = this->FindChildForData(op1_, fm_idx); //null if not found
    if (!moved_node) break; // shouldn't happen
    int to_idx;
    taiTreeDataNode* after_node = this->FindChildForData(op2_, to_idx); //null if not found
    if (!after_node) to_idx = indexOfChild(last_list_items_node); // insert, after
    ++to_idx; // after
    moveChild(fm_idx, to_idx);
    // only scroll to it if parent is visible
    if (isExpandedLeaf())
      treeView()->scrollTo(moved_node);
  }
    break;
  case DCR_GROUPS_SWAP: {	// op1=item1, op2=item2
    int n1_idx, n2_idx;
    taiTreeDataNode* node1 = this->FindChildForData(op1_, n1_idx); //null if not found
    taiTreeDataNode* node2 = this->FindChildForData(op2_, n2_idx); //null if not found
    if ((!node1) || (!node2)) break; // shouldn't happen
    swapChildren(n1_idx, n2_idx); 
  }
    break;
  case DCR_GROUPS_SORTED: {	// no ops
    int gp0_idx = indexOfChild(last_list_items_node) + 1; // valid if llin=NULL
    int nd_idx; // index of the node
    taGroup_impl* gp = this->tadata(); // cache
    for (int i = 0; i < gp->gp.size; ++i) {
      taBase* tab = (taBase*)gp->FastGp_(i);
      FindChildForData(tab, nd_idx);
      if ((gp0_idx+i) == nd_idx) continue; // in right place already
      moveChild(nd_idx, (gp0_idx+i));
    }
  }  break;
  default: return; // don't update names
  }
  UpdateGroupNames();
}

void tabGroupTreeDataNode::UpdateChildNames() {
  inherited::UpdateChildNames();
  UpdateGroupNames();
}

void tabGroupTreeDataNode::UpdateGroupNames() {
  String tree_nm;
  for (int i = 0; i < tadata()->gp.size; ++i) {
    // the subgroups are themselves taGroup items
    TypeDef* typ;
    void* el = tadata()->gp.GetTA_Element(i, typ); // gets the item, and its TypeDef
    if (!typ) continue; //TODO: maybe we should put a marker item in list???
    // if we get a taBase item, the type might only be the base type, not the derived type of the item
    // so we cast the item, and then grab the exact type right from the item
    if (typ->InheritsFrom(&TA_taBase)) {
        typ = ((taBase*)el)->GetTypeDef();
    }
    taiDataLink* dl = taiViewType::StatGetDataLink(el, typ);
    if (!dl) continue; // shouldn't happen unless null...

    tree_nm = dl->GetDisplayName();
    if (tree_nm.empty()) {
      tree_nm = "(subgroup " + String(i) + ")";
    } else {
      tree_nm = tree_nm + " subgroup";
    }
    int idx;
    iTreeViewItem* node1 = this->FindChildForData(el, idx); //null if not found
    if (node1 != NULL)
      node1->setText(0, tree_nm);
  }
}

void tabGroupTreeDataNode::willHaveChildren_impl(bool& will) const {
  if (tadata()->gp.size > 0) will = true;
  if (!will)
    inherited::willHaveChildren_impl(will);
}


//////////////////////////////////
//   iSearchDialog		//
//////////////////////////////////

class QSleazyFakeTreeWidget: public QTreeWidget {
public:
    using QTreeWidget::sizeHintForColumn;
};

iSearchDialog* iSearchDialog::New(int ft, iMainWindowViewer* par_window_) 
{
  iSearchDialog* rval = new iSearchDialog(par_window_);
  rval->setFont(taiM->dialogFont(ft));
  rval->Constr();
  return rval;
}

iSearchDialog::iSearchDialog(iMainWindowViewer* par_window_)
:inherited(par_window_, Qt::WindowStaysOnTopHint)
{
  init();
}

iSearchDialog::~iSearchDialog() {
  setRoot(NULL, false);
}

void iSearchDialog::init() {
  m_options = SO_DEF_OPTIONS;
  m_changing = 0;
  m_stop = false;
  for (int i = 0; i < num_sorts; ++i) 
    m_sorts[i] = -1;
  m_items.InitLinks();
  m_items.NewCol(DataCol::VT_INT, "row");
  m_items.NewCol(DataCol::VT_INT, "level");
  m_items.NewCol(DataCol::VT_STRING, "headline");
  m_items.NewCol(DataCol::VT_STRING, "href");
  m_items.NewCol(DataCol::VT_STRING, "desc");
  m_items.NewCol(DataCol::VT_STRING, "hits");
  m_items.NewCol(DataCol::VT_INT, "relev");
  setSizeGripEnabled(true);
  resize(taiM->dialogSize(taiMisc::hdlg_s)); // don't hog too much screen size
}

void iSearchDialog::Constr() {
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(taiM->vsep_c);
  layOuter->setSpacing(taiM->vspc_c); 
  
  TypeDef* typ = TA_iSearchDialog.sub_types.FindName("SearchOptions");
  bbOptions = new taiBitBox(true, typ, NULL, NULL, this);
  bbOptions->GetImage(m_options);
  layOuter->addWidget(bbOptions->GetRep());

  QHBoxLayout* lay = new QHBoxLayout();
  lay->setMargin(0);
  lay->setSpacing(0);
  search = new iLineEdit(this);
  search->setToolTip("Enter text to search for in item names, descriptions, and contents.");
  lay->addWidget(search, 1);
  btnGo = new QToolButton(this);
  btnGo->setText("&Go");
//TODO: trap Enter so user can hit Enter in field  ((QToolButton*)btnGo)->setDefault(true);
  lay->addWidget(btnGo);
  lay->addSpacing(taiM->vsep_c);
  btnStop = new QToolButton(this);
  btnStop->setText("X");
  btnStop->setToolTip("stop search");
  lay->addWidget(btnStop);
  lay->addSpacing(taiM->hspc_c); 
  layOuter->addLayout(lay);
  
  results = new iTextBrowser(this);
  layOuter->addWidget(results, 1); // results is item to expand in host
  results->setHtml("Enter search words in the box above.<br>Enclose phrases in \" (quotation marks).<br>You can exclude items by preceding a search word or phrase with - (minus).<br>You can change the sort order by clicking on the column header link.");
  
  status_bar = new QStatusBar(this);
  layOuter->addWidget(status_bar);

  connect(search, SIGNAL(returnPressed()), this, SLOT(go_clicked()) );
  connect(btnGo, SIGNAL(clicked()), this, SLOT(go_clicked()) );
  connect(btnStop, SIGNAL(clicked()), this, SLOT(stop_clicked()) );
  connect(results, SIGNAL(setSourceRequest(iTextBrowser*, const QUrl&, bool&)),
    this, SLOT(results_setSourceRequest(iTextBrowser*, const QUrl&, bool&)) );
  connect(results, SIGNAL(highlighted(const QString&)),
    status_bar, SLOT(message(const QString&)) );
  search->setFocus();
}

void iSearchDialog::AddItem(const String& headline, const String& href,
    const String& desc, const String& hits, int level, int relev)
{//note: newlines just to help make resulting str readable in debugger, etc.
  m_items.AddBlankRow();
  m_items.SetVal(m_row++, col_row, -1);
  m_items.SetVal(level, col_level, -1);
  m_items.SetVal(headline, col_headline, -1);
  m_items.SetVal(href, col_href, -1);
  m_items.SetVal(desc, col_desc, -1);
  m_items.SetVal(hits, col_hits, -1);
  m_items.SetVal(relev, col_relev, -1);
}

void iSearchDialog::DataLinkDestroying(taDataLink* dl) {
  Reset();
  RootSet(NULL);
}

void iSearchDialog::EndSection()
{
}

void iSearchDialog::FindNext() {
//TODO
}

void iSearchDialog::go_clicked() {
  Search();
}

void iSearchDialog::results_setSourceRequest(iTextBrowser* src,
  const QUrl& url, bool& cancel) 
{
  if ((url.scheme() == "sort")) {
    int col = url.path().toInt();
    setFirstSort(col);
    Render();
  } 
  else { // unknown, so forward to global, which is iMainWindowViewer::taUrlHandler
    QDesktopServices::openUrl(url); 
  }
  cancel = true;
  //NOTE: we never let results call its own setSource because we don't want
  // link clicking to cause us to change our source page
}

void iSearchDialog::RootSet(taiDataLink* root) {
  String cap = "Find: ";
  if (root) {
    cap += root->GetPath();
  }
  cap += " - " + taMisc::app_name;
  setWindowTitle(cap);
}

void iSearchDialog::ParseSearchString() {
  m_targets.Reset();
  m_kickers.Reset();
  String s = trim(String(search->text()));
  while (s.nonempty()) {
    bool k = false;
    // look for qualifier

    char c = s[0];
    if (c == '-') {
      k = true;
      s = s.from(1);
    } else if (c == '+') {
      s = s.from(1);
    }
    if (s.empty()) break;
    // look for quoted guys
    c = s[0];
    String t;
    if (c == '"') {
      t = s.after('"');
      if (t.contains('"')) {
        t = t.before('"');
        s = t.after('"');
      } else {
        s = _nilString;
      }
    } else if (s.contains(' ')) {
      t = s.before(" ", 1);
      s = s.after(" ", 1);
    } else {
      t = s;
      s = _nilString;
    }
    if (t.nonempty()) {
      if (k) m_kickers.AddUnique(t);
      else   m_targets.AddUnique(t);
    }
    s = triml(s);
  }
}

void iSearchDialog::Search() {
  if (!link()) return;
  // get the latest options
  bbOptions->GetValue(m_options);
  ParseSearchString();
  taMisc::Busy(true);
  Start();
  link()->Search(this);
  End();
  taMisc::Busy(false);
}

void iSearchDialog::Reset()
{
  m_stop = false;
  m_items.ResetData();
  m_row = 0;
  results->setText("");
  taiMisc::ProcessEvents();
}

void iSearchDialog::Start()
{
  Reset();
}

void iSearchDialog::End()
{
  Render();
}

void iSearchDialog::Render()
{
  taMisc::Busy(true);
  if (m_sorts[0] != -1) {
    m_items.Sort(m_sorts[0], true, m_sorts[1],
      true, m_sorts[2], true);
  }
  src = "<table border=1><tr><th>";
  if (m_sorts[0] == col_level) src += "nest";
  else src += "<a href=sort:" + String(col_level) + ">nest</a>";
  src += "</th><th>";
  if (m_sorts[0] == col_headline) src += "item";
  else src += "<a href=sort:" + String(col_headline) + ">item</a>";
  src += "</th><th>hits</th></tr>";
  for (int i = 0; i < m_items.rows; ++i) {
    int level = m_items.GetValAsInt(col_level, i);
    String headline =  m_items.GetValAsString(col_headline, i);
    String href =  m_items.GetValAsString(col_href, i);
    String desc =  m_items.GetValAsString(col_desc, i);
    String hits =  m_items.GetValAsString(col_hits, i);
    //int relev = m_items.GetValAsInt(col_relev, i);
    RenderItem(level, headline, href, desc, hits);
  }  
  src += "</table>";
  results->setHtml(src);
  taMisc::Busy(false);
}

void iSearchDialog::RenderItem(int level, const String& headline,
  const String& href, const String& desc, const String& hits)
{
  STRING_BUF(b, 200);
  b += "<tr><td>" + String(level) + "</td><td>";
  // item
  b += "<a href=\"" + href + "\">" + headline + "</a>";
  if (desc.nonempty()) {
  //TODO: need to somehow escape html nasties in this!!!
    b += "<br>" + desc ;
  }
  b += "</td><td>";
  // hits
  b += hits; // note: this will already have highlighting, breaks etc.
  b += "</td></tr>";
  src.cat(b);
}

void iSearchDialog::StartSection(const String& sec_name)
{
}


bool iSearchDialog::setFirstSort(int col) {
  if ((col < 0) || (col >= num_cols)) return false;
  if (m_sorts[0] == col) return false;
  // push down, also nuking any existing col guys (don't sort twice on same col)
  for (int i = num_sorts - 1; i >= 1; --i) {
    if (m_sorts[i-1] == col) m_sorts[i-1] = -1;
    m_sorts[i] = m_sorts[i-1];
  }
  m_sorts[0] = col;
  return true;
}

void iSearchDialog::setRoot(taiDataLink* root, bool update_gui) {
  if (link() != root) {
    if (link()) link()->RemoveDataClient(this); 
    if (root) root->AddDataClient(this);
  }
  if (update_gui)
    RootSet(root);
}

void iSearchDialog::setSearchStr(const String& srch_str) {
  stop();
  search->setText(srch_str);
  if(srch_str.nonempty())
    Search();
}

String iSearchDialog::searchStr() const {
  return search->text();
}

bool iSearchDialog::stop() const {
  taiMisc::ProcessEvents();
//TEMP
if (m_stop)
taMisc::Info("should be stopping...");
  return m_stop;
}

void iSearchDialog::stop_clicked() {
  m_stop = true;
}


//////////////////////////////////
//   taBase			//
//////////////////////////////////

bool taBase::EditPanel(bool new_tab, bool pin_tab) {
  if(!taMisc::gui_active) return false;
  iMainWindowViewer* inst = taiMisc::active_wins.Peek_MainWindow();
  if (!inst) return false; // shouldn't happen!
  taiDataLink* link = (taiDataLink*)GetDataLink();
  if (!link) return false; // shouldn't happen!
  bool rval = false;
  // for new_tab, we open new locked panels,
  // for existing, we do a browse to the item
  if (new_tab) {
    rval = inst->AssertPanel(link, new_tab, pin_tab);
  } else {
    rval = inst->AssertBrowserItem(link);
  }
  return rval;
}

bool taBase::BrowserSelectMe() {
  if(!taMisc::gui_active) return false;

  // first, check for an edit dialog and use that if found
  MainWindowViewer* edlg = MainWindowViewer::FindEditDialog(this);
  if(edlg) {
    edlg->Show();		// focus on it
    return true;
  }

  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return false;
  taiDataLink* link = (taiDataLink*)GetDataLink();
  if (!link) return false;
  
  bool rval = false;
  // iterate to find all Browsers 
  for (int i = 0; i < proj->viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(proj->viewers.FastEl(i));
    //if (vwr && (vwr->GetName() == "DefaultProjectBrowser")) return vwr;
    if (!(vwr && vwr->isProjBrowser())) continue;
    iMainWindowViewer* imwv = vwr->widget();
    if(!imwv) continue;
  
    rval = rval || (bool)imwv->AssertBrowserItem(link);
  }
  return rval;
}

bool taBase::BrowserExpandAll() {
  if(!taMisc::gui_active) return false;
  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return false;
  taiDataLink* link = (taiDataLink*)GetDataLink();
  if (!link) return false;
  
  bool rval = false;
  // iterate to find all Browsers 
  for (int i = 0; i < proj->viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(proj->viewers.FastEl(i));
    //if (vwr && (vwr->GetName() == "DefaultProjectBrowser")) return vwr;
    if (!(vwr && vwr->isProjBrowser())) continue;
    iMainWindowViewer* imwv = vwr->widget();
    if(!imwv) continue;
  
    rval = rval || (bool)imwv->BrowserExpandAllItem(link);
  }
  return rval;
}

bool taBase::BrowserCollapseAll() {
  if(!taMisc::gui_active) return false;
  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return false;
  taiDataLink* link = (taiDataLink*)GetDataLink();
  if (!link) return false;
  
  bool rval = false;
  // iterate to find all Browsers 
  for (int i = 0; i < proj->viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(proj->viewers.FastEl(i));
    //if (vwr && (vwr->GetName() == "DefaultProjectBrowser")) return vwr;
    if (!(vwr && vwr->isProjBrowser())) continue;
    iMainWindowViewer* imwv = vwr->widget();
    if(!imwv) continue;
  
    rval = rval || (bool)imwv->BrowserCollapseAllItem(link);
  }
  return rval;
}

bool taBase::GuiFindFromMe(const String& find_str) {
  if(!taMisc::gui_active) return false;
  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return false;
  taiDataLink* link = (taiDataLink*)GetDataLink();
  if (!link) return false;
  
  bool rval = false;
  // iterate to find all Browsers 
  for (int i = 0; i < proj->viewers.size; ++i) {
    MainWindowViewer* vwr = dynamic_cast<MainWindowViewer*>(proj->viewers.FastEl(i));
    //if (vwr && (vwr->GetName() == "DefaultProjectBrowser")) return vwr;
    if (!(vwr && vwr->isProjBrowser())) continue;
    iMainWindowViewer* imwv = vwr->widget();
    if(!imwv) continue;
  
    rval= true;
    imwv->Find(link, find_str);
  }
  return rval;
}


//////////////////////////
//   iHelpBrowser	//
//////////////////////////

QWebView* iWebView::createWindow(QWebPage::WebWindowType type) {
  QWebView* rval = NULL;
  emit sigCreateWindow(type, rval);
  if (!rval) 
    rval = inherited::createWindow(type);
  return rval;
}

iHelpBrowser* iHelpBrowser::inst;

iHelpBrowser* iHelpBrowser::instance() {
  if (!inst) {
    inst = new iHelpBrowser();
    iSize sz = taiM->dialogSize(taiMisc::dlgBig);
    inst->resize(sz.width(), (int)(1.2f * (float)sz.height())); // a bit bigger than .6h
    inst->show();
    taiMiscCore::ProcessEvents(); // run default stuff
    taiMiscCore::ProcessEvents(); // run default stuff
    taiMiscCore::ProcessEvents(); // run default stuff
  } else {
    inst->show();
    inst->raise();
  }
  return inst;
}

bool iHelpBrowser::IsUrlExternal(const String& url) {
  return (url.startsWith("http:") ||
    url.startsWith("https:") ||
    url.startsWith("file:")
  );
}

void iHelpBrowser::StatLoadEnum(TypeDef* typ) {
  instance()->LoadEnum(typ);
}

void iHelpBrowser::StatLoadMember(MemberDef* mbr) {
  instance()->LoadMember(mbr);
}

void iHelpBrowser::StatLoadMethod(MethodDef* mth) {
  instance()->LoadMethod(mth);
}

void iHelpBrowser::StatLoadType(TypeDef* typ) {
  instance()->LoadType(typ);
}

void iHelpBrowser::StatLoadUrl(const String& url) {
  instance()->LoadUrl(url);
}

String iHelpBrowser::UrlToTabText(const String& url) {
  String base_url;
  String anchor = url.after("#");
  if (anchor.empty())
    base_url = url;
  else
    base_url = url.before("#");
  if (base_url.contains(".Type.")) {
    return base_url.after(".Type.");
  }
  return url;
}


// note: we parent to main_win so something will delete it
iHelpBrowser::iHelpBrowser() 
:inherited(taiMisc::main_window)
{
  init();
}

iHelpBrowser::~iHelpBrowser() {
  if (this == inst) {
    inst = NULL;
  }
  // disconnect in case it will be firing
  disconnect(this, SIGNAL(tv_currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
}

void iHelpBrowser::init() {
  this->setAttribute(Qt::WA_DeleteOnClose, false); // keep alive when closed
  this->setWindowTitle("Help Browser");
//  this->setSizeGripEnabled(true);
  
  split = new iSplitter;
  
  QWidget* tvw = new QWidget;
  QVBoxLayout* lay_tv = new QVBoxLayout(tvw);
  lay_tv->setMargin(0);

  QToolBar* tool_bar = new QToolBar(tvw);
  lay_tv->addWidget(tool_bar);
  
  actBack = tool_bar->addAction("<");
  actBack->setToolTip("Back");
  actBack->setStatusTip(actBack->toolTip());
  actForward = tool_bar->addAction(">" );
  actForward->setToolTip("Forward");
  actForward->setStatusTip(actForward->toolTip());
  tool_bar->addSeparator();
  
  QLabel* lbl = new QLabel("search");
  lbl->setToolTip("Search for object type names to narrow the list below -- will find anything containing the text entered");
  tool_bar->addWidget(lbl);
  filter = new iLineEdit();
  filter->setToolTip(lbl->toolTip());
  tool_bar->addWidget(filter);
  
  tv = new QTreeWidget(tvw);
  tv->setColumnCount(2);
  // will always need a vert scroller so turn on so sizing is deterministic
  tv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
//  tv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  QTreeWidgetItem* hdr = tv->headerItem();
  hdr->setText(0, "Type");
  hdr->setText(1, "Category");
  lay_tv->addWidget(tv, 1);
  
  QWidget* wid_tab = new QWidget;
  QVBoxLayout* lay_tab = new QVBoxLayout(wid_tab);
  lay_tab->setMargin(0);
  
  tool_bar = new QToolBar(wid_tab);
  lay_tab->addWidget(tool_bar);
  
  url_text = new iLineEdit();
  tool_bar->addWidget(url_text);

  prog_bar = new QProgressBar(wid_tab);
  prog_bar->setRange(0, 100);
  prog_bar->setMaximumWidth(30);
  tool_bar->addWidget(prog_bar);

  actGo = tool_bar->addAction("Go");
  actStop = tool_bar->addAction("X");
  actStop->setToolTip("Stop");
  lay_tab->addWidget(tool_bar);
  tab = new QTabWidget(wid_tab);
  tab->setElideMode(Qt::ElideMiddle);
  tab->setUsesScrollButtons(true); // otherwise doesn't always, ex. on Mac
  btnAdd = new QToolButton;
  btnAdd->setText("+");
  btnAdd->setToolTip("add a new empty tab");
  tab->setCornerWidget(btnAdd, Qt::TopLeftCorner);
  lay_tab->addWidget(tab);
  
  setCentralWidget(split);
//  layOuter->addWidget(status_bar);
  
  // add all types -- only non-virtual, base types
  AddTypesR(&taMisc::types);
  tv->setSortingEnabled(true);
  tv->sortByColumn(0, Qt::AscendingOrder);
  // fit tree to minimum -- we have to force splitter to resize unfortunately
  tv->resizeColumnToContents(0);
  int tv_width = tv->columnWidth(0) + ((QSleazyFakeTreeWidget*)tv)->sizeHintForColumn(1)
    + tv->verticalScrollBar()->width() + 40;
  tv->resize(tv_width, tv->height());
//  tv->resizeColumnToContents(1);
  
  split->addWidget(tvw);
  split->addWidget(wid_tab);

  timFilter = new QTimer(this);
  timFilter->setSingleShot(true);
  timFilter->setInterval(500);
  
  status_bar = statusBar(); // asserts

  connect(actGo, SIGNAL(triggered()), this, SLOT(go_clicked()) );
  connect(actStop, SIGNAL(triggered()), this, SLOT(stop_clicked()) );
  connect(btnAdd, SIGNAL(clicked()), this, SLOT(addTab_clicked()) );
  connect(url_text, SIGNAL(returnPressed()), this, SLOT(go_clicked()) );
  connect(actBack, SIGNAL(triggered()), this, SLOT(back_clicked()) );
  connect(actForward, SIGNAL(triggered()), this, SLOT(forward_clicked()) );
  connect(tv, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
    this, SLOT(tv_currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
  connect(tab, SIGNAL(tabCloseRequested(int)),
    this, SLOT(tab_tabCloseRequested(int)) );
  connect(filter, SIGNAL(textChanged(const QString&)),
    this, SLOT(filter_textChanged(const QString&)) );
  connect(tab, SIGNAL(currentChanged(int)),
    this, SLOT(tab_currentChanged(int)) );
  connect(timFilter, SIGNAL(timeout()), this, SLOT(timFilter_timeout()) );
  
  AddWebView(_nilString); // so stuff lays out
}

void iHelpBrowser::addTab_clicked() {
  AddWebView("");
}
    
void iHelpBrowser::forward_clicked() {
  curWebView()->forward();
}

void iHelpBrowser::back_clicked() {
  curWebView()->back();
}

void iHelpBrowser::AddTypesR(TypeSpace* ts) {
  for (int i = 0; i < ts->size; ++i) {
    TypeDef* typ = ts->FastEl(i);
    if (!(typ->is_class() && typ->is_anchor()))
      continue;
    if (typ->InheritsFormal(TA_templ_inst) ||
      typ->HasOption("VIRT_BASE") ||
      typ->HasOption("HIDDEN") ||
      typ->HasOption("IGNORE")) 
      continue;
    // get rid of the junk stub types by looking for empties...
    if ((typ->members.size == 0) && (typ->methods.size == 0))
      continue;
    QTreeWidgetItem* twi = new QTreeWidgetItem(tv);
    twi->setText(0, typ->name);
    twi->setText(1, typ->GetCat());
    twi->setData(0, Qt::UserRole, QVariant((ta_intptr_t)typ));
//    AddTypesR(&typ->children);
  }
}

QWebView* iHelpBrowser::AddWebView(const String& label) {
  ++m_changing;
  QWebView* brow = new iWebView;
  QWebPage* wp = brow->page();
  wp->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
  wp->setNetworkAccessManager(taiMisc::net_access_mgr);
  int tidx = tab->addTab(brow, label.toQString());
  tab->setCurrentIndex(tidx); // not automatic
  url_text->setText("");// something else has to make it valid
  connect(brow, SIGNAL(linkClicked(const QUrl&)),
    this, SLOT(brow_linkClicked(const QUrl&)) );
  connect(brow, SIGNAL(statusBarMessage(const QString&)),
    status_bar, SLOT(showMessage(const QString&)) );
  connect(brow, SIGNAL(sigCreateWindow(QWebPage::WebWindowType,
    QWebView*&)), this, SLOT(brow_createWindow(QWebPage::WebWindowType,
    QWebView*&)) );
  connect(brow, SIGNAL(urlChanged(const QUrl&)),
    this, SLOT(brow_urlChanged(const QUrl&)) );
  // note: WebView doesn't show hover links in status by default so we do it
  connect(wp, SIGNAL(linkHovered(const QString&, const QString&, const QString&)),
    status_bar, SLOT(showMessage(const QString&)) );
  connect(wp, SIGNAL(unsupportedContent(QNetworkReply*)),
    this, SLOT(page_unsupportedContent(QNetworkReply*)) );
  connect(brow, SIGNAL(loadProgress(int)), prog_bar, SLOT(setValue(int)) );
  connect(brow, SIGNAL(loadStarted()), prog_bar, SLOT(reset()) );
  wp->setForwardUnsupportedContent(true);

  --m_changing;
  if (tab->count() > 1) {
#if (QT_VERSION >= 0x040500) //TEMP
    tab->setTabsClosable(true);
#endif
  }
  return brow;
}

void iHelpBrowser::ApplyFiltering() {
  ++m_changing;
  taMisc::Busy();
  QTreeWidgetItemIterator it(tv, QTreeWidgetItemIterator::All);
  QTreeWidgetItem* item;
  QString s;
  //QTreeWidgetItem* first_item = NULL;
  int n_items = 0;
  while ((item = *it)) { 
    // TODO (maybe): don't hide NULL item
    bool show = ShowItem(item);
    tv->setItemHidden(item, !show);
    if(show) {
      n_items++;
    }
    ++it;
  }
  taMisc::DoneBusy();
  --m_changing;
}

void iHelpBrowser::brow_createWindow(QWebPage::WebWindowType type,
    QWebView*& window)
{
  if (type == QWebPage::WebBrowserWindow) {
    window = AddWebView(_nilString);
  }
}

void iHelpBrowser::brow_linkClicked(const QUrl& url) 
{
  // forward to global, which is iMainWindowViewer::taUrlHandler
  // for .Type. urls (us) it just calls back to LoadUrl(url)
  QDesktopServices::openUrl(url); 
}

void iHelpBrowser::brow_urlChanged(const QUrl& url) 
{ // NOTE: we assume it is only the current visible guy who can do this
  ++m_changing;
  url_text->setText(url.toString());
  tab->setTabText(tab->currentIndex(), UrlToTabText(url.toString()));
  --m_changing; 
}

void iHelpBrowser::page_unsupportedContent(QNetworkReply* reply) {
  // this should only get called when a Open Window or Open Link gets ta: type of url
  QDesktopServices::openUrl(reply->url()); 
}

void iHelpBrowser::ClearFilter() {
  ++m_changing;
  taMisc::Busy();
  last_filter.clear();
  QTreeWidgetItemIterator it(tv, QTreeWidgetItemIterator::Hidden);
  QTreeWidgetItem* item;
  while ((item = *it)) { 
    tv->setItemHidden(item, false);
    ++it;
  }
  taMisc::DoneBusy();
  --m_changing;
} 

QWebView* iHelpBrowser::curWebView() {
  if (tab->count() == 0)
    return AddWebView(_nilString);
  return (QWebView*)tab->currentWidget();
}

QWebView* iHelpBrowser::EmptyWebView(int& idx) {
  QWebView* rval = NULL;
  for (idx = 0; idx < tab->count(); ++idx) {
    rval = webView(idx);
    String turl = rval->url().toString();
    if (turl.empty())
      return rval;
  }
  rval = AddWebView("");
  idx = tab->count() - 1;
  return rval;
}

void iHelpBrowser::filter_textChanged(const QString& /*text*/) {
  // following either starts timer, or restarts it
  timFilter->start();
}

QTreeWidgetItem* iHelpBrowser::FindItem(TypeDef* typ) {
  typ = typ->GetNonPtrType();
  typ = typ->GetNonConstNonRefType();
  QTreeWidgetItemIterator it(tv);
  QTreeWidgetItem* rval;
  while ((rval = *it)) {
    if (GetTypeDef(rval) == typ) 
      return rval;
    ++it;
  }
  return NULL;
}

QTreeWidgetItem* iHelpBrowser::FindItem(const String& typ_name_) {
  QString typ_name = typ_name_;
  QTreeWidgetItemIterator it(tv);
  QTreeWidgetItem* rval;
  while ((rval = *it)) {
    if (rval->text(0) == typ_name) 
      return rval;
    ++it;
  }
  return NULL;
}

QWebView* iHelpBrowser::FindWebView(const String& url, int& idx) {
  String base_url = url; // common
  if (url.contains("#")) {
    base_url = url.before("#");
  }
  
  for (idx = 0; idx < tab->count(); ++idx) {
    QWebView* rval = webView(idx);
    String turl = rval->url().toString();
    if (turl.startsWith(base_url))
      return rval;
  }
  idx = -1;
  return NULL;
}

TypeDef* iHelpBrowser::GetTypeDef(QTreeWidgetItem* item) {
  return (TypeDef*)QVARIANT_TO_INTPTR(item->data(0, Qt::UserRole));
}

void iHelpBrowser::go_clicked() {
  QUrl url(url_text->text());
  QDesktopServices::openUrl(url); 
}

void iHelpBrowser::ItemChanged(QTreeWidgetItem* item) {
  TypeDef* typ = GetTypeDef(item);
  LoadType(typ);
}

void iHelpBrowser::LoadEnum(TypeDef* typ) {
//TODO: maybe check if enum, maybe in debug mode? maybe not needed...
  LoadType(typ->GetOwnerType(), typ->name);
}

void iHelpBrowser::LoadMember(MemberDef* mbr) {
  LoadType(mbr->GetOwnerType(), mbr->name);
}

void iHelpBrowser::LoadMethod(MethodDef* mth) {
  LoadType(mth->GetOwnerType(), mth->name);
}

void iHelpBrowser::LoadType(TypeDef* typ, const String& anchor) {
  String base_url;
  if (typ) {
    typ = typ->GetNonPtrType();
    base_url = "ta:.Type." + typ->name ;
  }
  LoadType_impl(typ, base_url, anchor);
}

void iHelpBrowser::LoadUrl(const String& url) {
  String base_url = url;
  String anchor = url.after("#");
  if (anchor.nonempty())
    base_url = url.before("#");
  if (url.startsWith("ta:.Type.")) {
    String typ_name(base_url.after(".Type."));
    TypeDef* typ = taMisc::types.FindName(typ_name);
    LoadType_impl(typ, base_url, anchor);
  }
  else if(url.startsWith("http://.type.")) {
    String typ_name(base_url.after(".type."));
    String nw_url = "ta:.Type." + typ_name;
    LoadUrl(nw_url);		// convert..
  }
  else if (IsUrlExternal(url)) {
    LoadExternal_impl(url);
  }
  else {
    taMisc::Warning("Attempt to load unsupported url into Help Browser:",
      url);
  }
}

void iHelpBrowser::LoadExternal_impl(const String& url)
{
  int idx;
  QWebView* wv = FindWebView(url, idx);
  if (!wv)
    wv = EmptyWebView(idx); // always succeeds
  ++m_changing;
  if (tab->currentIndex() != idx) {
    tab->setCurrentIndex(idx);
  }
  String tab_text(UrlToTabText(url));
  wv->load(QUrl(url.toQString()));
  tab->setTabText(idx, tab_text.toQString());
  url_text->setText(wv->url().toString());
  --m_changing;
}

void iHelpBrowser::LoadType_impl(TypeDef* typ, const String& base_url,
    const String& anchor)
{
  String html;
  String url = base_url;
  if (anchor.nonempty())
    url.cat("#").cat(anchor);
  QTreeWidgetItem* twi = NULL;
  String tab_text;
  if (typ) {
    html = typ->GetHTML();
    tab_text = typ->name;
    twi = FindItem(typ);
  }
  if (twi != tv->currentItem()) {
    ++m_changing;
    tv->setCurrentItem(twi);
    --m_changing;
  }
  int idx;
  QWebView* wv = FindWebView(url, idx);
  if (!wv)
    wv = EmptyWebView(idx); // always succeeds
  ++m_changing;
  if (tab->currentIndex() != idx) {
    tab->setCurrentIndex(idx);
  }
  wv->setHtml(html.toQString(), url.toQString());
  tab->setTabText(idx, tab_text.toQString());
  url_text->setText(wv->url().toString());
  --m_changing;
}

bool iHelpBrowser::ShowItem(const QTreeWidgetItem* item) const {
  // we show the item unless it either doesn't meet filter criteria, or not in cat
  
/*  // category filter
  if (m_cat_filter != 0) {
    String act_cat = item->data(0, ObjCatRole).toString(); //s/b blank if none set
    String cat_txt = client()->catText(m_cat_filter - 1);// subtract 1 for 'all' item
    if(multi_cats && act_cat.contains(", ")) {
      bool any_match = false;
      while(true) {
	String cur_cat = act_cat.before(", ");
	act_cat = act_cat.after(", ");
	if(cur_cat == cat_txt) {
	  any_match = true; break;
	}
	if(act_cat.contains(", ")) continue;
	if(act_cat == cat_txt) {
	  any_match = true;
	}
	break;
      }
      if(!any_match) return false;
    }
    else {
      if (act_cat != cat_txt) 
	return false;
    }
  }*/
  
  // filter text filter
  if (!last_filter.isEmpty()) {
    bool hide = true;
    QString s;
    int cols = tv->columnCount();
    for (int i = 0; i < cols; ++i) {
      s = item->text(i);
      if (s.contains(last_filter, Qt::CaseInsensitive)) {
        hide = false;
        break;
      }  
    }
    if (hide) return false;
  }
  return true;
}

void iHelpBrowser::SetFilter(const QString& filt) {
  last_filter = filt;
  ApplyFiltering();
}

bool iHelpBrowser::SetItem(TypeDef* typ) {
  QTreeWidgetItem* item = FindItem(typ);
  if (item) {
    tv->setCurrentItem(item); // should raise signal
  }
  return (item != NULL);
}

void iHelpBrowser::showEvent(QShowEvent* event) {
  inherited::showEvent(event);
  QTimer::singleShot(150, this, SLOT(show_timeout()) );
}

void iHelpBrowser::show_timeout() {
  QTreeWidgetItem* ci = tv->currentItem();
  if (ci)
    tv->scrollToItem(ci);

  tv->resizeColumnToContents(0);
  int tv_width = tv->columnWidth(0) + ((QSleazyFakeTreeWidget*)tv)->sizeHintForColumn(1)
    + tv->verticalScrollBar()->width() + 60;

  QList<int> sizes;
  sizes << tv_width << split->width() - tv_width;
  split->setSizes(sizes);
}

void iHelpBrowser::stop_clicked() {
  curWebView()->stop();
}

void iHelpBrowser::tab_currentChanged(int index) {
  if (m_changing) return; // already expected
  QWebView* wv = webView(index); // safe
  ++m_changing;
  if (wv) {
    url_text->setText(wv->url().toString());
  } else {
    url_text->setText("");
  }
  UpdateTreeItem();
  --m_changing;
}

void iHelpBrowser::tab_tabCloseRequested(int index) {
  if (tab->count() <= 1) return; // always 1;
  tab->removeTab(index);
  // don't let user close last
  if (tab->count() == 1) {
#if (QT_VERSION >= 0x040500) //TEMP
    tab->setTabsClosable(false);
#endif
  }
}

void iHelpBrowser::timFilter_timeout() {
  // if nothing has changed in text, do nothing
  QString text = filter->text();
  if (last_filter == text) return;
  // if we are already filtering, then don't reenter, but just try again
  if (m_changing) {
    timFilter->start();
    return;
  }
  if (text.isEmpty()) ClearFilter();
  else SetFilter(text);
}

void iHelpBrowser::tv_currentItemChanged(QTreeWidgetItem* curr, QTreeWidgetItem* prev) {
  if (m_changing) return;
  ItemChanged(curr);
}

void iHelpBrowser::UpdateTreeItem() {
  String url = curWebView()->url().toString();
  if (!url.startsWith("ta:.Type.")) return;
  
  String typ_name = url.after("ta:.Type.");
  if (typ_name.contains("#"))
    typ_name = typ_name.before("#");
  
  QTreeWidgetItem* twi = FindItem(typ_name);
  if (twi != tv->currentItem()) {
    ++m_changing;
    tv->setCurrentItem(twi);
    --m_changing;
  }
}

QWebView* iHelpBrowser::webView(int index) {
  if ((index < 0) || (index >= tab->count()))
    return NULL;
  return (QWebView*)tab->widget(index);
}

