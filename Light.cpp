


#include "Light.h"

// Initialize static members
Vector LightManager::vSceneAmbient = Vector(0.20f, 0.20f, 0.20f);

char LightManager::icLightCount = 0;
bool LightManager::bLighting = false;
LightManager::Light LightManager::pLights[4];

	// Set defaults and light ID
	LightManager::Light::Light(void)
	{
		memset(&d3dLight, 0, sizeof(d3dLight));
		d3dLight.Ambient = *(D3DCOLORVALUE*)(Vector4D(0.2f, 0.2f, 0.2f, 1.0f).fComp);
		d3dLight.Diffuse = *(D3DCOLORVALUE*)(Vector4D(0.7f, 0.7f, 0.7f, 1.0f).fComp);
		d3dLight.Specular = *(D3DCOLORVALUE*)(Vector4D(1.0f, 1.0f, 1.0f, 1.0f).fComp);
		d3dLight.Type = D3DLIGHT_POINT;
		d3dLight.Attenuation1 = 1.0f;
		d3dLight.Range = 35.0f;
		
		bEnabled = false;
		iLightID = LightManager::icLightCount;
		LightManager::icLightCount++;
     }

	// Toggle the light
	void LightManager::Light::Enable(bool bEnable)
	{
		if (bEnable == bEnabled) 
			return;
		bEnabled = bEnable;
		Direct3D::GetDevice()->LightEnable(iLightID, (bEnable) ? TRUE : FALSE);		
	}

	// Enable OpenGL lighting.
	void LightManager::Enable(bool bEnable)
	{
		if (bEnable == bLighting) 
			return;

		bLighting = bEnable;
		Direct3D::Lighting(bLighting);

	}

		
	// Set the attenuation. Only valid for positional lights, so we will make sure it's positional if you set this.
	void LightManager::Light::SetAttenuation(Vector &vAtten)
	{
		d3dLight.Attenuation0 = vAtten[0];
		d3dLight.Attenuation1 = vAtten[1];
		d3dLight.Attenuation2 = vAtten[2];
	}

	// Set the attenuation. Only valid for positional lights, so we will make sure it's positional if you set this.
	void LightManager::Light::SetAttenuation(float fConstant, float fLinear, float fQuadratic)
	{
		d3dLight.Attenuation0 = fConstant;
		d3dLight.Attenuation1 = fLinear;
		d3dLight.Attenuation2 = fQuadratic;
	}

	
	// Sets the spotlight's angles (and makes it a spotlight if needed)
	// Inner is the area inside the cone that should be 100% lit
	// outer is the outer boundary of all light in the cone
	// Falloff is the % of fInner that fOuter is lit. If it's 1.0, no falloff
	// happens, and it's a bit faster
	void LightManager::Light::SetSpotlight(float fInner, float fOuter, float fFalloff)
	{
		// Make spot 
		d3dLight.Type = D3DLIGHT_SPOT;
		
		d3dLight.Theta = fInner;
		d3dLight.Phi = fOuter;
		d3dLight.Falloff = fFalloff;
	}



	// Draws the light's location as a yellow point in space
	void LightManager::Light::DrawPosition(void)
	{
		if (bDrawPosition)
		{
			DWORD dwTemp;
			Direct3D::GetDevice()->GetRenderState(D3DRS_POINTSIZE, &dwTemp);
			Direct3D::GetDevice()->DrawPrimitiveUP(D3DPT_POINTLIST, 1, &d3dLight.Position, 12);
			Direct3D::GetDevice()->SetRenderState(D3DRS_POINTSIZE, dwTemp);
		}
	}

