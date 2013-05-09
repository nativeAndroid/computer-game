#pragma once

#include "system.h"

#include <SFML/System.hpp>
#include <GLM/glm.hpp>

// helpers
#include <string>
#include <GLM/gtc/quaternion.hpp>
#include <BULLET/btBulletDynamicsCommon.h>


class ModuleCamera : public Module
{
	// general
	sf::Clock clock;
	float delta;
	void Init();
	void Update();
	void Listeners();

	// create
	unsigned int Create(glm::vec3 Position = glm::vec3(0), float Height = 2.0f);

	// state
	bool focus;
	void State();
	void State(bool Active);

	// conrol
	float campitch;
	void Rotate(glm::vec3 Amount, float Sensitivity = 0.002f);
	void Move(glm::vec3 Amount, float Speed = 7.0f);
	bool onground;
	std::pair<btVector3, btVector3> Ray(btVector3 &From, btVector3 &To);

	// calculation
	void Projection();
	void Projection(sf::Vector2u Size);
	void Calculate();
};
