
#ifndef StormForge_Matt_Direct3DHelpers
#define  StormForge_Matt_Direct3DHelpers
	
	// Make a matrix stack wrapper with some extra functionality for us
		class D3DMatrixStack : public ID3DXMatrixStack
		{
		public:
			void ApplyToWorld(void);
			void ApplyToView(void); 
			void ApplyToProjection(void);
			void LoadMatrixClass(Matrix &mMatrix);
		};

		// Make a material wrapper with some easier functionality

		class D3DMaterial 
		{
			D3DMATERIAL9 d3dMaterial;
		public:

			/////////////////////////////////
			// Null it out
			D3DMaterial(void) { memset(&d3dMaterial, 0, sizeof(D3DMATERIAL9)); }
				
			/////////////////////////////////
			// Set ambient/diffuse color
			// //ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/structures/d3dmaterial9.htm
			void SetColor(float fRed, float fGreen, float fBlue, float fAlpha = 1.0f);
			
			/////////////////////////////////
			// Set Specular color and power
			// //ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/structures/d3dmaterial9.htm
			void SetSpecularColor(float fRed, float fGreen, float fBlue, float fAlpha = 1.0f, float fPower = 8.0f);

			/////////////////////////////////
			// Set Emission
			// //ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/structures/d3dmaterial9.htm
			void SetEmission(float fRed, float fGreen, float fBlue, float fAlpha = 1.0f);
		
			/////////////////////////////////
			// Set Active (make it the active material)
			// //ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/structures/d3dmaterial9.htm
			void SetActive(void);
		
			/////////////////////////////////
			// Set Default (go back to the material stored in Direct3D class)
			// //ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/structures/d3dmaterial9.htm
			void SetDefault(void);
		
		};



		#define D3DVERTEXFVF D3DFVF_XYZ | D3DFVF_DIFFUSE

		struct D3DVertex
		{
			Vector vPosition;
			DWORD dwDiffuseColor;
		};

		
	// Structure for storing the current render states for D3D, to reduce redundant changes
	class D3DPointSpriteInfo
	{
	public:

		float fScaleA, fScaleB, fScaleC;
		float fSize, fMinSize, fMaxSize;
		bool bSprites, bScale;

		D3DPointSpriteInfo(void) { memset(this, 0, sizeof(*this)); }
	};


#endif
