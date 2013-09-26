#include "../Common/Common.h"
#include "../Thread/PosixThreads.h"
#include "NetworkDataRecognizer.h"
#include "NetworkBuffer.h"

#define BOOST_DATE_TIME_NO_LIB	/* prevent Boost.asio from linking to Boost.Date_Time lib */
#define BOOST_REGEX_NO_LIB		/* prevent Boost.asio from linking to Boost.Regex lib */

#include <boost/asio.hpp>
#include <boost/bind.hpp>

class NetworkCenter;

class PSDF_CORE_DLL_DECL NetworkConn : public PosixThread
{
	friend class NetworkCenter;

public:
	NetworkConn(NetworkDataRecognizer* dataRecognizer = NULL);
	virtual ~NetworkConn();

	virtual bool	send(char* data, size_t length) = 0;
	virtual void	quit() { this->stop(); }

	static int		onReceive(char* data, size_t receivedLen, NetworkConn* conn);

	void			outputLog(ofstream& fout);

protected:
	int						_index;
	size_t					_receivedCount;
	size_t					_handledCount;

	NetworkDataRecognizer*	_dataRecognizer;
	NetworkBuffer			_buffer;
	char*					_directBuffer;
};

//--------------------------
// Multicast
//--------------------------
class PSDF_CORE_DLL_DECL Multicast : public NetworkConn
{
	friend class NetworkCenter;

private:
	Multicast(const string& ipAddr, unsigned short recvPort, NetworkDataRecognizer* dataRecognizer = NULL);

public:
	bool joinGroup(const string& ipAddr, unsigned short recvPort);
	bool send(char* data, size_t length);
	void run();
	void quit();

private:
	boost::asio::ip::address		_address;
	unsigned short					_recvPort;
	boost::asio::io_service			_ioService;
	boost::asio::ip::udp::socket	_socket;
};

//--------------------------
// TCP
//--------------------------
class PSDF_CORE_DLL_DECL TCP : public NetworkConn
{
	friend class NetworkCenter;

private:
	TCP(const string& ipAddr, unsigned short port, NetworkDataRecognizer* dataRecognizer) {TOUCH(ipAddr); TOUCH(port); TOUCH(dataRecognizer);}
	~TCP() {}

public:
	bool connect(const string& ipAddr, unsigned short port);
	bool send(char* data, size_t length);
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
	UDP() {}
	~UDP() {}

public:
	bool connect(const string& ipAddr, unsigned short port);
	bool send(char* data, size_t length);
	void run();
	void quit();
};