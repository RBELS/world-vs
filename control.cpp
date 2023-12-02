#include "control.h"
#include "gametime.h"

glm::vec3 tiePos(0.0, 0.0, 0.0);
glm::vec3 tieDir(0.0, 0.0, 0.0);
glm::vec3 rotateAngles(0.0, 0.0, 0.0);
glm::highp_ivec2 prevMousePos(0, 0);

glm::vec4 rotX(1.0, 0.0, 0.0, 0.0), rotY(0.0, 1.0, 0.0, 0.0), rotZ(0.0, 0.0, 1.0, 0.0);

glm::vec3 rotAxis;
double rotAngle;

// private
bool firstMouseMove = true;

//camera

keys activeKeys = { false, false, false, false };
gamepad_state gamepadControl = {};

const double M_PI = 3.14159265358979323846;

//glm::vec3 getAnglesBetween(glm::vec3 v1, glm::vec3 v2)
//{
//    // Находим угол между векторами
//    float cosTheta = glm::dot(v1, v2);
//    float theta = glm::acos(cosTheta);
//    if (theta == 0)
//    {
//        return glm::vec3(0.0, 0.0, 0.0);
//    }
//
//    // Находим ось вращения
//    glm::vec3 rotationAxis = glm::normalize(glm::cross(v1, v2));
//
//    // Создаем кватернион поворота
//    glm::quat rotationQuaternion = glm::angleAxis(theta, rotationAxis);
//
//    // Извлекаем углы вращения
//    float theta_x = glm::atan(2.0f * (rotationQuaternion.y * rotationQuaternion.z + rotationQuaternion.w * rotationQuaternion.x),
//        1.0f - 2.0f * (rotationQuaternion.x * rotationQuaternion.x + rotationQuaternion.y * rotationQuaternion.y));
//
//    float theta_y = glm::asin(2.0f * (rotationQuaternion.w * rotationQuaternion.y - rotationQuaternion.x * rotationQuaternion.z));
//
//    float theta_z = glm::atan(2.0f * (rotationQuaternion.x * rotationQuaternion.y + rotationQuaternion.w * rotationQuaternion.z),
//        1.0f - 2.0f * (rotationQuaternion.y * rotationQuaternion.y + rotationQuaternion.z * rotationQuaternion.z));
//
//    // Вывод результатов
//    /*std::cout << "Угол вращения вокруг оси Ox: " << glm::degrees(theta_x) << " градусов\n";
//    std::cout << "Угол вращения вокруг оси Oy: " << glm::degrees(theta_y) << " градусов\n";
//    std::cout << "Угол вращения вокруг оси Oz: " << glm::degrees(theta_z) << " градусов\n";*/
//
//    return glm::vec3(glm::degrees(theta_x), glm::degrees(theta_y), glm::degrees(theta_z));
//}

void moveTie()
{
    //rotate angles
    float sidesCoeff = gamepadControl.lb - gamepadControl.rb;

    glm::vec3 rotateOffset(0.0, 0.0, 0.0);
    static float rotateSens = 1.0;
    rotateOffset.x = gamepadControl.lStickY * rotateSens;
    rotateOffset.y = 5.0 * sidesCoeff * gametime::deltaTicksF;
    rotateOffset.z = gamepadControl.lStickX * rotateSens;

    rotateAngles += rotateOffset;

    glm::quat qx = glm::angleAxis(glm::radians(rotateAngles.x), glm::vec3(1.0, 0.0, 0.0));
    glm::quat qy = glm::angleAxis(glm::radians(rotateAngles.y), glm::vec3(0.0, 1.0, 0.0));
    glm::quat qz = glm::angleAxis(glm::radians(rotateAngles.z), glm::vec3(0.0, 0.0, 1.0));

    glm::quat result = qz * qy * qx;
    rotAxis = glm::normalize(glm::vec3(result.x, result.y, result.z));
    rotAngle = glm::degrees(2.0 * acos(result.w));

    printf("%f\t%f\t%f\t%f\n", rotAxis.x, rotAxis.y, rotAxis.z, rotAngle);
    
    glm::vec3 newX = glm::normalize(result * glm::vec3(1.0, 0.0, 0.0));
    glm::vec3 newY = glm::normalize(result * glm::vec3(0.0, 1.0, 0.0));
    glm::vec3 newZ = glm::normalize(result * glm::vec3(0.0, 0.0, 1.0));

    glm::vec3 copy(newZ);
    float straightCoeff = gamepadControl.rtPow - gamepadControl.ltPow;;
    copy *= 3.0 * straightCoeff * gametime::deltaTicksF;
    tiePos += copy;

    /*printf("%f\t%f\t%f\t|||", newX.x, newX.y, newX.z);
    printf("%f\t%f\t%f\t|||", newY.x, newY.y, newY.z);
    printf("%f\t%f\t%f\n", newZ.x, newZ.y, newZ.z);*/
}

void ProcessMouseMove(DWORD posDword)
{
    /*PWORD ptrWord = (PWORD)&posDword;
    WORD xPos = ptrWord[1], yPos = ptrWord[0];

    if (firstMouseMove) {
        prevMousePos.x = xPos;
        prevMousePos.y = yPos;
        firstMouseMove = false;
    }

    glm::vec3 mouseOffset(-((float)xPos - (float)prevMousePos.x), -((float)yPos - (float)prevMousePos.y), 0.0);
    prevMousePos.x = xPos;
    prevMousePos.y = yPos;

    static float mouseSens = 0.1;
    mouseOffset *= mouseSens;
    rotateAngles += mouseOffset;*/
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