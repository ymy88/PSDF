#include "StdAfx.h"

#include <PSDFCore/Event/EventStation.h>
#include <iostream>
using namespace std;


//---------------------------------------------------
// EventStationDispatcher
//---------------------------------------------------
void EventStation::EventStationDispatcher::run()
{
	unsigned int eventType = 0;
	int param1 = 1, param2 = 0;

	for(;;)
	{
		peekEvent( eventType, param1, param2, true );
		dispatchEvent( eventType, param1, param2 );

		if( eventType == APP_QUIT ) { return; }
	}
}

void EventStation::EventStationDispatcher::manuallyRun()
{
	unsigned int eventType = 0;
	int param1 = 1, param2 = 0;

	dispatchEvent( FRAME_BEGIN, 0, 0 );

	for( ;; )
	{
		if( !peekEvent( eventType, param1, param2, false ) )
		{
			/* if queue is empty, quit */
			break;
		}

		if( eventType == APP_QUIT )
		{
			return;
		}

		dispatchEvent( eventType, param1, param2 );

		PosixThread::msleep( 1 );
	}

}

bool EventStation::EventStationDispatcher::peekEvent( unsigned int &eventType, int &param1, int &param2, bool shouldWait )
{
	if( shouldWait )
	{
		_station->_usedSem.acquire();
	}
	else if( !_station->_usedSem.tryAcquire() )
	{
		return false;
	}

	_station->_eventQueueMutex.lock();
	
	EventStationStruct& e = _station->_stationEventQueue[ _station->_getPos ];
	_station->_getPos = ( _station->_getPos == EVENT_STATION_QUEUE_LENGTH - 1 ) ? 0 : _station->_getPos + 1;
	
	_station->_eventQueueMutex.unlock();
	_station->_freeSem.release();
	
	eventType	= e.eventType;
	param1		= e.param1;
	param2		= e.param2;

	return true;
}

void EventStation::EventStationDispatcher::dispatchEvent( unsigned eventType, int param1, int param2 )
{
	if( getEventGroup( eventType ) == 0 )
	{
		return;
	}

	/* find the corresponding handler queue */
	int n;

	for( n = 0; n < EVENT_GROUP_COUNT; n++ )
	{
		if( ( ( EVENT_GROUP_BASE << n ) & eventType ) != 0 )
		{
			break;
		}
	}

	const vector< EventHandler*> &handlers = _station->_eventHandlerQueues[n].beginUpdate();

	/* 
	 * 在本次消息处理过程中新加入的handler不接收正在处理的消息
	 * 故使用：for( int i = 0; i < size; i++ )
	 * 而非：for( int i = 0; i < handlers.size(); i++ )
	  */
	int size = handlers.size();
	for( int i = 0; i < size; i++ )
	{
		EventHandler * handler = handlers[i];

		if(handler == NULL)
		{
			continue;
		}

		handler->handle( eventType, param1, param2, _station->getStationId() );

		if (eventType & SYSTEM_EVENT_GROUP)
		{
			_station->_handleSystemCount++;
		}
		else
		{
			_station->_handleCustomCount++;
		}
	}

	_station->_eventHandlerQueues[n].endUpdate();
}



//---------------------------------
// EventHandlerQueue
//---------------------------------
const vector< EventHandler * >& EventStation::EventHandlerQueue::beginUpdate()
{
	if( _pendingHandlers.size() == 0 )
	{
		_handlersLock.lockForRead();
		return _handlers;
	}

	_handlersLock.lockForWrite();

	solvePending();

	_handlersLock.unlockForWrite();

	_handlersLock.lockForRead();
	
	return _handlers;
}

void EventStation::EventHandlerQueue::endUpdate()
{
	_handlersLock.unlockForRead();
}

void EventStation::EventHandlerQueue::addHandler( EventHandler * handler )
{
	_pendingMutex.lock();

	if( _pendingHandlers.size() == 0 )
	{
		_pendingHandlers.push_back( handler );

		_pendingMutex.unlock();

		return;
	}

	vector< EventHandler * >::iterator it = _pendingHandlers.begin();
	bool inserted = false;
	for(; it != _pendingHandlers.end(); ++it )
	{
		if( handler->getPriority() >= ( *it )->getPriority() )
		{
			_pendingHandlers.insert( it, handler );
			inserted = true;
			break;
		}
	}
	if (!inserted)
	{
		_pendingHandlers.push_back(handler);
	}
	_pendingMutex.unlock();
}

