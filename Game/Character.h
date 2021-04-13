#pragma once
#include "defines.h"

#include <chrono>

#include "Object.h"

#include "Model.h"
#include "Bullet.h"


class Character : public Object
{
public:
	Character(Shader* shader);

	virtual glm::vec3 getVecFront();

	virtual glm::vec3 getVecFrontOrigin();

	glm::vec3 getVecUp();

	void calculateFrontandUpVector();

	virtual void interactWithObject();

	void resetVerticalMovement();

	void moveForward();

	void moveBackward();

	void moveRight();

	void moveLeft();

	void moveUp();

	void moveDown();

	void rollLeft();

	void rollRight();

	void jump();

	void activateJumping();

	void crouch(bool crouch = true);

	void run(bool run = true);

	std::shared_ptr<Object> getObjectLookingAt()
	{
		return objectLookingAt;
	}

	std::shared_ptr<Object> calculateObjectLookingAt();

	glm::vec3 getRotationInv()
	{
		return rotation + glm::vec3(0, 180, 0);
	}

	virtual std::shared_ptr<Bullet> shoot();

	int getTeam() {
		return team;
	}

	void setTeam(int newTeam){
		team = newTeam;
	}


public:
	float cha_roll = 0, cha_yaw = 0, cha_pitch = 0;

protected:

	bool isCrouched = false;
	bool isRunning = false;
	bool canJump = true;

	glm::vec3 vecFront;
	glm::vec3 vecUp;
	glm::vec3 vecRight;
	

	float32 maxSpeed = 60;
	float32 forwardAccel = 0.1f;				//per second
	float32 backwardSidewayAccel = 0.04f;		//per second
	float32 upwardAccel = 0.04f;				//per second
	float32 rollSpeed = 0.1f;

	float32 actualSpeed = 0;

	const float32 heigth = 4;

	std::chrono::system_clock::time_point lastTimeShot = std::chrono::system_clock::now() - std::chrono::hours(1);

	

	int team = 0;

	std::shared_ptr<Object> objectLookingAt;
};

