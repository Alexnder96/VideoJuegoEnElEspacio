#include "entities/Enemy.h"
#include "resources/AssetManager.h"
#include "common/Config.h"
#include "common/Random.h"
#include <cmath>
#include <utility>

Enemy::Enemy()
    : Entity(0.0f, 0.0f, 0.0f, 0.0f),
      movementStrategy(nullptr),
      pointsValue(0),
      enemyType(EnemyType::STRAIGHT),
      health(1),
      canFire(false),
      fireCooldown(0.0f),
      hitFlash(0.0f)
{
}

Enemy::Enemy(float startX, float startY, std::unique_ptr<MovementStrategy> strategy)
    : Enemy()
{
    x = startX;
    y = startY;
    movementStrategy = std::move(strategy);
    isActive = true;
}

// Cada tipo tiene su sprite sheet, puntaje, resistencia y estrategia.
void Enemy::spawn(float startX, float startY, EnemyType type,
                  float waveSpeed, float speedMultiplier, bool fireEnabled)
{
    const AssetManager& assets = AssetManager::getInstance();
    enemyType = type;

    switch (type)
    {
    case EnemyType::STRAIGHT:
        setSpriteSheet(assets.getTexture(TEX_ENEMY_STRAIGHT), Config::ENEMY_FRAMES, Config::ENEMY_ANIM_FPS, true);
        setStrategy(std::make_unique<StraightMove>());
        pointsValue = 100;
        health = 1;
        speedX = 0.0f;
        speedY = waveSpeed;
        break;

    case EnemyType::SINUSOIDAL:
        setSpriteSheet(assets.getTexture(TEX_ENEMY_SINE), Config::ENEMY_FRAMES, Config::ENEMY_ANIM_FPS, true);
        // frecuencia, amplitud y fase al azar para que no se muevan todas igual
        setStrategy(std::make_unique<SinusoidalMove>(RandomFloat(0.5f, 1.0f),
                                                     RandomFloat(40.0f, 80.0f),
                                                     RandomFloat(0.0f, 6.28f)));
        pointsValue = 150;
        health = 1;
        speedX = 0.0f;
        speedY = waveSpeed * 0.9f;
        break;

    case EnemyType::ZIGZAG:
        setSpriteSheet(assets.getTexture(TEX_ENEMY_ZIGZAG), Config::ENEMY_FRAMES, Config::ENEMY_ANIM_FPS, true);
        setStrategy(std::make_unique<ZigZagMove>(RandomFloat(0.7f, 1.2f), GetRandomValue(0, 1) == 1));
        pointsValue = 200;
        health = 2;
        speedX = 170.0f * speedMultiplier;
        speedY = waveSpeed * 0.85f;
        break;
    }

    x = startX;
    y = startY;
    hitboxInset = 5.0f;
    canFire = fireEnabled;
    fireCooldown = RandomFloat(0.5f, Config::ENEMY_FIRE_COOLDOWN);
    hitFlash = 0.0f;
    isActive = true;
}

// La estrategia anterior se borra sola (unique_ptr).
void Enemy::setStrategy(std::unique_ptr<MovementStrategy> strategy)
{
    movementStrategy = std::move(strategy);
}

void Enemy::update(float deltaTime)
{
    if (!isActive) return;

    // 1) Movimiento polimorfico: la estrategia calcula la nueva posicion.
    if (movementStrategy != nullptr)
        movementStrategy->move(x, y, speedX, speedY, deltaTime);

    // 2) Mantener la nave dentro de los bordes laterales.
    const float maxX = static_cast<float>(Config::SCREEN_WIDTH) - width;
    if (x < 0.0f) x = 0.0f;
    if (x > maxX) x = maxX;

    // 3) Si escapa por abajo vuelve a entrar por arriba: la oleada solo
    //    termina cuando TODAS las naves son destruidas (RF-09).
    if (y > static_cast<float>(Config::SCREEN_HEIGHT))
        y = static_cast<float>(Config::HUD_HEIGHT) - height;

    // 4) Animacion del sprite sheet y temporizadores.
    animation.update(deltaTime);
    if (fireCooldown > 0.0f) fireCooldown -= deltaTime;
    if (hitFlash > 0.0f)     hitFlash -= deltaTime;
}

void Enemy::render() const
{
    if (!isActive) return;

    // Al recibir un impacto se dibuja semitransparente por un instante.
    const Color tint = (hitFlash > 0.0f) ? Color{ 255, 255, 255, 120 } : WHITE;
    animation.draw(x, y, tint);
}

// Tecnica de IA simple basada en reglas:
//   PERCEPCION: calcula la distancia horizontal hasta el jugador.
//   DECISION:   dispara solo si esta "a tiro", visible y con el arma lista.
bool Enemy::decideToFire(float playerCenterX)
{
    if (!isActive || !canFire || fireCooldown > 0.0f) return false;

    const bool visible = y > static_cast<float>(Config::HUD_HEIGHT) &&
                         y < static_cast<float>(Config::SCREEN_HEIGHT) * 0.65f;
    const float distanceX = std::fabs((x + width / 2.0f) - playerCenterX);

    if (visible && distanceX < Config::AI_AIM_RANGE)
    {
        fireCooldown = Config::ENEMY_FIRE_COOLDOWN;
        return true;
    }
    return false;
}

bool Enemy::takeDamage(int amount)
{
    health -= amount;
    hitFlash = 0.08f;
    if (health <= 0)
    {
        isActive = false;
        return true;
    }
    return false;
}

int       Enemy::getPointsValue() const { return pointsValue; }
EnemyType Enemy::getEnemyType() const   { return enemyType; }

Vector2 Enemy::getGunPosition() const
{
    return Vector2{ x + width / 2.0f, y + height };
}
