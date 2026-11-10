#include "input/InputManager.h"
#include "common/Config.h"
#include "raylib.h"
#include <cmath>

// Junta teclado y mando en un solo resultado. Con el stick analogico la
// direccion puede ser intermedia (ej. 0.4): la nave acelera mas suave.
GameplayInput InputManager::readGameplay() const
{
    GameplayInput result{ 0.0f, false };

    // Teclado
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  result.moveDirection -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) result.moveDirection += 1.0f;
    if (IsKeyPressed(KEY_SPACE)) result.shootPressed = true;   // RF-04: una bala por pulsacion

    // Mando (gamepad)
    const int pad = Config::GAMEPAD_ID;
    if (IsGamepadAvailable(pad))
    {
        const float stick = GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_X);
        if (std::fabs(stick) > Config::GAMEPAD_DEADZONE) result.moveDirection += stick;

        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_LEFT))  result.moveDirection -= 1.0f;
        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) result.moveDirection += 1.0f;
        if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) result.shootPressed = true;
    }

    // Si se usan teclado y mando a la vez, la direccion no pasa de -1 a 1
    if (result.moveDirection > 1.0f)  result.moveDirection = 1.0f;
    if (result.moveDirection < -1.0f) result.moveDirection = -1.0f;
    return result;
}

bool InputManager::isConfirmPressed() const
{
    return IsKeyPressed(KEY_ENTER) ||
           (IsGamepadAvailable(Config::GAMEPAD_ID) &&
            IsGamepadButtonPressed(Config::GAMEPAD_ID, GAMEPAD_BUTTON_MIDDLE_RIGHT));
}

bool InputManager::isPausePressed() const
{
    return IsKeyPressed(KEY_P) ||
           (IsGamepadAvailable(Config::GAMEPAD_ID) &&
            IsGamepadButtonPressed(Config::GAMEPAD_ID, GAMEPAD_BUTTON_MIDDLE_RIGHT));
}

bool InputManager::isRestartPressed() const
{
    return IsKeyPressed(KEY_R) || isConfirmPressed();
}

bool InputManager::isMenuPressed() const
{
    return IsKeyPressed(KEY_M);
}

bool InputManager::isGamepadConnected() const
{
    return IsGamepadAvailable(Config::GAMEPAD_ID);
}
