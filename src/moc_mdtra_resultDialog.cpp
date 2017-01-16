/****************************************************************************
** Meta object code from reading C++ file 'mdtra_resultDialog.h'
**
** Created: Mon 16. Jan 19:04:10 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdtra_resultDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdtra_resultDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MDTRA_ResultDialog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x08,
      60,   19,   19,   19, 0x08,
      88,   19,   19,   19, 0x08,
     105,   19,   19,   19, 0x08,
     138,   19,   19,   19, 0x08,
     181,   19,   19,   19, 0x08,
     215,   19,   19,   19, 0x08,
     251,   19,   19,   19, 0x08,
     283,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_ResultDialog[] = {
    "MDTRA_ResultDialog\0\0"
    "exec_on_update_layout_and_scale_units()\0"
    "exec_on_check_resultInput()\0"
    "exec_on_accept()\0exec_on_add_result_data_source()\0"
    "exec_on_add_multiple_result_data_sources()\0"
    "exec_on_edit_result_data_source()\0"
    "exec_on_remove_result_data_source()\0"
    "exec_on_up_result_data_source()\0"
    "exec_on_down_result_data_source()\0"
};

const QMetaObject MDTRA_ResultDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MDTRA_ResultDialog,
      qt_meta_data_MDTRA_ResultDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_ResultDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_ResultDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_ResultDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_ResultDialog))
        return static_cast<void*>(const_cast< MDTRA_ResultDialog*>(this));
    if (!strcmp(_clname, "Ui_resultDialog"))
        return static_cast< Ui_resultDialog*>(const_cast< MDTRA_ResultDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MDTRA_ResultDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: exec_on_update_layout_and_scale_units(); break;
        case 1: exec_on_check_resultInput(); break;
        case 2: exec_on_accept(); break;
        case 3: exec_on_add_result_data_source(); break;
        case 4: exec_on_add_multiple_result_data_sources(); break;
        case 5: exec_on_edit_result_data_source(); break;
        case 6: exec_on_remove_result_data_source(); break;
        case 7: exec_on_up_result_data_source(); break;
        case 8: exec_on_down_result_data_source(); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
