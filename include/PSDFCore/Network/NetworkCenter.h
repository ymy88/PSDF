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

	int						createMulticast(const string& ipAddr, short port, NetworkDataRecognizer* dataRecognizer);
	int						createTcp(const string& ipAddr, short port, NetworkDataRecognizer* dataRecognizer);
	int						createUdp(const string& ipAddr, short port, NetworkDataRecognizer* dataRecognizer);

	void					send(unsigned int index, char* data, size_t length);

	void					outputLog(ofstream& fout);

private:
	vector<NetworkConn*>	_connections;
};
