/****************************************************************************
** Meta object code from reading C++ file 'mdtra_distanceSearchResultsDialog.h'
**
** Created: Mon 16. Jan 19:04:10 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdtra_distanceSearchResultsDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdtra_distanceSearchResultsDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MDTRA_DistanceSearchResultsDialog[] = {

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
      35,   34,   34,   34, 0x08,
      47,   34,   34,   34, 0x08,
      80,   68,   34,   34, 0x08,
     108,   34,   34,   34, 0x08,
     134,   34,   34,   34, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_DistanceSearchResultsDialog[] = {
    "MDTRA_DistanceSearchResultsDialog\0\0"
    "exec_save()\0exec_on_result_add()\0"
    "headerIndex\0exec_on_header_clicked(int)\0"
    "addToNewResultCollector()\0"
    "addToExistingResultCollector()\0"
};

const QMetaObject MDTRA_DistanceSearchResultsDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MDTRA_DistanceSearchResultsDialog,
      qt_meta_data_MDTRA_DistanceSearchResultsDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_DistanceSearchResultsDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_DistanceSearchResultsDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_DistanceSearchResultsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_DistanceSearchResultsDialog))
        return static_cast<void*>(const_cast< MDTRA_DistanceSearchResultsDialog*>(this));
    if (!strcmp(_clname, "Ui_distanceSearchResultsDialog"))
        return static_cast< Ui_distanceSearchResultsDialog*>(const_cast< MDTRA_DistanceSearchResultsDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MDTRA_DistanceSearchResultsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
