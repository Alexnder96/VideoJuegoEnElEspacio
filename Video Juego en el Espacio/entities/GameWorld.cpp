#include "entities/GameWorld.h"
#include "common/Config.h"

GameWorld::GameWorld()
    : player(0.0f, 0.0f),
      enemies(Config::MAX_ENEMIES),
      projectiles(Config::MAX_PROJECTILES),
      potions(Config::MAX_POTIONS),
      explosions(Config::MAX_EXPLOSIONS)
{
}

void GameWorld::updateEntities(float deltaTime)
{
    for (int i = 0; i < enemies.size(); i++)
        enemies.get(i).update(deltaTime);
    for (int i = 0; i < projectiles.size(); i++)
        projectiles.get(i).update(deltaTime);
    for (int i = 0; i < potions.size(); i++)
        potions.get(i).update(deltaTime);
    updateExplosions(deltaTime);
}

void GameWorld::updateExplosions(float deltaTime)
{
    for (int i = 0; i < explosions.size(); i++)
        explosions.get(i).update(deltaTime);
}

void GameWorld::render() const
{
    for (int i = 0; i < potions.size(); i++)     potions.get(i).render();
    for (int i = 0; i < enemies.size(); i++)     enemies.get(i).render();
    for (int i = 0; i < projectiles.size(); i++) projectiles.get(i).render();
    player.render();
    for (int i = 0; i < explosions.size(); i++)  explosions.get(i).render();
}

void GameWorld::spawnExplosion(float centerX, float centerY)
{
    Explosion* e = explosions.acquire();
    if (e != nullptr) e->spawn(centerX, centerY);
}

void GameWorld::releaseAll()
{
    enemies.releaseAll();
    projectiles.releaseAll();
    potions.releaseAll();
    explosions.releaseAll();
}
