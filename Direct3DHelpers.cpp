
#include "Direct3D.h"
#include "Direct3DHelpers.h"

				void Direct3D::D3DMatrixStack::ApplyToWorld(void) { Direct3D::SetWorldMatrix(*GetTop()); }
				void Direct3D::D3DMatrixStack::ApplyToView(void) { Direct3D::SetViewMatrix(*GetTop()); }
				void Direct3D::D3DMatrixStack::ApplyToProjection(void) { Direct3D::SetProjectionMatrix(*GetTop()); }
				void Direct3D::D3DMatrixStack::LoadMatrixClass(Matrix &mMatrix) { LoadMatrix((D3DXMATRIX*)(&mMatrix));  }

				/////////////////////////////////
				// Set ambient/diffuse color
				// //ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/structures/d3dmaterial9.htm
				void Direct3D::D3DMaterial::SetColor(float fRed, float fGreen, float fBlue, float fAlpha)
				{
					d3dMaterial.Ambient.r = d3dMaterial.Diffuse.r = fRed;
					d3dMaterial.Ambient.g = d3dMaterial.Diffuse.g = fGreen;
					d3dMaterial.Ambient.b = d3dMaterial.Diffuse.b = fBlue;
					d3dMaterial.Ambient.a = d3dMaterial.Diffuse.a = fAlpha;
				}

				
				/////////////////////////////////
				// Set Specular color and power
				// //ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/structures/d3dmaterial9.htm
				void Direct3D::D3DMaterial::SetSpecularColor(float fRed, float fGreen, float fBlue, float fAlpha, float fPower)
				{
					d3dMaterial.Specular.r = fRed;
					d3dMaterial.Specular.g = fGreen;
					d3dMaterial.Specular.b = fBlue;
					d3dMaterial.Specular.a = fAlpha;
					d3dMaterial.Power = fPower;
				}

				/////////////////////////////////
				// Set Emission
				// //ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/structures/d3dmaterial9.htm
				void Direct3D::D3DMaterial::SetEmission(float fRed, float fGreen, float fBlue, float fAlpha)
				{
					d3dMaterial.Emissive.r = fRed;
					d3dMaterial.Emissive.g = fGreen;
					d3dMaterial.Emissive.b = fBlue;
					d3dMaterial.Emissive.a = fAlpha;
				}

			/////////////////////////////////
			// Set Active (make it the active material)
			// //ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/structures/d3dmaterial9.htm
				void Direct3D::D3DMaterial::SetActive(void)
				{
					Direct3D::GetDevice()->SetMaterial(&d3dMaterial);
				}

			/////////////////////////////////
			// Set Default (go back to the one in D3D)
			// //ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/structures/d3dmaterial9.htm
				void Direct3D::D3DMaterial::SetDefault(void)
				{
					Direct3D::SetMaterial(NULL);
				}
