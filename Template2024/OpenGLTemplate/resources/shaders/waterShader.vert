#version 400 core
int seed=1;
float random() {
   seed = seed * 22695477 + 1;
	return (seed>>16)&0x7fff;
}

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
vec3 tangentVec;
vec3 binormalVec;
vec3 normalVec;
// Lights and materials passed in as uniform variables from client programme
uniform LightInfo light1; 
uniform MaterialInfo material1; 
uniform float t;
// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

// Vertex colour output to fragment shader -- using Gouraud (interpolated) shading
out vec3 vColour;	// Colour computed using reflectance model
out vec2 vTexCoord;	// Texture coordinate

out vec3 worldPosition;	// used for skybox

// This function implements the Phong shading model
// The code is based on the OpenGL 4.0 Shading Language Cookbook, Chapter 2, pp. 62 - 63, with a few tweaks. 
// Please see Chapter 2 of the book for a detailed discussion.
vec3 PhongModel(vec4 eyePosition, vec3 eyeNorm,vec3 pointPos)
{
	vec3 s = normalize(vec3(light1.position - eyePosition));
	//vec3 v = normalize(eyePosition.xyz);
	//vec3 h = normalize(v + s);
	//vec3 n = eyeNorm;
	//vec3 ambientColour = light1.La * material1.Ma;
	//float fDiffuseIntensity = max(dot(s, n), 0.0);
	//vec3 diffuseColour = light1.Ld * material1.Md * fDiffuseIntensity;
	//vec3 specularColour = vec3(0.0);
	//float fSpecularIntensity = 0.0;
	//if (fDiffuseIntensity > 0.0) {
	//	fSpecularIntensity = pow(max(dot(h, n), 0.0), material1.shininess);
	//	specularColour = light1.Ls * material1.Ms * fSpecularIntensity;
	//}
	
	//return diffuseColour+specularColour+ambientColour;

	//vec3 s= normalize(vec3(light1.position-vec4(pointPos,1)));
	return vec3(dot(s,eyeNorm));

}


vec3 UpdatePosition(vec3 p)
{
	vec2 planeVec=vec2(p.x,p.z);
	float seed =0;
	float tangent=0;
	float binormal=0;
	for(int i=0;i<20;i++)
	{
		
		vec2 randVec=vec2(cos(seed),sin(seed));
		float angle=dot(planeVec,randVec)*0.05f*pow(1.5,i) + t*0.001f*pow(1.1,i);
		 p.y+=6*pow(2.718,1*pow(0.6,i)*(sin(angle)-1));
		 tangent+=6*randVec.x*0.07f*pow(1.2,i)*1*pow(0.6,i)*cos(angle)*pow(2.718,1*pow(0.6,i)*(sin(angle)-1));
		 binormal+=6*randVec.y*0.07f*pow(1.2,i)*1*pow(0.6,i)*cos(angle)*pow(2.718,1*pow(0.6,i)*(sin(angle)-1));
		  seed=random();
	}
	tangentVec=normalize(vec3(1,0,tangent));
	binormalVec=normalize(vec3(0,1,binormal));
	normalVec= normalize(cross(tangentVec,binormalVec));
	p.x+=15*tangent;
	p.z+=15*binormal;
	return p;
}
// This is the entry point into the vertex shader
void main()
{	

// Save the world position for rendering the skybox
	worldPosition = inPosition;
	vec3 p = inPosition;
	p=UpdatePosition(p);
	// Transform the vertex spatial position using 
	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(p, 1.0f);
	
	// Get the vertex normal and vertex position in eye coordinates
	vec3 vEyeNorm = normalize(matrices.normalMatrix * normalVec);
	vec4 vEyePosition = matrices.modelViewMatrix * vec4(p, 1.0f);
		
	// Apply the Phong model to compute the vertex colour
	vColour = PhongModel(vEyePosition,vEyeNorm,p);
	
	// Pass through the texture coordinate
	vTexCoord = inCoord;
} 
	