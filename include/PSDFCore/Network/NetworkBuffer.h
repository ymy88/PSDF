#pragma once

#include "../Common/Common.h"
#include "../Thread/PosixThreads.h"
#include "NetworkDataRecognizer.h"

const unsigned int NETWORK_BUFFER_LENGTH = 1 << 23;				  /* 8M */
const unsigned int BUFFER_BEGIN_INDEX	 = 1 << 20;				  /* 缓冲区开头预留1M */
const unsigned int BUFFER_END_INDEX		 = (1 << 22) - (1 << 20); /* 缓冲区尾部预留1M */

class NetworkConn;

typedef int (*OnReceiveCallback)(char* data, size_t receivedLen, NetworkConn* conn);

class PSDF_CORE_DLL_DECL NetworkBuffer : public PosixThread
{
public:
	NetworkBuffer(OnReceiveCallback onReceive);
	~NetworkBuffer();

	void					setNetworkConn(NetworkConn* conn);

	void					update(size_t size);
	char*					getPutPos();
	size_t					getLength();
	void					run();

private:
	NetworkConn*			_conn;

	char*					_buffer;
	unsigned int			_putPos;
	unsigned int			_getPos;

	unsigned int			_posInTail;

	OnReceiveCallback		_onReceive;
};
