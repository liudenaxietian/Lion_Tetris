#include<iostream>
#include"CServer.h"

int main() {

	try {
		boost::asio::io_context io_context;
		CServer s(io_context, 10086);
		io_context.run();
	}
	catch (std::exception& e) {
		std::cerr << "Expection:" << e.what() << std::endl;
	}

	std::cin.get();

	return 0;
}