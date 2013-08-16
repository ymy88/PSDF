#include "StdAfx.h"

#include <PSDFCore/Event/EventCenter.h>

//---------------------------------------------
// EventCenterDispatcher
//---------------------------------------------
void EventCenter::EventCenterDispatcher::run()
{
	unsigned int eventType = 0;
	unsigned int stationId = 0;
	int param1 = 0, param2 = 0;
	bool isHighPriority;

	for(;;)
	{
		if( !peekEvent( eventType, param1, param2, stationId, isHighPriority ) )
		{
			msleep(10);
			continue;
		}

		dispatchEvent( eventType, param1, param2, stationId, isHighPriority );

		if( eventType == APP_QUIT ) { return; }
	}
}

bool EventCenter::EventCenterDispatcher::peekEvent( unsigned int &eventType, int &param1, int &param2, unsigned int &stationId, bool& isHighPriority )
{
	//优先级最高的最先投递分发
	for( int pl = PRIORITY_HIGH; pl >= PRIORITY_LOW; --pl )
	{
		if( _eventCenter->_pUsedSem[ pl ]->tryAcquire() )
		{
			eventType			= _eventCenter->_centerEventQueue[ pl ][ _eventCenter->_getPos[ pl ] ].eventType;
			param1				= _eventCenter->_centerEventQueue[ pl ][ _eventCenter->_getPos[ pl ] ].param1;
			param2				= _eventCenter->_centerEventQueue[ pl ][ _eventCenter->_getPos[ pl ] ].param2;
			stationId			= _eventCenter->_centerEventQueue[ pl ][ _eventCenter->_getPos[ pl ] ].stationId;
			isHighPriority		= (pl == PRIORITY_HIGH);

			_eventCenter->_getPos[ pl ] = ( _eventCenter->_getPos[pl] == EVENT_CENTER_QUEUE_LENGTH - 1 ) ? 0 : _eventCenter->_getPos[ pl ] + 1;
			_eventCenter->_pFreeSem[ pl ]->release();

			return true;
		}
	}
	return false;
}

void EventCenter::EventCenterDispatcher::dispatchEvent( unsigned int eventType, int param1, int param2, unsigned int stationId, bool isHighPriority )
{
	_eventCenter->_dispatchCount++;
	
	if( stationId == EVENT_STATION_ID_ALL_STA )
	{
		for( unsigned int i = 0; i < _eventCenter->_eventStations.size(); ++i )
		{
			_eventCenter->_eventStations[i]->triggerEvent(eventType, param1, param2, isHighPriority);
		}
	}

	else
	{
		for( unsigned int i = 0; i < _eventCenter->_eventStations.size(); ++i )
		{
			if( _eventCenter->_eventStations[i]->getStationId() == stationId )
			{
				_eventCenter->_eventStations[i]->triggerEvent(eventType, param1, param2, isHighPriority);
				break;
			}
		}
	}
}



//---------------------------------------
// EventCenter
//--------------------------------------

EventCenter* EventCenter::inst()
{
	static EventCenter instance;
	return &instance;
}

EventCenter::EventCenter()
{
	memset( _putPos, 0, sizeof( _putPos ) );
	memset( _getPos, 0, sizeof( _getPos ) );

	_triggerCount = 0;
	_dispatchCount = 0;
	
	for( int i = 0; i < PRIORITY_LEVEL_COUNT; i++ )
	{
		_pFreeSem[ i ] = new PosixSemaphore( EVENT_CENTER_QUEUE_LENGTH );
		_pUsedSem[ i ] = new PosixSemaphore();
	}
	
	//消息中心初始化就创建非视窗的消息站，非视窗的消息站编号为0x000000001
	_nonViewStation = new EventStation(EVENT_STATION_ID_NON_VIEW);

	//将非视窗的消息站添加到消息中心的消息站容器中	
	_eventStations.push_back( _nonViewStation );
	
	_dispatcher._eventCenter = this;
	_dispatcher.start();
}

EventCenter::~EventCenter(void)
{
	_dispatcher.stop();
	
	for ( int i = 0; i < PRIORITY_LEVEL_COUNT; i++ )
	{
		delete _pFreeSem[ i ];
		delete _pUsedSem[ i ];
	}

	for (unsigned int i = 0; i < _eventStations.size(); ++i)
	{
		delete _eventStations[i];
	}


	_eventStations.clear();
	
#ifndef _WIN32
	PosixThread::waitUntilDone(_dispatcher.getThreadId());
#endif
}


void EventCenter::registerStation( EventStation * station )
{
	// this stationId is reserved to serve the modules which don't belong to any viewers.
	if( station->getStationId() == EVENT_STATION_ID_NON_VIEW ) 
	{
		return;
	}

	_eventStations.push_back( station );
}

void EventCenter::unregisterStation( EventStation * station )
{
	vector< EventStation * >::iterator	iter;

	for( iter = _eventStations.begin(); iter != _eventStations.end(); ++iter )
	{
		if( ( *iter ) == station )
		{
			_eventStations.erase( iter );
			break;
		}
	}
}

void EventCenter::registerHandler( EventHandler * handler, unsigned int eventGroupMask, unsigned int stationId )
{
	//消息处理者向消息中心注册消息,消息中心将消息处理者指派到二级消息中心(消息站)
	//二级消息中心(消息站)将消息处理者添加到消息处理者队列

	for( unsigned int i = 0; i < _eventStations.size(); ++i )
	{
		if( _eventStations[ i ]->getStationId() == stationId )
		{
			_eventStations[ i ]->registerHandler( handler, eventGroupMask );
			break;
		}
	}
}

void EventCenter::unregisterHandler( EventHandler * handler, unsigned int eventGroupMask, unsigned int stationId )
{
	for( unsigned int i = 0; i < _eventStations.size(); ++i )
	{
		if( _eventStations[ i ]->getStationId() == stationId )
		{
			_eventStations[ i ]->unregisterHandler( handler, eventGroupMask );
			break;
		}
	}
}

void EventCenter::triggerEvent( unsigned int eventType, int param1, int param2, unsigned int stationId, EVENT_PRIORITY_LEVEL priority )
{
	if( priority >= PRIORITY_LEVEL_COUNT )
	{
		return;
	}

	// event is not very important, can be ignored if the queue is full
	if( priority <= PRIORITY_NORMAL )
	{
		if( !_pFreeSem[ priority ]->tryAcquire() )
		{
			return;
		}
	}
	// event is important, can NOT be ignored EVEN IF the queue is full
	else
	{
		_pFreeSem[ priority ]->acquire();
	}

	_eventQueueMutex.lock();
	
	EventCenterStruct& e = _centerEventQueue[priority][_putPos[priority]];
	_putPos[ priority ] = ( _putPos[ priority ] == EVENT_CENTER_QUEUE_LENGTH - 1 ) ? 0 : _putPos[ priority ] + 1;
	
	_eventQueueMutex.unlock();
	
	_triggerCount++;
	
	e.eventType		= eventType;
	e.param1		= param1;
	e.param2		= param2;
	e.stationId		= stationId;
	
	_pUsedSem[ priority ]->release();
}

void EventCenter::outputLog( ofstream& fout )
{
	fout << "EventCenter: " << endl
		 << "    Trigger count: " << _triggerCount << endl
		 << "    Dispatch count: " << _dispatchCount << endl
		 << endl;
	for (unsigned int i = 0; i < _eventStations.size(); ++i)
	{
		fout << "EventStation " << _eventStations[i]->getStationId() << ":" << endl;
		_eventStations[i]->outputLog(fout);
	}
}
