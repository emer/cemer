// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#include "ta_qtbrowse_def.h"
#include "ta_qt.h"
#include "ta_qtdata.h"
#include "ta_qtdialog.h"
#include "ta_qttype.h"
#include "ta_qtclipdata.h"
#include "ta_script.h"
#include "css_qt.h"
#include "css_machine.h"

#include "icolor.h"

#include <qaction.h>
#include <qapplication.h>
#include <qbuttongroup.h>
#include <qclipboard.h>
#include <QCustomEvent>
#include <qdialog.h>
#include <qevent.h>
#include <Q3HButtonGroup>
#include <qimage.h>
#include <QLayout>
#include <qmenubar.h>
//#include <qobjectlist.h>
#include <QMenu>
#include <QList>
#include <QScrollArea>
#include <qtooltip.h>
#include <qvariant.h>
#include <QVBoxLayout>
#include <qwhatsthis.h>
#include <Q3WidgetStack>

using namespace Qt;

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





/* Viewing/Browsing hierarchies

  CLASS HIERARCHY

  (taDataViewer)
    DataViewer -- (abstract) main taBase persistable object for controlling viewing windows
      DataBrowser [ta_qtbrowse.h] -- controller for tree-based data browser
      T3Viewer [t3viewer.h] --  controller for 3d-based data navigator

  (QMainWindow)
    iDataViewer -- (abstract) main window class that can be used by DataViewer classes; manages menus,
          docking toolbars
      iTabDataViewer -- (abstract) subclass that handles tabbed data panels
        iDataBrowser -- window for tree-based browsing
        iT3Viewer -- window for Inventor-based browsing

  (QFrame, IDataLinkClient)
    iDataPanel -- basic panel that can appear in tabbed viewers -- note that panels are not tied
          to any particular viewing subsystem, so can appear i.e., in browsing and 3d viewing
      iDataPanelSet -- contains any number of other types of panels, with a way to activate each
      iBrListDataPanel -- contains a list view of the item selected in the browse tree
      EditPanel -- property list -- based on the v3.2 EditDialog


  (Q3ListViewItem, IDataLinkClient)
    BrListViewItem  -- (abstract) nodes in either the tree view (left side), or list views (right panel)
      taiTreeDataNode -- (abstract) nodes in the tree view (left side)
        tabTreeDataNode -- nodes for taBase data items
          tabListTreeDataNode -- nodes for taList data items
            tabGroupTreeDataNode -- nodes for taGroup data items
      taiListDataNode -- nodes in the list views (right panel)

  (Q3ListView)
    iListView [implementation class, ta_qtbrowse.cc] -- light subclass of the Qt ListView

  (QWidget)
    iTabView -- pane with tabs, for showing panels; a iDataViewer can split-nest/unsplit-unnest these
        ad infinitum

  (QTabBar)
    iTabBar -- tab bar in iTabView right hand side

  (QTab)
    iTab -- tabs in the iTabBar -- has a reference to its panel



  OWNERSHIP

  DataViewer (1)
    iDataViewer (0:1)

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
    * creates the correct iDataViewer window class, and all its various subcomponents,
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

/*
//TODO:
 following used to be in taiDataLink::DataChanged
  if (viewer_win())
    viewer_win()->UpdateTabNames(); //in case any changed
*/

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

void taiDataLink::FillContextMenu(taiActions* menu) {
  FillContextMenu_impl(menu);
}



//////////////////////////////////
// 	taiListItemsDataLink	//
//////////////////////////////////
/*
taiListItemsDataLink::taiListItemsDataLink(taPtrList_impl* data_, iDataBrowser* browser_)
: taiDataLink(data_, *(data_->GetDataLinksRef()), browser_)
{
}

void taiListItemsDataLink::CreateChild(BrListViewItem* par_node, BrListViewItem* after_node, void* el) {
  taPtrList_impl* list = data();
  TypeDef* typ = list->GetElType();
  if (typ == NULL) return; //TODO: maybe we should put a marker item in list???
  // if we get a taBase item, the type might only be the base type, not the derived type of the item
  // so we cast the item, and then grab the exact type right from the item
  if (typ->InheritsFrom(&TA_taBase)) {
      typ = ((taBase*)el)->GetTypeDef();
  }
  taiType* it = typ->it;
  taiDataLink* dl = it->GetDataLink(el, typ);
  if (dl == NULL) return; // shouldn't happen...

  dl->CreateTreeDataNode(par_node, after_node, "", BrListViewItem::DNF_UPDATE_NAME); //name updated later...
}

void taiListItemsDataLink::CreateChildren(BrListViewItem* par_node) {
  taPtrList_impl* list = data();
  for (int i = 0; i < list->size; ++i) {
    TypeDef* typ;
    void* el = list->GetTA_Element_(i, typ); // gets the item, and its TypeDef
    if (typ == NULL) continue; //TODO: maybe we should put a marker item in list???
    // if we get a taBase item, the type might only be the base type, not the derived type of the item
    // so we cast the item, and then grab the exact type right from the item
    if (typ->InheritsFrom(&TA_taBase)) {
        typ = ((taBase*)el)->GetTypeDef();
    }
    taiType* it = typ->it;
    taiDataLink* dl = it->GetDataLink(el, typ);
    if (dl == NULL) continue; // shouldn't happen...

    last_child_node = dl->CreateTreeDataNode(par_node, last_child_node, dl->GetText(DLT_TREE_NAME), BrListViewItem::DNF_UPDATE_NAME);
  }
}

void taiListItemsDataLink::DataChanged_impl(BrListViewItem* nd, int dcr, void* op1_, void* op2_) {
  taiDataLink::DataChanged_impl(nd, dcr, op1_, op2_);
  if (!nd->children_created) return;
  switch (dcr) {
  case DCR_LIST_INIT: break;
  case DCR_LIST_ITEM_INSERT: {	// op1=item, op2=item_after, null=at beginning
    BrListViewItem* after_node = nd->FindChildForData(op2_); //null if not found
    CreateChild(nd, after_node, op1_);
  }
    break;
  case DCR_LIST_ITEM_REMOVE: {	// op1=item -- note, item not DisOwned yet, but has been removed from list
    BrListViewItem* gone_node = nd->FindChildForData(op1_); //null if not found
    if (gone_node) delete gone_node; // goodbye!
  }
    break;
  case DCR_LIST_ITEM_MOVED: {	// op1=item, op2=item_after, null=at beginning
    BrListViewItem* moved_node = nd->FindChildForData(op1_); //null if not found
    if (moved_node == NULL) break; // shouldn't happen
    BrListViewItem* after_node = nd->FindChildForData(op2_); //null if not found
    moved_node->moveItem(after_node);
  }
    break;
  case DCR_LIST_ITEMS_SWAP: {	// op1=item1, op2=item2
    BrListViewItem* node1 = nd->FindChildForData(op1_); //null if not found
    BrListViewItem* node2 = nd->FindChildForData(op2_); //null if not found
    if ((node1 == NULL) || (node2 == NULL)) break; // shouldn't happen
    // need to seek to find item before
    BrListViewItem* before1 = NULL;
    BrListViewItem* tn = (BrListViewItem*)nd->firstChild();
    while ((tn != NULL) && (tn != node1)) {
      before1 = tn;
      tn = (BrListViewItem*)tn->nextSibling();
    }
    node1->moveItem(node2);
    node2->moveItem(before1);
  }class iDataBrowser;
class taiDataNode;

    break;
  }
}

bool taiListItemsDataLink::HasChildItems() {
  return (data()->size > 0);
}

String taiListItemsDataLink::GetName() const {
  return String("items");
}

iDataPanel* taiListItemsDataLink::CreateDataPanel(BrListViewItem* sel_node) {
  //TODO: return a list panel
  return new iDataPanel(this); // TEMP: dummy panel
}
*/


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

void tabDataLink::QueryEditActions_impl(taiMimeSource* ms, int& allowed, int& forbidden) {
  data()->QueryEditActions(ms, allowed, forbidden);
}

int tabDataLink::EditAction_impl(taiMimeSource* ms, int ea) {
  return data()->EditAction(ms, ea);
}

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
}

/* Context Menus are filled in the following order (with indicated separators)
------
Browser/gui framework items (ex. "Browse from here")
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

*/
void tabDataLink::FillContextMenu_impl(taiActions* menu) {
  inherited::FillContextMenu_impl(menu);

  TypeDef* typ = GetDataTypeDef();
  if (typ == NULL)  return;
  taiMenu_List ta_menus;
  taiActions* cur_menu = NULL;
  String men_nm;

  // add all the #MENU_CONTEXT items first, so they always appear before the nested submenus
  int cnt = 0;
  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    if (!(md->HasOption("MENU_CONTEXT"))) continue;
    if (md->im == NULL)  continue;
    taiMethodData* mth_rep = md->im->GetMethodRep(data(), NULL, NULL, NULL);
    if (mth_rep == NULL)  continue;
    if (cnt == 0) menu->AddSep();
    mth_rep->AddToMenu(menu); //TODO: need to make sure this works for MENU, BUTTON, and MENU_BUTTON types
    ++cnt;
  }

  // add the BUTTON and MENU_BUTTON items
  cnt = 0;
  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    //check skip conditions
    if (md->HasOption("NO_MENU_CONTEXT") || md->HasOption("MENU_CONTEXT") ||
        !(md->HasOption("BUTTON") || md->HasOption("MENU_BUTTON")) )
        continue;
    if (md->im == NULL)  continue;
    taiMethodData* mth_rep = md->im->GetMethodRep(data(), NULL, NULL, NULL);
    if (mth_rep == NULL)  continue;
    if (cnt == 0) menu->AddSep();

    if (md->HasOption("BUTTON")) {
      mth_rep->AddToMenu(menu); //TODO: need to make sure this works for BUTTON, and MENU_BUTTON types
    } else { // has to be "MENU_BUTTON"
      // create the submenus when needed, and locate -- default is last created one
      men_nm = md->OptionAfter("MENU_ON_");
      if (men_nm != "") {
        cur_menu = ta_menus.FindName(men_nm);
        if (cur_menu != NULL)  goto cont1;
      }
      if (cur_menu != NULL)  goto cont1;

      if (men_nm == "")
        men_nm = "Misc";  //see note in taiEditDataHost::SetCurMenuButton
      cur_menu = menu->AddSubMenu(men_nm);
      ta_menus.Add(cur_menu);

cont1:
      mth_rep->AddToMenu(cur_menu);
    }
    ++cnt;
  } // end BUTTON and MENU_BUTTON loop

  // now, finally, add the normal submenu items, ex. Object, Edit, etc.
  ta_menus.Reset();
  cur_menu = NULL;
  cnt = 0;
  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    //check skip conditions
    if ((!md->HasOption("MENU")) || (md->HasOption("NO_MENU_CONTEXT") || md->HasOption("MENU_CONTEXT") ||
        md->HasOption("BUTTON") || md->HasOption("MENU_BUTTON")) )
        continue;
    if (md->im == NULL)  continue;
    taiMethodData* mth_rep = md->im->GetMethodRep(data(), NULL, NULL, NULL);
    if (mth_rep == NULL)  continue;
    if (cnt == 0) menu->AddSep();
    // create the submenus when needed, and locate -- default is last created one
    men_nm = md->OptionAfter("MENU_ON_");
    if (men_nm != "") {
      cur_menu = ta_menus.FindName(men_nm);
      if (cur_menu != NULL)  goto cont2;
    }
    if (cur_menu != NULL) goto cont2;

    if (men_nm == "")
      men_nm = "Actions";
    cur_menu = menu->AddSubMenu(men_nm);
    ta_menus.Add(cur_menu);

cont2:
    mth_rep->AddToMenu(cur_menu);
    ++cnt;
  } // end normal submenu items

}

TypeDef* tabDataLink::GetDataTypeDef() const {
  return data()->GetTypeDef();
}

const QPixmap* tabDataLink::GetIcon(int bmf, int& flags_supported) {
  return data()->GetDataNodeBitmap(bmf, flags_supported);
}

MemberDef* tabODataLink::GetDataMemberDef() const {
  if (!m_data) return NULL;
  taBase* owner = data()->GetOwner();
  if (owner) return owner->FindMember(data());
  else return NULL;
}

taiMimeItem* tabDataLink::GetMimeItem() {
  taiMimeItem* rval = taiMimeItem::New(data());
  Assert_QObj();
  QObject::connect(qobj, SIGNAL(destroyed()), rval, SLOT(obj_destroyed()) );
  return rval;
}

String tabDataLink::GetName() const {
  return data()->GetName();
}

bool tabDataLink::HasChildItems() {
  // we only search up until we can say yes...
  MemberSpace* ms = &GetDataTypeDef()->members;
  for (int i = 0; i < ms->size; ++ i) {
    MemberDef* md = ms->FastEl(i);
    if (ShowMember(md)) return true;
  }
  return false;
}

bool tabDataLink::ShowMember(MemberDef* md) {
  TypeDef* td = md->type;
  if (td == NULL) return false; // shouldn't happen...
  // show: normally visible lists; items marked BROWSE
  if (md->HasOption("NO_BROWSE")) return false;
  return ((td->InheritsFrom(&TA_taList_impl) && md->ShowMember(taMisc::NORM_MEMBS))
    || md->HasOption("BROWSE"));
}


//////////////////////////
//   tabODataLink	//
//////////////////////////

tabODataLink::tabODataLink(taOBase* data_)
:inherited((taBase*)data_, *(data_->addr_data_link()))
{
}



//////////////////////////////////
//   tabListDataLink		//
//////////////////////////////////

tabListDataLink::tabListDataLink(taList_impl* data_)
:inherited((taOBase*)data_)
{
}

String tabListDataLink::ChildGetColText(taDataLink* child, int col, int itm_idx)
{
  return data()->ChildGetColText(child->data(), child->GetDataTypeDef(), col, itm_idx);
}

void tabListDataLink::fileNew() {
  data()->New();
}

String tabListDataLink::GetColHeading(int col) {
  return data()->GetColHeading(col);
}

taiDataLink* tabListDataLink::GetListChild(int itm_idx) {
  if ((itm_idx < 0) || (itm_idx >= data()->size))
    return NULL;
  TypeDef* typ;
  void* el = data()->GetTA_Element(itm_idx, typ); // gets the item, and its TypeDef
  if (typ == NULL) return NULL; //TODO: maybe we should return a null link???
  // if we get a taBase item, the type might only be the base type, not the derived type of the item
  // so we cast the item, and then grab the exact type right from the item
  if (typ->InheritsFrom(&TA_taBase)) {
      typ = ((taBase*)el)->GetTypeDef();
  }
  // get the ui type object, and get a link
  taiViewType* tiv = typ->iv;

  taiDataLink* dl = tiv->GetDataLink(el);
  return dl;
}

int tabListDataLink::NumListCols() {
  return data()->NumListCols();
}

//////////////////////////////////
// 	tabGroupDataLink	//
//////////////////////////////////


