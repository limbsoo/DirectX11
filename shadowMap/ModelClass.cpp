#include "stdafx.h"
#include "TextureClass.h"
#include "ModelClass.h"

#include<vector>
#include <fstream>
using namespace std;


ModelClass::ModelClass()
{


}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename, char* objFilename)
{
	// �� �����͸� �ε��մϴ�.
	if (!LoadModel(objFilename))
	{
		return false;
	}

	// ���� �� �ε��� ���۸� �ʱ�ȭ�մϴ�.
	if (!InitializeBuffers(device))
	{
		return false;
	}

	// �� ���� �ؽ�ó�� �ε��մϴ�.
	return LoadTexture(device, deviceContext, textureFilename);
}


void ModelClass::Shutdown()
{
	// �� �ؽ��ĸ� ��ȯ�մϴ�.
	ReleaseTexture();

	// ���ؽ� �� �ε��� ���۸� �����մϴ�.
	ShutdownBuffers();
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// �׸��⸦ �غ��ϱ� ���� �׷��� ������ ���ο� �������� �ε��� ���۸� �����ϴ�.
	RenderBuffers(deviceContext);
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}


ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	m_indexCount = m_nNumFace * 3;

	VertexType* vertices = new VertexType[m_indexCount];
	if (!vertices)
	{
		return false;
	}

	// �ε��� �迭�� ����ϴ�.
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	for (int i = 0; i < m_nNumFace; i++) // ���� ���� ��ŭ
	{
		vertices[i * 3].position = XMFLOAT3(m_vertex[m_face[i].m_vertex[0]][0], m_vertex[m_face[i].m_vertex[0]][1], m_vertex[m_face[i].m_vertex[0]][2]);
		vertices[i * 3].texture = XMFLOAT2(m_uv[m_face[i].m_uv[0]][0], m_uv[m_face[i].m_uv[0]][1]);

		vertices[i * 3 + 1].position = XMFLOAT3(m_vertex[m_face[i].m_vertex[1]][0], m_vertex[m_face[i].m_vertex[1]][1], m_vertex[m_face[i].m_vertex[1]][2]);
		vertices[i * 3 + 1].texture = XMFLOAT2(m_uv[m_face[i].m_uv[1]][0], m_uv[m_face[i].m_uv[1]][1]);

		vertices[i * 3 + 2].position = XMFLOAT3(m_vertex[m_face[i].m_vertex[2]][0], m_vertex[m_face[i].m_vertex[2]][1], m_vertex[m_face[i].m_vertex[2]][2]);
		vertices[i * 3 + 2].texture = XMFLOAT2(m_uv[m_face[i].m_uv[2]][0], m_uv[m_face[i].m_uv[2]][1]);


		if (haveFaceNormal)
		{
			vertices[i * 3].normal = XMFLOAT3(m_vertexNormal[m_face[i].m_vertexNormal[0]][0], m_vertexNormal[m_face[i].m_vertexNormal[0]][1], m_vertexNormal[m_face[i].m_vertexNormal[0]][2]);
			vertices[i * 3 + 1].normal = XMFLOAT3(m_vertexNormal[m_face[i].m_vertexNormal[1]][0], m_vertexNormal[m_face[i].m_vertexNormal[1]][1], m_vertexNormal[m_face[i].m_vertexNormal[1]][2]);
			vertices[i * 3 + 2].normal = XMFLOAT3(m_vertexNormal[m_face[i].m_vertexNormal[2]][0], m_vertexNormal[m_face[i].m_vertexNormal[2]][1], m_vertexNormal[m_face[i].m_vertexNormal[2]][2]);
		}

		else
		{
			vertices[i * 3].normal = XMFLOAT3(m_vertexNormal[m_face[i].m_vertex[0]][0], m_vertexNormal[m_face[i].m_vertex[0]][1], m_vertexNormal[m_face[i].m_vertex[0]][2]);
			vertices[i * 3 + 1].normal = XMFLOAT3(m_vertexNormal[m_face[i].m_vertex[1]][0], m_vertexNormal[m_face[i].m_vertex[1]][1], m_vertexNormal[m_face[i].m_vertex[1]][2]);
			vertices[i * 3 + 2].normal = XMFLOAT3(m_vertexNormal[m_face[i].m_vertex[2]][0], m_vertexNormal[m_face[i].m_vertex[2]][1], m_vertexNormal[m_face[i].m_vertex[2]][2]);
		}
	}

	for (int i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}


	// ���� ���� ������ ����ü�� �����մϴ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_indexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ���۸� ����ϴ�.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// ���� �ε��� ������ ����ü�� �����մϴ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// �ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ��մϴ�.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� �����մϴ�.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// �����ǰ� ���� �Ҵ�� ���� ���ۿ� �ε��� ���۸� �����մϴ�.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


