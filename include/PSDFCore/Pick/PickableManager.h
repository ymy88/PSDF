#pragma once

#include "../common/Common.h"

#include "../Event/EventHandler.h"

class Pickable;

class OsgViewerBase;

/*
 * Click left button to pick item.
 * Click left button at blank place to cancel pick.
 * Click right button to cancel pick.
 * NOTICE: item is NOT considered picked (i.e. IPlugin::onPick() will not be called) if drag starts.
 */
class PSDF_CORE_DLL_DECL PickableManager : public EventHandler
{
public:
	PickableManager( OsgViewerBase * parentViewer );
	~PickableManager();

private:
	PickableManager( const PickableManager & ) : EventHandler() {}

public:
	const Pickable*					getCurrPicked() { return _currPicked; }
	void							registerPickable( Pickable * pickable );
	void							unregisterPickable( Pickable * pickable );
	
protected:
	virtual void					handleSystemEvent( unsigned int eventType, int param1, int param2 );

private:
	void							onLButtonDown( short mouseScrX, short mouseScrY );
	void							onLButtonUp( short mouseScrX, short mouseScrY );
	void							preDrag( short scrX, short scrY );
	void							onDrag( short endScrX, short endScrY );

private:
	OsgViewerBase *					_parentViewer;		//拾取管理者所属父视窗口	

	vector< Pickable * >			_pickables;
	Pickable*						_currPicked;
	bool							_pickEnabled;

	bool							_beginDrag;
	bool							_dragging;

	PosixMutex						_registerMutex;
};