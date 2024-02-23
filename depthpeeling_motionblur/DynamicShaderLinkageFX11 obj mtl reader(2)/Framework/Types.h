
#ifndef TYPES_H
#define TYPES_H

//#include "D3D11.h"
//#include "xnamath.h"
//#include <DirectXMath.h>
//#include"d3dx9math.h"

#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include "LightHelper.h"

#include <string>
#include <vector>
#include <anim.h>
//#include <xnamath.h>

struct Vertex
{
	Vertex(){};
	Vertex(float x, float y, float z, 
		float tx, float ty, float tz,
		float nx, float ny, float nz, 
		float u, float v)
		: mPosition(x,y,z), mTangent(tx, ty, tz), mNormal(nx,ny,nz), mTexC(u,v){};

	XMFLOAT3 mPosition;
	XMFLOAT3 mTangent;
	XMFLOAT3 mNormal;
	XMFLOAT2 mTexC;
	XMFLOAT4 mWeight;
	float Bones[4];	
};

struct SubsetData
{
	int mStartIndex;
	int mTriangleCount;

	XMFLOAT4X4 mWorld;

	XMFLOAT4 mDiffuse;
	XMFLOAT4 mSpecular;
	XMFLOAT4 mAmbient;
	XMFLOAT4 mEmission;
	float mShininess;

	std::string mDiffuseMapFilePath;
	std::string mNormalMapFilePath;
};

struct MeshData
{
	Vertex* mpVertices;
	DWORD mVertexCount;
	DWORD* mpIndices;
	DWORD mIndexCount;

	DWORD mSubsetCount;
	SubsetData* mpSubsets;

	void Release()
	{
		delete mpVertices;
		delete mpIndices;
		delete mpSubsets;
	}
};

struct BoneInfo
{
	XMFLOAT4X4  BoneOffset;
	XMFLOAT4X4  FinalTransformation;

	BoneInfo()
    {
        ZeroMemory(&BoneOffset, sizeof(BoneOffset));
        ZeroMemory(&FinalTransformation, sizeof(FinalTransformation));
    }
};

struct MeshEntry {
	unsigned int NumIndices;
	unsigned int BaseVertex;
	unsigned int BaseIndex;
	unsigned int MaterialIndex;
	ID3D11ShaderResourceView* pTexture;
};

#define NUM_BONES_PER_VEREX 4

struct VertexBoneData
{        
    UINT IDs[NUM_BONES_PER_VEREX];
    float Weights[NUM_BONES_PER_VEREX];

    VertexBoneData()
    {
        Reset();
    };
        
    void Reset()
    {
        ZeroMemory(&IDs, sizeof(IDs));
        ZeroMemory(&Weights, sizeof(Weights));
    }
        
    void AddBoneData(UINT BoneID, float Weight);
};

struct MyVertex
{
	struct BoneData
	{
		float vWeight[4];
		UINT iBones[4];

		BoneData()
		{
			vWeight[0] = vWeight[1] = vWeight[2] = vWeight[3] = 0.0f;
			iBones[0] = iBones[1] = iBones[2] = iBones[3] = 0.0f;
		};
	};

	XMFLOAT3 vPos;
	XMFLOAT3 vNormal;
	XMFLOAT2 vTexCoord;
	BoneData	vBone;	
};

struct CB_VS_PER_OBJECT
{
	XMMATRIX m_mWorldViewProjection;
	XMMATRIX m_mWorld;
};

struct CB_VS_PER_Frame
{
	DirectionalLight mDirLight;
	PointLight mPointLight;
	SpotLight mSpotLight;
	XMFLOAT4 gEyePosW;
	Material mLandMat;
};

class cBone 
{
	public:
		std::string Name;
		XMMATRIX Offset;
		XMMATRIX LocalTransform;
		XMMATRIX GlobalTransform;
		XMMATRIX OriginalLocalTransform;

		cBone* Parent;
		std::vector<cBone*> Children;

		cBone() :Parent(0){}
		~cBone(){ for(size_t i(0); i< Children.size(); i++) delete Children[i]; }
};

class cAnimationChannel{
	public:
		std::string Name;
		std::vector<aiVectorKey> mPositionKeys;
		std::vector<aiQuatKey> mRotationKeys;
		std::vector<aiVectorKey> mScalingKeys;
};

#endif