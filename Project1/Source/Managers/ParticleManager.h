#pragma once

class ParticlesComponent;
struct Particle;
class InstancedParticle;

class ParticleManager
{
public:
	GEAR_API static ParticleManager* GetInstance();

	/*!
	* To find unused particles
	* in the Particle Container

	*/

	 /*	Cleaning up the Instances
	 *
	 */
	GEAR_API void CleanUp();

	GEAR_API void Draw();
	//static uint32_t m_uDisabledID;

	//GEAR_API int FindUnusedParticle(Particle* temp, int max);

	///*!
	// *	Function that can tell whether a Particle must be put
	// *	before or after another Particle in the container
	// */
	//GEAR_API void SortParticles(Particle* tem, int max);

	///*!
	// * Initializing the data required
	// *
	// */
	//GEAR_API void Init();

	///*!

	///*!
private:
	GEAR_API ParticleManager();
	GEAR_API ~ParticleManager();
	static ParticleManager* mInstance;
};