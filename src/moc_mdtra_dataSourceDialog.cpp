/****************************************************************************
** Meta object code from reading C++ file 'mdtra_dataSourceDialog.h'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdtra_dataSourceDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdtra_dataSourceDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MDTRA_DataSourceDialog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      33,   24,   23,   23, 0x08,
      60,   23,   23,   23, 0x08,
      92,   23,   23,   23, 0x08,
     130,   23,   23,   23, 0x08,
     159,   23,   23,   23, 0x08,
     176,   23,   23,   23, 0x08,
     203,   23,   23,   23, 0x08,
     230,   23,   23,   23, 0x08,
     257,   23,   23,   23, 0x08,
     284,   23,   23,   23, 0x08,
     311,   23,   23,   23, 0x08,
     338,   23,   23,   23, 0x08,
     362,   23,   23,   23, 0x08,
     386,   23,   23,   23, 0x08,
     410,   23,   23,   23, 0x08,
     434,   23,   23,   23, 0x08,
     458,   23,   23,   23, 0x08,
     482,   23,   23,   23, 0x08,
     518,   23,   23,   23, 0x08,
     557,   23,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_DataSourceDialog[] = {
    "MDTRA_DataSourceDialog\0\0newIndex\0"
    "exec_on_update_stream(int)\0"
    "exec_on_check_dataSourceInput()\0"
    "exec_on_check_dataSourceInput_noSel()\0"
    "exec_on_check_args_enabled()\0"
    "exec_on_accept()\0exec_on_update_residue_a()\0"
    "exec_on_update_residue_b()\0"
    "exec_on_update_residue_c()\0"
    "exec_on_update_residue_d()\0"
    "exec_on_update_residue_e()\0"
    "exec_on_update_residue_f()\0"
    "exec_on_update_atom_a()\0exec_on_update_atom_b()\0"
    "exec_on_update_atom_c()\0exec_on_update_atom_d()\0"
    "exec_on_update_atom_e()\0exec_on_update_atom_f()\0"
    "exec_on_selection_editingFinished()\0"
    "exec_on_selection_showWholeSelection()\0"
    "exec_on_advanced()\0"
};

const QMetaObject MDTRA_DataSourceDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MDTRA_DataSourceDialog,
      qt_meta_data_MDTRA_DataSourceDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_DataSourceDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_DataSourceDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_DataSourceDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_DataSourceDialog))
        return static_cast<void*>(const_cast< MDTRA_DataSourceDialog*>(this));
    if (!strcmp(_clname, "Ui_dataSourceDialog"))
        return static_cast< Ui_dataSourceDialog*>(const_cast< MDTRA_DataSourceDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MDTRA_DataSourceDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: exec_on_update_stream((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: exec_on_check_dataSourceInput(); break;
        case 2: exec_on_check_dataSourceInput_noSel(); break;
        case 3: exec_on_check_args_enabled(); break;
        case 4: exec_on_accept(); break;
        case 5: exec_on_update_residue_a(); break;
        case 6: exec_on_update_residue_b(); break;
        case 7: exec_on_update_residue_c(); break;
        case 8: exec_on_update_residue_d(); break;
        case 9: exec_on_update_residue_e(); break;
        case 10: exec_on_update_residue_f(); break;
        case 11: exec_on_update_atom_a(); break;
        case 12: exec_on_update_atom_b(); break;
        case 13: exec_on_update_atom_c(); break;
        case 14: exec_on_update_atom_d(); break;
        case 15: exec_on_update_atom_e(); break;
        case 16: exec_on_update_atom_f(); break;
        case 17: exec_on_selection_editingFinished(); break;
        case 18: exec_on_selection_showWholeSelection(); break;
        case 19: exec_on_advanced(); break;
        default: ;
        }
        _id -= 20;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
