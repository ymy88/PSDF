//-----------------------------------------
// ReferencedData
//-----------------------------------------
struct ReferencedData
{
	unsigned short	refCount;
	unsigned short	bufferLength;
	unsigned short	dataLength;
	char*	        buffer;
	clock_t         timestamp;

	ReferencedData():
	refCount(0), bufferLength(0), dataLength(0), buffer(NULL), timestamp(0) {}

	void destruct()
	{

	}
};

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

	/* pointer operations */
	T& operator*() const { return *_ptr; }
	T* operator->() const { return _ptr; }
	T* get() const { return _ptr; }

	/* bool operations */
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
	ReferencedData*  _refData;
	T*                  _ptr;

};