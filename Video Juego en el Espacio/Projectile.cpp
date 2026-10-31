#include "Projectile.h"
#include "AssetManager.h"
#include "Config.h"

Projectile::Projectile()
    : Entity(0.0f, 0.0f, 0.0f, 0.0f),
      damage(1),
      isPlayerBullet(true)
{
}

// RF-04: la bala del jugador viaja en Y negativa. Al salir de la zona de
// juego se desactiva y vuelve a quedar libre en el pool (RNF-01).
void Projectile::update(float deltaTime)
{
    if (!isActive) return;

    y += speedY * deltaTime;

    const bool outTop    = (y + height) < static_cast<float>(Config::HUD_HEIGHT);
    const bool outBottom = y > static_cast<float>(Config::SCREEN_HEIGHT);
    if (outTop || outBottom)
        isActive = false;
}

// Reutiliza la bala: asigna textura y dano segun quien dispara y la centra
// en startX.
void Projectile::reset(float startX, float startY, float newSpeedY, bool isPlayer)
{
    const AssetManager& assets = AssetManager::getInstance();
    isPlayerBullet = isPlayer;
    setTexture(assets.getTexture(isPlayer ? TEX_BULLET_PLAYER : TEX_BULLET_ENEMY));
    damage = isPlayer ? Config::PLAYER_BULLET_DAMAGE : Config::ENEMY_BULLET_DAMAGE;

    x = startX - width / 2.0f;
    y = isPlayer ? startY - height : startY;
    speedX = 0.0f;
    speedY = newSpeedY;
    isActive = true;
}

int  Projectile::getDamage() const    { return damage; }
bool Projectile::isFromPlayer() const { return isPlayerBullet; }