void ModelClass::ShutdownBuffers()
{
	// �ε��� ���۸� �����մϴ�.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// ���� ���۸� �����մϴ�.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// ���� ������ ������ �������� �����մϴ�.
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻���� �����մϴ�. ���⼭�� �ﰢ������ �����մϴ�.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ModelClass::LoadModel(char* filename)
{
	char text[256];
	vector<string>line;

	int faceVertex[20];
	int faceNormal[20];
	int faceUV[20];
	int UV;
	int nomal;

	// �� ������ ���ϴ�.
	ifstream fin;
	fin.open(filename);

	// ������ �� �� ������ �����մϴ�.
	if (fin.fail())
	{
		return false;
	}

	while (!fin.eof())
	{
		fin.getline(text, 256);
		line.push_back(text);
	}

	for (int i = 0; i < line.size(); i++)
	{
		if (line[i][0] == 'v' && line[i][1] == ' ')
		{
			sscanf_s(line[i].c_str(), "v %f %f %f", &m_vertex[m_nNumVertex][0], &m_vertex[m_nNumVertex][1], &m_vertex[m_nNumVertex][2]);
			m_nNumVertex++;
		}

		else if (line[i][0] == 'v' && line[i][1] == 't')
		{
			sscanf_s(line[i].c_str(), "vt %f %f", &m_uv[m_nNumTexture][0], &m_uv[m_nNumTexture][1]);
			m_nNumTexture++;
			haveUV = true;
		}

		else if (line[i][0] == 'v' && line[i][1] == 'n')
		{
			sscanf_s(line[i].c_str(), "vn %f %f %f", &m_vertexNormal[m_nNumNormal][0], &m_vertexNormal[m_nNumNormal][1], &m_vertexNormal[m_nNumNormal][2]);
			m_nNumNormal++;
			haveNormal = true;
		}

		else if (i < line.size() && line[i][0] == 'f')
		{
			int j = 0, numVertex = 0;
			for (; j < line[i].size(); j++)
			{
				if (line[i][j] == ' ')
				{
					numVertex++;
				}
			}
			if (line[i][j - 1] == ' ')
			{
				numVertex--;
			}
			m_face[m_nNumFace].m_nNumVertex = numVertex;
			int faceVerNum = 0, faceVerNorNum = 0, faceUVNum = 0;

			if (line[i][1] == ' ' && haveUV && haveNormal)
			{
				haveFaceNormal = true;
				for (int j = 1; j < line[i].size(); j++)
				{
					if (line[i][j] == ' ') {
						sscanf_s(&line[i][j + 1], "%d/%d/%d", &faceVertex[faceVerNum], &faceUV[faceVerNum], &faceNormal[faceVerNum]);
						faceVerNum++;
					}
				}
				for (int j = 0; j < m_face[m_nNumFace].m_nNumVertex; j++)
				{
					m_face[m_nNumFace].m_vertex[j] = faceVertex[j] - 1;
					m_face[m_nNumFace].m_vertexNormal[j] = faceNormal[j] - 1;
					m_face[m_nNumFace].m_uv[j] = faceUV[j] - 1;
				}
			}

			else if (line[i][1] == ' ' && !haveUV && haveNormal)
			{
				haveFaceNormal = true;
				for (int j = 1; j < line[i].size(); j++)
				{
					if (line[i][j] == ' ')
					{
						sscanf_s(&line[i][j + 1], "%d//%d", &faceVertex[faceVerNum], &faceNormal[faceVerNum]);
						faceVerNum++;
					}
				}
				for (int j = 0; j < m_face[m_nNumFace].m_nNumVertex; j++)
				{
					m_face[m_nNumFace].m_vertex[j] = faceVertex[j] - 1;
					m_face[m_nNumFace].m_vertexNormal[j] = faceNormal[j] - 1;
				}
			}

			else
			{
				for (int j = 1; j < line[i].size(); j++)
				{
					if (line[i][j] == ' ')
					{
						sscanf_s(&line[i][j + 1], "%d", &faceVertex[faceVerNum]);
						faceVerNum++;
					}
				}
				for (int j = 0; j < m_face[m_nNumFace].m_nNumVertex; j++)
				{
					m_face[m_nNumFace].m_vertex[j] = faceVertex[j] - 1;
				}
			}
			m_nNumFace++;
		}
	}

	fin.close();

	return true;
}


bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	// �ؽ�ó ������Ʈ�� �����Ѵ�.
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// �ؽ�ó ������Ʈ�� �ʱ�ȭ�Ѵ�.
	return m_Texture->Initialize(device, deviceContext, filename);
}


void ModelClass::ReleaseTexture()
{
	// �ؽ�ó ������Ʈ�� �������Ѵ�.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
}

void ModelClass::SetPosition(float x, float y, float z)
{
	m_position = { x, y, z };
}


void ModelClass::GetPosition(float& x, float& y, float& z)
{
	x = m_position.x;
	y = m_position.y;
	z = m_position.z;
}