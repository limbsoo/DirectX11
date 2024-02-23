#pragma once

#define NUM_MAX_VERTEX_PER_FACE 20
#define NUM_MAX_VERTEX 10000
#define NUM_MAX_FACE 10000

class TextureClass;

class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	typedef struct
	{
		int m_nNumVertex;
		//unsigned char m_color[3];
		int m_vertex[NUM_MAX_VERTEX_PER_FACE];

		int m_vertexNormal[NUM_MAX_VERTEX_PER_FACE];

		int m_uv[NUM_MAX_VERTEX_PER_FACE];

	} Face_t;

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();


	bool haveUV = false;
	bool haveNormal = false;
	bool haveFaceNormal = false;

	int m_nNumVertex = 0;
	int m_nNumFace = 0;
	int m_nNumTexture = 0;
	int m_nNumNormal = 0;

	float m_vertex[NUM_MAX_VERTEX][3] = { 0 };
	float m_uv[NUM_MAX_VERTEX][2] = { 0 };
	float m_vertexNormal[NUM_MAX_VERTEX][3] = { 0 };
	Face_t m_face[NUM_MAX_FACE] = { 0 };

	int m_faceNormal[NUM_MAX_FACE][3] = { 0 };

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	bool LoadModel(char* filename);

	void ReleaseTexture();

private:
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	int m_vertexCount = 0;
	int m_indexCount = 0;
	TextureClass* m_Texture = nullptr;
};