tabGroupDataLink::tabGroupDataLink(taGroup_impl* data_)
:inherited((taList_impl*)data_)
{
}

//void tabListDataLink::InitDataNode(BrListViewItem* node);

bool tabGroupDataLink::ShowMember(MemberDef* md) {
  if ((md->name == "gp") || (md->name == "super_gp") || (md->name == "leaf_gp"))
    return false;
  else return tabListDataLink::ShowMember(md);
}

/* maybe need this:
String tabListItemsDataLink::GetText(DataLinkText dlt) const {
  //TODO: replace with better choice for tab text
  switch (dlt) {
  case DLT_TAB_TEXT:
    return data()->GetDataType() + " items";
  case DLT_TREE_NAME:
    return "list"; //note: would only happen in a list of lists
  default:
    return "";
  }
} */


//////////////////////////
//   iToolBar 	//
//////////////////////////

iToolBar::iToolBar(ToolBar* toolBar_, const QString& label, iDataViewer* par_win)
:QToolBar(par_win)
{
  m_toolBar = toolBar_;
  setLabel(label);
}

iToolBar::~iToolBar() {
  if (toolBar()) {
    m_toolBar->WindowClosing();
  }
}

void iToolBar::showEvent(QShowEvent* e) {
  inherited::showEvent(e);
  Showing(true);
}

void iToolBar::hideEvent(QHideEvent* e) {
  inherited::hideEvent(e);
  Showing(false);
}

void iToolBar::Showing(bool showing) {
  if (!m_toolBar) return;
  iDataViewer* dv = m_toolBar->viewer_win();
  if (!dv) return;
  taiMenuEl* me = dv->toolBarMenu->items.SafeEl(m_toolBar->index);
  if (!me) return;
  me->setChecked(showing);
}


//////////////////////////////////
// 	iToolBar_List 	//
//////////////////////////////////

iToolBar_List::~iToolBar_List() {}

iToolBar* iToolBar_List::FindToolBar(const char* name) const {
  iToolBar* rval = NULL;
  //TODO
  return rval;
}


//////////////////////////
//   ISelectable	//
//////////////////////////

// called from Ui for cut/paste etc. -- not called for drag/drop ops
int ISelectable::EditAction_(ISelectable_PtrList& sel_items, int ea) {
  taiMimeSource* ms = NULL;
  taiClipData* cd = NULL;
  int rval = 0; //not really used, but 0 is ignored, 1 is done, -1 is forbidden, -2 is error
  // get the appropriate data, either clipboard data, or item data, depending on op
  if  (ea & (taiClipData::EA_SRC_OPS)) { // no clipboard data
    if (sel_items.size <= 1) { // single select
      rval = EditActionS_impl_(ea);
    } else { // multi src
      // iterate items while they are done and not forbidden or error
      // must iterate from end, since action could be delete
      for (int i = sel_items.size - 1; i >= 0 ; --i) {
        ISelectable* is = sel_items.FastEl(i);
        int trval = is->EditActionS_impl_(ea);
        if (trval == 0) continue;
        rval = trval;
        if (rval < 0) break; // forbidden or error
      }
    }
    // clipboard ops: if successful, must now put data on cb
    if ((ea & (taiClipData::EA_CUT | taiClipData::EA_COPY)) && (rval == 1)) { // copy-like op, get item data
      cd = GetClipData(sel_items, taiClipData::ClipOpToSrcCode(ea), false);
      // note that a Cut is a Copy, possibly followed later by a xxx_data_taken command, if client pastes it
      QApplication::clipboard()->setData(cd, QClipboard::Clipboard);
      cd = NULL; // clipboard now owns it
    }
  } else { // paste-like op, get item data
    //TODO: maybe we should confirm only 1 item selected???
    ms = taiMimeSource::New(QApplication::clipboard()->data(QClipboard::Clipboard));
    rval = EditActionD_impl_(ms, ea);
  }
  if (ms) delete ms;
  if (cd) delete cd;
  return rval;
}

int ISelectable::EditActionD_impl_(taiMimeSource* ms, int ea) {//note: follows same logic as the Query
  taiDataLink* pdl = par_link();
  //note: called routines must requery for allowed

  int rval = 0;
  if (pdl) {
    rval = pdl->ChildEditAction_impl(par_md(), link(), ms, ea);
  }
  if (rval == 0)
    rval = link()->ChildEditAction_impl(this->md(), NULL, ms, ea);
  if (rval == 0)
    rval = link()->EditAction_impl(ms, ea);
  return rval;
}

int ISelectable::EditActionS_impl_(int ea) {//note: follows same logic as the Query
  taiDataLink* pdl = par_link();
  //note: called routines must requery for allowed

  int rval = 0;
  if (pdl) {
    rval = pdl->ChildEditAction_impl(par_md(), link(), NULL, ea);
  }
  if (rval == 0)
    rval = link()->EditAction_impl(NULL, ea);
  return rval;
}

void ISelectable::FillContextMenu(ISelectable_PtrList& sel_items, taiActions* menu) {
  FillContextMenu_impl(menu);
  int allowed = GetEditActions_(sel_items);
  FillContextMenu_EditItems_impl(menu, allowed);
  link()->FillContextMenu(menu);
}

void ISelectable::FillContextMenu_EditItems_impl(taiActions* menu, int allowed) {
  if (allowed == 0) return;
  if (menu->count() > 0)
    menu->AddSep();
  //TODO: maybe always show the basic ones, and only enable/disable
  //TODO: provide a way to add these via already existing Actions (ex toolbar)
//  cut copy paste link delete
  if (allowed & taiClipData::EA_CUT) {
    taiMenuEl* mel = menu->AddItem("Cu&t", taiMenu::use_default,
        taiMenuEl::men_act, widget(), SLOT(mnuEditAction(taiMenuEl*)), this);
    mel->usr_data = (void*)taiClipData::EA_CUT;
  }
  if (allowed & taiClipData::EA_COPY) {
    taiMenuEl* mel = menu->AddItem("&Copy", taiMenu::use_default,
        taiMenuEl::men_act, widget(), SLOT(mnuEditAction(taiMenuEl*)), this);
    mel->usr_data = (void*)taiClipData::EA_COPY;
  }
  if (allowed & taiClipData::EA_PASTE) {
    taiMenuEl* mel = menu->AddItem("&Paste", taiMenu::use_default,
        taiMenuEl::men_act, widget(), SLOT(mnuEditAction(taiMenuEl*)), this);
    mel->usr_data = (void*)taiClipData::EA_PASTE;
  }
  if (allowed & taiClipData::EA_LINK) {
    taiMenuEl* mel = menu->AddItem("&Link", taiMenu::use_default,
        taiMenuEl::men_act, widget(), SLOT(mnuEditAction(taiMenuEl*)), this);
    mel->usr_data = (void*)taiClipData::EA_LINK;
  }
  if (allowed & taiClipData::EA_DELETE) {
    taiMenuEl* mel = menu->AddItem("&Delete", taiMenu::use_default,
        taiMenuEl::men_act, widget(), SLOT(mnuEditAction(taiMenuEl*)), this);
    mel->usr_data = (void*)taiClipData::EA_DELETE;
  }
  link()->FillContextMenu_EditItems(menu, allowed);
}

taiClipData* ISelectable::GetClipData(const ISelectable_PtrList& sel_items, int src_edit_action,
  bool for_drag) const
{
  taiClipData* rval = NULL;
  QWidget* par = (for_drag) ?  widget() : NULL; // must have parent for drag, and must not for clipboard

  if (sel_items.size <= 1) { // single
    taiMimeItem* mi = this->GetMimeItem();
    rval = new taiSingleClipData(mi, src_edit_action, par);
  } else { // multi select
    taiMimeItem_List* mil = new taiMimeItem_List(); // note: mimeitem takes ownership of list
    for (int i = 0; i < sel_items.size; ++i) {
      ISelectable* dn = sel_items.FastEl(i);
      mil->Add(dn->GetMimeItem());
    }
    rval = new taiMultiClipData(mil, src_edit_action, par);
  }
  return rval;
}

int ISelectable::GetEditActions_(taiMimeSource* ms) const {
  int allowed = 0;
  int forbidden = 0;
  GetEditActionsD_impl_(ms, allowed, forbidden);
  return (allowed & (~forbidden));
}

int ISelectable::GetEditActions_(const ISelectable_PtrList& sel_items) const {
  int allowed = 0;
  int forbidden = 0;
  if (sel_items.size <= 1) { // single select
    GetEditActionsS_impl_(allowed, forbidden);
    taiMimeSource* ms = taiMimeSource::New(QApplication::clipboard()->data(QClipboard::Clipboard));
    GetEditActionsD_impl_(ms, allowed, forbidden);
    delete ms;
  } else { // multi select
    for (int i = 0; i < sel_items.size; ++i) {
      ISelectable* is = sel_items.FastEl(i);
      is->GetEditActionsS_impl_(allowed, forbidden);
    }
  }
  return (allowed & (~forbidden));
}

void ISelectable::GetEditActionsD_impl_(taiMimeSource* ms, int& allowed, int& forbidden) const {
  // parent object will generally manage CUT, and DELETE
  // parent object normally passes on to child object
  taiDataLink* pdl = par_link();
  if (pdl) pdl->ChildQueryEditActions_impl(par_md(), link(), ms, allowed, forbidden); // ex. DROP of child on another child, to reorder
  link()->ChildQueryEditActions_impl(this->md(), NULL, ms, allowed, forbidden); // ex. DROP of child on parent, to insert as first item
  link()->QueryEditActions_impl(ms, allowed, forbidden); // ex. COPY
}

void ISelectable::GetEditActionsS_impl_(int& allowed, int& forbidden) const {
  // parent object will generally manage CUT, and DELETE
  // parent object normally passes on to child object
  taiDataLink* pdl = par_link();
  if (pdl) pdl->ChildQueryEditActions_impl(par_md(), link(), NULL, allowed, forbidden); // ex. CUT of child
  link()->QueryEditActions_impl(NULL, allowed, forbidden); // ex. COPY
}

taiMimeItem* ISelectable::GetMimeItem() const {
  return link()->GetMimeItem();
}

QWidget* ISelectable::widget() const {
  return host()->This();
}

//////////////////////////
//   ISelectable_PtrList//
//////////////////////////

TypeDef* ISelectable_PtrList::CommonSubtype1N() { // greatest common subtype of items 1-N
  if (size == 0) return NULL;
  TypeDef* rval = FastEl(0)->link()->GetDataTypeDef();
  for (int i = 1; (rval && (i < size)); ++i) {
    rval = TypeDef::GetCommonSubtype(rval, FastEl(i)->link()->GetDataTypeDef());
  }
  return rval;
}

TypeDef* ISelectable_PtrList::CommonSubtype2N() { // greatest common subtype of items 2-N
  if (size <= 1) return NULL;
  TypeDef* rval = FastEl(1)->link()->GetDataTypeDef();
  for (int i = 2; (rval && (i < size)); ++i) {
    rval = TypeDef::GetCommonSubtype(rval, FastEl(i)->link()->GetDataTypeDef());
  }
  return rval;
}

