#include "core/Game.h"
#include "common/Config.h"

// -----------------------------------------------------------------------------
//  Construccion y destruccion
// -----------------------------------------------------------------------------
Game::Game()
    : currentState(GameState::MENU),
      world(),
      inputManager(),
      waveManager(),
      combatSystem(),
      potionSystem(),
      collisionSystem(),
      hudManager(),
      starfield(),
      assetManager(AssetManager::getInstance()),
      input{ 0.0f, false },
      currentScore(0),
      currentWave(0),
      highScore(0),
      newRecord(false),
      isRunning(true)
{
    InitWindow(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, Config::WINDOW_TITLE);
    InitAudioDevice();
    SetTargetFPS(Config::TARGET_FPS);   // velocidad constante: 60 cuadros por segundo

    // RNF-09: todos los recursos se cargan aqui, nunca dentro del bucle.
    assetManager.loadAll();
    world.player.setSpriteSheet(assetManager.getTexture(TEX_PLAYER),
                                Config::PLAYER_FRAMES, Config::PLAYER_ANIM_FPS, true);

    const Vector2 start = playerStartPosition();
    world.player.setPosition(start.x, start.y);
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
//  Controles: el modulo de entrada dice QUE se pidio; aqui se decide que hacer.
//  Los cambios de estado se resuelven aqui; el movimiento y el disparo se
//  guardan en 'input' para usarlos en update().
// -----------------------------------------------------------------------------
void Game::processEvents()
{
    input = GameplayInput{ 0.0f, false };

    switch (currentState)
    {
    case GameState::MENU:
        if (inputManager.isConfirmPressed())
        {
            restart();
            changeState(GameState::PLAYING);
        }
        break;

    case GameState::PLAYING:
        if (inputManager.isPausePressed())
            changeState(GameState::PAUSED);
        else
            input = inputManager.readGameplay();
        break;

    case GameState::PAUSED:
        if (inputManager.isPausePressed())
            changeState(GameState::PLAYING);
        else if (inputManager.isMenuPressed())
            changeState(GameState::MENU);
        break;

    case GameState::GAME_OVER:
        if (inputManager.isRestartPressed())
        {
            restart();
            changeState(GameState::PLAYING);
        }
        else if (inputManager.isMenuPressed())
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
        world.updateExplosions(deltaTime);
        break;
    }
}

// Todo lo que pasa en un fotograma de juego: cada paso lo hace un modulo.
void Game::updatePlaying(float deltaTime)
{
    Player& player = world.player;

    // 1) Jugador: aceleracion segun los controles (RF-01)
    player.accelerate(input.moveDirection, deltaTime);
    player.update(deltaTime);

    // 2) Disparo: primero se valida la recarga (caso de uso "Disparar")
    if (input.shootPressed && player.canShoot())
        combatSystem.firePlayerProjectile(world);

    // 3) Oleadas: aparicion de enemigos con temporizador (RF-02)
    waveManager.update(deltaTime, world.enemies);

    // 4) Actualizar todas las entidades activas de cada pool
    world.updateEntities(deltaTime);

    // 5) IA enemiga y pociones
    combatSystem.updateEnemyAI(world);
    potionSystem.update(deltaTime, world);

    // 6) Colisiones
    checkCollisions();
    currentScore = player.getScore();

    // 7) Fin de la partida (RF-07)
    if (!player.isAlive())
    {
        const Rectangle b = player.getGlobalBounds();
        world.spawnExplosion(b.x + b.width / 2.0f, b.y + b.height / 2.0f);
        assetManager.playSound(SND_EXPLOSION);
        player.setActive(false);
        changeState(GameState::GAME_OVER);
        return;
    }

    // 8) Cambio de oleada: se destruyeron todas las naves (RF-09)
    if (waveManager.isWaveCleared(world.enemies))
    {
        const int bonus = Config::WAVE_CLEAR_BONUS * currentWave;
        player.addScore(bonus);
        hudManager.showNotification(TextFormat("OLEADA SUPERADA  +%i", bonus), GOLD);
        spawnWave();
    }
}

// El modulo de colisiones resuelve los choques y devuelve un reporte;
// con ese reporte Game avisa en el HUD.
void Game::checkCollisions()
{
    const CollisionReport report = collisionSystem.resolve(world);

    for (int i = 0; i < report.potionsCollected; i++)
    {
        hudManager.showNotification(
            TextFormat("+%i%% VIDA", static_cast<int>(Config::POTION_HEAL_PERCENT * 100.0f + 0.5f)), LIME);
    }
}

// -----------------------------------------------------------------------------
//  Estados, oleadas y reinicio
// -----------------------------------------------------------------------------
void Game::changeState(GameState newState)
{
    if (newState == GameState::GAME_OVER)
    {
        currentScore = world.player.getScore();
        newRecord = currentScore > highScore;
        if (newRecord) highScore = currentScore;
    }
    else if (newState == GameState::MENU)
    {
        world.releaseAll();
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
    world.releaseAll();
    const Vector2 start = playerStartPosition();
    world.player.reset(start.x, start.y);
    waveManager.reset();
    potionSystem.reset();
    hudManager.clear();
    currentScore = 0;
    currentWave = 0;
    newRecord = false;
    spawnWave();
}

Vector2 Game::playerStartPosition() const
{
    const Player& player = world.player;
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

    world.render();

    hudManager.renderHUD(world.player, currentWave, waveManager.getRemainingEnemies(world.enemies), highScore,
                         inputManager.isGamepadConnected());
    if (waveManager.isShowingBanner())
        hudManager.renderWaveBanner(currentWave, waveManager.getBannerTimer(), waveManager.getConfig());
    hudManager.renderNotification();

    if (currentState == GameState::PAUSED)
        hudManager.renderPause();
    else if (currentState == GameState::GAME_OVER)
        hudManager.renderGameOver(currentScore, currentWave, highScore, newRecord);

    EndDrawing();
}
