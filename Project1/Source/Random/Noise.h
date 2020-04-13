#pragma once

#define NOISE Noise::GetInstance()

 /*!
  * 
  * Noise generator in 3 dimension based on 2002 paper 
  * Improving Noise by Ken Pelin 
  */
class Noise
{
public:
	/*!
	 * Up to 3D Perlin Noise generation
	 * 
	 * \param a_octave The number of Octaves for noise generation
	 * \param a_persistence The persistence for noise generation
	 * \param a_scale The scale for noise generation
	 * \param a_x Given x position in for noise generation default 0
	 * \param a_y Given y position in for noise generation default 0
	 * \param a_z Given z position in for noise generation default 0
	 * \param a_lo Minimum value default -1
	 * \param a_hi Maximum value default 1
	 * \return A float between [a_lo,a_hi]
	 */
	GEAR_API float PerlinNoise(int a_octave, float a_persistence, float a_scale, float a_x, float a_y = 0, float a_z = 0, float a_lo = -1, float a_hi = 1);

	/*!
	 * Initializes the Perlin Noise Permutation array
	 * with integers 0 - 255 in a random order
	 *
	 */
	GEAR_API void GenPermutations();
	
	/*!
	 * Get the Current Instance of PerlinNoise or if it not initialized it will
	 * create a new instance following the singleton pattern
	 * 
	 * \return The Noise Instance
	 */
	GEAR_API static Noise* GetInstance();

	/*!
	 * Clean up the instance deleting the current instance
	 * 
	 */
	GEAR_API void Cleanup();

private:
	GEAR_API float RawPerlinNoise(float a_x, float a_y, float a_z);
	GEAR_API float Grad(int a_index, float a_x, float a_y, float a_z);
	GEAR_API float Fade(float a_n);
	GEAR_API float Lerp(float a_deltaTime, float a_n0, float a_n1);
	GEAR_API Noise();
	GEAR_API ~Noise();
	
private:
	static Noise* mInstance;
	int mPermuations[512];
};

