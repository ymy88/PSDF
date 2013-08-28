#pragma once

#include "../Common/Common.h"
#include "../Event/EventType.h"

class NetworkDataRecognizer
{
public:
	NetworkDataRecognizer() : _refCount(1) {}
	NetworkDataRecognizer* retain()
	{
		_refCount++;
		return this;
	}
	void release()
	{
		_refCount--;
		if (_refCount == 0)
		{
			delete this;
		}
	}

protected:
	virtual ~NetworkDataRecognizer() {}
	
public:

	/* return eventType */
	virtual void recognize(_IN char* data, _OUT size_t& length, _OUT unsigned int& eventType) const = 0;

private:
	int _refCount;
};