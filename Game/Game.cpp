#include "Game.h"



#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION


SDL_Window* Game::window;

std::vector<std::shared_ptr<Object>> Game::map;
std::vector<std::shared_ptr<Object>> Game::objects;
std::vector< std::shared_ptr<Character> > Game::characters;
std::vector< std::shared_ptr<Player> > Game::players;
std::vector< std::shared_ptr<NPC> > Game::npcs;
std::vector< std::shared_ptr<Bullet> > Game::bullets;


bool Game::pressedKeys[20];
bool Game::pressedMouseButtons[6];

float32  Game::FPS = 0;
float32  Game::fps_limit = 0;
bool  Game::close = false;

uint64 Game::perfCounterFrequency;
uint64 Game::lastCounter;
float32 Game::delta;

bool Game::showInfo = false;
GameState Game::gameState;
int Game::maxBulletCount = 20;

bool Game::showShadowMap = false;
bool Game::postprocess = true;

Menu* Game::menu_Main = nullptr;
Menu* Game::menu_Pause = nullptr;
Menu* Game::menu_Options = nullptr;

Menu* Game::menu_Current = nullptr;

irrklang::ISoundEngine* Game::SoundEngine = irrklang::createIrrKlangDevice();

/// <summary>
/// this methods starts the game
/// </summary>
void Game::startGame()
{
	perfCounterFrequency = SDL_GetPerformanceFrequency();
	lastCounter = SDL_GetPerformanceCounter();
	delta = 0.0f;
	gameState = GameState::GAME_ACTIVE;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	float musicvolume = std::stof(ConfigManager::readConfig("musicvolume"));
	SoundEngine->setSoundVolume(musicvolume / 100);

	irrklang::ISound* music = SoundEngine->play2D("audio/breakout.mp3", true);



	gameLoop();
}


/// <summary>
/// this function inits everything before the game can be started
/// </summary>
void Game::init()
{
	Renderer::initOpenGL();
	Renderer::loadShader();

	UI::init();

	Renderer::drawLoadingScreen();


	maxBulletCount = stoi(ConfigManager::readConfig("max_bullets"));
	fps_limit = stof(ConfigManager::readConfig("fps_limit"));
	std::string levelname = ConfigManager::readConfig("level");
	Map::load(levelname);

	menu_Main = new Menu_Main();
	menu_Pause = new Menu_Pause();
	menu_Options = new Menu_Options();

	Renderer::init();
}

