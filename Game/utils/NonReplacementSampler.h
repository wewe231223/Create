#pragma once 
// 음 이거도 템플릿으로 하면? 
constexpr int TOTAL_SAMPLES = 1024;

// 비복원 샘플링을 위한 클래스 
class NonReplacementSampler {
public:
    NonReplacementSampler();
    ~NonReplacementSampler();

    // 비복원 샘플링 함수
    int Sample();
    void Free(int sign);
private:
    std::uniform_int_distribution<int> mDistribution{ 0,TOTAL_SAMPLES-1 };  
    std::bitset<TOTAL_SAMPLES> mUsed{};                               
    int mCount{ 0 };                           
};

extern NonReplacementSampler NrSampler;