#include "controller.h"
#include "gametime.h"

glm::dvec3 controller::tiePos(0.0, 0.0, 0.0);
glm::dvec3 controller::tieDir(0.0, 0.0, 0.0);
glm::dvec3 controller::tieUp(0.0, 0.0, 0.0);

glm::dvec3 controller::cameraPos(0.0, 0.0, 0.0);
glm::dvec3 controller::cameraPoint(0.0, 0.0, -4.0);
glm::dvec3 controller::cameraUp(0.0, 0.0, 0.0);

glm::dvec3 rotX(1.0, 0.0, 0.0), rotY(0.0, 1.0, 0.0), rotZ(0.0, 0.0, 1.0);

gamepad_state gamepadControl = {};

const double M_PI = 3.14159265358979323846;

glm::dvec3 rotateAnglesR(0.0, 0.0, 0.0);

const double acc = 1.0;
const double defaultMinusAcc = -0.06;
const double maxSpeed = 8.0;
float speed = 0.0;

glm::dvec3 rotateOffsetR(0.0, 0.0, 0.0);
glm::dvec3 rotateOffsetL(0.0, 0.0, 0.0);
glm::dvec3 maxRotateOffsetL(0.7, 0.35, 0.7);
glm::dvec3 defaultStopAcc(0.02, 0.01, 0.02);

glm::dquat qx, qy, qz, resultQuat;

void controller::Move()
{
    if (gamepadControl.back)
    {
        PostQuitMessage(0);
    }
    // rotate angles
    float sidesCoeff = gamepadControl.lb - gamepadControl.rb;

    static float rotateSens = 0.1;
    rotateOffsetL.x += gametime::deltaTicksF * ( - gamepadControl.lStickY * rotateSens);
    rotateOffsetL.y += gametime::deltaTicksF * (sidesCoeff * rotateSens * 0.5);
    rotateOffsetL.z += gametime::deltaTicksF * ( - gamepadControl.lStickX * rotateSens);

    if (rotateOffsetL.x > 0)
    {
        rotateOffsetL.x = max(0.0, rotateOffsetL.x - gametime::deltaTicksF * defaultStopAcc.x);
    }
    else if (rotateOffsetL.x < 0)
    {
        rotateOffsetL.x = min(0.0, rotateOffsetL.x + gametime::deltaTicksF * defaultStopAcc.x);
    }

    if (rotateOffsetL.y > 0)
    {
        rotateOffsetL.y = max(0.0, rotateOffsetL.y - gametime::deltaTicksF * defaultStopAcc.y);
    }
    else if (rotateOffsetL.y < 0)
    {
        rotateOffsetL.y = min(0.0, rotateOffsetL.y + gametime::deltaTicksF * defaultStopAcc.y);
    }

    if (rotateOffsetL.z > 0)
    {
        rotateOffsetL.z = max(0.0, rotateOffsetL.z - gametime::deltaTicksF * defaultStopAcc.z);
    }
    else if (rotateOffsetL.z < 0)
    {
        rotateOffsetL.z = min(0.0, rotateOffsetL.z + gametime::deltaTicksF * defaultStopAcc.z);
    }


    rotateOffsetL.x = max(rotateOffsetL.x, -maxRotateOffsetL.x);
    rotateOffsetL.x = min(rotateOffsetL.x, maxRotateOffsetL.x);
    rotateOffsetL.y = max(rotateOffsetL.y, -maxRotateOffsetL.y);
    rotateOffsetL.y = min(rotateOffsetL.y, maxRotateOffsetL.y);
    rotateOffsetL.z = max(rotateOffsetL.z, -maxRotateOffsetL.z);
    rotateOffsetL.z = min(rotateOffsetL.z, maxRotateOffsetL.z);

    qx = glm::angleAxis(glm::radians(rotateOffsetL.x), rotX);
    qy = glm::angleAxis(glm::radians(rotateOffsetL.y), rotY);
    qz = glm::angleAxis(glm::radians(rotateOffsetL.z), rotZ);
    resultQuat = qz * qy * qx;
    
    rotX = glm::normalize(resultQuat * rotX);
    rotY = glm::normalize(resultQuat * rotY);
    rotZ = glm::normalize(resultQuat * rotZ);

    tieDir = rotZ;
    tieUp = rotY;
    glm::dvec3 copy(rotZ);
    float straightCoeff = gamepadControl.rtPow - gamepadControl.ltPow;
    speed += gametime::deltaTicksF * (straightCoeff * acc + defaultMinusAcc);

    speed = min(speed, maxSpeed);
    speed = max(speed, 0.0);

    copy *= speed * gametime::deltaTicksF * 0.01;
    tiePos += copy;

    // camera
    static float rotateSensR = 10.0;
    rotateOffsetR.x = -gamepadControl.rStickY * rotateSensR * gametime::deltaTicksF;
    rotateOffsetR.y = -gamepadControl.rStickX * rotateSensR * gametime::deltaTicksF;
    rotateOffsetR.z = 0.0;
    rotateAnglesR += rotateOffsetR;
    if (rotateAnglesR.x >= 89.0)
    {
        rotateAnglesR.x = 89.0;
    }
    else if (rotateAnglesR.x <= -89.0)
    {
        rotateAnglesR.x = -89.0;
    }

    qx = glm::angleAxis(glm::radians(rotateAnglesR.x), rotX);
    qy = glm::angleAxis(glm::radians(rotateAnglesR.y), rotY);
    qz = glm::angleAxis(glm::radians(rotateAnglesR.z), rotZ);
    resultQuat = qz * qy * qx;

    cameraPos = 0.01 * (5.0 + 2.0 * speed / maxSpeed) * glm::normalize(resultQuat * (glm::dvec3) tieDir) - (glm::dvec3) tiePos;
    cameraPoint = -tiePos;
    cameraUp = tieUp;
}

