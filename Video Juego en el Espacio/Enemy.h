#pragma once
#include "Entity.h"
#include "MovementStrategy.h"
#include <memory>

// Tipos de nave enemiga (cada uno usa una estrategia de movimiento distinta)
enum class EnemyType
{
    STRAIGHT,     // roja    - movimiento recto        - 100 pts - 1 impacto
    SINUSOIDAL,   // verde   - movimiento en onda      - 150 pts - 1 impacto
    ZIGZAG        // morada  - movimiento en zig-zag   - 200 pts - 2 impactos
};

// =============================================================================
//  Enemy: nave enemiga.
//  No sabe COMO moverse: se lo pregunta a su MovementStrategy (patron
//  Strategy). La estrategia se guarda en un std::unique_ptr, un puntero
//  inteligente que borra solo el objeto anterior al cambiarlo (sin fugas).
//
//  RNF-07: para crear un tipo nuevo se agrega un valor a EnemyType, una clase
//  derivada de MovementStrategy y un 'case' en Enemy::spawn(). El WaveManager
//  no se modifica.
// =============================================================================
class Enemy : public Entity
{
public:
    /// Constructor por defecto (lo usa el Object Pool). Empieza inactivo.
    Enemy();

    /// Constructor del diagrama UML: nave en (x, y) con una estrategia.
    Enemy(float startX, float startY, std::unique_ptr<MovementStrategy> strategy);

    /// Reactiva una nave del pool y la configura segun su tipo.
    void spawn(float startX, float startY, EnemyType type,
               float waveSpeed, float speedMultiplier, bool fireEnabled);

    /// Cambia el algoritmo de movimiento en tiempo de ejecucion.
    void setStrategy(std::unique_ptr<MovementStrategy> strategy);

    /// Mueve la nave con su estrategia, anima el sprite y controla la recarga.
    void update(float deltaTime) override;

    /// Dibuja la nave; se aclara un instante al recibir un impacto.
    void render() const override;

    /// IA de disparo (percepcion -> decision):
    /// si el jugador esta justo debajo y el arma esta lista, decide disparar.
    bool decideToFire(float playerCenterX);

    /// Resta vida a la nave. Devuelve true si fue destruida.
    bool takeDamage(int amount);

    int       getPointsValue() const;
    EnemyType getEnemyType() const;
    Vector2   getGunPosition() const;

private:
    std::unique_ptr<MovementStrategy> movementStrategy;
    int       pointsValue;
    EnemyType enemyType;
    int       health;
    bool      canFire;       // en oleadas iniciales no disparan
    float     fireCooldown;  // tiempo que falta para poder disparar
    float     hitFlash;      // tiempo del destello al recibir dano
};
