#include "StdAfx.h"

#include <PSDFViewQt/impl/ViewerQt3D.h>

using namespace std;

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

ViewerQt3D::ViewerQt3D( QWidget * parent, DisplayMode displayMode, const string& viewType, bool needRoam):
QGLWidgetAdapter( parent, displayMode, viewType)
{
	StateSet* sceneDataState = _sceneData->getOrCreateStateSet();
	
	sceneDataState->setMode(GL_MULTISAMPLE, StateAttribute::ON);
	sceneDataState->setMode(GL_DEPTH_TEST, StateAttribute::ON);
	
	setMainCamera(new Camera3D(this, needRoam));
}
