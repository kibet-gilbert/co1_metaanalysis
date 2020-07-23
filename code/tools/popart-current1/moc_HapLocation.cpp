/****************************************************************************
** Meta object code from reading C++ file 'HapLocation.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/gui/HapLocation.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HapLocation.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_HapLocation_t {
    QByteArrayData data[8];
    char stringdata0[88];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HapLocation_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HapLocation_t qt_meta_stringdata_HapLocation = {
    {
QT_MOC_LITERAL(0, 0, 11), // "HapLocation"
QT_MOC_LITERAL(1, 12, 7), // "nameSet"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 11), // "locationSet"
QT_MOC_LITERAL(4, 33, 26), // "Marble::GeoDataCoordinates"
QT_MOC_LITERAL(5, 60, 8), // "seqAdded"
QT_MOC_LITERAL(6, 69, 11), // "setLocation"
QT_MOC_LITERAL(7, 81, 6) // "coords"

    },
    "HapLocation\0nameSet\0\0locationSet\0"
    "Marble::GeoDataCoordinates\0seqAdded\0"
    "setLocation\0coords"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HapLocation[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       3,    1,   37,    2, 0x06 /* Public */,
       5,    2,   40,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   45,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::UInt,    2,    2,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 4,    7,

       0        // eod
};

void HapLocation::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HapLocation *_t = static_cast<HapLocation *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->nameSet((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->locationSet((*reinterpret_cast< const Marble::GeoDataCoordinates(*)>(_a[1]))); break;
        case 2: _t->seqAdded((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 3: _t->setLocation((*reinterpret_cast< const Marble::GeoDataCoordinates(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Marble::GeoDataCoordinates >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Marble::GeoDataCoordinates >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (HapLocation::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HapLocation::nameSet)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (HapLocation::*_t)(const Marble::GeoDataCoordinates & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HapLocation::locationSet)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (HapLocation::*_t)(const QString & , unsigned  );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HapLocation::seqAdded)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject HapLocation::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_HapLocation.data,
      qt_meta_data_HapLocation,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *HapLocation::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HapLocation::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HapLocation.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int HapLocation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void HapLocation::nameSet(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void HapLocation::locationSet(const Marble::GeoDataCoordinates & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void HapLocation::seqAdded(const QString & _t1, unsigned  _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
