#include "DXUT.h"
#include "ults.h"

void AiMatrix4x4ToD3DXMatrix(const aiMatrix4x4 *pSource, XMFLOAT4X4*pDest)
{
	for(int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			pDest->m[i][j] = (*pSource)[i][j];
		}
	}
}

void AiVector3DToD3DXVector3(const aiVector3D *pVector3D, XMFLOAT3*pOut)
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

void AiMatrix4x4ToXmmatrix(const aiMatrix4x4 *pSource, XMFLOAT4X4*pDest)
{
	for(int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			pDest->m[i][j] = (*pSource)[i][j];
		}
	}
}

void TransformMatrix(XMMATRIX&out,const aiMatrix4x4 &in){// there is some type of alignment issue with my mat4 and the aimatrix4x4 class, so the copy must be manually
	
	XMFLOAT4X4 _Data;
	XMStoreFloat4x4(&_Data, out);

	_Data._11=in.a1;
	_Data._12=in.a2;
	_Data._13=in.a3;
	_Data._14=in.a4;

	_Data._21=in.b1;
	_Data._22=in.b2;
	_Data._23=in.b3;
	_Data._24=in.b4;

	_Data._31=in.c1;
	_Data._32=in.c2;
	_Data._33=in.c3;
	_Data._34=in.c4;

	_Data._41=in.d1;
	_Data._42=in.d2;
	_Data._43=in.d3;
	_Data._44=in.d4;
}

void ConvertAndTranspose(const aiMatrix4x4 *pSource, XMFLOAT4X4*pDest)
{
	AiMatrix4x4ToD3DXMatrix(pSource, pDest);

	XMMATRIX mTemp = XMLoadFloat4x4(pDest);
	XMMatrixTranspose(mTemp);
}

void Convert_Transpose_Inverse(const aiMatrix4x4 *pSource, XMFLOAT4X4*pDest)
{
	AiMatrix4x4ToD3DXMatrix(pSource, pDest);
	XMMATRIX mTemp = XMLoadFloat4x4(pDest);
	XMMatrixTranspose(mTemp);

	mTemp = XMLoadFloat4x4(pDest);
	XMMatrixInverse(NULL, mTemp);
}