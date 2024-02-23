#pragma once

#include "DXUT.h"
#include "Model.h"
#include <Importer.hpp>
#include <postprocess.h>
//#include "ults.h"

#include "d3dx11.h"

#include "SDKmisc.h"
#include <StringHelper.h>
#include <d3dx11effect.h>

//#include <D3DX11tex.h>

Model::Model(void)
{

}

Model::~Model(void)
{

}

bool Model::MakeLoadModel(const string& Filename, ID3D11Device* pDevice, ID3D11DeviceContext* deviceContext)
{
    this->device = pDevice;
    this->deviceContext = deviceContext;

    this->directory = StringHelper::GetDirectoryFromPath(Filename);

    Assimp::Importer importer;

    const aiScene* pScene = importer.ReadFile(Filename,
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded);

    if (pScene == nullptr)
        return false;


    this->ProcessNode(pScene->mRootNode, pScene);
    return true;
}


void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (UINT i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        aiVector3D aiVertex = node->mTransformation * scene->mMeshes[node->mMeshes[i]]->mVertices[i];
        scene->mMeshes[node->mMeshes[i]]->mVertices[i] = aiVertex;

        meshes.push_back(this->ProcessMesh(mesh, scene));
    }

    for (UINT i = 0; i < node->mNumChildren; i++)
    {
        this->ProcessNode(node->mChildren[i], scene);
    }
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    // Data to fill
    std::vector<MakeVertex> vertices;
    std::vector<DWORD> indices;

    //Get vertices
    for (UINT i = 0; i < mesh->mNumVertices; i++)
    {
        MakeVertex vertex;
        vertex.pos.x = mesh->mVertices[i].x;
        vertex.pos.y = mesh->mVertices[i].y;
        vertex.pos.z = mesh->mVertices[i].z;

        if (mesh->mTextureCoords[0])
        {
            vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
            vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
        }

        vertices.push_back(vertex);

    }

    //Get indices
    for (UINT i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (UINT j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    std::vector<Texture> textures;
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<Texture> diffuseTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, scene);
    textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

    return Mesh(this->device, this->deviceContext, vertices, indices, textures);
}

TextureStorageType Model::DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType)
{
    if (pMat->GetTextureCount(textureType) == 0)
        return TextureStorageType::None;

    aiString path;
    pMat->GetTexture(textureType, index, &path);
    std::string texturePath = path.C_Str();

    //Check if texture is an embedded indexed texture by seeing if the file path is an index #
    if (texturePath[0] == '*')
    {
        if (pScene->mTextures[0]->mHeight == 0)
        {
            return TextureStorageType::EmbeddedIndexCompressed;
        }
        else
        {
            assert("SUPPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSED TEXTURES!" && 0);
            return TextureStorageType::EmbeddedIndexNonCompressed;
        }
    }
    //Check if texture is an embedded texture but not indexed (path will be the texture's name instead of #)

    if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
    {
        if (pTex->mHeight == 0)
        {
            return TextureStorageType::EmbeddedCompressed;
        }
        else
        {
            assert("SUPPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSED TEXTURES!" && 0);
            return TextureStorageType::EmbeddedNonCompressed;
        }
    }

    //Lastly check if texture is a filepath by checking for period before extension name
    if (texturePath.find('.') != std::string::npos)
    {
        return TextureStorageType::Disk;
    }

    return TextureStorageType::None; // No texture exists
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene)
{
    std::vector<Texture> materialTextures;
    TextureStorageType storetype = TextureStorageType::Invalid;
    unsigned int textureCount = pMaterial->GetTextureCount(textureType);

    if (textureCount == 0) //If there are no textures
    {
        storetype = TextureStorageType::None;
        aiColor3D aiColor(0.0f, 0.0f, 0.0f);
        switch (textureType)
        {
        case aiTextureType_DIFFUSE:
            pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
            if (aiColor.IsBlack()) //If color = black, just use grey
            {
                materialTextures.push_back(Texture(this->device, N_Color::UnloadedTextureColor, textureType));
                return materialTextures;
            }
            materialTextures.push_back(Texture(this->device, Color(aiColor.r * 255, aiColor.g * 255, aiColor.b * 255), textureType));
            return materialTextures;
        }
    }
    else
    {
        for (UINT i = 0; i < textureCount; i++)
        {
            aiString path;
            pMaterial->GetTexture(textureType, i, &path);

            //std::string texturePath = path.C_Str();
            //std::string filename = this->directory;

            string tempDirectory = this->directory;
            string tempTextureName = path.C_Str();

            tempDirectory.erase(0, 1);

            for (int i = 0; i < tempTextureName.size(); i++)
            {
                if (tempTextureName[i] == '\\')
                {
                    tempTextureName.erase(tempTextureName.begin() + i);
                    tempTextureName.insert(i, "/");
                }
            }

            if (tempTextureName.find(tempDirectory) != string::npos)
            {
                int index = tempTextureName.find(tempDirectory);
                tempTextureName.erase(tempTextureName.begin(), tempTextureName.begin() + index + tempDirectory.size());
            }


            TextureStorageType storetype = DetermineTextureStorageType(pScene, pMaterial, i, textureType);
            switch (storetype)
            {
            case TextureStorageType::Disk:
            {
                std::string textureName = this->directory + '/' + tempTextureName;

                //texturePath.find(filename);
                //std::string filename = this->directory + '\\' + path.C_Str();
                //Texture diskTexture(this->device, filename, textureType);
                Texture diskTexture(this->device, this->deviceContext, textureName, textureType);
                
                materialTextures.push_back(diskTexture);
                break;
            }
            }
        }
    }

    if (materialTextures.size() == 0)
    {
        materialTextures.push_back(Texture(this->device, N_Color::UnhandledTextureColor, aiTextureType::aiTextureType_DIFFUSE));
    }
    return materialTextures;

}

void Model::Draw(ID3DX11EffectShaderResourceVariable* m_effect_texture, ID3DX11EffectTechnique* g_pTech, D3DX11_TECHNIQUE_DESC techDesc , ID3D11DeviceContext* pd3dImmediateContext, ID3D11InputLayout* g_pLayout)
{
    for (int i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw(m_effect_texture, g_pTech, techDesc, pd3dImmediateContext, g_pLayout);
    }

}

void Model::DrawFSQ(ID3DX11EffectShaderResourceVariable* m_effect_texture, ID3DX11EffectTechnique* g_pTech, D3DX11_TECHNIQUE_DESC techDesc, ID3D11DeviceContext* pd3dImmediateContext, ID3D11InputLayout* g_pLayout)
{
    for (int i = 0; i < meshes.size(); i++)
    {
        meshes[i].DrawFSQ(m_effect_texture, g_pTech, techDesc, pd3dImmediateContext, g_pLayout);
    }

}
