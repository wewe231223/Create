#include "pch.h"

#include "../ClientNetwork/core/NetworkManager.h"

int main(int argc, char* argv[])
{
    NetworkManager n;
    n.InitializeNetwork();
    n.Connect("../Common/serverip.ini");

    /* TODO ... Chat Logic 작성 */
    while (true) {

    }

    n.JoinThreads();
}