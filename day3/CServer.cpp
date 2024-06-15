#include"CServer.h"
#include<iostream>

CServer::CServer(boost::asio::io_context& io_context, short port) :io_context(io_context), mPort(port),mAcceptor(io_context,tcp::endpoint(tcp::v4(),port)) {
	std::cout << "Server Start success, listen on port:" << mPort << std::endl;
	StartAccept();
}

void CServer::handleAccept(std::shared_ptr<CSession> newSession, const boost::system::error_code& error) {
	if (!error) {
		newSession->Start();
		mSessions.insert(std::make_pair(newSession->GetUUID(), newSession));
	}
	else {
		std::cout << "session accept failed error is:"<<error.what() << std::endl;
	}
	StartAccept();
}


void CServer::StartAccept() {
	std::shared_ptr<CSession> newSession = std::make_shared<CSession>(io_context, this);
	mAcceptor.async_accept(newSession->GetSocket(), std::bind(&CServer::handleAccept, this, newSession, std::placeholders::_1));
}

void CServer::ClearSession(std::string& uuid) {
	mSessions.erase(uuid);
}