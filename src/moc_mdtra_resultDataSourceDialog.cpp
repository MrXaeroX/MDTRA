/****************************************************************************
** Meta object code from reading C++ file 'mdtra_resultDataSourceDialog.h'
**
** Created: Mon 16. Jan 19:04:10 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdtra_resultDataSourceDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdtra_resultDataSourceDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MDTRA_ResultDataSourceDialog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_ResultDataSourceDialog[] = {
    "MDTRA_ResultDataSourceDialog\0"
};

const QMetaObject MDTRA_ResultDataSourceDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MDTRA_ResultDataSourceDialog,
      qt_meta_data_MDTRA_ResultDataSourceDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_ResultDataSourceDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_ResultDataSourceDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_ResultDataSourceDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_ResultDataSourceDialog))
        return static_cast<void*>(const_cast< MDTRA_ResultDataSourceDialog*>(this));
    if (!strcmp(_clname, "Ui_resultDataSourceDialog"))
        return static_cast< Ui_resultDataSourceDialog*>(const_cast< MDTRA_ResultDataSourceDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MDTRA_ResultDataSourceDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
