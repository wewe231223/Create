#pragma once

class NetworkManager {
    static constexpr size_t BUFFER_SIZE = std::numeric_limits<unsigned short>::max();

public:
    NetworkManager();
    ~NetworkManager();

public:
    bool InitializeNetwork();
    bool Connect(const std::filesystem::path& ipFilePath);

    void JoinThreads();

private:
    SOCKET mSocket;
    std::mutex mRecvLock;
    std::mutex mSendLock;

    std::condition_variable mSendConditionVar;

    std::thread mSendThread;
    std::thread mRecvThread;
};