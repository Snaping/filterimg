/****************************************************************************
** Meta object code from reading C++ file 'filtercontrolspanel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/widgets/filtercontrolspanel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filtercontrolspanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FilterControlsPanel_t {
    QByteArrayData data[17];
    char stringdata0[193];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FilterControlsPanel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FilterControlsPanel_t qt_meta_stringdata_FilterControlsPanel = {
    {
QT_MOC_LITERAL(0, 0, 19), // "FilterControlsPanel"
QT_MOC_LITERAL(1, 20, 18), // "filterChainChanged"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 17), // "parametersChanged"
QT_MOC_LITERAL(4, 58, 11), // "setGridRows"
QT_MOC_LITERAL(5, 70, 4), // "rows"
QT_MOC_LITERAL(6, 75, 11), // "setGridCols"
QT_MOC_LITERAL(7, 87, 4), // "cols"
QT_MOC_LITERAL(8, 92, 8), // "gridRows"
QT_MOC_LITERAL(9, 101, 8), // "gridCols"
QT_MOC_LITERAL(10, 110, 11), // "onAddFilter"
QT_MOC_LITERAL(11, 122, 14), // "onRemoveFilter"
QT_MOC_LITERAL(12, 137, 8), // "onMoveUp"
QT_MOC_LITERAL(13, 146, 10), // "onMoveDown"
QT_MOC_LITERAL(14, 157, 16), // "onFilterSelected"
QT_MOC_LITERAL(15, 174, 3), // "row"
QT_MOC_LITERAL(16, 178, 14) // "onParamChanged"

    },
    "FilterControlsPanel\0filterChainChanged\0"
    "\0parametersChanged\0setGridRows\0rows\0"
    "setGridCols\0cols\0gridRows\0gridCols\0"
    "onAddFilter\0onRemoveFilter\0onMoveUp\0"
    "onMoveDown\0onFilterSelected\0row\0"
    "onParamChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FilterControlsPanel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x06 /* Public */,
       3,    0,   75,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   76,    2, 0x0a /* Public */,
       6,    1,   79,    2, 0x0a /* Public */,
       8,    0,   82,    2, 0x0a /* Public */,
       9,    0,   83,    2, 0x0a /* Public */,
      10,    0,   84,    2, 0x08 /* Private */,
      11,    0,   85,    2, 0x08 /* Private */,
      12,    0,   86,    2, 0x08 /* Private */,
      13,    0,   87,    2, 0x08 /* Private */,
      14,    1,   88,    2, 0x08 /* Private */,
      16,    0,   91,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void,

       0        // eod
};

void FilterControlsPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<FilterControlsPanel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->filterChainChanged(); break;
        case 1: _t->parametersChanged(); break;
        case 2: _t->setGridRows((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->setGridCols((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: { int _r = _t->gridRows();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 5: { int _r = _t->gridCols();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 6: _t->onAddFilter(); break;
        case 7: _t->onRemoveFilter(); break;
        case 8: _t->onMoveUp(); break;
        case 9: _t->onMoveDown(); break;
        case 10: _t->onFilterSelected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->onParamChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (FilterControlsPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FilterControlsPanel::filterChainChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (FilterControlsPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FilterControlsPanel::parametersChanged)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject FilterControlsPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_FilterControlsPanel.data,
    qt_meta_data_FilterControlsPanel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *FilterControlsPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FilterControlsPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FilterControlsPanel.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int FilterControlsPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void FilterControlsPanel::filterChainChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void FilterControlsPanel::parametersChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
