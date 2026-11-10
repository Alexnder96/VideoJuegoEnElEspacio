#pragma once
#include "raylib.h"
#include <string>

// Identificadores de las texturas del juego
enum TextureId
{
    TEX_PLAYER,           // sprite sheet de 4 cuadros
    TEX_ENEMY_STRAIGHT,   // sprite sheet de 2 cuadros
    TEX_ENEMY_SINE,       // sprite sheet de 2 cuadros
    TEX_ENEMY_ZIGZAG,     // sprite sheet de 2 cuadros
    TEX_EXPLOSION,        // sprite sheet de 6 cuadros
    TEX_BULLET_PLAYER,
    TEX_BULLET_ENEMY,
    TEX_POTION,
    TEXTURE_COUNT
};

// Identificadores de los efectos de sonido
enum SoundId
{
    SND_SHOOT,
    SND_EXPLOSION,
    SND_HIT,
    SND_POTION,
    SND_WAVE,
    SOUND_COUNT
};

// =============================================================================
//  AssetManager (patron Singleton: existe un solo objeto en todo el programa)
//  Carga todas las texturas y sonidos UNA sola vez al arrancar (RNF-09).
//  Durante el juego solo entrega lo que ya esta en memoria, sin leer el disco.
//  Usa rutas relativas a la carpeta "assets" (RNF-10).
//  Si falta un archivo crea un rectangulo de color para no cerrar el juego.
// =============================================================================
class AssetManager
{
public:
    /// Acceso a la unica instancia.
    static AssetManager& getInstance();

    /// Precarga todos los recursos. Llamar despues de InitWindow().
    void loadAll();

    /// Libera los recursos. Llamar antes de CloseWindow().
    void unloadAll();

    const Texture2D* getTexture(TextureId id) const;
    void playSound(SoundId id) const;

private:
    AssetManager();                                   // privado: nadie mas puede crearlo
    AssetManager(const AssetManager&) = delete;       // prohibido copiarlo
    AssetManager& operator=(const AssetManager&) = delete;

    std::string findAssetsFolder() const;
    Texture2D   loadTextureOrFallback(const char* fileName, int w, int h, Color color);

    Texture2D   textures[TEXTURE_COUNT];
    Sound       sounds[SOUND_COUNT];
    bool        soundLoaded[SOUND_COUNT];
    std::string assetsPath;
    bool        loaded;
};
