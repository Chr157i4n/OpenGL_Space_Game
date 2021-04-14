#pragma once
#include "defines.h"

#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"

#include "Camera_FPS.h"
#include "Camera_ThirdPerson.h"
#include "UI_Element_ProgressBar.h"
#include "UI_Element_Label.h"
#include "Character.h"

class Player : public Character, public ThirdPersonCamera
{

public:

	Player(Shader* shader, float fov, float width, float height);

	//glm::vec3 getLookDirection();

	//virtual glm::vec3 getLookOrigin();

	void calculationBeforeFrame();

	void calculationAfterFrame();

	void onMouseMove(float xRel, float yRel);

	void updateCameraPosition();

	void resetCameraPosition();

	glm::vec3 getCameraPosition();

	void updateAngleAroundCharacter(float targetYawAroundCharacter, float targetPitchAroundCharacter);

	void setCameraPosition(glm::vec3 newPosition);

	void activateFlashlight(bool enabled);

	void toggleFlashlight();

	void registerHit();

	void rollLeft();

	void rollRight();

	virtual void reactToCollision(CollisionResult collisionResult);

	void createUIElements();

	virtual void addToHealth(float32 addHealth);

	void spawn(glm::vec3 position = glm::vec3(20,0,0), glm::vec3 lookAt = glm::vec3(1,0,0));

	void move();

private:

	bool flashlightActive = false;
	UI_Element_ProgressBar* prb_health;
	UI_Element_ProgressBar* prb_PlayerSpeed;
	UI_Element_Label* lbl_PlayerSpeed;

	glm::vec3 cameraOffset = glm::vec3(0, 0, 0);

	float32 maxPitch = 0.003f;
	float32 maxYaw = 0.003f;

	float32 yawAroundCharacter = 0;
	float32 pitchAroundCharacter = 10;
	float32 cameraAroundCharacterSpeed = 0.02f;

	bool MouseMoved = false;
	



};

