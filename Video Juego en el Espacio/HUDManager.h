#pragma once
#include "raylib.h"
#include "Player.h"
#include "WaveManager.h"
#include "AssetManager.h"
#include <string>

// =============================================================================
//  HUDManager: interfaz de usuario (RF-08).
//  Dibuja la barra superior (puntaje, oleada, enemigos, vida, record, FPS),
//  los avisos temporales y las pantallas de Menu, Pausa y Game Over.
//  Se dibuja siempre al final para quedar en la capa superior.
// =============================================================================
class HUDManager
{
public:
    HUDManager();

    /// Actualiza el temporizador de las notificaciones.
    void update(float deltaTime);

    /// Barra superior con los datos de la partida en tiempo real.
    void renderHUD(const Player& player, int wave, int remainingEnemies, int highScore,
                   bool gamepadConnected) const;

    /// Aviso central "OLEADA N" con los parametros de dificultad.
    void renderWaveBanner(int wave, float timer, const WaveConfig& config) const;

    /// Mensaje breve en pantalla (por ejemplo "+1 VIDA").
    void showNotification(const std::string& text, Color color);
    void renderNotification() const;

    void renderMenu(int highScore, float time) const;
    void renderPause() const;
    void renderGameOver(int score, int wave, int highScore, bool newRecord) const;

    /// Borra las notificaciones pendientes (al reiniciar).
    void clear();

private:
    void drawHeart(float cx, float cy, float size, Color color) const;
    void drawHealthBar(const Player& player, float x, float y, float w, float h) const;
    void drawCentered(const char* text, int y, int fontSize, Color color) const;
    void drawLegendRow(TextureId tex, const char* name, const char* detail, int y) const;

    std::string notificationText;
    Color       notificationColor;
    float       notificationTimer;
};
