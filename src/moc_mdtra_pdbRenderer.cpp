/****************************************************************************
** Meta object code from reading C++ file 'mdtra_pdbRenderer.h'
**
** Created: Thu 25. Feb 14:08:21 2016
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdtra_pdbRenderer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdtra_pdbRenderer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MDTRA_PDB_Renderer[] = {

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
      20,   19,   19,   19, 0x08,
      40,   19,   19,   19, 0x08,
      68,   59,   19,   19, 0x08,
      92,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_PDB_Renderer[] = {
    "MDTRA_PDB_Renderer\0\0on_start_playback()\0"
    "on_stop_playback()\0snapshot\0"
    "on_change_snapshot(int)\0"
    "on_selection_editingFinished()\0"
};

const QMetaObject MDTRA_PDB_Renderer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MDTRA_PDB_Renderer,
      qt_meta_data_MDTRA_PDB_Renderer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_PDB_Renderer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_PDB_Renderer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_PDB_Renderer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_PDB_Renderer))
        return static_cast<void*>(const_cast< MDTRA_PDB_Renderer*>(this));
    return QWidget::qt_metacast(_clname);
}

int MDTRA_PDB_Renderer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_start_playback(); break;
        case 1: on_stop_playback(); break;
        case 2: on_change_snapshot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: on_selection_editingFinished(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
