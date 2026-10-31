#pragma once
#include "Enemy.h"
#include "ObjectPool.h"

// Parametros de dificultad de una oleada (se calculan con Config.h)
struct WaveConfig
{
    int   enemyCount;       // naves que forman la oleada
    float enemySpeed;       // velocidad vertical base
    float speedMultiplier;  // factor que aumenta las velocidades
    float spawnInterval;    // segundos entre la aparicion de cada nave
    bool  enemiesCanFire;   // desde cierta oleada las naves disparan
};

// =============================================================================
//  WaveManager: generacion de oleadas (RF-02) y progresion (RF-09, RNF-11).
//  Las naves de cada oleada aparecen una por una con un temporizador.
//  Cuando se destruyen todas, Game inicia la siguiente con mas dificultad.
// =============================================================================
class WaveManager
{
public:
    WaveManager();

    /// Vuelve a la oleada 0 (antes de empezar una partida).
    void reset();

    /// Prepara la oleada indicada y muestra el aviso "OLEADA N".
    void startWave(int waveNumber);

    /// Cuenta el tiempo del aviso y luego crea una nave cada spawnInterval.
    void update(float deltaTime, ObjectPool<Enemy>& enemies);

    /// True cuando ya aparecieron todas las naves y ninguna sigue viva.
    bool isWaveCleared(const ObjectPool<Enemy>& enemies) const;

    int   getCurrentWave() const;
    bool  isShowingBanner() const;
    float getBannerTimer() const;
    int   getRemainingEnemies(const ObjectPool<Enemy>& enemies) const;
    WaveConfig getConfig() const;

    /// Formula de dificultad: devuelve los parametros de la oleada n.
    static WaveConfig computeConfig(int waveNumber);

private:
    /// Toma una nave libre del pool y le asigna un tipo al azar.
    void spawnEnemy(ObjectPool<Enemy>& enemies);

    /// Tipo al azar entre los desbloqueados: oleada 1 = rojo,
    /// oleada 2 = rojo o verde, oleada 3+ = rojo, verde o morado.
    EnemyType randomEnemyType() const;

    int        currentWave;
    int        enemiesToSpawn;
    float      spawnTimer;
    float      bannerTimer;
    WaveConfig config;
};
