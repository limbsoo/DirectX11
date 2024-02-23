#pragma once
#include <map>
#include <vector>
#include <assert.h>
#include <scene.h>
#include <Importer.hpp>

#include "D3D11.h"
//#include <D3DX11tex.h>
//#include "Types.h"
//#include "SceneAnimator.h"
//#include "cAnimationController.h"

#include<Mesh.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include "LightHelper.h"

#include <string>
#include <vector>


using namespace std;

class Model
{
public:
	Model(void);
	~Model(void);

	void Draw(ID3DX11EffectShaderResourceVariable* m_effect_texture, ID3DX11EffectTechnique* g_pTech, D3DX11_TECHNIQUE_DESC techDesc, ID3D11DeviceContext* pd3dImmediateContext, ID3D11InputLayout* g_pLayout);
	void DrawFSQ(ID3DX11EffectShaderResourceVariable* m_effect_texture, ID3DX11EffectTechnique* g_pTech, D3DX11_TECHNIQUE_DESC techDesc, ID3D11DeviceContext* pd3dImmediateContext, ID3D11InputLayout* g_pLayout);
	bool MakeLoadModel(const string& Filename, ID3D11Device* pDevice, ID3D11DeviceContext* deviceContext);


private:

	vector<Mesh> meshes;

	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader = nullptr;
	std::string directory = "";

	int cnt = 0;

};

