#include "pch.h"
#include "Listener.h"
#include "Utils.h"

void Listener::InitializeNetwork() {
    WSAData data;
    if (0 != ::WSAStartup(MAKEWORD(2, 2), &data)) {
        ErrorHandle::WSAErrorMessageBoxExit("WSAStartup failure");
    }

    mListenSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == mListenSocket) {
        ErrorHandle::WSAErrorMessageBoxExit("socket creation failure");
    }

    mAcceptThread = std::thread{ [=]() { AcceptWorker(); } };
}

void Listener::AcceptWorker() {
    for (int i = 0; i < 100; ++i) {
        std::cout << "Test Accept Worker" << std::endl;
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
