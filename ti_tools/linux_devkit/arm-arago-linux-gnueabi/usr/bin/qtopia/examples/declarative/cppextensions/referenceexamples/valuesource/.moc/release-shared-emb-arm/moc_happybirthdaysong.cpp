/****************************************************************************
** Meta object code from reading C++ file 'happybirthdaysong.h'
**
** Created: Wed Jul 25 18:52:28 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../happybirthdaysong.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'happybirthdaysong.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_HappyBirthdaySong[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       1,   19, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x08,

 // properties: name, type, flags
      37,   29, 0x0a095103,

       0        // eod
};

static const char qt_meta_stringdata_HappyBirthdaySong[] = {
    "HappyBirthdaySong\0\0advance()\0QString\0"
    "name\0"
};

const QMetaObject HappyBirthdaySong::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_HappyBirthdaySong,
      qt_meta_data_HappyBirthdaySong, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &HappyBirthdaySong::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *HappyBirthdaySong::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *HappyBirthdaySong::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HappyBirthdaySong))
        return static_cast<void*>(const_cast< HappyBirthdaySong*>(this));
    if (!strcmp(_clname, "QDeclarativePropertyValueSource"))
        return static_cast< QDeclarativePropertyValueSource*>(const_cast< HappyBirthdaySong*>(this));
    if (!strcmp(_clname, "com.trolltech.qml.QDeclarativePropertyValueSource"))
        return static_cast< QDeclarativePropertyValueSource*>(const_cast< HappyBirthdaySong*>(this));
    return QObject::qt_metacast(_clname);
}

int HappyBirthdaySong::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: advance(); break;
        default: ;
        }
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = name(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setName(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