void EventStation::EventHandlerQueue::removeHandler( EventHandler* handler )
{
	vector< EventHandler * >::iterator it = _handlers.begin();

	for(; it != _handlers.end(); ++it )
	{
		if( handler == ( *it ) )
		{
			/* 
				We don't use erase() because the erase()
				function will literaly remove an element
				from the vector. Thus in the dispachEvent()
				function, if a handler wants to unregister
				itself, the next handler will be skipped
				during the dispatch process because of the
				erase() function.
			 */
			( *it ) = NULL;
			break;
		}
	}

	vector< EventHandler * >::iterator it2 = _pendingHandlers.begin();

	for(; it2 != _pendingHandlers.end(); )
	{
		if( handler == ( *it2 ) )
		{
			it2 = _pendingHandlers.erase(it2);

			/* there may exist duplicated pending handlers */
		}
		else
		{
			++it2;
		}
	}
}

void EventStation::EventHandlerQueue::solvePending()
{
	vector< EventHandler * >::iterator it = _handlers.begin();
	
	for(; it != _handlers.end(); ++it )
	{
		if( ( *it ) == NULL )
		{
			it = _handlers.erase( it );
		}
	}

	vector< EventHandler * >::iterator	it1, it2;

	it1 = _handlers.begin();

	it2 = _pendingHandlers.begin();
	
	while( it1 != _handlers.end() && it2 != _pendingHandlers.end() )
	{
		EventHandler * h1 = ( * it1 );
		EventHandler * h2 = ( * it2 );
		if( h2 == h1 )
		{
			++it2;
			continue;
		}
		if( h2->getPriority() >= h1->getPriority() )
		{
			it1 = _handlers.insert( it1, h2 );
			++it1;
			++it2;
		}
		else
		{
			++it1;
		}
	}
	if( it2 != _pendingHandlers.end() )
	{
		_handlers.insert( _handlers.end(), it2, _pendingHandlers.end() );
	}
	_pendingHandlers.clear();
}




//--------------------------------------
// EventStation
//--------------------------------------

unsigned int EventStation::_nextStationId = EVENT_STATION_ID_NON_VIEW << 1;

EventStation::EventStation(int stationId) : _freeSem( EVENT_STATION_QUEUE_LENGTH ), _usedSem( 0 ), _putPos( 0 ), _getPos( 0 )
{
	if (stationId == -1)
	{
		_stationId = _nextStationId;
	}
	else
	{
		_stationId = stationId;
	}

	_dispatcher._station = this;

	_nextStationId += 1;


	/* 
	 * If the event station belongs to a view,
	 *		then the dispatch process is activated manually before every frame, in main thread of the view.
	 * Else if the event station doesn't belong to any view, 
	 *		then the dispatch process is automatically activated every few millisecond, in separate threads.
	  */
	
	/* 非视窗的消息站启动消息投递分发线程 */
	if( _stationId == EVENT_STATION_ID_NON_VIEW )
	{
		_dispatcher.start();
	}

	_triggerCount = 0;
	_handleSystemCount = 0;
	_handleCustomCount = 0;
}

EventStation::~EventStation()
{
	if (_stationId == EVENT_STATION_ID_NON_VIEW)
	{
		_dispatcher.waitUntilDone();
	}
}

/* 二级消息中心(消息站)将消息处理者添加到消息处理者队列 */
void EventStation::registerHandler( EventHandler * handler, unsigned eventGroupMask )
{
	for( short i = 0; i < EVENT_GROUP_COUNT; i++ )
	{
		if ( ( ( EVENT_GROUP_BASE << i ) & eventGroupMask ) == 0 )
		{
			continue;
		}		

		_eventHandlerQueues[ i ].addHandler( handler );
	}
}

void EventStation::unregisterHandler( EventHandler * handler, unsigned eventGroupMask )
{
	for ( short i = 0; i < EVENT_GROUP_COUNT; i++ )
	{
		if ( ( ( EVENT_GROUP_BASE << i ) & eventGroupMask ) == 0 )
		{
			continue;
		}

		_eventHandlerQueues[ i ].removeHandler( handler );
	}
}

void EventStation::triggerEvent( unsigned eventType, int param1, int param2, bool shouldWait )
{
	if (shouldWait)
	{
		_freeSem.acquire();
	}
	else
	{
		if (!_freeSem.tryAcquire()){ return; }
	}

	EventStationStruct& e = _stationEventQueue[ _putPos ];
	
	_putPos = ( _putPos == EVENT_STATION_QUEUE_LENGTH - 1 ) ? 0 : _putPos + 1;
	
	e.eventType		= eventType;
	e.param1		= param1;
	e.param2		= param2;

	_usedSem.release();
	
	_triggerCount++;
}

/* 视窗消息站手动消息投递分发 */
void EventStation::manuallyDispatchEvent()
{
	_dispatcher.manuallyRun();
}

void EventStation::outputLog( ofstream& fout )
{
	fout << "    Trigger count: " << _triggerCount << endl
		 << "    Handle system event: " << _handleSystemCount << " times" << endl
		 << "    Handle custom event: " << _handleCustomCount << " times" << endl;
}
