/****************************************************************************
** Meta object code from reading C++ file 'mdtra_plotDataFilterDialog.h'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdtra_plotDataFilterDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdtra_plotDataFilterDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MDTRA_PlotDataFilterDialog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      28,   27,   27,   27, 0x0a,
      37,   27,   27,   27, 0x08,
      56,   27,   27,   27, 0x08,
      75,   27,   27,   27, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_PlotDataFilterDialog[] = {
    "MDTRA_PlotDataFilterDialog\0\0reject()\0"
    "checkbox_clicked()\0pe_combo_changed()\0"
    "ie_combo_changed()\0"
};

const QMetaObject MDTRA_PlotDataFilterDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MDTRA_PlotDataFilterDialog,
      qt_meta_data_MDTRA_PlotDataFilterDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_PlotDataFilterDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_PlotDataFilterDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_PlotDataFilterDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_PlotDataFilterDialog))
        return static_cast<void*>(const_cast< MDTRA_PlotDataFilterDialog*>(this));
    if (!strcmp(_clname, "Ui_plotDataFilterDialog"))
        return static_cast< Ui_plotDataFilterDialog*>(const_cast< MDTRA_PlotDataFilterDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MDTRA_PlotDataFilterDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: reject(); break;
        case 1: checkbox_clicked(); break;
        case 2: pe_combo_changed(); break;
        case 3: ie_combo_changed(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
