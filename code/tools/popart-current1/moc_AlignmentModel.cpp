/****************************************************************************
** Meta object code from reading C++ file 'AlignmentModel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/gui/AlignmentModel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AlignmentModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AlignmentModel_t {
    QByteArrayData data[10];
    char stringdata0[132];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AlignmentModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AlignmentModel_t qt_meta_stringdata_AlignmentModel = {
    {
QT_MOC_LITERAL(0, 0, 14), // "AlignmentModel"
QT_MOC_LITERAL(1, 15, 17), // "characterInserted"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 16), // "characterDeleted"
QT_MOC_LITERAL(4, 51, 9), // "fetchedTo"
QT_MOC_LITERAL(5, 61, 8), // "pushedTo"
QT_MOC_LITERAL(6, 70, 15), // "charTypeChanged"
QT_MOC_LITERAL(7, 86, 18), // "Sequence::CharType"
QT_MOC_LITERAL(8, 105, 11), // "maskChanged"
QT_MOC_LITERAL(9, 117, 14) // "rowMaskChanged"

    },
    "AlignmentModel\0characterInserted\0\0"
    "characterDeleted\0fetchedTo\0pushedTo\0"
    "charTypeChanged\0Sequence::CharType\0"
    "maskChanged\0rowMaskChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AlignmentModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       3,    1,   52,    2, 0x06 /* Public */,
       4,    2,   55,    2, 0x06 /* Public */,
       5,    2,   60,    2, 0x06 /* Public */,
       6,    1,   65,    2, 0x06 /* Public */,
       8,    2,   68,    2, 0x06 /* Public */,
       9,    2,   73,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QModelIndex,    2,
    QMetaType::Void, QMetaType::QModelIndex,    2,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::QModelIndex,    2,    2,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::QModelIndex,    2,    2,
    QMetaType::Void, 0x80000000 | 7,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,

       0        // eod
};

void AlignmentModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AlignmentModel *_t = static_cast<AlignmentModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->characterInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: _t->characterDeleted((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: _t->fetchedTo((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 3: _t->pushedTo((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 4: _t->charTypeChanged((*reinterpret_cast< Sequence::CharType(*)>(_a[1]))); break;
        case 5: _t->maskChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: _t->rowMaskChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (AlignmentModel::*_t)(const QModelIndex & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AlignmentModel::characterInserted)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (AlignmentModel::*_t)(const QModelIndex & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AlignmentModel::characterDeleted)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (AlignmentModel::*_t)(const QModelIndex & , const QModelIndex & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AlignmentModel::fetchedTo)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (AlignmentModel::*_t)(const QModelIndex & , const QModelIndex & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AlignmentModel::pushedTo)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (AlignmentModel::*_t)(Sequence::CharType );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AlignmentModel::charTypeChanged)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (AlignmentModel::*_t)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AlignmentModel::maskChanged)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (AlignmentModel::*_t)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AlignmentModel::rowMaskChanged)) {
                *result = 6;
                return;
            }
        }
    }
}

const QMetaObject AlignmentModel::staticMetaObject = {
    { &QAbstractTableModel::staticMetaObject, qt_meta_stringdata_AlignmentModel.data,
      qt_meta_data_AlignmentModel,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *AlignmentModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AlignmentModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AlignmentModel.stringdata0))
        return static_cast<void*>(this);
    return QAbstractTableModel::qt_metacast(_clname);
}

int AlignmentModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractTableModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void AlignmentModel::characterInserted(const QModelIndex & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AlignmentModel::characterDeleted(const QModelIndex & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AlignmentModel::fetchedTo(const QModelIndex & _t1, const QModelIndex & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void AlignmentModel::pushedTo(const QModelIndex & _t1, const QModelIndex & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void AlignmentModel::charTypeChanged(Sequence::CharType _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void AlignmentModel::maskChanged(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void AlignmentModel::rowMaskChanged(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
