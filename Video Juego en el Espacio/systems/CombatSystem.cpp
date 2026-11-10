#include "systems/CombatSystem.h"
#include "resources/AssetManager.h"
#include "common/Config.h"

// Toma una bala libre del pool. Si no hay, no dispara (no se crea memoria nueva).
bool CombatSystem::firePlayerProjectile(GameWorld& world) const
{
    Projectile* bullet = world.projectiles.acquire();
    if (bullet == nullptr) return false;

    const Vector2 gun = world.player.getGunPosition();
    bullet->reset(gun.x, gun.y, Config::PLAYER_BULLET_SPEED, true);
    world.player.shoot();
    AssetManager::getInstance().playSound(SND_SHOOT);
    return true;
}

void CombatSystem::updateEnemyAI(GameWorld& world) const
{
    const Rectangle p = world.player.getGlobalBounds();
    const float playerCenterX = p.x + p.width / 2.0f;

    for (int i = 0; i < world.enemies.size(); i++)
    {
        Enemy& enemy = world.enemies.get(i);
        if (enemy.isEntityActive() && enemy.decideToFire(playerCenterX))
            fireEnemyProjectile(world, enemy);
    }
}

void CombatSystem::fireEnemyProjectile(GameWorld& world, const Enemy& enemy) const
{
    Projectile* bullet = world.projectiles.acquire();
    if (bullet == nullptr) return;

    const Vector2 gun = enemy.getGunPosition();
    bullet->reset(gun.x, gun.y, Config::ENEMY_BULLET_SPEED, false);
}