/// <summary>
/// main game loop
/// </summary>
void Game::gameLoop()
{
	std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point b = std::chrono::system_clock::now();

	UI_Element_Graph* fpsGraph = new UI_Element_Graph(10, getWindowHeight() * 3 / 4, 100, 100, 0, glm::vec4(0, 0, 1, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), true);
	UI::addElement(fpsGraph);


	while (!close)
	{

		a = std::chrono::system_clock::now();

		#pragma region gameloop

		processInput();

		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

			for (std::shared_ptr<Object> object : objects)
			{
				if (!object->getEnabled()) continue;
				object->calculationBeforeFrame();
			}



			for (std::shared_ptr<NPC> npc : npcs)
			{
				if (!npc->getEnabled()) continue;
				npc->doCurrentTask();
			}

			//Move every Object
			for (std::shared_ptr<Object> object : objects)
			{
				if (!object->getEnabled()) continue;
				if (object->getType() == ObjectType::Object_Environment) continue; //Environment doesnt move

				object->fall();
				object->move();

			}

			for (std::shared_ptr<Bullet> bullet : bullets)
			{
				if (!bullet->getEnabled()) continue;
				bullet->checkHit();
			}

			//Check every Object for collision
			processCollision();

			for (std::shared_ptr<Player> player : players)
			{
				if (!player->getEnabled()) continue;
				player->updateCameraPosition();
				//player->update();
				if (player->getPosition().x > 95 && player->getPosition().z > 95)
				{
					UI_Element* eastereggLabel = new UI_Element_Label(getWindowWidth() / 2 - 200, getWindowHeight() / 2 - 100, 10, 10, "Nice, du hast das Easter-Egg gefunden", 1000, 1, glm::vec4(1, 0, 0, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), false);
					UI::addElement(eastereggLabel);
				}
			}

			for (std::shared_ptr<Character> character : characters)
			{
				if (!character->getEnabled()) continue;
				character->resetVerticalMovement();
			}

			deleteObjects();

			if (npcs.size() <= 0 && gameState == GameState::GAME_ACTIVE)
			{
				UI_Element* victoryLabel = new UI_Element_Label(getWindowWidth() / 2 - 100, getWindowHeight() / 2, 10, 10, "Du hast alle Bots besiegt", 1000, 1, glm::vec4(0, 0, 1, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), false);
				UI::addElement(victoryLabel);
				gameState = GameState::GAME_GAME_OVER;
			}

			

			

		}
		
		if (gameState != GameState::GAME_PAUSED)
		{
			render();
		}

		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			for (std::shared_ptr<Object> object : objects)
			{
				if (!object->getEnabled()) continue;
				object->calculationAfterFrame();
			}
		}

		#pragma endregion

		b = std::chrono::system_clock::now();

		std::chrono::duration<double, std::milli> work_time = b - a;

		if (work_time.count() < 1000/fps_limit && fps_limit!=0)
		{
			std::chrono::duration<double, std::milli> delta_ms(1000 / fps_limit - work_time.count());
			auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
			std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		}

		b = std::chrono::system_clock::now();
		std::chrono::duration<double, std::milli> loop_time = b - a;

		delta = loop_time.count();
		FPS = 1 / (delta/1000);

		fpsGraph->setValue(FPS);

	}
}

/// <summary>
/// process the Collision for every object
/// </summary>
void Game::processCollision()
{
	for (std::shared_ptr<Object> object : objects)
	{
		CollisionResult collisionResult = object->checkCollision();
		if (!collisionResult.collided) continue;
		if (!object->getEnabled()) continue;

		for (CollidedObject* collidedObject : collisionResult.collidedObjectList)
		{
			if (object->getType() & ObjectType::Object_Character && collidedObject->onTop == true)
			{
				Character* character = static_cast<Character*>(object.get());
				character->activateJumping();
			}

			if ((object->getType() & ObjectType::Object_NPC) && (collidedObject->object->getType() & (ObjectType::Object_Entity | ObjectType::Object_Character)))
			{
				NPC* npc = static_cast<NPC*>(object.get());
				npc->evade(collisionResult);
			}
		}


		object->reactToCollision(collisionResult);

	}
}

/// <summary>
/// in this methods the game decides what needs to be rendered
/// </summary>
void Game::render()
{
	switch (gameState)
	{
	case GameState::GAME_ACTIVE:
	case GameState::GAME_GAME_OVER:
		{
			Renderer::calcLight();

			if (ConfigManager::shadowOption != ShadowOption::off)
				Renderer::calcShadows();


			if (postprocess) Renderer::frameBuffer.bind();
			Renderer::clearBuffer();
			Renderer::renderOpaqueObjects();

			Renderer::renderSkybox(glm::mat4(glm::mat3(players[0]->getView())), players[0]->getProj());
			Renderer::renderTransparentObjects();
			if (postprocess) Renderer::frameBuffer.unbind();

			//Postprocessing
			if (postprocess) Renderer::postProcessing();

			//show the Shadow map
			if (showShadowMap) Renderer::showShadowMap();


			if (showInfo)
		{
			Renderer::renderAxis(players[0]->getLookDirection(), 8, 5);
			Renderer::renderAxis(Renderer::transformedSunDirection3, 8, 3);

			UI::updateFPS();
			UI::updatePos(players[0]);
			UI::updateRot(players[0]);
		}

			UI::drawUI();
		}
		break;
	case GameState::GAME_MENU:
		{
			if (menu_Current != nullptr)
			{
				GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
				GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

				if (postprocess) Renderer::postProcessing();
				GLCALL(glDisable(GL_DEPTH_TEST));
				GLCALL(glDisable(GL_CULL_FACE));
				menu_Current->drawMenu();
				GLCALL(glEnable(GL_DEPTH_TEST));
				GLCALL(glEnable(GL_CULL_FACE));

				//swapBuffer();
			}
		}
		break;
	}

	Game::swapBuffer();
}