TypeDef* ISelectable_PtrList::Type1() {
  if (size == 0) return NULL;
  else           return FastEl(0)->link()->GetDataTypeDef();
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

void DynMethod_PtrList::Fill(ISelectable_PtrList& sel_items) {
  Reset();
  if (sel_items.size == 0) return;

  TypeDef* t1n = sel_items.CommonSubtype1N(); // greatest common subtype of items 1-N
  if (t1n == NULL) return; // typically for non-taBase types, ex Class browsing
  MethodDef* md;
  // Type_N methods (common to all)
  for (int i = 0; i < t1n->methods.size; ++i) {
    md = t1n->methods.FastEl(i);
    if (!md->HasOption("DYN1")) continue;
    AddNew(Type_1N, md);
  }

  if (sel_items.size == 1) return;
  TypeDef* t1 = sel_items.FastEl(0)->link()->GetDataTypeDef(); // type of 1st item
  TypeDef* t2n = sel_items.CommonSubtype2N(); // greatest common subtype of items 2-N
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



//////////////////////////
//	iDataViewer	//
//////////////////////////

iDataViewer::iDataViewer(DataViewer* viewer_, QWidget* parent)
: QMainWindow(parent, (Qt::WType_TopLevel |Qt:: WStyle_SysMenu | Qt::WStyle_MinMax | Qt::WDestructiveClose))
{
  m_viewer = viewer_;
  init();
}

/*obs iDataViewer::iDataViewer(DataViewer* viewer_, QWidget* parent, WFlags flags)
: QMainWindow(parent, NULL, flags)
{
  m_viewer = viewer_;
  init();
} */

iDataViewer::~iDataViewer() {
  m_sel_chg_cnt = -1000000; //effectively eliminates any selection-related activity
  taiMisc::viewer_wins.Remove(this);
  if (m_viewer) {
    m_viewer->m_window = NULL;
    m_viewer->menu = NULL;
    m_viewer = NULL;
  }
//TODO: need to delete menu, but just doing a delete causes an exception (prob because Qt
// has already deleted the menu items
//  if (menu) delete menu;
  menu = NULL;
}

void iDataViewer::init() {
  menu = NULL;
  m_body = NULL;
  last_enabler = NULL;
  is_root = false;
  m_sel_chg_cnt = 0;
  m_last_action_idx = -1;
  // set maximum size
  iSize ss = taiM->scrn_s;
  setMaximumSize(ss.width(), ss.height());
  // set default size as the big hor dialog
  resize(taiM->dialogSize(taiMisc::hdlg_b));

  setFont(taiM->dialogFont(taiM->ctrl_size));

  (void)statusBar(); // creates the status bar
  
}

void iDataViewer::actionsMenu_aboutToShow() {
  // clear out the previous dynamic items
  int i;
  for (i = actionsMenu->count() - 1; i > m_last_action_idx; --i) {
    actionsMenu->DeleteItem(i);
  }
  // add actions corresponding to dynamic list
  for (i = 0; i < (int)dyn_actions.count(); ++i) {
    taiAction* act = dyn_actions.FastEl(i);
    act->AddTo(actionsMenu);
  }
}

void iDataViewer::FillContextMenu(taiActions* menu) {
  if (dyn_actions.count() == 0) return; // prevents spurious separator
  menu->AddSep();
  // add actions corresponding to dynamic list
  for (int i = 0; i < (int)dyn_actions.count(); ++i) {
    taiAction* act = dyn_actions.FastEl(i);
    act->AddTo(menu);
  }
}

taiAction* iDataViewer::AddAction(taiAction* act) {
  actions.Add(act); // refs the act
  return act;
}

void iDataViewer::AddSelectedItem(ISelectable* item,  bool forced) {
  m_sel_items.AddUnique(item);
  if (m_sel_chg_cnt == 0)
    SelectionChanged(forced);
}

iToolBar* iDataViewer::AddToolBar(iToolBar* tb) {
  toolbars.append(tb);
  return tb;
}

void iDataViewer::AddToolBarMenu(const String& name, int index) {
//taiMenuEl* menu =
  toolBarMenu->AddItem(name, taiMenu::toggle,
    taiMenuEl::int_act, this, SLOT(this_ToolBarSelect(int)), index);
}

void iDataViewer::ch_destroyed() {
  last_enabler = NULL;
  UpdateUi();
}

void iDataViewer::closeEvent (QCloseEvent* e) {
  bool forced = cssiSession::quitting || (m_viewer == NULL); // always closing if we no longer have our mummy
  bool cancel = false; // falling through to end of routine is "proceed"

  if (m_viewer && (forced || !cancel)) {
    m_viewer->WindowClosing(cancel);
  }
  if (forced || !cancel) Closing(forced, cancel);
  if (cancel) e->ignore();
  else        e->accept();
}

void iDataViewer::Constr() {
  Constr_Menu_impl();
  // we always put the fileclose action at bottom of menu
  fileMenu->insertSeparator();
  fileCloseWindowAction->AddTo(fileMenu);

  Constr_Body_impl();
  actionsMenu->insertSeparator();
  m_last_action_idx = actionsMenu->count() - 1;

  taiMisc::viewer_wins.AddUnique(this);
}

void iDataViewer::Constr_Menu_impl() {
  if (menu) return;
  // create a taiMenu wrapper around the window's provided menubar
  menu = new taiMenuBar(this, taiMisc::fonBig, menuBar());

  QImage img;
  img.loadFromData( image0_data, sizeof( image0_data ), "PNG" );
  image0 = img;
  img.loadFromData( image1_data, sizeof( image1_data ), "PNG" );
  image1 = img;
  img.loadFromData( image2_data, sizeof( image2_data ), "PNG" );
  image2 = img;
  img.loadFromData( image3_data, sizeof( image3_data ), "PNG" );
  image3 = img;
  img.loadFromData( image4_data, sizeof( image4_data ), "PNG" );
  image4 = img;
  img.loadFromData( image5_data, sizeof( image5_data ), "PNG" );
  image5 = img;
  img.loadFromData( image6_data, sizeof( image6_data ), "PNG" );
  image6 = img;
  img.loadFromData( image7_data, sizeof( image7_data ), "PNG" );
  image7 = img;
  img.loadFromData( image8_data, sizeof( image8_data ), "PNG" );
  image8 = img;

  // default actions
  fileNewAction = AddAction(new taiAction("&New...", QKeySequence("Ctrl+N"), _fileNewAction ));
  fileNewAction->setIconSet( QIconSet( image0 ) );
  fileOpenAction = AddAction(new taiAction("&Open...", QKeySequence("Ctrl+O"), _fileOpenAction ));
  fileOpenAction->setIconSet( QIconSet( image1 ) );
  fileSaveAction = AddAction(new taiAction("&Save", QKeySequence("Ctrl+S"), _fileSaveAction ));
  fileSaveAction->setIconSet( QIconSet( image2 ) );
  fileSaveAsAction = AddAction(new taiAction("Save &As...", QKeySequence(), _fileSaveAsAction ));
  fileCloseAction = AddAction(new taiAction("Close", QKeySequence(), "fileCloseAction" ));
  filePrintAction = AddAction(new taiAction("&Print...", QKeySequence("Ctrl+P"), _filePrintAction ));
  filePrintAction->setIconSet( QIconSet( image3 ) );
  if (is_root)
    fileCloseWindowAction = AddAction(new taiAction("&Quit", QKeySequence("Ctrl+Q"), _fileCloseWindowAction ));
  else
    fileCloseWindowAction = AddAction(new taiAction("C&lose Window", QKeySequence("Ctrl+W"), _fileCloseWindowAction ));
  editUndoAction = AddAction(new taiAction("&Undo", QKeySequence("Ctrl+Z"), _editUndoAction ));
  editUndoAction->setEnabled( FALSE );
  editUndoAction->setIconSet( QIconSet( image4 ) );
  editRedoAction = AddAction(new taiAction("&Redo", QKeySequence("Ctrl+Y"), _editRedoAction ));
  editRedoAction->setEnabled( FALSE );
  editRedoAction->setIconSet( QIconSet( image5 ) );
  editCutAction = AddAction(new taiAction(taiClipData::EA_CUT, "Cu&t", QKeySequence("Ctrl+X"), _editCutAction ));
  editCutAction->setIconSet( QIconSet( image6 ) );
  editCopyAction = AddAction(new taiAction(taiClipData::EA_COPY, "&Copy", QKeySequence("Ctrl+C"), _editCopyAction ));
  editCopyAction->setIconSet( QIconSet( image7 ) );
  editPasteAction = AddAction(new taiAction(taiClipData::EA_PASTE, "&Paste", QKeySequence("Ctrl+V"), _editPasteAction ));
  editPasteAction->setIconSet( QIconSet( image8 ) );
  editDeleteAction = AddAction(new taiAction(taiClipData::EA_DELETE, "&Delete", QKeySequence("Shift+D"), _editDeleteAction ));
//  editDeleteAction->setIconSet( QIconSet( image8 ) );
  editLinkAction = AddAction(new taiAction(taiClipData::EA_LINK, "&Link", QKeySequence("Ctrl+L"), _editLinkAction ));
  viewRefreshAction = AddAction(new taiAction("&Refresh", QKeySequence("F5"), _viewRefreshAction ));
  helpHelpAction = AddAction(new taiAction("&Help", QKeySequence(), _helpHelpAction ));
  helpAboutAction = AddAction(new taiAction("&About", QKeySequence(), _helpAboutAction ));

  fileMenu = menu->AddSubMenu("&File");
  fileNewAction->AddTo(fileMenu);
  fileOpenAction->AddTo(fileMenu );
  fileSaveAction->AddTo(fileMenu );
  fileSaveAsAction->AddTo(fileMenu);
  fileExportMenu = fileMenu->AddSubMenu("Export"); // submenu -- empty and disabled in base
  fileMenu->insertSeparator();
  filePrintAction->AddTo( fileMenu );

  editMenu = menu->AddSubMenu("&Edit");
  editUndoAction->AddTo( editMenu );
  editRedoAction->AddTo( editMenu );
  editMenu->insertSeparator();
  editCutAction->AddTo( editMenu );
  editCopyAction->AddTo( editMenu );
  editPasteAction->AddTo( editMenu );
  editLinkAction->AddTo( editMenu );
  editDeleteAction->AddTo( editMenu );

  viewMenu = menu->AddSubMenu("&View");
  viewRefreshAction->AddTo(viewMenu);
  viewMenu->insertSeparator();
  toolBarMenu = viewMenu->AddSubMenu("Toolbars");
  viewMenu->insertSeparator();

  actionsMenu = menu->AddSubMenu("&Actions");

  toolsMenu = menu->AddSubMenu("&Tools");
  
  helpMenu = menu->AddSubMenu("&Help");;
  helpHelpAction->AddTo(helpMenu );
  helpMenu->insertSeparator();
  helpAboutAction->AddTo(helpMenu );

    // signals and slots connections
  connect( fileNewAction, SIGNAL( activated() ), this, SLOT( fileNew() ) );
  connect( fileOpenAction, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
  connect( fileSaveAction, SIGNAL( activated() ), this, SLOT( fileSave() ) );
  connect( fileSaveAsAction, SIGNAL( activated() ), this, SLOT( fileSaveAs() ) );
  connect( fileCloseAction, SIGNAL( activated() ), this, SLOT( fileClose() ) );
//   connect( filePrintAction, SIGNAL( activated() ), this, SLOT( filePrint() ) ); */
    connect( fileCloseWindowAction, SIGNAL( activated() ), this, SLOT( fileCloseWindow() ) );
//    connect( editUndoAction, SIGNAL( activated() ), this, SLOT( editUndo() ) );
//   connect( editRedoAction, SIGNAL( activated() ), this, SLOT( editRedo() ) );
    connect( editCutAction, SIGNAL( activated(int) ), this, SIGNAL(EditAction(int)) );
    connect( editCopyAction, SIGNAL( activated(int) ), this, SIGNAL(EditAction(int)) );
    connect( editPasteAction, SIGNAL( activated(int) ), this, SIGNAL(EditAction(int)) );
    connect( editLinkAction, SIGNAL( activated(int) ), this, SIGNAL(EditAction(int)) );
    connect( editDeleteAction, SIGNAL( activated(int) ), this, SIGNAL(EditAction(int)) );
    connect( viewRefreshAction, SIGNAL( activated() ), this, SLOT(viewRefresh()) );
    connect(actionsMenu->GetRep(), SIGNAL( aboutToShow() ), this, SLOT(actionsMenu_aboutToShow()) );
//    connect( helpContentsAction, SIGNAL( activated() ), this, SLOT( helpContents() ) );
    connect( helpAboutAction, SIGNAL( activated() ), this, SLOT( helpAbout() ) );

}

iToolBar* iDataViewer::Constr_ToolBar(ToolBar* tb, String name) {
  iToolBar* rval = new iToolBar(tb, name, this);
  return rval;
}

void iDataViewer::customEvent(QEvent* ev) {
  switch ((int)ev->type()) {
  case TA_QT_EVENT_SEL_CHANGED:
    //Qt4: hopefully this is a QCustomEvent!!!
    selectionChangedEvent((QCustomEvent*)ev);
    break;
  default:
    inherited::customEvent(ev);
    break;
  }
}

bool iDataViewer::InitToolBar(const String& name, iToolBar* tb) {
  bool rval = false;
  if (name == "Application") {
    fileNewAction->addTo(tb);
    fileOpenAction->addTo(tb);
    fileSaveAction->addTo(tb);
    fileSaveAsAction->addTo(tb);
    fileCloseAction->addTo(tb);
    filePrintAction->addTo(tb);
    tb->addSeparator();
    editUndoAction->addTo(tb);
    editRedoAction->addTo(tb);
    tb->addSeparator();
    editCutAction->addTo(tb);
    editCopyAction->addTo(tb);
    editPasteAction->addTo(tb);
    tb->addSeparator();
    helpHelpAction->addTo(tb);
    return true;
  }
  return rval;
}

void iDataViewer::emit_EditAction(int param) {
  emit EditAction(param);
  taiMisc::RunPending();
  UpdateUi();
}

/*obs void iDataViewer::emit_selectionChanged()
{
  emit selectionChanged();
} */

void iDataViewer::emit_GetEditActionsEnabled(int& ea) {
  emit GetEditActionsEnabled(ea);
}

void iDataViewer::emit_SetActionsEnabled() {
  emit SetActionsEnabled();
}

void iDataViewer::fileCloseWindow() {
  if (is_root) {
    if (taMisc::app) taMisc::app->Quit();
  } else close();
}

int iDataViewer::GetEditActions() {
  int rval = 0;
  emit_GetEditActionsEnabled(rval);
  return rval;
}

void iDataViewer::mnuDynAction(int idx) {
  if (idx >= dyn_methods.size) return; // shouldn't happen
  DynMethodDesc* dmd = dyn_methods.FastEl(idx);
  // NOTE: this function is based on the ta_qtdata:taiMethod::CallFun function
  // NOTE: we can't show the return values from any of the functions

  MethodDef* meth =  dmd->md;
  int use_argc = meth->fun_argc;
  int hide_args = (dmd->dmd_type == DynMethod_PtrList::Type_1N) ? 0 : 1;

  if (meth->stubp == NULL) return;
  TypeDef* typ = NULL; // type of calling method's class
  void* base = NULL; // the object calling the method

//TODO:  ApplyBefore();
  String argc_str = meth->OptionAfter("ARGC_");
  if (argc_str != "")
    use_argc = (int)argc_str;
  use_argc = MIN(use_argc, meth->arg_types.size);
  use_argc = MIN(use_argc, meth->arg_names.size);

  // use a copy of selected items list, to avoid issues if items change during these operations
  ISelectable_PtrList sel_items(m_sel_items);

  // if no params to prompt for, and no confirmation required, just do it
  if (((use_argc - hide_args) == 0) && !meth->HasOption("CONFIRM")) {
//TODO    GenerateScript();
#ifdef DMEM_COMPILE
    // don't actually run the command when using gui in dmem mode: everything happens via the script!
    if (taMisc::dmem_nprocs == 1) {
#endif
      cssEl* rval; // ignored
      int i;
      ISelectable* itN;

      switch(dmd->dmd_type) {
      case DynMethod_PtrList::Type_1N: { // same for all
        for (i = 0; i < sel_items.size; ++i) {
          itN = sel_items.FastEl(i);
          typ = itN->GetDataTypeDef();
          base = itN->link()->data();
          rval = (*(meth->stubp))(base, 0, (cssEl**)NULL);
        }
      }
      case DynMethod_PtrList::Type_1_2N: { // call 1 with 2:N as a param
        cssEl* param[2];
        param[0] = &cssMisc::Void;
        param[1] = new cssCPtr();
        ISelectable* it1 = sel_items.FastEl(0);
        typ = it1->GetDataTypeDef();
        base = it1->link()->data();
        for (i = 1; i < sel_items.size; ++i) {
          itN = sel_items.FastEl(i);
          *param[1] = (void*)itN->link()->data();
          rval = (*(meth->stubp))(base, 1, param); // note: "array" of 1 item
        }
        delete param[1];
      }
      case DynMethod_PtrList::Type_2N_1: { // call 2:N with 1 as param
      }
      }
//TODO:      UpdateAfter();
#ifdef DMEM_COMPILE
    }
#endif
    return;
  }
/*  arg_dlg = new cssiArgDialog(meth, typ, base, use_argc, 0); //modal
  const iColor* bgclr = NULL;
  if (typ->InheritsFrom(TA_taBase)) {
    bgclr = ((TAPtr)base)->GetEditColorInherit();
  }
  if ((bgclr == NULL) && (host != NULL)) bgclr = host->bg_color;
  arg_dlg->Constr("", "", bgclr);
  int ok_can = arg_dlg->Edit(true);	// true = wait for a response
  if (ok_can && !arg_dlg->err_flag) {
    GenerateScript();
#ifdef DMEM_COMPILE
    // don't actually run the command when using gui in dmem mode: everything happens via the script!
    if (taMisc::dmem_nprocs == 1) {
#endif
      cssEl* rval = (*(meth->stubp))(base, arg_dlg->obj->members->size-1,
				     arg_dlg->obj->members->els);
      UpdateAfter();
      if (rval != &cssMisc::Void)
	ShowReturnVal(rval);
#ifdef DMEM_COMPILE
    }
#endif
  }
  delete arg_dlg;
*/

}

void iDataViewer::mnuEditAction(taiMenuEl* mel) {
   // called from context; cast obj to an taiClipData::EditAction
  emit_EditAction((int)(mel->usr_data));
}

bool iDataViewer::ObjectRemoving(ISelectable* item) {
  // just blindly try removing it from sel list
  return RemoveSelectedItem(item);
}

bool iDataViewer::RemoveSelectedItem(ISelectable* item,  bool forced) {
  bool rval = m_sel_items.Remove(item);
  if ((rval || forced) && (m_sel_chg_cnt == 0))
    SelectionChanged(forced);
  return rval;
}

void iDataViewer::SelectionChanged(bool forced) {
  QCustomEvent* ev = new QCustomEvent(TA_QT_EVENT_SEL_CHANGED);
  QApplication::postEvent(this, ev); // defers until we return to event loop
}

void iDataViewer::selectionChangedEvent(QCustomEvent* ev) {
  emit selectionChanged(m_sel_items);
  UpdateUi();
}

void iDataViewer::SelectionChanging(bool begin, bool forced) {
  if (begin)
    ++m_sel_chg_cnt;
  else {
    if (--m_sel_chg_cnt == 0)
      SelectionChanged(forced);
  }
}

void iDataViewer::SetActionsEnabled_impl() {
/*  cerr << m_sel_items.size << " items selected.\n";
  if (m_sel_items.size > 0) {
    TypeDef* typ = m_sel_items.CommonSubtype1N();
    String name;
    if (typ) name = typ->name;
    else     name = "(none)";
    cerr << "  CommonSubtype1N: " << name << "\n";
    if (m_sel_items.size > 1) {
      typ = m_sel_items.CommonSubtype2N();
      if (typ) name = typ->name;
      else     name = "(none)";
      cerr << "  CommonSubtype2N: " << name << "\n";
    }
  }
*/ // end TEMP

  // enumerate dynamic methods
  dyn_methods.Reset();
  dyn_methods.Fill(m_sel_items);


  // dynamically create actions
  dyn_actions.Reset(); // note: items ref deleted if needed
  for (int i = 0; i < dyn_methods.size; ++i) {
    DynMethodDesc* dmd = dyn_methods.FastEl(i);
    taiAction* act = new taiAction(i, dmd->md->GetLabel(), QKeySequence(), dmd->md->name );
    connect(act, SIGNAL(activated(int)), this, SLOT(mnuDynAction(int)));
    dyn_actions.Add(act);
  }
}

void iDataViewer::setCurItem(ISelectable* item, bool forced) {
  ISelectable* ci = m_sel_items.SafeEl(0);
  if ((ci == item) && (m_sel_items.size <= 1) && (!forced)) return;

  SelectionChanging(true, forced);
    while (m_sel_items.size > 0) {
      RemoveSelectedItem(m_sel_items.Peek(), forced);
    }
    if (item)
      AddSelectedItem(item, forced);
  SelectionChanging(false, forced);
}


void iDataViewer::SetClipboardHandler(QObject* handler_obj,
  const char* edit_enabled_slot,
  const char* edit_action_slot,
  const char* actions_enabled_slot,
  const char* update_ui_signal)
{
  if (last_enabler == handler_obj) return; // nothing to do
  // always disconnect first
  if (last_enabler) {
    disconnect(this, SIGNAL(GetEditActionsEnabled(int&)), last_enabler, NULL);
    disconnect(this, SIGNAL(EditAction(int)), last_enabler, NULL);
    disconnect(this, SIGNAL(SetActionsEnabled()), last_enabler, NULL);
    disconnect(last_enabler, NULL, this, SLOT(UpdateUi()));
    disconnect(last_enabler, SIGNAL(destroyed()), this, SLOT(ch_destroyed()));
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
  }
  last_enabler = handler_obj; // whether NULL or not
  UpdateUi();
}

void iDataViewer::setFrameGeometry(const iRect& r) {
  //NOTE: this may only work before calling show() on X
    this->resize(r.size());   // set size
    this->move(r.topLeft());  // set position
}

void  iDataViewer::setFrameGeometry(int left, int top, int width, int height) {
  setFrameGeometry(iRect(left, top, width, height));
}

void iDataViewer::SetThisAsHandler() {
  SetClipboardHandler(this,
    SLOT(this_GetEditActionsEnabled(int&)),
    SLOT(this_EditAction(int)),
    SLOT(this_SetActionsEnabled()) );
}

/*was
void iDataViewer::SetClipboardHandler(QWidget* handler_obj, const char* enabler_signal,
      const char* handler_slot)
{
  if (last_enabler == handler_obj) return; // nothing to do
  // always disconnect first
  if (last_enabler)
    disconnect(last_enabler, NULL, this, SLOT(GetEditActionsEnabled(int&)) );
  disconnect(this, SIGNAL(EditAction(int)), NULL, NULL );
  // now connect, if supplied
  if (handler_obj) {
    connect(handler_obj, enabler_signal, this, SLOT(GetEditActionsEnabled(int&)) );
    connect(this, SIGNAL(EditAction(int)), handler_obj, handler_slot );
  }
  last_enabler = handler_obj; // whether NULL or not
  SelectionChanged(true);
}*/

void iDataViewer::this_EditAction(int ea) { // this is only used by the treeview
  ISelectable* ci = curItem();
  if (ci == NULL) return;
  ci->EditAction_(sel_items(), ea);
}

void iDataViewer::this_GetEditActionsEnabled(int& ea) { // this is only used by the treeview
  ISelectable* ci = curItem();
  if (ci == NULL) return;
  ea = ci->GetEditActions_(sel_items());
}

void iDataViewer::this_SetActionsEnabled() {
  SetActionsEnabled_impl();
}

void iDataViewer::this_ToolBarSelect(int param) {
  if (!m_viewer) return;
  taiMenuEl* me = toolBarMenu->items.SafeEl(param);
  if (!me) return; // shouldn't happen
  ToolBar* tb = m_viewer->toolbars.SafeEl(param);
  if (!tb) return; // shouldn't happen
  if (me->isChecked()) { //note: check has already been toggled
    tb->Show();
  } else { //need to show
    tb->Hide();
  }
}

void iDataViewer::UpdateUi() {
  int ea = GetEditActions();
  editCutAction->setEnabled(ea & taiClipData::EA_CUT);
  editCopyAction->setEnabled(ea & taiClipData::EA_COPY);
  editPasteAction->setEnabled(ea & taiClipData::EA_PASTE);
  editLinkAction->setEnabled(ea & taiClipData::EA_LINK);
  editDeleteAction->setEnabled(ea & taiClipData::EA_DELETE);
/*todo  editCutAction->setEnabled(ea & taiClipData::EA_LINK);
  editCutAction->setEnabled(ea & taiClipData::EA_SET_AS_SUBGROUP);
  editCutAction->setEnabled(ea & taiClipData::EA_SET_AS_SUBITEM); */
  emit_SetActionsEnabled();
}

void iDataViewer::windowActivationChange(bool oldActive) {
  if (isActiveWindow()) {
    int idx = taiMisc::viewer_wins.Find(this);
    if (idx < 0) {
      taMisc::Error("iDataViewer::windowActivationChange", "Unexpectedly not in taiMisc::viewer_wins");
    } else {
      if (idx < (taiMisc::viewer_wins.size - 1)) {
        // move us to the end
        taiMisc::viewer_wins.Move(idx, taiMisc::viewer_wins.size - 1);
      }
    }
  }
  inherited::windowActivationChange(oldActive);
}

//////////////////////////
//   ToolBar		//
//////////////////////////

void ToolBar::Initialize() {
  index = -1; // assigned when added to list
  lft = 0.0f;
  top = 0.0f;
  o = Horizontal;
  mapped = false;
  m_window = NULL;
  m_viewer = NULL;
}

void ToolBar::Destroy() {
  CutLinks();
}

void ToolBar::Copy_(const ToolBar& cp) {
  lft = cp.lft;
  top = cp.top;
  o = cp.o;
  mapped = cp.mapped;
}

void ToolBar::CutLinks() {
  taiMisc::CloseEdits((void*)this, GetTypeDef());
  CloseWindow();
//obs  win_owner = NULL;
  inherited::CutLinks();
}

void ToolBar::CloseWindow() {
  if (!m_window) return;
  m_window->m_toolBar = NULL;
  m_window->deleteLater();
  m_window = NULL;
}

void ToolBar::Constr_Window_impl() {
  if (m_window) return; // shouldn't happen
  iDataViewer* vw = viewer_win();
  if (!vw) return;

  m_window = vw->Constr_ToolBar(this, this->GetName());
  viewer_win()->AddToolBar(m_window);
}

void ToolBar::GetWinPos() {
  if (!m_window) return;
  iRect r = m_window->frameGeometry();
  // convert from screen coords to relative (note, allowed to be >1.0)
  lft = (float)r.left() / (float)(taiM->scrn_s.w); // all of these convert from screen coords
  top = (float)r.top() / (float)(taiM->scrn_s.h);
}

void ToolBar::OpenNewWindow_impl() {
  iDataViewer* vw = viewer_win();
  if (!vw) return;
  Constr_Window_impl();
  if (!m_window) return; // shouldn't happen
  vw->InitToolBar(GetName(), m_window);
  SetWinPos();
  m_window->show();
  mapped = true;
}

void ToolBar::SetWinPos() {
  if (!m_window) return;

  //TODO: docked
  m_window->setOrientation((Qt::Orientation)o);
  iRect r = m_window->frameGeometry();
  r.x = (int)(lft * taiM->scrn_s.w);
  r.y = (int)(top * taiM->scrn_s.h);
  m_window->move(r.topLeft());
  m_window->resize(r.size());
}

void ToolBar::Show() {
  if (m_window) m_window->show();
  else OpenNewWindow_impl();
}

void ToolBar::Hide() {
  if (!m_window) return;
  m_window->hide();
}

DataViewer* ToolBar::viewer() {
  if (!m_viewer) {
    m_viewer = GET_MY_OWNER(DataViewer);
  }
  return m_viewer;
}

iDataViewer* ToolBar::viewer_win() {
  return (viewer()) ? m_viewer->window() : NULL;
}

void ToolBar::WindowClosing() {
  GetWinPos();
  m_window = NULL;
}


//////////////////////////
//	WinGeometry	//
//////////////////////////

float WinGeometry::Offs(float cur, float by) {
  float rval = cur + by;
  if (rval > 1.0f) rval -= 1.0f;
  if (rval < 0.0f) rval = 0.0f;
  return rval;
}

void WinGeometry::Initialize() {
  owner = NULL;
  lft = top = 0.0f;
  wd = ht = 0.6f; // default window size
}

void WinGeometry::Copy_(const WinGeometry& cp) {
  lft = cp.lft;
  top = cp.top;
  wd = cp.wd;
  ht = cp.ht;
}

void WinGeometry::GetWinPos() {
  if ((owner == NULL) || (owner->m_window == NULL))
    return;
  iRect r = owner->m_window->frameGeometry();
  // convert from screen coords to relative (note, allowed to be >1.0)
  lft = (float)r.left() / (float)(taiM->scrn_s.w); // all of these convert from screen coords
  top = (float)r.top() / (float)(taiM->scrn_s.h);
  wd = (float)r.width() / (float)(taiM->scrn_s.w);
  ht = (float)r.height() / (float)(taiM->scrn_s.h);
}

void WinGeometry::SetWinPos() {
  if ((owner == NULL) || (owner->m_window == NULL))
    return;

  iRect tr = iRect(
    (int)(lft * taiM->scrn_s.w),
    (int)(top * taiM->scrn_s.h),
    (int)(wd * taiM->scrn_s.w),
    (int)(ht * taiM->scrn_s.h)
  ); // converts to pixels

  owner->m_window->setFrameGeometry(tr);
}

void WinGeometry::ScriptWinPos(ostream& strm) {
  if (owner == NULL) return;
  GetWinPos();
  String temp = owner->GetPath();
  if(owner->iconified) {
    temp += ".Iconify();\n";
  }
  else {
    temp += String(".SetWinPos(") + String(lft) + ", " +
      String(top) + ", " + String(wd) + ", " + String(ht) + ");\n";
  }
  if(taMisc::record_script != NULL)  taMisc::RecordScript(temp);
  else   strm << temp;
}

void WinGeometry::UpdateAfterEdit() {
  taBase::UpdateAfterEdit();
  SetWinPos();
}


//////////////////////////
//   DataViewer		//
//////////////////////////

void DataViewer::Initialize() {
  m_window = NULL;
  iconified = false;
  m_is_root = false;
  m_has_changes = false;

  menu = NULL;
  cur_menu = NULL;
  ta_file= NULL;
  print_file = NULL;
  display_toggle = true;
  // default window size --
  link_type = &TA_taiDataLink; // specialized in descendant classes
}

void DataViewer::Destroy() {
  CutLinks();
  if(ta_file != NULL)    taRefN::unRefDone(ta_file);  ta_file = NULL;
}

void DataViewer::InitLinks() {
  taDataView::InitLinks();
  taBase::Own(win_pos, this);
  taBase::Own(toolbars, this);
  WinInit();			// initialize after linking, so that path is available
  //TODO: fixup for panels
  if(owner && (!owner->InheritsFrom(&TA_TypeDefault))) { // no windows for defaults
    if(taMisc::is_loading)
      taiMisc::unopened_windows.LinkUnique(this);
    //note: in v3.2 we opened the window here for interactive, but this leads to complexity
    // so we now require caller to do it after everything is built
  }
  GetFileDlg();

  // add toolbars
  ToolBar* tb = new ToolBar();
  tb->SetName("Application");
  toolbars.Add(tb);
}


void DataViewer::Copy_(const DataViewer& cp) {
  win_pos = cp.win_pos;
  win_pos.lft = WinGeometry::Offs(win_pos.lft, 0.05f);
  win_pos.top = WinGeometry::Offs(win_pos.top, 0.05f);
  if(!GetName().empty())
    SetName(GetName() + "_copy");		// only label winbases as copies

  if (ta_file && cp.ta_file) *ta_file = *(cp.ta_file);
}

void DataViewer::WinInit() {
  if(!taMisc::gui_active) return;
}

void DataViewer::CutLinks() {
  taiMisc::CloseEdits((void*)this, GetTypeDef());
  CloseWindow();
  win_pos.CutLinks();
  toolbars.CutLinks();
//obs  win_owner = NULL;
  inherited::CutLinks();
}

void DataViewer::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  if (!IsMapped()) return;
// this is a bad idea -- iconifies to quickly upon loading!
// need to let window open then iconify it -- otherwise get zombies
// and it breaks CYGWIN
//  if(iconified) Iconify();
  SetWinName();
}

void DataViewer::Changed(bool value) {
  m_has_changes = value;
}

void DataViewer::Clear() {
  if (!IsMapped()) return;
  inherited::Clear();
}

void DataViewer::Close() {
  if (GetOwner() == NULL) return;
  tabMisc::Close_Obj(this);
}

void DataViewer::CloseWindow() {
  if (!taMisc::gui_active) return;
  if (print_file != NULL) {
    taRefN::unRefDone(print_file);
    print_file = NULL;
  }
  ta_menus.Reset();
  cur_menu = NULL;
  menu = NULL; // window deletes
  if (!m_window) return;
  window()->m_viewer = NULL;

  m_window->m_viewer = NULL;
  m_window->deleteLater();
  m_window = NULL;
}

void DataViewer::Constr_Toolbars_impl() {
  for (int i = 0; i < toolbars.size; ++i) {
    ToolBar* tb = toolbars.FastEl(i);
    if (!tb->mapped) continue;
    tb->Show();
  }
}

/*TODO: somehow need to merge this concept with existing more hardwired menus on iDataViewer
void DataViewer::Constr_Menu_impl() {
  TypeDef* typ = GetTypeDef();

  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    if ((!md->HasOption("VMENU")) || (md->im == NULL))
      continue;

    taiMethodData* mth_rep = md->im->GetMethodRep(NULL, NULL, NULL, NULL); // no buttons;
    if (mth_rep == NULL) continue;

//    this_meths.Add(mth_rep);

    // find or make current menu
    String men_nm = md->OptionAfter("MENU_ON_");
    if (men_nm != "") {
      cur_menu = ta_menus.FindName(men_nm);
      if (cur_menu != NULL)  goto cont;
    }
    if (cur_menu != NULL) goto cont;

    if (men_nm == "")
      men_nm = "File";
    cur_menu = menu->AddSubMenu(men_nm);
    ta_menus.Add(cur_menu);

cont:
    mth_rep->AddToMenu(cur_menu);
  }
} */

void DataViewer::DeIconify() {
  if(!taMisc::gui_active || (m_window == NULL)) return;
  iconified = false;
  if (m_window->isMinimized())
    m_window->showNormal();
}

int DataViewer::Dump_Load_Value(istream& strm, TAPtr par) {
  int rval = taDataView::Dump_Load_Value(strm, par);
  taiMisc::RunPending();
  return rval;
}

int DataViewer::Edit(bool) { //TODO: remove wait param
  win_pos.GetWinPos();
  taiEdit* ie = GetTypeDef()->ie;
  if (ie != NULL) {
    const iColor* bgclr = GetEditColorInherit();
    return ie->Edit((void*)this, false, bgclr);
  }
  return false;
}

/*obs??? taFiler* DataViewer::GetFileDlg() {
  if(!taMisc::gui_active)
    return NULL;
  if(ta_file != NULL)
    return ta_file;

  ta_file = taDataView::GetFileDlg();
  taRefN::Ref(ta_file);
  MenuGroup_impl* mg_own = GET_MY_OWNER(MenuGroup_impl);
  if((mg_own != NULL) && (mg_own->ta_file != NULL)) {
    ta_file->fname = mg_own->ta_file->fname; // get file name from menugroup
  }

  TypeDef* td = GetTypeDef();
  String fltr;
  bool cmprs;
  int i;
  for(i=TA_DataViewer.members.size; i<td->members.size; i++) {
    MemberDef* md = td->members.FastEl(i);
    if(md->type->InheritsFrom(&TA_MenuGroup_impl)) {
      MenuGroup_impl* mg = (MenuGroup_impl*)md->GetOff((void*)this);
      GetFileProps(mg->el_typ, fltr, cmprs);
      if(mg->ta_file != NULL)
	taRefN::unRefDone(mg->ta_file);
      mg->ta_file = taFiler_CreateInstance(".", fltr, cmprs);
      taRefN::Ref(mg->ta_file);
    }
  }
  return ta_file;
}*/

taDataLink* DataViewer::GetDataLink_(void* el, TypeDef* el_typ, int param) {
  if (!el || !el_typ) return NULL; // subclass will have to grok

//<ADDED> 06/09/05 to handle pointers to base types
  // handle ptrs by derefing the type and the el
  if (el_typ->ptr > 0) {
    int ptr = el_typ->ptr; // need to deref this many times
    el_typ = el_typ->GetNonPtrType(); // gets base type in one step
    while (el && ptr) {
      el = *((void**)el);
      --ptr;
    }
  }
//</ADDED>

  if (!el || !el_typ) return NULL;
  if (!el_typ->iv) return NULL;
  taDataLink* rval = NULL;
  taiViewType* tiv = el_typ->iv;
  rval = tiv->GetDataLink(el);
  return rval; //NULL if not taBase
}

taiDataLink* DataViewer::GetDataLink(void* el, TypeDef* el_type, int param) {
  return (taiDataLink*)GetDataLink_(el, el_type, param);
}

// called once upon initialization of the wineve
// allocates a getfile structure for the wineve + all of its menugroups..
void DataViewer::GetFileProps(TypeDef* td, String& fltr, bool& cmprs) {
  int opt;
  if((opt = td->opts.FindContains("EXT_")) >= 0) {
    fltr = td->opts.FastEl(opt).after("EXT_");
    fltr = "*." + fltr + "*";
  }
  else
    fltr = "*";
  cmprs = false;
  if(td->HasOption("COMPRESS"))
    cmprs = true;
}

void DataViewer::GetPrintFileDlg(PrintFmt fmt) {
  static PrintFmt last_fmt = POSTSCRIPT;
  if(!taMisc::gui_active) return;
  if((print_file != NULL) && (last_fmt == fmt))  return;
  if(print_file != NULL) {
    taRefN::unRefDone(print_file);
    print_file = NULL;
  }

  String ext = GetPrintFileExt(fmt);
  String filter = "*" + ext + "*";

  print_file = taFiler_CreateInstance(".", filter, false);
  taRefN::Ref(print_file);
  last_fmt = fmt;
}

String DataViewer::GetPrintFileExt(PrintFmt fmt) {
  String ext;
  if(fmt == POSTSCRIPT)
    ext = ".ps";
  else if(fmt == JPEG)
    ext = ".jpg";
  else if(fmt == TIFF)
    ext = ".tiff";
  else if(fmt == PDF)
    ext = ".pdf";
  return ext;
}

void DataViewer::Iconify() {
  if(!taMisc::gui_active || (m_window == NULL)) return;
  if (!m_window->isMinimized())
    m_window->showMinimized();
  iconified = true;
}

bool DataViewer::IsMapped() {
  return (taMisc::gui_active && m_window);
}

int DataViewer::Load(istream& strm, TAPtr par) {
  if(taMisc::gui_active)  taMisc::Busy();
  int rval = taDataView::Load(strm, par);
  // updating of menus done by calling function..
  taMisc::DoneBusy();
  Changed(false);
  return rval;
}

void DataViewer::Lower() {
  if (!taMisc::gui_active || (m_window == NULL)) return;
  if (m_window)
    m_window->lower();
}

/*obs void DataViewer::Move(float dx, float dy) {
  // move = place with delta
  //TODO: Qt version reverses sign of dy -- verify in callers
  Place(win_pos.lft + dx, win_pos.top + dy);
} */

void DataViewer::OpenNewWindow() {
  if (!taMisc::gui_active) return;

  if (m_window != NULL) {
    SetWinName();
    return;
  }
  OpenNewWindow_impl();
  Render();
}

void DataViewer::OpenNewWindow_impl() {
  Constr_Window_impl();
  if (!m_window) return; // shouldn't happen
  m_window->is_root = m_is_root;
  m_window->Constr();
//  Constr_Menu_impl();
  menu = m_window->menu;
  // populate the toolbar list
  for (int i = 0; i < toolbars.size; ++i) {
    ToolBar* tb = toolbars.FastEl(i);
    m_window->AddToolBarMenu(tb->GetName(), i);
  }
  win_pos.SetWinPos();

  SetWinName();
  m_window->show();
  m_window->SelectionChanged(true); // initializes selection system
}

/*obs void DataViewer::Place(float left, float top){ // 0 defaults
  if (!taMisc::gui_active || (m_window == NULL)) return;
  // check to make sure that its not off the screen
  iCoord newleft, newtop;
//  ivDisplay* dsp = ivSession::instance()->default_display();
  iSize dsp = taiM->scrn_s; //note: excludes task bar
  newleft =    MIN(left, dsp.width()-win_pos.wd);
  newtop =  MIN(top ,dsp.height()-win_pos.ht - 20.0f);
  newleft = MAX((float)newleft, -win_pos.wd + 20.0f);
  newtop = MAX((float)newtop, -win_pos.ht + 20.0f);
  m_window->move(newleft.asPixel(), newtop.asPixel());
  win_pos.lft = newleft;
  win_pos.top = newtop;
} */

void DataViewer::Print(PrintFmt format, const char* fname) {
//TODO:  if(win_box != NULL) Print_impl(format, win_box, fname);
}

//TODO: Qt port
// *** warning GetPrintData() must return a Patch;
QWidget* DataViewer::GetPrintData(){
return NULL;
//TODO:  return win_box;
}

void DataViewer::Print_Data(PrintFmt format, const char* fname) {
  //TODO: verify
  QWidget* obj = this->GetPrintData();
  if (obj == NULL) return;
  Print_impl(format, obj, fname);
}

void DataViewer::Print_impl(PrintFmt format, QWidget* obj, const char* fname) {
  //NOTE: Iv version exited immediately if not gui_active, but also had the
  // sundry gui_active tests sprinkled throughout -- they are retained
  // (pending further analysis as regards this anomoly) even though they are useless

  // obj is really a patch
  if(!taMisc::gui_active) return;
/* TODO
  taMisc::Busy();
  GetWinPos();
  GetPrintFileDlg(format);
  if(print_file == NULL) {
    if(taMisc::gui_active)  taMisc::DoneBusy();
    return;
  }
  ostream* strm = NULL;
  if(fname == NULL)
    strm = print_file->SaveAs(fname);
  else {
    print_file->fname = fname;
    strm = print_file->open_write();
  }
  if(strm == NULL) {
    if (taMisc::gui_active)  taMisc::DoneBusy();
    return;
  }
  if (!((format == POSTSCRIPT) || (format == PDF))) {
#ifdef CYGWIN
    taMisc::Error("MS Windows version does not support JPEG or TIFF window saving formats -- POSTSCRIPT/PDF only!");
#else
    const ivAllocation& alloc = ((ivPatch*)obj)->allocation();
    ivCanvas* c = window->canvas();
    int xstart = c->to_pixels(alloc.left(), Dimension_X);
    int xend = c->to_pixels(alloc.right(), Dimension_X);
    int ystart = c->pheight() - c->to_pixels(alloc.top(), Dimension_Y);
    int yend = c->pheight() - c->to_pixels(alloc.bottom(), Dimension_Y);
    if(format == JPEG) {
      taiMisc::DumpJpegIv(window, print_file->fname, taMisc::jpeg_quality, xstart, ystart, xend-xstart, yend-ystart);
    }
    else if(format == TIFF) {
      taiMisc::DumpTiffIv(window, print_file->fname, xstart, ystart, xend-xstart, yend-ystart);
    }
#endif
  }
  else {			// POSTSCRIPT 0r PDF
//TODO: PDF support
    if (format == PDF) { {taMisc::Error("PDF not yet supported!"); return; }
    ivPrinter* p = new ivPrinter(strm);
    String myname = String(ivSession::instance()->classname()) +
      ": " + GetPath();
    p->prolog(myname);
    const ivAllocation& a = ((ivPatch*) obj)->allocation();
    p->resize(a.left(), a.bottom(), a.right(), a.top());
    p->page("1");
    obj->print(p, a);
    p->epilog();
    obj->undraw();
    print_file->Close();
    delete p;

    window->repair();
    ReSize(win_pos.wd, win_pos.ht); // updates everything
    //    if(InheritsFrom(TA_WinView)) {
    //      ((WinView*)this)->InitDisplay();
    //    }
  }
  if (taMisc::gui_active)  taMisc::DoneBusy(); */
}

void DataViewer::Raise() {
  if (!taMisc::gui_active || (m_window == NULL)) return;
  if (m_window)
    m_window->raise();
}

void DataViewer::Render() {
  if (!IsMapped()) return;
  inherited::Render();
}

void DataViewer::Reset() {
  if (!IsMapped()) return;
  inherited::Reset();
}

/*obs void DataViewer::ReSize(float width, float height) {
  if (!taMisc::gui_active || (m_window == NULL)) return;
  if ((width > 0.0f) && (height > 0.0f)) {
    win_pos.wd = width;
    win_pos.ht = height;
    win_pos.SetWinPos();
  } else {
    GetWinPos();
  }
} */

bool DataViewer::Save() {
  //TODO
  return false;
}

int DataViewer::Save(ostream& strm, TAPtr par, int indent) {
  taMisc::Busy();
    win_pos.GetWinPos();		// get window position *before* saving!
    SetWinName();			// this also gets file_name
    int rval = taDataView::Save(strm, par, indent);
  taMisc::DoneBusy();
  Changed(false);
  return rval;
}

void DataViewer::SetFileName(const char* fname) {
  if(fname == NULL) return;
  file_name = fname;
}

void DataViewer::SetWinName() {
  if(!taMisc::gui_active) return;
  if (m_window == NULL) return;

  String prog_nm = taiM->classname();
  String name;
  if (data()) name = data()->GetName();
  String nw_name = prog_nm + ": " + GetPath() + "(" + name + ")";
  if((ta_file != NULL) && (!ta_file->fname.empty())) {
    nw_name += String(" Fn: ") + ta_file->fname;
    SetFileName(ta_file->fname);
  }
  if(nw_name == win_name) return; // no need to set, same name!
  win_name = nw_name;
  m_window->setCaption(win_name);
}

bool DataViewer::ThisMenuFilter(MethodDef* md) {
  if((md->name == "GetAllWinPos") || (md->name == "ScriptAllWinPos") ||
     (md->name == "SetWinPos") || (md->name == "SelectForEdit") ||
     (md->name == "SelectFunForEdit")) return false;
  return true;
}

//void DataViewer::ViewWindow(float left, float top, float width, float height) {
void DataViewer::ViewWindow() {
  if (!taMisc::gui_active) return;
  if (!m_window) {
    OpenNewWindow();
//    if(((left != -1.0f) && (top != -1.0f)) || ((width != -1.0f) && (height != -1.0f)))
//      SetWinPos(left, top, width, height);
  } else {
    DeIconify();
    Raise();
  }
}

void DataViewer::WindowClosing(bool& cancel) {
  bool forced = cssiSession::quitting;
  cancel = false; // falling through to end of routine is "proceed"

  if (HasChanges()) {
    int chs = taMisc::Choice("You have unsaved changes -- do you want to save before closing?", "&Save", "&Discard Changes", "&Cancel");
    switch (chs) {
    case 0:
      if (Save()) {
        cancel = false;
        break;
      } else return;
    case 1:
      cancel = false;
      break;
    case 2:
      cancel = true;
      return;
    }
  }

  //following done only for a root win
  if (!forced && m_window->is_root) {
    int chs = taMisc::Choice("Closing this window will end the application.", "&Quit", "&Save All and Quit", "&Cancel");
    switch (chs) {
    case 1:
      if (taMisc::app) taMisc::app->SaveAll();
      //fall through
    case 0:
      cssiSession::quitting = true;
      taiM->MainWindowClosing(cancel);
      if (cancel) {
       cssiSession::quitting = false;
       return;
      }
      forced = cssiSession::quitting;
      break;
    case 2:
      cancel = true;
      return;
    }
  }

  //note: should have exited if cancel, but we check just in case
  if (!cancel)  tabMisc::Close_Obj(this);

/*was TODO  cancel = true; // falling through to end of routine is "proceed"
  if (InheritsFrom(TA_WinView) && (((WinView*)this)->mgr != NULL)) {
    WinMgr* mgr = ((WinView*)this)->mgr;
    int chs = taMisc::Choice("Close (PERMANENTLY Destroy) this VIEW of the object, or destroy the ENTIRE object including all views, losing all unsaved changes?", "Close View", "Close Obj", "Save Then Close Obj", "Cancel");
    if (chs == 3)
      return; //cancel
    else if(chs == 2) {
      taFiler* taf = mgr->GetFileDlg();
      taRefN::Ref(taf);
      ostream* strm = taf->Save();
      if((strm != NULL) && strm->good()) {
	taiMisc::RecordScript(mgr->GetPath() + ".Save(" + taf->fname + ");\n");
	mgr->SetFileName(taf->fname);
	DMEM_GUI_RUN_IF {
	  mgr->Save(*strm);
	}
      }
      taRefN::unRef(taf); //no Done, in case supplier isn't using ref counting
    } else if(chs == 0) {
      taiMisc::RecordScript(GetPath() + ".Close();\n");
      DMEM_GUI_RUN_IF {
	tabMisc::Close_Obj(this);
      }
      cancel = false;
      return;
    }
    taiMisc::RecordScript(mgr->GetPath() + ".Close();\n");
    DMEM_GUI_RUN_IF {
      tabMisc::Close_Obj(mgr);
    }
  } else {
    int chs = taMisc::Choice("Ok to Close (PERMANENTLY Destroy) this object, losing all unsaved changes?", "Close", "Save Then Close", "Cancel");
    if(chs == 2)
      return; //cancel
    else if (chs == 1) {
      taFiler* taf = GetFileDlg();
      taRefN::Ref(taf);
      ostream* strm = taf->Save();
      if((strm != NULL) && strm->good()) {
	taiMisc::RecordScript(GetPath() + ".Save(" + taf->fname + ");\n");
	SetFileName(taf->fname);
	DMEM_GUI_RUN_IF {
	  Save(*strm);
	}
      }
      taRefN::unRefDone(taf);
    }
    taiMisc::RecordScript(GetPath() + ".Close();\n");
    DMEM_GUI_RUN_IF {
      tabMisc::Close_Obj(this);
    }
  }
  cancel = false; */
}


//////////////////////////
//   iTabDataViewer 	//
//////////////////////////

iTabDataViewer::iTabDataViewer(DataViewer* viewer_, QWidget* parent)
: iDataViewer(viewer_, parent)
{
  init();
}

/* iTabDataViewer::iTabDataViewer(DataViewer* viewer_, QWidget* parent, WFlags fl)
: iDataViewer(viewer_, parent, fl)
{
  init();
} */

iTabDataViewer::~iTabDataViewer()
{
  delete m_tabViews;
  m_tabViews = NULL;
}

void iTabDataViewer::init() {
  m_tabViews = new  iTabView_PtrList();
  m_curTabView = NULL;
}

void iTabDataViewer::AddPanel(iDataPanel* panel) {
  tabView()->AddPanel(panel);
}

void iTabDataViewer::AddPanelNewTab(iDataPanel* panel) {
  tabView()->AddPanelNewTab(panel);
}

void iTabDataViewer::AddTab() {
  tabView()->AddTab();
}

iTabView* iTabDataViewer::AddTabView(QWidget* parCtrl, iTabView* splitBuddy) {
  iTabView* rval;
  if (splitBuddy) { // split
    rval = new iTabView(this, parCtrl);
    m_tabViews->Add(rval);
    //TODO:
  } else { // no split
    rval = new iTabView(this, parCtrl);
    m_tabViews->Add(rval);
  }
  rval->show();
  return rval;
}

void iTabDataViewer::CloseTab() {
  tabView()->CloseTab();
}

void iTabDataViewer::Closing(bool forced, bool& cancel) {
  tabView()->Closing(forced, cancel);
}

void iTabDataViewer::Constr_Menu_impl() {
  iDataViewer::Constr_Menu_impl();
  viewSplitVerticalAction = AddAction(new taiAction(this, "Split &Vertical", QKeySequence("Ctrl+Shift+L"),
     "viewSplitVerticalAction"));
  viewSplitVerticalAction->AddTo(viewMenu);
  connect( viewSplitVerticalAction, SIGNAL( activated() ), this, SLOT( viewSplitVertical() ) );

  viewSplitHorizontalAction = AddAction(new taiAction(this, "Split &Horizontal", QKeySequence("Ctrl+Shift+T"),
     "viewSplitHorizontalAction"));
  viewSplitHorizontalAction->AddTo(viewMenu);
  connect(viewSplitHorizontalAction, SIGNAL( activated() ), this, SLOT(viewSplitHorizontal() ) );

  viewCloseCurrentViewAction = AddAction(new taiAction(this, "Close &Current View", QKeySequence("Ctrl+Shift+R"),
     "viewCloseCurrentViewAction"));
  viewCloseCurrentViewAction->setEnabled(false); // only enabled for multi tabs
  viewCloseCurrentViewAction->AddTo(viewMenu);
  connect(viewCloseCurrentViewAction, SIGNAL( activated() ), this, SLOT(viewCloseCurrentView() ) );

}

iDataPanel* iTabDataViewer::MakeNewDataPanel_(taiDataLink* link) {
  iDataPanel* rval = NULL;
  TypeDef* typ = link->GetDataTypeDef();
  //typ can be null for non-taBase classes
  if ((typ == NULL) || (typ->iv == NULL)) return NULL;
  taiViewType* tiv = typ->iv;
  rval = tiv->CreateDataPanel(link);
  return rval;
}

void iTabDataViewer::selectionChangedEvent(QCustomEvent* ev) {
  //TODO: should actually check if old panel=new panel, since theoretically, two different
  // gui items can have the same datalink (i.e., underlying data)
  iDataPanel* pn = NULL;
  iDataPanel* new_pn = NULL;
  bool done;
  if (m_curTabView) {
    pn = m_curTabView->cur_panel();
    ISelectable* ci = curItem();
    // first, check for the trivial cases (esp. when multi-selections are made), no change
    if (!ci) goto end; // no selected item, so no change
    if (!pn) {
      new_pn = m_curTabView->GetDataPanel(ci->link());
    } else {
      if (pn->lockInPlace()) goto end; // locked, user must create one and change
      if (pn->link() == ci->link()) goto end; //this is panel for item,


      new_pn = m_curTabView->GetDataPanel(ci->link());
      //if not exists, or is dirty, make a new one, else just switch
      if (!pn || pn->dirty()) {
        // first try activating an existing panel -- don't keep creating new ones
        done = m_curTabView->ActivatePanel(ci->link());
        if (done) goto end;
        // ok, no panel for that link, so need to create a new one
        m_curTabView->AddPanelNewTab(new_pn);
      }
    }
    m_curTabView->SetPanel(new_pn);
  }
end:
  inherited::selectionChangedEvent(ev);
}

void iTabDataViewer::TabView_Destroying(iTabView* tv) {
  int idx = m_tabViews->Find(tv);
  m_tabViews->Remove(idx);
  if (m_curTabView != tv) return;
  // focus next, if any, or prev, if any
  if (idx >= m_tabViews->size) --idx;
  TabView_Selected(m_tabViews->SafeEl(idx)); // NULL if no more
}

void iTabDataViewer::TabView_Selected(iTabView* tv) {
  if (m_curTabView == tv) return;
  if (m_curTabView) {
    m_curTabView->Activated(false);
  }
  m_curTabView = tv;
  if (m_curTabView) {
    m_curTabView->Activated(true);
  }
}

void iTabDataViewer::SetPanel(iDataPanel* panel) {
  tabView()->SetPanel(panel);
}

void iTabDataViewer::UpdateTabNames() { // called by a datalink when a tab name might have changed
  tabView()->UpdateTabNames();
}

void iTabDataViewer::viewCloseCurrentView() { // closes split, unless it is last
  // We are going to delete the current splitter and the current tabview.
  // We will reparent the current tabview's buddy to the grandparent splitter.

  if (m_tabViews->size < 2) return; //shouldn't happen, because should be disabled
  if (m_tabViews->size == 2) { // will only be one left, so disable closing
    viewCloseCurrentViewAction->setEnabled(false); // only enabled for multi tabs
  }
  iTabView* closer = m_curTabView;
  // get splitter parent, and the buddy tabview
  //we check type, as a precaution, even though it should be a splitter
  if (!closer->parentWidget()->inherits("QSplitter")) {
    cerr << "TabDataViewer::viewCloseCurrentView Unexpected parent for tabview\n";
    return;
  }
  QSplitter* par_spl = (QSplitter*)closer->parentWidget();
  iTabView* buddy = NULL;
  const QObjectList& it = par_spl->children();
  QObject* obj;
  foreach (obj, it) {
    if (obj == closer) continue;
    if (!obj->inherits("iTabView")) continue;
    buddy = (iTabView*) obj;
    break;
  }
  if (!buddy) {
    cerr << "TabDataViewer::viewCloseCurrentView Unexpected could not find buddy\n";
    return;
  }


  // find the splitter grandparent of pair we are modifying -- we get sizes of its panes to fix up after
  // QSplitter visual widget order is independent of children() order, and is inaccessible, so
  // we determine if the parent is visually earlier than its buddy, by comparing left/top values
  bool move_first = false; // will need to move the buddy back to first
  QSplitter* gpar_spl = NULL;
  QList<int> gpar_spl_sizes;
  if (par_spl->parentWidget()->inherits("QSplitter")) {
    gpar_spl = (QSplitter* )par_spl->parentWidget();
    gpar_spl_sizes = gpar_spl->sizes(); // easiest way to set new list is to get an old one

    const QObjectList& it = gpar_spl->children();
    QWidget* widg;
    foreach (obj, it) {
      if (obj == par_spl) continue;
      if (!obj->isWidgetType()) continue;
      widg = (QWidget*)obj;

      if ((par_spl->x() < widg->x()) || (par_spl->y() < widg->y())) {
        move_first = true;
        break;
      }
    }
  }
  // determine placement of old splitter par in the grandparent

  // ok, set the new tabview as selected, and do close processing on old one
  TabView_Selected(buddy);
  bool dummy;
  closer->Closing(true, dummy);

  // reparent the remaining tabview to its grandparent
  buddy->reparent(par_spl->parentWidget(), 0, QPoint(), true);
  par_spl->hide();
  par_spl->deleteLater();
  // if grandparent (aka new parent) was splitter (it should have been), then fix up panel order, and sizes
  if (gpar_spl) {
    if (move_first) gpar_spl->moveToFirst(buddy);
    else  gpar_spl->moveToLast(buddy);
    gpar_spl->setSizes(gpar_spl_sizes);
  }
}

void iTabDataViewer::viewSplit(int o) {
  //NOTE: 'o' is opposite from type of split we are doing, since it represents the splitter type

  // We are going to take the current tab view, replace it (visually) with a splitter, then
  // put the tab view, and a new tab view, in this splitter. We will also fix up the sizes and
  // orders of the splitter, and a possible parent splitter, so they are the same as before the split.

  iTabView* old = m_curTabView;
  QWidget* old_par = old->parentWidget();

  // if old parent is splitter, get it, and save the pane sizes, so we can resize the same after rebuilding
  QSplitter* old_par_spl = NULL;
  bool old_par_is_spl = (old_par->inherits("QSplitter"));
  QList<int> par_spl_sizes;
  bool move_first = false; // will need to move the new splitter back to first
  if (old_par_is_spl) {
    old_par_spl = (QSplitter* )old_par;
    par_spl_sizes = old_par_spl->sizes(); // easiest way to set new list is to get an old one

    // QSplitter visual widget order is independent of children() order, and is inaccessible, so
    // we determine order by comparing left/top values
    const QObjectList& it = old_par_spl->children();
    QObject* obj;
    QWidget* widg;
    foreach (obj, it) {
      if (obj == old) continue;
      if (!obj->isWidgetType()) continue;
      widg = (QWidget*)obj;

      if ((old->x() < widg->x()) || (old->y() < widg->y())) {
        move_first = true;
        break;
      }
    }
  }

  // get available space (we will divide it equally)
  int spc = ((Qt::Orientation)o == Qt::Horizontal) ? old->width() : old->height();

  // create a new splitter whose parent is the old parent of the splitee tabview
  QSplitter* splNew = new QSplitter((Qt::Orientation)o, old->parentWidget());
  // reparent the existing tabview to the new splitter
  old->reparent(splNew, 0, QPoint(), true);
  splNew->show();

  // create the new tab view, and split space evenly
  //iTabView* new_tv =
  AddTabView(splNew, old);
  QList<int> new_spl_sizes = splNew->sizes(); // easiest way to set new list is to get an old one
  new_spl_sizes[0] = spc / 2;
  new_spl_sizes[1] = spc / 2;
  splNew->setSizes(new_spl_sizes);

  // fix up the visual order and size to the way it was before reparenting
  if (old_par_is_spl) {
    if (move_first)// will need to move our new pair back into first place in parent splitter
        old_par_spl->moveToFirst(splNew);
    else
        old_par_spl->moveToLast(splNew);
    old_par_spl->setSizes(par_spl_sizes);
  }
  // divide the space evenly
  viewCloseCurrentViewAction->setEnabled(true); // only enabled for multi tabs
}

void iTabDataViewer::viewSplitVertical() {
  viewSplit(Qt::Horizontal); //note: Hor is the orientation of the splitter, which is opposite to how we split window
}

void iTabDataViewer::viewSplitHorizontal() {
  viewSplit(Qt::Vertical); //note: Ver is the orientation of the splitter, which is opposite to how we split window
}

/*obs
//////////////////////////
//    iPanelTab 	//
//////////////////////////

iPanelTab::iPanelTab(iDataPanel* panel_)
: QTab()
{
  m_panel = NULL;
  SetPanel(panel_, true);
}

iPanelTab::~iPanelTab() {
  if (m_panel) {
    m_panel = NULL;
  }
} */


//////////////////////////
// 	iTabBar 	//
//////////////////////////

iTabBar::iTabBar(iTabView* parent_)
:QTabBar((QWidget*)parent_)
{
  defPalette = palette();

//  defBackgroundColor = paletteBackgroundColor();
//  defBaseColor = colorGroup().base();
}

iTabBar::~iTabBar() {
}

void iTabBar::contextMenuEvent(QContextMenuEvent * e) {
  QMenu* menu = new QMenu(this);
  Q_CHECK_PTR(menu);
  tabView()->FillTabBarContextMenu(menu);
  menu->exec(QCursor::pos());
  delete menu;
}

void iTabBar::mousePressEvent(QMouseEvent* e) {
  QTabBar::mousePressEvent(e);
  if (tabView()->m_viewer_win)
    tabView()->m_viewer_win->TabView_Selected(tabView());
}
/*
 void iTabBar::paint(QPainter* p, QTab* t, bool selected ) const {
TODO: hangs in here  iDataPanel* panel = ((iPanelTab*)t)->panel();
  const iColor* bc = NULL;
  if (panel != NULL) bc = panel->GetTabColor(selected);
  if (bc != NULL) {
    QPalette pal = QPalette(defPalette);
    if (selected) {
      pal.setColor(QPalette::Active, QColorGroup::Background, *bc);
      pal.setColor(QPalette::Inactive, QColorGroup::Background, *bc); //for when window is not active
    } else {
      pal.setColor(QPalette::Active, QColorGroup::Button, *bc);
      pal.setColor(QPalette::Inactive, QColorGroup::Button, *bc);
    }
    ((QTabBar*)this)->setPalette(pal);
//found to hang on this line:    QTabBar::paint(p, t, selected);
    ((QTabBar*)this)->setPalette(defPalette);
  } else 
    QTabBar::paint(p, t, selected);
}*/

iDataPanel* iTabBar::panel(int idx) {
  QVariant data(tabData(idx)); // returns NULL variant if out of range
  if (data.isNull() || !data.isValid()) return NULL;
  intptr_t rval = data.value<intptr_t>(); //NOTE: if probs in msvc, use the qvariant_cast thingy
  return (iDataPanel*)rval;
  
}

int iTabBar::addTab(iDataPanel* panel) {
  int idx = QTabBar::addTab("");
  setTabData(idx, QVariant((intptr_t)0)); // set to valid null value
  SetPanel(idx, panel);
  return idx;
}

void iTabBar::SetPanel(int idx, iDataPanel* value, bool force) {
  QVariant data(tabData(idx)); // returns NULL variant if out of range
  iDataPanel* m_panel = NULL;
  if (!data.isNull() && data.isValid())
    m_panel = (iDataPanel*)data.value<intptr_t>(); //NOTE: if probs in msvc, use the qvariant_cast thingy
  
  if ((m_panel == value) && !force) return;
  m_panel = value;
  if (m_panel) {
    setTabText(idx, m_panel->TabText());
//    m_panel->mtab_cnt++;
    // make sure we show latest data
    m_panel->GetImage();
  } else {
    setTabText(idx, "");
  }
  data = (intptr_t)m_panel;
  setTabData(idx, data);
}



//////////////////////////
//   iTabView		//
//////////////////////////

iTabView::iTabView(QWidget* parent)
:QWidget(parent)
{
  m_viewer_win = NULL;
  init();
}

iTabView::iTabView(iTabDataViewer* data_viewer_, QWidget* parent)
:QWidget(parent)
{
  m_viewer_win = data_viewer_;
  init();
}


iTabView::~iTabView()
{
  if (m_viewer_win)
    m_viewer_win->TabView_Destroying(this);
}


void iTabView::init() {
  layDetail = new QVBoxLayout(this);
  tbPanels = new iTabBar(this);
  layDetail->addWidget(tbPanels);
  wsPanels = new Q3WidgetStack(this);
  layDetail->addWidget(wsPanels);
//obs add a dummy data panel with id=0 to show blank
//obs  wsPanels->addWidget(new iDataPanel(NULL), 0);
  // add the default tab
/*Qt3  iPanelTab* pt  = new iPanelTab();
  tbPanels->addTab(pt); */
  
  tbPanels->addTab("");
  connect(tbPanels, SIGNAL(currentChanged(int)),
      this, SLOT(panelSelected(int)) );
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
      tbPanels->setCurrentIndex(i);
//???      panelSelected(id);
      return true;
    }
  }
  return false;
}

