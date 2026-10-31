#include "Explosion.h"
#include "AssetManager.h"
#include "Config.h"

Explosion::Explosion()
    : Entity(0.0f, 0.0f, 0.0f, 0.0f)
{
}

void Explosion::spawn(float centerX, float centerY)
{
    // loop = false: la animacion se reproduce una sola vez
    setSpriteSheet(AssetManager::getInstance().getTexture(TEX_EXPLOSION),
                   Config::EXPLOSION_FRAMES, Config::EXPLOSION_ANIM_FPS, false);
    x = centerX - width / 2.0f;
    y = centerY - height / 2.0f;
    isActive = true;
}

void Explosion::update(float deltaTime)
{
    if (!isActive) return;

    animation.update(deltaTime);
    if (animation.isFinished())
        isActive = false;
}
