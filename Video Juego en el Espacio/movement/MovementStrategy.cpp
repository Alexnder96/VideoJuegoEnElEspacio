#include "movement/MovementStrategy.h"
#include "common/Config.h"
#include <cmath>

namespace
{
    constexpr float TWO_PI = 6.28318530718f;
}

// ---------------------------- StraightMove ----------------------------------
// Solo avanza en Y. speedX no se usa.
void StraightMove::move(float& /*x*/, float& y, float /*speedX*/, float speedY, float deltaTime)
{
    y += speedY * deltaTime;
}

// ---------------------------- SinusoidalMove --------------------------------
SinusoidalMove::SinusoidalMove(float oscFrequency, float oscAmplitude, float startPhase)
    : frequency(oscFrequency), amplitude(oscAmplitude), phase(startPhase),
      initialX(0.0f), elapsed(0.0f), initialized(false)
{
}

// Baja a velocidad constante y calcula X con una funcion seno respecto al
// punto donde aparecio la nave. speedX no se usa: la amplitud define el ancho.
void SinusoidalMove::move(float& x, float& y, float /*speedX*/, float speedY, float deltaTime)
{
    if (!initialized)
    {
        initialX = x;
        initialized = true;
    }

    elapsed += deltaTime;
    y += speedY * deltaTime;
    x = initialX + amplitude * std::sin(TWO_PI * frequency * elapsed + phase);
}

// ---------------------------- ZigZagMove ------------------------------------
ZigZagMove::ZigZagMove(float interval, bool startRight)
    : directionTimer(0.0f), switchInterval(interval), moveRight(startRight)
{
}

// Se mueve en diagonal. Cambia de sentido al cumplirse el intervalo o al
// llegar a un borde de la pantalla, formando la trayectoria en "Z".
void ZigZagMove::move(float& x, float& y, float speedX, float speedY, float deltaTime)
{
    directionTimer += deltaTime;
    if (directionTimer >= switchInterval)
    {
        directionTimer = 0.0f;
        moveRight = !moveRight;
    }

    const float direction = moveRight ? 1.0f : -1.0f;
    x += direction * speedX * deltaTime;
    y += speedY * deltaTime;

    // Rebote en los bordes (el ancho maximo de una nave es 48 px)
    const float maxX = static_cast<float>(Config::SCREEN_WIDTH) - 48.0f;
    if (x < 0.0f)  { x = 0.0f;  moveRight = true;  directionTimer = 0.0f; }
    if (x > maxX)  { x = maxX;  moveRight = false; directionTimer = 0.0f; }
}
