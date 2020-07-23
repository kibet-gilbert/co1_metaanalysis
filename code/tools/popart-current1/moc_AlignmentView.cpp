/****************************************************************************
** Meta object code from reading C++ file 'AlignmentView.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/gui/AlignmentView.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AlignmentView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AlignmentView_t {
    QByteArrayData data[21];
    char stringdata0[286];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AlignmentView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AlignmentView_t qt_meta_stringdata_AlignmentView = {
    {
QT_MOC_LITERAL(0, 0, 13), // "AlignmentView"
QT_MOC_LITERAL(1, 14, 12), // "newSelection"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 11), // "draggedLeft"
QT_MOC_LITERAL(4, 40, 12), // "draggedRight"
QT_MOC_LITERAL(5, 53, 17), // "hideMaskedColumns"
QT_MOC_LITERAL(6, 71, 16), // "selectionChanged"
QT_MOC_LITERAL(7, 88, 14), // "QItemSelection"
QT_MOC_LITERAL(8, 103, 12), // "columnsAdded"
QT_MOC_LITERAL(9, 116, 9), // "rowsAdded"
QT_MOC_LITERAL(10, 126, 15), // "mousePressEvent"
QT_MOC_LITERAL(11, 142, 12), // "QMouseEvent*"
QT_MOC_LITERAL(12, 155, 14), // "mouseMoveEvent"
QT_MOC_LITERAL(13, 170, 17), // "mouseReleaseEvent"
QT_MOC_LITERAL(14, 188, 15), // "characterInsert"
QT_MOC_LITERAL(15, 204, 15), // "characterDelete"
QT_MOC_LITERAL(16, 220, 6), // "moveTo"
QT_MOC_LITERAL(17, 227, 14), // "changeCharType"
QT_MOC_LITERAL(18, 242, 18), // "Sequence::CharType"
QT_MOC_LITERAL(19, 261, 10), // "changeMask"
QT_MOC_LITERAL(20, 272, 13) // "changeRowMask"

    },
    "AlignmentView\0newSelection\0\0draggedLeft\0"
    "draggedRight\0hideMaskedColumns\0"
    "selectionChanged\0QItemSelection\0"
    "columnsAdded\0rowsAdded\0mousePressEvent\0"
    "QMouseEvent*\0mouseMoveEvent\0"
    "mouseReleaseEvent\0characterInsert\0"
    "characterDelete\0moveTo\0changeCharType\0"
    "Sequence::CharType\0changeMask\0"
    "changeRowMask"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AlignmentView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x06 /* Public */,
       3,    0,   95,    2, 0x06 /* Public */,
       4,    0,   96,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   97,    2, 0x0a /* Public */,
       6,    2,  100,    2, 0x09 /* Protected */,
       8,    3,  105,    2, 0x09 /* Protected */,
       9,    3,  112,    2, 0x09 /* Protected */,
      10,    1,  119,    2, 0x09 /* Protected */,
      12,    1,  122,    2, 0x09 /* Protected */,
      13,    1,  125,    2, 0x09 /* Protected */,
      14,    1,  128,    2, 0x08 /* Private */,
      15,    1,  131,    2, 0x08 /* Private */,
      16,    2,  134,    2, 0x08 /* Private */,
      17,    1,  139,    2, 0x08 /* Private */,
      19,    2,  142,    2, 0x08 /* Private */,
      20,    2,  147,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, 0x80000000 | 7, 0x80000000 | 7,    2,    2,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::Int, QMetaType::Int,    2,    2,    2,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::Int, QMetaType::Int,    2,    2,    2,
    QMetaType::Void, 0x80000000 | 11,    2,
    QMetaType::Void, 0x80000000 | 11,    2,
    QMetaType::Void, 0x80000000 | 11,    2,
    QMetaType::Void, QMetaType::QModelIndex,    2,
    QMetaType::Void, QMetaType::QModelIndex,    2,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::QModelIndex,    2,    2,
    QMetaType::Void, 0x80000000 | 18,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,

       0        // eod
};

void AlignmentView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AlignmentView *_t = static_cast<AlignmentView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->newSelection(); break;
        case 1: _t->draggedLeft(); break;
        case 2: _t->draggedRight(); break;
        case 3: _t->hideMaskedColumns((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->selectionChanged((*reinterpret_cast< const QItemSelection(*)>(_a[1])),(*reinterpret_cast< const QItemSelection(*)>(_a[2]))); break;
        case 5: _t->columnsAdded((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 6: _t->rowsAdded((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 7: _t->mousePressEvent((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 8: _t->mouseMoveEvent((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 9: _t->mouseReleaseEvent((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 10: _t->characterInsert((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 11: _t->characterDelete((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 12: _t->moveTo((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 13: _t->changeCharType((*reinterpret_cast< Sequence::CharType(*)>(_a[1]))); break;
        case 14: _t->changeMask((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 15: _t->changeRowMask((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QItemSelection >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (AlignmentView::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AlignmentView::newSelection)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (AlignmentView::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AlignmentView::draggedLeft)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (AlignmentView::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AlignmentView::draggedRight)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject AlignmentView::staticMetaObject = {
    { &QTableView::staticMetaObject, qt_meta_stringdata_AlignmentView.data,
      qt_meta_data_AlignmentView,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *AlignmentView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AlignmentView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AlignmentView.stringdata0))
        return static_cast<void*>(this);
    return QTableView::qt_metacast(_clname);
}

int AlignmentView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void AlignmentView::newSelection()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void AlignmentView::draggedLeft()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void AlignmentView::draggedRight()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
