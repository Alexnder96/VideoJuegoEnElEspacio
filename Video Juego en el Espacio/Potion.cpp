#include "Potion.h"
#include "AssetManager.h"
#include "Config.h"
#include <cmath>

// La estrategia de caida se crea UNA vez aqui (el pool crea las pociones al
// iniciar el juego), asi que durante la partida no se reserva memoria.
Potion::Potion()
    : Entity(0.0f, 0.0f, 0.0f, 0.0f),
      fallStrategy(std::make_unique<StraightMove>()),
      glowTimer(0.0f)
{
    width = 24.0f;
    height = 24.0f;
}

void Potion::spawn(float startX, float fallSpeed)
{
    setTexture(AssetManager::getInstance().getTexture(TEX_POTION));
    x = startX;
    y = static_cast<float>(Config::HUD_HEIGHT) - height;
    speedX = 0.0f;
    speedY = fallSpeed;
    glowTimer = 0.0f;
    isActive = true;
}

// Cae con la misma estrategia recta que las naves (RF-11).
void Potion::update(float deltaTime)
{
    if (!isActive) return;

    fallStrategy->move(x, y, speedX, speedY, deltaTime);
    glowTimer += deltaTime;

    if (y > static_cast<float>(Config::SCREEN_HEIGHT))
        isActive = false;
}

void Potion::render() const
{
    if (!isActive) return;

    // Halo que late detras de la pocion para que se note
    const float pulse = 0.5f + 0.5f * std::sin(glowTimer * 6.0f);
    const unsigned char alpha = static_cast<unsigned char>(60.0f + 80.0f * pulse);
    DrawCircleV(Vector2{ x + width / 2.0f, y + height / 2.0f }, width * 0.85f, Color{ 255, 80, 120, alpha });

    Entity::render();
}
