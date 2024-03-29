#version 400 core
int seed=1;
float random() {
   seed = seed * 22695477 + 1;
	return (seed>>16)&0x7fff;
}
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
	mat4 inverseViewMatrix;
} matrices;	

struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
};
uniform LightInfo light1; 

// Structure holding material information:  its ambient, diffuse, and specular colours, and shininess
struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;


};
uniform MaterialInfo material1; 
// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;
in vec3 eyeN;
in vec3 p;
in vec4 eyeP;// Interpolated texture coordinate using texture coordinate from the vertex shader
in vec3 vColour;
out vec4 vOutputColour;	
// The output colour

uniform sampler2D sampler0;  // The texture sampler
uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied
uniform bool renderSkybox;
in vec3 worldPosition;
uniform float t;

void main()
{
vec2 planeVec=vec2(worldPosition.x,worldPosition.z);
	

	if (renderSkybox) {
		vOutputColour = texture(CubeMapTex, worldPosition);

	} else {

		// Get the texel colour from the texture sampler
		vec4 vTexColour = texture(sampler0, vTexCoord);	
		vec3 refn = normalize(eyeN);
		vec3 refp = eyeP.xyz;
		
		 
		 vec3 reflected = (matrices.inverseViewMatrix * vec4(reflect(refp,refn), 1)).xyz;
		 vec3 refracted=(matrices.inverseViewMatrix * vec4(refract(refp,refn,0.66), 1)).xyz;
		if (bUseTexture)
			vOutputColour = vTexColour*vec4(vColour, 1.0f);	// Combine object colour and texture 
		else
			vOutputColour =vec4(vColour,1.0f)*texture(CubeMapTex,normalize(reflected))+vec4(smoothstep(460,480,p.y),smoothstep(460,470,p.y),smoothstep(460,470,p.y),1);// Just use the colour instead
			
	}
	
	
}
