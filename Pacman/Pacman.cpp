#include "Pacman.h"

#include <sstream>


Pacman::Pacman(int argc, char* argv[], int munchiecount, int ghostcount) : Game(argc, argv), _cPacmanSpeed(0.3f), _cPacmanMaxSpeed(5.0f),
	_cPacmanFrameTime(250), _cPowerUpDuration(300)
{
	srand(time(NULL));
	gameState = 0;
	ghostCount = ghostcount;
	munchieCount = munchiecount;

	_pacman = new Player();
	_pacman->CurrentFrameTime = 0;
	_pacman->Frame = 0;

	_ghosts = new MovingEnemy[ghostCount];
	for (int i = 0; i < ghostCount; i++)
	{
		_ghosts[i].Direction = 0;
		_ghosts[i].speed = 0.2f;
		_ghosts[i].CurrentFrameTime = 0;
		_ghosts[i].Frame = 0;

	}

	_menu = new Menu();
	_menu->_paused = false;
	_menu->_pKeyDown = false;
	_menu->_start = true;
	_score = new Score();

	_start = new Start();



	_munchie= new Munchie[munchieCount];


	//Munchie
	for (int i = 0; i < munchieCount; i++)
	{

		_munchie[i].CurrentFrameTime = 0;
		_munchie[i].Frame = rand() % 1;
		_munchie[i].FrameTime = rand() % 500 + 50;
		_munchie[i].random = false;
		_munchie[i].rKeyDown = false;
		_munchie[i].eaten = false;
	}

	cooldown=false;
	cooldownTimer = 10;

	// Power Up
	_powerUp = new PowerUp();
	_powerUp->CurrentFrameTime = 0;
	_powerUp->Frame = 0;

	Audio::Initialise();

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();

}

Pacman::~Pacman()
{
	delete _pacman->Texture;
	delete _pacman->SourceRect;

	for (int i = 0; i < munchieCount; i++)
	{
		delete munchieTex;
		delete _munchie[i].SourceRect;
	}
}

