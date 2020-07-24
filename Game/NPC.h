#pragma once
#include "defines.h"

#include <string>
#include <vector>

#include "Character.h"

enum CurrentTask {
	Idle = 0,
	Follow_Character = 1,
	Follow_NavPoint = 2
};

class NPC : public Character
{

public:

	NPC(Shader* shader);

	void followCharacter(std::shared_ptr<Character> character);

	void followNavPoints();

	void addNavPoint(glm::vec3 newNavPoint);

	void setNavPoints(std::vector<glm::vec3> newNavPoints);

	std::vector<glm::vec3> getNavPoints();

	void doCurrentTask();

	void setCurrentTask(CurrentTask newCurrentTask);

	CurrentTask getGurrentTask();

	void evade(CollisionResult collisionResult);

	void reactToCollision(CollisionResult collisionResult);

private:

	std::vector<glm::vec3> navPoints;
	int currentNavPoint = 0;
	CurrentTask currentTask = CurrentTask::Idle;
};

