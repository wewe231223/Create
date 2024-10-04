#include "pch.h"
#include "Game/utils/NonReplacementSampler.h"
#include "Game/utils/RandomEngine.h"


NonReplacementSampler::NonReplacementSampler()
{

}

NonReplacementSampler::~NonReplacementSampler()
{
}


int NonReplacementSampler::Sample()
{
	assert(mCount < TOTAL_SAMPLES);  // 샘플링할 수 있는 숫자가 남아있는지 확인

    int number;
    do {
        number = mDistribution(MersenneTwister);  // 난수 생성
    } while (mUsed.test(number));  // 이미 선택된 숫자인지 확인

    mUsed.set(number);  // 숫자를 선택된 것으로 마크
    ++mCount;
    return number;
}

void NonReplacementSampler::Free(int sign)
{
	mUsed.reset(sign);
	--mCount;
}

NonReplacementSampler NrSampler{};