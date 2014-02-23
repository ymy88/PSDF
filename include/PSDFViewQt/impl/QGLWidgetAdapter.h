#pragma once

#include "Common.h"

using Qt::WindowFlags;


class PSDF_VIEWQT_DLL_DECL ViewerUpdater : public QObject
{
	Q_OBJECT

public slots:
	void updateAllViewers();
};

class PSDF_VIEWQT_DLL_DECL QGLWidgetAdapter : public QGLWidget, public OsgViewerBase
{
	Q_OBJECT

public:
    QGLWidgetAdapter( QWidget * parent, DisplayMode displayMode, const string& viewType);
    virtual ~QGLWidgetAdapter() {}

    osgViewer::GraphicsWindow *			getGraphicsWindow() { return _gw.get(); }

	virtual void						paintGL();

signals:
	void								resizeGLSignal( int width, int height );
	void								mousePressSignal( QMouseEvent *event );
	void								mouseReleaseSignal( QMouseEvent *event );
	void								mouseDoubleClickSignal( QMouseEvent *event );
	void								mouseMoveSignal( QMouseEvent *event );
	void								wheelSignal( QWheelEvent *);
	void								keyPressSignal( QKeyEvent *event );
	void								keyReleaseSignal( QKeyEvent *event );
	void								focusInSignal( QFocusEvent *event );
	void								focusOutSignal( QFocusEvent *event );

protected:
	virtual void						resizeGL( int width, int height );
	virtual void						mousePressEvent( QMouseEvent *event );
	virtual void						mouseReleaseEvent( QMouseEvent *event );
	virtual void						mouseDoubleClickEvent( QMouseEvent *event );
	virtual void						mouseMoveEvent( QMouseEvent *event );
	virtual void						wheelEvent( QWheelEvent *);
	virtual void						keyPressEvent( QKeyEvent *event );
	virtual void						keyReleaseEvent( QKeyEvent *event );
	virtual void						focusInEvent( QFocusEvent *event );
	virtual void						focusOutEvent( QFocusEvent *event );

protected:
    osg::ref_ptr< osgViewer::GraphicsWindowEmbedded >	_gw;

	StatsHandler*						_statsHandler;
};


//------------------------
// Global function
//------------------------
void PSDF_VIEWQT_DLL_DECL setGlobalTimer(int millisecond);
void PSDF_VIEWQT_DLL_DECL allViewAdjustCamera();