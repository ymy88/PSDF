#include "StdAfx.h"

#include <PSDFCore/Data/DataCenter.h>
#include <PSDFCore/Event/EventType.h>

DataCenter::DataCenter()
{
	_lookedCount= 0;
	_hitCount= 0;
	_lastIndex = 0;
	_round = 1;
}

DataCenter::~DataCenter()
{
	for (unsigned int i = 0; i < DATACENTER_BUFFER_LENGTH; ++i)
	{
		_bufferLock[i].lock();
		if (_dataBuffer[i].buffer)
		{
			delete [] _dataBuffer[i].buffer;
			_dataBuffer[i].buffer = NULL;
			_dataBuffer[i].bufferLength = 0;
		}
		_bufferLock[i].unlock();
	}

}

DataCenter* DataCenter::inst()
{
	static DataCenter instance;
	return &instance;
}

bool DataCenter::tryClearData( ReferencedData& refData, int newDataLength )
{
	if (refData.refCount > 0) { return false; }

	int roundDiff = _round - refData.round;
	if (roundDiff == 0) { return false; }
	

	if (refData.buffer == NULL)
	{
		refData.buffer = new char[newDataLength];
		refData.bufferLength = newDataLength;
	}
	else if (newDataLength > refData.bufferLength)
	{
		delete [] refData.buffer;	
		refData.buffer = new char[newDataLength];
		refData.bufferLength = newDataLength;
	}

	refData.refCount = 0;
	refData.dataLength = newDataLength;
	refData.round = _round;

	return true;
}

int DataCenter::addData(char* data, int length)
{
	unsigned int i = _lastIndex + 1;
	int currRound = _round;

	for (;; ++i)
	{
		if (i >= DATACENTER_BUFFER_LENGTH)
		{
			i = 0;
			++currRound;
			if (currRound > (int)DATACENTER_MAX_ROUND) { currRound = 1; }
			
			_round = currRound;
		}

		++_lookedCount;

		bool locked = _bufferLock[i].tryLock();
		if (!locked) { continue; }

		if (tryClearData(_dataBuffer[i], length))
		{
			++_hitCount;

			memcpy(_dataBuffer[i].buffer, data, length);

			_lastIndex = i;
			_bufferLock[i].unlock();
			break;
		}

		_bufferLock[i].unlock();
	}

	int res = createDataIndex(_round, i);
	return res;
}

void DataCenter::outputLog( ofstream& fout )
{
	double rate = 0;
	if (_lookedCount> 0)
	{
		rate = 1.0 * _hitCount / _lookedCount;
	}

	fout << "DataCenter: " << endl
		 << "\tLooked " << _lookedCount << " positions in buffer\n"
		 << "\tHit    " << _hitCount << " times\n"
		 << "\tHit rate: " << rate << endl;
}

