#include "systems/PotionSystem.h"
#include "common/Config.h"
#include "common/Random.h"

PotionSystem::PotionSystem()
    : potionTimer(0.0f)
{
}

void PotionSystem::reset()
{
    potionTimer = Config::POTION_MIN_INTERVAL * 0.5f;
}

void PotionSystem::update(float deltaTime, GameWorld& world)
{
    const float healthPct = world.player.getHealthPercent();

    if (healthPct >= 1.0f)
    {
        world.potions.releaseAll();
        potionTimer = Config::POTION_MIN_INTERVAL * 0.5f;
        return;
    }

    if (healthPct > Config::POTION_HEALTH_THRESHOLD) return;

    potionTimer -= deltaTime;
    if (potionTimer > 0.0f) return;

    Potion* potion = world.potions.acquire();
    if (potion != nullptr)
    {
        const float x = RandomFloat(40.0f, static_cast<float>(Config::SCREEN_WIDTH) - 64.0f);
        potion->spawn(x, Config::POTION_FALL_SPEED);
    }
    potionTimer = RandomFloat(Config::POTION_MIN_INTERVAL, Config::POTION_MAX_INTERVAL);
}
