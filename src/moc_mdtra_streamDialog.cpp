/****************************************************************************
** Meta object code from reading C++ file 'mdtra_streamDialog.h'
**
** Created: Mon 16. Jan 19:04:10 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdtra_streamDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdtra_streamDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MDTRA_StreamDialog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x08,
      48,   19,   19,   19, 0x08,
      65,   19,   19,   19, 0x08,
      84,   19,   19,   19, 0x08,
     111,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_StreamDialog[] = {
    "MDTRA_StreamDialog\0\0exec_on_check_streamInput()\0"
    "exec_on_accept()\0add_stream_files()\0"
    "add_stream_files_by_mask()\0"
    "remove_stream_files()\0"
};

const QMetaObject MDTRA_StreamDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MDTRA_StreamDialog,
      qt_meta_data_MDTRA_StreamDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_StreamDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_StreamDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_StreamDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_StreamDialog))
        return static_cast<void*>(const_cast< MDTRA_StreamDialog*>(this));
    if (!strcmp(_clname, "Ui_streamDialog"))
        return static_cast< Ui_streamDialog*>(const_cast< MDTRA_StreamDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MDTRA_StreamDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: exec_on_check_streamInput(); break;
        case 1: exec_on_accept(); break;
        case 2: add_stream_files(); break;
        case 3: add_stream_files_by_mask(); break;
        case 4: remove_stream_files(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
