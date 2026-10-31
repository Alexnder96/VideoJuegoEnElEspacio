#include "AssetManager.h"

AssetManager::AssetManager()
    : textures(), sounds(), soundLoaded(), assetsPath("assets/"), loaded(false)
{
}

AssetManager& AssetManager::getInstance()
{
    static AssetManager instance;   // se crea la primera vez y dura todo el programa
    return instance;
}

// Visual Studio ejecuta el juego desde la carpeta del proyecto, pero el .exe
// tambien puede abrirse directo. Se prueban varias rutas RELATIVAS.
std::string AssetManager::findAssetsFolder() const
{
    const std::string exeDir = GetApplicationDirectory();
    const std::string candidates[] = {
        "assets/",
        "../assets/",
        exeDir + "assets/",
        exeDir + "../assets/",
        exeDir + "../../assets/",
        exeDir + "../../../assets/"
    };

    for (const std::string& path : candidates)
    {
        if (DirectoryExists(path.c_str()))
            return path;
    }
    return "assets/";
}

// Carga una imagen del disco; si no existe crea un rectangulo de color.
Texture2D AssetManager::loadTextureOrFallback(const char* fileName, int w, int h, Color color)
{
    const std::string fullPath = assetsPath + fileName;
    if (FileExists(fullPath.c_str()))
    {
        Texture2D tex = LoadTexture(fullPath.c_str());
        if (tex.id != 0) return tex;
    }

    TraceLog(LOG_WARNING, "ASSETS: no se encontro %s, se usa un rectangulo", fullPath.c_str());
    Image img = GenImageColor(w, h, color);
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

void AssetManager::loadAll()
{
    if (loaded) return;

    assetsPath = findAssetsFolder();
    TraceLog(LOG_INFO, "ASSETS: cargando recursos desde %s", assetsPath.c_str());

    // Tamano de reemplazo = tamano total de la hoja (ancho de un cuadro x cuadros)
    textures[TEX_PLAYER]         = loadTextureOrFallback("player_sheet.png",         192, 52, SKYBLUE);
    textures[TEX_ENEMY_STRAIGHT] = loadTextureOrFallback("enemy_straight_sheet.png",  80, 40, RED);
    textures[TEX_ENEMY_SINE]     = loadTextureOrFallback("enemy_sine_sheet.png",      80, 40, GREEN);
    textures[TEX_ENEMY_ZIGZAG]   = loadTextureOrFallback("enemy_zigzag_sheet.png",    80, 40, PURPLE);
    textures[TEX_EXPLOSION]      = loadTextureOrFallback("explosion_sheet.png",      288, 48, ORANGE);
    textures[TEX_BULLET_PLAYER]  = loadTextureOrFallback("bullet_player.png",          6, 16, YELLOW);
    textures[TEX_BULLET_ENEMY]   = loadTextureOrFallback("bullet_enemy.png",           6, 12, PINK);
    textures[TEX_POTION]         = loadTextureOrFallback("potion.png",                24, 24, RED);

    // Sonidos (solo si la PC tiene salida de audio)
    const char* soundFiles[SOUND_COUNT] = {
        "shoot.wav", "explosion.wav", "hit.wav", "potion.wav", "wave.wav"
    };
    for (int i = 0; i < SOUND_COUNT; i++)
    {
        soundLoaded[i] = false;
        const std::string fullPath = assetsPath + soundFiles[i];
        if (IsAudioDeviceReady() && FileExists(fullPath.c_str()))
        {
            sounds[i] = LoadSound(fullPath.c_str());
            soundLoaded[i] = IsSoundValid(sounds[i]);
            if (soundLoaded[i]) SetSoundVolume(sounds[i], 0.45f);
        }
    }

    loaded = true;
}

void AssetManager::unloadAll()
{
    if (!loaded) return;

    for (int i = 0; i < TEXTURE_COUNT; i++)
    {
        if (textures[i].id != 0) UnloadTexture(textures[i]);
        textures[i] = Texture2D{};
    }
    for (int i = 0; i < SOUND_COUNT; i++)
    {
        if (soundLoaded[i]) UnloadSound(sounds[i]);
        soundLoaded[i] = false;
    }
    loaded = false;
}

const Texture2D* AssetManager::getTexture(TextureId id) const
{
    return &textures[id];
}

void AssetManager::playSound(SoundId id) const
{
    if (soundLoaded[id])
        PlaySound(sounds[id]);
}
