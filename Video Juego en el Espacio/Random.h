#pragma once
#include "raylib.h"

// Numero decimal aleatorio entre minValue y maxValue (usa el generador de raylib).
inline float RandomFloat(float minValue, float maxValue)
{
    const float t = static_cast<float>(GetRandomValue(0, 10000)) / 10000.0f;
    return minValue + (maxValue - minValue) * t;
}
