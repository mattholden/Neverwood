
#include "base.h"
#include "particle.h"
#include "Vector.h"
#include "Direct3D.h"
#include "timer.h"
#include "Shapes.h"
#include "TextureMgr.h"
#include "MathUtils.h"
#include "Emitter.h"
#include "fileutils.h"
#include "Game.h"

// Set default scale to full
float Emitter::sfRateScale = 1.00f;

// Read an effect from a file
void Emitter::ReadEffect(char *strFile)
	{
		
		FILE *pFile = fopen(strFile, "rb");
		if (!pFile) return;

		// Used to be "little endian" flag
		bool bRead;
		fwrite(&bRead, 1, 1, pFile);
		
		fread(strTextureFile, 200, 1, pFile);

		fread(&fEmitRate, 4, 1, pFile);
		fInverseRate = 1 / fEmitRate;

//		fread(&fQuadHalfHeight, 4, 1, pFile);
//		fread(&fQuadHalfWidth, 4, 1, pFile);
		fread(&fLifeMin, 4, 1, pFile);
		fread(&fLifeMax, 4, 1, pFile);
		fread(vVelocityRangeMin.fComp, 4, 3, pFile);
		fread(vVelocityRangeMax.fComp, 4, 3, pFile);
		Vector vTemp;
		fread(vTemp.fComp, 4, 3, pFile);
		mGlobal.SetPos(vTemp);
		fread(vPositionRangeMin.fComp, 4, 3, pFile);
		fread(vPositionRangeMax.fComp, 4, 3, pFile);
		fread(vColorRangeMin.fComp, 4, 4, pFile);
		fread(vColorRangeMax.fComp, 4, 4, pFile);
		fread(pInfo.vColorDelta.fComp, 4, 4, pFile);

		
		/*fread(vTextureCoordX.fComp, 4, 4, pFile);
		fread(vTextureCoordY.fComp, 4, 4, pFile);
		fread(pInfo.vTextureDeltaX.fComp, 4, 4, pFile);
		fread(pInfo.vTextureDeltaY.fComp, 4, 4, pFile);*/

		fread(&pInfo.bSpiral, 1, 1, pFile);
		fread(&pInfo.bGravity, 1, 1, pFile);
		fread(&pInfo.bApplyForce, 1, 1, pFile);
		
		RContext->bLight = ReadBit(pFile);
		RContext->bAlpha = ReadBit(pFile);
		RContext->bTexture = ReadBit(pFile);
		
		fclose(pFile);
	}

	// Save an effect to a file
	void Emitter::WriteEffect(char *strFile)
	{
		FILE *pFile = fopen(strFile, "wb");
		
		// Used to be little endian
		/*bool bWrite = true;
		fwrite(&bWrite, 1, 1, pFile);*/

		fwrite(strTextureFile, 200, 1, pFile);
		fwrite(&fEmitRate, 4, 1, pFile);
		//fwrite(&fQuadHalfHeight, 4, 1, pFile);
		//fwrite(&fQuadHalfWidth, 4, 1, pFile);
		fwrite(&fLifeMin, 4, 1, pFile);
		fwrite(&fLifeMax, 4, 1, pFile);
		fwrite(vVelocityRangeMin.fComp, 4, 3, pFile);
		fwrite(vVelocityRangeMax.fComp, 4, 3, pFile);
		fwrite(mGlobal.GetPos().fComp, 4, 3, pFile);
		fwrite(vPositionRangeMin.fComp, 4, 3, pFile);
		fwrite(vPositionRangeMax.fComp, 4, 3, pFile);
		fwrite(vColorRangeMin.fComp, 4, 4, pFile);
		fwrite(vColorRangeMax.fComp, 4, 4, pFile);
		fwrite(pInfo.vColorDelta.fComp, 4, 4, pFile);
		//fwrite(vTextureCoordX.fComp, 4, 4, pFile);
		//fwrite(vTextureCoordY.fComp, 4, 4, pFile);
		//fwrite(pInfo.vTextureDeltaX.fComp, 4, 4, pFile);
		//fwrite(pInfo.vTextureDeltaY.fComp, 4, 4, pFile);

		fwrite(&pInfo.bSpiral, 1, 1, pFile);
		fwrite(&pInfo.bGravity, 1, 1, pFile);
		fwrite(&pInfo.bApplyForce, 1, 1, pFile);

		WriteBit(pFile, RContext->bLight);
		WriteBit(pFile, RContext->bAlpha);
		WriteBit(pFile, RContext->bTexture);
		
		fclose(pFile);
	}