void iTabView::AddPanel(iDataPanel* panel) {
  wsPanels->addWidget(panel);
  panel->m_tabView = this;
  panels.Add(panel);
}

void iTabView::AddPanelNewTab(iDataPanel* panel) {
  AddPanel(panel);
/*Qt3  iPanelTab* pt = new iPanelTab(panel);
  int id = tbPanels->addTab(pt); */
  int id = tbPanels->addTab(panel);
  tbPanels->setCurrentTab(id);
}

void iTabView::AddTab() {
/*Qt3  iPanelTab* pt  = (iPanelTab*)tbPanels->tab(tbPanels->currentTab());
  pt = new iPanelTab(pt->panel());
  int id = tbPanels->addTab(pt); */
  iDataPanel* pan  = tbPanels->panel(tbPanels->currentIndex());
  int id = tbPanels->addTab(pan);
  tbPanels->setCurrentTab(id);
}

void iTabView::CloseTab() {
  // don't allow closing last tab for a modified panel
  if (tbPanels->count() > 1) {
    tbPanels->removeTab(tbPanels->currentIndex());
    panelSelected(tbPanels->currentIndex()); // needed to resync proper panel with tab
  } else { // last tab
    panelSelected(-1);
  }
}

void iTabView::Closing(bool forced, bool& cancel) {
  // close all panels
  for (int i = panels.size - 1; i >= 0; --i) {
    iDataPanel* panel = panels.FastEl(i);
    panel->Closing(forced, cancel);
    if (cancel && !forced) return; // can stop now

    RemoveDataPanel(panel); // note: removes from tabs, and deletes tabs
  }
}

