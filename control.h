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

extern glm::vec3 tiePos;
extern glm::vec3 tieDir;
extern glm::vec3 tieUp;
extern keys activeKeys;

extern glm::vec3 cameraPos;
extern glm::vec3 cameraPoint;
extern glm::vec3 cameraUp;

extern gamepad_state gamepad;

void moveTie();
void ReadGamepadInput();
DWORD WINAPI controlThreadProc(LPVOID lpParam);