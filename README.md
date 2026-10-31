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

## Diseño del código
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

La carpeta `assets` (8 imágenes PNG y 5 sonidos WAV) se encuentra con ruta relativa; no hay que configurar nada más.

## Equipo
- Willy Alexander Pino Acuña
- Andrés Lozano Pérez
- C. Giovanni Castañeda León
- Jhair Alonso Alvarado Rios

Docente: Ing. Bryan Luis Solano Condor
