#include "pch.h"
#include "Client.h"

Client::Client()
{
}

Client::~Client()
{
}

bool Client::InitializeClient(SOCKET socket)
{
	return false;
}

void Client::ShutdownClient()
{
}

BYTE Client::GetId()
{
	return 0;
}

std::mutex& Client::GetRecvMutex()
{
	// TODO: 여기에 return 문을 삽입합니다.
}

std::mutex& Client::GetSendMutex()
{
	// TODO: 여기에 return 문을 삽입합니다.
}

void Client::WakeSendThread()
{
}

void Client::SendWorker()
{
}

void Client::RecvWorker()
{
}

void Client::ReadFromRecvBuffer(PIStream& stream)
{
}

void Client::SendLoginPass(bool success)
{
}

void Client::SendPlayerExit()
{
}

void Client::SendPlayerEntered()
{
}

void Client::SendPlayerReady(BYTE color)
{
}

void Client::SendGameEnd(bool win)
{
}

void Client::SendPlayerInfo(float hp, const  DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Quaternion rotationParent, const DirectX::SimpleMath::Quaternion rotationChild)
{
}

void Client::SendBulletInfo(short bulletIndex, const  DirectX::SimpleMath::Vector3& position)
{
}
