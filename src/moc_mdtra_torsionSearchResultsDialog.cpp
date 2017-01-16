/****************************************************************************
** Meta object code from reading C++ file 'mdtra_torsionSearchResultsDialog.h'
**
** Created: Mon 16. Jan 19:04:10 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdtra_torsionSearchResultsDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdtra_torsionSearchResultsDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MDTRA_TorsionSearchResultsDialog[] = {

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
      34,   33,   33,   33, 0x08,
      46,   33,   33,   33, 0x08,
      79,   67,   33,   33, 0x08,
     107,   33,   33,   33, 0x08,
     133,   33,   33,   33, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_TorsionSearchResultsDialog[] = {
    "MDTRA_TorsionSearchResultsDialog\0\0"
    "exec_save()\0exec_on_result_add()\0"
    "headerIndex\0exec_on_header_clicked(int)\0"
    "addToNewResultCollector()\0"
    "addToExistingResultCollector()\0"
};

const QMetaObject MDTRA_TorsionSearchResultsDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MDTRA_TorsionSearchResultsDialog,
      qt_meta_data_MDTRA_TorsionSearchResultsDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_TorsionSearchResultsDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_TorsionSearchResultsDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_TorsionSearchResultsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_TorsionSearchResultsDialog))
        return static_cast<void*>(const_cast< MDTRA_TorsionSearchResultsDialog*>(this));
    if (!strcmp(_clname, "Ui_torsionSearchResultsDialog"))
        return static_cast< Ui_torsionSearchResultsDialog*>(const_cast< MDTRA_TorsionSearchResultsDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MDTRA_TorsionSearchResultsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: exec_save(); break;
        case 1: exec_on_result_add(); break;
        case 2: exec_on_header_clicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: addToNewResultCollector(); break;
        case 4: addToExistingResultCollector(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
