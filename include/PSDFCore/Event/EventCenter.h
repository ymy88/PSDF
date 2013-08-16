#pragma once

#include "../Common/Common.h"
#include "../Thread/PosixThreads.h"

#include "EventType.h"
#include "EventHandler.h"
#include "EventStation.h"


const short	EVENT_CENTER_QUEUE_LENGTH = 32000;

class PSDF_CORE_DLL_DECL EventCenter
{
private:

	struct EventCenterStruct
	{
		unsigned int	eventType;
		int				param1;
		int				param2;
		unsigned int	stationId;
		EventCenterStruct() : eventType(0), param1(0), param2(0), stationId(0) {}
	};

	class EventCenterDispatcher : public PosixThread
	{
	public:
		virtual void		run();

		//消息读取
		bool				peekEvent( unsigned int &eventType, int &param1, int &param2, unsigned int &stationId, bool& isHighPriority );

		//消息分发
		void				dispatchEvent( unsigned int eventType, int param1, int param2, unsigned int stationId, bool isHighPriority ); 

	public:
		EventCenter*		_eventCenter;
	};


private:
	EventCenter( void );
	~EventCenter( void );

public:
	static EventCenter *		inst(); 

	void						setFocusedStation( EventStation * station ){ _focusedStation = station; }

	void						registerStation( EventStation * station );
	void						unregisterStation( EventStation * station );

	void						registerHandler( EventHandler * handler, unsigned int eventGroupMask, unsigned int stationId = EVENT_STATION_ID_NON_VIEW );
	void						unregisterHandler( EventHandler * handler, unsigned int eventGroupMask, unsigned int stationId = EVENT_STATION_ID_NON_VIEW );

	/*
	 * eventType	: Type of this event. (see EventType.h)
	 * param1		: A 32-bit params of the event. (see EventType.h) 
	 * param2		: A 32-bit params of the event. (see EventType.h) 
	 * stationId	: Which  station should receive this event.
	 * priority		: The priority of this event. (see EventType.h) 
	 *
	 * **Note** A module can receive a certain event, if all the following are satisfied:
	 *  (1) The module registered the group of the event.
	 *  (2) The module registered to the station whose id equals stationId, or stationId == EVENT_STATION_ID_ALL_STA.
	 */
	void						triggerEvent( unsigned int eventType, int param1 = 0, int param2 = 0, unsigned int stationId = EVENT_STATION_ID_NON_VIEW,
											  EVENT_PRIORITY_LEVEL priority = PRIORITY_NORMAL );

	void						outputLog(ofstream& fout);

private:
	//消息中心消息队列 Event queue
	//消息等级按照优先级分为四级，每级可存储100条消息	
	EventCenterStruct			_centerEventQueue[ PRIORITY_LEVEL_COUNT ][ EVENT_CENTER_QUEUE_LENGTH ];
	short						_putPos[ PRIORITY_LEVEL_COUNT ];
	short						_getPos[ PRIORITY_LEVEL_COUNT ];
	PosixSemaphore *			_pFreeSem[ PRIORITY_LEVEL_COUNT ];
	PosixSemaphore *			_pUsedSem[ PRIORITY_LEVEL_COUNT ];
	PosixMutex					_eventQueueMutex;
	
	EventCenterDispatcher		_dispatcher;							//消息中心消息投递线程

	vector< EventStation * >	_eventStations;						//消息站容器,存储所有视窗的消息站和非视窗消息站
	EventStation *				_focusedStation;						//焦点所在视窗口消息站
	EventStation *				_nonViewStation;						//非视窗口消息站
	
	int							_triggerCount;
	int							_dispatchCount;
};
