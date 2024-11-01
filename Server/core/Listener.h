#pragma once

/// <summary>
/// 클라이언트의 Connect요청을 수락할 Accpet작업을 수행하는 클래스
/// 메인쓰레드와 다른 쓰레드에서 동작함.
/// </summary>
class Listener {
public:
    void InitializeNetwork();
    void AcceptWorker();
    void ShutdownAccept();

    void JoinAcceptWorker();

public:
    std::thread mAcceptThread;
    SOCKET mListenSocket;
};
