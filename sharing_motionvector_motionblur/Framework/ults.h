#include <assimp/types.h>
#include "Types.h"
#include "d3dx9math.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

void AiMatrix4x4ToD3DXMatrix(const aiMatrix4x4 *pSource, D3DXMATRIX *pDest);
void AiVector3DToD3DXVector3(const aiVector3D *pVector3D, D3DXVECTOR3 *pOut);
void AiVector3DToXmfloat3(const aiVector3D *pVector3D, XMFLOAT3* pOut);
void AiColor4DToXmfloat4(const aiColor4D *pSource, XMFLOAT4 *pDest);
void AiMatrix4x4ToXmmatrix(const aiMatrix4x4 *pSource, XMMATRIX *pDest);
void TransformMatrix(D3DXMATRIX &out,const aiMatrix4x4 &in);
void ConvertAndTranspose(const aiMatrix4x4 *pSource, D3DXMATRIX *pDest);
void Convert_Transpose_Inverse(const aiMatrix4x4 *pSource, D3DXMATRIX *pDest);
