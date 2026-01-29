# Plan de Optimización para ESP32 - Metroidvania

Este documento detalla las fases para mejorar el rendimiento (FPS) del ejemplo Metroidvania en hardware ESP32, abordando los cuellos de botella identificados en el análisis de colisiones y renderizado.

## Fase 1: Optimización de Colisiones (Ganancia Inmediata)
*Objetivo: Eliminar el costo O(N) de iterar sobre listas de rectángulos.*

1.  **Migración a Tile-Based Collision:**
    *   Sustituir el bucle de `platformRects` en `PlayerActor::update` por una comprobación directa de los tiles circundantes al jugador.
    *   Implementar una función auxiliar `getTileAt(x, y)` que devuelva el índice del tile en O(1).
2.  **Unificación de Detección de Escaleras:**
    *   Consolidar la lógica de `isOverlappingStairs` para que se use el mismo resultado en la resolución de colisiones y en el cambio de estados, evitando accesos repetitivos al mapa de memoria.
3.  **Simplificación de Hitboxes:**
    *   Reducir el número de puntos de comprobación vertical de 4 a 2 (cabeza y pies) si es posible sin afectar el "feel" del juego.

## Fase 2: Optimización del Renderizado (Ganancia Mayor)
*Objetivo: Reducir la carga de procesamiento por pixel y llamadas innecesarias.*

1.  **Implementación de Viewport Culling:**
    *   Modificar `Renderer::drawTileMap` para que solo procese tiles dentro de las coordenadas visibles de la pantalla. Actualmente se dibujan 1200 tiles por capa independientemente de si son visibles.
2.  **Optimización de `drawSprite` (4bpp):**
    *   Optimizar el bucle interno de dibujado para reducir desplazamientos de bits.
    *   Minimizar llamadas a `drawPixel` mediante el uso de buffers de línea o ráfagas de datos si el driver lo permite.
3.  **Renderizado Condicional de Capas:**
    *   Evaluar si todas las capas (Background, Platforms, Details, Stairs) necesitan dibujarse en cada frame.

## Fase 3: Gestión de Escena y Memoria
*Objetivo: Estructurar mejor los datos para acceso rápido.*

1.  **Flattening de Capas Estáticas:**
    *   Combinar las capas de `Background` y `Details` en una sola capa de tiles pre-calculada para reducir el número de pasadas de renderizado.
2.  **Uso de Punto Fijo (Fixed-Point):**
    *   Sustituir cálculos críticos de física de `float` a aritmética de punto fijo para aliviar la carga de la FPU.

## Fase 4: Optimizaciones de Bajo Nivel (Hardware)
*Objetivo: Aprovechar al máximo las capacidades de la ESP32.*

1.  **Soporte DMA (Direct Memory Access):**
    *   Asegurar que el envío del buffer a la pantalla se realice mediante DMA para liberar ciclos de CPU.
2.  **Optimización de Memoria IRAM:**
    *   Marcar funciones críticas de renderizado con `IRAM_ATTR` para una ejecución más rápida desde la memoria interna.

---
*Nota: Se recomienda realizar pruebas de FPS después de completar cada fase para validar el impacto de los cambios.*
