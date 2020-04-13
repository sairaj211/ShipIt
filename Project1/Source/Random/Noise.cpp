#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Noise.h"
#include "Random/RandomNumberGenerator.h"

Noise* Noise::mInstance = nullptr;

float Noise::RawPerlinNoise(float a_x, float a_y, float a_z)
{
	//find the cube in the grid
	int X = int(std::floor(a_x)) & 255; int Y = int(std::floor(a_y)) & 255; int Z = int(std::floor(a_z)) & 255;
	//get the relative coordinates in the cub itself
	float _x = a_x- std::floor(a_x);
	float _y = a_y- std::floor(a_y);
	float _z = a_z- std::floor(a_z);

	//Get the fade curves
	float u = Fade(_x);                                
	float v = Fade(_y);                               
	float w = Fade(_z);

	//Corners of a curve
	int A = mPermuations[X] + Y, 
		AA = mPermuations[A] + Z, 
		AB = mPermuations[A + 1] + Z,      
		B = mPermuations[X + 1] + Y, 
		BA = mPermuations[B] + Z, 
		BB = mPermuations[B + 1] + Z;      

	return Lerp(w, 
			Lerp(v, 
				Lerp(u, Grad(mPermuations[AA], _x, _y, _z),  Grad(mPermuations[BA], _x - 1, _y, _z)),  
			Lerp(u, Grad(mPermuations[AB], _x, _y - 1, _z), Grad(mPermuations[BB], _x - 1, _y - 1, _z))),
		Lerp(v, 
			Lerp(u, Grad(mPermuations[AA + 1], _x, _y, _z - 1),  Grad(mPermuations[BA + 1], _x - 1, _y, _z - 1)), 
			Lerp(u, Grad(mPermuations[AB + 1], _x, _y - 1, _z - 1), Grad(mPermuations[BB + 1], _x - 1, _y - 1, _z - 1))));
}

void Noise::GenPermutations()
{
	for (int i = 0; i < 256; ++i) {
		mPermuations[i] = i;
	}

	std::shuffle(std::begin(mPermuations), std::begin(mPermuations) + 256, RNG->GetGenerator());

	for (int i = 0; i < 256; ++i) {
		mPermuations[256 + i] = mPermuations[i];
	}
}

float Noise::PerlinNoise(int a_octave, float a_persistence, float a_scale, float a_x, float a_y, float a_z, float a_lo, float a_hi)
{
	float t = 0; float f = a_scale; float a = 1;//total frequency and amplitude

	float maxA = 0;

	for (int o = 0; o < int(a_octave); ++o) {

		t += RawPerlinNoise(a_x * f, a_y * f, a_z*f) * a;

		f = f * 2;

		maxA += a;

		a = a * a_persistence;
	}

	return (t / maxA) * (a_hi - a_lo) / 2 + (a_hi + a_lo) / 2; //normilize values between -1 and 1
}

float Noise::Fade(float a_n)
{
	return a_n * a_n * a_n * (a_n * (a_n * 6 - 15) + 10);
}

float Noise::Lerp(float a_deltaTime, float a_n0, float a_n1)
{
	return a_n0 + a_deltaTime * (a_n1 - a_n0);
}

float Noise::Grad(int a_index, float a_x, float a_y, float a_z)
{
	int h = a_index & 15;
	float u = h < 8 ? a_x : a_y;
	float v = h < 4 ? a_y : h == 12 || h == 14 ? a_x : a_z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

Noise * Noise::GetInstance()
{
	if (mInstance == nullptr)
		mInstance = new Noise();
	
	return mInstance;
}

void Noise::Cleanup()
{
	delete mInstance;
	mInstance = nullptr;
}

Noise::Noise() 
{
	GenPermutations();
}

Noise::~Noise()
{

}
