#include "Character.h"

#include "Game.h"
#include "Raypicker.h"

#include <cmath>

Character::Character(Shader* shader) : Object(shader, "character.bmf")
{
	this->shader = shader;
	vecFront = glm::vec3(1.0f, 0.0f, 0.0f);
	vecUp = glm::vec3(0.0f, 1.0f, 0.0f);
	vecRight = glm::vec3(0.0f, 0.0f, 1.0f);
	this->name = "Character";
	this->setType(ObjectType::Object_Character);
	this->setCollisionBoxType(CollisionBoxType::cube);
}

glm::vec3 Character::getVecFront()
{
	return vecFront;
}

glm::vec3 Character::getVecFrontOrigin()
{
	glm::vec3 lookOrigin = this->getPosition();
	lookOrigin.y += this->getDimensions().y * 0.75;
	return lookOrigin;
}

glm::vec3 Character::getVecUp()
{
	return vecUp;
}

void Character::calculateFrontandUpVector()
{
	glm::vec3 vecFrontLocal = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 vecUpLocal = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 vecRightLocal = glm::vec3(1.0f, 0.0f, 0.0f);
	vecFront = glm::normalize(glm::rotate(rotationQuat, vecFrontLocal));
	vecUp = glm::normalize(glm::rotate(rotationQuat, vecUpLocal));
	vecRight = glm::normalize(glm::rotate(rotationQuat, vecRightLocal));
}

void Character::interactWithObject()
{
	std::shared_ptr<Object> objectPlayerLookingAt = this->getObjectLookingAt();
	if (objectPlayerLookingAt != nullptr)
	{
		if (this->getDistance(objectPlayerLookingAt) < 10)
		{
			objectPlayerLookingAt->interact_click();
		}
	}
}

void Character::resetVerticalMovement()
{
	////movement = movement * glm::vec3(0, 0, 0);
	float brakevalue = 0.02f;
	//
	Logger::logVector(movement,"Move",2,"\033[34m");

	//if (movement.x > brakevalue) movement.x -= brakevalue;
	//if (movement.x < -brakevalue) movement.x += brakevalue;
	//if (movement.x > -brakevalue && movement.x < brakevalue) movement.x = 0;

	//if (movement.y > brakevalue) movement.y -= brakevalue;
	//if (movement.y < -brakevalue) movement.y += brakevalue;
	//if (movement.y > -brakevalue && movement.y < brakevalue) movement.y = 0;

	//if (movement.z > brakevalue) movement.z -= brakevalue;
	//if (movement.z < -brakevalue) movement.z += brakevalue;
	//if (movement.z > -brakevalue && movement.z < brakevalue) movement.z = 0;

	/*if (!Game::isKeyPressed(PlayerAction::moveForward)) {
		glm::vec3 moveFront = movement * vecFront;
		if (glm::length(moveFront) > brakevalue) {
			if (glm::dot(movement, vecFront) > 0) {
				Logger::warn("moving forward");
				movement += glm::normalize(moveFront) * brakevalue;
			}
			else {
				Logger::warn("moving backward");
				movement -= glm::normalize(moveFront) * brakevalue;
			}
		}
	}*/

	glm::vec3 moveVec = glm::normalize(movement);
	actualSpeed = glm::length(movement);

	if (actualSpeed > 0) {
		movement -= moveVec * brakevalue;
	}
}

void Character::moveForward() {
	glm::vec3 a = vecFront * forwardAccel;

	if (glm::length(movement) < maxSpeed)
	{
		movement += a;
	}
}

void Character::moveBackward() {
	glm::vec3 a = vecFront * -backwardSidewayAccel;

	if (glm::length(movement) < maxSpeed)
	{
		movement += a;
	}
}

void Character::moveRight() {
	
	glm::vec3 a = vecRight * backwardSidewayAccel;

	if (glm::length(movement) < maxSpeed)
	{
		movement += a;
	}
}

void Character::moveLeft() {

	glm::vec3 a = vecRight * -backwardSidewayAccel;

	if (glm::length(movement) < maxSpeed)
	{
		movement += a;
	}
}

void Character::moveUp()
{
	glm::vec3 a = vecUp * upwardAccel;

	if (glm::length(movement) < maxSpeed)
	{
		movement += a;
	}
}

