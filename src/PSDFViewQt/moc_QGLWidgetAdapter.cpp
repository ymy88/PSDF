/****************************************************************************
** Meta object code from reading C++ file 'QGLWidgetAdapter.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "Stdafx.h"
#include <PSDFViewQt/impl/QGLWidgetAdapter.h>

#if !defined(Q_MOC_OUTPUT_REVISION)
//#error "The header file 'QGLWidgetAdapter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ViewerUpdater[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ViewerUpdater[] = {
    "ViewerUpdater\0\0updateAllViewers()\0"
};

void ViewerUpdater::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ViewerUpdater *_t = static_cast<ViewerUpdater *>(_o);
        switch (_id) {
        case 0: _t->updateAllViewers(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ViewerUpdater::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ViewerUpdater::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ViewerUpdater,
      qt_meta_data_ViewerUpdater, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ViewerUpdater::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ViewerUpdater::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ViewerUpdater::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ViewerUpdater))
        return static_cast<void*>(const_cast< ViewerUpdater*>(this));
    return QObject::qt_metacast(_clname);
}

int ViewerUpdater::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_QGLWidgetAdapter[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: signature, parameters, type, tag, flags
      31,   18,   17,   17, 0x05,
      61,   55,   17,   17, 0x05,
      92,   55,   17,   17, 0x05,
     125,   55,   17,   17, 0x05,
     162,   55,   17,   17, 0x05,
     192,   17,   17,   17, 0x05,
     218,   55,   17,   17, 0x05,
     245,   55,   17,   17, 0x05,
     274,   55,   17,   17, 0x05,
     302,   55,   17,   17, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QGLWidgetAdapter[] = {
    "QGLWidgetAdapter\0\0width,height\0"
    "resizeGLSignal(int,int)\0event\0"
    "mousePressSignal(QMouseEvent*)\0"
    "mouseReleaseSignal(QMouseEvent*)\0"
    "mouseDoubleClickSignal(QMouseEvent*)\0"
    "mouseMoveSignal(QMouseEvent*)\0"
    "wheelSignal(QWheelEvent*)\0"
    "keyPressSignal(QKeyEvent*)\0"
    "keyReleaseSignal(QKeyEvent*)\0"
    "focusInSignal(QFocusEvent*)\0"
    "focusOutSignal(QFocusEvent*)\0"
};

void QGLWidgetAdapter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QGLWidgetAdapter *_t = static_cast<QGLWidgetAdapter *>(_o);
        switch (_id) {
        case 0: _t->resizeGLSignal((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->mousePressSignal((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 2: _t->mouseReleaseSignal((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 3: _t->mouseDoubleClickSignal((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 4: _t->mouseMoveSignal((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 5: _t->wheelSignal((*reinterpret_cast< QWheelEvent*(*)>(_a[1]))); break;
        case 6: _t->keyPressSignal((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        case 7: _t->keyReleaseSignal((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        case 8: _t->focusInSignal((*reinterpret_cast< QFocusEvent*(*)>(_a[1]))); break;
        case 9: _t->focusOutSignal((*reinterpret_cast< QFocusEvent*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QGLWidgetAdapter::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QGLWidgetAdapter::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_QGLWidgetAdapter,
      qt_meta_data_QGLWidgetAdapter, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QGLWidgetAdapter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QGLWidgetAdapter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QGLWidgetAdapter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QGLWidgetAdapter))
        return static_cast<void*>(const_cast< QGLWidgetAdapter*>(this));
    if (!strcmp(_clname, "OsgViewerBase"))
        return static_cast< OsgViewerBase*>(const_cast< QGLWidgetAdapter*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int QGLWidgetAdapter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void QGLWidgetAdapter::resizeGLSignal(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QGLWidgetAdapter::mousePressSignal(QMouseEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QGLWidgetAdapter::mouseReleaseSignal(QMouseEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QGLWidgetAdapter::mouseDoubleClickSignal(QMouseEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QGLWidgetAdapter::mouseMoveSignal(QMouseEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QGLWidgetAdapter::wheelSignal(QWheelEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QGLWidgetAdapter::keyPressSignal(QKeyEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QGLWidgetAdapter::keyReleaseSignal(QKeyEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void QGLWidgetAdapter::focusInSignal(QFocusEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void QGLWidgetAdapter::focusOutSignal(QFocusEvent * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_END_MOC_NAMESPACE
