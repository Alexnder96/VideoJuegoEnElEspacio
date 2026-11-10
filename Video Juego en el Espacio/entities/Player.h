#pragma once
#include "entities/Entity.h"

// =============================================================================
//  Player: nave del jugador.
//  - Se mueve solo en el eje X y nunca sale de la pantalla (RF-01).
//  - Usa ACELERACION: al presionar una direccion la velocidad crece poco a
//    poco hasta un maximo, y al soltar frena por friccion (inercia).
//  - Dispara hacia arriba con tiempo de recarga (RF-04).
//  - Administra su vida (RF-07).
//  - Su sprite sheet anima el fuego del motor.
// =============================================================================
class Player : public Entity
{
public:
    Player(float startX, float startY);

    /// Actualiza la animacion, la recarga del disparo y la invulnerabilidad.
    void update(float deltaTime) override;

    /// Dibuja la nave (parpadea mientras es invulnerable).
    void render() const override;

    /// Aplica aceleracion segun la direccion (-1 izquierda, 0 nada, 1 derecha).
    /// Con un mando analogico la direccion puede ser un valor intermedio (ej. 0.5).
    void accelerate(float direction, float deltaTime);

    /// Desplaza la nave dx pixeles en X (dy se ignora: solo se mueve en
    /// horizontal) y la mantiene dentro de los bordes.
    void move(float dx, float dy);

    bool    canShoot() const;       // true si ya paso el tiempo de recarga
    void    shoot();                // reinicia la recarga (la bala la crea CombatSystem)
    Vector2 getGunPosition() const; // punta de la nave

    /// Resta vida. Durante la invulnerabilidad no recibe dano.
    /// Devuelve true si el dano se aplico.
    bool  takeDamage(int amount);
    void  heal(int amount);         // suma vida sin pasar del maximo

    int   getHealth() const;
    int   getMaxHealth() const;
    float getHealthPercent() const; // 0.0 a 1.0
    bool  isAlive() const;
    bool  isInvulnerable() const;
    float getVelocity() const;

    void  addScore(int points);
    int   getScore() const;

    /// Restablece vida, puntaje, velocidad y posicion.
    void  reset(float startX, float startY);

private:
    int   health;
    int   maxHealth;
    int   score;
    float velocityX;       // velocidad actual en X (cambia con la aceleracion)
    float shootCooldown;   // tiempo que falta para poder disparar
    float timer;           // tiempo que falta de invulnerabilidad
};
