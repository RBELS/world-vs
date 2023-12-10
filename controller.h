#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "windows.h"
#include <iostream>
#include <XInput.h>
#include <math.h>

struct gamepad_state
{
	volatile double rtPow;
	volatile double ltPow;
	volatile double lStickX, lStickY;
	volatile double rStickX, rStickY;
	volatile double lb, rb;
	volatile int back;
};

namespace controller
{
	extern glm::dvec3 tiePos;
	extern glm::dvec3 tieDir;
	extern glm::dvec3 tieUp;

	extern glm::dvec3 cameraPos;
	extern glm::dvec3 cameraPoint;
	extern glm::dvec3 cameraUp;

	void Move();
	void ReadGamepadInput();
	DWORD WINAPI controlThreadProc(LPVOID lpParam);
};