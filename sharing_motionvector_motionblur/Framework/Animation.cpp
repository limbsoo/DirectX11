#pragma once

#include "DXUT.h"
#include "Animation.h"
#include <assimp/Importer.hpp>
#include <assimp\postprocess.h>
#include "ults.h"

Animation::Animation(void)
{
	m_NumBones = 0;
    m_pScene = NULL;
}

Animation::~Animation(void)
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	//for (UINT i=0; i<m_pDiffuseMap.size(); i++)
	//SAFE_RELEASE(m_pDiffuseMap[i]);
	//m_pScene->~aiScene();
}

void VertexBoneData::AddBoneData(UINT BoneID, float Weight)
{
    for (UINT i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(IDs) ; i++) {
        if (Weights[i] == 0.0) {
            IDs[i]     = BoneID;
            Weights[i] = Weight;
            return;
        }        
    }
    
    // should never get here - more bones than we have space for
    assert(0);
}

bool Animation::LoadMesh(const string& Filename, ID3D11Device* pDevice)
{
	//const aiScene* m_pScene = importer.ReadFile(Filename.c_str(), 0 
	//const aiScene* pScene = importer.ReadFile(Filename.c_str(),

	UINT iFlag = aiProcessPreset_TargetRealtime_Quality | 
				 aiProcess_ConvertToLeftHanded;

	

	const aiScene* pScene = m_Importer.ReadFile(Filename.c_str(), iFlag);
	
    if (pScene != NULL) {
		aiNode* pRootNode = pScene->mRootNode;
		bool Ret = InitFromScene(pScene, Filename, pDevice);

		ScaleAsset(pScene);

		// If
		m_AnimationController.Init(pScene);

		m_pScene = m_Importer.GetOrphanedScene();
		//-------------------------------------------------------
    }
    else {
        printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
    }

    return true;
}

bool Animation::InitFromScene(const aiScene* pScene, const string& Filename, ID3D11Device* pDevice)
{
    vector<VertexBoneData> Bones;
    vector<UINT> Indices;

    UINT NumVertices = 0;
    UINT NumIndices = 0;

	m_Entries.resize(pScene->mNumMeshes);

    // Count the number of vertices and indices
    for (UINT i = 0 ; i < m_Entries.size() ; i++) {
        m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;        
        m_Entries[i].NumIndices    = pScene->mMeshes[i]->mNumFaces * 3;
        m_Entries[i].BaseVertex    = NumVertices;
        m_Entries[i].BaseIndex     = NumIndices;
        
        NumVertices += pScene->mMeshes[i]->mNumVertices;
        NumIndices  += m_Entries[i].NumIndices;
    }

	// Reserve space in the vectors for the vertex attributes and indices
    Bones.resize(NumVertices);
    Indices.reserve(NumIndices);
	m_Vertices.reserve(NumVertices);
        
    // Initialize the meshes in the scene one by one
    for (UINT i = 0 ; i < m_Entries.size() ; i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh, Bones, Indices, m_Entries[i].BaseVertex);
    }

	//Create a vertex buffer and an index buffer
	createBuffers(Indices, pDevice);

    if (!InitMaterials(pScene, Filename, pDevice)) {
        return false;
    }

    return true;
}

void Animation::InitMesh(UINT MeshIndex,
                    const aiMesh* paiMesh,
                    vector<VertexBoneData>& Bones,
                    vector<UINT>& Indices, UINT VertexOffset)
{    
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    // Populate the vertex attribute vectors
    for (UINT i = 0 ; i < paiMesh->mNumVertices ; i++) {
        const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		MyVertex v;
		v.vPos = D3DXVECTOR3(pPos->x, pPos->y, pPos->z);
		v.vNormal = D3DXVECTOR3(pNormal->x, pNormal->y, pNormal->z);
		v.vTexCoord = D3DXVECTOR2(pTexCoord->x, pTexCoord->y);
		
		m_Vertices.push_back(v);
    }
    
    LoadBones(MeshIndex, paiMesh, Bones);

    // Populate the index buffer
    for (UINT i = 0 ; i < paiMesh->mNumFaces ; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        //assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }
}

void Animation::LoadBones(UINT MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones)
{
    for (UINT i = 0 ; i < pMesh->mNumBones ; i++) 
	{                
        UINT BoneIndex = 0;        
        string BoneName(pMesh->mBones[i]->mName.data);
        
        if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
            // Allocate an index for a new bone
            BoneIndex = m_NumBones;
            m_NumBones++;            
	        BoneInfo bi;			
			m_BoneInfo.push_back(bi);
			
			aiMatrix4x4 mOffset = pMesh->mBones[i]->mOffsetMatrix;
			ConvertAndTranspose(&mOffset, &(m_BoneInfo[BoneIndex].BoneOffset));

            m_BoneMapping[BoneName] = BoneIndex;
        }
        else {
            BoneIndex = m_BoneMapping[BoneName];
        }                      
        
        for (UINT j = 0 ; j < pMesh->mBones[i]->mNumWeights ; j++) {
            UINT VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
            float Weight  = pMesh->mBones[i]->mWeights[j].mWeight;                   
			
			for (UINT i = 0 ; i < 4; i++) 
			{				
				if (m_Vertices[VertexID].vBone.vWeight[i] <= 0.0) // can be replace "==" with "<="
				{
					m_Vertices[VertexID].vBone.iBones[i] = BoneIndex;
					m_Vertices[VertexID].vBone.vWeight[i] = Weight;
					break;
				}
			}        
		}
	}

	int a = 1;
}