iDataPanel* iTabView::cur_panel() {
  iDataPanel* rval  = tbPanels->panel(tbPanels->currentIndex());
  return rval; // could be NULL if empty
}

void iTabView::DataPanelDestroying(iDataPanel* panel) {
  RemoveDataPanel(panel);
}

void iTabView::FillTabBarContextMenu(QMenu* contextMenu) {
  taiAction* act = new taiAction("&Add Tab",  this, SLOT(AddTab()), CTRL+ALT+Key_N );
  // note: need to parent
  act->setParent(contextMenu);
  contextMenu->addAction(act);
  act = new taiAction("&Close Tab", this, SLOT(CloseTab()), CTRL+ALT+Key_Q );
  act->setParent(contextMenu);
  contextMenu->addAction(act);
  act->setEnabled((tbPanels->count() > 1));
}

iDataPanel* iTabView::panel(int idx) {
  return panels.SafeEl(idx);
}

int iTabView::panel_count() {
  return panels.size;
}

iDataPanel* iTabView::GetDataPanel(taiDataLink* link) {
  iDataPanel* rval;
  for (int i = 0; i < panels.size; ++i) {
    rval = panels.FastEl(i);
    if (rval->m_link == link) return rval;
  }

  rval = viewer_win()->MakeNewDataPanel_(link);
  if (rval != NULL) {
    AddPanel(rval);
    rval->show(); // may be required in some contexts
  }
  return rval;
}

