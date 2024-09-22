#pragma once 
std::optional<std::wstring> CheckHR(const HRESULT& hr, const std::source_location& location = std::source_location::current() );


/// <summary>
///  a * (1 - t) + b * t
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="a"></param>
/// <param name="b"></param>
/// <param name="t"></param>
/// <returns></returns>
template<typename T>
T Lerp(T& a, T& b, float t) {
	return a * (1 - t) + b * t;
}


template<typename T> 
constexpr size_t GetCBVSize() {
	return (sizeof(T) + 255) & ~255;
}

template <typename T, std::size_t Capacity>
class RingBuffer {
public:
    class Iterator {
    public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::forward_iterator_tag;

        Iterator(std::array<T, Capacity>& buffer, std::size_t index, std::size_t head)
            : mBuffer(buffer), mIndex(index), mHead(head) {}

        reference operator*() {
            return mBuffer[(mHead + mIndex) % Capacity];
        }

        Iterator& operator++() {
            ++mIndex;
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return mIndex != other.mIndex;
        }

    private:
        std::array<T, Capacity>& mBuffer;
        size_t mIndex;
        size_t mHead;
    };
public:
    RingBuffer() : mHead(0), mTail(0), mSize(0) {}

    void Push(const T& value) 
    {
        mBuffer[mTail] = value;
        mTail = (mTail + 1) % Capacity;
        if (mSize < Capacity) {
            ++mSize;
        }
        else {
            mHead = (mHead + 1) % Capacity;
        }
    }

    template <typename... Args>
    void Emplace(Args&&... args) 
    {
        mBuffer[mTail] = T(std::forward<Args>(args)...);
        mTail = (mTail + 1) % Capacity;
        if (mSize < Capacity) {
            ++mSize;
        }
        else {
            mHead = (mHead + 1) % Capacity;
        }
    }

    bool Empty() const 
    {
        return mSize == 0;
    }

    std::size_t Size() const 
    {
        return mSize;
    }
    // 애들은 범
    Iterator begin() 
    {
        return Iterator(mBuffer, 0, mHead);
    }

    Iterator end() 
    {
        return Iterator(mBuffer, mSize, mHead);
    }

private:
    std::array<T, Capacity> mBuffer{};
    std::size_t mHead{};
    std::size_t mTail{};
    std::size_t mSize{};
};

