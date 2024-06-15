#ifndef __CSERVER_H__
#define __CSERVER_H__

#include<boost/asio.hpp>
#include<memory.h>
#include<map>
#include"CSession.h"

using  boost::asio::ip::tcp;

class CServer {
public:
	CServer(boost::asio::io_context& io_context, short port);
	void ClearSession(std::string& uuid);
private:
	void handleAccept(std::shared_ptr<CSession>, const boost::system::error_code& error);
	void StartAccept();
	boost::asio::io_context& io_context;
	short mPort;
	tcp::acceptor mAcceptor;
	std::map<std::string, std::shared_ptr<CSession>> mSessions;
};

#endif;//__CSERVER_H__
