#include "GameMode_SinglePlayer.h"

#include "Logger.h"
#include "Game.h"

void GameMode_SinglePlayer::init()
{
}

void GameMode_SinglePlayer::gameLoop()
{

	Game::lbl_ObjectCount->setText("Objects: " + std::to_string(Game::objects.size()));

	stopwatch1.start();
#ifdef DEBUG_LOOP
	Logger::log("Debug Input Step");
#endif
	Game::processInput();
	double stopwatch1duration = stopwatch1.stop();
	Game::lbl_stopwatch1->setText("Input: " + std::to_string(stopwatch1duration));

	if (Game::gameState == GameState::GAME_ACTIVE || Game::gameState == GameState::GAME_GAME_OVER)
	{
		GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

#ifdef DEBUG_LOOP
		Logger::log("Debug BeforeFrame Step");
#endif
		for (std::shared_ptr<Object> object : Game::objects)
		{
			if (!object->getEnabled()) continue;
			object->calculationBeforeFrame();
		}

#ifdef DEBUG_LOOP
		Logger::log("Debug NPC Step");
#endif
		stopwatch1.start();

			for (std::shared_ptr<NPC> npc : Game::npcs)
			{
				if (!npc->getEnabled()) continue;
				npc->doCurrentTask();
			}
		
		double stopwatch2duration = stopwatch1.stop();
		Game::lbl_stopwatch2->setText("NPCs: " + std::to_string(stopwatch2duration));

#ifdef DEBUG_LOOP
		Logger::log("Debug Movement Step");
#endif
		//Move every Object
		for (std::shared_ptr<Object> object : Game::objects)
		{
			if (NetworkManager::getIsConnected() && !(object->getType() & ObjectType::Object_Player)) continue;
			if (!object->getEnabled()) continue;
			//if (object->getType() & ObjectType::Object_Environment) continue; //Environment doesnt move

			object->fall();
			object->move();

		}

#ifdef DEBUG_LOOP
		Logger::log("Debug Bullet Hit Detection Step");
#endif
		stopwatch1.start();

			for (std::shared_ptr<Bullet> bullet : Game::bullets)
			{
				if (!bullet->getEnabled()) continue;
				bullet->checkHit();
			}

#ifdef DEBUG_LOOP
		Logger::log("Debug Collision Step");
#endif
		//Check every Object for collision
		Game::processCollision();
		double stopwatch3duration = stopwatch1.stop();
		Game::lbl_stopwatch3->setText("Col: " + std::to_string(stopwatch3duration));

		for (std::shared_ptr<Player> player : Game::players)
		{
			if (!player->getEnabled()) continue;
			player->updateCameraPosition();
			//player->update();
			if (player->getPosition().x > 95 && player->getPosition().z > 95)
			{
				UI_Element* eastereggLabel = new UI_Element_Label(Game::getWindowWidth() / 2 - 200, Game::getWindowHeight() / 2 - 100, 10, 10, "Nice, du hast das Easter-Egg gefunden", 1000, 1, glm::vec4(1, 0, 0, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), false);
				UI::addElement(eastereggLabel);
			}
		}

		//Reset Vertical Movement

			for (std::shared_ptr<Object> object : Game::objects)
			{
				if (!object->getEnabled()) continue;
				object->resetMovement();
			}


		//Deactivate Objects if Health <= 0

			Game::deleteObjects();

		//UI Output
		if (Game::npcs.size() <= 0 && Game::gameState == GameState::GAME_ACTIVE) //todo npcs get disabled
		{
			UI_Element* victoryLabel = new UI_Element_Label(Game::getWindowWidth() / 2 - 100, Game::getWindowHeight() / 2, 10, 10, "Du hast alle Bots besiegt", 1000, 1, glm::vec4(0, 0, 1, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), false);
			UI::addElement(victoryLabel);
			Game::gameState = GameState::GAME_GAME_OVER;
		}

		stopwatch1.start();
		//testing - Raypicking
#ifdef DEBUG_LOOP
		Logger::log("Debug Raypicking Step");
#endif
		Game::players[0]->calculateObjectLookingAt();
		std::shared_ptr<Object> objectPlayerLookingAt = Game::players[0]->getObjectLookingAt();
		if (objectPlayerLookingAt != nullptr)
		{
			if (Game::players[0]->getDistance(objectPlayerLookingAt) < 10)
			{
				if (objectPlayerLookingAt->getType() & ObjectType::Object_Interactable)
				{
					objectPlayerLookingAt->markObject();
				}
			}
		}
		double stopwatch5duration = stopwatch1.stop();
		Game::lbl_stopwatch5->setText("Raypicking: " + std::to_string(stopwatch5duration));

	}

	stopwatch1.start();
#ifdef DEBUG_LOOP
	Logger::log("Debug Render Step");
#endif
	if (Game::gameState != GameState::GAME_PAUSED)
	{
		Game::render();
	}
	double stopwatch4duration = stopwatch1.stop();
	Game::lbl_stopwatch4->setText("Render: " + std::to_string(stopwatch4duration));


#ifdef DEBUG_LOOP
	Logger::log("Debug AfterFrame Step");
#endif

		if (Game::gameState == GameState::GAME_ACTIVE || Game::gameState == GameState::GAME_GAME_OVER)
		{
			for (std::shared_ptr<Object> object : Game::objects)
			{
				if (!object->getEnabled()) continue;
				object->calculationAfterFrame();
			}
		}

}
