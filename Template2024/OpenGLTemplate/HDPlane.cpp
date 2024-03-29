#include "Common.h"
#include "HDPlane.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


HDPlane::HDPlane()
{}

HDPlane::~HDPlane()
{}


// Create the plane, including its geometry, texture mapping, normal, and colour
void HDPlane::Create(string directory, string filename, float width, float height, int resolution)
{

	m_width = width;
	m_height = height;

	// Load the texture
	m_texture.Load(directory + filename, true);

	m_directory = directory;
	m_filename = filename;

	// Set parameters for texturing using sampler object
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);


	// Use VAO to store state associated with vertices
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Create a VBO
	m_vbo.Create();
	m_vbo.Bind();

	float halfWidth = m_width / 2.0f;
	float halfHeight = m_height / 2.0f;

	// Vertex positions
	std::vector<glm::vec3> planeVertices;
	std::vector<glm::vec2> textureValues;
	
	for (int i = 0; i < resolution; i++)
	{
		for (int j = 0; j < resolution; j++)
		{
			glm::vec3 pos = glm::vec3((width * (float)i / (float)resolution) - halfWidth, 0, (height * (float)j / (float)resolution)-halfHeight);
			glm::vec2 texPos = glm::vec2((float)i / (float)resolution, (float)j / (float)resolution);
			glm::vec3 norm(0, 1, 0);
			planeVertices.push_back(pos);
			textureValues.push_back(texPos);
			m_vbo.AddVertexData(&pos, sizeof(glm::vec3));
			m_vbo.AddVertexData(&texPos, sizeof(glm::vec2));
			m_vbo.AddVertexData(&norm, sizeof(glm::vec3));

		}
	}

	// Texture coordinates 

	// Plane normal
	glm::vec3 planeNormal = glm::vec3(0.0f, 1.0f, 0.0f);

	// Put the vertex attributes in the VBO
	for (unsigned int i = 0; i < resolution-1; i++) {
		for (unsigned int j= 0; j < resolution-1; j++) {
			
			unsigned int i0 = i * (unsigned int)resolution + j;
			unsigned int i1 = i * (unsigned int)resolution + j+1;
			unsigned int i2 = (i + 1) * (unsigned int)resolution + j;
			unsigned int i3 = (i + 1) * (unsigned int)resolution + j + 1;
			m_vbo.AddIndexData(&i2, sizeof(unsigned int));
			m_vbo.AddIndexData(&i0,sizeof(unsigned int));
			m_vbo.AddIndexData(&i3,sizeof(unsigned int));

			m_vbo.AddIndexData(&i0, sizeof(unsigned int));
			m_vbo.AddIndexData(&i1, sizeof(unsigned int));
			m_vbo.AddIndexData(&i3, sizeof(unsigned int));
			count += 6;
		}
	}


	// Upload the VBO to the GPU
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	// Set the vertex attribute locations
	GLsizei istride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}

// Render the plane as a triangle strip
void HDPlane::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);

}

// Release resources
void HDPlane::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}