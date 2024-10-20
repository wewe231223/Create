#pragma once 

template <typename ObjectTy, size_t size>
class ObjectPool {
public:
    ObjectPool() = default;

    template<typename... Args> requires std::constructible_from<ObjectTy, Args...>
    inline void Initialize(Args&&... args) {
        mFree.resize(size);
        for (auto& object : mFree) {
            object = std::make_shared<ObjectTy>(args...);
        }
    }

    inline ObjectTy* Acquire() {
        if (mFree.size() == 0) {
            return nullptr;
        }

        auto ptr = mFree.back();
        mFree.pop_back();
        mUsed.emplace_back(ptr);

        return ptr.get();
    }

    void Release(ObjectTy* obj);

    // 획득된 객체 순회
    auto begin() const { return mUsed.begin(); }
    auto end() const { return mUsed.end(); }

private:
    std::vector<std::shared_ptr<ObjectTy>> mFree;  // 사용 가능한 객체 목록
    std::vector<std::shared_ptr<ObjectTy>> mUsed;  // 획득된 객체 목록
};

template<typename ObjectTy, size_t size>
inline void ObjectPool<ObjectTy, size>::Release(ObjectTy* obj) {
    // 획득된 객체 목록에서 찾아서 삭제
    auto it = std::find_if(mUsed.begin(), mUsed.end(), [obj](const std::shared_ptr<ObjectTy>& p) { return p.get() == obj; });

    if (it != mUsed.end()) {
        mFree.emplace_back(std::move(*it));
        mUsed.erase(it);
    }
}
