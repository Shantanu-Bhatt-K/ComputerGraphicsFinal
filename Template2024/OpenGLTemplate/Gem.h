#pragma once

#include "Texture.h"
#include "VertexBufferObjectIndexed.h"


class CGem
{
public:
	CGem();
	~CGem();
	void CreateInterleaved(string directory, string front, int sides, float height, float thickness);
	void CreateGrouped(string directory, string front, int sides, float height, float thickness);
	void Render();
	void RenderInstanced(int n);
	void Release();
private:
	UINT m_vao;
	UINT m_vbo;
	UINT m_indices;
	//CVertexBufferObjectIndexed m_vbo;
	CTexture m_texture;
	string m_directory;
	string m_filename;
	int m_numtriangles;

	struct CVertex {
		glm::vec3 vertex_position;
		glm::vec2 texture_position;
		glm::vec3 normal;
	};

};