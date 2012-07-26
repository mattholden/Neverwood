

// Wrapper for DirectX Lights, and a manager to singleton-ize them

#ifndef StormForge_Matt_Light
#define StormForge_Matt_Light

#include "Direct3D.h"
#include "MemoryMgr.h"
#include "base.h"
#include "Vector.h"
#include "Matrix.h"

#define MAX_LIGHTS 4

// LightManager contains the Light class; it's not available outside this class.
// Why would you ever need to declare a light?
class LightManager
{
private:

class Light
{
	
	friend class LightManager;

	private:
	D3DLIGHT9 d3dLight;
    	
	public:
	bool bDrawPosition;				// If true, we can draw the light's position onscreen
	int iLightID;							// Hardware light #
	bool bEnabled;

	// Set defaults and light ID
	Light(void);

	// Make any changes to the light with Direct3D.
	// This is left as a seperate call so that lighting changes are MUCH
	// faster. The individual calls change only a struct,
	// but we don't engage D3D until this is called.
	// As a result, it must be called to commit any lighting change 
	// you make.
	void Update(void)
	{
		Direct3D::GetDevice()->SetLight(iLightID, &d3dLight);
	}
	// Check if this light is on
	bool IsEnabled(void) { return bEnabled; }

	// Toggle the light.
	void Disable(void) {Enable(false);}

	// Toggle the light.
	void Enable(bool bEnable = true);

	// Draws the light's location as a yellow sphere in space
	void Light::DrawPosition(void);


	// Face a certain direction
	void FaceDirection(Vector vDirection) 
	{ 
		d3dLight.Direction = *(D3DVECTOR*)(vDirection.fComp);
	}

	// Shine on an object
	void LookAt(Base *pObject) 
	{ 
		d3dLight.Direction = *(D3DVECTOR*)(pObject->GetPos().fComp);
	}
	
	// Move the light.
	void MoveTo(Vector vMoveTo)
	{
		d3dLight.Position = *(D3DVECTOR*)(vMoveTo.fComp);
	}

	// Move the light.
	void MoveBy(Vector vMoveBy)
	{
		d3dLight.Position.x += vMoveBy.fComp[0];
		d3dLight.Position.y += vMoveBy.fComp[1];
		d3dLight.Position.z += vMoveBy.fComp[2];
	}

	// Check the light's type.
	// return types:
	//ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/enums/d3dlighttype.htm
	int GetType(void) { return d3dLight.Type; }

	// Set the ambient light.
	void SetAmbient(Vector4D &vAmb)
	{
		d3dLight.Ambient = *(D3DCOLORVALUE*)(vAmb.fComp);
	}
	
	// Set the ambient light.
	void SetAmbient(float fRed, float fGreen, float fBlue, float fAlpha = 1.0f)
	{
		d3dLight.Ambient.r = fRed;
		d3dLight.Ambient.g = fGreen;
		d3dLight.Ambient.b = fBlue;
		d3dLight.Ambient.a = fAlpha;
	}

	// Set the diffuse light.
	void SetDiffuse(Vector4D &vDiff)
	{
		d3dLight.Ambient = *(D3DCOLORVALUE*)(vDiff.fComp);
	}

	// Set the diffuse light.
	void SetDiffuse(float fRed, float fGreen, float fBlue, float fAlpha = 1.0f)
	{
		d3dLight.Diffuse.r = fRed;
		d3dLight.Diffuse.g = fGreen;
		d3dLight.Diffuse.b = fBlue;
		d3dLight.Diffuse.a = fAlpha;
	}

	// Set the specular light.
	void SetSpecular(Vector4D &vSpec)
	{
		d3dLight.Specular = *(D3DCOLORVALUE*)(vSpec.fComp);
	}
	// Set the specular light.
	void SetSpecular(float fRed, float fGreen, float fBlue, float fAlpha = 1.0f)
	{
		d3dLight.Specular.r = fRed;
		d3dLight.Specular.g = fGreen;
		d3dLight.Specular.b = fBlue;
		d3dLight.Specular.a = fAlpha;
	}


	// Set the attenuation. Only valid for positional lights, so we will make sure it's positional if you set this.
	void Light::SetAttenuation(Vector &vAtten);
	// Set the attenuation. Only valid for positional lights, so we will make sure it's positional if you set this.
	void Light::SetAttenuation(float fConstant, float fLinear, float fQuadratic);
	
	// Sets the spotlight's angles (and makes it a spotlight if needed)
	// Inner is the area inside the cone that should be 100% lit
	// outer is the outer boundary of all light in the cone
	// Falloff is the % of fInner that fOuter is lit. If it's 1.0, no falloff
	// happens, and it's a bit faster
	void LightManager::Light::SetSpotlight(float fInner, float fOuter, float fCutoff);
	
	// Set the maximum distance from the light at which the light will shine on the object
	void LightManager::Light::SetRange(float fRange) { d3dLight.Range = fRange; }

	// Make it a directional light.
	void LightManager::Light::MakeDirectional(void) { d3dLight.Type = D3DLIGHT_DIRECTIONAL; }
	// Make it a positional light.
	void LightManager::Light::MakePoint(void) { d3dLight.Type = D3DLIGHT_POINT; }

	// Manage memory
	#if _DEBUG
		ManageMemory(Light, CORETYPE_RENDERER_LIGHT);
	#endif

};


friend class Light;

	// Our actual lights
	static Light pLights[MAX_LIGHTS];

	static Vector		vSceneAmbient;		// The "background scene ambience" light values
	static bool bLighting;				// TRUE if OpenGL lighting is enabled
	static char icLightCount;			// For auto-numbering of lights.
	
public:

	// Manage memory.
	#if _DEBUG
		ManageMemory(LightManager, CORETYPE_RENDERER_LIGHTMGR);
	#endif

	// Get a light. Initialize it if it's NULL.
	static Light *GetLight(int iLight = 0)
	{
		if (iLight < 0 || iLight >= MAX_LIGHTS) return NULL;
		return &pLights[iLight];
	}

	// Set the scene's background ambient light.
	static void SetSceneAmbient(Vector &vSceneAmb)
	{
		memcpy(vSceneAmbient.fComp, vSceneAmb.fComp, 12);
		Direct3D::AmbientColor(VectorColor(vSceneAmb));
	}

	// Set the scene's background ambient light.
	static void SetSceneAmbient(float fRed, float fGreen, float fBlue)
	{
		vSceneAmbient = Vector(fRed, fGreen, fBlue);
		Direct3D::AmbientColor(VectorColor(vSceneAmbient));
	}

	// Set the scene's background ambient light. This is done with one default
	// parameter so that it can be called with empty () to initialize it
	static void SetSceneAmbient(float fNothing = 0.0f)
	{
		Direct3D::AmbientColor(VectorColor(vSceneAmbient));
	}

	// Toggle specular highlights
	static void SpecularHighlights(bool bEnable = false)
	{	Direct3D::GetDevice()->SetRenderState(D3DRS_SPECULARENABLE, (bEnable) ? TRUE : FALSE); }

	// Check if OpenGL lighting is on
	static bool IsEnabled(void) { return bLighting; }
	
	// Enable Direct3D lighting
	static void Enable(bool bEnable = true);
	// Disable Direct3D lighting
	static void Disable(void) { Enable(false); }
	
	
};


#endif
