#version 400 core

in vec3 vColour;			// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader

out vec4 vOutputColour;		// The output colour
in vec3 eyePos;
uniform sampler2D sampler0;  // The texture sampler
uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied
uniform bool renderSkybox;
in vec3 worldPosition;


void main()
{
	vec3 fogColour=  vec3(0.75f);
	float rho=0.03;
		float d = length(eyePos.xyz);
		float w = exp(-pow(rho*d,2))+pow(smoothstep(0,1000,worldPosition.y),1);
	if (renderSkybox) {
		
		vOutputColour = texture(CubeMapTex, worldPosition)*vec4(vColour, 1.0f);;
		vOutputColour.rgb =mix(fogColour, vOutputColour.rgb,w);	

	} else {

		// Get the texel colour from the texture sampler
		vec4 vTexColour = texture(sampler0, vTexCoord);	
		w = exp(-pow(rho*d,2))+pow(smoothstep(0,100,worldPosition.y),1);
		if (bUseTexture)
		{
		vOutputColour= vTexColour*vec4(vColour, 1.0f);
		
		}
			// Combine object colour and texture 
		else{
		vOutputColour= vec4(vColour, 1.0f);
		
		}
				// Just use the colour instead
	}
	
	
}
