#include "pch.h"
#include "Server/core/Listener.h"
#include "Server/utils/Utils.h"

constexpr unsigned short SERVER_PORT = 25000;

void Listener::InitializeNetwork() {
    WSAData data;
    if (0 != ::WSAStartup(MAKEWORD(2, 2), &data)) {
        ErrorHandle::WSAErrorMessageBoxExit("WSAStartup failure");
    }

    mListenSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == mListenSocket) {
        ErrorHandle::WSAErrorMessageBoxExit("socket creation failure");
    }

    sockaddr_in serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
    serverAddr.sin_port = ::htons(SERVER_PORT);

    if (SOCKET_ERROR == ::bind(mListenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr))) {
        ErrorHandle::WSAErrorMessageBoxExit("socket creation failure");
    }

    if (SOCKET_ERROR == ::listen(mListenSocket, SOMAXCONN)) {
        ErrorHandle::WSAErrorMessageBoxExit("socket creation failure");
    }
        
    mAcceptThread = std::thread{ [=]() { AcceptWorker(); } };
}

void Listener::AcceptWorker() {
    while (true) {
        sockaddr_in clientAddr;
        int addrLen{ sizeof(clientAddr) };
        ::memset(&clientAddr, 0, sizeof(clientAddr));
        SOCKET clientSocket = ::accept(mListenSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);
        if (INVALID_SOCKET == clientSocket) {
            ErrorHandle::WSAErrorMessageBoxExit("accept failure");
        }

        /* TODO... -> 클라이언트 생성 코드 필요함 지금은 그냥 shutdown 하기 */
        ::shutdown(clientSocket, SD_BOTH);
        ::closesocket(clientSocket);
    }
}

void Listener::ShutdownAccept() { 
    ::WSACleanup();
    ::closesocket(mListenSocket);
}

void Listener::JoinAcceptWorker() {
    if (mAcceptThread.joinable()) {
        mAcceptThread.join();
    }
}
