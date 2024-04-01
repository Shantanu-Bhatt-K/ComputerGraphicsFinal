#include "Common.h"
#include<fstream>
#define _USE_MATH_DEFINES
#define BUFFER_OFFSET(i) ((char*)NULL+(i))

#include "Gem.h"
#include <math.h>

CGem::CGem()
{}

CGem::~CGem()
{}

void CGem::CreateInterleaved(string a_sDirectory, string a_sFilename, int sides, float height, float radius)
{
	
	// check if filename passed in -- if so, load texture

	m_texture.Load(a_sDirectory + a_sFilename);

	m_directory = a_sDirectory;
	m_filename = a_sFilename;

	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT); 
	
	
	
	std::vector<glm::vec3> upper_loop;
	std::vector<glm::vec3> lower_loop;
	
	
	// topmost point
	glm::vec3 top_point = glm::vec3(0, height, 0);

	//lowest point
	glm::vec3 bottom_point = glm::vec3(0, 0, 0);
	
	
	// first loop
	for (int i = 0; i < sides; i++)
	{
		glm::vec3 tempVector=glm::vec3(radius * sin(2 * i * glm::pi<float>() / sides), 5*height/6, radius * cos(2 * i * glm::pi<float>() / sides));
		upper_loop.push_back(tempVector);
	}

	//second loop

	for (int i = 0; i < sides; i++)
	{
		glm::vec3 tempVector = glm::vec3(radius * sin(2 * i * glm::pi<float>() / sides), height / 6, radius * cos(2 * i * glm::pi<float>() / sides));
		lower_loop.push_back(tempVector);
	}

	int vertexCount = 0;
	std::vector<CVertex> vertices;
	float perimeter = 2 * radius * sides * sin(glm::pi<float>() / sides);
	float side_length = 2 * height / 3;
	float side_perimeter_ratio = side_length / perimeter;
	float tex_side = side_perimeter_ratio;
	float tex_radius = 1 / (2 * sides * sin(glm::pi<float>() / sides));
	glm::vec2 tex_center(tex_side + tex_radius, tex_side + tex_radius);
	//top part
	for (int i = 0; i < sides; i++)
	{
		//determine the positions, normal and texture coordinate 
		CVertex v1,v2,v3;
		v1.vertex_position= upper_loop[i];
		v2.vertex_position= upper_loop[(i + 1) % sides];
		v3.vertex_position= top_point;
		/*glm::vec3 v1 = upper_loop[i];
		glm::vec3 v2 = upper_loop[(i + 1) % sides];
		glm::vec3 v3 = top_point;*/
		glm::vec3 n = glm::cross(v2.vertex_position - v1.vertex_position, v3.vertex_position - v2.vertex_position);
		v1.normal = n;
		v2.normal = n;
		v3.normal = n;
		v1.texture_position= glm::vec2(tex_radius * sin(2 * i * glm::pi<float>() / sides), tex_radius * cos(2 * i * glm::pi<float>() / sides)) + tex_center;
		v2.texture_position= glm::vec2(tex_radius * sin(2 * (i + 1) * glm::pi<float>() / sides), tex_radius * cos(2 * (i + 1) * glm::pi<float>() / sides)) + tex_center;
		v3.texture_position= tex_center;
		/*glm::vec2 t1 = glm::vec2(0.f, 5.f / 6.f);
		glm::vec2 t2 = glm::vec2(1.f, 5.f / 6.f);
		glm::vec2 t3 = glm::vec2(0.5f, 1.f);*/
		
		//vertex1
		
		vertices.push_back(v1);
		/*m_vbo.AddVertexData(&v1, sizeof(glm::vec3));
		m_vbo.AddVertexData(&t1, sizeof(glm::vec2));
		m_vbo.AddVertexData(&n, sizeof(glm::vec3));*/

		//vertex2
		vertices.push_back(v2);
		/*m_vbo.AddVertexData(&v2, sizeof(glm::vec3));
		m_vbo.AddVertexData(&t2, sizeof(glm::vec2));
		m_vbo.AddVertexData(&n, sizeof(glm::vec3));*/

		//vertex3
		vertices.push_back(v3);
		/*m_vbo.AddVertexData(&v3, sizeof(glm::vec3));
		m_vbo.AddVertexData(&t3, sizeof(glm::vec2));
		m_vbo.AddVertexData(&n, sizeof(glm::vec3));*/

		vertexCount += 3;
	}

	//sides
	for (int i = 0; i < sides; i++)
	{

		CVertex v1, v2, v3,v4;
		v1.vertex_position = upper_loop[i];
		v2.vertex_position = upper_loop[(i + 1) % sides];
		v3.vertex_position = lower_loop[i];
		v4.vertex_position = lower_loop[(i + 1) % sides];

		glm::vec3 n = glm::cross(v2.vertex_position - v4.vertex_position, v3.vertex_position - v4.vertex_position);
		v1.normal = n;
		v2.normal = n;
		v3.normal = n;
		v4.normal = n;


		v1.texture_position = glm::vec2(0, (float)i / sides);
		v2.texture_position = glm::vec2(0, (float)(i + 1) / sides);
		v3.texture_position = glm::vec2(tex_side, (float)i / sides);
		v4.texture_position = glm::vec2(tex_side, (float)(i + 1) / sides);

		
		//first triangle
		//vertex1
		vertices.push_back(v1);
		/*m_vbo.AddVertexData(&v1, sizeof(glm::vec3));
		m_vbo.AddVertexData(&t1, sizeof(glm::vec2));
		m_vbo.AddVertexData(&n, sizeof(glm::vec3));*/

		//vertex2
		vertices.push_back(v3);
		/*m_vbo.AddVertexData(&v3, sizeof(glm::vec3));
		m_vbo.AddVertexData(&t3, sizeof(glm::vec2));
		m_vbo.AddVertexData(&n, sizeof(glm::vec3));*/

		//vertex3
		vertices.push_back(v4);
		/*m_vbo.AddVertexData(&v4, sizeof(glm::vec3));
		m_vbo.AddVertexData(&t4, sizeof(glm::vec2));
		m_vbo.AddVertexData(&n, sizeof(glm::vec3));*/


		//second triangle
		//vertex4
		vertices.push_back(v4);
		/*m_vbo.AddVertexData(&v4, sizeof(glm::vec3));
		m_vbo.AddVertexData(&t4, sizeof(glm::vec2));
		m_vbo.AddVertexData(&n, sizeof(glm::vec3));*/

		//vertex 5
		vertices.push_back(v2);
		/*m_vbo.AddVertexData(&v2, sizeof(glm::vec3));
		m_vbo.AddVertexData(&t2, sizeof(glm::vec2));
		m_vbo.AddVertexData(&n, sizeof(glm::vec3));*/


		//vertex6
		vertices.push_back(v1);
		/*m_vbo.AddVertexData(&v1, sizeof(glm::vec3));
		m_vbo.AddVertexData(&t1, sizeof(glm::vec2));
		m_vbo.AddVertexData(&n, sizeof(glm::vec3));*/

		vertexCount += 6;
	}

	//bottom part
	for (int i = 0; i < sides; i++)
	{
		CVertex v1, v2, v3;
		//determine the positions, normal and texture coordinate 
		v1.vertex_position = lower_loop[i];
		v2.vertex_position = lower_loop[(i + 1) % sides];
		v3.vertex_position = bottom_point;
		glm::vec3 n = glm::cross(v1.vertex_position - v2.vertex_position, v3.vertex_position - v2.vertex_position);
		
		v1.normal = n;
		v2.normal = n;
		v3.normal = n;

		v1.texture_position = glm::vec2(tex_radius * sin(2 * i * glm::pi<float>() / sides), tex_radius * cos(2 * i * glm::pi<float>() / sides)) + tex_center;
		v2.texture_position = glm::vec2(tex_radius * sin(2 * (i + 1) * glm::pi<float>() / sides), tex_radius * cos(2 * (i + 1) * glm::pi<float>() / sides)) + tex_center;
		v3.texture_position = tex_center;
		//vertex1
		vertices.push_back(v2);
		/*m_vbo.AddVertexData(&v2, sizeof(glm::vec3));
		m_vbo.AddVertexData(&t2, sizeof(glm::vec2));
		m_vbo.AddVertexData(&n, sizeof(glm::vec3));*/

		//vertex2
		vertices.push_back(v1);
		/*m_vbo.AddVertexData(&v1, sizeof(glm::vec3));
		m_vbo.AddVertexData(&t1, sizeof(glm::vec2));
		m_vbo.AddVertexData(&n, sizeof(glm::vec3));*/

		//vertex3
		vertices.push_back(v3);
		/*m_vbo.AddVertexData(&v3, sizeof(glm::vec3));
		m_vbo.AddVertexData(&t3, sizeof(glm::vec2));
		m_vbo.AddVertexData(&n, sizeof(glm::vec3));*/

		vertexCount += 3;
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(CVertex), &vertices[0], GL_STATIC_DRAW);
	
	GLsizei stride = sizeof(CVertex);

	//position vectors
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	//texture vectors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));

	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
	
	/*glGenBuffers(1, &m_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices);
	std::vector<unsigned int> indices;
	
	for (unsigned int i = 0; i < vertexCount; i++)
	{
		indices.push_back(i);

	}
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);*/

	m_numtriangles = vertexCount / 3;
	
	
}

