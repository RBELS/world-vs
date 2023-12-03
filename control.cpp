#include "control.h"
#include "gametime.h"

glm::vec3 tiePos(0.0, 0.0, 0.0);
glm::vec3 tieDir(0.0, 0.0, 0.0);
glm::vec3 tieUp(0.0, 0.0, 0.0);

glm::vec3 cameraPos(0.0, 0.0, 0.0);
glm::vec3 cameraPoint(0.0, 0.0, -4.0);
glm::vec3 cameraUp(0.0, 0.0, 0.0);

glm::vec3 rotX(1.0, 0.0, 0.0), rotY(0.0, 1.0, 0.0), rotZ(0.0, 0.0, 1.0);

keys activeKeys = { false, false, false, false };
gamepad_state gamepadControl = {};

const double M_PI = 3.14159265358979323846;

glm::vec3 rotateAnglesR(0.0, 0.0, 0.0);

const float acc = 1.0;
const float defaultMinusAcc = -0.06;
const float maxSpeed = 8.0;
float speed = 0.0;

glm::vec3 rotateOffsetR(0.0, 0.0, 0.0);
glm::vec3 rotateOffsetL(0.0, 0.0, 0.0);
glm::vec3 maxRotateOffsetL(0.7, 0.35, 0.7);
glm::vec3 defaultStopAcc(0.02, 0.01, 0.02);

void moveTie()
{
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

    glm::quat qx = glm::angleAxis(glm::radians(rotateOffsetL.x), rotX);
    glm::quat qy = glm::angleAxis(glm::radians(rotateOffsetL.y), rotY);
    glm::quat qz = glm::angleAxis(glm::radians(rotateOffsetL.z), rotZ);
    glm::quat resultQuat = qz * qy * qx;
    
    rotX = glm::normalize(resultQuat * rotX);
    rotY = glm::normalize(resultQuat * rotY);
    rotZ = glm::normalize(resultQuat * rotZ);

    tieDir = rotZ;
    tieUp = rotY;
    glm::vec3 copy(rotZ);
    float straightCoeff = gamepadControl.rtPow - gamepadControl.ltPow;
    speed += gametime::deltaTicksF * (straightCoeff * acc + defaultMinusAcc);

    speed = min(speed, maxSpeed);
    speed = max(speed, 0.0);

    copy *= speed * gametime::deltaTicksF;
    tiePos += copy;

    // camera
    static float rotateSensR = 10.0;
    rotateOffsetR.x = -gamepadControl.rStickY * rotateSensR * gametime::deltaTicksF;
    rotateOffsetR.y = -gamepadControl.rStickX * rotateSensR * gametime::deltaTicksF;
    rotateOffsetR.z = 0.0;
    rotateAnglesR += rotateOffsetR;
    if (rotateAnglesR.x >= 89.0f)
    {
        rotateAnglesR.x = 89.0f;
    }
    else if (rotateAnglesR.x <= -89.0f)
    {
        rotateAnglesR.x = -89.0f;
    }

    qx = glm::angleAxis(glm::radians(rotateAnglesR.x), rotX);
    qy = glm::angleAxis(glm::radians(rotateAnglesR.y), rotY);
    qz = glm::angleAxis(glm::radians(rotateAnglesR.z), rotZ);
    resultQuat = qz * qy * qx;

    cameraPos = (5.0f + 2.0f * speed / maxSpeed) * glm::normalize(resultQuat * tieDir) - tiePos;
    cameraPoint = -tiePos;
    cameraUp = tieUp;
}

DWORD WINAPI controlThreadProc(LPVOID lpParam)
{
    while (true)
    {
        ReadGamepadInput();
    }
}

void ReadGamepadInput()
{
    XINPUT_STATE gamepadState;

    // Get the state of the gamepad
    DWORD result = XInputGetState(0, &gamepadState);
    if (result == ERROR_SUCCESS)
    {
        WORD buttons = gamepadState.Gamepad.wButtons;

        SHORT rightStickX = gamepadState.Gamepad.sThumbRX;
        SHORT rightStickY = gamepadState.Gamepad.sThumbRY;
        

        //std::cout << "RT: " << (int) gamepadState.Gamepad.bRightTrigger << std::endl;
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
    }
    else
    {
        std::cout << "Something was wrong..." << std::endl;
    }
}