void Pacman::LoadContent()
{
	levelComplete=false;
	// Load Audio
	_pop = new SoundEffect();
	_pop->Load("Sounds/pop.wav");
	_background = new SoundEffect();
	_background->Load("Sounds/background.wav");
	_death = new SoundEffect();
	_death->Load("Sounds/death.wav");


	// Load Player
	_pacman->Texture = new Texture2D();
	_pacman->Texture->Load("Textures/Fighter.tga", false);
	_pacman->Position = new Rect(350.0f, 350.0f, 32, 32);
	_pacman->SourceRect = new Rect(0.0f, 0.0f, 32, 32);
	_pacman->ChaseRect = new Rect(_pacman->Position->X-200, _pacman->Position->Y-200, 432, 432);
	_pacman->SpeedMultiplier = 1.0f;
	_pacman->CurrentFrameTime = 0;
	_pacman->Direction = 0;
	_pacman->Frame = 0;
	_pacman->Dead = false;


	// Load Munchie
	munchieTex = new Texture2D();
	munchieTex->Load("Textures/Munchie.png", true);



	for (int i = 0; i < munchieCount; i++)
	{

		_munchie[i].SourceRect = new Rect(0, 0, 12, 12);
		do
		{
			_munchie[i].Position = new Rect(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight(), 12, 12);
		}while (_munchie[i].Position->Intersects(*_pacman->Position));
		_munchie[i].eaten = false;

	}

	// Load Ghosts
	for (int i = 0; i < ghostCount; i++)
	{
		_ghosts[i].texture = new Texture2D();
		_ghosts[i].texture->Load("Textures/GhostBlue.png", true);
		do
		{
			_ghosts[i].posRect = new Rect(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight(), 20, 20);
		}
		while (_ghosts[i].posRect->Intersects(*_pacman->ChaseRect));

		_ghosts[i].sourceRect = new Rect(0, 0, 20, 20);



		if (i>=7 && i<12) // Chase Ghosts
		{
			_ghosts[i].chase = true;
			_ghosts[i].texture->Load("Textures/GhostRed.png", true);
		}
		else if (i>=13) // Patrol Ghosts
		{
			_ghosts[i].patrol = true;
			_ghosts[i].posRect = new Rect((rand()%20)*50.0f, 50.0f, 20, 20);
			_ghosts[i].texture->Load("Textures/GhostYel.png", true);
		}
		else //Bounce ghosts
		{
			_ghosts[i].bounce= true;
			_ghosts[i].Direction = ((int) rand()%4);
		}

	}
	// Patrol Points

	patrolPoints[0] = new Vector2(150,150);
	patrolPoints[1] = new Vector2(Graphics::GetViewportWidth()-150,150);
	patrolPoints[2] = new Vector2(Graphics::GetViewportWidth()-150,Graphics::GetViewportHeight()-150);
	patrolPoints[3] = new Vector2(150,Graphics::GetViewportHeight()-150);
	patrolPoints[4] = new Vector2(Graphics::GetViewportWidth()/2,Graphics::GetViewportWidth()/2);

	for (int i = 0; i < ghostCount; i++)
	{
		patrolDest[i] = 0;
	}

	// Power Up
	_powerUp->texture = new Texture2D;
	_powerUp->texture->Load("Textures/Cherry.png", true);
	_powerUp->active=false;
	_powerUp->duration=_cPowerUpDuration;
	_powerUp->powerUpStringPosition = new Vector2(Graphics::GetViewportWidth()/2,50.0f);
	_powerUp->sourceRect = new Rect(0, 0, 32, 32);

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	// Set Menu Paramters
	_menu->_menuBackground = new Texture2D();
	_menu->_menuBackground->Load("Textures/Transparency.png", false);
	_menu->_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight()); 
	_menu->_menuStringPosition = new Vector2((Graphics::GetViewportWidth() / 3.0f), Graphics::GetViewportHeight() / 2.0f);
	_menu->_menuRestartStringPos = new Vector2(50.0f,Graphics::GetViewportHeight()-100);

	//  Start Menu
	_start->backgroundRectPos = new Rect(200.0f, Graphics::GetViewportHeight()/2, 640, (Graphics::GetViewportHeight()/2)-200);
	_start->onePos = new Vector2((_start->backgroundRectPos->X)+30, ((_start->backgroundRectPos->Y)+30));
	_start->twoPos = new Vector2((_start->backgroundRectPos->X)+60, ((_start->backgroundRectPos->Y)+60));
	_start->threePos = new Vector2((_start->backgroundRectPos->X)+90, ((_start->backgroundRectPos->Y)+90));
	_start->titlePos = new Rect(200, 50, 640, 300);
	_start->selectedModePos = new Vector2((_start->backgroundRectPos->X)+30,((_start->backgroundRectPos->Y)+120));
	_start->title = new Texture2D();
	_start->title->Load("Textures/Title.jpg", true);
	_start->selectedMode=0;


	// Score parameters
	_score->scoreStringPosition = new Vector2(Graphics::GetViewportWidth()-400,25.0f);
	_score->score=0;
	_start->score=0;
	

	// Play audio
	Audio::Play(_background);

}

void Pacman::Update(int elapsedTime)

{

	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	Input::MouseState* mouseState = Input::Mouse::GetState();

	//Start Menu
	if (gameState==0)
	{
		StartMenu(keyboardState);
	}
	
	//In-game
	if (gameState!=0)
	{


		CheckPaused(keyboardState, Input::Keys::P);


		if (!_menu->_gameOver)
		{
			if (!_menu->_paused)
			{


				Input(elapsedTime, keyboardState, mouseState);

				UpdatePacman(elapsedTime);

				UpdateGhosts(elapsedTime);

				UpdateMunchie(elapsedTime);

				CheckViewportCollision();

				if(_powerUp->active==false)
				{
					CheckGhostCollisions();
				}

				CheckMunchieCollisions();

				UpdateGhostsFrame(elapsedTime);

				CheckPowerUp(elapsedTime);

				UpdatePowerUpFrame(elapsedTime);

				CheckLevelComplete(levelComplete);

			}

		}
		else
		{
			CheckRestart(keyboardState, Input::Keys::SPACE);
		}
	}


}

