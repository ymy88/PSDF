#include "StdAfx.h"

#include <PSDFViewQt/impl/QGLWidgetAdapter.h>

//------------------------------------------------
// ViewerUpdater
//------------------------------------------------
void ViewerUpdater::updateAllViewers()
{
	const vector<OsgViewerBase*>& allViewers = OsgViewerBase::getAllViewers();

	for (unsigned int i = 0; i < allViewers.size(); i++)
	{
		QGLWidgetAdapter* view = static_cast<QGLWidgetAdapter*>(allViewers[i]);
		view->updateGL();
	}
}



//--------------------------------------------------
// QGLWidgetAdapter
//--------------------------------------------------

QGLWidgetAdapter::QGLWidgetAdapter( QWidget * parent, DisplayMode displayMode, const string& viewType):
	QGLWidget(parent), OsgViewerBase(displayMode, viewType)
{
	_gw = new osgViewer::GraphicsWindowEmbedded(0, 0, width(), height());

	_gw->setDataVariance( Object::DYNAMIC );
    
	setFocusPolicy( Qt::ClickFocus );
}

void QGLWidgetAdapter::resizeGL( int width, int height )
{
    _gw->resized( 0, 0, width, height );
	if (_mainCamera)
	{
		_mainCamera->resize( width, height );
	}
	emit resizeGLSignal(width, height);
}

void QGLWidgetAdapter::mousePressEvent( QMouseEvent* event )
{
	int mod = event->modifiers();

	unsigned eventType = 0;
	
	switch( event->button() )
	{
	case( Qt::LeftButton ):
		eventType = LBUTTON_DOWN;
		break;
	case( Qt::MidButton ):
		eventType = MBUTTON_DOWN;
		break;
	case( Qt::RightButton ):
		eventType = RBUTTON_DOWN;
		break;
	default:
		return;
	}

	/* 发送系统消息 */
	EventCenter::inst()->triggerEvent( eventType, createParam( event->x(), event->y() ), mod, getStationId() );

	emit mousePressSignal(event);
}

void QGLWidgetAdapter::mouseReleaseEvent( QMouseEvent *event )
{
	int mod = event->modifiers();

	unsigned eventType = 0;
	
	switch( event->button() )
	{
	case( Qt::LeftButton ):
		eventType = LBUTTON_UP;
		break;
	case( Qt::MidButton ):
		eventType = MBUTTON_UP;
		break;
	case( Qt::RightButton ):
		eventType = RBUTTON_UP;
		break;
	default:
		return;
	}

	/* 发送系统消息 */
	EventCenter::inst()->triggerEvent( eventType, createParam( event->x(), event->y() ), mod, getStationId() );

	emit mouseReleaseSignal(event);
}

void QGLWidgetAdapter::mouseDoubleClickEvent( QMouseEvent *event )
{
	int mod = event->modifiers();

	unsigned eventType = 0;
	
	switch( event->button() )
	{
	case( Qt::LeftButton ):
		eventType = LBUTTON_DOUBLECLICK;
		break;	
	case( Qt::RightButton ):
		eventType = RBUTTON_DOUBLECLICK;
		break;
	default:
		return;
	}

	/* 发送系统消息 */
	EventCenter::inst()->triggerEvent( eventType, createParam( event->x(), event->y() ), mod, getStationId() );

	emit mouseDoubleClickSignal(event);
}

void QGLWidgetAdapter::mouseMoveEvent( QMouseEvent *event )
{
	int mod = event->modifiers();

	unsigned eventType = MOUSE_MOVE;

	/* 发送系统消息 */
	EventCenter::inst()->triggerEvent( eventType, createParam( event->x(), event->y() ), mod, getStationId() );

	emit mouseMoveSignal(event);
}

void QGLWidgetAdapter::wheelEvent( QWheelEvent *event )
{
	if( event->orientation() == Qt::Horizontal )
	{
		return;
	}
	
	int mod = event->modifiers();
	int eventType;
	int delta;
	if( event->delta() > 0 )
	{
		delta = event->delta();

		eventType = SCROLL_UP;
	}
	else
	{
		delta = -event->delta();

		eventType = SCROLL_DOWN;
	}

	/* 发送系统消息 */
	EventCenter::inst()->triggerEvent( eventType, delta, mod, getStationId() );	

	emit wheelSignal(event);
}

void QGLWidgetAdapter::keyPressEvent( QKeyEvent* event )
{
	if( event->key() == 0 )
	{
		return;
	}

	int mod = event->modifiers();

	/* 发送系统消息 */
	EventCenter::inst()->triggerEvent( KEY_DOWN, event->key(), mod, getStationId() );

	emit keyPressSignal(event);
}

void QGLWidgetAdapter::keyReleaseEvent( QKeyEvent *event )
{
	if( event->key() == 0 )
	{
		return;
	}

	int mod = event->modifiers();
	
	/* 发送系统消息 */
	EventCenter::inst()->triggerEvent( KEY_UP, event->key(), mod, getStationId() );

	emit keyReleaseSignal(event);
}

void QGLWidgetAdapter::focusInEvent( QFocusEvent *event )
{
	this->setFocused();

	/* 发送系统消息 */
	EventCenter::inst()->triggerEvent( FOCUS_IN, 0, 0, getStationId() );

	emit focusInSignal(event);
}

void QGLWidgetAdapter::focusOutEvent( QFocusEvent *event )
{
	/* 发送系统消息 */
	EventCenter::inst()->triggerEvent( FOCUS_OUT, 0, 0, getStationId() );

	emit focusOutSignal(event);
}

void QGLWidgetAdapter::paintGL()
{
	currFrameAdd();	
	_eventStation->manuallyDispatchEvent();
	frame();
}



//---------------------------------------
// Global function
//---------------------------------------
void setGlobalTimer(int millisecond)
{
	static bool alreadySet = false;
	static QTimer timer;
	static ViewerUpdater viewerUpdater;

	if (!alreadySet)
	{
		QObject::connect( &timer, SIGNAL( timeout() ), &viewerUpdater, SLOT( updateAllViewers() ) );
		timer.start(millisecond);
		alreadySet = true;
	}
	else
	{
		timer.stop();
		timer.start(millisecond);
	}
}

void allViewAdjustCamera()
{
	const vector<OsgViewerBase*>& allViewers = OsgViewerBase::getAllViewers();
	for (unsigned int i = 0; i < allViewers.size(); ++i)
	{
		QGLWidgetAdapter* w = static_cast<QGLWidgetAdapter*>(allViewers[i]);
		allViewers[i]->adjustCamera(w->width(), w->height());
	}
}
