#pragma once

#include "../Common/Common.h"
#include "../Thread/PosixThreads.h"
#include "DataPtr.h"
#include <ctime>

const unsigned int DATACENTER_BUFFER_LENGTH = 100000;
const unsigned int DATACENTER_MAX_ROUND = 7;

class PSDF_CORE_DLL_DECL DataCenter
{
private:
	DataCenter();
	DataCenter(const DataCenter&) {}
	~DataCenter();

public:
	static  DataCenter*	inst();

	int		addData(char* data, int length); /* return dataIndex */

    template<typename T>
	bool	getData(DataPtr<T>& dataPtr, unsigned int dataIndex)
	{
		if (_dataBuffer[dataIndex].buffer == NULL ||
			_dataBuffer[dataIndex].dataLength == 0)
		{
			return false;
		}

		dataPtr.assign(_dataBuffer[dataIndex]);
		return true;
	}

    template<typename T>
    DataPtr<T> getData(unsigned int dataIndex)
	{
        DataPtr<T> ptr;

		if (_dataBuffer[dataIndex].buffer == NULL ||
			_dataBuffer[dataIndex].dataLength == 0 )
		{
			return ptr;
		}

		ptr.assign(_dataBuffer[dataIndex]);
		return ptr;
	}

	void	outputLog(ofstream& fout);

private:
	bool tryClearData(ReferencedData& refData, int newDataLength, int timestamp);

private:
    ReferencedData          _dataBuffer[DATACENTER_BUFFER_LENGTH];

	PosixMutex				_bufferLock[DATACENTER_BUFFER_LENGTH];

	unsigned int			_lastIndex;
	PosixMutex				_indexMutex;
	
	/* log */
	int						_lookedCount;
	int						_hitCount;
};
