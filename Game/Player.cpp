#include "Player.h"

#include "Game.h"
#include "Bullet.h"
#include "Shader.h"

Player::Player(Shader* shader, float fov, float width, float height) : Character(shader), ThirdPersonCamera(fov, width, height)
{
	setType(ObjectType::Object_Player | ObjectType::Object_Character);
	

	position.x = 20;
	position.y = 0;
	position.z = 0;

	setRotationQuat(rotationQuat);
	calculateFrontandUpVector();
	lookAt = vecFront;
	resetCameraPosition();

	this->name = "Player";

	createUIElements();
}


void Player::onMouseMove(float xRel, float yRel)
{

	//Logger::log("vecRight: ("+std::to_string(vecRight.x)+"|" + std::to_string(vecRight.y) + "|" + std::to_string(vecRight.z) + ")");
	//Logger::log("quat: (" + std::to_string(rotationQuat.x) + "|" + std::to_string(rotationQuat.y) + "|" + std::to_string(rotationQuat.z) + "|" + std::to_string(rotationQuat.w) + ")");
	
	float maxPitch=0.003f;
	float maxYaw = 0.003f;

	float anglePitch = glm::radians(yRel * mouseSensitivity);
	float angleYaw = glm::radians(-xRel * mouseSensitivity);
	anglePitch = std::min(anglePitch, maxPitch);
	angleYaw = std::min(angleYaw, maxYaw);
	anglePitch = std::max(anglePitch, -maxPitch);
	angleYaw = std::max(angleYaw, -maxYaw);


	//Logger::log("Pitch: "+std::to_string(anglePitch)+"\tYaw: "+ std::to_string(angleYaw));

	glm::quat qPitch = glm::angleAxis(anglePitch, glm::vec3(1, 0, 0));
	glm::quat qYaw = glm::angleAxis(angleYaw, glm::vec3(0, 1, 0));
	// omit roll
	rotationQuat = rotationQuat * qPitch * qYaw;

	calculateFrontandUpVector();


	//rotationQuat = glm::rotate(rotationQuat, glm::radians(yRel * mouseSensitivity), vecRight);
	//rotationQuat = glm::rotate(rotationQuat, glm::radians(-xRel * mouseSensitivity), vecUp);

	setRotationQuat(rotationQuat);
	



	//onMouseMoved();

	//calculateFrontandUpVector();
	lookAt = vecFront;
	updateCameraPosition();
}

void Player::updateCameraPosition()
{
	float cameraspeed = 0.1;

	glm::vec3 cameratargetposition = position - getVecFront() * camera_distancetoPlayer;
	cameratargetposition += getVecUp() * glm::vec3(2);

	glm::vec3 cameramovedirection = cameratargetposition - cameraposition;
	float cameratargetdistance = glm::length(cameramovedirection);
	cameramovedirection = glm::normalize(cameramovedirection);

	if (cameratargetdistance > cameraspeed) {
		cameraposition += cameramovedirection * glm::vec3(cameraspeed);
	}
	else {
		cameraposition = cameratargetposition;
	}

	lookAt = vecFront;
	update2(vecUp);
}

void Player::resetCameraPosition()
{
	cameraposition = position - getVecFront() * camera_distancetoPlayer;
	cameraposition += getVecUp() * glm::vec3(2);

	lookAt = vecFront;
	update2(vecUp);
}



glm::vec3 Player::getCameraPosition()
{
	return cameraposition;
}

void Player::setCameraPosition(glm::vec3 newPosition)
{
	cameraposition = newPosition;
}

void Player::activateFlashlight(bool enabled)
{
	glm::vec3 spotLightColor = glm::vec3(0);

	if (enabled)
	{
		spotLightColor = glm::vec3(1.0f);
	}
	else {
		spotLightColor = glm::vec3(0);
	}
	shader->bind();
	GLCALL(glUniform3fv(glGetUniformLocation(shader->getShaderId(), "u_spot_light.diffuse"), 1, (float*)&spotLightColor));
	GLCALL(glUniform3fv(glGetUniformLocation(shader->getShaderId(), "u_spot_light.specular"), 1, (float*)&spotLightColor));
	spotLightColor *= 0.2f;
	GLCALL(glUniform3fv(glGetUniformLocation(shader->getShaderId(), "u_spot_light.ambient"), 1, (float*)&spotLightColor));
	shader->unbind();
}

