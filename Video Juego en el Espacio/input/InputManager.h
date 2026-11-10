#pragma once

// Controles de la partida leidos en un fotograma
struct GameplayInput
{
    float moveDirection;   // -1 izquierda ... 0 quieto ... 1 derecha
    bool  shootPressed;
};

// =============================================================================
//  Modulo de ENTRADA: InputManager
//  Es el unico modulo que consulta el teclado y el mando. El resto del juego
//  solo pregunta "que quiere hacer el jugador?", sin saber con que tecla o
//  boton se pidio. Para cambiar un control basta con modificar este modulo.
// =============================================================================
class InputManager
{
public:
    /// Movimiento y disparo: junta teclado y mando en un solo resultado.
    GameplayInput readGameplay() const;

    bool isConfirmPressed() const;    // ENTER o START
    bool isPausePressed() const;      // P o START
    bool isRestartPressed() const;    // R, ENTER o START
    bool isMenuPressed() const;       // M
    bool isGamepadConnected() const;
};
