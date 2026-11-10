#include "entities/Entity.h"

Entity::Entity(float startX, float startY, float startSpeedX, float startSpeedY)
    : x(startX), y(startY), speedX(startSpeedX), speedY(startSpeedY),
      width(16.0f), height(16.0f), hitboxInset(0.0f),
      texture(nullptr), animation(), isActive(false)
{
}

// Dibuja el cuadro actual. Si no hay textura dibuja un rectangulo para que
// la entidad siga siendo visible.
void Entity::render() const
{
    if (!isActive) return;

    if (texture != nullptr && texture->id != 0)
        animation.draw(x, y, WHITE);
    else
        DrawRectangle(static_cast<int>(x), static_cast<int>(y),
                      static_cast<int>(width), static_cast<int>(height), MAGENTA);
}

Vector2 Entity::getPosition() const
{
    return Vector2{ x, y };
}

void Entity::setPosition(float newX, float newY)
{
    x = newX;
    y = newY;
}

// Bounding Box reducida por hitboxInset en los cuatro lados.
Rectangle Entity::getGlobalBounds() const
{
    return Rectangle{ x + hitboxInset, y + hitboxInset,
                      width - 2.0f * hitboxInset, height - 2.0f * hitboxInset };
}

bool Entity::isEntityActive() const
{
    return isActive;
}

void Entity::setActive(bool active)
{
    isActive = active;
}

void Entity::setTexture(const Texture2D* newTexture)
{
    setSpriteSheet(newTexture, 1, 0.0f, true);
}

// El tamano de la entidad es el de UN cuadro, no el de toda la hoja.
void Entity::setSpriteSheet(const Texture2D* sheet, int frames, float framesPerSecond, bool loop)
{
    texture = sheet;
    animation.setup(sheet, frames, framesPerSecond, loop);
    if (texture != nullptr && texture->id != 0)
    {
        width = animation.getFrameWidth();
        height = animation.getFrameHeight();
    }
}

float Entity::getWidth() const
{
    return width;
}

float Entity::getHeight() const
{
    return height;
}