void Player::toggleFlashlight()
{
	glm::vec3 spotLightColor = glm::vec3(0);
	flashlightActive = !flashlightActive;

	if (flashlightActive)
	{
		spotLightColor = glm::vec3(1.0f);
		shader->bind();
		GLCALL(glUniform3fv(glGetUniformLocation(shader->getShaderId(), "u_spot_light.color"), 1, (float*)&spotLightColor));
		shader->unbind();
	}
	else 
	{
		shader->bind();
		GLCALL(glUniform3fv(glGetUniformLocation(shader->getShaderId(), "u_spot_light.color"), 1, (float*)&spotLightColor));
		shader->unbind();
	}


}

void Player::registerHit()
{
	addToHealth(-20);
	prb_health->setValue(health);
}

void Player::rollLeft()
{
	Character::rollLeft();
	updateCameraPosition();
}

void Player::rollRight()
{
	Character::rollRight();
	updateCameraPosition();
}

void Player::reactToCollision(CollisionResult collisionResult)
{
	this->Object::reactToCollision(collisionResult);
}

void Player::createUIElements()
{
	prb_health = new UI_Element_ProgressBar(10, 10, 100, 20, 0, 0);
	prb_health->setForeColor(glm::vec4(1, 0, 0, 0.5));
	prb_health->setBackColor(glm::vec4(0.2, 0.2, 0.2, 0.4));
	prb_health->setValue(100);
	UI::addElement(prb_health);


	prb_PlayerSpeed = new UI_Element_ProgressBar(120, 10, 100, 20, 0, 0);
	prb_PlayerSpeed->setForeColor(glm::vec4(0, 0.5, 1, 0.5));
	prb_PlayerSpeed->setBackColor(glm::vec4(0.2, 0.2, 0.2, 0.4));
	prb_PlayerSpeed->setValue(actualSpeed);
	prb_PlayerSpeed->setMaxValue(maxSpeed);
	UI::addElement(prb_PlayerSpeed);

	lbl_PlayerSpeed = new UI_Element_Label(120, 40, 100, 50, "0.00", 0, 1, glm::vec4(0, 0.5, 1, 0.5), glm::vec4(0.2, 0.2, 0.2, 0.4));
	UI::addElement(lbl_PlayerSpeed);
}

void Player::addToHealth(float32 addHealth)
{
	health += addHealth;
	prb_health->setValue(health);

	if (addHealth < -10)
	{
		Renderer::applyPostprocessingEffect(PostProcessingEffect::blood, 0.005 * std::abs(addHealth));
	}

	if (addHealth > 0)
		Logger::info(printObject() + " got healed by " + std::to_string((int)addHealth) + ". New Health: " + std::to_string((int)health));
	if (addHealth < 0)
		Logger::info(printObject() + " got " + std::to_string((int)addHealth) + " damage. New Health: " + std::to_string((int)health));

	if (health <= 0)
	{
		Logger::info(printObject() + " got destroyed");
		UI_Element* victoryLabel = new UI_Element_Label(Game::getWindowWidth() / 2 - 80, Game::getWindowHeight() / 2, 10, 10, "Du bist gestorben", 5000, 1, glm::vec4(1, 0, 0, 1), glm::vec4(0.2, 0.2, 0.2, 0.4), false);
		UI::addElement(victoryLabel);
		Game::setGameState(GameState::GAME_GAME_OVER);
	}
}

void Player::spawn(glm::vec3 position, glm::vec3 lookAt)
{
	this->setPosition(position);
	this->lookAt = lookAt;

	this->health = 100;
	prb_health->setValue(100);

	this->setEnabled(true);
}

void Player::move()
{
	this->Object::move();

	prb_PlayerSpeed->setValue(actualSpeed);
	lbl_PlayerSpeed->setText(Helper::to_string_with_precision(actualSpeed,2));

	if (movement != glm::vec3(0, 0, 0))
	{
		NetworkManager::sendPlayerPosition();
	}
}