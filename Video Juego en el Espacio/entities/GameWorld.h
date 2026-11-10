#pragma once
#include "entities/Player.h"
#include "entities/Enemy.h"
#include "entities/Projectile.h"
#include "entities/Potion.h"
#include "entities/Explosion.h"
#include "common/ObjectPool.h"

// =============================================================================
//  GameWorld: el "mundo" de la partida.
//  Agrupa al jugador y los Object Pools de naves, balas, pociones y
//  explosiones. Es el dato que comparten los modulos del juego: cada sistema
//  (oleadas, combate, pociones, colisiones) recibe el mundo y hace su parte.
//  Los pools reservan su memoria aqui, una sola vez, antes de jugar (RNF-01).
// =============================================================================
struct GameWorld
{
    GameWorld();

    /// Actualiza naves, balas, pociones y explosiones (el jugador va aparte).
    void updateEntities(float deltaTime);

    /// Solo las explosiones (en Game Over siguen animandose).
    void updateExplosions(float deltaTime);

    /// Dibuja todas las entidades. Cada render() ignora los objetos inactivos.
    void render() const;

    /// Toma una explosion libre del pool y la inicia en ese punto.
    void spawnExplosion(float centerX, float centerY);

    /// Devuelve todas las entidades a sus pools (al reiniciar la partida).
    void releaseAll();

    Player                 player;
    ObjectPool<Enemy>      enemies;
    ObjectPool<Projectile> projectiles;
    ObjectPool<Potion>     potions;
    ObjectPool<Explosion>  explosions;
};
