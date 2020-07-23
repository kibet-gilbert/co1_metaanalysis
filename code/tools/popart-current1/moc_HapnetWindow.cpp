/****************************************************************************
** Meta object code from reading C++ file 'HapnetWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/gui/HapnetWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HapnetWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_HapnetWindow_t {
    QByteArrayData data[68];
    char stringdata0[1006];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HapnetWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HapnetWindow_t qt_meta_stringdata_HapnetWindow = {
    {
QT_MOC_LITERAL(0, 0, 12), // "HapnetWindow"
QT_MOC_LITERAL(1, 13, 11), // "sizeChanged"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 9), // "printprog"
QT_MOC_LITERAL(4, 36, 13), // "openAlignment"
QT_MOC_LITERAL(5, 50, 13), // "saveNexusFile"
QT_MOC_LITERAL(6, 64, 15), // "importAlignment"
QT_MOC_LITERAL(7, 80, 12), // "importTraits"
QT_MOC_LITERAL(8, 93, 13), // "importGeoTags"
QT_MOC_LITERAL(9, 107, 15), // "changeDelimiter"
QT_MOC_LITERAL(10, 123, 14), // "setMergeDelims"
QT_MOC_LITERAL(11, 138, 12), // "setHasHeader"
QT_MOC_LITERAL(12, 151, 13), // "setHasVHeader"
QT_MOC_LITERAL(13, 165, 14), // "closeAlignment"
QT_MOC_LITERAL(14, 180, 10), // "closeTrees"
QT_MOC_LITERAL(15, 191, 11), // "closeTraits"
QT_MOC_LITERAL(16, 203, 12), // "saveGraphics"
QT_MOC_LITERAL(17, 216, 13), // "exportNetwork"
QT_MOC_LITERAL(18, 230, 4), // "quit"
QT_MOC_LITERAL(19, 235, 8), // "buildMSN"
QT_MOC_LITERAL(20, 244, 8), // "buildMJN"
QT_MOC_LITERAL(21, 253, 8), // "buildAPN"
QT_MOC_LITERAL(22, 262, 10), // "buildIntNJ"
QT_MOC_LITERAL(23, 273, 8), // "buildTCS"
QT_MOC_LITERAL(24, 282, 8), // "buildTSW"
QT_MOC_LITERAL(25, 291, 8), // "buildUMP"
QT_MOC_LITERAL(26, 300, 14), // "displayNetwork"
QT_MOC_LITERAL(27, 315, 15), // "finaliseDisplay"
QT_MOC_LITERAL(28, 331, 18), // "finaliseClustering"
QT_MOC_LITERAL(29, 350, 12), // "showNetError"
QT_MOC_LITERAL(30, 363, 17), // "showIdenticalSeqs"
QT_MOC_LITERAL(31, 381, 23), // "showNucleotideDiversity"
QT_MOC_LITERAL(32, 405, 12), // "showSegSites"
QT_MOC_LITERAL(33, 418, 18), // "showParsimonySites"
QT_MOC_LITERAL(34, 437, 11), // "showTajimaD"
QT_MOC_LITERAL(35, 449, 12), // "computeAmova"
QT_MOC_LITERAL(36, 462, 9), // "showAmova"
QT_MOC_LITERAL(37, 472, 12), // "showAllStats"
QT_MOC_LITERAL(38, 485, 6), // "search"
QT_MOC_LITERAL(39, 492, 17), // "changeColourTheme"
QT_MOC_LITERAL(40, 510, 12), // "changeColour"
QT_MOC_LITERAL(41, 523, 14), // "setTraitGroups"
QT_MOC_LITERAL(42, 538, 14), // "setTraitColour"
QT_MOC_LITERAL(43, 553, 18), // "changeVertexColour"
QT_MOC_LITERAL(44, 572, 16), // "changeVertexSize"
QT_MOC_LITERAL(45, 589, 16), // "changeEdgeColour"
QT_MOC_LITERAL(46, 606, 22), // "changeEdgeMutationView"
QT_MOC_LITERAL(47, 629, 8), // "QAction*"
QT_MOC_LITERAL(48, 638, 10), // "toggleView"
QT_MOC_LITERAL(49, 649, 18), // "toggleActiveTraits"
QT_MOC_LITERAL(50, 668, 20), // "toggleExternalLegend"
QT_MOC_LITERAL(51, 689, 19), // "updateTraitLocation"
QT_MOC_LITERAL(52, 709, 22), // "std::pair<float,float>"
QT_MOC_LITERAL(53, 732, 22), // "changeBackgroundColour"
QT_MOC_LITERAL(54, 755, 20), // "toggleShowNodeLabels"
QT_MOC_LITERAL(55, 776, 15), // "changeLabelFont"
QT_MOC_LITERAL(56, 792, 16), // "changeLegendFont"
QT_MOC_LITERAL(57, 809, 14), // "changeMapTheme"
QT_MOC_LITERAL(58, 824, 13), // "redrawNetwork"
QT_MOC_LITERAL(59, 838, 16), // "toggleNetActions"
QT_MOC_LITERAL(60, 855, 22), // "toggleAlignmentActions"
QT_MOC_LITERAL(61, 878, 18), // "toggleTraitActions"
QT_MOC_LITERAL(62, 897, 17), // "fixBarchartButton"
QT_MOC_LITERAL(63, 915, 15), // "fixTaxBoxButton"
QT_MOC_LITERAL(64, 931, 12), // "graphicsMove"
QT_MOC_LITERAL(65, 944, 37), // "QList<QPair<QGraphicsItem*,QP..."
QT_MOC_LITERAL(66, 982, 17), // "showDocumentation"
QT_MOC_LITERAL(67, 1000, 5) // "about"

    },
    "HapnetWindow\0sizeChanged\0\0printprog\0"
    "openAlignment\0saveNexusFile\0importAlignment\0"
    "importTraits\0importGeoTags\0changeDelimiter\0"
    "setMergeDelims\0setHasHeader\0setHasVHeader\0"
    "closeAlignment\0closeTrees\0closeTraits\0"
    "saveGraphics\0exportNetwork\0quit\0"
    "buildMSN\0buildMJN\0buildAPN\0buildIntNJ\0"
    "buildTCS\0buildTSW\0buildUMP\0displayNetwork\0"
    "finaliseDisplay\0finaliseClustering\0"
    "showNetError\0showIdenticalSeqs\0"
    "showNucleotideDiversity\0showSegSites\0"
    "showParsimonySites\0showTajimaD\0"
    "computeAmova\0showAmova\0showAllStats\0"
    "search\0changeColourTheme\0changeColour\0"
    "setTraitGroups\0setTraitColour\0"
    "changeVertexColour\0changeVertexSize\0"
    "changeEdgeColour\0changeEdgeMutationView\0"
    "QAction*\0toggleView\0toggleActiveTraits\0"
    "toggleExternalLegend\0updateTraitLocation\0"
    "std::pair<float,float>\0changeBackgroundColour\0"
    "toggleShowNodeLabels\0changeLabelFont\0"
    "changeLegendFont\0changeMapTheme\0"
    "redrawNetwork\0toggleNetActions\0"
    "toggleAlignmentActions\0toggleTraitActions\0"
    "fixBarchartButton\0fixTaxBoxButton\0"
    "graphicsMove\0QList<QPair<QGraphicsItem*,QPointF> >\0"
    "showDocumentation\0about"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HapnetWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      63,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  329,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,  332,    2, 0x08 /* Private */,
       4,    0,  335,    2, 0x08 /* Private */,
       5,    0,  336,    2, 0x08 /* Private */,
       6,    0,  337,    2, 0x08 /* Private */,
       7,    0,  338,    2, 0x08 /* Private */,
       8,    0,  339,    2, 0x08 /* Private */,
       9,    1,  340,    2, 0x08 /* Private */,
      10,    1,  343,    2, 0x08 /* Private */,
      11,    1,  346,    2, 0x08 /* Private */,
      12,    1,  349,    2, 0x08 /* Private */,
      13,    0,  352,    2, 0x08 /* Private */,
      14,    0,  353,    2, 0x08 /* Private */,
      15,    0,  354,    2, 0x08 /* Private */,
      16,    0,  355,    2, 0x08 /* Private */,
      17,    0,  356,    2, 0x08 /* Private */,
      18,    0,  357,    2, 0x08 /* Private */,
      19,    0,  358,    2, 0x08 /* Private */,
      20,    0,  359,    2, 0x08 /* Private */,
      21,    0,  360,    2, 0x08 /* Private */,
      22,    0,  361,    2, 0x08 /* Private */,
      23,    0,  362,    2, 0x08 /* Private */,
      24,    0,  363,    2, 0x08 /* Private */,
      25,    0,  364,    2, 0x08 /* Private */,
      26,    0,  365,    2, 0x08 /* Private */,
      27,    0,  366,    2, 0x08 /* Private */,
      28,    0,  367,    2, 0x08 /* Private */,
      29,    1,  368,    2, 0x08 /* Private */,
      30,    0,  371,    2, 0x08 /* Private */,
      31,    0,  372,    2, 0x08 /* Private */,
      32,    0,  373,    2, 0x08 /* Private */,
      33,    0,  374,    2, 0x08 /* Private */,
      34,    0,  375,    2, 0x08 /* Private */,
      35,    0,  376,    2, 0x08 /* Private */,
      36,    0,  377,    2, 0x08 /* Private */,
      37,    0,  378,    2, 0x08 /* Private */,
      38,    0,  379,    2, 0x08 /* Private */,
      39,    0,  380,    2, 0x08 /* Private */,
      40,    1,  381,    2, 0x08 /* Private */,
      41,    0,  384,    2, 0x08 /* Private */,
      42,    0,  385,    2, 0x08 /* Private */,
      43,    0,  386,    2, 0x08 /* Private */,
      44,    0,  387,    2, 0x08 /* Private */,
      45,    0,  388,    2, 0x08 /* Private */,
      46,    1,  389,    2, 0x08 /* Private */,
      48,    0,  392,    2, 0x08 /* Private */,
      49,    0,  393,    2, 0x08 /* Private */,
      50,    0,  394,    2, 0x08 /* Private */,
      51,    2,  395,    2, 0x08 /* Private */,
      53,    0,  400,    2, 0x08 /* Private */,
      54,    0,  401,    2, 0x08 /* Private */,
      55,    0,  402,    2, 0x08 /* Private */,
      56,    0,  403,    2, 0x08 /* Private */,
      57,    1,  404,    2, 0x08 /* Private */,
      58,    0,  407,    2, 0x08 /* Private */,
      59,    1,  408,    2, 0x08 /* Private */,
      60,    1,  411,    2, 0x08 /* Private */,
      61,    1,  414,    2, 0x08 /* Private */,
      62,    1,  417,    2, 0x08 /* Private */,
      63,    1,  420,    2, 0x08 /* Private */,
      64,    1,  423,    2, 0x08 /* Private */,
      66,    0,  426,    2, 0x08 /* Private */,
      67,    0,  427,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QSize,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 47,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt, 0x80000000 | 52,    2,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 47,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, 0x80000000 | 65,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void HapnetWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HapnetWindow *_t = static_cast<HapnetWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sizeChanged((*reinterpret_cast< const QSize(*)>(_a[1]))); break;
        case 1: _t->printprog((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->openAlignment(); break;
        case 3: _t->saveNexusFile(); break;
        case 4: _t->importAlignment(); break;
        case 5: _t->importTraits(); break;
        case 6: _t->importGeoTags(); break;
        case 7: _t->changeDelimiter((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->setMergeDelims((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->setHasHeader((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->setHasVHeader((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->closeAlignment(); break;
        case 12: _t->closeTrees(); break;
        case 13: _t->closeTraits(); break;
        case 14: _t->saveGraphics(); break;
        case 15: _t->exportNetwork(); break;
        case 16: _t->quit(); break;
        case 17: _t->buildMSN(); break;
        case 18: _t->buildMJN(); break;
        case 19: _t->buildAPN(); break;
        case 20: _t->buildIntNJ(); break;
        case 21: _t->buildTCS(); break;
        case 22: _t->buildTSW(); break;
        case 23: _t->buildUMP(); break;
        case 24: _t->displayNetwork(); break;
        case 25: _t->finaliseDisplay(); break;
        case 26: _t->finaliseClustering(); break;
        case 27: _t->showNetError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 28: _t->showIdenticalSeqs(); break;
        case 29: _t->showNucleotideDiversity(); break;
        case 30: _t->showSegSites(); break;
        case 31: _t->showParsimonySites(); break;
        case 32: _t->showTajimaD(); break;
        case 33: _t->computeAmova(); break;
        case 34: _t->showAmova(); break;
        case 35: _t->showAllStats(); break;
        case 36: _t->search(); break;
        case 37: _t->changeColourTheme(); break;
        case 38: _t->changeColour((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 39: _t->setTraitGroups(); break;
        case 40: _t->setTraitColour(); break;
        case 41: _t->changeVertexColour(); break;
        case 42: _t->changeVertexSize(); break;
        case 43: _t->changeEdgeColour(); break;
        case 44: _t->changeEdgeMutationView((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 45: _t->toggleView(); break;
        case 46: _t->toggleActiveTraits(); break;
        case 47: _t->toggleExternalLegend(); break;
        case 48: _t->updateTraitLocation((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< std::pair<float,float>(*)>(_a[2]))); break;
        case 49: _t->changeBackgroundColour(); break;
        case 50: _t->toggleShowNodeLabels(); break;
        case 51: _t->changeLabelFont(); break;
        case 52: _t->changeLegendFont(); break;
        case 53: _t->changeMapTheme((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 54: _t->redrawNetwork(); break;
        case 55: _t->toggleNetActions((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 56: _t->toggleAlignmentActions((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 57: _t->toggleTraitActions((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 58: _t->fixBarchartButton((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 59: _t->fixTaxBoxButton((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 60: _t->graphicsMove((*reinterpret_cast< QList<QPair<QGraphicsItem*,QPointF> >(*)>(_a[1]))); break;
        case 61: _t->showDocumentation(); break;
        case 62: _t->about(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 44:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        case 53:
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
            typedef void (HapnetWindow::*_t)(const QSize & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HapnetWindow::sizeChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject HapnetWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_HapnetWindow.data,
      qt_meta_data_HapnetWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *HapnetWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HapnetWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HapnetWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int HapnetWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 63)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 63;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 63)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 63;
    }
    return _id;
}

// SIGNAL 0
void HapnetWindow::sizeChanged(const QSize & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