void Animation::createBuffers(vector<UINT>& Indices, ID3D11Device* pDevice)
{
	//----------------------VB and IB----------------------------
	HRESULT hr;
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MyVertex) * m_Vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData = D3D11_SUBRESOURCE_DATA();
	initData.pSysMem = &m_Vertices[0];
	hr = pDevice->CreateBuffer( &bd, &initData, &m_pVertexBuffer );

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(UINT) * Indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initIndexData = D3D11_SUBRESOURCE_DATA();
	initIndexData.pSysMem = &Indices[0];

	hr = pDevice->CreateBuffer( &ibd, &initIndexData, &m_pIndexBuffer );
	//------------------------------------------------------------
}

bool Animation::InitMaterials(const aiScene* pScene, const string& Filename, ID3D11Device* pDevice)
{
	 // Extract the directory part from the file name
    string::size_type SlashIndex = Filename.find_last_of("/");
    string Dir;

    if (SlashIndex == string::npos) {
        Dir = ".";
    }
    else if (SlashIndex == 0) {
        Dir = "/";
    }
    else {
        Dir = Filename.substr(0, SlashIndex);
    }

    bool Ret = true;
	HRESULT hr;

	m_pDiffuseMap.resize(pScene->mNumMaterials);

    // Initialize the materials
    for (UINT i = 0 ; i < pScene->mNumMaterials ; i++) {
        const aiMaterial* pMaterial = pScene->mMaterials[i];

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                string p(Path.data);
                
                if (p.substr(0, 2) == ".\\") {                    
                    p = p.substr(2, p.size() - 2);
                }
                               
                string FullPath = Dir + "/" + p;
                    
				ID3D11ShaderResourceView* pResource;
				hr = D3DX11CreateShaderResourceViewFromFileA(pDevice, FullPath.c_str(),
										NULL, NULL, &pResource, &hr);

                if (hr == E_FAIL) {
                    printf("Error loading texture '%s'\n", FullPath.c_str());					
                    return false;
                }

				//m_pDiffuseMap.push_back(pResource);
				m_pDiffuseMap[i] = pResource;
            }
        }
    }
    return true;
}

int Animation::CalculateBounds(aiNode* piNode, aiVector3D* p_avOut, 
                    const aiMatrix4x4& piMatrix, const aiScene* pcScene)
{
    assert(NULL != piNode);
    assert(NULL != p_avOut);

    aiMatrix4x4 mTemp = piNode->mTransformation;
    mTemp.Transpose();
    const aiMatrix4x4 aiMe = mTemp * piMatrix;

    for (unsigned int i = 0; i < piNode->mNumMeshes;++i)
    {
        for( unsigned int a = 0; a < pcScene->mMeshes[piNode->mMeshes[i]]->mNumVertices;++a)
        {

            const aiVector3D pc = pcScene->mMeshes[piNode->mMeshes[i]]->mVertices[a];
			XMFLOAT3 pc11;
			AiVector3DToXmfloat3(&pc, &pc11);
            
			//XMFLOAT4X4 aiMe1;
			XMMATRIX aiMe1;
			AiMatrix4x4ToXmmatrix(&aiMe, &aiMe1);

            //XMVECTOR pc1 = XMVector3TransformCoord(XMLoadFloat3(&pc11),XMLoadFloat4x4(&aiMe1));
			XMVECTOR pc1 = XMVector3TransformCoord(XMLoadFloat3(&pc11),aiMe1);

            p_avOut[0].x = min( p_avOut[0].x, pc1.m128_f32[0]);
            p_avOut[0].y = min( p_avOut[0].y, pc1.m128_f32[1]);
            p_avOut[0].z = min( p_avOut[0].z, pc1.m128_f32[2]);
            p_avOut[1].x = max( p_avOut[1].x, pc1.m128_f32[0]);
            p_avOut[1].y = max( p_avOut[1].y, pc1.m128_f32[1]);
            p_avOut[1].z = max( p_avOut[1].z, pc1.m128_f32[2]);
        }
    }
    for (unsigned int i = 0; i < piNode->mNumChildren;++i)
    {
        CalculateBounds( piNode->mChildren[i], p_avOut, aiMe, pcScene);
    }
    return 1;
}

//-------------------------------------------------------------------------------
// Scale the asset that it fits perfectly into the viewer window
// The function calculates the boundaries of the mesh and modifies the
// global world transformation matrix according to the aset AABB
//-------------------------------------------------------------------------------
int Animation::ScaleAsset(const aiScene* pcScene)
{
    aiVector3D aiVecs[2] = {
        aiVector3D( 1e10f, 1e10f, 1e10f),
        aiVector3D( -1e10f, -1e10f, -1e10f) };

    if (pcScene->mRootNode)
    {
        aiMatrix4x4 m;
        CalculateBounds(pcScene->mRootNode,aiVecs,m, pcScene);
    }

    aiVector3D vDelta = aiVecs[1] -  aiVecs[0];
    aiVector3D vHalf =  aiVecs[0] + (vDelta / 2.0f);
    float fScale = 10.0f / vDelta.Length();

	float m_scale = 1.0f;
    aiMatrix4x4 mWorld =  aiMatrix4x4(
                            1.0f,0.0f,0.0f,0.0f,
                            0.0f,1.0f,0.0f,0.0f,
                            0.0f,0.0f,1.0f,0.0f,
                            -vHalf.x,-vHalf.y,-vHalf.z,1.0f) *
        aiMatrix4x4(
                    fScale*m_scale,0.0f,0.0f,0.0f,
                    0.0f,fScale*m_scale,0.0f,0.0f,
                    0.0f,0.0f,fScale*m_scale,0.0f,
                    0.0f,0.0f,0.0f,1.0f);
		
	AiMatrix4x4ToD3DXMatrix(&mWorld, &m_mScaling);
    return 1;
}
