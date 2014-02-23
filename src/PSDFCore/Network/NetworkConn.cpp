#include "StdAfx.h"


#include <PSDFCore/Network/NetworkCenter.h>
#include <PSDFCore/Data/DataCenter.h>
#include <PSDFCore/Event/EventCenter.h>
#include <PSDFCore/Network/NetworkConn.h>
#include <fstream>

int	NetworkCallback::operator()(char* data, size_t receivedLen, NetworkConn* conn)
{
	unsigned int eventType;
	size_t dataLen = 0;
    bool isRecged = false;
    for (unsigned int i = 0; i < conn->_dataRecognizers.size(); ++i)
	{
        if (conn->_dataRecognizers[i]->recognize(data, dataLen, eventType))
		{
            isRecged = true;
            break;
		}
	}
    if (!isRecged)
	{
        return 0;
	}
	if (eventType != 0 && receivedLen >= dataLen && dataLen > 0)
	{
		int dataIndex = DataCenter::inst()->addData(data, dataLen);
		int connIndex = conn->_index | (conn->_lastEndpointIndex & 0x0000ffff);
		EventCenter::inst()->triggerEvent(eventType, connIndex, dataIndex);
		conn->_handledCount++;
	}

	return dataLen;
}




//----------------------------------------
// NetConnection
//----------------------------------------
NetworkCallback NetworkConn::_callback;
NetworkBuffer NetworkConn::_buffer(&_callback);
NetworkConn::NetworkConn(NetworkDataRecognizer* dataRecognizer)
{
	_receivedCount = 0;
	_handledCount = 0;

	_buffer.setNetworkConn(this);
	if (dataRecognizer != NULL)
	{
		dataRecognizer->retain();
        _dataRecognizers.push_back(dataRecognizer);
		_buffer.start();
	}
}

bool NetworkConn::isSameConn(const string& ipAddr, unsigned short port)
{
	return (_ip == ipAddr && _port == port);
}

bool NetworkConn::isSameConn(const NetworkConn& conn)
{
	return (_ip == conn._ip && _port == conn._port);
}

NetworkConn::~NetworkConn()
{
	if (_dataRecognizers.size() > 0)
	{
        for (unsigned int i = 0; i < _dataRecognizers.size(); ++i)
		{
            _dataRecognizers[i]->release();
		}
		_buffer.stop();
	}
}

void NetworkConn::outputLog( ofstream& fout )
{
	fout << "    Received count: " << _receivedCount << endl
		 << "    Handled count: " << _handledCount << endl;
}

void NetworkConn::addDataRecognizer( NetworkDataRecognizer* recg )
{
    for (unsigned int i = 0; i < _dataRecognizers.size(); ++i)
	{
        if (_dataRecognizers[i] == recg)
		{
            return;
		}
	}
    recg->retain();
    _dataRecognizers.push_back(recg);
}


//-------------------------------------
// Multicast
//-------------------------------------
Multicast::Multicast(NetworkDataRecognizer* dataRecognizer ) : NetworkConn(dataRecognizer), _socket(_ioService)
{
}

bool Multicast::initConn( const string& ipAddr, unsigned short recvPort )
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

		_ip = ipAddr;
		_port = recvPort;

		this->start();

		return true;
	}
	catch(...)
	{
		return false;
	}
}

bool Multicast::send( unsigned short index, char* data, size_t length )
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
	//char* directBuffer = new char[1 << 20];
	//mutable_buffers_1 tmpBuffer(_buffer.getPutPos(), _buffer.getLength());  //  buffer(directBuffer, 1 << 20);
	try
	{
		while (true)
		{
			mutable_buffers_1 tmpBuffer(_buffer.getPutPos(), _buffer.getLength());
			size_t size = _socket.receive(tmpBuffer);
			++_receivedCount;
			//memcpy(_buffer.getPutPos(), directBuffer, size);
			_buffer.update(size);
		}
		//delete [] directBuffer;
	}
	catch(...)
	{
		//delete [] directBuffer;
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
bool TCP::initConn( const string& ipAddr, unsigned short port )
{
	_ip = ipAddr;
	_port = port;
	return true;
}

bool TCP::send( unsigned short index, char* data, size_t length )
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
UDP::UDP(NetworkDataRecognizer* dataRecognizer)
: NetworkConn(dataRecognizer), _socket(NULL)
{}

bool UDP::initConn( const string& ipAddr, unsigned short port )
{
	if (ipAddr == "0.0.0.0")
	{
		// server
		if (_socket && _socket->local_endpoint().port() != port)
		{
			_socket->shutdown(ip::udp::socket::shutdown_both);
			_socket->close();
			this->stop();
			this->waitUntilDone();
			delete _socket;
		}
		_socket = new ip::udp::socket(_ioService, ip::udp::endpoint(ip::udp::v4(), port));
		this->start();
	}
	else // client
	{
		if (!_socket)
		{
			_socket = new ip::udp::socket(_ioService);
			_socket->open(ip::udp::v4());
			this->start();
		}
		if (_remoteEndpoints.size() > 0)
		{
			for (unsigned int i = 0; i < _remoteEndpoints.size(); ++i)
			{
				delete _remoteEndpoints[i];
			}
		}
		_remoteEndpoints.clear();

		ip::address address = ip::address::from_string(ipAddr);
		ip::udp::endpoint* ep = new ip::udp::endpoint(address, port);
		_remoteEndpoints.push_back(ep);
	}

	_ip = ipAddr;
	_port = port;
	return true;
}

bool UDP::send( unsigned short index, char* data, size_t length )
{
	try
	{
		_socket->send_to(boost::asio::buffer(data, length), *_remoteEndpoints[index]);
		return true;
	}
	catch(...)
	{
		return false;
	}
}

void UDP::run()
{
	char* directBuffer = new char[1 << 20];
	mutable_buffers_1 tmpBuffer = buffer(directBuffer, 1 << 20);

	ip::udp::endpoint* remoteEndpoint = new ip::udp::endpoint;
	boost::system::error_code err;
	try
	{
		while (true)
		{
			size_t size = _socket->receive_from(tmpBuffer, *remoteEndpoint, 0, err);
			if (err && err != error::message_size)
			{
				break;
			}
			bool isNewEndpoint = true;
			for (unsigned int i = 0; i < _remoteEndpoints.size(); ++i)
			{
				if (_remoteEndpoints[i]->address() == remoteEndpoint->address() &&
					_remoteEndpoints[i]->port() == remoteEndpoint->port())
				{
					isNewEndpoint = false;
					_lastEndpointIndex = i;
					break;
				}
			}
			if (isNewEndpoint)
			{
				_remoteEndpoints.push_back(remoteEndpoint);
				_lastEndpointIndex = _remoteEndpoints.size() - 1;
				remoteEndpoint = new ip::udp::endpoint;
			}

			++_receivedCount;
			memcpy(_buffer.getPutPos(), directBuffer, size);
			_buffer.update(size);
		}

		delete remoteEndpoint;
		delete [] directBuffer;
	}
	catch(...)
	{
		delete remoteEndpoint;
		delete [] directBuffer;
	}
}

void UDP::quit()
{
	boost::system::error_code code;
	string errMsg;
	_socket->shutdown(ip::udp::socket::shutdown_both, code);
	if (code.value() != 0)
	{
		errMsg = code.message();
	}
	_socket->close(code);
	if (code.value() != 0)
	{
		errMsg = code.message();
	}
	_ioService.stop();

	this->stop();
}
