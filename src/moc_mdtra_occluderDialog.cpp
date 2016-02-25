/****************************************************************************
** Meta object code from reading C++ file 'mdtra_occluderDialog.h'
**
** Created: Thu 25. Feb 14:08:21 2016
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdtra_occluderDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdtra_occluderDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MDTRA_OccluderDialog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x08,
      58,   21,   21,   21, 0x08,
      97,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_OccluderDialog[] = {
    "MDTRA_OccluderDialog\0\0"
    "exec_on_selection_editingFinished()\0"
    "exec_on_selection_showWholeSelection()\0"
    "exec_on_check_input()\0"
};

const QMetaObject MDTRA_OccluderDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MDTRA_OccluderDialog,
      qt_meta_data_MDTRA_OccluderDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_OccluderDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_OccluderDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_OccluderDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_OccluderDialog))
        return static_cast<void*>(const_cast< MDTRA_OccluderDialog*>(this));
    if (!strcmp(_clname, "Ui_occluderDialog"))
        return static_cast< Ui_occluderDialog*>(const_cast< MDTRA_OccluderDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MDTRA_OccluderDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: exec_on_selection_editingFinished(); break;
        case 1: exec_on_selection_showWholeSelection(); break;
        case 2: exec_on_check_input(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
