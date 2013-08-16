#pragma once

#include "../Common/Common.h"
#include "../Thread/PosixThreads.h"

#include "EventType.h"


enum HandlerPriority
{
	PRIORITY_1, //最低优先级
	PRIORITY_2,
	PRIORITY_3,
	PRIORITY_4,
	PRIORITY_5	// 最高优先级
};

class PSDF_CORE_DLL_DECL EventHandler
{
public:
	EventHandler() : _priority( PRIORITY_3 ) {}
	EventHandler( HandlerPriority pr ) : _priority( pr ) {}
	EventHandler( const EventHandler& handler ) { _priority = handler._priority; _handlerMutex = handler._handlerMutex; }

	virtual ~EventHandler() {}

public:
	void				setPriority( HandlerPriority pr ) { _priority = pr; }
	HandlerPriority		getPriority() { return _priority; }

	void				handle( unsigned eventType, int param1, int param2, int currStationId)
	{
		// 此处使用mutex保证handle函数只会同时被一个线程调用

		_handlerMutex.lock();
		_currStationId = currStationId;
		if( eventType & SYSTEM_EVENT_GROUP )
		{
			handleSystemEvent( eventType, param1, param2 );
		}
		else
		{
			handleCustomEvent( eventType, param1, param2 );
		}
		_handlerMutex.unlock();
	}

protected:	
	int					getCurrStationId() { return _currStationId; }

	virtual void		handleSystemEvent( unsigned eventType, int param1, int param2 )
	{
		TOUCH( eventType );
		TOUCH( param1 );
		TOUCH( param2 );
	}
	virtual void		handleCustomEvent( unsigned eventType, int param1, int param2 )
	{
		TOUCH( eventType );
		TOUCH( param1 );
		TOUCH( param2 );
	}

private:	
	HandlerPriority		_priority;
	PosixMutex			_handlerMutex;

	int					_currStationId;
};