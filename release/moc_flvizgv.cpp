/****************************************************************************
** Meta object code from reading C++ file 'flvizgv.h'
**
** Created: Tue 23. Jun 10:45:40 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../flvizgv.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'flvizgv.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FLVizGv[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_FLVizGv[] = {
    "FLVizGv\0"
};

const QMetaObject FLVizGv::staticMetaObject = {
    { &QGraphicsView::staticMetaObject, qt_meta_stringdata_FLVizGv,
      qt_meta_data_FLVizGv, 0 }
};

const QMetaObject *FLVizGv::metaObject() const
{
    return &staticMetaObject;
}

void *FLVizGv::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FLVizGv))
        return static_cast<void*>(const_cast< FLVizGv*>(this));
    return QGraphicsView::qt_metacast(_clname);
}

int FLVizGv::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