// Set up the emitter
void Emitter::Initialize(char *strFile)
{

// TESTING CODE
// Used to quickly check out values	
#define HARDCODE 1
#if HARDCODE
		fEmitRate = 150.0f;
		fInverseRate = 1 / fEmitRate;
		fLifeMin = 0.6f;
		fLifeMax = 1.3f;
		
		strcpy(strTextureFile, "textures\\particles\\blue.bmp\0");
		vVelocityRangeMin = Vector(-0.05f, 0.5f, -0.01f);
		vVelocityRangeMax = Vector(0.05f, 0.55f, 0.01f);
		vPositionRangeMin = Vector(-0.0f, -0.000f, -0.00f);
		vPositionRangeMax = Vector(0.0f, 0.00f, 0.00f);
		
		/*vTextureCoordX.fComp[0] = 0.00f;
		vTextureCoordX.fComp[1] = 0.00f;
		vTextureCoordX.fComp[2] = 1.0;
		vTextureCoordX.fComp[3] = 1.0;
		vTextureCoordY.fComp[0] = 1.00f;
		vTextureCoordY.fComp[1] = 0.00f;
		vTextureCoordY.fComp[2] = 0.00f;
		vTextureCoordY.fComp[3] = 1.00f;
		pInfo.vTextureDeltaX.SetToZeroXYZW();
		pInfo.vTextureDeltaY.SetToZeroXYZW();
		*/
		pInfo.vColorDelta.SetToZero();	
		vColorRangeMin = Vector4D(1,1,1,1);
		vColorRangeMax = Vector4D(1,1,1,1);

		SpriteInfo.fSize = 2.0f;
		SpriteInfo.fMinSize = 1.0f;
		SpriteInfo.fMaxSize = 5.0f;
		SpriteInfo.fScaleA = 1.0f;
		SpriteInfo.fScaleB = 1.0f;
		SpriteInfo.fScaleC = 0.0f;
		SpriteInfo.bSprites = true;
		SpriteInfo.bScale = true;


		pInfo.bApplyForce = true;
		pInfo.bGravity = true;
		pInfo.bSpiral = false;
		
		RContext->bAlpha = true;
		RContext->bTexture = true;
		RContext->bLight = true;
#else
	
	ReadEffect(strFile);
	 
#endif		

		//// Load texture
		RContext->iTexture = TextureMgr::LoadTexture(strTextureFile); 		
					
		// Default to not drawing the emitter; this is only for debugging
		RContext->bDraw = false;

		// Emitter starts "on"
		bEmitting = true;
		bUpdate = true;
		
		// Make particles and space for quads
		uMaxParticles = (unsigned int)(fEmitRate * fLifeMax);
		pParticles = new Particle[uMaxParticles];
			
		// Set up forces
		pInfo.vForce.SetToZero();
		pInfo.vGravity = Game::pWorld->vGravity;
				
		// Timer
		fEmissionTime = 0;
		iuLastUpdated = 0;

		pInfo.pvArray = pvParticles;

		// Set up texture coordinates
		//for (unsigned int i = 0; i < uMaxParticles; ++i)
		//{
		//	for (int j = 0; j < 4; j++)
		//	{
		//		pGeometry[i + j].fTextureCoord[0] = vTextureCoordX.fComp[j];
		//		pGeometry[i + j].fTextureCoord[1] = vTextureCoordY.fComp[j];
		//	}
		//}

		

	}


	// Update & emit particles
	void Emitter::Update(Camera *pCamera)
	{

		// Don't update if we shouldn't.
		if (!bUpdate) return;

		// Check the updated time.
		pInfo.fTime = Timer::GetElapsedTime();
		fEmissionTime += pInfo.fTime;

		// Don't make a jillion new ones
		Vector4D vColor;

		// Update only as many particles as the sfRateScale says we can
		for (int i = 0; i < int((uParticles * sfRateScale)); iuLastUpdated++, i++)
		{

			// Loop around if we reach the end of our list
			if (uParticles == iuLastUpdated)
				iuLastUpdated = 0;

			// If it's time for a particle to die...
			if (bEmitting && fEmissionTime >= fInverseRate && pParticles[iuLastUpdated].fLife <= 0.0f)
			{
				fEmissionTime = 0;
				pParticles[iuLastUpdated].Clear();
				pParticles[iuLastUpdated].fLife = (FloatEqual(fLifeMin, fLifeMax)) ? fLifeMin : RandFloat(fLifeMin, fLifeMax, 3);
				pParticles[iuLastUpdated].vVelocity = (vVelocityRangeMin == vVelocityRangeMax) ? vVelocityRangeMax : Vector::GetRandomVector(vVelocityRangeMin, vVelocityRangeMax);
				pvParticles[iuLastUpdated].vPosition = mGlobal.GetPos() + ((vPositionRangeMin == vPositionRangeMax) ? vPositionRangeMax : Vector::GetRandomVector(vPositionRangeMin, vPositionRangeMax));
				vColor = (vColorRangeMin == vColorRangeMax) ? vColorRangeMax : Vector4D::GetRandomVector(vColorRangeMin, vColorRangeMax);
				pvParticles[iuLastUpdated].dwDiffuseColor = FloatColor(vColor[0], vColor[1], vColor[2], vColor[3]);
				
			}

			// If a particle is still alive, update it
			if (pParticles[iuLastUpdated].fLife > 0.0f)
			{ 
				pParticles[iuLastUpdated].Update(&pInfo, iuLastUpdated);
			}
			// If a particle is dead, but we didn't get to update it yet, just get rid of it for now
			else
			{
				pParticles[iuLastUpdated].Clear();
			}		
		}
		
	}

	// Draw our particles
	void Emitter::Draw(void)
	{
		if (RContext->bDrawPosition)
		{
			Shapes::SetColor(0.0f, 1.0f, 0.0f, 0.75f);
			Shapes::Sphere(&mGlobal, 0.025f, 6, 3);
		}

		// Match D3D's settings to ours.
		Direct3D::SetupPointSprites(SpriteInfo.fSize, SpriteInfo.fMinSize, SpriteInfo.fMaxSize, SpriteInfo.fScaleA, SpriteInfo.fScaleB, SpriteInfo.fScaleC);

		// Draw the whole thing.
		Direct3D::GetDevice()->SetFVF(D3DVERTEXFVF);
		Direct3D::GetDevice()->DrawPrimitiveUP(D3DPT_POINTLIST, uParticles, pvParticles, sizeof(Direct3D::D3DVertex));
		
		
	}

	
	// Kill emitter
	void	Emitter::Shutdown(void)
	{
		delete [ ] pParticles;
		pParticles = NULL;
		uMaxParticles = 0;
		uParticles = 0;

		bEmitting = false;
		bUpdate = false;
		
	}



// Particle update
void Particle::Update(ParticleUpdateInfo *pInfo, unsigned int u)
	{
		
		// Get older
		fLife -= pInfo->fTime;

		// Move along velocity.
		if (pInfo->bGravity) 
			vVelocity += (pInfo->vGravity * pInfo->fTime);

		// If spiraling, add the appropriate force
		if (pInfo->bApplyForce)
			vVelocity += (pInfo->bSpiral) ? (pInfo->vForce & vVelocity).Scale(pInfo->fTime) : (pInfo->vForce * pInfo->fTime);
		
		// Add velocity scaled by time to the position to make it move
			pInfo->pvArray[u].vPosition += (vVelocity * pInfo->fTime);

		// Apply color change if needed
			if (!pInfo->vColorDelta.IsZeroVector())
			{
                Vector4D vColor = (pInfo->vColorDelta * pInfo->fTime);
				pInfo->pvArray[u].dwDiffuseColor += FloatColor(vColor[0], vColor[1], vColor[2], vColor[3]);
			}
	}



