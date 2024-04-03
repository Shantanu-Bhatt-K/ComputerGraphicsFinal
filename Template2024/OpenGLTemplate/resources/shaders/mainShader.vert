#version 400 core

// Structure for matrices
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;

// Structure holding light information:  its position as well as ambient, diffuse, and specular colours
struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
};

// Structure holding material information:  its ambient, diffuse, and specular colours, and shininess
struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};
struct SpotLightInfo
{
vec4 position;
vec3 La;
vec3 Ld;
vec3 Ls;
vec3 direction;
float exponent;
float cutoff;
};
// Lights and materials passed in as uniform variables from client programme
uniform LightInfo light1;
uniform SpotLightInfo spotLight1;
uniform MaterialInfo material1; 
uniform bool isInstanced;
uniform mat4 instanceLocs[20];
uniform bool shouldRender[20];
uniform int instanceCount;

// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;


out vec2 vTexCoord;	// Texture coordinate

out vec3 worldPosition;	// used for skybox
out vec3 eyePos;
out vec4 p;
out vec3 n;
// This function implements the Phong shading model
// The code is based on the OpenGL 4.0 Shading Language Cookbook, Chapter 2, pp. 62 - 63, with a few tweaks. 
// Please see Chapter 2 of the book for a detailed discussion.






// This is the entry point into the vertex shader
void main()
{	

// Save the world position for rendering the skybox
	
	worldPosition = inPosition;

	// Transform the vertex spatial position using 
	if(isInstanced)
	{

	int i=gl_InstanceID;
	
	mat4 shift = instanceLocs[i];

	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * shift*vec4(inPosition, 1.0f);
	
	
	}
	else
	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(inPosition, 1.0f);
	
	// Get the vertex normal and vertex position in eye coordinates
	vec3 vEyeNorm = normalize(matrices.normalMatrix * inNormal);
	vec4 vEyePosition = matrices.modelViewMatrix * vec4(inPosition, 1.0f);
	p=vEyePosition;
	n=vEyeNorm;
	eyePos=vEyePosition.xyz;	
	// Apply the Phong model to compute the vertex colour
	
	// Pass through the texture coordinate
	vTexCoord = inCoord;
} 
	