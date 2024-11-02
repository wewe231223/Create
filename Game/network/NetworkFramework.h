#pragma once 

class NetworkFramework {
	static constexpr size_t BUFFER_SIZE = std::numeric_limits<unsigned short>::max();
	static constexpr short PORT = 9000;
public:
	NetworkFramework();
	~NetworkFramework();
public:
	bool InitializeNetwork();
	bool Connect(const fs::path& ipFilePath);

	void RecvWorker();
	void SendWorker();

	void ShutDownRecvWorker();
	void ShutDownSendWorker();

	// 11.02 매개변수 변경 : const char* logIn		-> const std::string& logIn
	// 11.02 매개변수 변경 : const char* password	-> const std::string& password
	void SendLoginPacket(const std::string& logIn, const std::string& password);

	void SendPlayerReady(BYTE color);
	void SendRequestFire(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction);
	void SendKeyInput(BYTE key, bool down);
	void SendPlayerInfo(const DirectX::SimpleMath::Quaternion& roationChild);
private:
	SOCKET mSocket{};

	std::condition_variable mRecvCV;
	std::thread mRecvThread;
	std::thread mSendThread;

	std::array<char, BUFFER_SIZE> mRecvBuffer{};
	std::array<char, BUFFER_SIZE> mSendBuffer{};
};

extern NetworkFramework gNetworkFramework;