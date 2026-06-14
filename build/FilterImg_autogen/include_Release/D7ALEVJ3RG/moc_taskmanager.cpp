/****************************************************************************
** Meta object code from reading C++ file 'taskmanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../src/threading/taskmanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'taskmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TaskManager_t {
    QByteArrayData data[16];
    char stringdata0[198];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TaskManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TaskManager_t qt_meta_stringdata_TaskManager = {
    {
QT_MOC_LITERAL(0, 0, 11), // "TaskManager"
QT_MOC_LITERAL(1, 12, 15), // "progressChanged"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 7), // "percent"
QT_MOC_LITERAL(4, 37, 9), // "elapsedMs"
QT_MOC_LITERAL(5, 47, 11), // "remainingMs"
QT_MOC_LITERAL(6, 59, 15), // "regionCompleted"
QT_MOC_LITERAL(7, 75, 11), // "regionIndex"
QT_MOC_LITERAL(8, 87, 19), // "regionThreadIdReady"
QT_MOC_LITERAL(9, 107, 8), // "threadId"
QT_MOC_LITERAL(10, 116, 18), // "processingFinished"
QT_MOC_LITERAL(11, 135, 6), // "result"
QT_MOC_LITERAL(12, 142, 15), // "ProcessingStats"
QT_MOC_LITERAL(13, 158, 5), // "stats"
QT_MOC_LITERAL(14, 164, 19), // "processingCancelled"
QT_MOC_LITERAL(15, 184, 13) // "onAllFinished"

    },
    "TaskManager\0progressChanged\0\0percent\0"
    "elapsedMs\0remainingMs\0regionCompleted\0"
    "regionIndex\0regionThreadIdReady\0"
    "threadId\0processingFinished\0result\0"
    "ProcessingStats\0stats\0processingCancelled\0"
    "onAllFinished"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TaskManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   44,    2, 0x06 /* Public */,
       6,    2,   51,    2, 0x06 /* Public */,
       8,    2,   56,    2, 0x06 /* Public */,
      10,    2,   61,    2, 0x06 /* Public */,
      14,    0,   66,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      15,    0,   67,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::LongLong, QMetaType::LongLong,    3,    4,    5,
    QMetaType::Void, QMetaType::Int, QMetaType::LongLong,    7,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::LongLong,    7,    9,
    QMetaType::Void, QMetaType::QImage, 0x80000000 | 12,   11,   13,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void TaskManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TaskManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->progressChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2])),(*reinterpret_cast< qint64(*)>(_a[3]))); break;
        case 1: _t->regionCompleted((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        case 2: _t->regionThreadIdReady((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        case 3: _t->processingFinished((*reinterpret_cast< const QImage(*)>(_a[1])),(*reinterpret_cast< const ProcessingStats(*)>(_a[2]))); break;
        case 4: _t->processingCancelled(); break;
        case 5: _t->onAllFinished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TaskManager::*)(int , qint64 , qint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TaskManager::progressChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TaskManager::*)(int , qint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TaskManager::regionCompleted)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TaskManager::*)(int , qint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TaskManager::regionThreadIdReady)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (TaskManager::*)(const QImage & , const ProcessingStats & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TaskManager::processingFinished)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (TaskManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TaskManager::processingCancelled)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TaskManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_TaskManager.data,
    qt_meta_data_TaskManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TaskManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TaskManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TaskManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TaskManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void TaskManager::progressChanged(int _t1, qint64 _t2, qint64 _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TaskManager::regionCompleted(int _t1, qint64 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void TaskManager::regionThreadIdReady(int _t1, qint64 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void TaskManager::processingFinished(const QImage & _t1, const ProcessingStats & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void TaskManager::processingCancelled()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
