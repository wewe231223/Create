#include "pch.h"
#include "NetworkFramework.h"

#define SERVERPORT 9000

void NetworkFramework::InitializeNetwork()
{
	int result;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		std::cerr << "WSA initialization failed" << std::endl;
		exit(0);
	}

	mListenSocket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERVERPORT);

	result = bind(mListenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR) {
		std::cerr << "Socket bind failed" << std::endl;
		exit(0);
	}

	result = listen(mListenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		std::cerr << "Socket Listening failed" << std::endl;
		exit(0);
	}


	try {
		mAcceptThread = std::thread(&NetworkFramework::AcceptWorker, this);
		mAcceptThread.join();
	}
	catch (const std::system_error& err) {
		std::cerr << "Failed to create thread : " << err.what() << std::endl;
	}





}

void NetworkFramework::AcceptWorker()
{
}
