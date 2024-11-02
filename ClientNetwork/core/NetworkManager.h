#pragma once

class NetworkManager {
    static constexpr size_t BUFFER_SIZE = std::numeric_limits<unsigned short>::max();
    static constexpr size_t RECV_AT_ONCE = 1024;
    static constexpr size_t SEND_AT_ONCE = 512;

public:
    NetworkManager();
    ~NetworkManager();

public:
    bool InitializeNetwork();
    bool Connect(const std::filesystem::path& ipFilePath);

    void SendWorker();
    void RecvWorker();

    void JoinThreads();

private:
    SOCKET mSocket;
    std::mutex mRecvLock;
    std::mutex mSendLock;

    std::condition_variable mSendConditionVar;

    std::thread mSendThread;
    std::thread mRecvThread;

    std::unique_ptr<class SendBuffer> mSendBuffer;
    std::unique_ptr<class RecvBuffer> mRecvBuffer;
};