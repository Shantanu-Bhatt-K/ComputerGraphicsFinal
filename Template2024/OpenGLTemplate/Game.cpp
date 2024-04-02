/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting, 
 different camera controls, different shaders, etc.
 
 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 

 version 6.0a 29/01/2019
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)

 version 6.1a 13/02/2022 - Sorted out Release mode and a few small compiler warnings
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)

*/


#include "game.h"
int seed = 1;
float random() {
	seed = seed * 22695477 + 1;
	return (seed >> 16) & 0x7fff;
}


// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "HDPlane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "CatmullRom.h"
#include "Gem.h"

// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pPlanarTerrain = NULL;
	m_pFtFont = NULL;
	m_pCar = NULL;
	m_pBlimp = NULL;
	m_pSphere = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_pCatmullRom = NULL;
	m_startLine = NULL;
	powerup = NULL;
	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_pFtFont;
	delete m_pCar;
	delete m_pSphere;
	delete m_pAudio;
	delete m_pCatmullRom;
	delete powerup;
	delete m_startLine;
	delete m_pBlimp;
	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{
	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	/// Create objects
	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pPlanarTerrain = new HDPlane;
	m_pFtFont = new CFreeTypeFont;
	m_pCar = new COpenAssetImportMesh;
	m_pSphere = new CSphere;
	m_pAudio = new CAudio;
	m_pCatmullRom = new CCatmullRom;
	powerup = new CGem;
	m_startLine = new COpenAssetImportMesh;
	m_pBlimp = new COpenAssetImportMesh;
	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height); 
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float) width / (float) height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");
	sShaderFileNames.push_back("waterShader.vert");
	sShaderFileNames.push_back("waterShader.frag");

	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);
	CShaderProgram* pWaterProgram = new CShaderProgram;
	pWaterProgram->CreateProgram();
	pWaterProgram->AddShaderToProgram(&shShaders[4]);
	pWaterProgram->AddShaderToProgram(&shShaders[5]);
	pWaterProgram->LinkProgram();
	m_pShaderPrograms->push_back(pWaterProgram);

	// You can follow this pattern to load additional shaders

	// Create the skybox
	// Skybox downloaded from http://www.akimbo.in/forum/viewtopic.php?f=10&t=9
	m_pSkybox->Create(2500.0f);
	
	// Create the planar terrain
	m_pPlanarTerrain->Create("resources\\textures\\", "grassfloor01.jpg", 2000.0f, 2000.0f, 200); // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013

	m_pFtFont->LoadSystemFont("arial.ttf", 32);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	//Create player body
	m_pCar->Load("resources\\models\\Car\\Car.obj");  // Downloaded from http://www.psionicgames.com/?page_id=24 on 24 Jan 2013
	m_startLine->Load("resources\\models\\StartPost\\StartLine.obj");  // Downloaded from http://opengameart.org/content/horse-lowpoly on 24 Jan 2013
	
	//CreatePowerUp
	powerup->CreateInterleaved("resources\\textures\\", "dirtpile01.jpg", 8, 3.f, 0.7f);
	
	// Create a sphere
	m_pSphere->Create("resources\\textures\\", "dirtpile01.jpg", 25, 25);  // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013
	
	m_pBlimp->Load("resources\\models\\Blimp\\Blimp.obj");
	//glEnable(GL_CULL_FACE);
	
	// Initialise audio and play background music
	m_pAudio->Initialise();
	m_pAudio->LoadEventSound("resources\\Audio\\Boing.wav");					// Royalty free sound from freesound.org
	m_pAudio->LoadMusicStream("resources\\Audio\\DST-Garote.mp3");	// Royalty free music from http://www.nosoapradio.us/
	
	//Create CatmullCentreLine
	m_pCatmullRom->CreateTrack();
	for (int i = 1; i < gemCount+1; i++)
	{
		float dist = m_pCatmullRom->totalDist() * (i) / (gemCount+1);
		glm::vec3 _forward, _up, _pos;
		m_pCatmullRom->Sample(dist, _pos, _up, _forward);
		glm::vec3 _normal = glm::normalize(glm::cross(_forward, _up));
		_up = glm::normalize(glm::cross(_normal, _forward));
		_forward = glm::normalize(_forward);
		glm::vec3 gemPos = _pos+7*sin(0.05f*dist)*_normal;
		glm::mat4 transformMat = glm::mat4(1.0f);
		transformMat = glm::translate(transformMat, gemPos);
		transformMat*= glm::mat4(glm::mat3(_forward, _up, _normal));
		gemPositions[i-1]=transformMat;
		activePositions.push_back(transformMat);

	}
	glm::vec3 _forward, _up, _pos;
	m_pCatmullRom->Sample(0, _pos, _up, _forward);
	glm::vec3 _normal = glm::normalize(glm::cross(_forward, _up));
	_up = glm::normalize(glm::cross(_normal, _forward));
	_forward = glm::normalize(_forward);
	startLineTransform = glm::mat4(1.0f);
	startLineTransform = glm::translate(_pos);
	startLineTransform*= glm::mat4(glm::mat3(_forward, _up, _normal));
	glEnable(GL_MULTISAMPLE);
	
}

