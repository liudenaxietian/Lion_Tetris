#include<iostream>
#include<boost/asio.hpp>
#include<thread>

using namespace boost::asio::ip;

const int MAX_LENGTH = 1024 * 2;
const int HEAD_LENGTH = 2;

int main() {
	try {
		boost::asio::io_context ioc;
		tcp::endpoint remote_ep(address::from_string("127.0.0.1"), 10086);
		tcp::socket sock(ioc);
		boost::system::error_code error = boost::asio::error::host_not_found;
		sock.connect(remote_ep, error);
		if (error) {
			std::cout << "connect failed,code is " << error.what() << " error msg is " << error.message();
			return 0;
		}
		//�����߳�
		std::thread send_thread([&sock] {
			for (;;) {
				std::this_thread::sleep_for(std::chrono::milliseconds(2));
				const char* request = "hello world!";
				short request_length = strlen(request);
				char sendData[MAX_LENGTH] = { 0 };
				//תΪ�����ֽ���
				short request_host_len = boost::asio::detail::socket_ops::host_to_network_short(request_length);
				memcpy(sendData, &request_host_len, HEAD_LENGTH);
				memcpy(sendData + HEAD_LENGTH, request, request_length);
				boost::asio::write(sock, boost::asio::buffer(sendData, request_length + 2));
				std::cout << "send data: " << sendData <<std::endl;
			}
			});
		//�����߳�
		std::thread recv_thread([&sock] {
			for (;;) {
				std::this_thread::sleep_for(std::chrono::milliseconds(2));
				std::cout << "begin to receive: " << std::endl;
				char replyHead[HEAD_LENGTH];
				size_t replyLen = boost::asio::read(sock, boost::asio::buffer(replyHead, HEAD_LENGTH));
				short msgLen = 0;
				memcpy(&msgLen, &replyHead, HEAD_LENGTH);
				//תΪ�����ֽ���
				msgLen = boost::asio::detail::socket_ops::network_to_host_short(msgLen);
				char msg[MAX_LENGTH] = { 0 };
				size_t msgLength = boost::asio::read(sock, boost::asio::buffer(msg, msgLen));
				std::cout << "Reply is: ";
				std::cout.write(msg, msgLen) << std::endl;
				std::cout << "Reply len is " << msgLen;
				std::cout << "\n";
			}
			});

		send_thread.join();
		recv_thread.join();

	}
	catch (std::exception& e) {
		std::cerr << "Expection:" << e.what() << std::endl;
	}

	return 0;
}