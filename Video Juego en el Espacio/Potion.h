#pragma once
#include "Entity.h"
#include "MovementStrategy.h"
#include <memory>

// =============================================================================
//  Potion: pocion de vida (RF-11).
//  Cae desde arriba reutilizando la estrategia StraightMove, igual que las
//  naves enemigas. Si el jugador la toca recupera un porcentaje de su vida.
//  Tambien vive en un Object Pool.
// =============================================================================
class Potion : public Entity
{
public:
    /// Crea la pocion inactiva con su estrategia de caida (una sola vez).
    Potion();

    /// Activa la pocion en la parte superior, en la coordenada X indicada.
    void spawn(float startX, float fallSpeed);

    /// Cae con StraightMove y se libera al salir por abajo.
    void update(float deltaTime) override;

    /// Dibuja la pocion con un brillo pulsante para que se note.
    void render() const override;

private:
    std::unique_ptr<MovementStrategy> fallStrategy;
    float glowTimer;
};
