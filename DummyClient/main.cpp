#include "pch.h"

#include "../ClientNetwork/core/NetworkManager.h"
#include "../ClientNetwork/core/RecvBuffer.h"

int main(int argc, char* argv[])
{
    NetworkManager n;
    n.InitializeNetwork();
    if (not n.Connect("../Common/serverip.ini")) {
        return EXIT_FAILURE;
    }

    /* TODO ... Chat Logic 작성 */
    std::string chat;
    RecvBuffer rBuffer;
    PacketChatting chatPacket{ };
    while (true) {
        n.ReadFromRecvBuffer(rBuffer);
        while (false == rBuffer.Empty()) {
            if (rBuffer.Read(reinterpret_cast<char*>(&chatPacket), sizeof(PacketChatting))) {
                std::cout << std::format("Client {}: {}\n", chatPacket.id, chatPacket.chatBuffer);
            }
            else {
                std::cout << "read failure" << std::endl;
                break;
            }
        }
        rBuffer.Clean();

        std::cin >> chat;

        std::lock_guard lock{ n.GetSendMutex() };

        n.SendChatPacket(chat);

        n.WakeSendThread();
    }

    n.JoinThreads();
}