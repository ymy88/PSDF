#include "StdAfx.h"

#include <PSDFCore/Pick/Pickable.h>
#include <PSDFCore/Pick/PickableManager.h>
#include <PSDFCore/View/OsgViewerBase.h>

PickableManager::PickableManager( OsgViewerBase * parentViewer ) 	
{
	_parentViewer = parentViewer;
	_beginDrag = false;
	_dragging = false;
	_pickEnabled = true;
	_currPicked = NULL;
	
	EventCenter::inst()->registerHandler(this, MOUSE_CLICK_GROUP | MOUSE_MOTION_GROUP, _parentViewer->getStationId() );
}

PickableManager::~PickableManager()
{
	EventCenter::inst()->unregisterHandler(this, EVENT_GROUP_ALL, _parentViewer->getStationId());
}

void PickableManager::handleSystemEvent( unsigned eventType, int param1, int param2 )
{
	short mouseScrX, mouseScrY;
	if (SceneCenter::inst()->getMouseMode() != MOUSE_MODE_ROAM &&
		SceneCenter::inst()->getMouseMode() != MOUSE_MODE_DRAG)
	{
		return;
	}
	
	switch( eventType )
	{
	case LBUTTON_DOWN:		
		getFromParam( param1, mouseScrX, mouseScrY );
		onLButtonDown( mouseScrX, mouseScrY );
		break;
		
	case LBUTTON_UP:		
		getFromParam( param1, mouseScrX, mouseScrY );
		onLButtonUp( mouseScrX, mouseScrY );
		break;	

	case LBUTTON_DOUBLECLICK:
		getFromParam(param1, mouseScrX, mouseScrY);
		onLButtonDoubleClick(mouseScrX, mouseScrY);
		break;

	case RBUTTON_DOWN:
		_beginDrag = false;
		if( _currPicked )
		{
			_currPicked = NULL;
			SceneCenter::inst()->setMouseMode(MOUSE_MODE_ROAM);
		}

		break;

	case MOUSE_MOVE:
		if( _beginDrag == true )
		{
			short endScrX, endScrY;
			getFromParam( param1, endScrX, endScrY );
			if (_dragging == false)
			{
				preDrag(endScrX, endScrY);
				_dragging = true;
			}
			else
			{
				onDrag( endScrX, endScrY );
			}
		}
		break;

	default:
		break;
	}
}

void PickableManager::onLButtonDown( short mouseScrX, short mouseScrY )
{
	if( _currPicked )
	{
		_currPicked = NULL;
		SceneCenter::inst()->setMouseMode(MOUSE_MODE_ROAM);
	}

	for( unsigned int i = 0; i < _pickables.size(); ++i )
	{
		Node* pickablcNode = dynamic_cast<Node*>(_pickables[i]);
		if( pickablcNode && pickablcNode->getNodeMask() == 0 ) { continue; }

		if( _pickables[i]->isPicked(mouseScrX, mouseScrY) )
		{
			_currPicked = _pickables[ i ];
			_beginDrag = true;
			break;
		}
	}
}

void PickableManager::onLButtonUp( short mouseScrX, short mouseScrY )
{
	if( _currPicked == NULL ) { return; }

	/* Dragging just happened. */
	if( _dragging )
	{
		_currPicked = NULL;
		_beginDrag = false;
		_dragging = false;
		
		SceneCenter::inst()->setMouseMode(MOUSE_MODE_ROAM);

		return;
	}

	/* Dragging not happened, just pick. */
	if( _currPicked )
	{
		_currPicked->onPick(mouseScrX, mouseScrY);
	}
}

void PickableManager::preDrag( short scrX, short scrY )
{
	if( _currPicked == NULL ) { return; }

	SceneCenter::inst()->setMouseMode(MOUSE_MODE_DRAG);
	_currPicked->preDrag( scrX, scrY );
}

void PickableManager::onDrag( short endScrX, short endScrY )
{
	if( _currPicked == NULL ) { return; }

	if (SceneCenter::inst()->getMouseMode() == MOUSE_MODE_DRAG)
	{
		_currPicked->drag( endScrX, endScrY );
	}
}

void PickableManager::registerPickable( Pickable * pickable )
{
	_registerMutex.lock();
	_pickables.push_back( pickable );
	_registerMutex.unlock();
}

void PickableManager::unregisterPickable( Pickable * pickable )
{
	_registerMutex.lock();

	vector< Pickable * >::iterator iter = _pickables.begin();

	for( ; iter != _pickables.end(); ++iter )
	{
		if( ( *iter ) == pickable )
		{
			_pickables.erase( iter );
            if (_currPicked == pickable)
            {
                _currPicked = NULL;
                _beginDrag = false;
                _dragging = false;
                SceneCenter::inst()->setMouseMode(MOUSE_MODE_ROAM);
            }
			_registerMutex.unlock();
			return;
		}
	}
	_registerMutex.unlock();
}

void PickableManager::onLButtonDoubleClick( short mouseScrX, short mouseScrY )
{
	if( _currPicked )
	{
		_currPicked = NULL;
		SceneCenter::inst()->setMouseMode(MOUSE_MODE_ROAM);
	}

	for( unsigned int i = 0; i < _pickables.size(); ++i )
	{
		Node* pickablcNode = dynamic_cast<Node*>(_pickables[i]);
		if( pickablcNode && pickablcNode->getNodeMask() == 0 ) { continue; }

		if( _pickables[i]->isPicked(mouseScrX, mouseScrY) )
		{
			_currPicked = _pickables[i];
			_currPicked->onLDClick(mouseScrX, mouseScrY);
			break;
		}
	}
}
