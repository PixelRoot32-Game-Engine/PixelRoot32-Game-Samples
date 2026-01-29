# Optimizaciones FPS – Metroidvania en ESP32

Objetivo: subir de ~14 FPS estables a ~20 FPS sin cambiar la jugabilidad ni la arquitectura del juego.

## Contexto

- **Motor**: PixelRoot32 con colisión por tiles (O(1) por celda), ya optimizada según `COLLISION_LAYERS_IMPLEMENTATION_PLAN.md`.
- **Pantalla**: 240×240, mapa 30×30 tiles de 8×8 px (3 capas: background, platforms, stairs).
- **Cuello de botella principal**: tiempo de dibujado (3 tilemaps + jugador) y lecturas desde Flash (PROGMEM) en colisión y render.

---

## 1. Implementada: caché de escaleras en RAM (PlayerActor)

**Qué hace**: En `init` se construye una máscara de bits en RAM (1 bit por celda de escalera) a partir de `stairsIndices` (PROGMEM). Todas las comprobaciones de “¿hay escalera aquí?” leen de RAM en lugar de Flash.

**Dónde**: `PlayerActor::buildStairsCache()`, llamado desde `MetroidvaniaScene::init()` tras `setStairs()`.

**Impacto esperado**: Menos lecturas a Flash en `isOverlappingStairs()` y en el bucle de colisión vertical (hasLadder). En ESP32, Flash es más lento que RAM; puede aportar 1–2 FPS según escena.

**Coste**: ~113 bytes de RAM (30×30 bits) y un único barrido de la capa de escaleras al iniciar.

---

## 2. Build y configuración (PlatformIO)

- **Optimización de código**: En `platformio.ini`, env `esp32dev`, añadir:
  ```ini
  build_flags = 
    ... 
    -O2
  ```
  Si ya usas `-Os` por tamaño, probar `-O2` solo para el target de FPS; suele dar mejor velocidad.

- **FPS overlay**: Si no necesitas el contador en pantalla en release, quitar `-D PIXELROOT32_ENABLE_FPS_DISPLAY` del env de ESP32. Evita `drawText` cada frame (o cada N frames cuando se actualiza el número). Ganancia pequeña pero sin tocar lógica del juego.

- **Frecuencia CPU**: Comprobar que la placa esté a 240 MHz (por defecto en muchas ESP32). En `platformio.ini`:
  ```ini
  board_build.f_cpu = 240000000L
  ```

- **SPI de la pantalla**: Tienes `SPI_FREQUENCY=40000000`. Si el display y el cableado lo permiten, probar 80 MHz puede reducir tiempo de envío al TFT (revisar datasheet del panel). Cualquier ganancia aquí se nota en todo el dibujado.

---

## 3. Juego (sin tocar motor)

- **Animación del jugador**: `ANIMATION_FRAME_TIME_MS = 120` en `GameConstants.h`. Subir a 150–160 ms reduce la frecuencia de cambio de frame y de decisiones de estado; el movimiento se ve casi igual. Ahorro pequeño pero sin impacto en mecánicas.

- **Evitar trabajo innecesario en `update()`**: La lógica de colisión ya está acotada (pocas celdas por frame). Mantener así; no añadir más entidades o comprobaciones en el bucle principal.

---

## 4. Motor / Renderer (si puedes tocar el core)

- **Viewport culling**: El `drawTileMap` 4bpp del motor ya hace culling por viewport; con mapa 240×240 a pantalla completa se dibujan todos los tiles visibles. No hay margen extra ahí sin cambiar tamaño de mapa o cámara.

- **Una sola pasada para varias capas**: Hoy se hacen 3 llamadas a `drawTileMap` (background, platforms, stairs). Una optimización más invasiva sería un “tilemap compuesto” que, por cada celda visible, elija el tile de la capa superior no vacía y dibuje una sola vez. Requeriría cambios en el editor y en la estructura de datos; no se considera “no invasiva” para este documento.

- **Datos en RAM para el frame**: Los `indices` de cada capa están en PROGMEM. Copiar solo la ventana visible de índices a un buffer en RAM cada frame (o cuando la cámara se mueve) reduciría lecturas desde Flash durante el dibujado, a costa de más RAM y lógica en el Renderer.

---

## 5. Resumen de acciones recomendadas (por prioridad)

| Prioridad | Acción | Invasividad | Impacto esperado |
|-----------|--------|-------------|------------------|
| 1 | Caché de escaleras en RAM (ya hecha) | Baja | 1–2 FPS |
| 2 | Añadir `-O2` para ESP32 | Ninguna | Variable, a menudo 1–3 FPS |
| 3 | Quitar FPS overlay en build de release | Ninguna | Pequeño |
| 4 | Aumentar `ANIMATION_FRAME_TIME_MS` a 150 | Baja | Pequeño |
| 5 | Comprobar CPU 240 MHz y, si aplica, SPI 80 MHz | Ninguna / baja | Variable |

Combinando 1 + 2 + 3 (+ 4 si quieres) es razonable apuntar a unos 18–20 FPS estables; si el cuello de botella sigue en el TFT, la siguiente vía sería probar SPI más alto o reducción de resolución/área dibujada (más invasivo).
