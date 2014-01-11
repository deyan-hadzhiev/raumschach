#ifndef __RANDOM_GENERATOR_H__
#define __RANDOM_GENERATOR_H__

#include <random>
#include <math.h>

class RandomGenerator
{
public:
	RandomGenerator(unsigned seed = 123u)
	{
		Seed(seed);
	}

	void Seed(unsigned s)
	{
		generator.seed(s);
	}

	// get the next random generated number
	unsigned GetRand()
	{
		std::uniform_int_distribution<unsigned> gen;
		return gen(generator);
	}
	// get the next random generated number from [0..upperBound)
	unsigned GetRand(unsigned upperBound)
	{
		std::uniform_int_distribution<unsigned> gen(0, upperBound - 1);
		return gen(generator);
	}
	// get the next random generated number from [a..b)
	unsigned GetRand(unsigned a, unsigned b)
	{
		std::uniform_int_distribution<unsigned> gen(a, b - 1);
		return gen(generator);
	}

private:
	std::mt19937 generator; // Mersene Twister generator
};

#endif // __RANDOM_GENERATOR_H__