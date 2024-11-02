#pragma once




class Client {
public:
	Client();
	~Client();

	bool InitializeClient(SOCKET socket);
	void ShutdownClient();
	BYTE GetId();

	std::mutex& GetRecvMutex();
	std::mutex& GetSendMutex();

	void WakeSendThread();
	void SendWorker();
	void RecvWorker();
	void ReadFromRecvBuffer(class PIStream& stream);

	void SendLoginPass(bool success);
	void SendPlayerExit();
	void SendPlayerEntered();
	void SendPlayerReady(BYTE color);
	void SendGameEnd(bool win);
	void SendPlayerInfo(float hp, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Quaternion& rotationParent, const DirectX::SimpleMath::Quaternion& rotationChild);
	void SendBulletInfo(short bulletIndex, const DirectX::SimpleMath::Vector3& position);


private:
	SOCKET					mSocket;
	char					mIp[INET_ADDRSTRLEN];
	unsigned short			mPort;
	std::thread				mSendThread;
	std::thread				mRecvThread;
	std::mutex				mSendLock;
	std::mutex				mRecvLock;
	std::condition_variable mSendConditionVar;

	

};
