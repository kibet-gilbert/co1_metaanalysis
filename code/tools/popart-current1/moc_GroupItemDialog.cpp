/****************************************************************************
** Meta object code from reading C++ file 'GroupItemDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/gui/GroupItemDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GroupItemDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GroupItemDialog_t {
    QByteArrayData data[7];
    char stringdata0[106];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GroupItemDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GroupItemDialog_t qt_meta_stringdata_GroupItemDialog = {
    {
QT_MOC_LITERAL(0, 0, 15), // "GroupItemDialog"
QT_MOC_LITERAL(1, 16, 14), // "checkAndAccept"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 8), // "addGroup"
QT_MOC_LITERAL(4, 41, 23), // "addSelectedItemsToGroup"
QT_MOC_LITERAL(5, 65, 13), // "deassignItems"
QT_MOC_LITERAL(6, 79, 26) // "QList<QPair<QString,int> >"

    },
    "GroupItemDialog\0checkAndAccept\0\0"
    "addGroup\0addSelectedItemsToGroup\0"
    "deassignItems\0QList<QPair<QString,int> >"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GroupItemDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x08 /* Private */,
       3,    0,   35,    2, 0x08 /* Private */,
       4,    1,   36,    2, 0x08 /* Private */,
       5,    1,   39,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, 0x80000000 | 6,    2,

       0        // eod
};

void GroupItemDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GroupItemDialog *_t = static_cast<GroupItemDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->checkAndAccept(); break;
        case 1: _t->addGroup(); break;
        case 2: _t->addSelectedItemsToGroup((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->deassignItems((*reinterpret_cast< const QList<QPair<QString,int> >(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject GroupItemDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_GroupItemDialog.data,
      qt_meta_data_GroupItemDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *GroupItemDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GroupItemDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GroupItemDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int GroupItemDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
struct qt_meta_stringdata_UnsortedListWidget_t {
    QByteArrayData data[9];
    char stringdata0[99];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UnsortedListWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UnsortedListWidget_t qt_meta_stringdata_UnsortedListWidget = {
    {
QT_MOC_LITERAL(0, 0, 18), // "UnsortedListWidget"
QT_MOC_LITERAL(1, 19, 13), // "groupSelected"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 8), // "addGroup"
QT_MOC_LITERAL(4, 43, 5), // "group"
QT_MOC_LITERAL(5, 49, 11), // "removeGroup"
QT_MOC_LITERAL(6, 61, 11), // "renameGroup"
QT_MOC_LITERAL(7, 73, 16), // "setSelectedGroup"
QT_MOC_LITERAL(8, 90, 8) // "QAction*"

    },
    "UnsortedListWidget\0groupSelected\0\0"
    "addGroup\0group\0removeGroup\0renameGroup\0"
    "setSelectedGroup\0QAction*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UnsortedListWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   42,    2, 0x0a /* Public */,
       5,    1,   45,    2, 0x0a /* Public */,
       6,    2,   48,    2, 0x0a /* Public */,
       7,    1,   53,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    2,    2,
    QMetaType::Void, 0x80000000 | 8,    2,

       0        // eod
};

void UnsortedListWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        UnsortedListWidget *_t = static_cast<UnsortedListWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->groupSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->addGroup((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->removeGroup((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->renameGroup((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 4: _t->setSelectedGroup((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (UnsortedListWidget::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UnsortedListWidget::groupSelected)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject UnsortedListWidget::staticMetaObject = {
    { &QListWidget::staticMetaObject, qt_meta_stringdata_UnsortedListWidget.data,
      qt_meta_data_UnsortedListWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *UnsortedListWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UnsortedListWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UnsortedListWidget.stringdata0))
        return static_cast<void*>(this);
    return QListWidget::qt_metacast(_clname);
}

int UnsortedListWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListWidget::qt_metacall(_c, _id, _a);
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
void UnsortedListWidget::groupSelected(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_GroupedTreeWidget_t {
    QByteArrayData data[12];
    char stringdata0[201];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GroupedTreeWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GroupedTreeWidget_t qt_meta_stringdata_GroupedTreeWidget = {
    {
QT_MOC_LITERAL(0, 0, 17), // "GroupedTreeWidget"
QT_MOC_LITERAL(1, 18, 12), // "itemsRemoved"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 26), // "QList<QPair<QString,int> >"
QT_MOC_LITERAL(4, 59, 12), // "groupDeleted"
QT_MOC_LITERAL(5, 72, 11), // "groupLocked"
QT_MOC_LITERAL(6, 84, 13), // "groupUnlocked"
QT_MOC_LITERAL(7, 98, 16), // "groupNameChanged"
QT_MOC_LITERAL(8, 115, 23), // "toggleLockSelectedGroup"
QT_MOC_LITERAL(9, 139, 20), // "deassignSelectedItem"
QT_MOC_LITERAL(10, 160, 20), // "deleteSelectedGroups"
QT_MOC_LITERAL(11, 181, 19) // "renameSelectedGroup"

    },
    "GroupedTreeWidget\0itemsRemoved\0\0"
    "QList<QPair<QString,int> >\0groupDeleted\0"
    "groupLocked\0groupUnlocked\0groupNameChanged\0"
    "toggleLockSelectedGroup\0deassignSelectedItem\0"
    "deleteSelectedGroups\0renameSelectedGroup"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GroupedTreeWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       4,    1,   62,    2, 0x06 /* Public */,
       5,    1,   65,    2, 0x06 /* Public */,
       6,    1,   68,    2, 0x06 /* Public */,
       7,    2,   71,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    0,   76,    2, 0x08 /* Private */,
       9,    0,   77,    2, 0x08 /* Private */,
      10,    0,   78,    2, 0x08 /* Private */,
      11,    0,   79,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    2,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void GroupedTreeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GroupedTreeWidget *_t = static_cast<GroupedTreeWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->itemsRemoved((*reinterpret_cast< const QList<QPair<QString,int> >(*)>(_a[1]))); break;
        case 1: _t->groupDeleted((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->groupLocked((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->groupUnlocked((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->groupNameChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 5: _t->toggleLockSelectedGroup(); break;
        case 6: _t->deassignSelectedItem(); break;
        case 7: _t->deleteSelectedGroups(); break;
        case 8: _t->renameSelectedGroup(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (GroupedTreeWidget::*_t)(const QList<QPair<QString,int> > & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GroupedTreeWidget::itemsRemoved)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (GroupedTreeWidget::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GroupedTreeWidget::groupDeleted)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (GroupedTreeWidget::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GroupedTreeWidget::groupLocked)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (GroupedTreeWidget::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GroupedTreeWidget::groupUnlocked)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (GroupedTreeWidget::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GroupedTreeWidget::groupNameChanged)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject GroupedTreeWidget::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_GroupedTreeWidget.data,
      qt_meta_data_GroupedTreeWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *GroupedTreeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GroupedTreeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GroupedTreeWidget.stringdata0))
        return static_cast<void*>(this);
    return QTreeWidget::qt_metacast(_clname);
}

int GroupedTreeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void GroupedTreeWidget::itemsRemoved(const QList<QPair<QString,int> > & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GroupedTreeWidget::groupDeleted(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void GroupedTreeWidget::groupLocked(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void GroupedTreeWidget::groupUnlocked(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void GroupedTreeWidget::groupNameChanged(QString _t1, QString _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