void Pacman::StartMenu(Input::KeyboardState* state)
{
	if(!state->IsKeyDown(Input::Keys::RETURN))
	{
		if (state->IsKeyDown(Input::Keys::NUMPAD1))
		{
			_start->selectedMode=1;
		}
		if (state->IsKeyDown(Input::Keys::NUMPAD2))
		{
			_start->selectedMode=2;
		}
		if (state->IsKeyDown(Input::Keys::NUMPAD3))
		{
			_start->selectedMode=3;
		}
	}

	if (state->IsKeyDown(Input::Keys::RETURN))
	{
		//_menu->_start=true;
		gameState = _start->selectedMode;
	}

	if (gameState == 3)
	{
		ghostCount = 25;
		munchieCount = 500;
		RestartGame(true);
	}

	if (gameState==2)
	{
		ghostCount=15;
		munchieCount=5;
		RestartGame(true);
	}

	if(gameState==1)
	{
		ghostCount=5;
		munchieCount=5;
		RestartGame(false);
	}



}
void Pacman::Draw(int elapsedTime)
{
	SpriteBatch::BeginDraw();
	if (gameState == 0)
	{
		std::stringstream optionOne;
		std::stringstream optionTwo;
		std::stringstream optionThree;
		std::stringstream selectedOption;
		optionOne << "1. Level Mode";
		optionTwo << "2. Endless Mode";
		optionThree << "3. Super Awesome Extreme Endless Mode";
		selectedOption << "Selected; " << _start->selectedMode;		
		SpriteBatch::DrawRectangle(_start->backgroundRectPos, Color::Red);
		SpriteBatch::Draw(_start->title,_start->titlePos);
		SpriteBatch::DrawString(optionOne.str().c_str(), _start->onePos, Color::Green);
		SpriteBatch::DrawString(optionTwo.str().c_str(), _start->twoPos, Color::Green);
		SpriteBatch::DrawString(optionThree.str().c_str(), _start->threePos, Color::Green);
		SpriteBatch::DrawString(selectedOption.str().c_str(), _start->selectedModePos, Color::Blue);


	}
	else
	{

		// Allows us to easily create a string
		std::stringstream stream;
		std::stringstream streamScore;
		std::stringstream streamPowerUp;
		stream << "Pacman X: "<< _pacman->Position->X << " Y: " << _pacman->Position->Y << "    " << "Warp Factor: " << _pacman->SpeedMultiplier << " HighScore: "<< _start->score;

		streamScore << "Score:" << _score->score;

		streamPowerUp << "POWER UP ACTIVATED!!! " << _powerUp->duration << " remaining";


		if (!_pacman->Dead)
			SpriteBatch::Draw(_pacman->Texture, _pacman->Position, _pacman->SourceRect); // Draws Pacman

		for (int i = 0; i < ghostCount; i++)
		{
			SpriteBatch::Draw(_ghosts[i].texture, _ghosts[i].posRect, _ghosts[i].sourceRect); // Draws Ghosts
		}

		for (int i = 0; i < munchieCount; i++)
		{
			if (!_munchie[i].eaten)
			{
				SpriteBatch::Draw( munchieTex, _munchie[i].Position, _munchie[i].SourceRect); // Draws Munchies
			}
		}
		//PowerUP
		if (_powerUp->spawned==true)
		{

			SpriteBatch::Draw(_powerUp->texture, _powerUp->posRect, _powerUp->sourceRect); // Draw PowerUp
		}
		if (_powerUp->active==true)
			SpriteBatch::DrawString(streamPowerUp.str().c_str(), _powerUp->powerUpStringPosition, Color::Red);
		// Draws String
		SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
		SpriteBatch::DrawString(streamScore.str().c_str(), _score->scoreStringPosition, Color::Green);

		// Pause Menu
		if (_menu->_paused)
		{
			std::stringstream menuStream;
			menuStream << "PAUSED!";
			SpriteBatch::Draw(_menu->_menuBackground, _menu->_menuRectangle, nullptr);
			SpriteBatch::DrawString(menuStream.str().c_str(), _menu->_menuStringPosition, 
				Color::Red);
		}

		if (_menu->_gameOver)
		{
			std::stringstream menuStream;
			menuStream << "GAME OVER!     Final Score:" << _score->score;
			SpriteBatch::Draw(_menu->_menuBackground, _menu->_menuRectangle, nullptr);
			SpriteBatch::DrawString(menuStream.str().c_str(), _menu->_menuStringPosition, Color::Red);
			SpriteBatch::DrawString("Press 'Space' to restart, or ESC to return to the Main Menu", _menu->_menuRestartStringPos, Color::Red);
		}


	}
	SpriteBatch::EndDraw(); // Ends Drawing
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* state,Input::MouseState* mouseState)
{
	float pacmanSpeed = _cPacmanSpeed * elapsedTime * _pacman->SpeedMultiplier;
	// keyboard inputs
	if (state->IsKeyDown(Input::Keys::D))
	{
		_pacman->Position->X += pacmanSpeed;
		_pacman->Direction= 0;
	}
	else if (state->IsKeyDown(Input::Keys::A))
	{
		_pacman->Position->X -= pacmanSpeed;
		_pacman->Direction = 2;
	}
	else if (state->IsKeyDown(Input::Keys::W))
	{
		_pacman->Position->Y -= pacmanSpeed;
		_pacman->Direction = 3;
	}
	else if (state->IsKeyDown(Input::Keys::S))
	{
		_pacman->Position->Y += pacmanSpeed;
		_pacman->Direction = 1;
	}


	// mouse inputs BLACK HOLE BOMB
	if(mouseState->LeftButton==Input::ButtonState::PRESSED)
	{
		for (int i = 0; i < munchieCount; i++)
		{
			if(_munchie[i].Position->X > mouseState->X)
				_munchie[i].Position->X -= _cPacmanSpeed * elapsedTime;
			else
				_munchie[i].Position->X += _cPacmanSpeed * elapsedTime;

			if(_munchie[i].Position->Y > mouseState->Y)
				_munchie[i].Position->Y -= _cPacmanSpeed * elapsedTime;
			else
				_munchie[i].Position->Y += _cPacmanSpeed * elapsedTime;
		}

		/*		cooldownTimer--;


		}
		if (cooldownTimer<0)
		{
		cooldown = true;
		}
		if (cooldown == true)
		{
		cooldownTimer=cooldownTimer+50;
		if (cooldownTimer>5000)
		{
		cooldown=false;
		cooldownTimer=500;
		}
		}*/
	}

	// speed boost WARP DRIVE
	if (state->IsKeyDown(Input::Keys::LEFTSHIFT))
	{
		_pacman->SpeedMultiplier += 0.001f * elapsedTime; // mouseState->ScrollWheelValue; bugged

		if (_pacman->SpeedMultiplier > _cPacmanMaxSpeed)
		{
			_pacman->SpeedMultiplier = _cPacmanMaxSpeed;
		}
	}
	else
	{
		_pacman->SpeedMultiplier = 1.0f;
	}

	// munchie randomiser QUANTUM HARMONISER


	for (int i = 0; i < munchieCount; i++)
	{
		if (state->IsKeyDown(Input::Keys::R) && !_munchie[i].rKeyDown)
		{
			_munchie[i].rKeyDown = true;
			_munchie[i].random = !_munchie[i].random;
			_munchie[i].Position->X = rand() % Graphics::GetViewportWidth();
			_munchie[i].Position->Y = rand() % Graphics::GetViewportHeight();
		}

		if (state->IsKeyUp(Input::Keys::R))
			_munchie[i].rKeyDown = false;
	}


}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	if (state->IsKeyDown(Input::Keys::P) && !_menu->_pKeyDown)
	{
		_menu->_pKeyDown = true;
		_menu->_paused = !_menu->_paused;
	}

	if (state->IsKeyUp(Input::Keys::P))
		_menu->_pKeyDown = false;
}
void Pacman::CheckRestart(Input::KeyboardState* state, Input::Keys restartKey)
{
	//Restart level
	if (state->IsKeyDown(Input::Keys::SPACE) && !_menu->_rKeyDown)
	{
		_menu->_rKeyDown=true;
		_menu->_gameOver=false;
		_pacman->Dead=false;
		Pacman::RestartGame(levelComplete);
	}
	if (state->IsKeyUp(Input::Keys::SPACE))
	{
		_menu->_rKeyDown=false;
	}

	//Restart Menu
	if (state->IsKeyDown(Input::Keys::ESCAPE) && !_menu->_rKeyDown)
	{
		_menu->_rKeyDown=true;
		_menu->_gameOver=false;
		_pacman->Dead=false;
		Pacman::RestartGame(levelComplete);
		gameState=0;
	}
	if (state->IsKeyUp(Input::Keys::ESCAPE))
	{
		_menu->_rKeyDown=false;
	}
}


