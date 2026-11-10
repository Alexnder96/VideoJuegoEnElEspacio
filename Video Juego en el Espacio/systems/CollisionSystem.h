#pragma once
#include "entities/GameWorld.h"

// Lo que ocurrio en las colisiones de un fotograma (para avisar en el HUD)
struct CollisionReport
{
    int potionsCollected;   // pociones que tomo el jugador
};

// =============================================================================
//  Modulo de COLISIONES: CollisionSystem (RF-06)
//  Motor de colisiones con Bounding Boxes (CheckCollisionRecs). Revisa:
//    a) bala del jugador contra nave enemiga
//    b) bala enemiga contra el jugador
//    c) nave enemiga contra el jugador
//    d) pocion contra el jugador
//  No dibuja nada: devuelve un reporte y Game decide que mostrar.
// =============================================================================
class CollisionSystem
{
public:
    CollisionReport resolve(GameWorld& world) const;

private:
    void bulletsVsShips(GameWorld& world) const;
    void enemiesVsPlayer(GameWorld& world) const;
    int  potionsVsPlayer(GameWorld& world) const;
};
