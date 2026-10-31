#pragma once
#include "raylib.h"
#include "Animation.h"

// =============================================================================
//  Entity (clase base abstracta)
//  Contiene lo comun a todas las entidades: posicion (x, y), velocidad,
//  textura (sprite sheet) y estado activo. Player, Enemy, Projectile, Potion
//  y Explosion derivan de ella y escriben su propio update().
// =============================================================================
class Entity
{
public:
    /// Crea la entidad en (x, y) con la velocidad indicada. Empieza inactiva.
    Entity(float startX, float startY, float startSpeedX, float startSpeedY);
    virtual ~Entity() = default;

    /// Logica de cada fotograma. deltaTime = segundos que duro el frame.
    virtual void update(float deltaTime) = 0;

    /// Dibuja el cuadro actual del sprite (o un rectangulo si no hay textura).
    virtual void render() const;

    Vector2   getPosition() const;
    void      setPosition(float newX, float newY);

    /// Caja delimitadora (Bounding Box) que usa el motor de colisiones.
    /// Se achica con hitboxInset para que el choque coincida con el dibujo.
    Rectangle getGlobalBounds() const;

    bool  isEntityActive() const;
    void  setActive(bool active);

    /// Textura de una sola imagen.
    void  setTexture(const Texture2D* newTexture);

    /// Textura tipo Sprite Sheet con varios cuadros y su velocidad de animacion.
    void  setSpriteSheet(const Texture2D* sheet, int frames, float framesPerSecond, bool loop);

    float getWidth() const;
    float getHeight() const;

protected:
    float x;
    float y;
    float speedX;
    float speedY;
    float width;
    float height;
    float hitboxInset;             // margen interno de la caja de colision
    const Texture2D* texture;      // la textura vive en el AssetManager
    Animation animation;           // cuadro actual del sprite sheet
    bool  isActive;
};
