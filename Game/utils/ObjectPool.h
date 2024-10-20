#pragma once 

template <typename ObjectTy, size_t size>
class ObjectPool {
public:
    ObjectPool() = default;

    void AssignValidateCallBack(std::function<bool(const std::shared_ptr<ObjectTy>&)>&& callBack);

    template<typename... Args> requires std::constructible_from<ObjectTy, Args...>
    inline void Initialize(Args&&... args) {
        mFree.resize(size);
        mUsed.reserve(size);
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

    void Update();

    // 획득된 객체 순회
    auto begin() const { return mUsed.begin(); }
    auto end() const { return mUsed.end(); }

	std::vector<std::shared_ptr<ObjectTy>>& GetUsed() { return mUsed; }
private:
    std::vector<std::shared_ptr<ObjectTy>> mFree;  // 사용 가능한 객체 목록
    std::vector<std::shared_ptr<ObjectTy>> mUsed;  // 획득된 객체 목록

    std::function<bool(const std::shared_ptr<ObjectTy>&)> mValidateCallBack{};
};

template<typename ObjectTy, size_t size>
inline void ObjectPool<ObjectTy, size>::AssignValidateCallBack(std::function<bool(const std::shared_ptr<ObjectTy>&)>&& callBack)
{
	mValidateCallBack = std::move(callBack);
}

template<typename ObjectTy, size_t size>
inline void ObjectPool<ObjectTy, size>::Release(ObjectTy* obj) {
    // 획득된 객체 목록에서 찾아서 삭제
    auto it = std::find_if(mUsed.begin(), mUsed.end(), [obj](const std::shared_ptr<ObjectTy>& p) { return p.get() == obj; });

    if (it != mUsed.end()) {
        mFree.emplace_back(std::move(*it));
        mUsed.erase(it);
    }
}



template<typename ObjectTy, size_t size>
inline void ObjectPool<ObjectTy, size>::Update()
{
    if (mValidateCallBack) {
		auto it = std::partition(mUsed.begin(), mUsed.end(), mValidateCallBack);
        if (it == mUsed.end()) return;
		std::move(it, mUsed.end(), std::back_inserter(mFree));
        mUsed.erase(it, mUsed.end());
    }
}