void Pacman::CheckViewportCollision()
{
	// Checks if Pacman is trying to disappear

	// Pacman hit right wall - reset his position
	if (_pacman->Position->X > Graphics::GetViewportWidth() + _pacman->SourceRect->Width )
	{
		_pacman->Position->X = 0 ;//+ _pacman->SourceRect->Width;
	}
	// Pacman hit left wall - reset his position
	if (_pacman->Position->X + _pacman->SourceRect->Width < 0)
	{
		_pacman->Position->X = Graphics::GetViewportWidth() - _pacman->SourceRect->Width;
	}
	// Pacman hit top wall - reset his position
	if (_pacman->Position->Y  < 0 - _pacman->SourceRect->Height)
	{
		_pacman->Position->Y = Graphics::GetViewportHeight() - _pacman->SourceRect->Height;
	}
	// Pacman hit bottom wall - reset his position
	if (_pacman->Position->Y > Graphics::GetViewportHeight() + _pacman->SourceRect->Height )
	{
		_pacman->Position->Y = 0 ;//+ _pacman->SourceRect->Height;
	}
}

void Pacman::UpdatePacman(int elapsedTime)
{
	_pacman->CurrentFrameTime += elapsedTime;

	if (_pacman->CurrentFrameTime > _cPacmanFrameTime)
	{
		_pacman->Frame++;

		if (_pacman->Frame >= 2) 
			_pacman->Frame = 0;

		_pacman->CurrentFrameTime = 0;
	}
	_pacman->SourceRect->Y = _pacman->SourceRect->Height * _pacman->Direction;
	_pacman->SourceRect->X = _pacman->SourceRect->Width * _pacman->Frame;
	_pacman->ChaseRect->X = _pacman->Position->X-200;
	_pacman->ChaseRect->Y = _pacman->Position->Y-200;
}

