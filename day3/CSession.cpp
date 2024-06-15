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
				//�յ������ݲ���ͷ����С
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
				//��ȡͷ������
				short dataLen = 0;
				memcpy(&dataLen, mRecvHeadNode->mData, HEAD_LENGTH);
				//�����ֽ���תΪ�����ֽ���
				dataLen = boost::asio::detail::socket_ops::network_to_host_short(dataLen);
				std::cout << "data len is: " << dataLen << std::endl;
				if (dataLen > MAX_LENGTH) {
					std::cout << "invalid data length is:" << dataLen << std::endl;
					mServer->ClearSession(mUUid);
					return;
				}
				mRecvMsgNode = std::make_shared<MsgNode>(dataLen);
				//��Ϣ����С��ͷ���涨�����ݳ���,����δ����,���ֽ�������Ϣ�ŵ����ܽڵ�����
				if (bytes_transferred < dataLen) {
					memcpy(mRecvMsgNode->mData + mRecvMsgNode->mCurLen, mData + mCopyLen, bytes_transferred);
					mRecvMsgNode->mCurLen += bytes_transferred;

					memset(mData, 0, MAX_LENGTH);
					mSocket.async_read_some(boost::asio::buffer(mData, MAX_LENGTH), std::bind(&CSession::handleRead, this, std::placeholders::_1, std::placeholders::_2, sharedSelf));
					mHeadParse = true;//ͷ���������,���ٽ�������ͷ
					return;
				}

				memcpy(mRecvMsgNode->mData + mRecvMsgNode->mCurLen, mData + mCopyLen, dataLen);
				mRecvMsgNode->mCurLen += dataLen;
				mCopyLen += dataLen;
				bytes_transferred -= dataLen;
				mRecvMsgNode->mData[mRecvMsgNode->mTotalLen] = '\0';
				std::cout << "receive data is :" << mRecvMsgNode->mData << std::endl;
				//�˴��ɵ���send���Ͳ���;
				Send(mRecvMsgNode->mData, mRecvMsgNode->mTotalLen);
				//��������ʣ��δ���������
				mHeadParse = false;
				mRecvHeadNode->Clear();
				if (bytes_transferred <= 0) {
					memset(mData, 0, MAX_LENGTH);
					mSocket.async_read_some(boost::asio::buffer(mData, MAX_LENGTH), std::bind(&CSession::handleRead, this, std::placeholders::_1, std::placeholders::_2, sharedSelf));
					return;
				}

				continue;
			}
			
			//������ͷ������������δ������
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
			//����
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