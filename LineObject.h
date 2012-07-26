//////////////////////////////////////
// LineObject.h
//
// a line object represents a line as a base object
// the primary purpose for LineObject is to draw debug information to the screen
//////////////////////////////////////

#include "Base.h"
class LineObject: public Base
{
	LineObject(): Base (BASETYPE_LINEOBJECT)
	{
		pModel = new DXModel();
		LPDIRECT3DVERTEXBUFFER9 pVertexBuffer = pModel->GetVertexBuffer();
		Direct3D::GetDevice()->CreateVertexBuffer (3*sizeof(float)*2, , D3DFVF_XYZ, D3DPOOL_MANAGED, &pVertexBuffer, 0);
	}

	LineObject (Vector vBegin, Vector vEnd): Base (BASETYPE_LINEOBJECT)
	{
		pModel = new DXModel();
		LPDIRECT3DVERTEXBUFFER9 pVertexBuffer = pModel->GetVertexBuffer();
		Direct3D::GetDevice()->CreateVertexBuffer (3*sizeof(float)*2, , D3DFVF_XYZ, D3DPOOL_MANAGED, &pVertexBuffer, 0);

		float* pVerts;
		pVertexBuffer->Lock (0,0, &pVerts, 0);
		memcpy (pVerts, vBegin.fComp, 12);
		memcpy (pVerts+12, vEnd.fComp, 12);
		pVertexBuffer->Unlock();
	}

	LineObject (Line line): Base (BASETYPE_LINEOBJECT)
	{
		pModel = new DXModel();
		LPDIRECT3DVERTEXBUFFER9 pVertexBuffer = pModel->GetVertexBuffer();
		Direct3D::GetDevice()->CreateVertexBuffer (3*sizeof(float)*2, , D3DFVF_XYZ, D3DPOOL_MANAGED, &pVertexBuffer, 0);

		float* pVerts;
		pVertexBuffer->Lock (0,0, &pVerts, 0);
		memcpy (pVerts, vBegin.fComp, 12);
		memcpy (pVerts+12, vEnd.fComp, 12);
		pVertexBuffer->Unlock();
	}

	virtual void Draw()
	{
		Shapes::SetColor (fRed, fGreen, fBlue);
		Direct3D::GetDevice()->SetStreamSource (0, pModel->GetVertexBuffer, 0, 12);
		Direct3D::GetDevice()->SetFVF (D3DFVF_XYZ);
		Direct3D::GetDevice()->DrawPrimitive (D3DPT_LINELIST, 0,1);
	}
};