DWORD WINAPI controller::controlThreadProc(LPVOID lpParam)
{
    while (true)
    {
        ReadGamepadInput();
    }
}

void controller::ReadGamepadInput()
{
    XINPUT_STATE gamepadState;

    // Get the state of the gamepad
    DWORD result = XInputGetState(0, &gamepadState);
    if (result == ERROR_SUCCESS)
    {
        WORD buttons = gamepadState.Gamepad.wButtons;

        SHORT rightStickX = gamepadState.Gamepad.sThumbRX;
        SHORT rightStickY = gamepadState.Gamepad.sThumbRY;
        
        gamepadControl.rtPow = gamepadState.Gamepad.bRightTrigger / 255.0;
        gamepadControl.ltPow = gamepadState.Gamepad.bLeftTrigger / 255.0;

        gamepadControl.lStickX = 0;
        gamepadControl.lStickY = 0;
        if (abs(gamepadState.Gamepad.sThumbLX) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
        {
            gamepadControl.lStickX = gamepadState.Gamepad.sThumbLX / 32768.0;
        }
        if (abs(gamepadState.Gamepad.sThumbLY) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
        {
            gamepadControl.lStickY = gamepadState.Gamepad.sThumbLY / 32768.0;
        }

        gamepadControl.rStickX = 0;
        gamepadControl.rStickY = 0;
        if (abs(gamepadState.Gamepad.sThumbRX) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
        {
            gamepadControl.rStickX = gamepadState.Gamepad.sThumbRX / 32768.0;
        }
        if (abs(gamepadState.Gamepad.sThumbRY) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
        {
            gamepadControl.rStickY = gamepadState.Gamepad.sThumbRY / 32768.0;
        }

        gamepadControl.lb = buttons & XINPUT_GAMEPAD_LEFT_SHOULDER ? 1.0 : 0.0;
        gamepadControl.rb = buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER ? 1.0 : 0.0;
        gamepadControl.back = buttons & XINPUT_GAMEPAD_BACK ? 1 : 0;
    }
}