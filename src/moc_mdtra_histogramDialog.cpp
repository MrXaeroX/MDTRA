/****************************************************************************
** Meta object code from reading C++ file 'mdtra_histogramDialog.h'
**
** Created: Thu 25. Feb 14:08:21 2016
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdtra_histogramDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdtra_histogramDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MDTRA_HistogramDialog[] = {

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
      23,   22,   22,   22, 0x08,
      32,   22,   22,   22, 0x08,
      66,   22,   22,   22, 0x08,
      91,   22,   22,   22, 0x08,
     116,   22,   22,   22, 0x08,
     140,   22,   22,   22, 0x08,
     161,   22,   22,   22, 0x08,
     179,   22,   22,   22, 0x08,
     194,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_HistogramDialog[] = {
    "MDTRA_HistogramDialog\0\0reject()\0"
    "exec_on_colorChangeButton_click()\0"
    "exec_on_desired_update()\0"
    "exec_on_desired_redraw()\0"
    "exec_on_result_change()\0exec_on_rds_change()\0"
    "exec_on_rebuild()\0exec_on_save()\0"
    "exec_on_export()\0"
};

const QMetaObject MDTRA_HistogramDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MDTRA_HistogramDialog,
      qt_meta_data_MDTRA_HistogramDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_HistogramDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_HistogramDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_HistogramDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_HistogramDialog))
        return static_cast<void*>(const_cast< MDTRA_HistogramDialog*>(this));
    if (!strcmp(_clname, "Ui_histogramDialog"))
        return static_cast< Ui_histogramDialog*>(const_cast< MDTRA_HistogramDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MDTRA_HistogramDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: reject(); break;
        case 1: exec_on_colorChangeButton_click(); break;
        case 2: exec_on_desired_update(); break;
        case 3: exec_on_desired_redraw(); break;
        case 4: exec_on_result_change(); break;
        case 5: exec_on_rds_change(); break;
        case 6: exec_on_rebuild(); break;
        case 7: exec_on_save(); break;
        case 8: exec_on_export(); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
