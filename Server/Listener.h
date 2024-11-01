#pragma once

class Listener {
public:
    void InitializeNetwork();
    void AcceptWorker();
    void ShutdownAccept();

public:
    std::thread mAcceptThread;
};

