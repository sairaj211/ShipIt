#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "RandomNumberGenerator.h"

RandomNumberGenerator* RandomNumberGenerator::mInstance = nullptr;

RandomNumberGenerator::RandomNumberGenerator()
{

}

RandomNumberGenerator::~RandomNumberGenerator()
{
	LOG_INFO("Seed: " + std::to_string(mSeed));
}

RandomNumberGenerator* RandomNumberGenerator::GetInstance()
{
	if (!mInstance)
		mInstance = new RandomNumberGenerator();

	return mInstance;
}

void RandomNumberGenerator::Init()
{
	LOG_INFO("RandomNumberGenerator Init");
	std::random_device randomDevice;
	mSeed = randomDevice();
	mRNGen = std::mt19937_64(mSeed);
}

void RandomNumberGenerator::Reseed(unsigned int a_seed)
{
	mSeed = a_seed;
	mRNGen = std::mt19937_64(mSeed);
}

short RandomNumberGenerator::GenerateShort(short a_min, short a_max)
{
	std::uniform_int_distribution<short> shortDistribution(a_min, a_max);

	return shortDistribution(mRNGen);
}

int RandomNumberGenerator::GenerateInteger(int a_min, int a_max)
{
	std::uniform_int_distribution<int> integerDistribution(a_min, a_max);

	return integerDistribution(mRNGen);
}

long RandomNumberGenerator::GenerateLong(long a_min, long a_max)
{
	std::uniform_int_distribution<long> longDistribution(a_min, a_max);

	return longDistribution(mRNGen);
}

float RandomNumberGenerator::GenerateFloat(float a_min, float a_max)
{
	std::uniform_real_distribution<float> floatDistribution(a_min, a_max);

	return floatDistribution(mRNGen);
}

double RandomNumberGenerator::GenerateDouble(double a_min, double a_max)
{
	std::uniform_real_distribution<double> doubleDistribution(a_min, a_max);

	return doubleDistribution(mRNGen);
}

std::mt19937_64 RandomNumberGenerator::GetGenerator() {
	return mRNGen;
}

void RandomNumberGenerator::Cleanup()
{
	delete mInstance;
	mInstance = nullptr;
}