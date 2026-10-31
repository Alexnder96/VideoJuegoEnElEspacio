#include "Game.h"
#include "Config.h"
#include "Random.h"
#include <cmath>

// -----------------------------------------------------------------------------
//  Construccion y destruccion
// -----------------------------------------------------------------------------

// Los Object Pools reservan su memoria aqui, una sola vez, antes de jugar.
Game::Game()
    : currentState(GameState::MENU),
      player(0.0f, 0.0f),
      enemies(Config::MAX_ENEMIES),
      projectilePool(Config::MAX_PROJECTILES),
      potionPool(Config::MAX_POTIONS),
      explosionPool(Config::MAX_EXPLOSIONS),
      waveManager(),
      hudManager(),
      assetManager(AssetManager::getInstance()),
      starfield(),
      input{ 0.0f, false },
      currentScore(0),
      currentWave(0),
      highScore(0),
      newRecord(false),
      potionTimer(0.0f),
      isRunning(true)
{
    InitWindow(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, Config::WINDOW_TITLE);
    InitAudioDevice();
    SetTargetFPS(Config::TARGET_FPS);   // velocidad constante: 60 cuadros por segundo

    // RNF-09: todos los recursos se cargan aqui, nunca dentro del bucle.
    assetManager.loadAll();
    player.setSpriteSheet(assetManager.getTexture(TEX_PLAYER),
                          Config::PLAYER_FRAMES, Config::PLAYER_ANIM_FPS, true);

    const Vector2 start = playerStartPosition();
    player.setPosition(start.x, start.y);
    starfield.init();
}

Game::~Game()
{
    assetManager.unloadAll();
    if (IsAudioDeviceReady()) CloseAudioDevice();
    CloseWindow();
}

// -----------------------------------------------------------------------------
//  Ciclo de juego (Game Loop)
// -----------------------------------------------------------------------------
void Game::run()
{
    while (isRunning && !WindowShouldClose())
    {
        // Delta time = segundos que duro el ultimo frame. Todo movimiento se
        // multiplica por el, asi el juego va a la misma velocidad en cualquier PC.
        float deltaTime = GetFrameTime();
        if (deltaTime > Config::MAX_DELTA) deltaTime = Config::MAX_DELTA;

        processEvents();
        update(deltaTime);
        render();
    }
}

// -----------------------------------------------------------------------------
//  Manejo de controles (teclado y mando)
// -----------------------------------------------------------------------------

// Junta teclado y mando en un solo resultado. Con el stick analogico la
// direccion puede ser intermedia (ej. 0.4): la nave acelera mas suave.
Game::InputState Game::readGameplayInput() const
{
    InputState result{ 0.0f, false };

    // Teclado
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  result.moveDirection -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) result.moveDirection += 1.0f;
    if (IsKeyPressed(KEY_SPACE)) result.shootPressed = true;   // RF-04: una bala por pulsacion

    // Mando (gamepad)
    const int pad = Config::GAMEPAD_ID;
    if (IsGamepadAvailable(pad))
    {
        const float stick = GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_X);
        if (std::fabs(stick) > Config::GAMEPAD_DEADZONE) result.moveDirection += stick;

        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_LEFT))  result.moveDirection -= 1.0f;
        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) result.moveDirection += 1.0f;
        if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) result.shootPressed = true;
    }

    // Si se usan teclado y mando a la vez, la direccion no pasa de -1 a 1
    if (result.moveDirection > 1.0f)  result.moveDirection = 1.0f;
    if (result.moveDirection < -1.0f) result.moveDirection = -1.0f;
    return result;
}

bool Game::isConfirmPressed() const
{
    return IsKeyPressed(KEY_ENTER) ||
           (IsGamepadAvailable(Config::GAMEPAD_ID) &&
            IsGamepadButtonPressed(Config::GAMEPAD_ID, GAMEPAD_BUTTON_MIDDLE_RIGHT));
}

bool Game::isPausePressed() const
{
    return IsKeyPressed(KEY_P) ||
           (IsGamepadAvailable(Config::GAMEPAD_ID) &&
            IsGamepadButtonPressed(Config::GAMEPAD_ID, GAMEPAD_BUTTON_MIDDLE_RIGHT));
}

