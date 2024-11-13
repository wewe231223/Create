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

    void SetId(UINT8 id);
    UINT8 GetId() const;

    std::mutex& GetSendMutex();
    std::mutex& GetRecvMutex();

    void SendWorker();
    void RecvWorker();

    /// <summary>
    /// 이 함수에서 내부 SendBuffer에 대한 어떠한 보호도 해주지 않는다.
    /// Thread Safe하게 사용하기 위해서 해당 함수를 호출하기 전에 lock을 걸어줄것
    /// </summary>
    /// <param name="str"></param>
    void SendChatPacket(std::string_view str);
    void SendPlayerInfoPacket(const DirectX::SimpleMath::Vector3& position);

    void JoinThreads();

    void ReadFromRecvBuffer(class RecvBuffer& buffer);
    void WakeSendThread();

    size_t CheckPackets(char* buffer, size_t len);

private:
    SOCKET mSocket;
    std::mutex mRecvLock;
    std::mutex mSendLock;

    std::condition_variable mSendConditionVar;

    std::thread mSendThread;
    std::thread mRecvThread;

    std::unique_ptr<class SendBuffer> mSendBuffer;
    std::unique_ptr<class RecvBuffer> mRecvBuffer;

    UINT8 mId;
};