#include "StdAfx.h"

#include <PSDFCore/View/OsgViewerBase.h>
#include <PSDFCore/View/Camera2D.h>
#include <PSDFCore/View/StaticNodeGroup.h>
#include <PSDFCore/Plugin/Plugin.h>

//------------------------------------------
// OsgViewerBase
//------------------------------------------

vector< OsgViewerBase * >	OsgViewerBase::_allViewers;
OsgViewerBase *				OsgViewerBase::_focusedViewer;

OsgViewerBase::OsgViewerBase(DisplayMode displayMode, const string& viewType)
{
	_currFrame = 0;

	_mainCamera = NULL;

	_displayMode = displayMode;
	_viewType = viewType;

	/* 将本视窗添加到全局视窗容器中 */
	_allViewers.push_back( this );
	
	_focusedViewer = this;

	_eventStation = new EventStation();

	/* 将视窗的消息站添加到消息中心的消息站容器中 */
	EventCenter::inst()->registerStation( _eventStation );

	_layerManager = new LayerManager( this );
	_screenManager = new ScreenManager( this );
	_pickableManager = new PickableManager( this );


	/* scene */
	Group* dynamicRoot = new Group;
	Group* staticRoot = new Group;
	dynamicRoot->setName("Dynamic root");
	staticRoot->setName("Static root");

	_sceneData = new Group;
	_sceneData->setName("Scene Data");

	if (displayMode == MODE_2D)
	{
		/* 创建静态场景根节点 */
		ref_ptr< StaticNodeGroup > staticNodeGroup = new StaticNodeGroup( this );
		staticNodeGroup->addChild( staticRoot );	
		_sceneData->addChild( staticNodeGroup.get() );
	}
	else
	{
		_sceneData->addChild(staticRoot);
	}
	_sceneData->addChild( dynamicRoot );

	_layerManager->setRoot( staticRoot, dynamicRoot );

	/* screen */

	Group* screenRoot = new Group;
	screenRoot->setName("Screen root");
	_screenManager->setRoot(screenRoot);
	_sceneData->addChild(screenRoot);

    setThreadingModel( osgViewer::Viewer::SingleThreaded );

	//_statsHandler = new StatsHandler(this);


	/* light */

	StateSet* sceneDataState = _sceneData->getOrCreateStateSet();

	Group* lightGroup = new Group;
	lightGroup->setName("Light group");

	_sunLight = new osg::Light;
	_sunLight->setLightNum(0);
	_sunLight->setPosition(osg::Vec4(1.0f, 0.0f, -1.0f, 0.0f));
	_sunLight->setAmbient(osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f));
	_sunLight->setDiffuse(osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f));

	osg::LightSource* lightS = new osg::LightSource;    
	lightS->setLight(_sunLight);
	lightS->setStateSetModes(*sceneDataState, osg::StateAttribute::ON);

	LightModel* lightModel = new osg::LightModel;
	lightModel->setAmbientIntensity(osg::Vec4(0.0f,0.0f,0.0f,1.0f));
	lightS->getOrCreateStateSet()->setAttribute(lightModel, StateAttribute::ON);

	lightGroup->addChild(lightS);
	_sceneData->addChild(lightGroup);

	sceneDataState->setMode( GL_LIGHTING, StateAttribute::ON );
}

OsgViewerBase::~OsgViewerBase()
{
	vector< OsgViewerBase * >::iterator	iter;

	for( iter = _allViewers.begin(); iter != _allViewers.end(); ++iter )
	{
		if( ( *iter ) == this )
		{
			_allViewers.erase( iter );
			break;
		}
	}

	delete _layerManager;
	delete _pickableManager;
	delete _screenManager;
	//delete _statsHandler;
}

long64 OsgViewerBase::currFrame() const
{
	return _currFrame;
}

void OsgViewerBase::currFrameAdd()
{
	++_currFrame;
}

unsigned int OsgViewerBase::getStationId() const
{
	return _eventStation->getStationId();
}

void OsgViewerBase::setDisplayMode(DisplayMode mode)
{
	_displayMode = mode;
}

void OsgViewerBase::setViewType(const string& discription)
{
	_viewType = discription;
}

DisplayMode OsgViewerBase::getDisplayMode() const
{
	return _displayMode;
}

string OsgViewerBase::getViewType() const
{
	return _viewType;
}

const vector< OsgViewerBase * >& OsgViewerBase::getAllViewers()
{
	return _allViewers;
}

const OsgViewerBase * OsgViewerBase::getFocusedViewer()
{
	return _focusedViewer;
}

const OsgViewerBase * OsgViewerBase::getViewerWithStationId(unsigned int stationId)
{
	for (unsigned int i = 0; i < _allViewers.size(); ++i)
	{
		if (_allViewers[i]->getStationId() == stationId)
		{
			return _allViewers[i];
		}
	}

	return NULL;
}

LayerManager * OsgViewerBase::getLayerManager() const
{
	return _layerManager;
}

ScreenManager * OsgViewerBase::getScreenManager() const
{
	return _screenManager;
}

PickableManager * OsgViewerBase::getPickableManager() const
{
	return _pickableManager;
}

void OsgViewerBase::setFocused()
{
	_focusedViewer = this;

	EventCenter::inst()->setFocusedStation( _eventStation );
}

CameraBase * OsgViewerBase::getMainCamera() const
{
	return _mainCamera;
}

void OsgViewerBase::setMainCamera(CameraBase* camera)
{
	_mainCamera = camera;
	_mainCamera->setGraphicsContext( getGraphicsWindow() );
	this->setCamera(_mainCamera);

	/* scene data必须在设置完相机后设置 */
	this->setSceneData(_sceneData);
}

void OsgViewerBase::adjustCamera(int viewWidth, int viewHeight)
{
	if (_mainCamera)
	{
		_mainCamera->adjustCamera(viewWidth, viewHeight);
	}
}

void OsgViewerBase::setSunLightPosition( const Vec3d& position )
{
	_sunLight->setPosition(Vec4(position.x(), position.y(), position.z(), 0.0f));
}

void OsgViewerBase::setSunLightValue( const Vec4d& ambient, const Vec4d& diffuse, const Vec4d& specular )
{
	_sunLight->setAmbient(ambient);
	_sunLight->setDiffuse(diffuse);
	_sunLight->setSpecular(specular);
}