void Pacman::UpdateMunchie(int elapsedTime)
{
	for (int i = 0; i < munchieCount; i++)
	{
		_munchie[i].CurrentFrameTime += elapsedTime;

		if (_munchie[i].CurrentFrameTime > _munchie[i].FrameTime)
		{
			_munchie[i].Frame++;

			if (_munchie[i].Frame >= 2) 
				_munchie[i].Frame = 0;

			_munchie[i].CurrentFrameTime = 0;
		}
		_munchie[i].SourceRect->X = _munchie[i].SourceRect->Width * _munchie[i].Frame;
	}
}

void Pacman::UpdateGhosts(int elapsedTime)
{
	for (int i = 0; i < ghostCount; i++)
	{
		if (_ghosts[i].bounce==true)
		{
			if (_ghosts[i].Direction == 3) //Moves Up
			{
				_ghosts[i].posRect->Y -= _ghosts[i].speed * elapsedTime;
			}
			else if (_ghosts[i].Direction == 1) //Moves Down
			{
				_ghosts[i].posRect->Y += _ghosts[i].speed * elapsedTime;
			}
			if (_ghosts[i].Direction == 2) //Moves Left
			{
				_ghosts[i].posRect->X -= _ghosts[i].speed * elapsedTime;
			}
			else if (_ghosts[i].Direction == 0) //Moves Right
			{
				_ghosts[i].posRect->X += _ghosts[i].speed * elapsedTime;
			}
		}
		else if (_ghosts[i].patrol==true)
		{
			//Why pointer?
			Vector2 currentDest = *patrolPoints[patrolDest[i]];

			if (currentDest.X > _ghosts[i].posRect->X)
				_ghosts[i].posRect->X += _ghosts[i].speed * elapsedTime;
			else
				_ghosts[i].posRect->X -= _ghosts[i].speed * elapsedTime;

			if (currentDest.Y > _ghosts[i].posRect->Y)
				_ghosts[i].posRect->Y += _ghosts[i].speed * elapsedTime;
			else
				_ghosts[i].posRect->Y -= _ghosts[i].speed * elapsedTime;

			if ((_ghosts[i].posRect->X >= currentDest.X - 5) && (_ghosts[i].posRect->X <= currentDest.X + 5) &&
				(_ghosts[i].posRect->Y >= currentDest.Y - 5) && (_ghosts[i].posRect->Y <= currentDest.Y + 5))
			{
				int patrolRandom = (int)rand() % POINTSCOUNT;

				patrolDest[i] = patrolRandom;
			}
		}

		if (_ghosts[i].chase==true)// && _ghosts[i].bounce==false)
		{
			if (_ghosts[i].posRect->Intersects(*_pacman->ChaseRect))
			{
				_ghosts[i].bounce = false;

				/*Vector2 ghostpacman(_ghosts[i].posRect->X,_pacman->Position->X);
				Vector2 direction;

				Vector2::Normalize(ghostpacman,direction);
				_ghosts[i].posRect->X = _ghosts[i].posRect->X + direction.X;

				Vector2 ghostpacman2(_ghosts[i].posRect->Y,_pacman->Position->Y);

				Vector2::Normalize(ghostpacman,direction);
				_ghosts[i].posRect->Y = _ghosts[i].posRect->Y - direction.Y;*/

				if((int)_ghosts[i].posRect->X != (int)_pacman->Position->X)
				{
					if(_ghosts[i].posRect->X > _pacman->Position->X)
					{

						_ghosts[i].posRect->X -= (_cPacmanSpeed * elapsedTime)/2;
					}
					else
					{
						_ghosts[i].posRect->X += (_cPacmanSpeed * elapsedTime)/2;
					}
				}
				if((int)_ghosts[i].posRect->Y != (int)_pacman->Position->Y)
				{
					if(_ghosts[i].posRect->Y > _pacman->Position->Y)
					{
						_ghosts[i].posRect->Y -= (_cPacmanSpeed * elapsedTime)/2;
					}
					else
					{
						_ghosts[i].posRect->Y += (_cPacmanSpeed * elapsedTime)/2;
					}
				}
			}
			else
				_ghosts[i].bounce = true;
		}


		// Viewport Collisions
		if (_ghosts[i].posRect->X + _ghosts[i].posRect->Width >= Graphics::GetViewportWidth()) //Hits Right edge
		{
			_ghosts[i].Direction = 2; //Change direction
		}
		else if (_ghosts[i].posRect->X <= 0) //Hits left edge
		{
			_ghosts[i].posRect->X = _ghosts[i].sourceRect->Width;
			_ghosts[i].Direction = 0; //Change direction
		}
		else if (_ghosts[i].posRect->Y <= 0) //Hits Top edge
		{
			_ghosts[i].posRect->Y = _ghosts[i].sourceRect->Height;
			_ghosts[i].Direction = 1; //Change direction
		}
		else if (_ghosts[i].posRect->Y+_ghosts[i].posRect->Height >=Graphics::GetViewportHeight()) //Hits Bottom edge
		{
			_ghosts[i].Direction = 3; //Change direction
		}
	}
}

