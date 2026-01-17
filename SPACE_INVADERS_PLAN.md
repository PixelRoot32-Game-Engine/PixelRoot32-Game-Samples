# Plan de Trabajo: Space Invaders (PixelRoot32 Engine)

Este plan adapta la lógica original de Space Invaders a la arquitectura del motor PixelRoot32, siguiendo los patrones de los ejemplos existentes (TicTacToe, Snake).

## Fase 1: Arquitectura de Escena y Datos (COMPLETADA)
- **Creación de Archivos:**
  - `src/examples/SpaceInvaders/SpaceInvadersScene.h`: Clase heredada de `pixelroot32::core::Scene`.
  - `src/examples/SpaceInvaders/SpaceInvadersScene.cpp`: Implementación principal.
  - `src/examples/SpaceInvaders/GameConstants.h`: Constantes de juego y físicas.
- **Estructuras de Datos:**
  - `Alien`, `Player`, `Projectile`, `Bunker` (definidos como structs simples o clases ligeras).
- **Integración con Motor:**
  - Implementar `init()`, `update(unsigned long deltaTime)`, `draw(Renderer& renderer)`.
  - Configurar acceso a `engine` (Input, Audio, Renderer).

## Fase 2: Sistema de Horda y Renderizado (COMPLETADA)
- **Renderizado "Pixel Art":**
  - Implementar rutinas de dibujado usando primitivas (`drawFilledRectangle`) para simular los sprites de los aliens (Squid, Crab, Octopus).
- **Movimiento Discreto (Stepping):**
  - Implementar temporizador para el movimiento de la horda (el "latido").
  - Lógica de cambio de dirección y descenso al tocar bordes.
  - Aceleración del ritmo conforme mueren enemigos.

## Fase 3: Combate y Colisiones
- **Input:**
  - Movimiento lateral del jugador.
  - Disparo (botón de acción).
- **Proyectiles:**
  - Gestión de proyectiles activos (pool o vector).
  - Disparo de enemigos (aleatorio desde la fila inferior).
- **Colisiones:**
  - Sistema AABB simple para Proyectil vs Alien/Player/Bunker.

## Fase 4: Audio (SFX y Música)
- **Música de Fondo:**
  - Implementar el patrón de 4 notas descendentes que se acelera, usando `MusicTrack` y `MusicNote`.
- **Efectos de Sonido (SFX):**
  - Disparo jugador (onda pulso/triángulo).
  - Explosión alien (ruido blanco).
  - U.F.O. (si se implementa).

## Fase 5: Ciclo de Juego Completo
- **Estados de Juego:**
  - Menú / "Press Start".
  - Playing.
  - Game Over / Win.
- **Puntuación y Vidas:**
  - HUD simple para Score y Vidas.
  - Persistencia de High Score (si el motor lo permite, o solo en memoria).
