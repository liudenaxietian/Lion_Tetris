#include"CSession.h"
#include"CServer.h"
#include<iostream>

CSession::CSession(boost::asio::io_context& io_context, CServer* server) :mSocket(io_context),mServer(server),mClose(false),mHeadParse(false) {
	boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
	mUUid = boost::uuids::to_string(a_uuid);
	mRecvHeadNode = std::make_shared<MsgNode>(HEAD_LENGTH);
}

CSession::~CSession() {
	std::cout << "CSession destruct" << std::endl;
}


tcp::socket& CSession::GetSocket() {
	return mSocket;
}

std::string& CSession::GetUUID() {
	return mUUid;
}

void CSession::Start() {
	memset(mData, 0, MAX_LENGTH);
	mSocket.async_read_some(boost::asio::buffer(mData, MAX_LENGTH), std::bind(&CSession::handleRead, this, std::placeholders::_1, std::placeholders::_2, SharedSelf()));
	
}

void CSession::Send(char* msg, int max_length) {
	std::lock_guard <std::mutex> lock(mSendLock);
	int send_que_size = mSendQueue.size();
	if (send_que_size > MAX_SENDQUE) {
		std::cout << "session: " << mUUid << " send que failed,size is" << send_que_size << std::endl;
		return;
	}

	mSendQueue.push(std::make_shared<MsgNode>(msg, max_length));
	if (send_que_size > 0) {

		return;
	}

	auto& msgNode = mSendQueue.front();
	boost::asio::async_write(mSocket, boost::asio::buffer(msgNode->mData, msgNode->mTotalLen), std::bind(&CSession::handleWrite, this, std::placeholders::_1, SharedSelf()));
}

void CSession::Close(){
	mSocket.close();
	mClose = true;
}


std::shared_ptr<CSession> CSession::SharedSelf() {
	return shared_from_this();
}


void CSession::handleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> sharedSelf) {
	if (!error) {
		std::lock_guard<std::mutex> lock(mSendLock);
		std::cout << "send data: " << mSendQueue.front()->mData + HEAD_LENGTH << std::endl;
		mSendQueue.pop();
		if (!mSendQueue.empty()) {
			auto& msgNode = mSendQueue.front();
			boost::asio::async_write(mSocket, boost::asio::buffer(msgNode->mData, msgNode->mTotalLen), std::bind(&CSession::handleWrite, this, std::placeholders::_1, sharedSelf));
		}
	}
	else {
		std::cout << "handle write failed, error is " << error.what() << std::endl;
		Close();
		mServer->ClearSession(mUUid);
	}
}


