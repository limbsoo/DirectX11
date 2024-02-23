#include "DXUT.h"
#include "ults.h"

void AiMatrix4x4ToD3DXMatrix(const aiMatrix4x4 *pSource, D3DXMATRIX *pDest)
{
	for(int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			pDest->m[i][j] = (*pSource)[i][j];
		}
	}
}

void AiVector3DToD3DXVector3(const aiVector3D *pVector3D, D3DXVECTOR3 *pOut)
{
	pOut->x = pVector3D->x;
	pOut->y = pVector3D->y;
	pOut->z = pVector3D->z;
}

void AiVector3DToXmfloat3(const aiVector3D *pVector3D, XMFLOAT3* pOut)
{
	pOut->x = pVector3D->x;
	pOut->y = pVector3D->y;
	pOut->z = pVector3D->z;
}

void AiColor4DToXmfloat4(const aiColor4D *pSource, XMFLOAT4 *pDest)
{
	XMStoreFloat4(pDest, XMVectorSet(pSource->r, pSource->g, pSource->b, pSource->a));
}

void AiMatrix4x4ToXmmatrix(const aiMatrix4x4 *pSource, XMMATRIX *pDest)
{
	for(int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			pDest->m[i][j] = (*pSource)[i][j];
		}
	}
}

void TransformMatrix(D3DXMATRIX &out,const aiMatrix4x4 &in){// there is some type of alignment issue with my mat4 and the aimatrix4x4 class, so the copy must be manually
	out._11=in.a1;
	out._12=in.a2;
	out._13=in.a3;
	out._14=in.a4;

	out._21=in.b1;
	out._22=in.b2;
	out._23=in.b3;
	out._24=in.b4;

	out._31=in.c1;
	out._32=in.c2;
	out._33=in.c3;
	out._34=in.c4;

	out._41=in.d1;
	out._42=in.d2;
	out._43=in.d3;
	out._44=in.d4;
}

void ConvertAndTranspose(const aiMatrix4x4 *pSource, D3DXMATRIX *pDest)
{
	AiMatrix4x4ToD3DXMatrix(pSource, pDest);
	D3DXMATRIX mTemp = *pDest;
	D3DXMatrixTranspose(pDest, &mTemp);
}

void Convert_Transpose_Inverse(const aiMatrix4x4 *pSource, D3DXMATRIX *pDest)
{
	AiMatrix4x4ToD3DXMatrix(pSource, pDest);
	D3DXMATRIX mTemp = *pDest;
	D3DXMatrixTranspose(pDest, &mTemp);

	mTemp = *pDest;
	D3DXMatrixInverse(pDest, NULL, &mTemp);
}