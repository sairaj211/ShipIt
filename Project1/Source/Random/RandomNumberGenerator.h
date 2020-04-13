#pragma once

#define RNG RandomNumberGenerator::GetInstance()

/*!
 * Random Number Generator that can generate
 * numbers in a range of values of a specified 
 * type
 *
 */
class RandomNumberGenerator
{
public:
	/*!
	 * Get the RandomNumberGenerator Singleton
	 * reference
	 *
	 * \return The RandomNumberGenerator reference
	 */
	GEAR_API static RandomNumberGenerator* GetInstance();
	/*!
	 * Initialize the generator with a default
	 * seed
	 *
	 */
	GEAR_API void Init();
	/*!
	 * Set the seed of the RandomNumberGenerator
	 *
	 * \param a_seed The new seed for the generator
	 */
	GEAR_API void Reseed(unsigned int a_seed);
	/*!
	 * Generate a random short in the range [a_min, a_max]
	 *
	 * \param a_min The minimum value that will be generated
	 * \param a_max The maximum value that will be generated
	 * \return The randomly generated short in the range [a_min, a_max]
	 */
	GEAR_API short GenerateShort(short a_min, short a_max);
	/*!
	 * Generate a random integer in the range [a_min, a_max]
	 *
	 * \param a_min The minimum value that will be generated
	 * \param a_max The maximum value that will be generated
	 * \return The randomly generated integer in the range [a_min, a_max]
	 */
	GEAR_API int GenerateInteger(int a_min, int a_max);
	/*!
	 * Generate a random long in the range [a_min, a_max]
	 *
	 * \param a_min The minimum value that will be generated
	 * \param a_max The maximum value that will be generated
	 * \return The randomly generated long in the range [a_min, a_max]
	 */
	GEAR_API long GenerateLong(long a_min, long a_max);
	/*!
	 * Generate a random float in the range [a_min, a_max)
	 *
	 * \param a_min The minimum value that will be generated
	 * \param a_max The maximum value that will be generated
	 * \return The randomly generated float in the range [a_min, a_max)
	 */
	GEAR_API float GenerateFloat(float a_min, float a_max);
	/*!
	 * Generate a random double in the range [a_min, a_max)
	 *
	 * \param a_min The minimum value that will be generated
	 * \param a_max The maximum value that will be generated
	 * \return The randomly generated double in the range [a_min, a_max)
	 */
	GEAR_API double GenerateDouble(double a_min, double a_max);
	/*!
	 * Get the generator used to generate the
	 * random numbers
	 *
	 * \return The generator used to generate the random numbers
	 */
	GEAR_API std::mt19937_64 GetGenerator();
	/*!
	 * Delete the RandomNumberGenerator Singleton 
	 *
	 */
	GEAR_API void Cleanup();

private:
	GEAR_API RandomNumberGenerator();
	GEAR_API ~RandomNumberGenerator();

private:
	static RandomNumberGenerator* mInstance;
	std::mt19937_64 mRNGen;
	unsigned int mSeed;
};