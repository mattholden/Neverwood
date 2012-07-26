
#ifndef StormForge_Matt_DXShapes
#define StormForge_Matt_DXShapes

#include "Direct3D.h"
#include "Matrix.h"

#define SHAPE_SLICES 8
#define SHAPE_STACKS 8

class Shapes
{
private:
	static LPD3DXMESH pMesh;	

	//ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/structures/d3dmaterial9.htm
	static Direct3D::D3DMaterial d3dMaterial;

	//////////////////////////////
	// Draw the mesh
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3dx/interfaces/id3dxbasemesh/drawsubset.htm
	static void DrawMesh(Matrix *mGlobal);

public:

	/////////////////////////////////
	// Set ambient/diffuse color
	// //ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/structures/d3dmaterial9.htm
	static void SetColor(float fRed, float fGreen, float fBlue, float fAlpha = 1.0f)
	{
		d3dMaterial.SetColor(fRed, fGreen, fBlue, fAlpha);
	}

	
	/////////////////////////////////
	// Set Specular color and power
	// //ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/structures/d3dmaterial9.htm
	static void SetSpecularColor(float fRed, float fGreen, float fBlue, float fAlpha = 1.0f, float fPower = 8.0f)
	{
		d3dMaterial.SetSpecularColor(fRed, fGreen, fBlue, fAlpha, fPower);
	}

	/////////////////////////////////
	// Set Emission
	// //ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/structures/d3dmaterial9.htm
	static void SetEmission(float fRed, float fGreen, float fBlue, float fAlpha = 1.0f)
	{
		d3dMaterial.SetEmission(fRed, fGreen, fBlue, fAlpha);
	}

	/////////////////////////////////
	// Create a cylinder and draw it
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3dx/functions/shape/d3dxcreatecylinder.htm
	static void Cylinder(Matrix *mMatrix, float fRad1, float fRad2, float fLength, unsigned int iSlices = SHAPE_SLICES, unsigned int iStacks = SHAPE_STACKS)
	{
		// Make shape
		D3DXCreateCylinder(Direct3D::GetDevice(), fRad1, fRad2, fLength, iSlices, iStacks, &pMesh, NULL);
		// Draw
		DrawMesh(mMatrix);
		// Prep for next use.
		SAFE_RELEASE(pMesh);
	}
	
	/////////////////////////////////
	// Create a sphere and draw it
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3dx/functions/shape/d3dxcreatesphere.htm
	static void Sphere(Matrix *mMatrix, float fRadius, unsigned int iSlices = SHAPE_SLICES, unsigned int iStacks = SHAPE_STACKS)
	{
		// Make shape
		D3DXCreateSphere(Direct3D::GetDevice(), fRadius, iSlices, iStacks, &pMesh, NULL);
		// Draw
		DrawMesh(mMatrix);
		// Prep for next use.
		SAFE_RELEASE(pMesh);
		
	}
	
	/////////////////////////////////
	// Create a cube and draw it
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3dx/functions/shape/d3dxcreatebox.htm
	static void Cube(Matrix *mMatrix, float fWidth, float fHeight, float fDepth)
	{
		// Make shape
		D3DXCreateBox(Direct3D::GetDevice(), fWidth, fHeight, fDepth, &pMesh, NULL);
		// Draw
		DrawMesh(mMatrix);
		// Prep for next use.
		SAFE_RELEASE(pMesh);
		
	}

	/////////////////////////////////
	// Create a torus and draw it
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3dx/functions/shape/d3dxcreatetorus.htm
	static void Torus(Matrix *mMatrix, float fInnerRad, float fOuterRad, unsigned int iSlices = SHAPE_SLICES, unsigned int iStacks = SHAPE_STACKS)
	{
		// Make shape
		D3DXCreateTorus(Direct3D::GetDevice(), fInnerRad, fOuterRad, iSlices, iStacks, &pMesh, NULL);
		// Draw
		DrawMesh(mMatrix);
		// Prep for next use.
		SAFE_RELEASE(pMesh);
		
	}

	/////////////////////////////////
	// Create a teapot and draw it :)
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3dx/functions/shape/d3dxcreateteapot.htm
	static void Teapot(Matrix *mMatrix)
	{
		// Make shape
		D3DXCreateTeapot(Direct3D::GetDevice(), &pMesh, NULL);
		// Draw
		DrawMesh(mMatrix);
		// Prep for next use.
		SAFE_RELEASE(pMesh);

	}


};
#endif