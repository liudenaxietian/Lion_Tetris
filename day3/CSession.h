#ifndef __CSESSION_H__
#define __CSESSION_H__
#include<boost/asio.hpp>
#include<boost/uuid/uuid_io.hpp>
#include<boost/uuid/uuid_generators.hpp>
#include<queue>
#include<mutex>
#include<memory>
#include"const.h"

using boost::asio::ip::tcp;


class CServer;

class MsgNode {
	friend class CSession;
public:
	MsgNode(char* msg, short max_length) :mCurLen(0), mTotalLen(MAX_LENGTH + HEAD_LENGTH) {
		mData = new char[mTotalLen+1]();
		//转为网络字节序
		int max_len_host = boost::asio::detail::socket_ops::host_to_network_short(max_length);
		memcpy(mData, &max_len_host, HEAD_LENGTH);
		memcpy(mData + HEAD_LENGTH, msg, max_length);
		mData[mTotalLen] = '\0';
	}

	MsgNode(short max_length) :mTotalLen(max_length), mCurLen(0) {
		mData = new char[mTotalLen + 1]();
	}

	~MsgNode() {
		delete[] mData;
	}

	void Clear() {
		memset(mData, 0, mTotalLen);
		mCurLen = 0;
	}

private:
	short mCurLen;
	short mTotalLen;
	char* mData;
};


class CSession:public std::enable_shared_from_this<CSession> {
public:
	CSession(boost::asio::io_context& io_context, CServer* server);
	~CSession();
	tcp::socket& GetSocket();
	std::string& GetUUID();
	void Start();
	void Send(char* msg, int max_length);
	void Close();
	std::shared_ptr<CSession> SharedSelf();

private:
	void handleRead(const boost::system::error_code& error, size_t bytes_transferred, std::shared_ptr<CSession> shared_self);
	void handleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self);
	tcp::socket mSocket;
	std::string mUUid;
	char mData[MAX_LENGTH];
	CServer* mServer;
	bool mClose;
	std::queue<std::shared_ptr<MsgNode>> mSendQueue;
	std::mutex mSendLock;
	std::shared_ptr<MsgNode> mRecvMsgNode; //收到的消息结构
	bool mHeadParse;
	std::shared_ptr<MsgNode> mRecvHeadNode;//收到的头部结构

};


#endif //__CSESSION_H__