void CSession::handleRead(const boost::system::error_code& error, size_t bytes_transferred, std::shared_ptr<CSession> sharedSelf) {
	if (!error) {
		int mCopyLen = 0;
		while (bytes_transferred > 0) {
			if (!mHeadParse) {
				//收到的数据不足头部大小
				if (bytes_transferred + mRecvHeadNode->mCurLen < HEAD_LENGTH) {
					memcpy(mRecvHeadNode->mData+mRecvHeadNode->mCurLen, mData+mCopyLen, bytes_transferred);
					mRecvHeadNode->mCurLen += bytes_transferred;
					memset(mData, 0, MAX_LENGTH);
					mSocket.async_read_some(boost::asio::buffer(mData,MAX_LENGTH), std::bind(&CSession::handleRead, this, std::placeholders::_1, std::placeholders::_2, sharedSelf));
					return;
				}
				int headRemain = HEAD_LENGTH - mRecvHeadNode->mCurLen;
				memcpy(mRecvHeadNode->mData + mRecvHeadNode->mCurLen, mData + mCopyLen, headRemain);
				mCopyLen += headRemain;
				bytes_transferred -= headRemain;
				//获取头部数据
				short dataLen = 0;
				memcpy(&dataLen, mRecvHeadNode->mData, HEAD_LENGTH);
				//网络字节序转为本机字节序
				dataLen = boost::asio::detail::socket_ops::network_to_host_short(dataLen);
				std::cout << "data len is: " << dataLen << std::endl;
				if (dataLen > MAX_LENGTH) {
					std::cout << "invalid data length is:" << dataLen << std::endl;
					mServer->ClearSession(mUUid);
					return;
				}
				mRecvMsgNode = std::make_shared<MsgNode>(dataLen);
				//消息长度小于头部规定的数据长度,数据未收完,则现将部分消息放到接受节点里面
				if (bytes_transferred < dataLen) {
					memcpy(mRecvMsgNode->mData + mRecvMsgNode->mCurLen, mData + mCopyLen, bytes_transferred);
					mRecvMsgNode->mCurLen += bytes_transferred;

					memset(mData, 0, MAX_LENGTH);
					mSocket.async_read_some(boost::asio::buffer(mData, MAX_LENGTH), std::bind(&CSession::handleRead, this, std::placeholders::_1, std::placeholders::_2, sharedSelf));
					mHeadParse = true;//头部处理完成,不再解析数据头
					return;
				}

				memcpy(mRecvMsgNode->mData + mRecvMsgNode->mCurLen, mData + mCopyLen, dataLen);
				mRecvMsgNode->mCurLen += dataLen;
				mCopyLen += dataLen;
				bytes_transferred -= dataLen;
				mRecvMsgNode->mData[mRecvMsgNode->mTotalLen] = '\0';
				std::cout << "receive data is :" << mRecvMsgNode->mData << std::endl;
				//此处可调用send发送测试;
				Send(mRecvMsgNode->mData, mRecvMsgNode->mTotalLen);
				//继续处理剩余未处理的数据
				mHeadParse = false;
				mRecvHeadNode->Clear();
				if (bytes_transferred <= 0) {
					memset(mData, 0, MAX_LENGTH);
					mSocket.async_read_some(boost::asio::buffer(mData, MAX_LENGTH), std::bind(&CSession::handleRead, this, std::placeholders::_1, std::placeholders::_2, sharedSelf));
					return;
				}

				continue;
			}
			
			//处理完头部，但是数据未接收完
			int remainMsg = mRecvMsgNode->mTotalLen - mRecvMsgNode->mCurLen;
			if (bytes_transferred < remainMsg) {
				memcpy(mRecvMsgNode->mData + mRecvMsgNode->mCurLen, mData + mCopyLen, bytes_transferred);
				mRecvMsgNode->mCurLen += bytes_transferred;
				memset(mData, 0, MAX_LENGTH);
				mSocket.async_read_some(boost::asio::buffer(mData, MAX_LENGTH), std::bind(&CSession::handleRead, this, std::placeholders::_1, std::placeholders::_2, sharedSelf));
				return;
			}
			memcpy(mRecvMsgNode->mData + mRecvMsgNode->mCurLen, mData + mCopyLen, remainMsg);
			mRecvMsgNode->mCurLen += remainMsg;
			bytes_transferred -= remainMsg;
			mCopyLen += remainMsg;
			mRecvMsgNode->mData[mRecvMsgNode->mTotalLen] = '\0';
			std::cout << "receive data is :" << mRecvMsgNode->mData << std::endl;
			//测试
			Send(mRecvMsgNode->mData, mRecvMsgNode->mTotalLen);

			mHeadParse = false;
			mRecvHeadNode->Clear();
			if (bytes_transferred <= 0) {
				memset(mData, 0, MAX_LENGTH);
				mSocket.async_read_some(boost::asio::buffer(mData, MAX_LENGTH), std::bind(&CSession::handleRead, this, std::placeholders::_1, std::placeholders::_2, sharedSelf));
				return;
			}
			continue;
		}
	}
	else {
		std::cout << "handle read failed, error is " << error.what() << std::endl;
		Close();
		mServer->ClearSession(mUUid);
	}
}