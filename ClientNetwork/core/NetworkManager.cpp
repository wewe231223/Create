#include "pch.h"
#include "core/NetworkManager.h"
#include "utils/Utils.h"
#include "utils/Constants.h"

NetworkManager::NetworkManager()
    : mSocket{ INVALID_SOCKET }
{
}

NetworkManager::~NetworkManager()
{
    // JoinThreads 함수를 실행하지 않거나 모종의 이유로 쓰레드가 종료되었을 때 소멸자에서 소켓 및 라이브러리 정리s
    if (INVALID_SOCKET != mSocket) {
        ::shutdown(mSocket, SD_BOTH);
        ::closesocket(mSocket);

        mSocket = INVALID_SOCKET;
        WSACleanup();
    }
}

bool NetworkManager::InitializeNetwork()
{
    WSADATA wsaData;
    if (0 != ::WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        ErrorHandle::WSAErrorMessageBox("WSAStartup failure");
        return false;
    }

    mSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == mSocket) {
        ErrorHandle::WSAErrorMessageBox("socket creation failure");
        return false;
    }

    return true;
}

bool NetworkManager::Connect(const std::filesystem::path& ipFilePath)
{
    std::ifstream ipFile{ ipFilePath };
    if (not ipFile) {
        ErrorHandle::CommonErrorMessageBox("ipFile loading failure", "ip file not exists");
        return false;
    }

    std::string ip;
    ipFile >> ip;

    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = ::htons(Global::PORT);

    if (0 > ::inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr)) {
        ErrorHandle::WSAErrorMessageBox("ip address binding failure");
        return false;
    }

    if (SOCKET_ERROR == ::connect(mSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(sockaddr))) {
        ErrorHandle::WSAErrorMessageBox("connect to server failure");
        return false;
    }

    mSendThread = std::thread{ [=]() { } };
    mRecvThread = std::thread{ [=]() { } };

    return true;
}

void NetworkManager::JoinThreads()
{
    if (mSendThread.joinable()) {
        mSendThread.join();
    }

    if (mRecvThread.joinable()) {
        mRecvThread.join();
    }

    ::shutdown(mSocket, SD_BOTH);
    ::closesocket(mSocket);

    mSocket = INVALID_SOCKET;
    WSACleanup();
}
