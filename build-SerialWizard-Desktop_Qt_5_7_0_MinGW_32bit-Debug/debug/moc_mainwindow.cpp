/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[16];
    char stringdata0[202];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 18), // "serialStateChanged"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 14), // "openSerialPort"
QT_MOC_LITERAL(4, 46, 15), // "closeSerialPort"
QT_MOC_LITERAL(5, 62, 8), // "sendData"
QT_MOC_LITERAL(6, 71, 23), // "setOpenSerialButtonText"
QT_MOC_LITERAL(7, 95, 6), // "isOpen"
QT_MOC_LITERAL(8, 102, 18), // "displayReceiveData"
QT_MOC_LITERAL(9, 121, 4), // "data"
QT_MOC_LITERAL(10, 126, 15), // "displaySentData"
QT_MOC_LITERAL(11, 142, 4), // "open"
QT_MOC_LITERAL(12, 147, 4), // "save"
QT_MOC_LITERAL(13, 152, 4), // "tool"
QT_MOC_LITERAL(14, 157, 17), // "openDataValidator"
QT_MOC_LITERAL(15, 175, 26) // "openFrameInfoSettingDialog"

    },
    "MainWindow\0serialStateChanged\0\0"
    "openSerialPort\0closeSerialPort\0sendData\0"
    "setOpenSerialButtonText\0isOpen\0"
    "displayReceiveData\0data\0displaySentData\0"
    "open\0save\0tool\0openDataValidator\0"
    "openFrameInfoSettingDialog"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   77,    2, 0x0a /* Public */,
       4,    0,   78,    2, 0x0a /* Public */,
       5,    0,   79,    2, 0x0a /* Public */,
       6,    1,   80,    2, 0x08 /* Private */,
       8,    1,   83,    2, 0x08 /* Private */,
      10,    1,   86,    2, 0x08 /* Private */,
      11,    0,   89,    2, 0x08 /* Private */,
      12,    0,   90,    2, 0x08 /* Private */,
      13,    0,   91,    2, 0x08 /* Private */,
      14,    0,   92,    2, 0x08 /* Private */,
      15,    0,   93,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::QByteArray,    9,
    QMetaType::Void, QMetaType::QByteArray,    9,
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
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->serialStateChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->openSerialPort(); break;
        case 2: _t->closeSerialPort(); break;
        case 3: _t->sendData(); break;
        case 4: _t->setOpenSerialButtonText((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->displayReceiveData((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 6: _t->displaySentData((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 7: _t->open(); break;
        case 8: _t->save(); break;
        case 9: _t->tool(); break;
        case 10: _t->openDataValidator(); break;
        case 11: _t->openFrameInfoSettingDialog(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MainWindow::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::serialStateChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
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
void MainWindow::serialStateChanged(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
