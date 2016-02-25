/****************************************************************************
** Meta object code from reading C++ file 'mdtra_preferencesDialog.h'
**
** Created: Thu 25. Feb 14:08:22 2016
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdtra_preferencesDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdtra_preferencesDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MDTRA_PreferencesDialog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      25,   24,   24,   24, 0x08,
      47,   42,   24,   24, 0x08,
      95,   42,   24,   24, 0x08,
     146,   24,   24,   24, 0x08,
     180,   24,   24,   24, 0x08,
     211,   24,   24,   24, 0x08,
     239,   24,   24,   24, 0x08,
     260,   24,   24,   24, 0x08,
     282,   24,   24,   24, 0x08,
     306,   24,   24,   24, 0x08,
     322,   24,   24,   24, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_PreferencesDialog[] = {
    "MDTRA_PreferencesDialog\0\0exec_on_accept()\0"
    "item\0exec_on_colorList_itemClicked(QListWidgetItem*)\0"
    "exec_on_colorList_itemDblClicked(QListWidgetItem*)\0"
    "exec_on_colorChangeButton_click()\0"
    "exec_on_formatList_selChange()\0"
    "exec_on_format_setDefault()\0"
    "exec_on_format_add()\0exec_on_format_edit()\0"
    "exec_on_format_delete()\0browse_RasMol()\0"
    "browse_VMD()\0"
};

const QMetaObject MDTRA_PreferencesDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MDTRA_PreferencesDialog,
      qt_meta_data_MDTRA_PreferencesDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_PreferencesDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_PreferencesDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_PreferencesDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_PreferencesDialog))
        return static_cast<void*>(const_cast< MDTRA_PreferencesDialog*>(this));
    if (!strcmp(_clname, "Ui_preferencesDialog"))
        return static_cast< Ui_preferencesDialog*>(const_cast< MDTRA_PreferencesDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MDTRA_PreferencesDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: exec_on_accept(); break;
        case 1: exec_on_colorList_itemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 2: exec_on_colorList_itemDblClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 3: exec_on_colorChangeButton_click(); break;
        case 4: exec_on_formatList_selChange(); break;
        case 5: exec_on_format_setDefault(); break;
        case 6: exec_on_format_add(); break;
        case 7: exec_on_format_edit(); break;
        case 8: exec_on_format_delete(); break;
        case 9: browse_RasMol(); break;
        case 10: browse_VMD(); break;
        default: ;
        }
        _id -= 11;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
