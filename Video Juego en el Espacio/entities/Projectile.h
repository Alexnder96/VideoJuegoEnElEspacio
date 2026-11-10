#pragma once
#include "entities/Entity.h"

// =============================================================================
//  Projectile: bala del jugador o de un enemigo.
//  Vive dentro de un ObjectPool: nunca se crea ni destruye durante la partida,
//  solo se reutiliza con reset() y se libera con setActive(false).
// =============================================================================
class Projectile : public Entity
{
public:
    /// Constructor por defecto (requerido por el Object Pool). Inicia inactivo.
    Projectile();

    /// Mueve la bala en Y y la devuelve al pool al salir de la pantalla (RNF-01).
    void update(float deltaTime) override;

    /// Reactiva la bala. (x, y) es el centro de salida; speedY negativa = sube.
    void reset(float startX, float startY, float newSpeedY, bool isPlayer);

    int  getDamage() const;
    bool isFromPlayer() const;

private:
    int  damage;
    bool isPlayerBullet;
};
