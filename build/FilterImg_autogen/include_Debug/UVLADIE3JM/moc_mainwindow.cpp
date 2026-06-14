/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[30];
    char stringdata0[368];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 11), // "onOpenImage"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 11), // "onSaveImage"
QT_MOC_LITERAL(4, 36, 13), // "onSaveAsImage"
QT_MOC_LITERAL(5, 50, 17), // "onReplaceOriginal"
QT_MOC_LITERAL(6, 68, 13), // "onApplyFilter"
QT_MOC_LITERAL(7, 82, 15), // "onPreviewFilter"
QT_MOC_LITERAL(8, 98, 20), // "onProcessingFinished"
QT_MOC_LITERAL(9, 119, 6), // "result"
QT_MOC_LITERAL(10, 126, 15), // "ProcessingStats"
QT_MOC_LITERAL(11, 142, 5), // "stats"
QT_MOC_LITERAL(12, 148, 21), // "onProcessingCancelled"
QT_MOC_LITERAL(13, 170, 18), // "onCancelProcessing"
QT_MOC_LITERAL(14, 189, 17), // "onProgressChanged"
QT_MOC_LITERAL(15, 207, 7), // "percent"
QT_MOC_LITERAL(16, 215, 7), // "elapsed"
QT_MOC_LITERAL(17, 223, 9), // "remaining"
QT_MOC_LITERAL(18, 233, 17), // "onRegionCompleted"
QT_MOC_LITERAL(19, 251, 3), // "idx"
QT_MOC_LITERAL(20, 255, 2), // "ms"
QT_MOC_LITERAL(21, 258, 20), // "onFilterChainChanged"
QT_MOC_LITERAL(22, 279, 19), // "onParametersChanged"
QT_MOC_LITERAL(23, 299, 12), // "onToggleGrid"
QT_MOC_LITERAL(24, 312, 4), // "show"
QT_MOC_LITERAL(25, 317, 8), // "onZoomIn"
QT_MOC_LITERAL(26, 326, 9), // "onZoomOut"
QT_MOC_LITERAL(27, 336, 11), // "onFitWindow"
QT_MOC_LITERAL(28, 348, 11), // "onZoomReset"
QT_MOC_LITERAL(29, 360, 7) // "onAbout"

    },
    "MainWindow\0onOpenImage\0\0onSaveImage\0"
    "onSaveAsImage\0onReplaceOriginal\0"
    "onApplyFilter\0onPreviewFilter\0"
    "onProcessingFinished\0result\0ProcessingStats\0"
    "stats\0onProcessingCancelled\0"
    "onCancelProcessing\0onProgressChanged\0"
    "percent\0elapsed\0remaining\0onRegionCompleted\0"
    "idx\0ms\0onFilterChainChanged\0"
    "onParametersChanged\0onToggleGrid\0show\0"
    "onZoomIn\0onZoomOut\0onFitWindow\0"
    "onZoomReset\0onAbout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  109,    2, 0x08 /* Private */,
       3,    0,  110,    2, 0x08 /* Private */,
       4,    0,  111,    2, 0x08 /* Private */,
       5,    0,  112,    2, 0x08 /* Private */,
       6,    0,  113,    2, 0x08 /* Private */,
       7,    0,  114,    2, 0x08 /* Private */,
       8,    2,  115,    2, 0x08 /* Private */,
      12,    0,  120,    2, 0x08 /* Private */,
      13,    0,  121,    2, 0x08 /* Private */,
      14,    3,  122,    2, 0x08 /* Private */,
      18,    2,  129,    2, 0x08 /* Private */,
      21,    0,  134,    2, 0x08 /* Private */,
      22,    0,  135,    2, 0x08 /* Private */,
      23,    1,  136,    2, 0x08 /* Private */,
      25,    0,  139,    2, 0x08 /* Private */,
      26,    0,  140,    2, 0x08 /* Private */,
      27,    0,  141,    2, 0x08 /* Private */,
      28,    0,  142,    2, 0x08 /* Private */,
      29,    0,  143,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QImage, 0x80000000 | 10,    9,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::LongLong, QMetaType::LongLong,   15,   16,   17,
    QMetaType::Void, QMetaType::Int, QMetaType::LongLong,   19,   20,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   24,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onOpenImage(); break;
        case 1: _t->onSaveImage(); break;
        case 2: _t->onSaveAsImage(); break;
        case 3: _t->onReplaceOriginal(); break;
        case 4: _t->onApplyFilter(); break;
        case 5: _t->onPreviewFilter(); break;
        case 6: _t->onProcessingFinished((*reinterpret_cast< const QImage(*)>(_a[1])),(*reinterpret_cast< const ProcessingStats(*)>(_a[2]))); break;
        case 7: _t->onProcessingCancelled(); break;
        case 8: _t->onCancelProcessing(); break;
        case 9: _t->onProgressChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2])),(*reinterpret_cast< qint64(*)>(_a[3]))); break;
        case 10: _t->onRegionCompleted((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        case 11: _t->onFilterChainChanged(); break;
        case 12: _t->onParametersChanged(); break;
        case 13: _t->onToggleGrid((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: _t->onZoomIn(); break;
        case 15: _t->onZoomOut(); break;
        case 16: _t->onFitWindow(); break;
        case 17: _t->onZoomReset(); break;
        case 18: _t->onAbout(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
