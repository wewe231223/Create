#pragma once
class NetworkFramework {
public:
	NetworkFramework();
	~NetworkFramework();
public:
	void InitializeNetwork();
	void AcceptWorker();

private:
	SOCKET			mListenSocket;
	std::thread		mAcceptThread;
};
