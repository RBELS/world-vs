#pragma once

#include "data.h"
#include "screen.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "windows.h"
#include <iostream>
#include <XInput.h>
#include <math.h>

extern glm::dvec3 tiePos;
extern glm::dvec3 tieDir;
extern glm::dvec3 tieUp;
extern keys activeKeys;

extern glm::dvec3 cameraPos;
extern glm::dvec3 cameraPoint;
extern glm::dvec3 cameraUp;

extern gamepad_state gamepad;

void moveTie();
void ReadGamepadInput();
DWORD WINAPI controlThreadProc(LPVOID lpParam);