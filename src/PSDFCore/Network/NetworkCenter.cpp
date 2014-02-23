#include "StdAfx.h"

#include <PSDFCore/Network/NetworkCenter.h>
#include <PSDFCore/Data/DataCenter.h>
#include <PSDFCore/Event/EventCenter.h>
#include <fstream>

using namespace boost::asio;

NetworkCenter::NetworkCenter()
{
}

NetworkCenter::~NetworkCenter()
{
	for (unsigned int i = 0; i < _connections.size(); ++i)
	{
		if (_connections[i]) { _connections[i]->quit(); }
	}

	for (unsigned int i = 0; i < _connections.size(); ++i)
	{
		if (_connections[i])
		{
			_connections[i]->waitUntilDone();
			delete _connections[i];
		}
	}
}

NetworkCenter* NetworkCenter::inst()
{
	static NetworkCenter instance;
	return &instance;
}

int NetworkCenter::createMulticast( const string& ipAddr, unsigned short port, NetworkDataRecognizer* dataRecognizer )
{
    int index = isExist(ipAddr, port);
    if (index != -1)
	{
        _connections[index]->addDataRecognizer(dataRecognizer);
        return index;
	}

	index = _connections.size();

	NetworkConn* conn = new Multicast(dataRecognizer);
	conn->initConn(ipAddr, port);
	_connections.push_back(conn);
	conn->_index = index;

	return index;
}

int NetworkCenter::createTcpClient( const string& remoteIP, unsigned short remotePort, NetworkDataRecognizer* dataRecognizer )
{
    int index = isExist(remoteIP, remotePort);
    if (index != -1)
	{
        _connections[index]->addDataRecognizer(dataRecognizer);
        return index;
	}

	index = _connections.size();
	index <<= 16;


	NetworkConn* conn = new TCP(dataRecognizer);
	conn->initConn(remoteIP, remotePort);
	_connections.push_back(conn);
	conn->_index = index;

	return index;
}

void NetworkCenter::createTcpServer( unsigned short localPort, NetworkDataRecognizer* dataRecognizer )
{
	string remoteIP = "0.0.0.0";
    int index = isExist(remoteIP, localPort);
	if (index != -1)
	{
		_connections[index]->addDataRecognizer(dataRecognizer);
	}

	index = _connections.size();
	index <<= 16;

	NetworkConn* conn = new TCP(dataRecognizer);
	conn->initConn("0.0.0.0", localPort);
	_connections.push_back(conn);
	conn->_index = index;
}

int NetworkCenter::createUdpClient( const string& remoteIP, unsigned short remotePort, NetworkDataRecognizer* dataRecognizer )
{
    int index = isExist(remoteIP, remotePort);
    if (index != -1)
	{
        _connections[index]->addDataRecognizer(dataRecognizer);
        return index;
	}

	index = _connections.size();
	index <<= 16;

	NetworkConn* conn = new UDP(dataRecognizer);
	conn->initConn(remoteIP, remotePort);
	_connections.push_back(conn);
	conn->_index = index;

	return index;
}

void NetworkCenter::createUdpServer( unsigned short localPort, NetworkDataRecognizer* dataRecognizer )
{
	string remoteIP = "0.0.0.0";
	int index = isExist(remoteIP, localPort);
	if (index != -1)
	{
		_connections[index]->addDataRecognizer(dataRecognizer);
	}

	index = _connections.size();
	index <<= 16;

	NetworkConn* conn = new UDP(dataRecognizer);
	conn->initConn(remoteIP, localPort);
	_connections.push_back(conn);
	conn->_index = index;
}

void NetworkCenter::send( unsigned int index, char* data, size_t length )
{
	unsigned int connIndex = (index >> 16) & 0x0000ffff;
	if (connIndex >= _connections.size()) { return; }

	NetworkConn* conn = _connections[connIndex];
	conn->send((unsigned short)(index & 0x0000ffff), data, length);
}

void NetworkCenter::outputLog(ofstream& fout)
{
	for (unsigned int i = 0; i < _connections.size(); ++i)
	{
		fout << "Connection " << i << ":\n";
		_connections[i]->outputLog(fout);
	}
}

int NetworkCenter::isExist( const string& ipAddr, unsigned short port )
{
    for (unsigned int i = 0; i < _connections.size(); ++i)
	{
        if (_connections[i]->isSameConn(ipAddr, port))
		{
            return i;
		}
	}
    return -1;
}
