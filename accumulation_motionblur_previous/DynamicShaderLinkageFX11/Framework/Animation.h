#pragma once
#include <map>
#include <vector>
#include <assert.h>
#include <scene.h>
#include <Importer.hpp>

#include "D3D11.h"
//#include <D3DX11tex.h>
#include "Types.h"
#include "SceneAnimator.h"
#include "cAnimationController.h"


using namespace std;

class Animation
{
public:
	Animation(void);
	~Animation(void);

	bool LoadMesh(const string& Filename, ID3D11Device* pDevice);
	UINT NumBones() const
    {
        return m_NumBones;
    }    
	
	//----------------------Getters------------------------------
	vector<XMMATRIX>& GetTransforms(float time)
	{
		return m_AnimationController.GetTransforms(time);
	}
	ID3D11ShaderResourceView* GetTexture(UINT index) const
	{
		return m_pDiffuseMap[index];
	}	
	ID3D11Buffer* GetVB() const 
	{ 
		return m_pVertexBuffer;
	}
	ID3D11Buffer* GetIB() const
	{
		return m_pIndexBuffer;
	}
	vector<MeshEntry> GetMesh() const
	{
		return m_Entries;
	}
	XMFLOAT4X4 GetScalingMatrix()
	{
		return m_mScaling;
	}
	//----------------------End of getters------------------------

//Methods
private:
	bool InitFromScene(const aiScene* pScene, const string& Filename, ID3D11Device* pDevice);
	void InitMesh(UINT MeshIndex,
                  const aiMesh* paiMesh,
                  vector<VertexBoneData>& Bones,
                  vector<unsigned int>& Indices, UINT VertexOffset);
	void LoadBones(UINT MeshIndex, const aiMesh* paiMesh, vector<VertexBoneData>& Bones);
	void createBuffers(vector<UINT>& Indices, ID3D11Device* pDevice);
	bool InitMaterials(const aiScene* pScene, const string& Filename, ID3D11Device* pDevice);
	int CalculateBounds(aiNode* piNode, aiVector3D* p_avOut, 
                    const aiMatrix4x4& piMatrix, const aiScene* pcScene);
	int ScaleAsset(const aiScene* pcScene);

//Members
private:
	vector<MeshEntry> m_Entries; //Submeshes
	vector<MyVertex> m_Vertices;

    map<string, UINT> m_BoneMapping; // maps a bone name to its index
    UINT m_NumBones;
    vector<BoneInfo> m_BoneInfo;

	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	vector<ID3D11ShaderResourceView*> m_pDiffuseMap; //model's textures

	//Scaling the model to fit our viewport
	XMFLOAT4X4 m_mScaling;

	//for getting the full path as loading texture
	string m_sFolder;

	cSceneAnimator m_AnimationController;

	Assimp::Importer m_Importer;
	aiScene* m_pScene;
};