/// <summary>
/// process the Input from Mouse and Keyboard
/// </summary>
void Game::processInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			close = true;
		}
		else if (event.type == SDL_KEYDOWN)
		{
			keyPressed(event.key.keysym.sym);
		}
		else if (event.type == SDL_KEYUP)
		{
			keyReleased(event.key.keysym.sym);
		}
		else if (event.type == SDL_MOUSEMOTION) {
			if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
			{
				if (SDL_GetRelativeMouseMode()) {
					players[0]->onMouseMove(event.motion.xrel, event.motion.yrel);
					updateAudioListener();
				}
			}
			else if (gameState == GameState::GAME_MENU)
			{
				menu_Current->onMouseMove(event.motion.x, event.motion.y);
			}
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
			{
				if (!SDL_GetRelativeMouseMode())
				{
					SDL_SetRelativeMouseMode(SDL_TRUE);
					continue;
				}
			}
			else if (gameState == GameState::GAME_MENU)
			{
				menu_Current->onMouseClick(event.motion.x, event.motion.y, event.button);
			}
			pressedMouseButtons[event.button.button] = true;

		}
		else if (event.type == SDL_MOUSEBUTTONUP) {
			pressedMouseButtons[event.button.button] = false;
		}
	}

	if (pressedMouseButtons[SDL_BUTTON_LEFT])
	{
		if (gameState == GameState::GAME_ACTIVE)
		{
			if (players[0]->shoot())	SoundEngine->play2D("audio/shoot.wav", false);
		}
		else if (gameState == GameState::GAME_MENU)
		{
			menu_Current->onMouseDown(event.motion.x, event.motion.y, event.button);
		}

	}

	if (pressedKeys[PlayerAction::moveForward])
	{
		players[0]->moveForward();
		updateAudioListener();
	}
	if (pressedKeys[PlayerAction::moveBackward])
	{
		players[0]->moveBackward();
		updateAudioListener();
	}
	if (pressedKeys[PlayerAction::moveLeft])
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			players[0]->moveLeft();
			updateAudioListener();
		}
		else if (gameState == GameState::GAME_MENU)
		{
			menu_Current->leftOnSelectedMenuElement();
		}
	}
	if (pressedKeys[PlayerAction::moveRight])
	{
		if (gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
		{
			players[0]->moveRight();
			updateAudioListener();
		}
		else if (gameState == GameState::GAME_MENU)
		{
			menu_Current->rightOnSelectedMenuElement();
		}
	}
	if (pressedKeys[PlayerAction::jump])
	{
		players[0]->jump();
	}
	if (pressedKeys[PlayerAction::sprint])
	{
		players[0]->run(true);
	}
	if (!pressedKeys[PlayerAction::sprint])
	{
		players[0]->run(false);
	}
	if (pressedKeys[PlayerAction::crouch])
	{
		players[0]->crouch(true);
	}
	if (!pressedKeys[PlayerAction::crouch])
	{
		players[0]->crouch(false);
	}
}

