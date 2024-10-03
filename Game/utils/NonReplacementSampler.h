#pragma once 


constexpr int TOTAL_SAMPLES = 65536;

// 비복원 샘플링을 위한 함수입니다. 
class NonReplacementSampler {
public:
    NonReplacementSampler();
    ~NonReplacementSampler();

    // 비복원 샘플링 함수
    int Sample();
private:
    std::uniform_int_distribution<int> mDistribution{ 0,TOTAL_SAMPLES-1 };  
    std::bitset<TOTAL_SAMPLES> mUsed{};                               
    int mCount{ 0 };                           
};

extern NonReplacementSampler NrSampler;