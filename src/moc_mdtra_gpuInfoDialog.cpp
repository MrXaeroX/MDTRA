/****************************************************************************
** Meta object code from reading C++ file 'mdtra_gpuInfoDialog.h'
**
** Created: Thu 25. Feb 14:08:21 2016
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdtra_gpuInfoDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdtra_gpuInfoDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MDTRA_GPUInfoDialog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x08,
      47,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_GPUInfoDialog[] = {
    "MDTRA_GPUInfoDialog\0\0exec_on_checkbox_change()\0"
    "exec_on_gpu_change()\0"
};

const QMetaObject MDTRA_GPUInfoDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MDTRA_GPUInfoDialog,
      qt_meta_data_MDTRA_GPUInfoDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_GPUInfoDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_GPUInfoDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_GPUInfoDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_GPUInfoDialog))
        return static_cast<void*>(const_cast< MDTRA_GPUInfoDialog*>(this));
    if (!strcmp(_clname, "Ui_gpuInfoDialog"))
        return static_cast< Ui_gpuInfoDialog*>(const_cast< MDTRA_GPUInfoDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MDTRA_GPUInfoDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: exec_on_checkbox_change(); break;
        case 1: exec_on_gpu_change(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