void CGem::CreateGrouped(string a_sDirectory, string a_sFilename, int sides, float height, float radius)
{
	// check if filename passed in -- if so, load texture

	m_texture.Load(a_sDirectory + a_sFilename);

	m_directory = a_sDirectory;
	m_filename = a_sFilename;

	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	//for creating obj. uncomment to test
	ofstream myfile;
	myfile.open("gem4.obj");
	myfile << "----------------------------\n";
	myfile << "mtllib gem4.mtl \n\n";
	myfile << "g Mesh\n\n";
	myfile << "usemtl Mesh_Material.001\n\n";



	std::vector<glm::vec3> upper_loop;
	std::vector<glm::vec3> lower_loop;


	// topmost point
	glm::vec3 top_point = glm::vec3(0, height/2, 0);

	//lowest point
	glm::vec3 bottom_point = glm::vec3(0, -height/2, 0);


	// first loop
	for (int i = 0; i < sides; i++)
	{
		glm::vec3 tempVector = glm::vec3(radius * sin(2 * i * glm::pi<float>() / sides), 2 * height / 6, radius * cos(2 * i * glm::pi<float>() / sides));
		upper_loop.push_back(tempVector);
	}

	//second loop
	float perimeter = 2 * radius * sides * sin(glm::pi<float>() / sides);
	float side_length = 2 * height / 3;
	float side_perimeter_ratio = side_length / perimeter;
	float tex_side = side_perimeter_ratio;
	float tex_radius = 1 / (2 * sides * sin(glm::pi<float>() / sides));
	glm::vec2 tex_center(tex_side + tex_radius, tex_side + tex_radius);
	for (int i = 0; i < sides; i++)
	{
		glm::vec3 tempVector = glm::vec3(radius * sin(2 * i * glm::pi<float>() / sides), -2*height / 6, radius * cos(2 * i * glm::pi<float>() / sides));
		lower_loop.push_back(tempVector);
	}
	
	std::vector<glm::vec3> vertex_positions;
	std::vector<glm::vec2> texture_positions;
	std::vector<glm::vec3> normals;
	int vertexCount = 0;

	for (int i = 0; i < sides; i++)
	{
		//determine the positions, normal and texture coordinate 
		
		glm::vec3 v1 = upper_loop[i];
		glm::vec3 v2 = upper_loop[(i + 1) % sides];
		glm::vec3 v3 = top_point;
		glm::vec3 n = glm::cross(v2 - v1, v3 - v2);
		
		glm::vec2 t1 = glm::vec2(tex_radius*sin(2 * i * glm::pi<float>() / sides), tex_radius * cos(2 * i * glm::pi<float>() / sides))+tex_center;
		glm::vec2 t2 = glm::vec2(tex_radius * sin(2 * (i+1) * glm::pi<float>() / sides), tex_radius * cos(2 * (i + 1) * glm::pi<float>() / sides)) + tex_center;
		glm::vec2 t3 = tex_center;

		//vertex1

		vertex_positions.push_back(v1);
		texture_positions.push_back(t1);
		normals.push_back(n);

		//vertex2
		vertex_positions.push_back(v2);
		texture_positions.push_back(t2);
		normals.push_back(n);

		//vertex3
		vertex_positions.push_back(v3);
		texture_positions.push_back(t3);
		normals.push_back(n);

		vertexCount += 3;
	}

	for (int i = 0; i < sides; i++)
	{

		glm::vec3 v1 = upper_loop[i];
		glm::vec3 v2 = upper_loop[(i + 1) % sides];
		glm::vec3 v3 = lower_loop[i];
		glm::vec3 v4 = lower_loop[(i + 1) % sides];

		glm::vec3 n = glm::cross(v2- v4, v3 - v4);
		


		glm::vec2 t1 = glm::vec2(0, (float)i / sides);
		glm::vec2 t2 = glm::vec2(0, (float)(i + 1) / sides);
		glm::vec2 t3 = glm::vec2(tex_side,(float)i/sides);
		glm::vec2 t4 = glm::vec2(tex_side,(float)(i + 1)/sides);

		//first triangle
		//vertex1
		vertex_positions.push_back(v1);
		texture_positions.push_back(t1);
		normals.push_back(n);

		//vertex2
		vertex_positions.push_back(v3);
		texture_positions.push_back(t3);
		normals.push_back(n);

		//vertex3
		vertex_positions.push_back(v4);
		texture_positions.push_back(t4);
		normals.push_back(n);


		//second triangle
		//vertex4
		vertex_positions.push_back(v4);
		texture_positions.push_back(t4);
		normals.push_back(n);

		//vertex 5
		vertex_positions.push_back(v2);
		texture_positions.push_back(t2);
		normals.push_back(n);


		//vertex6
		vertex_positions.push_back(v1);
		texture_positions.push_back(t1);
		normals.push_back(n);

		vertexCount += 6;
	}

	for (int i = 0; i < sides; i++)
	{
		//determine the positions, normal and texture coordinate 

		glm::vec3 v1 = lower_loop[i];
		glm::vec3 v2 = lower_loop[(i + 1) % sides];
		glm::vec3 v3 = bottom_point;
		glm::vec3 n = glm::cross(v1 - v2, v3 - v2);

		glm::vec2 t1 = glm::vec2(tex_radius * sin(2 * i * glm::pi<float>() / sides), tex_radius * cos(2 * i * glm::pi<float>() / sides)) + tex_center;
		glm::vec2 t2 = glm::vec2(tex_radius * sin(2 * (i + 1) * glm::pi<float>() / sides), tex_radius * cos(2 * (i + 1) * glm::pi<float>() / sides)) + tex_center;
		glm::vec2 t3 = tex_center;

		//vertex1

		vertex_positions.push_back(v2);
		texture_positions.push_back(t2);
		normals.push_back(n);

		//vertex2
		vertex_positions.push_back(v1);
		texture_positions.push_back(t1);
		normals.push_back(n);

		//vertex3
		vertex_positions.push_back(v3);
		texture_positions.push_back(t3);
		normals.push_back(n);

		vertexCount += 3;
	}

	////Code for creating obj file uncomment to test
	//for (int i = 0; i < vertexCount; i++)
	//{
	//	myfile << "v " << vertex_positions[i].x << " " << vertex_positions[i].y << " " << vertex_positions[i].z<<endl;
	//}
	//
	//for (int i = 0; i < vertexCount; i++)
	//{
	//	myfile << "vn " << normals[i].x << " " << normals[i].y << " " << normals[i].z << endl;
	//}

	//for (int i = 0; i < vertexCount; i++)
	//{
	//	myfile << "vt " << texture_positions[i].x << " " << texture_positions[i].y << endl;
	//}

	//for (int i = 0; i < vertexCount; i++)
	//{
	//	if (i % 3 == 0)
	//	{
	//		myfile << endl;
	//		myfile << "f ";

	//	}
	//	myfile << i+1 << "/" << i+1 << "/" << i+1 << " ";
	//}
	//myfile << "\n----------------------------\n";
	//myfile.close();
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* vertex_positions.size() + sizeof(glm::vec2) * texture_positions.size() + sizeof(glm::vec3) * normals.size(), NULL, GL_STATIC_DRAW);
	
	
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3)* vertex_positions.size(), &vertex_positions[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* vertex_positions.size(), sizeof(glm::vec2)* texture_positions.size(), &texture_positions[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* vertex_positions.size() + sizeof(glm::vec2) * texture_positions.size(), sizeof(glm::vec3)* normals.size(), &normals[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,0, (const GLvoid*)(sizeof(glm::vec3)*vertex_positions.size()));
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(sizeof(glm::vec3)*vertex_positions.size() + sizeof(glm::vec2)*texture_positions.size()));

	glGenBuffers(1, &m_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices);
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < vertexCount; i++)
	{

		indices.push_back(i);

	}
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	m_numtriangles = vertexCount / 3;
}

void CGem::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLES,0, m_numtriangles * 3 );
}
void CGem::RenderInstanced(int n)
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	glDrawArraysInstanced(GL_TRIANGLES, 0, m_numtriangles * 3,n);
}


void CGem :: Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	

}