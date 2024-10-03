#include "pch.h"
#include "Game/utils/RandomEngine.h"


std::random_device RandomDevice{};
std::mt19937 MersenneTwister{ RandomDevice() };