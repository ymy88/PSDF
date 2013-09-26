#include "StdAfx.h"

#include <PSDFCore/Network/NetworkCenter.h>
#include <PSDFCore/Data/DataCenter.h>
#include <PSDFCore/Event/EventCenter.h>

#include <fstream>

using namespace boost::asio;

//----------------------------------------
// NetConnection
//----------------------------------------
NetworkConn::NetworkConn( NetworkDataRecognizer* dataRecognizer /* = NULL */ ) :
_dataRecognizer(dataRecognizer), _buffer(onReceive)
{
	_receivedCount = 0;
	_handledCount = 0;
	_directBuffer = new char[1 << 20];

	_buffer.setNetworkConn(this);
	if (_dataRecognizer != NULL)
	{
		_buffer.start();
	}
}

int NetworkConn::onReceive(char* data, size_t receivedLen, NetworkConn* conn)
{
	unsigned int eventType;
	size_t dataLen = 0;
	conn->_dataRecognizer->recognize(data, dataLen, eventType);
	if (eventType != 0 && receivedLen >= dataLen && dataLen > 0)
	{
		int dataIndex = DataCenter::inst()->addData(data, dataLen);
		EventCenter::inst()->triggerEvent(eventType, conn->_index, dataIndex);
		conn->_handledCount++;
	}
	return dataLen;
}

NetworkConn::~NetworkConn()
{
	if (_dataRecognizer)
	{
		_dataRecognizer->release();
		_buffer.stop();
	}
	delete [] _directBuffer;
}

void NetworkConn::outputLog( ofstream& fout )
{
	fout << "    Received count: " << _receivedCount << endl
		 << "    Handled count: " << _handledCount << endl;
}


//-------------------------------------
// Multicast
//-------------------------------------
Multicast::Multicast( const string& ipAddr, unsigned short recvPort, NetworkDataRecognizer* dataRecognizer )
: NetworkConn(dataRecognizer), _socket(_ioService)
{
	joinGroup(ipAddr, recvPort);	
}

bool Multicast::joinGroup( const string& ipAddr, unsigned short recvPort )
{
	ip::address listenAddress = ip::address::from_string("0.0.0.0"); /* any address */

	_recvPort = recvPort;
	ip::udp::endpoint listenEndpoint(listenAddress, recvPort);

	try
	{
		_socket.open(listenEndpoint.protocol());
		_socket.set_option(ip::udp::socket::reuse_address(true));
		_socket.bind(listenEndpoint);

		/* join the multicast group */
		_address = ip::address::from_string(ipAddr);
		_socket.set_option(ip::multicast::join_group(_address));

		this->start();

		return true;
	}
	catch(...)
	{
		return false;
	}
}

bool Multicast::send( char* data, size_t length )
{
	io_service ioService;
	ip::udp::endpoint ep(_address, _recvPort);
	ip::udp::socket sendSocket(ioService, ep.protocol());
	try
	{
		sendSocket.send_to(buffer(data, length), ep);
		return true;	
	}
	catch(...)
	{
		return false;
	}
}

void Multicast::run()
{
	boost::asio::mutable_buffers_1 tmpBuffer = buffer(_directBuffer, 1 << 20);
	try
	{
		while (true)
		{
			size_t size = _socket.receive(tmpBuffer);
			++_receivedCount;
			memcpy(_buffer.getPutPos(), _directBuffer, size);
			_buffer.update(size);
		}
	}
	catch(...)
	{
		return;
	}
}

void Multicast::quit()
{
	boost::system::error_code code;
	string errMsg;
	_socket.shutdown(ip::udp::socket::shutdown_both, code);
	if (code.value() != 0)
	{
		errMsg = code.message();
	}
	_socket.close(code);
	if (code.value() != 0)
	{
		errMsg = code.message();
	}
	_ioService.stop();

	this->stop();
}


//----------------------------------------
// TCP
//----------------------------------------
bool TCP::connect( const string& ipAddr, unsigned short port )
{
	return true;
}

bool TCP::send( char* data, size_t length )
{
	return true;
}

void TCP::run()
{

}

void TCP::quit()
{
	this->stop();
}


//------------------------------------
// UDP
//------------------------------------
bool UDP::connect( const string& ipAddr, unsigned short port )
{
	return true;
}

bool UDP::send( char* data, size_t length )
{
	return true;
}

void UDP::run()
{

}

void UDP::quit()
{
	this->stop();
}
