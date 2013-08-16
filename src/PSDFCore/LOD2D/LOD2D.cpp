#include "StdAfx.h"

#include <PSDFCore/LOD2D/LOD2D.h>

#include <PSDFCore/View/OsgViewerBase.h>
#include <PSDFCore/View/Camera2D.h>


LOD2D::LOD2D( OsgViewerBase * parentViewer )
{
	_parentViewer = parentViewer;
	
	_minCameraRange = 0;

	_maxCameraRange = 1e10;

	this->setName("LOD2D");

	EventCenter::inst()->registerHandler( this, SCENE_GROUP, _parentViewer->getStationId() );
}

LOD2D::~LOD2D()
{
	EventCenter::inst()->unregisterHandler(this, SCENE_GROUP, _parentViewer->getStationId());
}

void LOD2D::handleSystemEvent( unsigned eventType, int param1, int param2 )
{
	if( eventType != CAMERA_CHANGED )
	{
		return;
	}

	short posChanged, scaleChanged;
	getFromParam(param2, posChanged, scaleChanged);
	if (!scaleChanged)
	{
		return;
	}

	Camera2D* camera = dynamic_cast<Camera2D*>(_parentViewer->getMainCamera());
	if (camera)
	{
		double cameraRange = camera->getViewportXRangeInWorld();
		if( _minCameraRange <= cameraRange && cameraRange <= _maxCameraRange )
		{
			setNodeMask( 0xFFFFFFFF );
		}
		else
		{
			setNodeMask( 0x00000000 );
		}
	}
}