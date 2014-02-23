#include "StdAfx.h"

#include <PSDFCore/Network/NetworkBuffer.h>

NetworkBuffer::NetworkBuffer( OnReceiveCallback* onReceive ) :
_onReceive(onReceive),
_quit(false)
{
	_buffer = new char[NETWORK_BUFFER_LENGTH];
	_putPos = BUFFER_BEGIN_INDEX;
	_getPos = BUFFER_BEGIN_INDEX;
	_posInTail = BUFFER_END_INDEX;
}

NetworkBuffer::~NetworkBuffer()
{
	delete [] _buffer;
}

void NetworkBuffer::update( size_t size )
{
	_putPos += size;
	if (_putPos > BUFFER_END_INDEX)
	{
		_posInTail = _putPos;
		_putPos = BUFFER_BEGIN_INDEX;
	}
}

char* NetworkBuffer::getPutPos()
{
	return ((char*)_buffer) + _putPos;
}

size_t NetworkBuffer::getLength()
{
	return NETWORK_BUFFER_LENGTH - _putPos;
}

void NetworkBuffer::run()
{
	size_t receivedLen = 0; /* 网络收到的数据长度 */
	size_t dataLen = 0; /* 一个完整的数据包的长度 */

	while (!_quit)
	{
		while (_getPos == _putPos)
		{
			msleep(1);
		}

		if (_putPos < _getPos)
		{
			receivedLen = _posInTail - _getPos;
		}
		else
		{
			receivedLen = _putPos - _getPos;
		}
		if (receivedLen == 0)
		{
			continue;
		}

		dataLen = (*_onReceive)(_buffer + _getPos, receivedLen, _conn);

		_getPos += dataLen;
		if (_getPos == _posInTail)
		{
			_getPos = BUFFER_BEGIN_INDEX;
			_posInTail = BUFFER_END_INDEX;
		}
	}
}

void NetworkBuffer::setNetworkConn( NetworkConn* conn )
{
	_conn = conn;
}
