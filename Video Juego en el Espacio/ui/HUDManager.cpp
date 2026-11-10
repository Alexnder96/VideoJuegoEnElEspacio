#include "ui/HUDManager.h"
#include "resources/AssetManager.h"
#include "common/Config.h"
#include <cmath>

namespace
{
    const Color HUD_BG     = { 12, 16, 38, 235 };
    const Color HUD_LINE   = { 70, 110, 200, 255 };
    const Color LABEL_GRAY = { 150, 160, 190, 255 };
    const Color PANEL_BG   = { 10, 14, 34, 220 };
}

HUDManager::HUDManager()
    : notificationText(), notificationColor(WHITE), notificationTimer(0.0f)
{
}

void HUDManager::update(float deltaTime)
{
    if (notificationTimer > 0.0f) notificationTimer -= deltaTime;
}

void HUDManager::clear()
{
    notificationTimer = 0.0f;
    notificationText.clear();
}

void HUDManager::drawCentered(const char* text, int y, int fontSize, Color color) const
{
    const int w = MeasureText(text, fontSize);
    DrawText(text, Config::SCREEN_WIDTH / 2 - w / 2, y, fontSize, color);
}

// Corazon hecho con dos circulos y un triangulo.
void HUDManager::drawHeart(float cx, float cy, float size, Color color) const
{
    const float r = size * 0.28f;
    DrawCircleV({ cx - r, cy - r * 0.3f }, r, color);
    DrawCircleV({ cx + r, cy - r * 0.3f }, r, color);
    DrawTriangle({ cx - 2.0f * r, cy - r * 0.05f },
                 { cx, cy + size * 0.5f },
                 { cx + 2.0f * r, cy - r * 0.05f }, color);
}

// Barra de vida con porcentaje y una marca en el umbral de las pociones.
void HUDManager::drawHealthBar(const Player& player, float x, float y, float w, float h) const
{
    const float pct = player.getHealthPercent();
    Color fill = LIME;
    if (pct <= Config::POTION_HEALTH_THRESHOLD) fill = GOLD;
    if (pct <= 0.2f) fill = RED;

    DrawRectangleRec({ x, y, w, h }, Color{ 40, 40, 60, 255 });
    DrawRectangleRec({ x, y, w * pct, h }, fill);
    DrawRectangleLinesEx({ x, y, w, h }, 1.0f, LABEL_GRAY);

    // Marca del umbral a partir del cual caen pociones
    const float markX = x + w * Config::POTION_HEALTH_THRESHOLD;
    DrawLineEx({ markX, y - 2.0f }, { markX, y + h + 2.0f }, 2.0f, SKYBLUE);

    const char* txt = TextFormat("%i%%", static_cast<int>(pct * 100.0f + 0.5f));
    DrawText(txt, static_cast<int>(x + w + 8.0f), static_cast<int>(y - 1.0f), 14, WHITE);
}

void HUDManager::renderHUD(const Player& player, int wave, int remainingEnemies, int highScore,
                           bool gamepadConnected) const
{
    DrawRectangle(0, 0, Config::SCREEN_WIDTH, Config::HUD_HEIGHT, HUD_BG);
    DrawLine(0, Config::HUD_HEIGHT, Config::SCREEN_WIDTH, Config::HUD_HEIGHT, HUD_LINE);

    // Puntaje
    DrawText("PUNTAJE", 16, 8, 14, LABEL_GRAY);
    DrawText(TextFormat("%07i", player.getScore()), 16, 26, 26, WHITE);

    // Oleada
    DrawText("OLEADA", 180, 8, 14, LABEL_GRAY);
    DrawText(TextFormat("%i", wave), 180, 26, 26, SKYBLUE);

    // Enemigos restantes
    DrawText("ENEMIGOS", 270, 8, 14, LABEL_GRAY);
    DrawText(TextFormat("%i", remainingEnemies), 270, 26, 26, ORANGE);

    // Vida: corazones + barra porcentual
    DrawText("VIDA", 390, 8, 14, LABEL_GRAY);
    for (int i = 0; i < player.getMaxHealth(); ++i)
    {
        const Color c = (i < player.getHealth()) ? Color{ 235, 50, 80, 255 } : Color{ 60, 60, 80, 255 };
        drawHeart(400.0f + static_cast<float>(i) * 24.0f, 36.0f, 20.0f, c);
    }
    drawHealthBar(player, 530.0f, 30.0f, 150.0f, 12.0f);
    DrawText("pociones", 530, 8, 14, Fade(SKYBLUE, 0.8f));

    // Record de la sesion
    DrawText("RECORD", 760, 8, 14, LABEL_GRAY);
    DrawText(TextFormat("%07i", highScore), 760, 26, 22, GOLD);

    // Indicador de mando conectado
    if (gamepadConnected)
        DrawText("MANDO", 16, Config::SCREEN_HEIGHT - 20, 14, Fade(SKYBLUE, 0.8f));

    // FPS en la esquina inferior derecha (RNF-03)
    DrawText(TextFormat("%i FPS", GetFPS()), Config::SCREEN_WIDTH - 70, Config::SCREEN_HEIGHT - 20, 14, Fade(LIME, 0.7f));
}

