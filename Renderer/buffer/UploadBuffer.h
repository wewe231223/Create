#pragma once

template<typename Ty, bool Constant = false>
class UploadBuffer {
public:
    // 반복자 정의
    class Iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = Ty;
        using difference_type = std::ptrdiff_t;
        using pointer = Ty*;
        using reference = Ty&;

        Iterator(pointer ptr) : mPtr(ptr) {}

        reference operator*() const { return *mPtr; }
        pointer operator->() { return mPtr; }

        // 전위 증가 연산자
        Iterator& operator++() {
            mPtr++;
            return *this;
        }

        // 후위 증가 연산자
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        // 전위 감소 연산자
        Iterator& operator--() {
            mPtr--;
            return *this;
        }

        // 후위 감소 연산자
        Iterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

        // 임의 접근 연산자
        Iterator operator+(difference_type offset) const {
            return Iterator(mPtr + offset);
        }

        Iterator operator-(difference_type offset) const {
            return Iterator(mPtr - offset);
        }

        difference_type operator-(const Iterator& other) const {
            return mPtr - other.mPtr;
        }

        bool operator==(const Iterator& other) const {
            return mPtr == other.mPtr;
        }

        bool operator!=(const Iterator& other) const {
            return mPtr != other.mPtr;
        }

        bool operator<(const Iterator& other) const {
            return mPtr < other.mPtr;
        }

        bool operator>(const Iterator& other) const {
            return mPtr > other.mPtr;
        }

        bool operator<=(const Iterator& other) const {
            return mPtr <= other.mPtr;
        }

        bool operator>=(const Iterator& other) const {
            return mPtr >= other.mPtr;
        }

    private:
        pointer mPtr;
    };

public:
    UploadBuffer() = default;
    UploadBuffer(const UploadBuffer& rhs) = delete;
    UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
    ~UploadBuffer() {
        if (mUploadBuffer != nullptr) {
            mUploadBuffer->Unmap(0, nullptr);
        }
        mMappedData = nullptr;
    }

    void Alloc(ComPtr<ID3D12Device>& device, size_t elementCount) {
        mElementByteSize = sizeof(Ty);
        mMemSize = mElementByteSize * elementCount;

        if constexpr (Constant) {
            mElementByteSize = (sizeof(Ty) + 255) & ~255;
        }

        D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount);

        CheckHR(device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mUploadBuffer)
        ));

        CheckHR(mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData)));
    }

    ID3D12Resource* Resource() const {
        return mUploadBuffer.Get();
    }

    // 전체 데이터 카피에 사용할 것
    void CopyData(const Ty& data) {
        memcpy(reinterpret_cast<void*>(mMappedData), reinterpret_cast<const void*>(std::addressof(data)), mMemSize);
    }

    Iterator begin() { return Iterator(mMappedData); }
    Iterator end() { return Iterator(mMappedData + mMemSize / mElementByteSize); }

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
    Ty* mMappedData = nullptr;

    UINT mElementByteSize = 0;
    size_t mMemSize = 0;
};
