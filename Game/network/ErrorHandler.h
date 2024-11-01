#pragma once

namespace ErrorHandle {
    inline void WSAErrorMessageBox(const char* caption = nullptr)
    {
        auto errorCode = WSAGetLastError();

        LPVOID lpMsgBuf;
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<char*>(&lpMsgBuf),
            0,
            NULL
        );

        MessageBoxA(nullptr, reinterpret_cast<char*>(lpMsgBuf), caption, MB_ICONERROR);

        LocalFree(lpMsgBuf);
    }

    inline void WSAErrorMessageBoxExit(const char* caption = nullptr)
    {
        auto errorCode = WSAGetLastError();

        LPVOID lpMsgBuf;
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<char*>(&lpMsgBuf),
            0,
            NULL
        );

        MessageBoxA(nullptr, reinterpret_cast<char*>(lpMsgBuf), caption, MB_ICONERROR);

        LocalFree(lpMsgBuf);

        exit(EXIT_FAILURE);
    }

    inline void WSAErrorMessageBoxAbort(const char* caption = nullptr)
    {
        auto errorCode = WSAGetLastError();

        LPVOID lpMsgBuf;
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<char*>(&lpMsgBuf),
            0,
            NULL
        );

        MessageBoxA(nullptr, reinterpret_cast<char*>(lpMsgBuf), caption, MB_ICONERROR);

        LocalFree(lpMsgBuf);

        abort();
    }
}