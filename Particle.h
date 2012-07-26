

// defines a particle

#ifndef StormForge_Matt_Particle
#define StormForge_Matt_Particle


#include "vector.h"
#include "memorymgr.h"



	// Structure for storing the current render states for D3D, to reduce redundant changes
	class PointSpriteInfo
	{
	public:

		float fScaleA, fScaleB, fScaleC;
		float fSize, fMinSize, fMaxSize;
		bool bSprites, bScale;

		PointSpriteInfo(void) { memset(this, 0, sizeof(*this)); }
	};



	struct ParticleUpdateInfo
	{
		Vector vGravity;
		Vector vForce;
		Vector4D vColorDelta;
		float fTime;

		// Special effects
		bool bSpiral;
		bool bGravity;
		bool bApplyForce;

		Direct3D::D3DVertex *pvArray;

		/*Vector vUp;
		Vector vRight;
		Vector vNormal;*/
		//Vector4D vTextureDeltaX;
		//Vector4D vTextureDeltaY;
		//Vertex *pGeometry;

		char __PADDING_ONLY_DO_NOT_USE;

	};









class Particle
{

public:

	float fLife;								// How long do I have left to live?
	Vector vVelocity;					// How fast am I going, and where to?

	// Get at the emitter's information
	friend class Emitter;
	
	// Start off empty
	Particle(void) { memset(this, 0, sizeof(*this)); }
	void Clear(void) {	memset(this, 0, sizeof(*this)); }
	
	// Update the life and values of the particle.
	void Update(ParticleUpdateInfo *pInfo, unsigned int u);

	// Manage memory for particles
	#if _DEBUG
		ManageMemory(Particle, BASETYPE_PARTICLE);
	#endif

};

#endif