// Lee los controles segun el estado. Los cambios de estado se hacen aqui;
// el movimiento y el disparo se guardan en 'input' para usarlos en update().
void Game::processEvents()
{
    input = InputState{ 0.0f, false };

    switch (currentState)
    {
    case GameState::MENU:
        if (isConfirmPressed())
        {
            restart();
            changeState(GameState::PLAYING);
        }
        break;

    case GameState::PLAYING:
        if (isPausePressed())
            changeState(GameState::PAUSED);
        else
            input = readGameplayInput();
        break;

    case GameState::PAUSED:
        if (isPausePressed())
            changeState(GameState::PLAYING);
        else if (IsKeyPressed(KEY_M))
            changeState(GameState::MENU);
        break;

    case GameState::GAME_OVER:
        if (IsKeyPressed(KEY_R) || isConfirmPressed())
        {
            restart();
            changeState(GameState::PLAYING);
        }
        else if (IsKeyPressed(KEY_M))
        {
            changeState(GameState::MENU);
        }
        break;
    }
}

// -----------------------------------------------------------------------------
//  Actualizacion
// -----------------------------------------------------------------------------

// RF-10: en PAUSED no se actualiza nada, la logica queda congelada.
void Game::update(float deltaTime)
{
    switch (currentState)
    {
    case GameState::MENU:
        starfield.update(deltaTime);
        break;

    case GameState::PLAYING:
        starfield.update(deltaTime);
        hudManager.update(deltaTime);
        updatePlaying(deltaTime);
        break;

    case GameState::PAUSED:
        break;

    case GameState::GAME_OVER:
        // Solo el fondo y las explosiones siguen animandose
        starfield.update(deltaTime);
        for (int i = 0; i < explosionPool.size(); i++)
            explosionPool.get(i).update(deltaTime);
        break;
    }
}

// Todo lo que pasa en un fotograma de juego.
void Game::updatePlaying(float deltaTime)
{
    // 1) Jugador: aceleracion segun los controles (RF-01)
    player.accelerate(input.moveDirection, deltaTime);
    player.update(deltaTime);

    // 2) Disparo: primero se valida la recarga (caso de uso "Disparar")
    if (input.shootPressed && player.canShoot())
        firePlayerProjectile();

    // 3) Oleadas: aparicion de enemigos con temporizador (RF-02)
    waveManager.update(deltaTime, enemies);

    // 4) Actualizar todas las entidades activas de cada pool
    for (int i = 0; i < enemies.size(); i++)
        enemies.get(i).update(deltaTime);
    for (int i = 0; i < projectilePool.size(); i++)
        projectilePool.get(i).update(deltaTime);
    for (int i = 0; i < potionPool.size(); i++)
        potionPool.get(i).update(deltaTime);
    for (int i = 0; i < explosionPool.size(); i++)
        explosionPool.get(i).update(deltaTime);

    // 5) IA enemiga y pociones
    updateEnemyAI();
    updatePotions(deltaTime);

    // 6) Colisiones
    checkCollisions();
    currentScore = player.getScore();

    // 7) Fin de la partida (RF-07)
    if (!player.isAlive())
    {
        const Rectangle b = player.getGlobalBounds();
        spawnExplosion(b.x + b.width / 2.0f, b.y + b.height / 2.0f);
        assetManager.playSound(SND_EXPLOSION);
        player.setActive(false);
        changeState(GameState::GAME_OVER);
        return;
    }

    // 8) Cambio de oleada: se destruyeron todas las naves (RF-09)
    if (waveManager.isWaveCleared(enemies))
    {
        const int bonus = Config::WAVE_CLEAR_BONUS * currentWave;
        player.addScore(bonus);
        hudManager.showNotification(TextFormat("OLEADA SUPERADA  +%i", bonus), GOLD);
        spawnWave();
    }
}

// Cada nave "piensa" si debe disparar segun donde esta el jugador.
void Game::updateEnemyAI()
{
    const Rectangle p = player.getGlobalBounds();
    const float playerCenterX = p.x + p.width / 2.0f;

    for (int i = 0; i < enemies.size(); i++)
    {
        Enemy& enemy = enemies.get(i);
        if (enemy.isEntityActive() && enemy.decideToFire(playerCenterX))
            fireEnemyProjectile(enemy);
    }
}

