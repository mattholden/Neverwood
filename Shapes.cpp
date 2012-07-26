
// What a waste of a .cpp...
#include "shapes.h"
#include "Direct3D.h"

ID3DXMesh *Shapes::pMesh = NULL;
Direct3D::D3DMaterial Shapes::d3dMaterial;

	//////////////////////////////
	// Draw the mesh
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3dx/interfaces/id3dxbasemesh/drawsubset.htm
	void Shapes::DrawMesh(Matrix *mGlobal)
	{
		// Make sure we have a good matrix
		if (!mGlobal)
		{
			Matrix mID; // identity
			mGlobal = &mID;
		}

		// Set orientation/position absolutely
		Direct3D::GetMatrixStack()->Push();
		Direct3D::GetMatrixStack()->LoadMatrixClass(*mGlobal);
		Direct3D::GetMatrixStack()->ApplyToWorld();


		// Draw
		
			// Set the shape material active
			d3dMaterial.SetActive();
			
			// Draw
			if (pMesh) pMesh->DrawSubset(0);

			// Go back to the default
			d3dMaterial.SetDefault();
		
		// Restore matrix
		Direct3D::GetMatrixStack()->Pop();
	}

