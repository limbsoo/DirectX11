
#include <Types.h>
//#include "d3dx9math.h"
//#include "d3d9.h"

#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

void AiMatrix4x4ToD3DXMatrix(const aiMatrix4x4 *pSource, XMFLOAT4X4*pDest);
void AiVector3DToD3DXVector3(const aiVector3D *pVector3D, XMFLOAT3*pOut);
void AiVector3DToXmfloat3(const aiVector3D *pVector3D, XMFLOAT3* pOut);
void AiColor4DToXmfloat4(const aiColor4D *pSource, XMFLOAT4 *pDest);
void AiMatrix4x4ToXmmatrix(const aiMatrix4x4 *pSource, XMFLOAT4X4*pDest);
void TransformMatrix(XMMATRIX&out,const aiMatrix4x4 &in);
void ConvertAndTranspose(const aiMatrix4x4 *pSource, XMFLOAT4X4*pDest);
void Convert_Transpose_Inverse(const aiMatrix4x4 *pSource, XMFLOAT4X4*pDest);
