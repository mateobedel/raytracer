#include "Walnut/Random.h"

namespace Walnut {

	std::mt19937 Random::s_RandomEngine;
	std::uniform_real_distribution<float> Walnut::Random::s_RealDistribution(0.0f, 1.0f);
	std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;

}