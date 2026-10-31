#pragma once
#include "raylib.h"
#include "Config.h"

// =============================================================================
//  Starfield: fondo de estrellas con efecto de profundidad (parallax).
//  Las estrellas lejanas son pequenas y lentas; las cercanas grandes y rapidas.
//  Usa un arreglo fijo: no reserva memoria durante la partida.
// =============================================================================
class Starfield
{
public:
    Starfield();

    /// Reparte las estrellas al azar por toda la pantalla.
    void init();

    /// Desplaza las estrellas hacia abajo; al salir reaparecen arriba.
    void update(float deltaTime);

    void render() const;

private:
    struct Star
    {
        float x;
        float y;
        float speed;
        float radius;
        unsigned char brightness;
    };

    Star stars[Config::STAR_COUNT];
};
