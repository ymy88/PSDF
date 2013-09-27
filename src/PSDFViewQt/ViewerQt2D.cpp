#include "StdAfx.h"

#include <PSDFViewQt/impl/ViewerQt2D.h>
//#include <osgViewer/ViewerEventHandlers>

#include <fstream>
#include <ctime>
using namespace std;

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

//------------------------------------------
// ViewerQt2D
//------------------------------------------
ViewerQt2D::ViewerQt2D( QWidget * parent, DisplayMode displayMode, const string& viewType, bool needRoam):
	QGLWidgetAdapter( parent, displayMode, viewType )
{
	StateSet* state = _sceneData->getOrCreateStateSet();

	state->setMode( GL_DEPTH_TEST, StateAttribute::OFF );
	state->setMode( GL_LIGHTING, StateAttribute::OFF );
	//state->setMode( GL_MULTISAMPLE, StateAttribute::ON );

	setMainCamera(new Camera2D(this, needRoam));

	//addEventHandler(new osgViewer::StatsHandler);
}