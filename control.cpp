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

void moveTie()
{
    // rotate angles
    float sidesCoeff = gamepadControl.lb - gamepadControl.rb;

    glm::vec3 rotateOffset(0.0, 0.0, 0.0);
    static float rotateSens = 10.0;
    rotateOffset.x = -gamepadControl.lStickY * rotateSens * gametime::deltaTicksF;
    rotateOffset.y = 5.0 * sidesCoeff * gametime::deltaTicksF;
    rotateOffset.z = -gamepadControl.lStickX * rotateSens * gametime::deltaTicksF;

    glm::quat qx = glm::angleAxis(glm::radians(rotateOffset.x), rotX);
    glm::quat qy = glm::angleAxis(glm::radians(rotateOffset.y), rotY);
    glm::quat qz = glm::angleAxis(glm::radians(rotateOffset.z), rotZ);
    glm::quat resultQuat = qz * qy * qx;
    
    rotX = glm::normalize(resultQuat * rotX);
    rotY = glm::normalize(resultQuat * rotY);
    rotZ = glm::normalize(resultQuat * rotZ);

    tieDir = rotZ;
    tieUp = rotY;
    glm::vec3 copy(rotZ);
    float straightCoeff = gamepadControl.rtPow - gamepadControl.ltPow;;
    copy *= 3.0 * straightCoeff * gametime::deltaTicksF;
    tiePos += copy;

    // camera
    cameraPos = 5.0f * tieDir - tiePos;
    cameraPoint = -tiePos;
    cameraUp = tieUp;
}

DWORD WINAPI controlThreadProc(LPVOID lpParam)
{
    while (true)
    {
        ReadGamepadInput();
        //Sleep(200);
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