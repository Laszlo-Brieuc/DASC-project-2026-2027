/****************************************************************************
** Meta object code from reading C++ file 'mainwindowclientbookencoder.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "mainwindowclientbookencoder.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindowclientbookencoder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindowClientBookEncoder_t {
    QByteArrayData data[9];
    char stringdata0[226];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindowClientBookEncoder_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindowClientBookEncoder_t qt_meta_stringdata_MainWindowClientBookEncoder = {
    {
QT_MOC_LITERAL(0, 0, 27), // "MainWindowClientBookEncoder"
QT_MOC_LITERAL(1, 28, 30), // "on_pushButtonAddAuthor_clicked"
QT_MOC_LITERAL(2, 59, 0), // ""
QT_MOC_LITERAL(3, 60, 31), // "on_pushButtonAddSubject_clicked"
QT_MOC_LITERAL(4, 92, 28), // "on_pushButtonAddBook_clicked"
QT_MOC_LITERAL(5, 121, 26), // "on_pushButtonClear_clicked"
QT_MOC_LITERAL(6, 148, 24), // "on_actionLogin_triggered"
QT_MOC_LITERAL(7, 173, 25), // "on_actionLogout_triggered"
QT_MOC_LITERAL(8, 199, 26) // "on_actionQuitter_triggered"

    },
    "MainWindowClientBookEncoder\0"
    "on_pushButtonAddAuthor_clicked\0\0"
    "on_pushButtonAddSubject_clicked\0"
    "on_pushButtonAddBook_clicked\0"
    "on_pushButtonClear_clicked\0"
    "on_actionLogin_triggered\0"
    "on_actionLogout_triggered\0"
    "on_actionQuitter_triggered"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindowClientBookEncoder[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x08 /* Private */,
       3,    0,   50,    2, 0x08 /* Private */,
       4,    0,   51,    2, 0x08 /* Private */,
       5,    0,   52,    2, 0x08 /* Private */,
       6,    0,   53,    2, 0x08 /* Private */,
       7,    0,   54,    2, 0x08 /* Private */,
       8,    0,   55,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindowClientBookEncoder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindowClientBookEncoder *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_pushButtonAddAuthor_clicked(); break;
        case 1: _t->on_pushButtonAddSubject_clicked(); break;
        case 2: _t->on_pushButtonAddBook_clicked(); break;
        case 3: _t->on_pushButtonClear_clicked(); break;
        case 4: _t->on_actionLogin_triggered(); break;
        case 5: _t->on_actionLogout_triggered(); break;
        case 6: _t->on_actionQuitter_triggered(); break;
        default: ;
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject MainWindowClientBookEncoder::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindowClientBookEncoder.data,
    qt_meta_data_MainWindowClientBookEncoder,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindowClientBookEncoder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindowClientBookEncoder::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindowClientBookEncoder.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindowClientBookEncoder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
