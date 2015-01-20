#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// Just need to include main header file
#include "S2D/S2D.h"


#define GHOSTCOUNT 15
#define POINTSCOUNT 5
#define MUNCHIECOUNT 50

#include <time.h>
// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
	
	struct Player 
	{
		// Data to represent Player
		Rect* Position;
		Rect* SourceRect;
		Rect* ChaseRect;
		Texture2D* Texture;
		int Direction;
		int Frame;
		int CurrentFrameTime;
		float SpeedMultiplier;
		bool Dead;
	};

	struct Munchie
	{
		// Data to represent Munchie
		Rect* SourceRect;
		Rect* Position;
		int CurrentFrameTime;
		int Frame;
		int FrameTime;
		bool rKeyDown;
		bool random;
		bool eaten;
		

	};
	struct MovingEnemy
	{
		Rect* posRect;
		Rect* sourceRect;
		Texture2D* texture;
		int Direction;
		int Frame;
		int CurrentFrameTime;
		float speed;
		bool patrol;
		bool chase;
		bool bounce;
		
	};
	struct PowerUp
	{
		Rect* posRect;
		Rect* sourceRect;
		Texture2D* texture;
		int Frame;
		int CurrentFrameTime;
		bool spawned;
		bool active;
		int duration;
		Vector2* powerUpStringPosition;
	};


	struct Menu
	{
		// Data for Menu
		Texture2D* _menuBackground;
		Rect* _menuRectangle;
		Vector2* _menuStringPosition;
		Vector2* _menuRestartStringPos;
		bool _paused;
		bool _pKeyDown;
		bool _gameOver;
		bool _rKeyDown;
		bool _start;
	};

	struct Score
	{
		Vector2* scoreStringPosition;
		int score;
	};

	struct Start
	{
		Rect* backgroundRectPos;
		Texture2D* title;
		Vector2* onePos;
		Vector2* twoPos;
		Vector2* threePos;
		int selectedMode;
		Vector2* selectedModePos;
		Rect* titlePos;
		string name;
		int score;

	};

private:

	Player* _pacman;
	Munchie* _munchie;
	Menu* _menu;
	MovingEnemy* _ghosts;
	Score* _score;
	PowerUp* _powerUp;
	Start* _start;

	
	int gameState;
	int munchieCount;
	int ghostCount;
		
	bool levelComplete;

	bool cooldown;
	int cooldownTimer;

	//Constant data for Game Variables
	const float _cPacmanSpeed;
	const int _cPacmanFrameTime;
	const float _cPacmanMaxSpeed;
	const int _cPowerUpDuration;
	
	Texture2D* munchieTex;
	// Audio
	SoundEffect* _pop;
	SoundEffect* _background;
	SoundEffect* _death;
	SoundEffect* _intro;
	
	// Position for String
	Vector2* _stringPosition;
	
	//Prototypes

		//Input
		void Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState);
		//Check
		void StartMenu(Input::KeyboardState* state);
		void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
		void CheckRestart(Input::KeyboardState* state, Input::Keys restartKey);
		void CheckViewportCollision();
		void CheckMunchieCollisions();
		void CheckGhostCollisions();
		void CheckPowerUp(int elapsedTime);
		void CheckLevelComplete(bool levelComplete);
		//Update
		void UpdatePacman(int elapsedTime);
		void UpdateMunchie(int elapsedTime);
		void UpdateGhosts(int elapsedTime);
		void UpdateGhostsFrame(int elapsedTime);
		void UpdatePowerUpFrame(int elapsedTime);
		void RestartGame(bool levelComplete);

	
	Vector2* patrolPoints[POINTSCOUNT];

	int patrolDest[GHOSTCOUNT];

public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[], int munchieCount, int ghostCount);
	

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
};