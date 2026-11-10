#include "graphics/Animation.h"

Animation::Animation()
    : sheet(nullptr), frameCount(1), currentFrame(0),
      frameDuration(0.0f), timer(0.0f), loop(true), finished(false)
{
}

void Animation::setup(const Texture2D* spriteSheet, int frames, float framesPerSecond, bool looping)
{
    sheet = spriteSheet;
    frameCount = (frames < 1) ? 1 : frames;
    frameDuration = (framesPerSecond > 0.0f) ? 1.0f / framesPerSecond : 0.0f;
    loop = looping;
    reset();
}

void Animation::reset()
{
    currentFrame = 0;
    timer = 0.0f;
    finished = false;
}

// Cada vez que el tiempo acumulado supera la duracion de un cuadro,
// se pasa al siguiente. Al llegar al final se repite o se detiene.
void Animation::update(float deltaTime)
{
    if (frameCount <= 1 || frameDuration <= 0.0f || finished) return;

    timer += deltaTime;
    while (timer >= frameDuration)
    {
        timer -= frameDuration;
        currentFrame++;

        if (currentFrame >= frameCount)
        {
            if (loop)
            {
                currentFrame = 0;
            }
            else
            {
                currentFrame = frameCount - 1;
                finished = true;
                return;
            }
        }
    }
}

// Recorta de la hoja solo el rectangulo del cuadro actual (DrawTextureRec).
void Animation::draw(float x, float y, Color tint) const
{
    if (sheet == nullptr || sheet->id == 0) return;

    const float w = getFrameWidth();
    const float h = getFrameHeight();
    const Rectangle source = { static_cast<float>(currentFrame) * w, 0.0f, w, h };
    DrawTextureRec(*sheet, source, Vector2{ x, y }, tint);
}

bool Animation::isFinished() const
{
    return finished;
}

float Animation::getFrameWidth() const
{
    if (sheet == nullptr) return 0.0f;
    return static_cast<float>(sheet->width) / static_cast<float>(frameCount);
}

float Animation::getFrameHeight() const
{
    if (sheet == nullptr) return 0.0f;
    return static_cast<float>(sheet->height);
}
