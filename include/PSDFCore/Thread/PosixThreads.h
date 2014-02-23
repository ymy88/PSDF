#pragma once

#include "../Common/Common.h"

#include <boost/thread.hpp>
#include <boost/chrono.hpp>

//-----------------------------------
// PosixThread
//-----------------------------------
class PSDF_CORE_DLL_DECL PosixThread
{
	struct ThreadCallable
	{
		PosixThread* _thread;
		void operator() () { _thread->run(); }
	};

public:
	PosixThread() : _started(false), _thread(NULL) {}
	virtual ~PosixThread()
	{
		if (_thread)
		{
			_thread->join();
			delete _thread;
		}
	}

public:
	void start()
	{
		_started = true;
		_callable._thread = this;
		_thread = new boost::thread(boost::ref(_callable));
	}

	virtual void stop() { _thread->interrupt(); }
	void		 waitUntilDone() { _thread->join(); }

	static void	msleep( long millisec ) { boost::this_thread::sleep_for(boost::chrono::milliseconds(millisec)); }
	static void	nsleep( long nanosec ) { boost::this_thread::sleep_for(boost::chrono::nanoseconds(nanosec)); }

protected:
	virtual void run() = 0;

private:
	boost::thread*	_thread;
	ThreadCallable	_callable;
	bool			_started;
};

//-----------------------------------
// PosixMutex
//-----------------------------------
class PSDF_CORE_DLL_DECL PosixMutex
{
public:
	PosixMutex()	{}
	~PosixMutex()	{}

public:
	void lock()		{ _mutex.lock(); }
	void unlock()	{ _mutex.unlock(); }
	bool tryLock()	{ return _mutex.try_lock(); }

private:
	boost::mutex	_mutex;	
};

class PSDF_CORE_DLL_DECL PosixScopeMutex
{
public:
	PosixScopeMutex(PosixMutex& mutex) : _mutex(mutex) {_mutex.lock();}
	~PosixScopeMutex(){_mutex.unlock();}

private:
	PosixScopeMutex(const PosixScopeMutex& m) : _mutex(m._mutex) {}
	PosixScopeMutex& operator=(const PosixScopeMutex&) { return *this; }

private:
	PosixMutex& _mutex;
};

//-----------------------------------
// PosixSemaphore
//-----------------------------------
class PSDF_CORE_DLL_DECL PosixSemaphore
{
public:
	explicit PosixSemaphore( int initCount = 0 ) : _val(initCount) { }
	~PosixSemaphore() { }

public:
	void acquire()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		while (_val == 0)
		{
			_cond.wait(lock);
		}
		--_val;
	}

	void release()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		++_val;
		_cond.notify_all();
	}
	bool tryAcquire()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		if (_val == 0) { return false; }

		--_val;
		return true;
	}

private:
	boost::mutex						_mutex;
	boost::condition_variable			_cond;
	int									_val;
};

//-------------------------------------
// PosixReadWriteLock
//-------------------------------------
class PSDF_CORE_DLL_DECL PosixReadWriteLock
{
public:
	PosixReadWriteLock()	{}
	~PosixReadWriteLock()	{}

public:
	void lockForRead()		{ _rwlock.lock_shared(); }
	void unlockForRead()	{ _rwlock.unlock_shared(); }
	void lockForWrite()		{ _rwlock.lock(); }
	void unlockForWrite()	{ _rwlock.unlock(); }
	bool tryLockForRead()	{ return _rwlock.try_lock_shared(); }
	bool tryLockForWrite()	{ return _rwlock.try_lock(); }

private:
	boost::shared_mutex	_rwlock;
};