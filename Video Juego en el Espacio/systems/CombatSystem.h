#pragma once
#include "entities/GameWorld.h"

// =============================================================================
//  Modulo de COMBATE: CombatSystem
//  Crea los disparos del jugador (RF-04) y aplica la IA de disparo de las
//  naves enemigas. Las balas salen del Object Pool: si no queda ninguna
//  libre no se dispara (nunca se reserva memoria nueva).
// =============================================================================
class CombatSystem
{
public:
    /// Dispara una bala del jugador. Devuelve true si la bala salio.
    bool firePlayerProjectile(GameWorld& world) const;

    /// Cada nave "piensa" si debe disparar segun donde esta el jugador.
    void updateEnemyAI(GameWorld& world) const;

private:
    void fireEnemyProjectile(GameWorld& world, const Enemy& enemy) const;
};
