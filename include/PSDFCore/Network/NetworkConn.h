#pragma once

#include "../Common/Common.h"
#include "../Thread/PosixThreads.h"

#include "NetworkBuffer.h"
#include "NetworkDataRecognizer.h"

#define BOOST_DATE_TIME_NO_LIB	/* prevent Boost.asio from linking to Boost.Date_Time lib */
#define BOOST_REGEX_NO_LIB		/* prevent Boost.asio from linking to Boost.Regex lib */

#include <boost/asio.hpp>
#include <boost/bind.hpp>

using namespace boost::asio;

class NetworkCenter;

/*
enum ConnType
{
	CONN_MULTICAST,
	CONN_TCP,
	CONN_UDP,
};
*/

struct NetworkCallback : OnReceiveCallback
{
	virtual int	operator()(char* data, size_t receivedLen, NetworkConn* conn);
};

class PSDF_CORE_DLL_DECL NetworkConn : public PosixThread
{
	friend class NetworkCenter;
	friend struct NetworkCallback;

public:
    NetworkConn(NetworkDataRecognizer * dataRecognizer = NULL);
	virtual ~NetworkConn();

	virtual bool	initConn(const string& ipAddr, unsigned short port) = 0;
	virtual bool	send(unsigned short index, char* data, size_t length) = 0;
	virtual bool	sendTo(char* data, size_t length, const string& ipAddr, unsigned short recvPort) { return false; }
	virtual void	quit() { this->stop(); }

    bool            isSameConn(const string& ipAddr, unsigned short port);
    bool            isSameConn(const NetworkConn& conn);
    void            addDataRecognizer( NetworkDataRecognizer* recg );

	void			outputLog(ofstream& fout);

protected:
	int				_index;
	int				_lastEndpointIndex;

	size_t			_receivedCount;
	size_t			_handledCount;


	vector<NetworkDataRecognizer*>	_dataRecognizers;

	static NetworkBuffer			_buffer;
	static NetworkCallback			_callback;

	//ConnType		_type;
	string			_ip;
	unsigned short	_port;
};

//--------------------------
// Multicast
//--------------------------
class PSDF_CORE_DLL_DECL Multicast : public NetworkConn
{
	friend class NetworkCenter;

private:
	Multicast(NetworkDataRecognizer* dataRecognizer = NULL);

public:
	bool initConn(const string& ipAddr, unsigned short recvPort); // join group
	bool send(unsigned short index, char* data, size_t length);
	void run();
	void quit();

private:
	ip::address		_address;
	unsigned short	_recvPort;
	io_service		_ioService;
	ip::udp::socket	_socket;

};

//--------------------------
// TCP
//--------------------------
class PSDF_CORE_DLL_DECL TCP : public NetworkConn
{
	friend class NetworkCenter;

private:
	TCP(NetworkDataRecognizer* dataRecognizer)
	: NetworkConn(dataRecognizer) {}
	~TCP() {}

public:
	bool initConn(const string& ipAddr, unsigned short port); // connect
	bool send(unsigned short index, char* data, size_t length);
	void run();
	void quit();
};

//--------------------------
// UDP
//--------------------------
class PSDF_CORE_DLL_DECL UDP : public NetworkConn
{
	friend class NetworkCenter;

private:
    UDP(NetworkDataRecognizer* dataRecognizer);
	~UDP() {}

public:
	bool initConn(const string& ipAddr, unsigned short port);
	bool send(unsigned short index, char* data, size_t length);
	void run();
	void quit();

private:
	io_service					_ioService;
	ip::udp::socket*			_socket;
	vector<ip::udp::endpoint*>	_remoteEndpoints;
};