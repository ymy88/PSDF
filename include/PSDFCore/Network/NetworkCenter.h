#pragma once

#include "../Common/Common.h"
#include "NetworkConn.h"
#include "NetworkDataRecognizer.h"

class PSDF_CORE_DLL_DECL NetworkCenter
{
private:
	
private:
	NetworkCenter();
	NetworkCenter(const NetworkCenter&){}
	~NetworkCenter();

public:
	static NetworkCenter*	inst();

	int						createMulticast(const string& groupIP, unsigned short port, NetworkDataRecognizer* dataRecognizer);
	int						createTcpClient(const string& remoteIP, unsigned short remotePort, NetworkDataRecognizer* dataRecognizer);
	void					createTcpServer(unsigned short localPort, NetworkDataRecognizer* dataRecognizer);
	int						createUdpClient(const string& remoteIP, unsigned short remotePort, NetworkDataRecognizer* dataRecognizer);
	void					createUdpServer(unsigned short localPort, NetworkDataRecognizer* dataRecognizer);

    int                     isExist( const string& ipAddr, unsigned short port );
	void					send(unsigned int index, char* data, size_t length);

	void					outputLog(ofstream& fout);

private:
	vector<NetworkConn*>	_connections;
};
