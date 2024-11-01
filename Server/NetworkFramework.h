#pragma once
class NetworkFramework
{
public:
	void InitializeNetwork();
	void AcceptWorker();

private:
	SOCKET			mListenSocket;
	std::thread		mAcceptThread;
};