void iTabView::panelSelected(int idx) {
  iDataPanel* panel = NULL;
  if (idx >= 0) panel = tbPanels->panel(idx); 
  if (panel == NULL) wsPanels->raiseWidget((int)0);
  else            wsPanels->raiseWidget(panel);
  if (m_viewer_win)
    m_viewer_win->TabView_Selected(this);
}

void iTabView::RemoveDataPanel(iDataPanel* panel) {
  panel->m_tabView = NULL;
  panels.Remove(panel);
  wsPanels->removeWidget(panel); // superfluous, but safe, if panel is destroying
  // remove any associated tabs, except leave last tab (will get deleted anyway if we are
  // destructing)
  for (int i = tbPanels->count() - 1; i >= 0; --i) {
    iDataPanel* dp = tbPanels->panel(i);
    if (dp == panel) {
      if ((i > 0) || (tbPanels->count() > 1)) {
        tbPanels->removeTab(i);
      } else {
        tbPanels->SetPanel(0, NULL); // no panel
      }
    }
  }

}

void iTabView::SetPanel(iDataPanel* panel) {
  wsPanels->raiseWidget(panel);
  tbPanels->SetPanel(tbPanels->currentIndex(), panel);
}

void iTabView::UpdateTabNames() { // called by a datalink when a tab name might have changed
  for (int i = 0; i < tbPanels->count(); ++i ) {
    iDataPanel* pan = tbPanels->panel(i);
    if (pan == NULL) continue; // shouldn't happen...
    tbPanels->setTabText(i, pan->TabText());
  }
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
  setFrameStyle(NoFrame | Plain);
  scr = new QScrollArea(this);
  scr->setWidgetResizable(true);
  QVBoxLayout* lay = new QVBoxLayout(this);
  lay->addWidget(scr);

  dl_->AddDataClient(this); // sets our m_link variable
}

