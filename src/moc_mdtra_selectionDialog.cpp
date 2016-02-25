/****************************************************************************
** Meta object code from reading C++ file 'mdtra_selectionDialog.h'
**
** Created: Thu 25. Feb 14:08:21 2016
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdtra_selectionDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdtra_selectionDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MDTRA_SelectionDialog[] = {

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
      23,   22,   22,   22, 0x08,
      59,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_SelectionDialog[] = {
    "MDTRA_SelectionDialog\0\0"
    "exec_on_selection_editingFinished()\0"
    "exec_on_selection_showWholeSelection()\0"
};

const QMetaObject MDTRA_SelectionDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MDTRA_SelectionDialog,
      qt_meta_data_MDTRA_SelectionDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_SelectionDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_SelectionDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_SelectionDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_SelectionDialog))
        return static_cast<void*>(const_cast< MDTRA_SelectionDialog*>(this));
    if (!strcmp(_clname, "Ui_selectionDialog"))
        return static_cast< Ui_selectionDialog*>(const_cast< MDTRA_SelectionDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MDTRA_SelectionDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: exec_on_selection_editingFinished(); break;
        case 1: exec_on_selection_showWholeSelection(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
