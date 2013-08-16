#pragma once

#include "../Common/Common.h"
#include "../Thread/PosixThreads.h"

#include "EventType.h"
#include "EventHandler.h"

class EventCenter;
class OsgViewerBase;

const short	EVENT_STATION_QUEUE_LENGTH = 10000;

class PSDF_CORE_DLL_DECL EventStation
{
	friend class EventCenter;
	friend class OsgViewerBase;

private:

	struct EventStationStruct
	{
		unsigned int	eventType;
		int				param1;
		int				param2;
		EventStationStruct() : eventType(0), param1(0), param2(0) {}
	};

	class EventStationDispatcher : public PosixThread
	{
	public:
		void				run();

		// this method is called by the main thread of the window, before every frame
		void				manuallyRun();

		bool				peekEvent( unsigned int &eventType, int &param1, int &param2, bool shouldWait );
		void				dispatchEvent( unsigned int eventType, int param1, int param2 );

	public:
		EventStation *		_station;
	};


	class EventHandlerQueue
	{
	public:
		const vector< EventHandler * >		&beginUpdate();
		void								endUpdate();
		void								addHandler( EventHandler * handler );
		void								removeHandler( EventHandler * handler );

	private:
		void								solvePending();

	private:
		vector< EventHandler * >			_handlers;
		vector< EventHandler * >			_pendingHandlers;
		PosixMutex							_pendingMutex;
		PosixReadWriteLock					_handlersLock;
	};

private:
	EventStation(int stationId = -1);
	~EventStation();

	void						triggerEvent( unsigned int eventType, int param1, int param2, bool shouldWait );
	/*
	 *	Parameter "eventGroupMask" can be any conbination of the event
	 *	cluster type. If eventGroupMask is EVENT_GROUP_TYPE_MASK_ALL,
	 *	it means the handler wants to handle all events (when registering), 
	 *	or does not want to handle any event any longer (when unregistering).
	 */
	void						registerHandler( EventHandler * handler, unsigned int eventGroupMask = EVENT_GROUP_ALL );
	void						unregisterHandler( EventHandler * handler, unsigned int eventGroupMask = EVENT_GROUP_ALL );
		
public:
	unsigned int				getStationId()				{ return _stationId; }
	void						manuallyDispatchEvent();

	void						outputLog(ofstream& fout);

private:
	//event handler queues for every event cluster.
	EventHandlerQueue			_eventHandlerQueues[ EVENT_GROUP_COUNT ];				//消息处理者队列

	EventStationDispatcher *	_dispatchers;											//消息站消息投递分发器
	short						_dispatcherCount;										//消息投递分发器个数

	EventStationStruct			_stationEventQueue[ EVENT_STATION_QUEUE_LENGTH ];		//消息站消息队列
	PosixMutex					_eventQueueMutex;										//消息队列互斥量
	PosixSemaphore				_freeSem;												//消息站空闲信号量
	PosixSemaphore				_usedSem;												//消息站使用信号量
	int							_putPos;												//消息队列消息存储位置索引
	int							_getPos;												//消息队列消息读取位置索引

	unsigned int				_stationId;												//消息站编号,视窗与消息站通过编号对应

	size_t						_triggerCount;
	size_t						_handleSystemCount;
	size_t						_handleCustomCount;

	static unsigned int		_nextStationId;											
};
