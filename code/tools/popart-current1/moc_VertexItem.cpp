/****************************************************************************
** Meta object code from reading C++ file 'VertexItem.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/gui/VertexItem.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VertexItem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_VertexItem_t {
    QByteArrayData data[10];
    char stringdata0[110];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VertexItem_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VertexItem_t qt_meta_stringdata_VertexItem = {
    {
QT_MOC_LITERAL(0, 0, 10), // "VertexItem"
QT_MOC_LITERAL(1, 11, 5), // "moved"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 12), // "hoverEntered"
QT_MOC_LITERAL(4, 31, 14), // "QGraphicsItem*"
QT_MOC_LITERAL(5, 46, 9), // "hoverLeft"
QT_MOC_LITERAL(6, 56, 19), // "setBackgroundColour"
QT_MOC_LITERAL(7, 76, 7), // "QColor&"
QT_MOC_LITERAL(8, 84, 6), // "colour"
QT_MOC_LITERAL(9, 91, 18) // "setHighlightColour"

    },
    "VertexItem\0moved\0\0hoverEntered\0"
    "QGraphicsItem*\0hoverLeft\0setBackgroundColour\0"
    "QColor&\0colour\0setHighlightColour"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VertexItem[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       3,    1,   42,    2, 0x06 /* Public */,
       5,    0,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   46,    2, 0x0a /* Public */,
       9,    1,   49,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QPointF,    2,
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7,    8,

       0        // eod
};

void VertexItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        VertexItem *_t = static_cast<VertexItem *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->moved((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 1: _t->hoverEntered((*reinterpret_cast< QGraphicsItem*(*)>(_a[1]))); break;
        case 2: _t->hoverLeft(); break;
        case 3: _t->setBackgroundColour((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        case 4: _t->setHighlightColour((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QGraphicsItem* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (VertexItem::*_t)(const QPointF & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VertexItem::moved)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (VertexItem::*_t)(QGraphicsItem * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VertexItem::hoverEntered)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (VertexItem::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VertexItem::hoverLeft)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject VertexItem::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_VertexItem.data,
      qt_meta_data_VertexItem,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *VertexItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VertexItem::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_VertexItem.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QGraphicsEllipseItem"))
        return static_cast< QGraphicsEllipseItem*>(this);
    return QObject::qt_metacast(_clname);
}

int VertexItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void VertexItem::moved(const QPointF & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void VertexItem::hoverEntered(QGraphicsItem * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void VertexItem::hoverLeft()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
