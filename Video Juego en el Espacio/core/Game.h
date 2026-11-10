#pragma once
#include "raylib.h"
#include "core/GameState.h"
#include "entities/GameWorld.h"
#include "input/InputManager.h"
#include "systems/WaveManager.h"
#include "systems/CombatSystem.h"
#include "systems/PotionSystem.h"
#include "systems/CollisionSystem.h"
#include "ui/HUDManager.h"
#include "graphics/Starfield.h"
#include "resources/AssetManager.h"

// =============================================================================
//  Modulo NUCLEO: Game
//  Administra el Game Loop y los estados del juego, y COORDINA a los demas
//  modulos. Game no resuelve los detalles: se los pide al modulo responsable.
//
//     input/      InputManager      -> lee teclado y mando
//     entities/   GameWorld         -> jugador y Object Pools de entidades
//     systems/    WaveManager       -> oleadas
//                 CombatSystem      -> disparos e IA enemiga
//                 PotionSystem      -> pociones de vida
//                 CollisionSystem   -> colisiones
//     ui/         HUDManager        -> HUD, menu, pausa y game over
//     graphics/   Starfield         -> fondo de estrellas
//     resources/  AssetManager      -> imagenes y sonidos
//
//  Cada fotograma (ciclo de juego):
//     processEvents()  -> lee teclado y mando
//     update(dt)       -> mueve entidades, colisiones, oleadas
//     render()         -> dibuja todo en pantalla
// =============================================================================
class Game
{
public:
    /// Crea la ventana, el audio y precarga los recursos.
    Game();

    /// Libera los recursos y cierra la ventana.
    ~Game();

    /// Bucle principal: se repite hasta cerrar la ventana.
    void run();

    /// Lee los controles y resuelve los cambios de estado.
    void processEvents();

    /// Actualiza la logica segun el estado actual.
    void update(float deltaTime);

    /// Dibuja la escena y el HUD.
    void render();

    /// Cambia de estado y aplica sus efectos (record, limpieza).
    void changeState(GameState newState);

    /// Motor de colisiones con Bounding Boxes (RF-06).
    void checkCollisions();

    /// Inicia la siguiente oleada con mas dificultad (RF-09).
    void spawnWave();

    /// Reinicia la partida: limpia entidades y contadores (RF-10).
    void restart();

private:
    void updatePlaying(float deltaTime);
    Vector2 playerStartPosition() const;

    GameState currentState;

    // --- Modulos ---
    GameWorld       world;             // entidades de la partida
    InputManager    inputManager;      // controles
    WaveManager     waveManager;       // oleadas
    CombatSystem    combatSystem;      // disparos e IA
    PotionSystem    potionSystem;      // pociones de vida
    CollisionSystem collisionSystem;   // colisiones
    HUDManager      hudManager;        // interfaz
    Starfield       starfield;         // fondo
    AssetManager&   assetManager;      // recursos (Singleton)

    GameplayInput input;   // controles leidos en processEvents(), usados en update()

    int   currentScore;
    int   currentWave;
    int   highScore;
    bool  newRecord;
    bool  isRunning;
};