void HUDManager::renderWaveBanner(int wave, float timer, const WaveConfig& config) const
{
    // Aparece y desaparece con transparencia
    const float t = timer / Config::WAVE_BANNER_TIME;
    const float alpha = (t > 0.85f) ? (1.0f - t) / 0.15f : (t < 0.2f ? t / 0.2f : 1.0f);

    DrawRectangle(0, 250, Config::SCREEN_WIDTH, 130, Fade(PANEL_BG, 0.85f * alpha));
    drawCentered(TextFormat("OLEADA %i", wave), 268, 56, Fade(SKYBLUE, alpha));
    drawCentered(TextFormat("%i naves  |  velocidad x%.2f%s", config.enemyCount, config.speedMultiplier,
                            config.enemiesCanFire ? "  |  disparan!" : ""),
                 338, 20, Fade(LIGHTGRAY, alpha));
}

void HUDManager::showNotification(const std::string& text, Color color)
{
    notificationText = text;
    notificationColor = color;
    notificationTimer = 1.6f;
}

void HUDManager::renderNotification() const
{
    if (notificationTimer <= 0.0f || notificationText.empty()) return;

    const float alpha = notificationTimer < 0.5f ? notificationTimer / 0.5f : 1.0f;
    const int rise = static_cast<int>((1.6f - notificationTimer) * 20.0f);
    drawCentered(notificationText.c_str(), 430 - rise, 32, Fade(notificationColor, alpha));
}

// Fila de la leyenda de enemigos del menu (sprite + nombre + detalle).
void HUDManager::drawLegendRow(TextureId tex, const char* name, const char* detail, int y) const
{
    // Se dibuja solo el PRIMER cuadro del sprite sheet (cuadros cuadrados)
    const Texture2D* t = AssetManager::getInstance().getTexture(tex);
    const float frameSize = static_cast<float>(t->height);
    const int left = Config::SCREEN_WIDTH / 2 - 190;
    DrawTextureRec(*t, Rectangle{ 0.0f, 0.0f, frameSize, frameSize },
                   Vector2{ static_cast<float>(left), static_cast<float>(y) }, WHITE);
    DrawText(name, left + 60, y + 4, 20, WHITE);
    DrawText(detail, left + 60, y + 26, 16, LABEL_GRAY);
}

void HUDManager::renderMenu(int highScore, float time) const
{
    const float bob = std::sin(time * 2.0f) * 6.0f;
    drawCentered("SPACE SHOOTER", 70 + static_cast<int>(bob), 64, SKYBLUE);
    drawCentered("Videojuego en el Espacio", 142, 24, LIGHTGRAY);

    DrawRectangle(Config::SCREEN_WIDTH / 2 - 220, 190, 440, 240, PANEL_BG);
    DrawRectangleLines(Config::SCREEN_WIDTH / 2 - 220, 190, 440, 240, HUD_LINE);
    drawLegendRow(TEX_ENEMY_STRAIGHT, "Nave recta",      "100 pts  -  desde la oleada 1", 205);
    drawLegendRow(TEX_ENEMY_SINE, "Nave sinusoidal", "150 pts  -  desde la oleada 2", 260);
    drawLegendRow(TEX_ENEMY_ZIGZAG, "Nave zig-zag",    "200 pts, 2 impactos - oleada 3", 315);
    drawLegendRow(TEX_POTION, "Pocion de vida",  "Cae con vida <= 60%: +20% vida", 375);

    drawCentered("Teclado:  A / D o FLECHAS mover  -  ESPACIO disparar  -  P pausa", 450, 18, LIGHTGRAY);
    drawCentered("Mando:  stick / cruceta mover  -  boton A disparar  -  START pausa", 475, 18, LIGHTGRAY);

    const float blink = 0.5f + 0.5f * std::sin(time * 4.0f);
    drawCentered("Presiona ENTER o START para comenzar", 525, 30, Fade(GOLD, 0.4f + 0.6f * blink));

    if (highScore > 0)
        drawCentered(TextFormat("Record: %07i", highScore), 580, 20, GOLD);
    drawCentered("ESC : salir", 650, 16, Fade(LABEL_GRAY, 0.8f));
}

void HUDManager::renderPause() const
{
    DrawRectangle(0, 0, Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, Fade(BLACK, 0.6f));
    drawCentered("PAUSA", 270, 64, WHITE);
    drawCentered("P / START : continuar      M : volver al menu", 350, 22, LIGHTGRAY);
}

void HUDManager::renderGameOver(int score, int wave, int highScore, bool newRecord) const
{
    DrawRectangle(0, 0, Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, Color{ 40, 0, 15, 190 });
    drawCentered("GAME OVER", 200, 72, RED);
    drawCentered(TextFormat("Puntaje final: %07i", score), 300, 30, WHITE);
    drawCentered(TextFormat("Oleada alcanzada: %i", wave), 342, 24, SKYBLUE);
    if (newRecord)
        drawCentered("NUEVO RECORD!", 385, 28, GOLD);
    else
        drawCentered(TextFormat("Record: %07i", highScore), 385, 22, GOLD);
    drawCentered("R / START : reiniciar      M : menu", 460, 24, LIGHTGRAY);
}
