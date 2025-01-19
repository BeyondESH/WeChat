/****************************************************************************
** Meta object code from reading C++ file 'registerdialog.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../registerdialog.h"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'registerdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSRegisterDialogENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSRegisterDialogENDCLASS = QtMocHelpers::stringData(
    "RegisterDialog",
    "backPBClicked",
    "",
    "on_backPushButton_clicked",
    "on_checkBox_checkStateChanged",
    "Qt::CheckState",
    "arg1",
    "on_checkBox_2_checkStateChanged",
    "on_getCodepushButton_clicked",
    "slot_mod_register_finished",
    "ReqId",
    "req_id",
    "res",
    "ErrorCodes",
    "ec",
    "on_submitPushButton_clicked",
    "on_accountLineEdit_textEdited",
    "on_accountLineEdit_editingFinished",
    "on_emailLineEdit_editingFinished",
    "on_emailLineEdit_textEdited",
    "on_passwordLineEdit_textEdited",
    "on_verifylineEdit_textEdited",
    "on_verifylineEdit_editingFinished",
    "on_passwordLineEdit_editingFinished",
    "on_codeLineEdit_editingFinished"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSRegisterDialogENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  110,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       3,    0,  111,    2, 0x08,    2 /* Private */,
       4,    1,  112,    2, 0x08,    3 /* Private */,
       7,    1,  115,    2, 0x08,    5 /* Private */,
       8,    0,  118,    2, 0x08,    7 /* Private */,
       9,    3,  119,    2, 0x08,    8 /* Private */,
      15,    0,  126,    2, 0x08,   12 /* Private */,
      16,    1,  127,    2, 0x08,   13 /* Private */,
      17,    0,  130,    2, 0x08,   15 /* Private */,
      18,    0,  131,    2, 0x08,   16 /* Private */,
      19,    1,  132,    2, 0x08,   17 /* Private */,
      20,    1,  135,    2, 0x08,   19 /* Private */,
      21,    1,  138,    2, 0x08,   21 /* Private */,
      22,    0,  141,    2, 0x08,   23 /* Private */,
      23,    0,  142,    2, 0x08,   24 /* Private */,
      24,    0,  143,    2, 0x08,   25 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10, QMetaType::QString, 0x80000000 | 13,   11,   12,   14,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject RegisterDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASSRegisterDialogENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSRegisterDialogENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSRegisterDialogENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<RegisterDialog, std::true_type>,
        // method 'backPBClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_backPushButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_checkBox_checkStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Qt::CheckState &, std::false_type>,
        // method 'on_checkBox_2_checkStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Qt::CheckState &, std::false_type>,
        // method 'on_getCodepushButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slot_mod_register_finished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<ReqId, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<ErrorCodes, std::false_type>,
        // method 'on_submitPushButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_accountLineEdit_textEdited'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'on_accountLineEdit_editingFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_emailLineEdit_editingFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_emailLineEdit_textEdited'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'on_passwordLineEdit_textEdited'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'on_verifylineEdit_textEdited'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'on_verifylineEdit_editingFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_passwordLineEdit_editingFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_codeLineEdit_editingFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void RegisterDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RegisterDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->backPBClicked(); break;
        case 1: _t->on_backPushButton_clicked(); break;
        case 2: _t->on_checkBox_checkStateChanged((*reinterpret_cast< std::add_pointer_t<Qt::CheckState>>(_a[1]))); break;
        case 3: _t->on_checkBox_2_checkStateChanged((*reinterpret_cast< std::add_pointer_t<Qt::CheckState>>(_a[1]))); break;
        case 4: _t->on_getCodepushButton_clicked(); break;
        case 5: _t->slot_mod_register_finished((*reinterpret_cast< std::add_pointer_t<ReqId>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<ErrorCodes>>(_a[3]))); break;
        case 6: _t->on_submitPushButton_clicked(); break;
        case 7: _t->on_accountLineEdit_textEdited((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->on_accountLineEdit_editingFinished(); break;
        case 9: _t->on_emailLineEdit_editingFinished(); break;
        case 10: _t->on_emailLineEdit_textEdited((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->on_passwordLineEdit_textEdited((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->on_verifylineEdit_textEdited((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->on_verifylineEdit_editingFinished(); break;
        case 14: _t->on_passwordLineEdit_editingFinished(); break;
        case 15: _t->on_codeLineEdit_editingFinished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RegisterDialog::*)();
            if (_t _q_method = &RegisterDialog::backPBClicked; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *RegisterDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RegisterDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSRegisterDialogENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int RegisterDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void RegisterDialog::backPBClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
