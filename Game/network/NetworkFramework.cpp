#include "pch.h"
#include "Game/network/NetworkFramework.h"
#include "Game/network/ErrorHandler.h"


NetworkFramework::NetworkFramework()
{
}

NetworkFramework::~NetworkFramework()
{
	// Recv Thread, Send Thread 종료
	ShutDownRecvWorker();
	ShutDownSendWorker();

	// Graceful shutdown
	// linger option을 이용하여 송수신이 완료될 때까지 대기
	// linger option을 이용하지 않으면 송신은 완료되지만 수신은 완료되지 않은 상태로 소켓이 닫힘
	// 이 경우 수신이 완료되지 않은 데이터는 손실됨
	linger lingerStruct{ 1, 0 };
	setsockopt(mSocket, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&lingerStruct), sizeof(linger));
	shutdown(mSocket, SD_BOTH);
	
	// Socket 닫기 및 Winsock 종료
	closesocket(mSocket);
	WSACleanup();
}


bool NetworkFramework::InitializeNetwork()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		ErrorHandle::WSAErrorMessageBox("WSA initialization failed");
		return false;
	}

	mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (mSocket == INVALID_SOCKET) {
		WSACleanup();
		ErrorHandle::WSAErrorMessageBox("Socket creation failed");
		return false;
	}

	return true;
}


bool NetworkFramework::Connect(const fs::path& ipFilePath)
{
	sockaddr_in serveraddr{};
	ZeroMemory(&serveraddr, sizeof(sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT);

	std::string serverIp{};
	std::ifstream ipFile{ ipFilePath };

	if (not ipFile) {
		closesocket(mSocket);
		WSACleanup();
		ErrorHandle::WSAErrorMessageBox("ip file open failed");
		return false;
	}

	ipFile >> serverIp;

	constexpr INT inet_pton_success = 1;

	if (inet_pton(AF_INET, serverIp.c_str(), &serveraddr.sin_addr) != inet_pton_success) {
		closesocket(mSocket);
		WSACleanup();
		ErrorHandle::WSAErrorMessageBox("inet_pton failed");
		return false;
	};

	if (connect(mSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		closesocket(mSocket);
		WSACleanup();
		ErrorHandle::WSAErrorMessageBox("connect failed");
		return false;
	}

	mRecvThread = std::thread{ [this]() { RecvWorker(); } };
	mSendThread = std::thread{ [this]() { SendWorker(); } };

	return true;
}	

void NetworkFramework::RecvWorker()
{

}

void NetworkFramework::SendWorker()
{
	//while (true) {
	//	std::unique_lock lock{ sendLock }; // send mutex의 락을 점유하려고 시도 -> 점유하는 경우 다음 스텝
	//	sendConditionVar.wait(lock, [] { return false == fq.empty(); }); // 조건을 확인하고 만족하지 않으면 unlock하고 대기
	//	//  notify 신호를 받고 깨어나면 조건을 확인하고 lock을 점유	
	//	while (false == fq.empty()) {
	//		float f = fq.front();
	//		fq.pop();
	//		// send
	//	}
	//}
}

void NetworkFramework::ShutDownRecvWorker()
{

}

void NetworkFramework::ShutDownSendWorker()
{
	if (mSendThread.joinable()) {
		mSendThread.join();
	}
}

void NetworkFramework::SendLoginPacket(const std::string& logIn, const std::string& password)
{
}

void NetworkFramework::SendPlayerReady(BYTE color)
{
}

void NetworkFramework::SendRequestFire(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction)
{
}

void NetworkFramework::SendKeyInput(BYTE key, bool down)
{
}

void NetworkFramework::SendPlayerInfo(const DirectX::SimpleMath::Quaternion& roationChild)
{
}

NetworkFramework gNetworkFramework{};