void Character::moveDown()
{
	glm::vec3 a = vecUp * -upwardAccel;

	if (glm::length(movement) < maxSpeed)
	{
		movement += a;
	}
}

void Character::rollLeft()
{
	glm::quat qRoll = glm::angleAxis(glm::radians(+rollSpeed), glm::vec3(0, 0, 1));

	rotationQuat = rotationQuat * qRoll;
	setRotationQuat(rotationQuat);

	calculateFrontandUpVector();
	
}

void Character::rollRight()
{
	glm::quat qRoll = glm::angleAxis(glm::radians(-rollSpeed), glm::vec3(0, 0, 1));

	rotationQuat = rotationQuat * qRoll;
	setRotationQuat(rotationQuat);

	calculateFrontandUpVector();
	
}

void Character::jump()
{
	if (canJump)
	{
#ifdef DEBUG_MOVEMENT
		Logger::log("Character: " + printObject() + " jumped");
#endif

		AudioManager::play3D("audio/jump.wav", this->getPosition());

		movement.y += 30;
		//movement.y += 0.06 * Game::getDelta();

		canJump = false;
	}
}

void Character::activateJumping()
{
	if (!canJump)
	{
		canJump = true;
		Logger::info("Jumping activated for: " + printObject());
	}
}

void Character::crouch(bool crouch)
{
	if (crouch && !isCrouched)
	{
		isCrouched = true;
	}
	else if (!crouch && isCrouched)
	{
		isCrouched = false;
	}
}

void Character::run(bool run)
{
	if (run && !isRunning)
	{
		forwardAccel = forwardAccel * 2;
		isRunning = true;
	}
	else if (!run && isRunning)
	{
		forwardAccel = forwardAccel / 2;
		isRunning = false;
	}
}

std::shared_ptr<Object> Character::calculateObjectLookingAt()
{
	glm::vec3 rayOrigin = this->getVecFrontOrigin();
	glm::vec3 rayDirection = this->getVecFront();

	RayPickingResult raypickingresult = Raypicker::doRayPicking(Ray(rayOrigin, rayDirection), Game::objects);

#ifdef DEBUG_RAYPICKING
	Logger::log(this->printObject()+" is looking at "+ raypickingresult.intersectedObject_nearest.object->printObject());
#endif
	objectLookingAt = raypickingresult.intersectedObject_nearest.object;
	return raypickingresult.intersectedObject_nearest.object;
}

/// <summary>
/// creates a bullet at the position of the character, with a little offset.
///  it gives the bullet also the direction the character is looking at.
/// </summary>
/// <returns>a shared pointer to the bullet</returns>
std::shared_ptr<Bullet> Character::shoot()
{
	float maxScatteringAngle = 1;

	float scatteringAngleX = std::rand() * maxScatteringAngle / RAND_MAX;
	scatteringAngleX -= maxScatteringAngle / 2;

	float scatteringAngleY = std::rand() * maxScatteringAngle / RAND_MAX;
	scatteringAngleY -= maxScatteringAngle / 2;

	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::duration<double, std::milli> notshotDuration = now - lastTimeShot;
	if (notshotDuration.count() > 1000)
	{
		glm::vec3 bulletCreationPosition = position + glm::vec3(0, 2, 0) +glm::vec3(1, 1, 1) * getVecFront();
		glm::vec3 bulletCreationRotation = glm::vec3(0, rotation.y - 90 + scatteringAngleY, -rotation.x - 90 + scatteringAngleX);
		glm::vec3 bulletCreationDirection = getVecFront();
		
		//rotate direction around Y
		bulletCreationDirection = glm::rotate(bulletCreationDirection, glm::radians(scatteringAngleY), glm::vec3(0, 1, 0));

		//rotate direction around z
		bulletCreationDirection = glm::rotate(bulletCreationDirection, glm::radians(scatteringAngleX), glm::vec3(0, 0, 1));


		std::shared_ptr<Bullet> newBullet = std::make_shared<Bullet>(shader, bulletCreationPosition, bulletCreationRotation, bulletCreationDirection);
		newBullet->setNumber(Game::objects.size());

		Game::bullets.push_back(newBullet);
		Game::objects.push_back(newBullet);
		newBullet->calculationBeforeFrame();


		lastTimeShot = std::chrono::system_clock::now();

		AudioManager::play3D("audio/shoot.wav", this->getPosition());

		return newBullet;
	}
	return nullptr;
}



