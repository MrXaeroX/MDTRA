/****************************************************************************
** Meta object code from reading C++ file 'mdtra_prepWaterShellDialog.h'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdtra_prepWaterShellDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdtra_prepWaterShellDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MDTRA_PrepWaterShellDialog[] = {

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
      28,   27,   27,   27, 0x08,
      37,   27,   27,   27, 0x08,
      61,   27,   27,   27, 0x08,
      85,   27,   27,   27, 0x08,
     107,   27,   27,   27, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_PrepWaterShellDialog[] = {
    "MDTRA_PrepWaterShellDialog\0\0reject()\0"
    "exec_on_stream_change()\0exec_on_browse_output()\0"
    "exec_on_check_input()\0exec_on_prep()\0"
};

const QMetaObject MDTRA_PrepWaterShellDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MDTRA_PrepWaterShellDialog,
      qt_meta_data_MDTRA_PrepWaterShellDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_PrepWaterShellDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_PrepWaterShellDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_PrepWaterShellDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_PrepWaterShellDialog))
        return static_cast<void*>(const_cast< MDTRA_PrepWaterShellDialog*>(this));
    if (!strcmp(_clname, "Ui_prepWaterShellDialog"))
        return static_cast< Ui_prepWaterShellDialog*>(const_cast< MDTRA_PrepWaterShellDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MDTRA_PrepWaterShellDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: reject(); break;
        case 1: exec_on_stream_change(); break;
        case 2: exec_on_browse_output(); break;
        case 3: exec_on_check_input(); break;
        case 4: exec_on_prep(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
