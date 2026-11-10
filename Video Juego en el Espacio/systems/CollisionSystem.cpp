#include "systems/CollisionSystem.h"
#include "resources/AssetManager.h"
#include "common/Config.h"

CollisionReport CollisionSystem::resolve(GameWorld& world) const
{
    CollisionReport report{ 0 };
    bulletsVsShips(world);
    enemiesVsPlayer(world);
    report.potionsCollected = potionsVsPlayer(world);
    return report;
}

void CollisionSystem::bulletsVsShips(GameWorld& world) const
{
    const AssetManager& assets = AssetManager::getInstance();
    Player& player = world.player;
    const Rectangle playerBox = player.getGlobalBounds();

    for (int b = 0; b < world.projectiles.size(); b++)
    {
        Projectile& bullet = world.projectiles.get(b);
        if (!bullet.isEntityActive()) continue;

        if (bullet.isFromPlayer())
        {
            // a) Bala del jugador contra enemigo: se destruyen y suma puntaje
            for (int e = 0; e < world.enemies.size(); e++)
            {
                Enemy& enemy = world.enemies.get(e);
                if (!enemy.isEntityActive()) continue;
                if (!CheckCollisionRecs(bullet.getGlobalBounds(), enemy.getGlobalBounds())) continue;

                bullet.setActive(false);
                const Rectangle box = enemy.getGlobalBounds();
                if (enemy.takeDamage(bullet.getDamage()))
                {
                    player.addScore(enemy.getPointsValue());          // RF-03
                    world.spawnExplosion(box.x + box.width / 2.0f, box.y + box.height / 2.0f);
                    assets.playSound(SND_EXPLOSION);
                }
                else
                {
                    assets.playSound(SND_HIT);                        // la nave resistio
                }
                break;   // una bala solo golpea a una nave
            }
        }
        else
        {
            // b) Bala enemiga contra el jugador: resta 1 vida
            if (CheckCollisionRecs(bullet.getGlobalBounds(), playerBox) &&
                player.takeDamage(bullet.getDamage()))
            {
                bullet.setActive(false);
                assets.playSound(SND_HIT);
            }
        }
    }
}

// c) Enemigo contra el jugador: la nave enemiga explota y el jugador pierde vida
void CollisionSystem::enemiesVsPlayer(GameWorld& world) const
{
    Player& player = world.player;
    const Rectangle playerBox = player.getGlobalBounds();

    for (int e = 0; e < world.enemies.size(); e++)
    {
        Enemy& enemy = world.enemies.get(e);
        if (!enemy.isEntityActive()) continue;
        if (!CheckCollisionRecs(enemy.getGlobalBounds(), playerBox)) continue;

        if (player.takeDamage(1))
        {
            const Rectangle box = enemy.getGlobalBounds();
            enemy.setActive(false);
            world.spawnExplosion(box.x + box.width / 2.0f, box.y + box.height / 2.0f);
            AssetManager::getInstance().playSound(SND_EXPLOSION);
        }
    }
}

// d) Pocion contra el jugador: recupera un porcentaje de vida (RF-11)
int CollisionSystem::potionsVsPlayer(GameWorld& world) const
{
    Player& player = world.player;
    const Rectangle playerBox = player.getGlobalBounds();
    int collected = 0;

    for (int i = 0; i < world.potions.size(); i++)
    {
        Potion& potion = world.potions.get(i);
        if (!potion.isEntityActive()) continue;
        if (!CheckCollisionRecs(potion.getGlobalBounds(), playerBox)) continue;

        // 20 % de 5 vidas = 1 vida (como minimo siempre cura 1)
        int heal = static_cast<int>(static_cast<float>(player.getMaxHealth()) * Config::POTION_HEAL_PERCENT + 0.5f);
        if (heal < 1) heal = 1;

        player.heal(heal);
        potion.setActive(false);
        AssetManager::getInstance().playSound(SND_POTION);
        collected++;
    }
    return collected;
}
