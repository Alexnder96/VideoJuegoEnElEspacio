#pragma once

// =============================================================================
//  Patron Strategy: MovementStrategy
//  Interfaz polimorfica que encapsula el algoritmo de movimiento de una nave.
//  Cada enemigo guarda un puntero (std::unique_ptr) a una estrategia concreta
//  y la llama en su update(). Cambiar la trayectoria es cambiar la estrategia,
//  sin tocar la logica de Enemy ni del juego (RF-05, RNF-07).
// =============================================================================
class MovementStrategy
{
public:
    virtual ~MovementStrategy() = default;

    /// Modifica la posicion (x, y) segun el patron de movimiento.
    /// speedX = velocidad horizontal base, speedY = velocidad de descenso.
    virtual void move(float& x, float& y, float speedX, float speedY, float deltaTime) = 0;
};

// -----------------------------------------------------------------------------
//  StraightMove: descenso vertical en linea recta.
//  Tambien lo usan las pociones de vida para caer como las naves (RF-11).
// -----------------------------------------------------------------------------
class StraightMove : public MovementStrategy
{
public:
    void move(float& x, float& y, float speedX, float speedY, float deltaTime) override;
};

// -----------------------------------------------------------------------------
//  SinusoidalMove: desciende mientras oscila a los lados siguiendo una onda
//  seno:  x = initialX + amplitude * sin(2*PI * frequency * t)
// -----------------------------------------------------------------------------
class SinusoidalMove : public MovementStrategy
{
public:
    explicit SinusoidalMove(float oscFrequency = 0.8f, float oscAmplitude = 60.0f, float startPhase = 0.0f);
    void move(float& x, float& y, float speedX, float speedY, float deltaTime) override;

private:
    float frequency;    // oscilaciones por segundo
    float amplitude;    // desplazamiento maximo a cada lado (px)
    float phase;        // desfase inicial para que no oscilen todas igual
    float initialX;     // centro de la oscilacion (se toma en el primer move)
    float elapsed;      // tiempo acumulado
    bool  initialized;
};

// -----------------------------------------------------------------------------
//  ZigZagMove: desciende moviendose en diagonal y cambia de direccion
//  horizontal cada switchInterval segundos (o al tocar un borde).
// -----------------------------------------------------------------------------
class ZigZagMove : public MovementStrategy
{
public:
    explicit ZigZagMove(float interval = 0.9f, bool startRight = true);
    void move(float& x, float& y, float speedX, float speedY, float deltaTime) override;

private:
    float directionTimer;   // tiempo desde el ultimo cambio
    float switchInterval;   // cada cuanto cambia de lado
    bool  moveRight;        // direccion horizontal actual
};