// RF-11: pociones de vida.
//  - Vida completa: no caen, y las que estaban cayendo desaparecen.
//  - Vida <= 60 %: cae una pocion cada 5 a 9 segundos.
//  - Entre 60 % y 100 %: no aparecen nuevas.
void Game::updatePotions(float deltaTime)
{
    const float healthPct = player.getHealthPercent();

    if (healthPct >= 1.0f)
    {
        potionPool.releaseAll();
        potionTimer = Config::POTION_MIN_INTERVAL * 0.5f;
        return;
    }

    if (healthPct > Config::POTION_HEALTH_THRESHOLD) return;

    potionTimer -= deltaTime;
    if (potionTimer > 0.0f) return;

    Potion* potion = potionPool.acquire();
    if (potion != nullptr)
    {
        const float x = RandomFloat(40.0f, static_cast<float>(Config::SCREEN_WIDTH) - 64.0f);
        potion->spawn(x, Config::POTION_FALL_SPEED);
    }
    potionTimer = RandomFloat(Config::POTION_MIN_INTERVAL, Config::POTION_MAX_INTERVAL);
}

// Toma una bala libre del pool. Si no hay, no dispara (no se crea memoria nueva).
void Game::firePlayerProjectile()
{
    Projectile* bullet = projectilePool.acquire();
    if (bullet == nullptr) return;

    const Vector2 gun = player.getGunPosition();
    bullet->reset(gun.x, gun.y, Config::PLAYER_BULLET_SPEED, true);
    player.shoot();
    assetManager.playSound(SND_SHOOT);
}

void Game::fireEnemyProjectile(const Enemy& enemy)
{
    Projectile* bullet = projectilePool.acquire();
    if (bullet == nullptr) return;

    const Vector2 gun = enemy.getGunPosition();
    bullet->reset(gun.x, gun.y, Config::ENEMY_BULLET_SPEED, false);
}

void Game::spawnExplosion(float centerX, float centerY)
{
    Explosion* e = explosionPool.acquire();
    if (e != nullptr) e->spawn(centerX, centerY);
}

// -----------------------------------------------------------------------------
//  Motor de colisiones (Bounding Boxes con CheckCollisionRecs)
// -----------------------------------------------------------------------------
void Game::checkCollisions()
{
    const Rectangle playerBox = player.getGlobalBounds();

    for (int b = 0; b < projectilePool.size(); b++)
    {
        Projectile& bullet = projectilePool.get(b);
        if (!bullet.isEntityActive()) continue;

        if (bullet.isFromPlayer())
        {
            // a) Bala del jugador contra enemigo: se destruyen y suma puntaje
            for (int e = 0; e < enemies.size(); e++)
            {
                Enemy& enemy = enemies.get(e);
                if (!enemy.isEntityActive()) continue;
                if (!CheckCollisionRecs(bullet.getGlobalBounds(), enemy.getGlobalBounds())) continue;

                bullet.setActive(false);
                const Rectangle box = enemy.getGlobalBounds();
                if (enemy.takeDamage(bullet.getDamage()))
                {
                    player.addScore(enemy.getPointsValue());          // RF-03
                    spawnExplosion(box.x + box.width / 2.0f, box.y + box.height / 2.0f);
                    assetManager.playSound(SND_EXPLOSION);
                }
                else
                {
                    assetManager.playSound(SND_HIT);                  // la nave resistio
                }
                break;   // una bala solo golpea a una nave
            }
        }
        else
        {
            // b) Bala enemiga contra el jugador: resta 1 vida
            if (CheckCollisionRecs(bullet.getGlobalBounds(), playerBox) &&
                player.takeDamage(bullet.getDamage()))
            {
                bullet.setActive(false);
                assetManager.playSound(SND_HIT);
            }
        }
    }

    // c) Enemigo contra el jugador: la nave enemiga explota y el jugador pierde vida
    for (int e = 0; e < enemies.size(); e++)
    {
        Enemy& enemy = enemies.get(e);
        if (!enemy.isEntityActive()) continue;
        if (!CheckCollisionRecs(enemy.getGlobalBounds(), playerBox)) continue;

        if (player.takeDamage(1))
        {
            const Rectangle box = enemy.getGlobalBounds();
            enemy.setActive(false);
            spawnExplosion(box.x + box.width / 2.0f, box.y + box.height / 2.0f);
            assetManager.playSound(SND_EXPLOSION);
        }
    }

    // d) Pocion contra el jugador: recupera un porcentaje de vida (RF-11)
    for (int i = 0; i < potionPool.size(); i++)
    {
        Potion& potion = potionPool.get(i);
        if (!potion.isEntityActive()) continue;
        if (!CheckCollisionRecs(potion.getGlobalBounds(), playerBox)) continue;

        // 20 % de 5 vidas = 1 vida (como minimo siempre cura 1)
        int heal = static_cast<int>(static_cast<float>(player.getMaxHealth()) * Config::POTION_HEAL_PERCENT + 0.5f);
        if (heal < 1) heal = 1;

        player.heal(heal);
        potion.setActive(false);
        hudManager.showNotification(TextFormat("+%i%% VIDA", static_cast<int>(Config::POTION_HEAL_PERCENT * 100.0f + 0.5f)), LIME);
        assetManager.playSound(SND_POTION);
    }
}

