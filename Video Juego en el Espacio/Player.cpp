#include "Player.h"
#include "Config.h"

Player::Player(float startX, float startY)
    : Entity(startX, startY, Config::PLAYER_MAX_SPEED, 0.0f),
      health(Config::PLAYER_MAX_HEALTH),
      maxHealth(Config::PLAYER_MAX_HEALTH),
      score(0),
      velocityX(0.0f),
      shootCooldown(0.0f),
      timer(0.0f)
{
    width = 48.0f;
    height = 52.0f;
    hitboxInset = 9.0f;     // la caja de choque es mas pequena que el dibujo
    isActive = true;
}

// Los temporizadores bajan con delta time: duran lo mismo en cualquier PC.
void Player::update(float deltaTime)
{
    animation.update(deltaTime);
    if (shootCooldown > 0.0f) shootCooldown -= deltaTime;
    if (timer > 0.0f)         timer -= deltaTime;
}

// Mientras es invulnerable la nave parpadea.
void Player::render() const
{
    if (!isActive) return;

    if (isInvulnerable())
    {
        const int blink = static_cast<int>(timer * 12.0f);
        if (blink % 2 == 0) return;
    }
    Entity::render();
}

// Aceleracion:  velocidad = velocidad + aceleracion * dt
// Friccion:     si no hay direccion, la velocidad baja hacia 0.
// Luego:        posicion = posicion + velocidad * dt
void Player::accelerate(float direction, float deltaTime)
{
    if (direction != 0.0f)
    {
        velocityX += direction * Config::PLAYER_ACCELERATION * deltaTime;
    }
    else
    {
        const float brake = Config::PLAYER_FRICTION * deltaTime;
        if (velocityX > brake)       velocityX -= brake;
        else if (velocityX < -brake) velocityX += brake;
        else                         velocityX = 0.0f;
    }

    // Limitar a la velocidad maxima
    if (velocityX >  Config::PLAYER_MAX_SPEED) velocityX =  Config::PLAYER_MAX_SPEED;
    if (velocityX < -Config::PLAYER_MAX_SPEED) velocityX = -Config::PLAYER_MAX_SPEED;

    move(velocityX * deltaTime, 0.0f);
}

// RF-01: la X queda entre 0 y (ancho de pantalla - ancho de la nave).
// Al chocar con un borde la velocidad se anula.
void Player::move(float dx, float /*dy*/)
{
    x += dx;
    const float maxX = static_cast<float>(Config::SCREEN_WIDTH) - width;
    if (x < 0.0f)  { x = 0.0f;  velocityX = 0.0f; }
    if (x > maxX)  { x = maxX;  velocityX = 0.0f; }
}

bool Player::canShoot() const
{
    return isActive && shootCooldown <= 0.0f;
}

void Player::shoot()
{
    shootCooldown = Config::PLAYER_SHOOT_COOLDOWN;
}

Vector2 Player::getGunPosition() const
{
    return Vector2{ x + width / 2.0f, y };
}

// RF-07: cada impacto resta vida y activa unos segundos de invulnerabilidad
// para que un mismo choque no quite varias vidas seguidas.
bool Player::takeDamage(int amount)
{
    if (!isActive || isInvulnerable()) return false;

    health -= amount;
    if (health < 0) health = 0;
    timer = Config::PLAYER_INVULNERABLE;
    return true;
}

void Player::heal(int amount)
{
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

int   Player::getHealth() const      { return health; }
int   Player::getMaxHealth() const   { return maxHealth; }
bool  Player::isAlive() const        { return health > 0; }
bool  Player::isInvulnerable() const { return timer > 0.0f; }
float Player::getVelocity() const    { return velocityX; }

float Player::getHealthPercent() const
{
    return static_cast<float>(health) / static_cast<float>(maxHealth);
}

void Player::addScore(int points)    { score += points; }
int  Player::getScore() const        { return score; }

void Player::reset(float startX, float startY)
{
    x = startX;
    y = startY;
    health = maxHealth;
    score = 0;
    velocityX = 0.0f;
    shootCooldown = 0.0f;
    timer = 0.0f;
    isActive = true;
    animation.reset();
}
