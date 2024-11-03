#pragma once


enum PacketType {
	PT_PlayerLogin = 0,
	PT_Ready = 1,
	PT_KeyInput = 2,
	PT_TurretRotation = 3,
	PT_RequestFire = 4,

	PT_LoginPass = 10,
	PT_PlayerEntered = 11,
	PT_PlayerExit = 12,
	PT_PlayerReady = 13,
	PT_PlayerInfo = 14,
	PT_BulletInfo = 15,
	PT_GameEnd = 16
};

struct Packet {
	BYTE size;
	BYTE type;
	BYTE id;
};

struct PacketLoginPass : Packet {
	bool success;
};

struct PacketPlayerReady : Packet {
	BYTE color;
};

struct PlayerEntered : Packet {

};

struct PacketPlayerExit : Packet {

};

struct PacketBulletInfo : Packet {
	short bulletIndex;
	DirectX::SimpleMath::Vector3 position;
};

struct PacketPlayerInfo : Packet {
	float HP;
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Quaternion rotationParent;
	DirectX::SimpleMath::Quaternion rotationChild;
};

struct PacketGameEnd : Packet {
	bool youWin;
};

struct PacketRequestLogin : Packet {
	char loginId[16];
	char loginPassword[16];
};

struct PacketReady : Packet {
	BYTE color;
};

struct PacketKeyInput : Packet {
	BYTE key;
	bool down;
};

struct PacketTurretRotation : Packet {
	DirectX::SimpleMath::Quaternion rotationChild;
};

struct PacketRequestFire : Packet {
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 direction;
};

