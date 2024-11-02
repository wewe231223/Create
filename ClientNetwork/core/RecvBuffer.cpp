#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer()
    : mReadCursor{ 0 },
    mWriteCursor{ 0 },
    mBufferSize{ BUFFER_SIZE * BUFFER_COUNT },
    mBuffer{ }
{
}

RecvBuffer::~RecvBuffer()
{
}

constexpr size_t RecvBuffer::GetBufferSize() const
{
    return BUFFER_SIZE * BUFFER_COUNT;
}

void RecvBuffer::Clean()
{
    INT32 dataSize = DataSize();
    if (0 == dataSize) {
        mReadCursor = 0;
        mWriteCursor = 0;
    }
    else if (FreeSize() < mBufferSize) {
        ::memcpy(&mBuffer[0], &mBuffer[mReadCursor], dataSize);
        mReadCursor = 0;
        mWriteCursor = dataSize;
    }
}

bool RecvBuffer::Read(char* data, INT32 readBytes)
{
    bool readAll = DataSize() >= readBytes;

    ::memcpy(data, &mBuffer[mReadCursor], readBytes);
    mReadCursor += readBytes;

    return readAll;
}

INT32 RecvBuffer::ReadAll(char* data)
{
    INT32 dataSize = DataSize();
    ::memcpy(data, &mBuffer[mReadCursor], dataSize);
    mReadCursor += dataSize;
    Clean();
    return dataSize;
}

bool RecvBuffer::Write(const char* data, INT32 writeBytes)
{
    if (FreeSize() < writeBytes) {
        return false;
    }

    ::memcpy(&mBuffer[mWriteCursor], data, writeBytes);
    mWriteCursor += writeBytes;
}

INT32 RecvBuffer::FreeSize() const
{
    return GetBufferSize() - mWriteCursor;
}

INT32 RecvBuffer::DataSize() const
{
    return mWriteCursor - mReadCursor;
}
