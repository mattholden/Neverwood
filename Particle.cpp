

#include "particle.h"


void Particle::Update(ParticleUpdateInfo *pInfo)
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
			pvParticle.vPosition += (vVelocity * pInfo->fTime);

		// Apply color change if needed
			if (!pInfo->vColorDelta.IsZeroVector())
			{
                vColor += (pInfo->vColorDelta * pInfo->fTime);
				pvParticle.dwColor = FloatColor(vColor[0], vColor[1], vColor[2], vColor[3]);
			}


			// Apply texture coords if needed
				//bool bTextureChanged = false; // As an optimization, only check once
				//Vector4D vNewTexX, vNewTexY;
				//if (!pInfo->vTextureDeltaX.IsZeroVector() || !pInfo->vTextureDeltaY.IsZeroVector())
				//{
				//	bTextureChanged = true;
				//}

		//// Update four positions
		//i *= 4;
		//
		//// Upper left
		//memcpy(pInfo->pGeometry[i].fNormal, pInfo->vNormal.fComp, 12);
		//memcpy(pInfo->pGeometry[i].fPosition, (vPosition + pInfo->vUp - pInfo->vRight).fComp, 12);
		//if (bColorChanged) memcpy(pInfo->pGeometry[i].fColor, vColor.fComp, 16);
		//if (bTextureChanged) 
		//{
		//	pInfo->pGeometry[i].fTextureCoord[0] += (pInfo->vTextureDeltaX.fComp[0] * pInfo->fTime);
		//	pInfo->pGeometry[i].fTextureCoord[1] += (pInfo->vTextureDeltaY.fComp[0] * pInfo->fTime);
		//}

		//// lower left
		//memcpy(pInfo->pGeometry[i + 1].fNormal, pInfo->vNormal.fComp, 12);
		//memcpy(pInfo->pGeometry[i + 1].fPosition, (vPosition - pInfo->vUp - pInfo->vRight).fComp, 12);
		//if (bColorChanged) memcpy(pInfo->pGeometry[i + 1].fColor, vColor.fComp, 16);
		//if (bTextureChanged) 
		//{
		//	pInfo->pGeometry[i + 1].fTextureCoord[0] += (pInfo->vTextureDeltaX.fComp[1] * pInfo->fTime);
		//	pInfo->pGeometry[i + 1].fTextureCoord[1] += (pInfo->vTextureDeltaY.fComp[1] * pInfo->fTime);
		//}

		//// Lower right
		//memcpy(pInfo->pGeometry[i + 2].fNormal, pInfo->vNormal.fComp, 12);
		//memcpy(pInfo->pGeometry[i + 2].fPosition, (vPosition - pInfo->vUp + pInfo->vRight).fComp, 12);
		//if (bColorChanged) memcpy(pInfo->pGeometry[i + 2].fColor, vColor.fComp, 16);
		//if (bTextureChanged) 
		//{
		//	pInfo->pGeometry[i + 2].fTextureCoord[0] += (pInfo->vTextureDeltaX.fComp[2] * pInfo->fTime);
		//	pInfo->pGeometry[i + 2].fTextureCoord[1] += (pInfo->vTextureDeltaY.fComp[2] * pInfo->fTime);
		//}

		//// Upper right
		//memcpy(pInfo->pGeometry[i + 3].fNormal, pInfo->vNormal.fComp, 12);
		//memcpy(pInfo->pGeometry[i + 3].fPosition, (vPosition + pInfo->vUp + pInfo->vRight).fComp, 12);
		//if (bColorChanged) memcpy(pInfo->pGeometry[i + 3].fColor, vColor.fComp, 16);
		//if (bTextureChanged) 
		//{
		//	pInfo->pGeometry[i + 3].fTextureCoord[0] += (pInfo->vTextureDeltaX.fComp[3] * pInfo->fTime);
		//	pInfo->pGeometry[i + 3].fTextureCoord[1] += (pInfo->vTextureDeltaY.fComp[3] * pInfo->fTime);
		//}

	}
