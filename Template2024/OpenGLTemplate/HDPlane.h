
#pragma once

#include "Texture.h"
#include "VertexBufferObjectIndexed.h"

// Class for generating a xz plane of a given size
class HDPlane
{
public:
	HDPlane();
	~HDPlane();
	void Create(string sDirectory, string sFilename, float fWidth, float fHeight, int resolution);
	void Render();
	void Release();
private:
	UINT m_vao;
	CVertexBufferObjectIndexed m_vbo;
	CTexture m_texture;
	string m_directory;
	string m_filename;
	float m_width;
	float m_height;
	int count = 0;
};