// -----------------------------------------------------------------------------
//  Estados, oleadas y reinicio
// -----------------------------------------------------------------------------
void Game::changeState(GameState newState)
{
    if (newState == GameState::GAME_OVER)
    {
        currentScore = player.getScore();
        newRecord = currentScore > highScore;
        if (newRecord) highScore = currentScore;
    }
    else if (newState == GameState::MENU)
    {
        releaseAllEntities();
        hudManager.clear();
    }
    currentState = newState;
}

void Game::spawnWave()
{
    currentWave = waveManager.getCurrentWave() + 1;
    waveManager.startWave(currentWave);
}

// RF-10: limpia todas las entidades y reajusta los contadores.
void Game::restart()
{
    releaseAllEntities();
    const Vector2 start = playerStartPosition();
    player.reset(start.x, start.y);
    waveManager.reset();
    hudManager.clear();
    currentScore = 0;
    currentWave = 0;
    newRecord = false;
    potionTimer = Config::POTION_MIN_INTERVAL * 0.5f;
    spawnWave();
}

void Game::releaseAllEntities()
{
    enemies.releaseAll();
    projectilePool.releaseAll();
    potionPool.releaseAll();
    explosionPool.releaseAll();
}

Vector2 Game::playerStartPosition() const
{
    return Vector2{ static_cast<float>(Config::SCREEN_WIDTH) / 2.0f - player.getWidth() / 2.0f,
                    static_cast<float>(Config::SCREEN_HEIGHT) - player.getHeight() - Config::PLAYER_BOTTOM_MARGIN };
}

// -----------------------------------------------------------------------------
//  Dibujo (el HUD va al final para quedar en la capa superior)
// -----------------------------------------------------------------------------
void Game::render()
{
    BeginDrawing();
    ClearBackground(Color{ 5, 6, 20, 255 });
    starfield.render();

    if (currentState == GameState::MENU)
    {
        hudManager.renderMenu(highScore, static_cast<float>(GetTime()));
        EndDrawing();
        return;
    }

    // Cada render() ya ignora los objetos inactivos
    for (int i = 0; i < potionPool.size(); i++)     potionPool.get(i).render();
    for (int i = 0; i < enemies.size(); i++)        enemies.get(i).render();
    for (int i = 0; i < projectilePool.size(); i++) projectilePool.get(i).render();
    player.render();
    for (int i = 0; i < explosionPool.size(); i++)  explosionPool.get(i).render();

    hudManager.renderHUD(player, currentWave, waveManager.getRemainingEnemies(enemies), highScore,
                         IsGamepadAvailable(Config::GAMEPAD_ID));
    if (waveManager.isShowingBanner())
        hudManager.renderWaveBanner(currentWave, waveManager.getBannerTimer(), waveManager.getConfig());
    hudManager.renderNotification();

    if (currentState == GameState::PAUSED)
        hudManager.renderPause();
    else if (currentState == GameState::GAME_OVER)
        hudManager.renderGameOver(currentScore, currentWave, highScore, newRecord);

    EndDrawing();
}
