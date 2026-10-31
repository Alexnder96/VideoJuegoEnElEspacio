#pragma once
#include "raylib.h"

// =============================================================================
//  Animation: reproduce un Sprite Sheet (varias imagenes en una sola textura,
//  colocadas una al lado de otra en horizontal).
//
//  Ejemplo: una hoja de 192x52 con 4 cuadros -> cada cuadro mide 48x52.
//  Cada cierto tiempo se avanza al siguiente cuadro. Ese tiempo depende de la
//  VELOCIDAD DE ANIMACION (cuadros por segundo), que es independiente de los
//  60 FPS del juego.
// =============================================================================
class Animation
{
public:
    Animation();

    /// Configura la hoja: cantidad de cuadros, velocidad (cuadros/seg) y si se repite.
    void setup(const Texture2D* spriteSheet, int frames, float framesPerSecond, bool looping);

    /// Avanza la animacion segun el delta time.
    void update(float deltaTime);

    /// Vuelve al primer cuadro.
    void reset();

    /// Dibuja el cuadro actual en (x, y).
    void draw(float x, float y, Color tint) const;

    /// True cuando una animacion sin repeticion llego al ultimo cuadro.
    bool isFinished() const;

    float getFrameWidth() const;
    float getFrameHeight() const;

private:
    const Texture2D* sheet;   // textura del AssetManager (no se libera aqui)
    int   frameCount;         // total de cuadros de la hoja
    int   currentFrame;       // cuadro que se dibuja ahora
    float frameDuration;      // segundos que dura cada cuadro = 1 / velocidad
    float timer;              // tiempo acumulado en el cuadro actual
    bool  loop;
    bool  finished;
};
