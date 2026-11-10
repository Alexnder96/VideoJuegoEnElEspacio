#pragma once
// =============================================================================
//  Config.h
//  Parametros globales del juego. Todos los valores de dificultad, velocidad,
//  vidas y oleadas estan en un solo lugar (RNF-11: niveles parametrizados).
//  Para balancear el juego basta con modificar este archivo.
// =============================================================================

namespace Config
{
    // ---------------- Ventana y velocidad constante del juego ----------------
    const int   SCREEN_WIDTH  = 900;
    const int   SCREEN_HEIGHT = 700;
    const int   HUD_HEIGHT    = 64;      // franja superior reservada para el HUD
    const int   TARGET_FPS    = 60;      // RNF-03
    const float MAX_DELTA     = 0.05f;   // tope del delta time (evita saltos)
    const char* const WINDOW_TITLE = "Space Shooter - Videojuego en el Espacio";

    // ---------------- Jugador (movimiento con aceleracion) ----------------
    const float PLAYER_MAX_SPEED      = 400.0f;   // velocidad maxima (px/s)
    const float PLAYER_ACCELERATION   = 1800.0f;  // cuanto acelera por segundo (px/s^2)
    const float PLAYER_FRICTION       = 1500.0f;  // cuanto frena al soltar la tecla (px/s^2)
    const int   PLAYER_MAX_HEALTH     = 5;        // vida completa = 100 %
    const float PLAYER_SHOOT_COOLDOWN = 0.18f;    // tiempo de recarga entre disparos
    const float PLAYER_INVULNERABLE   = 1.2f;     // segundos de invulnerabilidad tras un golpe
    const float PLAYER_BOTTOM_MARGIN  = 16.0f;    // distancia al borde inferior

    // ---------------- Controles ----------------
    const int   GAMEPAD_ID       = 0;       // primer mando conectado
    const float GAMEPAD_DEADZONE = 0.25f;   // ignora movimientos pequenos del stick

    // ---------------- Animaciones (sprite sheets) ----------------
    const int   PLAYER_FRAMES     = 4;      // cuadros del fuego del motor
    const float PLAYER_ANIM_FPS   = 12.0f;  // velocidad de animacion (cuadros/seg)
    const int   ENEMY_FRAMES      = 2;
    const float ENEMY_ANIM_FPS    = 4.0f;
    const int   EXPLOSION_FRAMES  = 6;
    const float EXPLOSION_ANIM_FPS = 14.0f;

    // ---------------- Proyectiles (Object Pool) ----------------
    const int   MAX_PROJECTILES      = 96;       // capacidad fija del pool
    const float PLAYER_BULLET_SPEED  = -620.0f;  // eje Y negativo (hacia arriba)
    const float ENEMY_BULLET_SPEED   = 300.0f;   // eje Y positivo (hacia abajo)
    const int   PLAYER_BULLET_DAMAGE = 1;
    const int   ENEMY_BULLET_DAMAGE  = 1;

    // ---------------- Enemigos ----------------
    const int   MAX_ENEMIES      = 50;       // RNF-03: 50 entidades simultaneas
    const float BASE_ENEMY_SPEED = 85.0f;    // velocidad vertical en la oleada 1

    // ---------------- IA de disparo enemigo ----------------
    //  La nave "mira" hacia abajo: si el jugador esta dentro de AI_AIM_RANGE
    //  pixeles en X y ya se recargo, decide disparar.
    const int   ENEMY_FIRE_START_WAVE = 3;       // desde esta oleada disparan
    const float AI_AIM_RANGE          = 60.0f;
    const float ENEMY_FIRE_COOLDOWN   = 1.6f;    // segundos minimos entre disparos

    // ---------------- Oleadas / niveles (RNF-11, RF-09) ----------------
    //  Oleada n (n >= 1):
    //    naves      = BASE_ENEMIES + ENEMIES_PER_WAVE * (n - 1)       (tope MAX_ENEMIES_PER_WAVE)
    //    velocidad  = BASE_ENEMY_SPEED * (1 + SPEED_INCREMENT * (n - 1))
    //    intervalo  = BASE_SPAWN_INTERVAL - SPAWN_INTERVAL_STEP * (n - 1) (minimo MIN_SPAWN_INTERVAL)
    //  La dificultad sube hasta MAX_DIFFICULTY_WAVE (10 niveles) y luego se mantiene.
    const int   MAX_DIFFICULTY_WAVE  = 10;
    const int   BASE_ENEMIES         = 6;
    const int   ENEMIES_PER_WAVE     = 3;
    const int   MAX_ENEMIES_PER_WAVE = 40;
    const float SPEED_INCREMENT      = 0.12f;
    const float BASE_SPAWN_INTERVAL  = 1.10f;
    const float SPAWN_INTERVAL_STEP  = 0.08f;
    const float MIN_SPAWN_INTERVAL   = 0.35f;
    const float WAVE_BANNER_TIME     = 2.5f;   // aviso "OLEADA N" entre oleadas
    const int   WAVE_CLEAR_BONUS     = 250;    // bonus = 250 * numero de oleada

    // ---------------- Pociones de vida (RF-11) ----------------
    //  - Con vida completa (100 %) NO caen pociones.
    //  - Caen cuando la vida es <= POTION_HEALTH_THRESHOLD (60 % = 3 de 5).
    //  - Cada pocion recupera POTION_HEAL_PERCENT de la vida maxima (20 % = 1 vida).
    const float POTION_HEALTH_THRESHOLD = 0.60f;
    const float POTION_HEAL_PERCENT     = 0.20f;
    const float POTION_MIN_INTERVAL     = 5.0f;
    const float POTION_MAX_INTERVAL     = 9.0f;
    const float POTION_FALL_SPEED       = 120.0f;
    const int   MAX_POTIONS             = 3;

    // ---------------- Efectos ----------------
    const int   MAX_EXPLOSIONS = 40;
    const int   STAR_COUNT     = 140;
}
