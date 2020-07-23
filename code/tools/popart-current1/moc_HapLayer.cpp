/****************************************************************************
** Meta object code from reading C++ file 'HapLayer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/gui/HapLayer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HapLayer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_HapLayer_t {
    QByteArrayData data[10];
    char stringdata0[124];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HapLayer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HapLayer_t qt_meta_stringdata_HapLayer = {
    {
QT_MOC_LITERAL(0, 0, 8), // "HapLayer"
QT_MOC_LITERAL(1, 9, 11), // "dirtyRegion"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 21), // "colourChangeTriggered"
QT_MOC_LITERAL(4, 44, 25), // "coordinateChangeTriggered"
QT_MOC_LITERAL(5, 70, 7), // "entered"
QT_MOC_LITERAL(6, 78, 4), // "left"
QT_MOC_LITERAL(7, 83, 9), // "clickable"
QT_MOC_LITERAL(8, 93, 12), // "changeColour"
QT_MOC_LITERAL(9, 106, 17) // "changeCoordinates"

    },
    "HapLayer\0dirtyRegion\0\0colourChangeTriggered\0"
    "coordinateChangeTriggered\0entered\0"
    "left\0clickable\0changeColour\0"
    "changeCoordinates"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HapLayer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       3,    1,   57,    2, 0x06 /* Public */,
       4,    1,   60,    2, 0x06 /* Public */,
       5,    1,   63,    2, 0x06 /* Public */,
       6,    1,   66,    2, 0x06 /* Public */,
       7,    1,   69,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    0,   72,    2, 0x08 /* Private */,
       9,    0,   73,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QRegion,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Bool,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void HapLayer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HapLayer *_t = static_cast<HapLayer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->dirtyRegion((*reinterpret_cast< const QRegion(*)>(_a[1]))); break;
        case 1: _t->colourChangeTriggered((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->coordinateChangeTriggered((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->entered((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->left((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->clickable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->changeColour(); break;
        case 7: _t->changeCoordinates(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (HapLayer::*_t)(const QRegion & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HapLayer::dirtyRegion)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (HapLayer::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HapLayer::colourChangeTriggered)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (HapLayer::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HapLayer::coordinateChangeTriggered)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (HapLayer::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HapLayer::entered)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (HapLayer::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HapLayer::left)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (HapLayer::*_t)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HapLayer::clickable)) {
                *result = 5;
                return;
            }
        }
    }
}

const QMetaObject HapLayer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_HapLayer.data,
      qt_meta_data_HapLayer,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *HapLayer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HapLayer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HapLayer.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Marble::LayerInterface"))
        return static_cast< Marble::LayerInterface*>(this);
    return QObject::qt_metacast(_clname);
}

int HapLayer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void HapLayer::dirtyRegion(const QRegion & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void HapLayer::colourChangeTriggered(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void HapLayer::coordinateChangeTriggered(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void HapLayer::entered(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void HapLayer::left(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void HapLayer::clickable(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
