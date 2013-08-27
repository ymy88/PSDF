#pragma once

#include "../Common/Common.h"

#ifdef _WIN32
#include "pthread/pthread.h"
#include "pthread/semaphore.h"
#else
#include <pthread.h>
#include <semaphore.h>
#endif

//-----------------------------------
// PosixThread
//-----------------------------------
class PSDF_CORE_DLL_DECL PosixThread
{
public:
	PosixThread( void ) {}

	virtual ~PosixThread( void ) {}

public:
	void start()
	{
		pthread_attr_t attr;

		pthread_attr_init( &attr );
		pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );

		pthread_create( &_threadId, &attr, threadFunc, ( void * )this );

		pthread_attr_destroy( &attr );
	}

	void		stop() { pthread_cancel(_threadId); }
	pthread_t	getThreadId() { return pthread_self(); }

	static void	waitUntilDone(pthread_t threadId) { pthread_join( threadId, NULL ); }
	static void	msleep( long millisec )
	{
#ifdef _WIN32
		Sleep(millisec);
#else
		usleep(millisec * 1000);
#endif
	}

protected:
	virtual void run() = 0;

private:
	static void* threadFunc( void * instance )
	{
		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

		PosixThread * _instance = ( PosixThread * )instance;
		_instance->run();

		pthread_exit( NULL );

		return NULL;
	}

private:
	pthread_t _threadId;
};

//-----------------------------------
// PosixMutex
//-----------------------------------
class PSDF_CORE_DLL_DECL PosixMutex
{
public:
	PosixMutex()	{ pthread_mutex_init( &_mutex, NULL ); }
	~PosixMutex()	{ pthread_mutex_destroy( &_mutex ); }

public:
	void lock()		{ pthread_mutex_lock( &_mutex ); }
	void unlock()	{ pthread_mutex_unlock( &_mutex ); }
	bool tryLock()	{ return ( 0 == pthread_mutex_trylock( &_mutex ) ); }

private:
	pthread_mutex_t		_mutex;	
};


//-----------------------------------
// PosixSemaphore
//-----------------------------------
/* pthread for Win32并没有实现sem_open()函数及相关的宏 */
/* 定义这些宏使得程序在windows下能编译通过 */
#ifndef SEM_FAILED
#define SEM_FAILED (sem_t*)-1
#endif
#ifndef O_CREAT
#define O_CREAT 0x1
#endif
#ifndef O_EXCL
#define O_EXCL 0x2
#endif
#ifndef S_IRWXU
#define S_IRWXU 700
#endif

class PSDF_CORE_DLL_DECL PosixSemaphore
{
public:
	explicit PosixSemaphore( int initCount = 0 )
	{
		if (-1 == sem_init( &_sem, 0, initCount ))
		{
			/* 某些系统不支持匿名信号量（比如Mac OS），则使用命名信号量 */
			srand(clock());
			do
			{
				memset(_name, 0, sizeof(_name));
				int n = rand();
				sprintf(_name, "%d", n);
				sem_unlink(_name);
				_pSem = (sem_t*)sem_open(_name, O_CREAT | O_EXCL, S_IRWXU, initCount);

			} while (_pSem == SEM_FAILED);
		}
		else
		{
			_pSem = &_sem;
		}
	}

	~PosixSemaphore()
	{
		if (-1 == sem_destroy( _pSem ))
		{
			sem_unlink(_name);
			sem_close(_pSem);
		}
	}

public:
	void acquire() { sem_wait( _pSem ); }
	void release() { sem_post( _pSem ); }
	bool tryAcquire() { return ( 0 == sem_trywait( _pSem ) ); }

private:
	sem_t	_sem;
	sem_t*	_pSem;
	char	_name[30];
};

//-------------------------------------
// PosixReadWriteLock
//-------------------------------------
class PSDF_CORE_DLL_DECL PosixReadWriteLock
{
public:
	PosixReadWriteLock()	{ pthread_rwlock_init( &_rwlock, NULL ); }
	~PosixReadWriteLock()	{ pthread_rwlock_destroy( &_rwlock ); }

public:
	void lockForRead()		{ pthread_rwlock_rdlock( &_rwlock ); }
	void lockForWrite()		{ pthread_rwlock_wrlock( &_rwlock ); }
	bool tryLockForRead()	{ return ( 0 == pthread_rwlock_tryrdlock( &_rwlock ) ); }
	bool tryLockForWrite()	{ return ( 0 == pthread_rwlock_trywrlock( &_rwlock ) ); }
	void unlock()			{ pthread_rwlock_unlock( &_rwlock ); }

private:
	pthread_rwlock_t	_rwlock;
};