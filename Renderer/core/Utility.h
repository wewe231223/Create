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
            : buffer_(buffer), index_(index), head_(head) {}

        reference operator*() {
            return buffer_[(head_ + index_) % Capacity];
        }

        Iterator& operator++() {
            ++index_;
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return index_ != other.index_;
        }

    private:
        std::array<T, Capacity>& buffer_;
        std::size_t index_;
        std::size_t head_;
    };
public:
    RingBuffer() : head_(0), tail_(0), size_(0) {}

    void Push(const T& value) 
    {
        buffer_[tail_] = value;
        tail_ = (tail_ + 1) % Capacity;
        if (size_ < Capacity) {
            ++size_;
        }
        else {
            head_ = (head_ + 1) % Capacity;
        }
    }

    template <typename... Args>
    void Emplace(Args&&... args) 
    {
        buffer_[tail_] = T(std::forward<Args>(args)...);
        tail_ = (tail_ + 1) % Capacity;
        if (size_ < Capacity) {
            ++size_;
        }
        else {
            head_ = (head_ + 1) % Capacity;
        }
    }

    bool Empty() const 
    {
        return size_ == 0;
    }

    std::size_t Size() const 
    {
        return size_;
    }
    // 애들은 범
    Iterator begin() 
    {
        return Iterator(buffer_, 0, head_);
    }

    Iterator end() 
    {
        return Iterator(buffer_, size_, head_);
    }

private:
    std::array<T, Capacity> buffer_{};
    std::size_t head_{};
    std::size_t tail_{};
    std::size_t size_{};
};

