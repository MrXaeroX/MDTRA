/****************************************************************************
** Meta object code from reading C++ file 'mdtra_2D_RMSD_Dialog.h'
**
** Created: Mon 16. Jan 19:04:10 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdtra_2D_RMSD_Dialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdtra_2D_RMSD_Dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MDTRA_2D_RMSD_Dialog[] = {

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
      22,   21,   21,   21, 0x08,
      31,   21,   21,   21, 0x08,
      55,   21,   21,   21, 0x08,
      91,   21,   21,   21, 0x08,
     130,   21,   21,   21, 0x08,
     154,   21,   21,   21, 0x08,
     178,   21,   21,   21, 0x08,
     206,   21,   21,   21, 0x08,
     224,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_2D_RMSD_Dialog[] = {
    "MDTRA_2D_RMSD_Dialog\0\0reject()\0"
    "exec_on_stream_change()\0"
    "exec_on_selection_editingFinished()\0"
    "exec_on_selection_showWholeSelection()\0"
    "exec_on_toggle_smooth()\0exec_on_toggle_legend()\0"
    "exec_on_update_plot_title()\0"
    "exec_on_rebuild()\0exec_on_save()\0"
};

const QMetaObject MDTRA_2D_RMSD_Dialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MDTRA_2D_RMSD_Dialog,
      qt_meta_data_MDTRA_2D_RMSD_Dialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_2D_RMSD_Dialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_2D_RMSD_Dialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_2D_RMSD_Dialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_2D_RMSD_Dialog))
        return static_cast<void*>(const_cast< MDTRA_2D_RMSD_Dialog*>(this));
    if (!strcmp(_clname, "Ui_rmsd2dDialog"))
        return static_cast< Ui_rmsd2dDialog*>(const_cast< MDTRA_2D_RMSD_Dialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MDTRA_2D_RMSD_Dialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: reject(); break;
        case 1: exec_on_stream_change(); break;
        case 2: exec_on_selection_editingFinished(); break;
        case 3: exec_on_selection_showWholeSelection(); break;
        case 4: exec_on_toggle_smooth(); break;
        case 5: exec_on_toggle_legend(); break;
        case 6: exec_on_update_plot_title(); break;
        case 7: exec_on_rebuild(); break;
        case 8: exec_on_save(); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
