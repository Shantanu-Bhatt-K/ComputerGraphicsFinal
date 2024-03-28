#version 400 core
int seed=1;
float random() {
   seed = seed * 22695477 + 1;
	return (seed>>16)&0x7fff;
}
in vec3 vColour;			// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader

out vec4 vOutputColour;		// The output colour

uniform sampler2D sampler0;  // The texture sampler
uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied
uniform bool renderSkybox;
in vec3 worldPosition;
uniform float t;

void main()
{
vec2 planeVec=vec2(worldPosition.x,worldPosition.z);
	float seed =0;
	float y=0;
	for(int i=0;i<100;i++)
	{
		
		vec2 randVec=vec2(cos(seed),sin(seed));
		float angle=dot(planeVec,randVec)*0.05f*pow(1.2,i) + t*0.001f*pow(1.2,i);
		 y+=pow(2.718,10*pow(0.8,i)*(sin(angle)-1));
		  seed=random();
	}
	y=pow(y/103,24);
	y=clamp(y,0,1);

	if (renderSkybox) {
		vOutputColour = texture(CubeMapTex, worldPosition);

	} else {

		// Get the texel colour from the texture sampler
		vec4 vTexColour = texture(sampler0, vTexCoord);	

		if (bUseTexture)
			vOutputColour = vTexColour*vec4(vColour, 1.0f);	// Combine object colour and texture 
		else
			vOutputColour =vec4(vColour, 1.0f);// Just use the colour instead
	}
	
	
}
