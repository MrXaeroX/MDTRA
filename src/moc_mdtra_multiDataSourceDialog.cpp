/****************************************************************************
** Meta object code from reading C++ file 'mdtra_multiDataSourceDialog.h'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdtra_multiDataSourceDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdtra_multiDataSourceDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MDTRA_MultiDataSourceDialog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      29,   28,   28,   28, 0x08,
      53,   28,   28,   28, 0x08,
      85,   28,   28,   28, 0x08,
     114,   28,   28,   28, 0x08,
     131,   28,   28,   28, 0x08,
     158,   28,   28,   28, 0x08,
     185,   28,   28,   28, 0x08,
     212,   28,   28,   28, 0x08,
     239,   28,   28,   28, 0x08,
     266,   28,   28,   28, 0x08,
     293,   28,   28,   28, 0x08,
     317,   28,   28,   28, 0x08,
     341,   28,   28,   28, 0x08,
     365,   28,   28,   28, 0x08,
     389,   28,   28,   28, 0x08,
     413,   28,   28,   28, 0x08,
     437,   28,   28,   28, 0x08,
     473,   28,   28,   28, 0x08,
     512,   28,   28,   28, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_MultiDataSourceDialog[] = {
    "MDTRA_MultiDataSourceDialog\0\0"
    "exec_on_update_stream()\0"
    "exec_on_check_dataSourceInput()\0"
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

const QMetaObject MDTRA_MultiDataSourceDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MDTRA_MultiDataSourceDialog,
      qt_meta_data_MDTRA_MultiDataSourceDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_MultiDataSourceDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_MultiDataSourceDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_MultiDataSourceDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_MultiDataSourceDialog))
        return static_cast<void*>(const_cast< MDTRA_MultiDataSourceDialog*>(this));
    if (!strcmp(_clname, "Ui_multiDataSourceDialog"))
        return static_cast< Ui_multiDataSourceDialog*>(const_cast< MDTRA_MultiDataSourceDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MDTRA_MultiDataSourceDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: exec_on_update_stream(); break;
        case 1: exec_on_check_dataSourceInput(); break;
        case 2: exec_on_check_args_enabled(); break;
        case 3: exec_on_accept(); break;
        case 4: exec_on_update_residue_a(); break;
        case 5: exec_on_update_residue_b(); break;
        case 6: exec_on_update_residue_c(); break;
        case 7: exec_on_update_residue_d(); break;
        case 8: exec_on_update_residue_e(); break;
        case 9: exec_on_update_residue_f(); break;
        case 10: exec_on_update_atom_a(); break;
        case 11: exec_on_update_atom_b(); break;
        case 12: exec_on_update_atom_c(); break;
        case 13: exec_on_update_atom_d(); break;
        case 14: exec_on_update_atom_e(); break;
        case 15: exec_on_update_atom_f(); break;
        case 16: exec_on_selection_editingFinished(); break;
        case 17: exec_on_selection_showWholeSelection(); break;
        case 18: exec_on_advanced(); break;
        default: ;
        }
        _id -= 19;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