/// <summary>
/// function for when a key is pressed
/// </summary>
/// <param name="key">the pressed key</param>
void Game::keyPressed(SDL_Keycode key)
{
	PlayerAction action = PlayerAction::None;

	auto it = keybindings.find(key);
	if (it != keybindings.end()) {
		action = it->second;
	}
	if (action == PlayerAction::None) { Logger::log("Keybinding not found!"); return; }

	//Single Action Keys, just one time per pressing
	if (!pressedKeys[action])
	{
		switch (gameState)
		{
			case GameState::GAME_ACTIVE:
			case GameState::GAME_GAME_OVER:
			case GameState::GAME_PAUSED:
			{
				switch (action)
				{
					case PlayerAction::interact:
						// todo
						break;
					case PlayerAction::toggleFlashlight:
						players[0]->toggleFlashlight();
						break;
				}
				break;
			}
			case GameState::GAME_MENU:
			{
				switch (action)
				{
					case PlayerAction::moveForward:
						menu_Current->selectPreviousElement();
						SoundEngine->play2D("audio/select.wav", false);
						break;
					case PlayerAction::moveBackward:
						menu_Current->selectNextElement();
						SoundEngine->play2D("audio/select.wav", false);
						break;
					case PlayerAction::enter:
						menu_Current->enterSelectedMenuElement();
						SoundEngine->play2D("audio/select.wav", false);
						break;
					case PlayerAction::jump:
						menu_Current->enterSelectedMenuElement();
						SoundEngine->play2D("audio/select.wav", false);
						break;
				}
				break;
			}
		}

		//Keybind which should work in every gamemode
		switch (action)
		{
		case PlayerAction::toggleInfo:
			showInfo = !showInfo;
			break;
		case PlayerAction::toggleWireframe:
			Renderer::toggleWireframe();
			break;
		case PlayerAction::toggleShowNormals:
			Renderer::toggleShowNormals();
			break;
		case toggleShowShadowMap:
			showShadowMap = !showShadowMap;
			break;
		case togglePostprocess:
			postprocess = !postprocess;
			break;
		case PlayerAction::pause:
			togglePause();
			break;
		case PlayerAction::menu:
			if (toggleMenu()) break;
			if (toggleMenuOptions()) break;
			break;
		case PlayerAction::toggleFullscreen:
			toggleFullscreen();
			break;
		case PlayerAction::toggleConsole:
			openConsole();
			break;
		}

	}

	pressedKeys[action] = true;
}

/// <summary>
/// function for when a key is released
/// </summary>
/// <param name="key">the released key</param>
void Game::keyReleased(SDL_Keycode key)
{
	PlayerAction action = PlayerAction::None;

	auto it = keybindings.find(key);
	if (it != keybindings.end()) {
		action = it->second;
	}
	if (action == PlayerAction::None) { Logger::log("Keybinding not found!"); return; }


	pressedKeys[action] = false;
}

/// <summary>
/// deletes or disable object, that are destroyed
/// </summary>
void Game::deleteObjects()
{
	for (int i = objects.size() - 1; i >= 0; i--)
	{
		if (objects[i]->getHealth() <= 0)
		{
			objects[i]->disable();
			
			//objects.erase(objects.begin() + i);
			//i++;
			/*std::shared_ptr<Object> objectToDelete = objects[i];

			int index = objectToDelete->getNumber();

			if (objectToDelete->getType() & ObjectType::Object_NPC)
			{
				auto it = std::find(npcs.begin(), npcs.end(), objectToDelete);
				if (it != npcs.end()) { npcs.erase(it); }
			}

			if (objectToDelete->getType() & ObjectType::Object_Character)
			{
				auto it2 = std::find(characters.begin(), characters.end(), objectToDelete);
				if (it2 != characters.end()) { characters.erase(it2); }
			}

			if (objectToDelete->getType() & ObjectType::Object_Bullet)
			{
				auto it3 = std::find(bullets.begin(), bullets.end(), objectToDelete);
				if (it3 != bullets.end()) { bullets.erase(it3); }
			}

			auto it1 = std::find(objects.begin(), objects.end(), objectToDelete);
			if (it1 != objects.end()) { objects.erase(it1); }
			*/
		}
	}

	if (bullets.size() > maxBulletCount)
	{
		std::shared_ptr<Object> objectToDelete = bullets[0];

		auto it3 = std::find(bullets.begin(), bullets.end(), objectToDelete);
		if (it3 != bullets.end()) { bullets.erase(it3); }

		auto it1 = std::find(objects.begin(), objects.end(), objectToDelete);
		if (it1 != objects.end()) { objects.erase(it1); }
	}
}

