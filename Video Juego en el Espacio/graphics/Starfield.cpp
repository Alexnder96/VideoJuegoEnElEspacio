#include "graphics/Starfield.h"
#include "common/Random.h"

Starfield::Starfield()
    : stars()
{
}

void Starfield::init()
{
    for (Star& s : stars)
    {
        const int layer = GetRandomValue(0, 2);   // 0 = lejos, 2 = cerca
        s.x = RandomFloat(0.0f, static_cast<float>(Config::SCREEN_WIDTH));
        s.y = RandomFloat(0.0f, static_cast<float>(Config::SCREEN_HEIGHT));
        s.speed = 20.0f + 45.0f * static_cast<float>(layer) + RandomFloat(0.0f, 15.0f);
        s.radius = 0.8f + 0.6f * static_cast<float>(layer);
        s.brightness = static_cast<unsigned char>(110 + 50 * layer);
    }
}

void Starfield::update(float deltaTime)
{
    for (Star& s : stars)
    {
        s.y += s.speed * deltaTime;
        if (s.y > static_cast<float>(Config::SCREEN_HEIGHT))
        {
            s.y = 0.0f;
            s.x = RandomFloat(0.0f, static_cast<float>(Config::SCREEN_WIDTH));
        }
    }
}

void Starfield::render() const
{
    for (const Star& s : stars)
    {
        const Color c{ s.brightness, s.brightness, 255, 255 };
        DrawCircleV({ s.x, s.y }, s.radius, c);
    }
}
