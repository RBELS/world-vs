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
extern glm::vec3 rotateAngles;
extern keys activeKeys;

extern gamepad_state gamepad;
extern glm::vec3 rotX, rotY, rotZ;

extern glm::vec3 rotAxis;
extern double rotAngle;

void moveTie();
void ReadGamepadInput();
DWORD WINAPI controlThreadProc(LPVOID lpParam);