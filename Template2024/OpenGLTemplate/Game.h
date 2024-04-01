#pragma once

#include "Common.h"
#include "GameWindow.h"

// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class HDPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class COpenAssetImportMesh;
class CAudio;
class CCatmullRom;
class CGem;

enum CameraView {
	top,
	side,
	third
};
class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void Render();

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	vector <CShaderProgram *> *m_pShaderPrograms;
	HDPlane *m_pPlanarTerrain;
	CFreeTypeFont *m_pFtFont;
	COpenAssetImportMesh *m_pCar;
	CSphere *m_pSphere;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CAudio *m_pAudio;
	CGem* powerup;
	CCatmullRom* m_pCatmullRom;

	//startLine
	COpenAssetImportMesh* m_startLine;
	glm::mat4 startLineTransform;

	// Some other member variables
	double m_dt;
	int m_framesPerSecond;
	bool m_appActive;
	float timer = 0;

	//player variables
	float m_currentDist_y = 0.f, m_currentDist_x = 0.f;
	float accel_y = 0.0003f;
	float m_accel_y, m_velocity_y;

	float accel_x = 0.0001f;
	float m_accel_x, m_velocity_x;


	//camera variables
	CameraView cView = third;
	glm::vec3 cPos;
	glm::vec3 cViewPoint;
	
	
	glm::vec3 playerPos;
	glm::vec3 curp, up,prevforward=glm::vec3(0,0,0), forward, normal;
	glm::mat4 playerOrientation;


	// gems variables
	glm::mat4 gemPositions[20];
	int gemCount = 20;

	// game controller variables;
	int reverseLaps = 0;
	int highestLaps = 0;
	int totalLaps = 0;
	float lapTime = 0;
	float lowestLapTime= std::numeric_limits<float>::max();
	bool gameStarted = false;


public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();

private:
	static const int FPS = 60;
	void DisplayFrameRate();
	void GameLoop();
	void Input();
	void Physics();
	void UpdateCamera();

	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;
	


};
