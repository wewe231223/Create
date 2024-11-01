#include "pch.h"
#include "Game/network/NetworkFramework.h"
#include "Game/network/ErrorHandler.h"

NetworkFramework::NetworkFramework()
{
}

NetworkFramework::~NetworkFramework()
{
}


bool NetworkFramework::InitializeNetwork()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		ErrorHandle::WSAErrorMessageBoxAbort("WSA initialization failed");
	}

	mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (mSocket == INVALID_SOCKET) {
		WSACleanup();
		ErrorHandle::WSAErrorMessageBoxAbort("Socket creation failed");
	}
}


bool NetworkFramework::Connect(const fs::path& ipFilePath)
{
	return false;
}

void NetworkFramework::RecvWorker()
{
}

void NetworkFramework::SendWorker()
{
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