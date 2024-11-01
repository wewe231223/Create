
#include "pch.h"
#include "NetworkFramework.h"

#define SERVERPORT 9000

NetworkFramework::NetworkFramework()
{
}

NetworkFramework::~NetworkFramework()
{
}

void NetworkFramework::InitializeNetwork()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		std::cerr << "WSA initialization failed" << std::endl;
		abort();
	}

	mListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (mListenSocket == INVALID_SOCKET) {
		std::cerr << "Socket creation failed" << std::endl;
		abort();
	}


	sockaddr_in serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERVERPORT);

	int result;
	result = bind(mListenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
	if (result == SOCKET_ERROR) {
		std::cerr << "Socket bind failed" << std::endl;
		abort();
	}

	result = listen(mListenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		std::cerr << "Socket Listening failed" << std::endl;
		abort();
	}

	std::thread mAcceptThread { [this]() { AcceptWorker(); } };
}

void NetworkFramework::AcceptWorker()
{
	std::cout << "working..." << std::endl;
}
