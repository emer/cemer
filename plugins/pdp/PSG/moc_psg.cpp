/****************************************************************************
** Meta object code from reading C++ file 'psg.h'
**
** Created: Sun Mar 4 22:17:22 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "psg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'psg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_PSGPlugin[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_PSGPlugin[] = {
    "PSGPlugin\0"
};

const QMetaObject PSGPlugin::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PSGPlugin,
      qt_meta_data_PSGPlugin, 0 }
};

const QMetaObject *PSGPlugin::metaObject() const
{
    return &staticMetaObject;
}

void *PSGPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PSGPlugin))
	return static_cast<void*>(const_cast<PSGPlugin*>(this));
    if (!strcmp(_clname, "IPlugin"))
	return static_cast<IPlugin*>(const_cast<PSGPlugin*>(this));
    if (!strcmp(_clname, "pdp.IPlugin/1.0"))
	return static_cast<IPlugin*>(const_cast<PSGPlugin*>(this));
    return QObject::qt_metacast(_clname);
}

int PSGPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