iDataPanel::~iDataPanel() {
  if (m_tabView)
    m_tabView->DataPanelDestroying(this);
}

void iDataPanel::ctrl_focusInEvent(QFocusEvent* ev) {
  viewer_win()->SetClipboardHandler(this, SLOT(this_GetEditActionsEnabled(int&)),
      SLOT(this_EditAction(int)) );
}

void iDataPanel::DataChanged_impl(int dcr, void* op1, void* op2) {
  if (dcr == DCR_UPDATE_AFTER_EDIT) {
    if (tabView())
      tabView()->UpdateTabNames(); //in case any changed
  }
}

void iDataPanel::setCentralWidget(QWidget* widg) {
  scr->setWidget(widg);
  widg->show(); 
}

String iDataPanel::TabText() const {
/*Qt3  ISelectable* ci = viewer_win()->curItem();
  if (ci) {
    return ci->view_name();
  } else return _nilString; */
  return link()->GetDisplayName();
}

void iDataPanel::this_EditAction(int param) {
  EditAction(param);
}

void iDataPanel::this_GetEditActionsEnabled(int& ea) {
  ea = GetEditActions();
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
}

iDataPanelFrame::~iDataPanelFrame() {
}

void iDataPanelFrame::ClosePanel() {
  bool cancel = false;
  Closing(true, cancel); // forced, ignore cancel
  if (m_tabView) // effectively unlink from system
    m_tabView->DataPanelDestroying(this);
  if (!m_dps) // if in a panelset, we let panelset destroy us
    deleteLater(); // per Qt specs, defer deletions to avoid issues
}

void iDataPanelFrame::DataLinkDestroying(taDataLink*) {
  if (!m_dps) // if in a panelset, we let panelset destroy us
    ClosePanel();
}

void iDataPanelFrame::GetImage() {
  if (!HasChanged()) GetImage_impl();
}

taiDataLink* iDataPanelFrame::par_link() const {
  if (m_dps) return m_dps->par_link();
  else       return (m_tabView) ? m_tabView->par_link() : NULL;
}

MemberDef* iDataPanelFrame::par_md() const {
  if (m_dps) return m_dps->par_md();
  else       return (m_tabView) ? m_tabView->par_md() : NULL;
}

String iDataPanelFrame::TabText() const {
  if (m_dps) return m_dps->TabText();
  else       return inherited::TabText();
}

iTabDataViewer* iDataPanelFrame::viewer_win() const {
  if (m_dps) return m_dps->viewer_win();
  else       return (m_tabView) ? m_tabView->viewer_win() : NULL;
}


//////////////////////////
//    iViewPanelFrame 	//
//////////////////////////

iViewPanelFrame::iViewPanelFrame(taDataView* dv_)
:inherited((taiDataLink*)dv_->GetDataLink()) //NOTE: link not created yet during loads
{
  m_dv = dv_;
  taDataLink* dl = dv_->GetDataLink();
  dl->AddDataClient(this);
}

iViewPanelFrame::~iViewPanelFrame() {
}

void iViewPanelFrame::ClosePanel() {
  bool cancel = false;
  Closing(true, cancel); // forced, ignore cancel
  if (m_tabView) // effectively unlink from system
    m_tabView->DataPanelDestroying(this);
  deleteLater(); // per Qt specs, defer deletions to avoid issues
}

void iViewPanelFrame::DataLinkDestroying(taDataLink*) {
  m_dv = NULL;
  ClosePanel();
}

void iViewPanelFrame::GetImage() {
  if (!m_dv) return;
  ++updating;
  GetImage_impl();
  --updating;
}

void iViewPanelFrame::InitPanel() {
  if (!m_dv) return;
  ++updating;
  InitPanel_impl();
  --updating;
}

String iViewPanelFrame::TabText() const {
  if (m_dv) return m_dv->GetLabel();
  else      return inherited::TabText();
}

iTabDataViewer* iViewPanelFrame::viewer_win() const {
  return (m_tabView) ? m_tabView->viewer_win() : NULL;
}


//////////////////////////
//    iDataPanelSet 	//
//////////////////////////

iDataPanelSet::iDataPanelSet(taiDataLink* link_)
:inherited(link_)
{
  cur_panel_id = -1;
  QWidget* widg = new QWidget();
  layDetail = new QVBoxLayout(widg);
  buttons = new Q3HButtonGroup(); // used invisibly
  buttons->setExclusive(true);
  buttons->setFont(taiM->buttonFont(taiMisc::sizSmall));
  frmButtons = new QFrame(this);
  frmButtons->setFrameShape(QFrame::Box);
  frmButtons->setFrameShadow(QFrame::Sunken);
  layButtons = new QHBoxLayout(frmButtons);
  layButtons->setMargin(2);
  layButtons->setSpacing(taiM->hspc_c);
  layDetail->addWidget(frmButtons);

  wsSubPanels = new Q3WidgetStack(this);
  layDetail->addWidget(wsSubPanels, 1);
  setCentralWidget(widg);

  connect(buttons, SIGNAL(pressed(int)), this, SLOT(btn_pressed(int)));
}

iDataPanelSet::~iDataPanelSet() {
  delete buttons; // unowned
}

void iDataPanelSet::AddSubPanel(iDataPanelFrame* pn) {
  pn->m_dps = this;
  panels.Add(pn);
  int id = panels.size - 1;
  wsSubPanels->addWidget(pn, id);
  QPushButton* but = new QPushButton(frmButtons);
  but->setMaximumHeight(taiM->button_height(taiMisc::sizSmall));
  if (id == 0) but->setDown(true); // first button should be down
  but->setToggleButton(true);
  but->setText(pn->panel_type());
  buttons->insert(but);
  layButtons->addWidget(but);
  but->show();
}
void iDataPanelSet::AllSubPanelsAdded() {
  layButtons->addStretch();
}

void iDataPanelSet::btn_pressed(int id) {
  set_cur_panel_id(id);
}

void iDataPanelSet::Closing(bool forced, bool& cancel) {
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    pn->Closing(forced, cancel);
    if (cancel) return;
  }
}

void iDataPanelSet::ClosePanel() {
  for (int i = panels.size - 1; i >= 0 ; --i) {
    iDataPanel* pn = panels.FastEl(i);
    pn->ClosePanel();
  }
  if (m_tabView) // effectively unlink from system
    m_tabView->DataPanelDestroying(this);
  deleteLater();
}

void iDataPanelSet::GetImage() {
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    pn->GetImage();
  }
}

const iColor* iDataPanelSet::GetTabColor(bool selected) const {
  iDataPanel* pn = cur_panel();
  if (pn) return pn->GetTabColor(selected);
  else    return inherited::GetTabColor(selected);
}