// Render method runs repeatedly in a loop
void Game::Render() 
{
	
	// Clear the buffers and enable depth testing (z-buffering)
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);

	
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10; 
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	

	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);

	
	// Set light and materials in main shader program
	glm::vec4 lightPosition1 = glm::vec4(0, 10, 0, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("light1.La", glm::vec3(1.0f));		// Ambient colour of light
	pMainProgram->SetUniform("light1.Ld", glm::vec3(1.0f));		// Diffuse colour of light
	pMainProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.3f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.9f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property
		



	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("renderSkybox", true);
		// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
		glm::vec3 vEye = m_pCamera->GetPosition();
		modelViewMatrixStack.Translate(vEye);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSkybox->Render(cubeMapTextureUnit);
		pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	


	


	// Turn on diffuse + specular materials
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance	

	

	pMainProgram->SetUniform("isInstanced", false);
	// Render the horse 


	
	
	

	// Render the player
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(playerPos);
		modelViewMatrixStack.Rotate(up,-10*m_velocity_x);
		modelViewMatrixStack *= playerOrientation;
		modelViewMatrixStack.Scale(0.3f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		pMainProgram->SetUniform("bUseTexture", true);
		m_pCar->Render();
	modelViewMatrixStack.Pop();

	//render the blimp
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-50));
	modelViewMatrixStack.Scale(2.f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	pMainProgram->SetUniform("bUseTexture", true);
	m_pBlimp->Render();
	modelViewMatrixStack.Pop();
	//render start Line
	modelViewMatrixStack.Push();
	
		modelViewMatrixStack *= startLineTransform;
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top())); 
			m_startLine->Render();
		modelViewMatrixStack.Pop();
	
	//render track line
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	
	m_pCatmullRom->RenderTrack();
	modelViewMatrixStack.Pop();
	
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0.0f, 0.0f, 0.0f));

	pMainProgram->SetUniform("isInstanced", true);
	for (unsigned int i = 0; i < gemCount; i++)
	{
		pMainProgram->SetUniform("instanceLocs[" + std::to_string(i) + "]", activePositions[i]);
#
	}
	pMainProgram->SetUniform("instancedCount", gemCount);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	pMainProgram->SetUniform("bUseTexture", false);
	powerup->RenderInstanced(gemCount);
	modelViewMatrixStack.Pop();
	pMainProgram->SetUniform("isInstanced", false);

	CShaderProgram* pWaterProgram = (*m_pShaderPrograms)[2];
	pWaterProgram->UseProgram();
	pWaterProgram->SetUniform("bUseTexture", false);
	pWaterProgram->SetUniform("sampler0", 0);
	pWaterProgram->SetUniform("t", timer);
	glm::vec4 lightPosition = glm::vec4(0,200,200, 1);
	pWaterProgram->SetUniform("light1.position", viewMatrix* lightPosition); // Position of light source *in eye coordinates*
	pWaterProgram->SetUniform("light1.La", glm::vec3(1.0f));		// Ambient colour of light
	pWaterProgram->SetUniform("light1.Ld", glm::vec3(1.0f));		// Diffuse colour of light
	pWaterProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light
	pWaterProgram->SetUniform("material1.Ma", glm::vec3(0.f,0.0f,0.1f));	// Ambient material reflectance
	pWaterProgram->SetUniform("material1.Md", glm::vec3(0.2f, 0.3f, 0.2f));	// Diffuse material reflectance
	pWaterProgram->SetUniform("material1.Ms", glm::vec3(0.4f, 0.8f, 1.f));	// Specular material reflectance
	pWaterProgram->SetUniform("material1.shininess", 200.0f);
	pWaterProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
	// Render the planar terrain
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0.0f, -650.0f, 0.0f));
	pWaterProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pWaterProgram->SetUniform("matrices.inverseViewMatrix", glm::inverse(m_pCamera->GetViewMatrix()));
	pWaterProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pPlanarTerrain->Render();
	modelViewMatrixStack.Pop();
	

		
	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRate();

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());		

}

// Update method runs repeatedly with the Render method
void Game::Update() 
{
	
	timer += m_dt;

	// Update the camera using the amount of time that has elapsed to avoid framerate dependent motion
	

	m_pCatmullRom->Sample(m_currentDist_y, curp,up,forward);
	forward = glm::normalize(forward);
	up = glm::normalize(up);
	normal = glm::cross(forward, up);
	
	normal = glm::normalize(normal);
	up = glm::cross(normal, forward);
	if(gameStarted)
	{
		Input();
		
		Physics();
		lapTime += m_dt;
		
	}
	else
		if(GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80) {
			gameStarted = true;
		}
	UpdateCamera();
	//m_pCamera->Update(m_dt);
	Collisions();
	totalLaps = m_pCatmullRom->CurrentLap(m_currentDist_y) + reverseLaps;
	if (totalLaps > highestLaps)
	{
		if (lowestLapTime > lapTime)
		{
			lowestLapTime = lapTime;
		}
		highestLaps = totalLaps;
		activePositions.clear();
		gemCount = 20;
		for(int i = 0; i < gemCount; i++)
		{
			activePositions.push_back(gemPositions[i]);
		}
		
		lapTime = 0;
	}
	playerPos = curp-up*0.9f+ normal * m_currentDist_x;

	playerOrientation = glm::mat4(glm::mat3(forward, up,normal));
	m_pAudio->Update();
}



