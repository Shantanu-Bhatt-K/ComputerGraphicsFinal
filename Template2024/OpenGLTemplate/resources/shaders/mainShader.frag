#version 400 core
			// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader

out vec4 vOutputColour;		// The output colour
in vec3 eyePos;
in vec4 p;
in vec3 n;
uniform sampler2D sampler0;  // The texture sampler
uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied
uniform bool renderSkybox;
in vec3 worldPosition;

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
uniform SpotLightInfo spotLight1,spotLight2;
uniform MaterialInfo material1; 

vec3 spotLightAmount(vec4 p, vec3 n,SpotLightInfo spotlight)
{
vec3 s = normalize(vec3(spotlight.position - p));
float angle = acos(dot(-s, spotlight.direction));
float cutoff = radians(clamp(spotlight.cutoff, 0.0, 90.0));
vec3 ambient = spotlight.La * material1.Ma;
if (angle < cutoff) {
float spotFactor = pow(dot(-s, spotlight.direction), spotlight.exponent);
vec3 v = normalize(-p.xyz);
vec3 h = normalize(v + s);
float sDotN = max(dot(s, n), 0.0);
vec3 diffuse = spotlight.Ld * material1.Md * sDotN;
vec3 specular = vec3(0.0);
if (sDotN > 0.0)
specular = spotlight.Ls * material1.Ms * pow(max(dot(h, n), 0.0),
material1.shininess);
return ambient + spotFactor*(specular+diffuse) ;
} else
return ambient;

}
vec3 PhongModel(vec4 eyePosition, vec3 eyeNorm)
{
	vec3 s = normalize(vec3(light1.position - eyePosition));
	vec3 v = normalize(-eyePosition.xyz);
	vec3 r = reflect(-s, eyeNorm);
	vec3 n = eyeNorm;
	vec3 ambient = light1.La * material1.Ma;
	float sDotN = max(dot(s, n), 0.0f);
	vec3 diffuse = light1.Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0f);
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (sDotN > 0.0f) 
		specular = light1.Ls * material1.Ms * pow(max(dot(r, v), 0.0f), material1.shininess + eps);
	

	return ambient + diffuse +specular ;

}
void main()
{
	vec3 fogColour=  vec3(0.75f);
	float rho=0.03;
		float d = length(eyePos.xyz);
		float w = exp(-pow(rho*d,2))+pow(smoothstep(0,1000,worldPosition.y),1);

	vec3 vColour=PhongModel(p, normalize(n));
	vec3 spotLightColour1=spotLightAmount(p,normalize(n),spotLight1);
	vec3 spotLightColour2=spotLightAmount(p,normalize(n),spotLight2);
	if (renderSkybox) {
		
		vOutputColour = texture(CubeMapTex, worldPosition)*vec4(vColour, 1.0f);
		vOutputColour.rgb =mix(fogColour, vOutputColour.rgb,w);	

	} else {

		// Get the texel colour from the texture sampler
		vec4 vTexColour = texture(sampler0, vTexCoord);	
		w = exp(-pow(rho*d,2))+pow(smoothstep(0,100,worldPosition.y),1);
		if (bUseTexture)
		{
		vOutputColour= vTexColour*vec4(vColour+spotLightColour1+spotLightColour2, 1.0f);
		
		}
			// Combine object colour and texture 
		else{
		vOutputColour= vec4(vColour+spotLightColour1+spotLightColour2, 1.0f);
		
		}
				// Just use the colour instead
	}
	
	
}