bool iDataPanelSet::HasChanged() {
  for (int i = 0; i < panels.size; ++i) {
    iDataPanel* pn = panels.FastEl(i);
    if (pn->HasChanged()) return true;
  }
  return false;
}

void iDataPanelSet::removeChild(QObject* obj) {
  panels.Remove_(obj); // harmless if not a panel
  inherited::removeChild(obj);
}

void iDataPanelSet::set_cur_panel_id(int cpi) {
  if (cur_panel_id == cpi) return;
  cur_panel_id = cpi; // set this early, to prevent unexpected calls from the signal
  iDataPanel* pn = panels.SafeEl(cpi);
  if (!pn) return; //shouldn't happen
  wsSubPanels->raiseWidget(pn);
  buttons->setButton(cpi); // for when called programmatically
  //TODO: maybe something to change tab color
}


//////////////////////////
//    iListViewItem 	//
//////////////////////////

class DataNodeDeleter: public QObject { // enables nodes to be put on deferredDelete list
public:
  iListViewItem* node;
  DataNodeDeleter(iListViewItem* node_): QObject() {node = node_;}
  ~DataNodeDeleter() {delete node;}
};

iListViewItem::iListViewItem(taiDataLink* link_, MemberDef* md_, iListViewItem* node,
  iListViewItem* last_child_, const String& tree_name, int flags_)
:inherited(node, last_child_, tree_name)
{
  init(link_, md_, flags_);
}

iListViewItem::iListViewItem(taiDataLink* link_, MemberDef* md_, Q3ListView* parent,
  iListViewItem* last_child_, const String& tree_name, int flags_)
:inherited(parent, last_child_, tree_name)
{
  init(link_, md_, flags_);
}

void iListViewItem::init(taiDataLink* link_, MemberDef* md_, int flags_) {
  link_->AddDataClient(this); // sets link
  m_md = md_;
  flags = flags_;
  setDragEnabled(flags & DNF_CAN_DRAG);
  setDropEnabled(!(flags & DNF_NO_CAN_DROP));
}

iListViewItem::~iListViewItem() {
}

bool iListViewItem::acceptDrop (const QMimeSource* mime) const {
  taiMimeSource* ms = taiMimeSource::New(mime);
  int ea = GetEditActions_(ms);
//  bool rval = (ea & taiClipData::EA_DROP_OPS);
  bool rval = (ea & taiClipData::EA_DROP_MOVE); //NOTE: we only use MOVE for drag/drop
  delete ms;
  return rval;
}

/* nnint iListViewItem::compare (Q3ListViewItem* item, int col, bool ascending) const {
  // if we have a visual parent, delegate to its data link, otherwise just do the default
  iListViewItem* par = parent();
  if (par)  {
    int rval = par->link->CompareChildItems(this, (iListViewItem*)item);
    if (ascending) return rval;
    else return rval * -1;
  } else
    return Q3ListViewItem::compare(item, col, ascending);
} */

void iListViewItem::DataLinkDestroying(taDataLink*) {
  delete this;
}

void iListViewItem::DecorateDataNode() {
  // set a + expansion handle if we (potentially) have child items
  int bmf = 0;
  int flags_supported = 0;
//  if (node->flags & iListViewItem::DNF_IS_FOLDER) bmi = node->isOpen() ? NBI_FOLDER_OPEN : NBI_FOLDER_CLOSED;
  if (isOpen()) bmf |= NBF_FOLDER_OPEN;
  const QPixmap* pm = link()->GetIcon(bmf, flags_supported);
  //TODO (or in GetIcon somewhere) add link iconlet and any other appropriate mods
  if (pm != NULL)
    setPixmap(0, *pm);
}

/*void iListViewItem::dragEntered() {
  Q3ListViewItem::dragEntered();
    if ( type() != Dir ||
	 type() == Dir && !QDir( itemFileName ).isReadable() )
	return;

    ( (QtFileIconView*)iconView() )->setOpenItem( this );
//TODO:    assertTimer(true);
//    timer->start( 1500 );
}

void iListViewItem::dragLeft() {
  Q3ListViewItem::dragLeft();
    if ( type() != Dir ||
	 type() == Dir && !QDir( itemFileName ).isReadable() )
	return;

    if (timer) {
      timer->stop();
      assertTimer(false);
    }
}*/

void iListViewItem::dropped(QDropEvent* ev) {
  taiMimeSource* ms = taiMimeSource::New(ev);
  int ea = 0;
  //NOTE: ev->action() was always observed to be Copy, whether the + was shown or not in the UI
  // NOTE: we always force clip ops to be MOVE in this version of the app
  ea |= taiClipData::EA_DROP_MOVE;
//TODO: always seems to be Copy even if + is not shown in Ui
/* not working:  switch (ev->action()) {
  case QDropEvent::Copy: ea |= taiClipData::EA_DROP_COPY; break;
  case QDropEvent::Link: ea |= taiClipData::EA_DROP_LINK; break;
  case QDropEvent::Move: ea |= taiClipData::EA_DROP_MOVE; break;
  default: break;
  } */
  EditActionD_impl_(ms, ea);
  delete ms;

/* todo: not using drop menu
    Q3PopupMenu* menu = new Q3PopupMenu(listView());
  Q_CHECK_PTR(menu);
  int last_id = -1;

  menu->insertItem("&Move Here", browser(), SLOT(mnuBrowseNodeDrop(int)), 0, ++last_id);
  menu->setItemParameter(last_id, BDA_MOVE );
  menu->insertItem("&Copy Here", browser(), SLOT(mnuBrowseNodeDrop(int)), 0, ++last_id);
  menu->setItemParameter(last_id, BDA_COPY );
  menu->insertItem("&Link Here", browser(), SLOT(mnuBrowseNodeDrop(int)), 0, ++last_id);
  menu->setItemParameter(last_id, BDA_LINK );
  menu->insertItem("Move as &Subgroup of This Item", browser(), SLOT(mnuBrowseNodeDrop(int)), 0, ++last_id);
  menu->setItemParameter(last_id, BDA_MOVE_AS_SUBGROUP );
  menu->insertItem("Move as &Subitem of This Item", browser(), SLOT(mnuBrowseNodeDrop(int)), 0, ++last_id);
  menu->setItemParameter(last_id, BDA_MOVE_AS_SUBITEM );
  menu->insertSeparator();
  menu->insertItem("C&ancel", browser(), SLOT(mnuBrowseNodeDrop(int)), 0, ++last_id);
  menu->setItemParameter(last_id, -1 );

  //TODO: any for us last (ex. delete)
  if (last_id > -1) { //only show if any items!
    browser()->mnuBrowseNodeDrop_param = -1; // in case Esc used
    QPoint pos = listView()->mapToGlobal(ev->pos() );
    menu->exec(pos);
    switch (browser()->mnuBrowseNodeDrop_param) {
    case BDA_MOVE: {
    }
      break;
    case BDA_COPY: {
    }
      break;
    case BDA_LINK: {
    }
      break;
    case BDA_MOVE_AS_SUBGROUP: {
    }
      break;
    case BDA_MOVE_AS_SUBITEM: {
    }
      break;
    }
  }
  delete menu; */
}

void iListViewItem::GetEditActionsS_impl_(int& allowed, int& forbidden) const {
  if (flags & DNF_IS_MEMBER) {
    forbidden |= (taiClipData::EA_CUT | taiClipData::EA_DELETE);
  }
  ISelectable::GetEditActionsS_impl_(allowed, forbidden);
}

String iListViewItem::view_name() const {
  return text(0);
}

//////////////////////////////////
// 	taiListDataNode 	//
//////////////////////////////////

taiListDataNode::taiListDataNode(int num_, iListDataPanel* panel_,
   taiDataLink* link_, Q3ListView* parent_, taiListDataNode* last_child_,int flags_)
:inherited(link_, NULL, parent_, last_child_, "", (flags_ | DNF_IS_LIST_NODE))
{
  num = num_;
  panel = panel_;
}

taiListDataNode::~taiListDataNode() {
}

int taiListDataNode::compare(Q3ListViewItem *i, int col, bool asc) const {
  if (col > 0)
    return Q3ListViewItem::compare(i, col, asc);
  else {
    taiListDataNode* ldn = (taiListDataNode*)i;
    return num - ldn->num;
  }
}

taiDataLink* taiListDataNode::par_link() const {
  return (panel) ? panel->par_link() : NULL;
}

MemberDef* taiListDataNode::par_md() const {
  return (panel) ? panel->par_md() : NULL;
}

QString taiListDataNode::text(int col) const {
  if (col > 0)
    return Q3ListViewItem::text(col);
  else
    return QString::number(num);
}

IDataViewHost* taiListDataNode::host() const {
  return (panel) ? panel->viewer_win() : NULL;
}


//////////////////////////
//    iLDPListView 	//
//////////////////////////

class iLDPListView: public Q3ListView {
typedef Q3ListView inherited;
public:
  iListDataPanel* panel;
  iLDPListView(iListDataPanel* parent = NULL, const char* name = NULL);

protected:

//  iListViewItem*	focus_item;

  Q3DragObject*  	dragObject(); // override
  void 			focusInEvent(QFocusEvent* ev); // override
//  void 			focusOutEvent(QFocusEvent* ev); // override

/*  void		doDragFocusEvent(QDropEvent* ev); // code is the same for all enter/move/leave events for focus
  void		contentsDragEnterEvent(QDragEnterEvent* ev); // override
  void 		contentsDragMoveEvent(QDragMoveEvent * ev); // override
  void 		contentsDragLeaveEvent(QDragLeaveEvent * ev); // override
  void		setDropFocus(iListViewItem* item); // draws a focus rec around the target (qt doesn't do this by default) */
};

iLDPListView::iLDPListView(iListDataPanel* parent, const char* name)
: Q3ListView(parent, name)
{
  panel = parent;
//  focus_item = NULL;
  setAcceptDrops(true);
  viewport()->setAcceptDrops(true);
}

Q3DragObject* iLDPListView::dragObject () {
  ISelectable_PtrList sel_items;
  panel->GetSelectedItems(sel_items);
  ISelectable* ci = sel_items.SafeEl(0);
  if (ci) {
    return ci->GetClipData(sel_items, taiClipData::EA_SRC_DRAG, true);
  } else {
    return NULL;
  }
}

void iLDPListView::focusInEvent(QFocusEvent* ev) {
  inherited::focusInEvent(ev);
  panel->ctrl_focusInEvent(ev);
}



//////////////////////////
//    iListDataPanel 	//
//////////////////////////

iListDataPanel::iListDataPanel(taiDataLink* dl_)
:inherited(dl_)
{
  list = new iLDPListView(this, "list");
  setCentralWidget(list);
  list->setSelectionMode(Q3ListView::Extended);
  list->setShowSortIndicator(true);
  ConfigHeader();
  connect(list, SIGNAL(contextMenuRequested(Q3ListViewItem*, const QPoint &, int)),
      this, SLOT(list_contextMenuRequested(Q3ListViewItem*, const QPoint &, int)) );
  connect(list, SIGNAL(selectionChanged()),
      this, SLOT(list_selectionChanged()) );
  FillList();
}

iListDataPanel::~iListDataPanel() {
}

void iListDataPanel::ClearList() {
  list->clear();
}

void iListDataPanel::ConfigHeader() {
  // set up number of cols, based on link
  int cnt = list->columns();
  if (cnt == 0) {
    list->addColumn("#");
    ++cnt;
  }
  for (int i = 0; i < link()->NumListCols(); ++i) {
    int hdr_idx = i + 1;
    if (hdr_idx >= cnt) {
      list->addColumn(link()->GetColHeading(i));
      ++cnt;
    } else
      list->setColumnText(hdr_idx, link()->GetColHeading(i));
  }
  // only if structural changes, delete unnecessary columns
  for (int i = cnt - 1; i > link()->NumListCols(); --i)
    list->removeColumn(i);
}

void iListDataPanel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  if (!( ((dcr >= DCR_LIST_MIN) && (dcr <= DCR_LIST_MAX))
    || (dcr == DCR_DATA_UPDATE_END) || (dcr == DCR_STRUCT_UPDATE_END))
  ) return;

  ClearList();
  if (dcr == DCR_STRUCT_UPDATE_END)
    ConfigHeader();
  FillList();
}

int iListDataPanel::EditAction(int ea) {
  int rval = 0;

  ISelectable_PtrList sel_list;
  GetSelectedItems(sel_list);
  ISelectable* ci = sel_list.SafeEl(0);
  if (ci)  {
    rval = ci->EditAction_(sel_list, ea);
  }
  return rval;
}

void iListDataPanel::FillList() {
  taiListDataNode* last_child = NULL;
  int i = 0;
  for (taiDataLink* child; (child = link()->GetListChild(i)); ++i) { //iterate until no more
    taiListDataNode* dn = new taiListDataNode(i + 1, this, child, list, last_child, (iListViewItem::DNF_CAN_DRAG));
    // set remaining col texts -- note that we subtract 1 from col, because we use a numbering column
    for (int col = 0; col < link()->NumListCols(); ++col) {
      dn->setText(col + 1, link()->ChildGetColText(child, col, i));
    }
    last_child = dn;
  }
}

int iListDataPanel::GetEditActions() {
  int rval = 0;

  ISelectable_PtrList sel_list;
  GetSelectedItems(sel_list);
  ISelectable* ci = sel_list.SafeEl(0);
  if (ci)  {
    rval = ci->GetEditActions_(sel_list);
    // certain things disallowed if more than one item selected
    if (sel_list.size > 1) {
      rval &= ~(taiClipData::EA_FORB_ON_MUL_SEL);
    }
  }
  return rval;
}

void iListDataPanel::GetSelectedItems(ISelectable_PtrList& lst) {
  Q3ListViewItemIterator it(list, Q3ListViewItemIterator::Selected);
  while (it.current()) {
    lst.Add((taiListDataNode*)it.current());
    ++it;
  }
}

void iListDataPanel::list_contextMenuRequested(Q3ListViewItem* item, const QPoint & pos, int col ) {
  //TODO: 'item' will be whatever is under the mouse, but we could have a multi select!!!
  taiListDataNode* nd = (taiListDataNode*)item;
  if (nd == NULL) return; //TODO: could possibly be multi select

  taiMenu* menu = new taiMenu(this, taiMenu::normal, taiMisc::fonSmall);
  //TODO: any for us first (ex. delete)

  ISelectable_PtrList sel_list;
  GetSelectedItems(sel_list);
  nd->FillContextMenu(sel_list, menu); // also calls link menu filler

  //TODO: any for us last (ex. delete)
  if (menu->count() > 0) { //only show if any items!
    menu->exec(pos);
  }
  delete menu;
}

void iListDataPanel::list_selectionChanged() {
  viewer_win()->UpdateUi();
}

String iListDataPanel::panel_type() const {
  static String str("List View");
  return str;
}

