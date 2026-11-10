#pragma once
#include "entities/GameWorld.h"

// =============================================================================
//  Modulo de POCIONES: PotionSystem (RF-11)
//  Decide cuando cae una pocion de vida:
//    - Vida completa: no caen, y las que estaban cayendo desaparecen.
//    - Vida <= 60 %: cae una pocion cada 5 a 9 segundos.
//    - Entre 60 % y 100 %: no aparecen nuevas.
// =============================================================================
class PotionSystem
{
public:
    PotionSystem();

    /// Reinicia el temporizador (al empezar una partida).
    void reset();

    void update(float deltaTime, GameWorld& world);

private:
    float potionTimer;   // segundos que faltan para la siguiente pocion
};
