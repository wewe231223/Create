#pragma once

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();

public:
    bool InitializeNetwork(const std::filesystem::path& ipFilePath);

private:
    SOCKET mSocket;
    std::mutex mRecvLock;
    std::mutex mSendLock;

    std::condition_variable mSendConditionVar;

    std::thread mSendThread;
    std::thread mRecvThread;
};