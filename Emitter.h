
// Defines a particle emitter


#ifndef StormForge_Matt_Emitter
#define StormForge_Matt_Emitter

#include "Vector.h"
#include "MemoryMgr.h"


class Camera;

class Emitter : public Base
{
private:
	#include "particle.h"


public:

	// Structure the particles will use to update themselves
	ParticleUpdateInfo pInfo;		

	// Tracking D3D RenderStates for particles
	Direct3D::D3DPointSpriteInfo SpriteInfo;
	
	// Particle initial value ranges
	Vector4D  vColorRangeMin;
	Vector4D  vColorRangeMax;
	Vector		vVelocityRangeMin;
	Vector		vVelocityRangeMax;
	Vector		vPositionRangeMin;
	Vector		vPositionRangeMax;
	
	// Particle inventory
	Particle *pParticles;			
	Direct3D::D3DVertex *pvParticles;

	unsigned int uParticles;			// number now
	unsigned int uMaxParticles;		// max particles allowed
	
	// Timing
	float fEmitRate;
	float fInverseRate;
	float fEmissionTime;
	float	fLifeMin;
	float	fLifeMax;
	int iuLastUpdated;

	// How fast they can be emitted
	private: 
		static float sfRateScale;
	public:	
	
	// Texture file
	char strTextureFile[200];
	
	// Emitter properties
	bool bEmitting;
	
	// Required for base
	void ResetBoundingShapeColor(void)	{}    

private:

// Read an effect from a file
	void Emitter::ReadEffect(char *strFile);

public:

	// Set up the base stuff.
	Emitter(void) : Base(BASETYPE_PARTICLE_EMITTER) { }

	// Save an effect to a file
	void Emitter::WriteEffect(char *strFile);
		
	// Set up the emitter
	void Emitter::Initialize(char *strFile);

	// Update & emit particles
	void Emitter::Update(Camera *pCamera);

	// Should we draw the emitter?
	void DrawEmitter(bool bDraw) { RContext->bDraw = bDraw; }

	// Start the emitter
	void StartEmitting(void) { bEmitting = true; } 

	// Stop the emitter
	void StopEmitting(void) { bEmitting = false; }

	// Start the emitter
	void StartUpdating(void) { bUpdate = true; } 

	// Stop the emitter
	void StopUpdating(void) { bUpdate = false; }

	// Draw our particles
	void Emitter::Draw(void);

	// Shut down the emitter and kill all particles
	void	Emitter::Shutdown(void);

	// Set the global scale
	static void	Emitter::ScaleEffects(float fScale) 
	{ 
			if (fScale >= 1.0f) sfRateScale = 1.0f; 
			else if (fScale <= 0.0f) sfRateScale = 0.0f;
			else sfRateScale = fScale;
	}

	// get the global scale
	static float Emitter::GetScale(void)
	{ 
			return sfRateScale;
	}

	// Add a force to the particles
	void ApplyForce(Vector vNewForce)
		{
			pInfo.vForce += vNewForce;
			pInfo.bApplyForce = true;	
		}

	// Set an absolute force
	void SetForce(Vector vNewForce)
		{
			pInfo.vForce = vNewForce;
			pInfo.bApplyForce = true;	
		}

	// Manage memory
	#if _DEBUG
		ManageMemory(Emitter, BASETYPE_PARTICLE_EMITTER);
	#endif

};

#endif