void Game::DisplayFrameRate()
{


	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;
	int width = dimensions.right - dimensions.left;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;
	int displayVel = m_velocity_y * 800;
	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		
		m_pFtFont->Render(20, height - 20, 20, "Speed: %d",displayVel);
		m_pFtFont->Render(20, height - 50, 20, "LAPS: %d",highestLaps);
		m_pFtFont->Render(20, height - 80, 20, "Current Lap: %g seconds", lapTime / 1000);
		if(highestLaps!=0)
		m_pFtFont->Render(20, height - 110, 20, "Fastest Lap: %f seconds",lowestLapTime/1000);
		
		m_pFtFont->Render(width-100, height - 50, 20, "FPS%d", m_framesPerSecond);
	}
}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	/*
	// Fixed timer
	dDt = pHighResolutionTimer->Elapsed();
	if (dDt > 1000.0 / (double) Game::FPS) {
		pHighResolutionTimer->Start();
		Update();
		Render();
	}
	*/
	
	
	// Variable timer
	m_pHighResolutionTimer->Start();
	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();
	

}

void Game::Input()
{
	
		
	m_accel_y = 0;
	m_accel_x = 0;
	if (GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80) {
		m_accel_y += accel_y;
	}

	if (GetKeyState(VK_DOWN) & 0x80 || GetKeyState('S') & 0x80) {
		m_accel_y -= accel_y;

	}
	if (GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80) {
		m_accel_x += accel_x;

	}
	if (GetKeyState(VK_LEFT) & 0x80 || GetKeyState('A') & 0x80) {
		m_accel_x -= accel_x;

	}
	

	//Camera view input

	if (GetKeyState('1') & 0x80)
	{
		cView = top;
	}
	else if (GetKeyState('2') & 0x80)
	{
		cView = side;
	}
	else if (GetKeyState('3') & 0x80)
	{
		cView = third;
	}
}

void Game::Physics()
{
	if (isSpeedingUp)
	{
		speedUpTimer += m_dt / 1000;
		m_accel_y += speedUpTimer / speedUpTime * 0.002f;
		if (speedUpTimer >= speedUpTime)
		{
			isSpeedingUp = false;
		}
	}
	m_accel_y += accel_y * glm::dot(glm::normalize(forward), glm::vec3(0, -1, 0));
	m_velocity_y += m_accel_y * m_dt;
	if (m_velocity_y > 0)
	{
		m_velocity_y -= 0.008f * m_dt * pow(m_velocity_y, 2);

	}
	else
	{
		m_velocity_y += 0.008f * m_dt * pow(m_velocity_y, 2);
	}

	m_currentDist_y += m_velocity_y * m_dt;

	if (m_currentDist_y <= 0)
	{

		m_currentDist_y = m_pCatmullRom->totalDist() - m_currentDist_y;
		reverseLaps--;
	}

	m_accel_x += m_velocity_y*accel_x * glm::dot(glm::normalize( normal), glm::vec3(0, -1, 0));
	m_velocity_x += m_accel_x * m_dt;
	if (m_velocity_x > 0)
	{
		m_velocity_x -= 0.3f * m_dt * pow(m_velocity_x, 2);

	}
	else
	{
		m_velocity_x += 0.3f * m_dt * pow(m_velocity_x, 2);
	}

	m_currentDist_x += m_velocity_x * m_dt;
	if (m_currentDist_x <= -8.f)
	{
		m_currentDist_x = -8.f;
		m_velocity_x *= -0.2;
		m_velocity_y *= 0.8f;
	}
	else if (m_currentDist_x >= 8.f)
	{
		m_currentDist_x = 8.f;
		m_velocity_x *= -0.2;
		m_velocity_y *= 0.8f;
	}
	
}

void Game::UpdateCamera()
{

	switch (cView)
	{
	case third:
		m_pCamera->Set(curp + up * 10.f - (forward * (10.f + 40.f * m_velocity_y)), curp + forward * 10.f, up);
		break;
	case top:
		m_pCamera->Set(curp + up * (10.f+40*m_velocity_y), curp, forward);
		break;
	case side:
		m_pCamera->Set(curp - normal * (20.f + 40.f * m_velocity_y) +up*10.f, curp, up);
	}

	
	

	
}

void Game::Collisions()
{
	for (int i = 0; i < gemCount; i++)
	{
		if (glm::length(glm::vec3((activePositions[i])[3]) - playerPos) <= 3.f)
		{
			speedUpTimer = 0.f;
			isSpeedingUp = true;
			activePositions.erase(activePositions.begin() + i);
			gemCount--;
		}

	}
}

WPARAM Game::Execute() 
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();

	
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch(w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case '1':
			//m_pAudio->PlayEventSound();
			break;
		case VK_F1:
			//m_pAudio->PlayEventSound();
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return int(game.Execute());
}
