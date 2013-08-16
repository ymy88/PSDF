#pragma once

#include "../Common/Common.h"
#include "../Thread/PosixThreads.h"
#include <ctime>

const unsigned int DATACENTER_BUFFER_LENGTH = 100000;
const unsigned int DATACENTER_MAX_ROUND = 7;

class PSDF_CORE_DLL_DECL DataCenter
{
private:

	//-----------------------------------------
	// ReferencedData
	//-----------------------------------------
	struct ReferencedData
	{
		short	refCount;
		int		bufferLength;
		int		dataLength;
		char*	buffer;
		short	round;

		ReferencedData():
		refCount(0), bufferLength(0), dataLength(0), buffer(NULL), round(-1) {}
	};
	//------------------------------------------

public:

	//-----------------------------------
	// DataPtr
	//-----------------------------------
	template<typename T>
	class DataPtr
	{
	public:
		DataPtr() { _refData = NULL; _ptr = NULL; }
		DataPtr(const DataPtr& dataPtr)
		{
			if (dataPtr._refData)
			{
				this->_refData = dataPtr._refData;
				this->_ptr = dataPtr._ptr;
				_refData->refCount++;
			}
		}
		~DataPtr()
		{
			if (_refData)
			{
				_refData->refCount--;
			}
		}

		// pointer operations
		T& operator*() const { return *_ptr; }
		T* operator->() const { return _ptr; }
		T* get() const { return _ptr; }

		// bool operations
		operator T*() { return _ptr; }
		bool operator==(const DataPtr& dataPtr) const { return this->_ptr == dataPtr._ptr; }
		bool operator==(const T* ptr) const { return this->_ptr == ptr; }
		friend bool operator==(const T* ptr, const DataPtr& dataPtr) { return ptr == dataPtr._ptr; }
		bool operator!=(const DataPtr& dataPtr) const { return this->_ptr != dataPtr._ptr; }
		bool operator!=(const T* ptr) const { return this->_ptr != ptr; }
		friend bool operator!=(const T* ptr, const DataPtr& dataPtr) { return ptr != dataPtr._ptr; }

		void assign(ReferencedData& refData)
		{
			if (_refData)
			{
				_refData->refCount--;
			}
			_refData = &refData;
			_ptr = (T*)(_refData->buffer);
			_refData->refCount++;
		}

	private:
		ReferencedData* _refData;
		T*				 _ptr;

	};
	//--------------------------------------------

private:
	DataCenter();
	DataCenter(const DataCenter&) {}
	~DataCenter();

public:
	static DataCenter*	inst();

	int		addData(char* data, int length); // return dataIndex

	template<typename T>
	bool	getData(DataPtr<T>& dataPtr, unsigned int dataIndex)
	{
		int r, i;
		getFromDataIndex(dataIndex, r, i);
		if (_dataBuffer[i].buffer == NULL ||
			_dataBuffer[i].dataLength == 0 ||
			_dataBuffer[i].round != r)
		{
			return false;
		}

		dataPtr.assign(_dataBuffer[i]);
		return true;
	}

	void	outputLog(ofstream& fout);

private:
	bool	tryClearData(ReferencedData& refData, int newDataLength);
	
	int		createDataIndex(int r, int i)
	{
		int res = (r << 28 | (i & 0x0fffffff));
		return res;
	}
	void	getFromDataIndex(int dataIndex, int& r, int& i)
	{
		r = (dataIndex >> 28) & 0x7;
		i = dataIndex & 0x0fffffff;
	}

private:
	ReferencedData			_dataBuffer[DATACENTER_BUFFER_LENGTH];

	PosixMutex				_bufferLock[DATACENTER_BUFFER_LENGTH];

	unsigned int			_lastIndex;
	PosixMutex				_indexMutex;

	unsigned int			_round;
	
	// log
	int						_lookedCount;
	int						_hitCount;
};