void Pacman::UpdateGhostsFrame(int elapsedTime)
{
	// Ghost animation frame
	for (int i = 0; i < ghostCount; i++)
	{
		_ghosts[i].CurrentFrameTime += elapsedTime;

		if (_ghosts[i].CurrentFrameTime > _cPacmanFrameTime)
		{
			_ghosts[i].Frame++;

			if (_ghosts[i].Frame >= 2) 
				_ghosts[i].Frame = 0;

			_ghosts[i].CurrentFrameTime = 0;
		}
		_ghosts[i].sourceRect->Y = _ghosts[i].sourceRect->Height * _ghosts[i].Direction;
		_ghosts[i].sourceRect->X = _ghosts[i].sourceRect->Width * _ghosts[i].Frame;
	}
}

void Pacman::CheckGhostCollisions()
{
	for (int i = 0; i < ghostCount; i++)
	{
		if (_ghosts[i].posRect->Intersects(*_pacman->Position))

		{
			_pacman->Dead=true;
			_menu->_gameOver=true;
			Audio::Stop(_background);
			Audio::Play(_death);
			if (_score->score > _start->score)
			{
				_start->score = _score->score;
			}
		}
	}

}
void Pacman::CheckMunchieCollisions()
{
	for (int i = 0; i < munchieCount; i++)
	{


		if (!_munchie[i].eaten)
		{
			if (_munchie[i].Position->Intersects(*_pacman->Position))

			{
				if (gameState==1)
				{
					_munchie[i].eaten=true;
				}
				if (gameState==2||gameState==3)
				{
					do
					{
						_munchie[i].Position = new Rect(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight(), 12, 12);
					}while (_munchie[i].Position->Intersects(*_pacman->Position));
				}
				Audio::Play(_pop);
				_score->score += 10;
			}

		}

	}
}
void Pacman::UpdatePowerUpFrame(int elapsedTime)
{
	_powerUp->CurrentFrameTime += elapsedTime;

	if (_powerUp->CurrentFrameTime > _cPacmanFrameTime)
	{
		_powerUp->Frame++;

		if (_powerUp->Frame >= 2) 
			_powerUp->Frame = 0;

		_powerUp->CurrentFrameTime = 0;
	}

	_powerUp->sourceRect->X = _powerUp->sourceRect->Width * _powerUp->Frame;
}
void Pacman::CheckPowerUp(int elapsedTime)
{
	// Random number to spawn PowerUp
	int test = 0;

	while(_powerUp->spawned==false && _powerUp->active==false)
	{
		if ((test=((int)rand()%100))==99)
		{
			_powerUp->spawned=true;

			_powerUp->posRect = new Rect(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight(), 32, 32);
		}
	}

	// Collision
	if (_powerUp->posRect->Intersects(*_pacman->Position))
	{
		_powerUp->spawned=false;
		_powerUp->active=true;
		_pacman->Texture->Load("Textures/PacmanPowerUp.tga", false);
		Audio::Play(_pop);
		_score->score += 100;
		_powerUp->posRect = new Rect(-50,-50,32,32);
	}

	// countdown
	if (_powerUp->active==true)
		_powerUp->duration -= 1;

	// deactivate and reset duration
	if (_powerUp->duration<0)
	{
		_powerUp->active=false;
		_pacman->Texture->Load("Textures/Fighter.tga", false);
		_powerUp->duration=_cPowerUpDuration;
	}
}
void Pacman::CheckLevelComplete(bool levelComplete)
{
	for (int i=0;i<munchieCount;i++)
	{
		if (_munchie[i].eaten==true)
		{
			levelComplete=true;
		}
		else
		{
			levelComplete=false;
			break;
		}
	}

	if (levelComplete == true)
	{
		RestartGame(levelComplete);
	}


}
void Pacman::RestartGame(bool levelComplete)
{
	if (levelComplete==true)
	{
		levelComplete=false;

		munchieCount += 5;
		ghostCount +=1;
	}
	else
	{
		if (gameState==1)
		{
			munchieCount = 5;
			ghostCount = 5;
		}
		//Score reset
		_score->score=0;
		// Play audio
		Audio::Play(_background);
	}

	//Player
	_pacman->Position = new Rect(350.0f, 350.0f, 32, 32);
	_pacman->SourceRect = new Rect(0.0f, 0.0f, 32, 32);
	_pacman->ChaseRect = new Rect(_pacman->Position->X-200, _pacman->Position->Y-200, 432, 432);
	_pacman->SpeedMultiplier = 1.0f;
	_pacman->CurrentFrameTime = 0;
	_pacman->Direction = 0;
	_pacman->Frame = 0;
	_pacman->Dead = false;
	_pacman->Texture->Load("Textures/Fighter.tga", false);

	delete [] _munchie;
	_munchie = new Munchie[munchieCount];
	//munchie
	for (int i = 0; i < munchieCount; i++)
	{
		//_munchie[i] = new Munchie;
		_munchie[i].CurrentFrameTime = 0;
		_munchie[i].Frame = rand() % 1;
		_munchie[i].FrameTime = rand() % 500 + 50;
		_munchie[i].random = false;
		_munchie[i].rKeyDown = false;
		_munchie[i].eaten = false;
	}

	//Munchie positions
	for (int i = 0; i < munchieCount; i++)
	{
		_munchie[i].SourceRect = new Rect(0, 0, 12, 12);
		do
		{
			_munchie[i].Position = new Rect(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight(), 12, 12);
		}
		while (_munchie[i].Position->Intersects(*_pacman->Position));
		_munchie[i].eaten = false;

	}

	// Load Ghosts
	delete [] _ghosts;
	_ghosts = new MovingEnemy[ghostCount];



	for (int i = 0; i < ghostCount; i++)
	{
		_ghosts[i].texture = new Texture2D();
		do
		{
			_ghosts[i].posRect = new Rect(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight(), 20, 20);
		}
		while (_ghosts[i].posRect->Intersects(*_pacman->ChaseRect));
		_ghosts[i].sourceRect = new Rect(0.0f, 0.0f, 20, 20); 
		_ghosts[i].CurrentFrameTime = 0;
		_ghosts[i].Frame = 0;
		_ghosts[i].Direction = 0;
		_ghosts[i].speed = 0.2f;
		_ghosts[i].texture->Load("Textures/GhostBlue.png", true);

		if (i>=7 && i<12) //3 Chase Ghosts
		{
			_ghosts[i].chase = true;
			_ghosts[i].texture->Load("Textures/GhostRed.png", true);
		}
		else if (i>=13) //5 Patrol Ghosts
		{
			_ghosts[i].patrol = true;
			_ghosts[i].posRect = new Rect((rand()%20)*50.0f, 50.0f, 20, 20);
			patrolDest[i] = 0;
			_ghosts[i].texture->Load("Textures/GhostYel.png", true);
		}

		else //Bounce ghosts
		{
			_ghosts[i].bounce= true;
			_ghosts[i].Direction = ((int) rand()%4);
		}
	}

	//Load Powerup
	_powerUp->active=false;
	_powerUp->duration=_cPowerUpDuration;
	_powerUp->sourceRect = new Rect(0, 0, 32, 32);
	_powerUp->posRect = new Rect(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight(), 32, 32);
	_powerUp->powerUpStringPosition = new Vector2(Graphics::GetViewportWidth()/2,50.0f);


}
