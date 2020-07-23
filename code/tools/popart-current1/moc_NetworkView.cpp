/****************************************************************************
** Meta object code from reading C++ file 'NetworkView.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/gui/NetworkView.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_NetworkView_t {
    QByteArrayData data[27];
    char stringdata0[369];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NetworkView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NetworkView_t qt_meta_stringdata_NetworkView = {
    {
QT_MOC_LITERAL(0, 0, 11), // "NetworkView"
QT_MOC_LITERAL(1, 12, 12), // "networkDrawn"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 10), // "itemsMoved"
QT_MOC_LITERAL(4, 37, 37), // "QList<QPair<QGraphicsItem*,QP..."
QT_MOC_LITERAL(5, 75, 17), // "legendItemClicked"
QT_MOC_LITERAL(6, 93, 15), // "caughtException"
QT_MOC_LITERAL(7, 109, 6), // "zoomIn"
QT_MOC_LITERAL(8, 116, 7), // "zoomOut"
QT_MOC_LITERAL(9, 124, 7), // "rotateL"
QT_MOC_LITERAL(10, 132, 7), // "rotateR"
QT_MOC_LITERAL(11, 140, 11), // "selectNodes"
QT_MOC_LITERAL(12, 152, 6), // "redraw"
QT_MOC_LITERAL(13, 159, 19), // "toggleShowBarcharts"
QT_MOC_LITERAL(14, 179, 16), // "toggleShowTaxBox"
QT_MOC_LITERAL(15, 196, 13), // "adjustAndDraw"
QT_MOC_LITERAL(16, 210, 10), // "showTaxBox"
QT_MOC_LITERAL(17, 221, 14), // "QGraphicsItem*"
QT_MOC_LITERAL(18, 236, 10), // "hideTaxBox"
QT_MOC_LITERAL(19, 247, 12), // "showBarchart"
QT_MOC_LITERAL(20, 260, 12), // "hideBarchart"
QT_MOC_LITERAL(21, 273, 11), // "LegendItem*"
QT_MOC_LITERAL(22, 285, 14), // "handleItemMove"
QT_MOC_LITERAL(23, 300, 18), // "setGrabbableCursor"
QT_MOC_LITERAL(24, 319, 17), // "setGrabbingCursor"
QT_MOC_LITERAL(25, 337, 18), // "setClickableCursor"
QT_MOC_LITERAL(26, 356, 12) // "updateTraits"

    },
    "NetworkView\0networkDrawn\0\0itemsMoved\0"
    "QList<QPair<QGraphicsItem*,QPointF> >\0"
    "legendItemClicked\0caughtException\0"
    "zoomIn\0zoomOut\0rotateL\0rotateR\0"
    "selectNodes\0redraw\0toggleShowBarcharts\0"
    "toggleShowTaxBox\0adjustAndDraw\0"
    "showTaxBox\0QGraphicsItem*\0hideTaxBox\0"
    "showBarchart\0hideBarchart\0LegendItem*\0"
    "handleItemMove\0setGrabbableCursor\0"
    "setGrabbingCursor\0setClickableCursor\0"
    "updateTraits"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NetworkView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  129,    2, 0x06 /* Public */,
       3,    1,  130,    2, 0x06 /* Public */,
       5,    1,  133,    2, 0x06 /* Public */,
       6,    1,  136,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,  139,    2, 0x0a /* Public */,
       8,    0,  140,    2, 0x0a /* Public */,
       9,    0,  141,    2, 0x0a /* Public */,
      10,    0,  142,    2, 0x0a /* Public */,
      11,    1,  143,    2, 0x0a /* Public */,
      12,    1,  146,    2, 0x0a /* Public */,
      13,    1,  149,    2, 0x0a /* Public */,
      14,    1,  152,    2, 0x0a /* Public */,
      15,    0,  155,    2, 0x08 /* Private */,
      16,    1,  156,    2, 0x08 /* Private */,
      18,    0,  159,    2, 0x08 /* Private */,
      19,    1,  160,    2, 0x08 /* Private */,
      20,    0,  163,    2, 0x08 /* Private */,
       5,    1,  164,    2, 0x08 /* Private */,
      22,    1,  167,    2, 0x08 /* Private */,
      23,    1,  170,    2, 0x08 /* Private */,
      24,    1,  173,    2, 0x08 /* Private */,
      25,    1,  176,    2, 0x08 /* Private */,
      26,    0,  179,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::UInt,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 17,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 17,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 21,    2,
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,

       0        // eod
};

void NetworkView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NetworkView *_t = static_cast<NetworkView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->networkDrawn(); break;
        case 1: _t->itemsMoved((*reinterpret_cast< QList<QPair<QGraphicsItem*,QPointF> >(*)>(_a[1]))); break;
        case 2: _t->legendItemClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->caughtException((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->zoomIn(); break;
        case 5: _t->zoomOut(); break;
        case 6: _t->rotateL(); break;
        case 7: _t->rotateR(); break;
        case 8: _t->selectNodes((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->redraw((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 10: _t->toggleShowBarcharts((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->toggleShowTaxBox((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->adjustAndDraw(); break;
        case 13: _t->showTaxBox((*reinterpret_cast< QGraphicsItem*(*)>(_a[1]))); break;
        case 14: _t->hideTaxBox(); break;
        case 15: _t->showBarchart((*reinterpret_cast< QGraphicsItem*(*)>(_a[1]))); break;
        case 16: _t->hideBarchart(); break;
        case 17: _t->legendItemClicked((*reinterpret_cast< LegendItem*(*)>(_a[1]))); break;
        case 18: _t->handleItemMove((*reinterpret_cast< QList<QPair<QGraphicsItem*,QPointF> >(*)>(_a[1]))); break;
        case 19: _t->setGrabbableCursor((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 20: _t->setGrabbingCursor((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 21: _t->setClickableCursor((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 22: _t->updateTraits(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 13:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QGraphicsItem* >(); break;
            }
            break;
        case 15:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QGraphicsItem* >(); break;
            }
            break;
        case 17:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< LegendItem* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (NetworkView::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkView::networkDrawn)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (NetworkView::*_t)(QList<QPair<QGraphicsItem*,QPointF> > );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkView::itemsMoved)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (NetworkView::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkView::legendItemClicked)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (NetworkView::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NetworkView::caughtException)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject NetworkView::staticMetaObject = {
    { &QAbstractItemView::staticMetaObject, qt_meta_stringdata_NetworkView.data,
      qt_meta_data_NetworkView,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *NetworkView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NetworkView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NetworkView.stringdata0))
        return static_cast<void*>(this);
    return QAbstractItemView::qt_metacast(_clname);
}

int NetworkView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    }
    return _id;
}

// SIGNAL 0
void NetworkView::networkDrawn()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void NetworkView::itemsMoved(QList<QPair<QGraphicsItem*,QPointF> > _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NetworkView::legendItemClicked(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NetworkView::caughtException(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
