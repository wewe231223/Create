#pragma once 

template <typename ObjectTy>
class ObjectPool {
public:

    template<typename... Args>
    inline ObjectTy* Acquire(Args&&... args) {
        if (mFree.empty()) {
            mFree.emplace_back(std::make_shared<ObjectTy>(std::forward(args)...));
        }
        auto ptr = mFree.back();
        mUsed.emplace_back(std::move(ptr));
        mUsed.pop_back();

        return mUsed.back().get();
    }

    void Release(ObjectTy* obj);
    // 사용 가능한 객체 순회
    auto begin() const { return mFree.begin(); }
    auto end() const { return mFree.end(); }

    // 획득된 객체 순회
    auto used_begin() const { return mUsed.begin(); }
    auto used_end() const { return mUsed.end(); }

private:
    std::vector<std::shared_ptr<ObjectTy>> mFree;  // 사용 가능한 객체 목록
    std::vector<std::shared_ptr<ObjectTy>> mUsed;  // 획득된 객체 목록
};




template<typename ObjectTy>
inline void ObjectPool<ObjectTy>::Release(ObjectTy* obj)
{
    // 획득된 객체 목록에서 찾아서 삭제
    auto it = std::find_if(mUsed.begin(), mUsed.end(), [obj](const std::shared_ptr<ObjectTy>& p) { return p.get() == obj; });

    if (it != mUsed.end()) {
        mUsed.erase(it);
        mFree.emplace_back(std::move(*it));
    }
}

