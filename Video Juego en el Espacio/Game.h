#pragma once
#include "raylib.h"
#include "Player.h"
#include "Enemy.h"
#include "Projectile.h"
#include "Potion.h"
#include "Explosion.h"
#include "ObjectPool.h"
#include "WaveManager.h"
#include "HUDManager.h"
#include "AssetManager.h"
#include "Starfield.h"

// Estados del juego (RF-10)
enum class GameState
{
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER
};

// =============================================================================
//  Game: administra el Game Loop, los controles, la actualizacion, el dibujo,
//  las colisiones y los estados del juego.
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
    // Controles leidos en processEvents() y usados en update()
    struct InputState
    {
        float moveDirection;   // -1 izquierda ... 0 quieto ... 1 derecha
        bool  shootPressed;
    };

    // --- Controles ---
    InputState readGameplayInput() const;
    bool isConfirmPressed() const;   // ENTER o START
    bool isPausePressed() const;     // P o START

    // --- Logica de juego ---
    void updatePlaying(float deltaTime);
    void updatePotions(float deltaTime);
    void updateEnemyAI();
    void firePlayerProjectile();
    void fireEnemyProjectile(const Enemy& enemy);
    void spawnExplosion(float centerX, float centerY);
    void releaseAllEntities();
    Vector2 playerStartPosition() const;

    GameState currentState;
    Player    player;
    ObjectPool<Enemy>      enemies;
    ObjectPool<Projectile> projectilePool;
    ObjectPool<Potion>     potionPool;
    ObjectPool<Explosion>  explosionPool;
    WaveManager   waveManager;
    HUDManager    hudManager;
    AssetManager& assetManager;
    Starfield     starfield;
    InputState    input;

    int   currentScore;
    int   currentWave;
    int   highScore;
    bool  newRecord;
    float potionTimer;
    bool  isRunning;
};
