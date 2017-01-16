/****************************************************************************
** Meta object code from reading C++ file 'mdtra_userTypeDialog.h'
**
** Created: Mon 16. Jan 19:04:10 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mdtra_userTypeDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mdtra_userTypeDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MDTRA_CodeEditor[] = {

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
      32,   18,   17,   17, 0x08,
      65,   63,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_CodeEditor[] = {
    "MDTRA_CodeEditor\0\0newBlockCount\0"
    "updateLineNumberAreaWidth(int)\0,\0"
    "updateLineNumberArea(QRect,int)\0"
};

const QMetaObject MDTRA_CodeEditor::staticMetaObject = {
    { &QPlainTextEdit::staticMetaObject, qt_meta_stringdata_MDTRA_CodeEditor,
      qt_meta_data_MDTRA_CodeEditor, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_CodeEditor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_CodeEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_CodeEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_CodeEditor))
        return static_cast<void*>(const_cast< MDTRA_CodeEditor*>(this));
    return QPlainTextEdit::qt_metacast(_clname);
}

int MDTRA_CodeEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPlainTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateLineNumberAreaWidth((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: updateLineNumberArea((*reinterpret_cast< const QRect(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_MDTRA_UserTypeDialog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x08,
      44,   21,   21,   21, 0x08,
      69,   21,   21,   21, 0x08,
      86,   21,   21,   21, 0x08,
     112,   21,   21,   21, 0x08,
     138,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MDTRA_UserTypeDialog[] = {
    "MDTRA_UserTypeDialog\0\0exec_on_check_input()\0"
    "exec_on_source_changed()\0exec_on_accept()\0"
    "exec_on_open_sourcefile()\0"
    "exec_on_save_sourcefile()\0exec_on_compile()\0"
};

const QMetaObject MDTRA_UserTypeDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MDTRA_UserTypeDialog,
      qt_meta_data_MDTRA_UserTypeDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MDTRA_UserTypeDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MDTRA_UserTypeDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MDTRA_UserTypeDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MDTRA_UserTypeDialog))
        return static_cast<void*>(const_cast< MDTRA_UserTypeDialog*>(this));
    if (!strcmp(_clname, "Ui_userTypeDialog"))
        return static_cast< Ui_userTypeDialog*>(const_cast< MDTRA_UserTypeDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MDTRA_UserTypeDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: exec_on_check_input(); break;
        case 1: exec_on_source_changed(); break;
        case 2: exec_on_accept(); break;
        case 3: exec_on_open_sourcefile(); break;
        case 4: exec_on_save_sourcefile(); break;
        case 5: exec_on_compile(); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
