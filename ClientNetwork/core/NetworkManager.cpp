#include "pch.h"
#include "core/NetworkManager.h"
#include "utils/Utils.h"
#include "utils/Constants.h"
#include "core/SendBuffer.h"
#include "core/RecvBuffer.h"

NetworkManager::NetworkManager()
    : mSocket{ INVALID_SOCKET },
    mId{ NULLID }
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

    // 버퍼 생성 및 쓰레드 생성
    mRecvBuffer = std::make_unique<RecvBuffer>();
    mSendBuffer = std::make_unique<SendBuffer>();

    mSendThread = std::thread{ [=]() { SendWorker(); } };
    mRecvThread = std::thread{ [=]() { RecvWorker(); } };

    return true;
}

void NetworkManager::SetId(UINT8 id)
{
    mId = id;
}

UINT8 NetworkManager::GetId() const
{
    return mId;
}

std::mutex& NetworkManager::GetSendMutex()
{
    return mSendLock;
}

std::mutex& NetworkManager::GetRecvMutex()
{
    return mRecvLock;
}

void NetworkManager::SendWorker()
{
    int sendResult = 0;
    while (true) {
        std::unique_lock lock{ mSendLock };
        mSendConditionVar.wait(lock, [=]() { return mSendBuffer->Empty(); });

        /* TODO send 기능 작성 */
        int dataSize = mSendBuffer->DataSize();
        while (true) {
            if (dataSize < SEND_AT_ONCE) {
                sendResult = ::send(mSocket, mSendBuffer->Buffer(), SEND_AT_ONCE, 0);
                dataSize = 0;
                break;
            }

            sendResult = ::send(mSocket, mSendBuffer->Buffer(), SEND_AT_ONCE, 0);
            dataSize -= SEND_AT_ONCE;
        }

        mSendBuffer->Clean();
    }
}

void NetworkManager::RecvWorker()
{
    int len = 0;
    char buffer[RECV_AT_ONCE];
    while (true) {
        len = ::recv(mSocket, buffer, RECV_AT_ONCE, 0);
        if (len < 0) {
            ErrorHandle::WSAErrorMessageBox("recv function failure");
            break;
        }
        else if (len == 0) {
            break;
        }

        /* TODO recv 기능 작성 */
        std::lock_guard lock{ mRecvLock };
        mRecvBuffer->Write(buffer, len);
    }
}

void NetworkManager::SendChatPacket(std::string_view str)
{
    PacketChatting chat{ sizeof(PacketChatting), PT_CS_PacketChatting, mId, { } };
    if (str.size() > CHAT_PACKET_MAX_SIZE) {
        ErrorHandle::CommonErrorMessageBox("ChatPacketSize Over MaxSize", "...");
        return;
    }
    memcpy(chat.chatBuffer, str.data(), str.size());

    mSendBuffer->Write(&chat, chat.size);
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

void NetworkManager::ReadFromRecvBuffer(RecvBuffer& buffer)
{
    std::lock_guard guard{ mRecvLock };
    buffer = std::move(*mRecvBuffer);
}
