#include "systems/WaveManager.h"
#include "resources/AssetManager.h"
#include "common/Config.h"
#include "common/Random.h"

WaveManager::WaveManager()
    : currentWave(0), enemiesToSpawn(0), spawnTimer(0.0f), bannerTimer(0.0f),
      config(computeConfig(1))
{
}

void WaveManager::reset()
{
    currentWave = 0;
    enemiesToSpawn = 0;
    spawnTimer = 0.0f;
    bannerTimer = 0.0f;
    config = computeConfig(1);
}

// RNF-11: formula de progresion. La dificultad sube hasta la oleada
// MAX_DIFFICULTY_WAVE y desde ahi se mantiene en su valor maximo.
WaveConfig WaveManager::computeConfig(int waveNumber)
{
    int level = waveNumber;
    if (level < 1) level = 1;
    if (level > Config::MAX_DIFFICULTY_WAVE) level = Config::MAX_DIFFICULTY_WAVE;
    const float step = static_cast<float>(level - 1);

    WaveConfig cfg;
    cfg.enemyCount = Config::BASE_ENEMIES + Config::ENEMIES_PER_WAVE * (level - 1);
    if (cfg.enemyCount > Config::MAX_ENEMIES_PER_WAVE) cfg.enemyCount = Config::MAX_ENEMIES_PER_WAVE;

    cfg.speedMultiplier = 1.0f + Config::SPEED_INCREMENT * step;
    cfg.enemySpeed = Config::BASE_ENEMY_SPEED * cfg.speedMultiplier;

    cfg.spawnInterval = Config::BASE_SPAWN_INTERVAL - Config::SPAWN_INTERVAL_STEP * step;
    if (cfg.spawnInterval < Config::MIN_SPAWN_INTERVAL) cfg.spawnInterval = Config::MIN_SPAWN_INTERVAL;

    cfg.enemiesCanFire = waveNumber >= Config::ENEMY_FIRE_START_WAVE;
    return cfg;
}

void WaveManager::startWave(int waveNumber)
{
    currentWave = waveNumber;
    config = computeConfig(waveNumber);
    enemiesToSpawn = config.enemyCount;
    spawnTimer = 0.0f;
    bannerTimer = Config::WAVE_BANNER_TIME;
    AssetManager::getInstance().playSound(SND_WAVE);
}

void WaveManager::update(float deltaTime, ObjectPool<Enemy>& enemies)
{
    // Mientras se muestra "OLEADA N" no aparecen naves.
    if (bannerTimer > 0.0f)
    {
        bannerTimer -= deltaTime;
        return;
    }

    if (enemiesToSpawn <= 0) return;

    // Temporizador de aparicion (RF-02)
    spawnTimer -= deltaTime;
    if (spawnTimer <= 0.0f)
    {
        spawnEnemy(enemies);
        spawnTimer = config.spawnInterval;
    }
}

EnemyType WaveManager::randomEnemyType() const
{
    int unlockedTypes = 1;
    if (currentWave >= 2) unlockedTypes = 2;
    if (currentWave >= 3) unlockedTypes = 3;

    const int choice = GetRandomValue(0, unlockedTypes - 1);
    if (choice == 1) return EnemyType::SINUSOIDAL;
    if (choice == 2) return EnemyType::ZIGZAG;
    return EnemyType::STRAIGHT;
}

void WaveManager::spawnEnemy(ObjectPool<Enemy>& enemies)
{
    Enemy* enemy = enemies.acquire();
    if (enemy == nullptr) return;   // pool lleno: se intenta en el siguiente intervalo

    // Margen lateral para que la onda sinusoidal no choque con los bordes.
    const float margin = 90.0f;
    const float startX = RandomFloat(margin, static_cast<float>(Config::SCREEN_WIDTH) - 40.0f - margin);
    const float startY = static_cast<float>(Config::HUD_HEIGHT) - 40.0f;   // entra desde arriba

    enemy->spawn(startX, startY, randomEnemyType(),
                 config.enemySpeed, config.speedMultiplier, config.enemiesCanFire);
    enemiesToSpawn--;
}

bool WaveManager::isWaveCleared(const ObjectPool<Enemy>& enemies) const
{
    return currentWave > 0 && bannerTimer <= 0.0f &&
           enemiesToSpawn <= 0 && enemies.activeCount() == 0;
}

int        WaveManager::getCurrentWave() const  { return currentWave; }
bool       WaveManager::isShowingBanner() const { return bannerTimer > 0.0f; }
float      WaveManager::getBannerTimer() const  { return bannerTimer; }
WaveConfig WaveManager::getConfig() const       { return config; }

int WaveManager::getRemainingEnemies(const ObjectPool<Enemy>& enemies) const
{
    return enemiesToSpawn + enemies.activeCount();
}
