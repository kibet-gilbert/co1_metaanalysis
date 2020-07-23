/****************************************************************************
** Meta object code from reading C++ file 'MapView.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/gui/MapView.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MapView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MapView_t {
    QByteArrayData data[23];
    char stringdata0[333];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MapView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MapView_t qt_meta_stringdata_MapView = {
    {
QT_MOC_LITERAL(0, 0, 7), // "MapView"
QT_MOC_LITERAL(1, 8, 15), // "positionChanged"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 24), // "seqColourChangeRequested"
QT_MOC_LITERAL(4, 50, 11), // "locationSet"
QT_MOC_LITERAL(5, 62, 22), // "std::pair<float,float>"
QT_MOC_LITERAL(6, 85, 14), // "setColourTheme"
QT_MOC_LITERAL(7, 100, 18), // "ColourTheme::Theme"
QT_MOC_LITERAL(8, 119, 6), // "colour"
QT_MOC_LITERAL(9, 126, 9), // "setColour"
QT_MOC_LITERAL(10, 136, 18), // "setClickableCursor"
QT_MOC_LITERAL(11, 155, 17), // "setExternalLegend"
QT_MOC_LITERAL(12, 173, 8), // "setTheme"
QT_MOC_LITERAL(13, 182, 11), // "savePDFFile"
QT_MOC_LITERAL(14, 194, 8), // "filename"
QT_MOC_LITERAL(15, 203, 11), // "savePNGFile"
QT_MOC_LITERAL(16, 215, 11), // "saveSVGFile"
QT_MOC_LITERAL(17, 227, 17), // "updateGeoPosition"
QT_MOC_LITERAL(18, 245, 17), // "updateDirtyRegion"
QT_MOC_LITERAL(19, 263, 22), // "requestChangeSeqColour"
QT_MOC_LITERAL(20, 286, 16), // "changeCoordinate"
QT_MOC_LITERAL(21, 303, 13), // "setMapToolTip"
QT_MOC_LITERAL(22, 317, 15) // "resetMapToolTip"

    },
    "MapView\0positionChanged\0\0"
    "seqColourChangeRequested\0locationSet\0"
    "std::pair<float,float>\0setColourTheme\0"
    "ColourTheme::Theme\0colour\0setColour\0"
    "setClickableCursor\0setExternalLegend\0"
    "setTheme\0savePDFFile\0filename\0savePNGFile\0"
    "saveSVGFile\0updateGeoPosition\0"
    "updateDirtyRegion\0requestChangeSeqColour\0"
    "changeCoordinate\0setMapToolTip\0"
    "resetMapToolTip"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MapView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  109,    2, 0x06 /* Public */,
       3,    1,  112,    2, 0x06 /* Public */,
       4,    2,  115,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,  120,    2, 0x0a /* Public */,
       6,    0,  123,    2, 0x2a /* Public | MethodCloned */,
       8,    1,  124,    2, 0x0a /* Public */,
       9,    2,  127,    2, 0x0a /* Public */,
      10,    1,  132,    2, 0x0a /* Public */,
      11,    1,  135,    2, 0x0a /* Public */,
      12,    1,  138,    2, 0x0a /* Public */,
      13,    1,  141,    2, 0x0a /* Public */,
      15,    1,  144,    2, 0x0a /* Public */,
      16,    1,  147,    2, 0x0a /* Public */,
      17,    1,  150,    2, 0x08 /* Private */,
      18,    1,  153,    2, 0x08 /* Private */,
      19,    1,  156,    2, 0x08 /* Private */,
      20,    1,  159,    2, 0x08 /* Private */,
      21,    1,  162,    2, 0x08 /* Private */,
      22,    1,  165,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::UInt, 0x80000000 | 5,    2,    2,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 7,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QColor,    2,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QRegion,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,

       0        // eod
};

void MapView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MapView *_t = static_cast<MapView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->positionChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->seqColourChangeRequested((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->locationSet((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< std::pair<float,float>(*)>(_a[2]))); break;
        case 3: _t->setColourTheme((*reinterpret_cast< ColourTheme::Theme(*)>(_a[1]))); break;
        case 4: _t->setColourTheme(); break;
        case 5: _t->colour((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->setColour((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 7: _t->setClickableCursor((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->setExternalLegend((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->setTheme((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->savePDFFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->savePNGFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: _t->saveSVGFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->updateGeoPosition((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 14: _t->updateDirtyRegion((*reinterpret_cast< const QRegion(*)>(_a[1]))); break;
        case 15: _t->requestChangeSeqColour((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->changeCoordinate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->setMapToolTip((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 18: _t->resetMapToolTip((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (MapView::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MapView::positionChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (MapView::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MapView::seqColourChangeRequested)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (MapView::*_t)(unsigned  , std::pair<float,float> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MapView::locationSet)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject MapView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MapView.data,
      qt_meta_data_MapView,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MapView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MapView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MapView.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int MapView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 19;
    }
    return _id;
}

// SIGNAL 0
void MapView::positionChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MapView::seqColourChangeRequested(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MapView::locationSet(unsigned  _t1, std::pair<float,float> _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
