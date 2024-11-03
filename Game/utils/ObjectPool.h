#pragma once 

template <typename ObjectTy, size_t size>
class ObjectPool {
public:
    using ObjectIndex = unsigned short;

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

	template<typename Callable, typename... Args> 
        requires    std::constructible_from<ObjectTy, Args...> && 
                    std::invocable<Callable, Args...> && 
                    std::same_as<std::invoke_result_t<Callable, Args...>, std::shared_ptr<ObjectTy>>
   inline void Initialize(Callable&& callable, Args&&... args) {
       mFree.resize(size);
	   mUsed.reserve(size);

       for (auto& object : mFree) {
		   object = std::invoke(callable, args...);
       }
   }

#ifdef DUMMY_SERVER_ENABLE 
	inline ObjectIndex Acquire() {
		if (mFree.size() == 0) {
			return std::numeric_limits<ObjectIndex>::max();
		}

		auto ptr = mFree.back();
		mFree.pop_back();
		mUsed.emplace_back(ptr);

        return static_cast<ObjectIndex>(mUsed.size() - 1);
    }
#else 
    inline ObjectTy* Acquire() {
        if (mFree.size() == 0) {
            return nullptr;
        }

        auto ptr = mFree.back();
        mFree.pop_back();
        mUsed.emplace_back(ptr);

        return ptr.get();
    }
#endif 

    inline ObjectTy* Acquire(ObjectIndex index) {
        auto ptr = mFree.back();
        mFree.pop_back();

        mUsed.emplace(mUsed.begin() + index, ptr);

        return ptr.get();
    }

    inline void Release(ObjectIndex index) {
        auto ptr = mUsed[index];

		mUsed.erase(mUsed.begin() + index);

        mFree.emplace_back(ptr);
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
    auto it = std::partition(mUsed.begin(), mUsed.end(), [](const std::shared_ptr<ObjectTy>& object) { return (bool(*object)); });
    if (it == mUsed.end()) return;
    std::move(it, mUsed.end(), std::back_inserter(mFree));
    mUsed.erase(it, mUsed.end());
}


