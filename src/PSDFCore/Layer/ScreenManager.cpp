#include "StdAfx.h"

#include <PSDFCore/Layer/ScreenManager.h>
#include <PSDFCore/View/OsgViewerBase.h>

ScreenManager::ScreenManager( OsgViewerBase * parentViewer ) : _parentViewer( parentViewer )
{
	EventCenter::inst()->registerHandler(this, SCENE_GROUP, _parentViewer->getStationId());

	int width = 800;
	int height = 600;

	_hudCamra = new Camera;
	_hudCamra->setClearMask(GL_DEPTH_BUFFER_BIT);
	_hudCamra->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
	_hudCamra->setViewMatrix( osg::Matrix::identity() );
	_hudCamra->setProjectionMatrixAsOrtho2D( 0, width, 0, height );
	_hudCamra->setViewport( 0, 0, width, height );
	_hudCamra->setRenderOrder(Camera::POST_RENDER);
	_hudCamra->getOrCreateStateSet()->setMode(GL_LIGHTING, StateAttribute::OFF);
}

ScreenManager::~ScreenManager()
{
	EventCenter::inst()->unregisterHandler(this, SCENE_GROUP, _parentViewer->getStationId()); 
}

void ScreenManager::addScreenItem( Node* node, ScreenRestriction restriction, int dist1, int dist2 )
{
	ScreenItem item;
	MatrixTransform* trans = new MatrixTransform;
	trans->addChild(node);
	item.trans = trans;
	item.restiction = restriction;
	item.dist1 = dist1;
	item.dist2 = dist2;

	_items.push_back(item);

	_hudCamra->addChild(item.trans);

	putAway(item);
}

void ScreenManager::handleSystemEvent( unsigned eventType, int param1, int param2 )
{
	if (eventType != CAMERA_CHANGED)
	{
		return;
	}
	if (param1 == 1)
	{
		int viewWidth = _parentViewer->getMainCamera()->getViewport()->width();
		int viewHeight = _parentViewer->getMainCamera()->getViewport()->height();
		_hudCamra->setProjectionMatrixAsOrtho2D( 0, viewWidth, 0, viewHeight );
		_hudCamra->setViewport( 0, 0, viewWidth, viewHeight );
		for (unsigned i = 0; i < _items.size(); i++)
		{
			putAway(_items[i]);
		}
	}
}

void ScreenManager::putAway( const ScreenItem& item )
{
	BoundingSphere bound = item.trans->getChild(0)->getBound();
	int viewWidth = _parentViewer->getMainCamera()->getViewport()->width();
	int viewHeight = _parentViewer->getMainCamera()->getViewport()->height();

	Matrixd matTrans;

	switch (item.restiction)
	{
	case SCREEN_LEFT_TOP:
		matTrans.makeTranslate(item.dist1, viewHeight-item.dist2, 0);
		break;
	case SCREEN_LEFT_CENTER:
		matTrans.makeTranslate(item.dist1, viewHeight/2, 0);
		break;
	case SCREEN_LEFT_BOTTOM:
		matTrans.makeTranslate(item.dist1, item.dist2, 0);
		break;
	case SCREEN_CENTER_TOP:
		matTrans.makeTranslate(viewWidth/2, viewHeight-item.dist2, 0);
		break;
	case SCREEN_CENTER_CENTER:
		matTrans.makeTranslate(viewWidth/2, viewHeight/2, 0);
		break;
	case SCREEN_CENTER_BOTTOM:
		matTrans.makeTranslate(viewWidth/2, item.dist2, 0);
		break;
	case SCREEN_RIGHT_TOP:
		matTrans.makeTranslate(viewWidth-item.dist1, viewHeight-item.dist2, 0);
		break;
	case SCREEN_RIGHT_CENTER:
		matTrans.makeTranslate(viewWidth-item.dist1, viewHeight/2, 0);
		break;
	case SCREEN_RIGHT_BOTTOM:
		matTrans.makeTranslate(viewWidth-item.dist1, item.dist2, 0);
		break;
	}

	item.trans->setMatrix(matTrans);
}

