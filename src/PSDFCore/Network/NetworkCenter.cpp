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

int NetworkCenter::createMulticast( const string& ipAddr, short port, NetworkDataRecognizer* dataRecognizer )
{
	int index = _connections.size();

	NetworkConn* conn = new Multicast(ipAddr, port, dataRecognizer);
	_connections.push_back(conn);
	conn->_index = index;

	return index;
}

int NetworkCenter::createTcp( const string& ipAddr, short port, NetworkDataRecognizer* dataRecognizer )
{
	int index = _connections.size();

	NetworkConn* conn = new TCP(ipAddr, port, dataRecognizer);
	_connections.push_back(conn);
	conn->_index = index;

	return index;
}

int NetworkCenter::createUdp( const string& ipAddr, short port, NetworkDataRecognizer* dataRecognizer )
{
	int index = _connections.size();

	NetworkConn* conn = new UDP;
	_connections.push_back(conn);
	conn->_index = index;

	return index;
}

void NetworkCenter::send( unsigned int index, char* data, size_t length )
{
	if (index >= _connections.size()) { return; }

	NetworkConn* conn = _connections[index];
	conn->send(data, length);
}

void NetworkCenter::outputLog(ofstream& fout)
{
	for (unsigned int i = 0; i < _connections.size(); ++i)
	{
		fout << "Connection " << i << ":\n";
		_connections[i]->outputLog(fout);
	}
}



