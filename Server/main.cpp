#include "pch.h"
#include "Listener.h"

int main()
{
    Listener l;
    l.InitializeNetwork();

    l.JoinAcceptWorker();
}