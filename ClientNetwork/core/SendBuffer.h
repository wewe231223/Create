#pragma once

class SendBuffer {
    inline static constexpr size_t BUFFER_SIZE = 4096;
public:
    SendBuffer();
    ~SendBuffer();

public:
    void Clean();

    const char* Buffer() const;
    bool Write(void* data, INT32 writeBytes);

    bool Empty() const;

private:
    INT32 mWriteCursor;
    std::array<char, BUFFER_SIZE> mBuffer;
}; 