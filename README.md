# Videojuego en el Espacio (Space Shooter)

Videojuego de disparos espacial en 2D hecho en **C++20** con la librería **raylib 5.5.0**. Proyecto del curso *Programación de Videojuegos con C++* (UTP).

## Cómo jugar
Controlas una nave que se mueve de lado a lado y dispara hacia arriba contra oleadas de naves enemigas que bajan con distintos patrones. Si te quedas sin vidas, termina la partida.

| Acción | Teclado | Mando |
|---|---|---|
| Mover | A / D o flechas | Stick o cruceta |
| Disparar | ESPACIO | Botón A |
| Pausa | P | START |
| Iniciar / reiniciar | ENTER o R | START |
| Volver al menú | M | |

## Enemigos
| Nave | Movimiento | Puntos |
|---|---|---|
| Roja | Recta | 100 |
| Verde | Sinusoidal | 150 |
| Morada | Zig-zag | 200 |

Con la vida en 60 % o menos caen pociones que recuperan el 20 % de la vida.

## Estructura modular
El código está dividido en **módulos**. Cada módulo es una carpeta con una sola responsabilidad; su interfaz está en los `.h` y su implementación en los `.cpp`.

```
Video Juego en el Espacio/
├── main.cpp        Punto de entrada
├── core/           Núcleo: Game (Game Loop y estados) y GameState
├── input/          Entrada: InputManager (teclado y mando)
├── entities/       Entidades: Entity, Player, Enemy, Projectile, Potion, Explosion, GameWorld
├── movement/       Movimiento: MovementStrategy (recto, sinusoidal, zig-zag)
├── systems/        Reglas del juego: WaveManager, CombatSystem, PotionSystem, CollisionSystem
├── ui/             Interfaz: HUDManager (HUD, menú, pausa, game over)
├── graphics/       Gráficos: Animation (sprite sheets) y Starfield (fondo)
├── resources/      Recursos: AssetManager (imágenes y sonidos)
├── common/         Común: Config, ObjectPool y Random
└── assets/         8 imágenes PNG y 5 sonidos WAV
```

| Módulo | Qué hace | Depende de |
|---|---|---|
| `core` | Ejecuta el Game Loop, cambia de estado y coordina a los demás módulos | todos |
| `input` | Lee teclado y mando y los traduce a acciones (mover, disparar, pausar) | `common` |
| `entities` | Define los objetos del juego y el mundo que los agrupa (`GameWorld`) | `common`, `graphics`, `movement`, `resources` |
| `movement` | Algoritmos de movimiento de las naves | `common` |
| `systems` | Oleadas, disparos e IA, pociones y colisiones | `common`, `resources`, `entities` |
| `ui` | Dibuja el HUD y las pantallas | `common`, `resources`, `entities`, `systems` |
| `graphics` | Animación de sprites y fondo de estrellas | `common` |
| `resources` | Carga las imágenes y los sonidos una sola vez | |
| `common` | Parámetros del juego y utilidades compartidas | |

Las dependencias van en una sola dirección (de `core` hacia `common`): ningún módulo de abajo conoce a los de arriba. Por eso se puede cambiar un módulo sin tocar los demás; por ejemplo, para cambiar una tecla solo se edita `input/InputManager.cpp`.

Cada fotograma, `Game` le pide a cada módulo su parte:

1. `InputManager` lee los controles.
2. `WaveManager` crea las naves de la oleada.
3. `GameWorld` actualiza las entidades.
4. `CombatSystem` resuelve los disparos y la IA enemiga.
5. `PotionSystem` decide si cae una poción.
6. `CollisionSystem` resuelve los choques.
7. `GameWorld` y `HUDManager` dibujan la escena y la interfaz.

## Diseño del código
- **Programación modular:** nueve módulos con una responsabilidad cada uno y dependencias en una sola dirección.
- **POO:** clase base abstracta `Entity`; de ella heredan `Player`, `Enemy`, `Projectile`, `Potion` y `Explosion`.
- **Patrón Strategy:** `MovementStrategy` con `StraightMove`, `SinusoidalMove` y `ZigZagMove`.
- **Object Pool:** `ObjectPool<T>` reutiliza balas, naves, pociones y explosiones, sin `new` ni `delete` durante la partida.
- **Singleton:** `AssetManager` carga las imágenes y los sonidos una sola vez.
- **Game Loop** a 60 FPS con delta time y estados: Menú, Juego, Pausa y Game Over.
- Oleadas con dificultad creciente (`WaveManager`), colisiones con Bounding Boxes y HUD en tiempo real.

## Cómo ejecutarlo
1. Instala **Visual Studio** con la carga de trabajo *Desarrollo para el escritorio con C++*.
2. Clona el repositorio:
   ```
   git clone https://github.com/Alexnder96/VideoJuegoEnElEspacio.git
   ```
3. Abre `Video Juego en el Espacio.slnx`. Visual Studio descarga raylib solo (paquete NuGet).
4. Elige la plataforma **x64** y pulsa **F5**.

La carpeta `assets` se encuentra con ruta relativa; no hay que configurar nada más.

## Equipo
- Willy Alexander Pino Acuña
- Andrés Lozano Pérez
- C. Giovanni Castañeda León
- Jhair Alonso Alvarado Rios

Docente: Ing. Bryan Luis Solano Condor
