#pragma once
#include "entities/Entity.h"

// =============================================================================
//  Explosion: efecto al destruir una nave.
//  Reproduce UNA vez el sprite sheet de 6 cuadros (sin repetir) y al terminar
//  se desactiva sola. Vive en un Object Pool y no participa en colisiones.
// =============================================================================
class Explosion : public Entity
{
public:
    Explosion();

    /// Inicia la explosion centrada en (centerX, centerY).
    void spawn(float centerX, float centerY);

    /// Avanza la animacion; cuando llega al ultimo cuadro se desactiva.
    void update(float deltaTime) override;
};
