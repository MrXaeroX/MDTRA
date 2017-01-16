/****************************************************************************
** Meta object code from reading C++ file 'mdtra_hbSearchResultsDialog.h'
**
** Created: Mon 16. Jan 19:04:10 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdtra_hbSearchResultsDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdtra_hbSearchResultsDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MDTRA_HBSearchResultsDialog[] = {

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
      29,   28,   28,   28, 0x0a,
      41,   28,   28,   28, 0x0a,
      74,   62,   28,   28, 0x0a,
     102,   28,   28,   28, 0x0a,
     130,   28,   28,   28, 0x0a,
     158,   28,   28,   28, 0x0a,
     187,   28,   28,   28, 0x0a,
     220,   28,   28,   28, 0x0a,
     253,   28,   28,   28, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_HBSearchResultsDialog[] = {
    "MDTRA_HBSearchResultsDialog\0\0exec_save()\0"
    "exec_on_result_add()\0headerIndex\0"
    "exec_on_header_clicked(int)\0"
    "addToNewResultCollectorXY()\0"
    "addToNewResultCollectorHY()\0"
    "addToNewResultCollectorXHY()\0"
    "addToExistingResultCollectorXY()\0"
    "addToExistingResultCollectorHY()\0"
    "addToExistingResultCollectorXHY()\0"
};

const QMetaObject MDTRA_HBSearchResultsDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MDTRA_HBSearchResultsDialog,
      qt_meta_data_MDTRA_HBSearchResultsDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_HBSearchResultsDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_HBSearchResultsDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_HBSearchResultsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_HBSearchResultsDialog))
        return static_cast<void*>(const_cast< MDTRA_HBSearchResultsDialog*>(this));
    if (!strcmp(_clname, "Ui_hbSearchResultsDialog"))
        return static_cast< Ui_hbSearchResultsDialog*>(const_cast< MDTRA_HBSearchResultsDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MDTRA_HBSearchResultsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: exec_save(); break;
        case 1: exec_on_result_add(); break;
        case 2: exec_on_header_clicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: addToNewResultCollectorXY(); break;
        case 4: addToNewResultCollectorHY(); break;
        case 5: addToNewResultCollectorXHY(); break;
        case 6: addToExistingResultCollectorXY(); break;
        case 7: addToExistingResultCollectorHY(); break;
        case 8: addToExistingResultCollectorXHY(); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