void Game::togglePause()
{
	if (gameState == GameState::GAME_PAUSED)
	{
		gameState = GameState::GAME_ACTIVE;
	}
	else if (gameState == GameState::GAME_ACTIVE)
	{
		gameState = GameState::GAME_PAUSED;
		UI::drawPause();
		Game::swapBuffer();
	}
}

bool Game::toggleMenu()
{
	if (gameState == GameState::GAME_PAUSED || gameState == GameState::GAME_ACTIVE || gameState == GameState::GAME_GAME_OVER)
	{
		gameState = GameState::GAME_MENU;
		menu_Current = menu_Pause;
		SDL_SetRelativeMouseMode(SDL_FALSE);
		return true;
	}
	else if (gameState == GameState::GAME_MENU)
	{
		if(players[0]->getEnabled())
			gameState = GameState::GAME_ACTIVE;
		else
			gameState = GameState::GAME_GAME_OVER;
		SDL_SetRelativeMouseMode(SDL_TRUE);
		return true;
	}
	return false;
}

bool Game::toggleMenuOptions()
{
	if (gameState == GameState::GAME_MENU && menu_Current == menu_Pause)
	{
		gameState = GameState::GAME_MENU;
		menu_Current = menu_Options;
		return true;
	}
	else if (gameState == GameState::GAME_MENU && menu_Current == menu_Options)
	{
		gameState = GameState::GAME_MENU;
		menu_Current = menu_Pause;
		return true;
	}
	return false;
}

void Game::toggleFullscreen()
{
	Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN;
	bool IsFullscreen = SDL_GetWindowFlags(window) & FullscreenFlag;
	SDL_SetWindowFullscreen(window, IsFullscreen ? 0 : FullscreenFlag);

	if (IsFullscreen) //now windowed
	{
		int resolutionW = std::stoi(ConfigManager::readConfig("windowed_resolution_width"));
		int resolutionH = std::stoi(ConfigManager::readConfig("windowed_resolution_height"));

		Game::changeSize(resolutionW, resolutionH);
		Renderer::changeResolution(resolutionW, resolutionH);
	}
	else {
		int resolutionW = std::stoi(ConfigManager::readConfig("fullscreen_resolution_width"));
		int resolutionH = std::stoi(ConfigManager::readConfig("fullscreen_resolution_height"));

		Game::changeSize(resolutionW, resolutionH);
		Renderer::changeResolution(resolutionW, resolutionH);
	}
}

void Game::updateAudioListener()
{
	irrklang::vec3df Lposition = irrklang::vec3df(players[0]->getPosition().x, players[0]->getPosition().y, players[0]->getPosition().z);
	irrklang::vec3df LlookAt = irrklang::vec3df(-players[0]->getLookDirection().x, players[0]->getLookDirection().y, -players[0]->getLookDirection().z);
	Game::SoundEngine->setListenerPosition(Lposition, LlookAt);
}

int Game::getWindowWidth()
{
	int width, height;
	SDL_GetWindowSize(window, &width, &height);

	return width;
}

int Game::getWindowHeight()
{
	int width, height;
	SDL_GetWindowSize(window, &width, &height);

	return height;
}

void Game::openConsole()
{
	std::string enteredText = "";

	std::cin >> enteredText;

	if (enteredText[0] == 'x')
	{
		players[0]->setPosition(glm::vec3(std::stof(enteredText.substr(1)), players[0]->getPosition().y, players[0]->getPosition().z));
	}
	if (enteredText[0] == 'y')
	{
		players[0]->setPosition(glm::vec3(players[0]->getPosition().x, std::stof(enteredText.substr(1)), players[0]->getPosition().z));
	}
	if (enteredText[0] == 'z')
	{
		players[0]->setPosition(glm::vec3(players[0]->getPosition().x, players[0]->getPosition().y, std::stof(enteredText.substr(1))));
	}
}

void Game::changeSize(int w, int h)
{
	SDL_SetWindowSize(window, w, h);
	Renderer::initFrameBuffer();
}