#pragma once

#include "../common/Common.h"
#include "../View/OsgViewerBase.h"

#include "PickableManager.h"

class Pickable
{
public:
	Pickable() : _lastScrX(0), _lastScrY(0), _parentViewer(NULL) {}
	virtual ~Pickable() { if (_parentViewer) { _parentViewer->getPickableManager()->unregisterPickable(this); } }

	void enablePickAndDragInViewer(OsgViewerBase* viewer)
	{
		if (_parentViewer)
		{
			_parentViewer->getPickableManager()->unregisterPickable(this);
		}
		_parentViewer = viewer;
		_parentViewer->getPickableManager()->registerPickable(this);
	}

	virtual bool	isPicked( int mouseScrX, int mouseScrY ) = 0;

	void			pick(int mouseScrX, int mouseScrY)		{ onPick(mouseScrX, mouseScrY); }
	void			preDrag(int mouseScrX, int mouseScrY)	{ _lastScrX = mouseScrX; _lastScrY = mouseScrY; onPreDrag(); }
	void			drag(int mouseScrX, int mouseScrY)		{ onDrag(mouseScrX, mouseScrY); _lastScrX = mouseScrX; _lastScrY = mouseScrY; }


	// actions
	virtual void	onPick(int mouseScrX, int mouseScrY)	{}
	virtual void	onCancelPick()							{}
	virtual void	onPreDrag()								{}
	virtual void	onDrag(int mouseScrX, int mouseScrY)	{ TOUCH( mouseScrX ); TOUCH( mouseScrY ); }

protected:
	void			getLastScrCoord(int& x, int& y)			{ x = _lastScrX; y = _lastScrY; }
	OsgViewerBase*	getParentViewer()						{ return _parentViewer; }

private:
	int _lastScrX;
	int _lastScrY;

	OsgViewerBase*	_parentViewer;
};
