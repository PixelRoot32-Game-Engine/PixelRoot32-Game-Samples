# Plan de trabajo por fases – Soporte plataformas laterales tipo NES (PixelRoot32)

Este plan define pasos incrementales para que PixelRoot32 soporte cómodamente juegos de plataformas laterales estilo NES (tipo Mario), con scroll horizontal basado en tilemaps.

Se centra en:
- Mantener las restricciones de ESP32 (RAM, ancho de banda, 60 FPS).
- Reutilizar las piezas actuales del motor (Renderer, tilemaps, PhysicsActor).
- Evitar sobreingeniería (no “engine de PC”).

---

## Fase 0 – Alcance y restricciones (COMPLETADA)

**Objetivos**
- Definir qué queremos soportar exactamente:
  - Scroll horizontal continuo en niveles 2D basados en tiles.
  - Protagonista con salto, colisión con suelo/techo/laterales y enemigos simples.
- Fijar parámetros objetivo en ESP32:
  - Resolución lógica (ej. 160x120 o 200x150 para dejar margen de rendimiento).
  - FPS objetivo (idealmente 60 estables).

**Tareas**
- Elegir resolución lógica concreta para “modo plataformas” y documentarla.
- Validar coste de dibujar un tilemap a pantalla completa a esa resolución en ESP32.
- Confirmar que PhysicsActor y el sistema de colisiones actuales son suficientes para un primer plataformas sin rampas.

**Resultado**
- Una “configuración de referencia” para plataformas (resolución, FPS, límites de actores) que guíe las fases siguientes.

**Configuración de referencia acordada**
- Resolución lógica modo plataformas: **160x120** píxeles (relación 4:3), centrada dentro del framebuffer físico típico de 240x240 mediante `DisplayConfig`.
- Tilemap de fondo a pantalla completa: tiles **8x8** (20x15 tiles visibles → 300 tiles por frame), usando `Renderer::drawTileMap` en 1bpp.
- Objetivo de rendimiento: **60 FPS** estables en ESP32 para un nivel lateral razonable (unas pocas “pantallas” encadenadas) con fondo tileado completo.
- Colisiones: el stack actual (`Entity` + `Actor` + `PhysicsActor` + `CollisionSystem` con AABB y máscaras) es suficiente para un primer plataformas sin rampas, definiendo la lógica específica de plataformas sobre `PhysicsActor` en fases posteriores (p.ej. `PlatformerActor`).

---

## Fase 1 – Cámara 2D y scroll básico (COMPLETADA)

**Objetivos**
- Introducir un concepto claro de cámara 2D sobre el Renderer actual.
- Permitir scroll horizontal suave usando `setDisplayOffset` sin romper los ejemplos existentes.

**Tareas**
- Diseñar una pequeña abstracción de cámara (por ejemplo, una clase ligera `Camera2D` en el juego o en el motor) que:
  - Tenga posición `cameraX`/`cameraY`.
  - Exponga métodos para seguir al jugador con límites (clamp dentro del nivel).
- Integrar la cámara en un ejemplo de prueba (puede ser un nivel de prueba vacío con un jugador moviéndose sobre un fondo fijo).
- Conectar la cámara con Renderer:
  - Aplicar `setDisplayOffset(-cameraX, -cameraY)` antes de dibujar.
- Verificar que el resto de escenas (Pong, Space Invaders, etc.) siguen funcionando sin usar la cámara (opcional).

**Criterios de éxito**
- Scroll horizontal suave en un escenario sencillo.
- La cámara puede seguir al jugador en X sin jitter visible.

---

## Fase 2 – Tilemaps para niveles largos (COMPLETADA)

**Objetivos**
- Pasar de fondos pequeños a niveles laterales más largos basados en tiles.
- Mantener el consumo de RAM bajo control.

**Tareas**
- Definir un formato de tilemap para plataformas:
  - Tiles 8x8 u 8x16 (reutilizando el sistema actual).
  - Mapa más ancho que la pantalla (por ejemplo, varias “pantallas” encadenadas).
- Implementar en un ejemplo:
  - Nivel de prueba con varias secciones horizontales.
  - Dibujo del tilemap con scroll usando la cámara de la Fase 1.
- Evaluar opciones para gestionar niveles largos:
  - Opción A: mapa completo en RAM (para niveles moderados).
  - Opción B (más adelante): estructura de “chunks” o secciones para no cargar todo a la vez.

**Criterios de éxito**
- Un nivel lateral más ancho que la pantalla dibujado correctamente con scroll.
- Sin caídas de FPS apreciables en ESP32 para un tamaño de nivel razonable.

**Notas de implementación**
- Formato elegido: tilemap 1bpp con tiles de **8x8** píxeles (`TileMap` de `Renderer`), manteniendo índices `uint8_t` y reutilizando sprites desde un pequeño array de tiles (`PLATFORMER_TILES`).
- Ejemplo de referencia: la escena **CameraDemo** ahora usa un nivel lateral basado en tilemap (tres “pantallas” de ancho) con scroll horizontal controlado por la cámara de la Fase 1 y dibujado mediante `Renderer::drawTileMap`.
- Gestión de niveles largos: se adopta por ahora la **Opción A (mapa completo en RAM)** para niveles moderados; la **Opción B (chunks/secciones)** se deja como extensión futura para proyectos que necesiten mapas mucho más extensos.

---

## Fase 3 – Físicas de plataformas básicas (COMPLETADA)

**Objetivos**
- Construir sobre `PhysicsActor` una lógica específica de plataformas:
  - Gravedad, salto, frenado horizontal sencillo.
  - Colisión sólida con el suelo, plataformas y paredes.

**Tareas**
- Definir un `PlatformerActor` (o patrón similar) que:
  - Use AABB para detectar suelo/techo/laterales.
  - Tenga estados simples (en suelo, en caída, salto, golpe con techo).
  - Evite penetraciones visibles mediante un paso de resolución simple (por eje).
- Integrar al protagonista en el nivel tileado de la Fase 2:
  - Colisión con tiles “sólidos” vs tiles decorativos.
  - Soporte de plataformas escalonadas tipo “bloques” (sin rampas todavía).
- Ajustar constantes (gravedad, velocidad, altura de salto) para que la jugabilidad se sienta “arcade” pero amigable a 60 FPS.

**Criterios de éxito**
- Protagonista puede correr y saltar por el nivel sin quedarse atascado en esquinas.
- No se observan “tunneling” ni saltos inconsistentes a FPS objetivo.

**Notas de implementación**
- Se ha creado un `PlatformerActor` ligero derivado de `PhysicsActor` que:
  - Aplica gravedad constante, entrada horizontal (`moveDir`) y salto discreto (`jumpVelocity`).
  - Resuelve colisiones contra el tilemap por ejes (X luego Y) usando AABB sobre tiles sólidos.
  - Implementa `getHitBox()` sobre `Rect` para integrarse con el sistema de colisiones existente, aunque en esta fase solo se usa contra el tilemap.
- En la escena **CameraDemo**:
  - El rectángulo amarillo ahora es un `PlatformerActor` con físicas de plataformas básicas (correr y saltar sobre suelo y plataformas del tilemap).
  - La cámara de la Fase 1 sigue al centro del actor, manteniendo el scroll suave mientras se desplaza por el nivel lateral.

---

## Fase 4 – Mejora de experiencia y rendimiento en scroll (COMPLETADA)

**Objetivos**
- Afinar la experiencia visual y de rendimiento del scroll lateral.
- Introducir mejoras opcionales como parallax ligero y optimizaciones de dibujo.

**Tareas**
- Añadir soporte simple de parallax:
  - Una o dos capas de fondo que se muevan más despacio que la capa principal.
  - Mantener el coste de dibujo controlado (fondos más simples o resoluciones menores).
- Evaluar optimizaciones en el dibujado de tilemaps:
  - Medir si merece la pena limitar el dibujado solo a la región visible (si no se hace ya).
  - Considerar buffers o trucos simples si el ancho del nivel empieza a ser grande.
- Ajustar la cámara para:
  - Tener zonas muertas horizontales (el jugador se mueve un poco antes de desplazar la cámara).
  - Evitar vibraciones en el scroll.

**Criterios de éxito**
- Scroll suave con parallax opcional sin romper los objetivos de FPS.
- Jugabilidad estable y sin saltos en el tiempo de respuesta.

**Notas de implementación**
- Parallax simple añadido en **CameraDemo**:
  - Dos capas de fondo dibujadas con offsets de cámara escalados (`0.4x` y `0.7x`) antes de la capa principal.
  - La capa de juego (tilemap de plataformas + jugador) usa el offset completo (`1.0x`) para mantener la jugabilidad.
- El scroll lateral se mantiene suave gracias a la `Camera2D` con zona muerta horizontal y al ajuste de `drawSprite`/`drawTileMap` para respetar correctamente el `setDisplayOffset`.

---

## Fase 5 – Extensiones opcionales (RAMPAS, verticalidad, enemigos)

**Objetivos**
- Añadir, de forma opcional, features que acerquen más a un Mario NES completo.
- Mantener estas features como “opt-in” para no complicar el motor base.

**Tareas**
- RAMPAS / suelos inclinados:
  - Investigar si se pueden representar con segmentos (`physics::Segment`) ligeros.
  - Proveer una solución mínima para rampas sencillas sin convertir el motor en un Box2D.
- Scroll vertical opcional:
  - Extender la cámara para soportar también movimiento vertical (niveles con pequeñas secciones hacia arriba/abajo).
- Enemigos de plataformas:
  - Definir un patrón simple para enemigos que caminan sobre el terreno, caen por huecos y causan daño por contacto.

**Notas de implementación**
- Enemigos de plataformas:
  - Prototipo de enemigo lateral implementado en **CameraDemo** mediante la clase `PlatformerEnemy`, derivada de `PhysicsActor`.
  - El enemigo patrulla sobre el mismo tilemap que el jugador, aplicando gravedad y colisión simple con tiles. Al contactar con el jugador, este se reinicia al punto de inicio y la cámara vuelve a `x = 0`.
- Scroll vertical opcional:
  - La clase `Camera2D` del motor se ha extendido con límites verticales (`setVerticalBounds`) y seguimiento opcional en Y (`followTarget(x, y)`), manteniendo compatibilidad con escenas existentes que solo usan X.
  - **CameraDemo** utiliza actualmente solo el desplazamiento horizontal, pero la API está lista para niveles con pequeñas variaciones verticales.
- Rampas / suelos inclinados:
  - Se ha implementado un prototipo ligero de rampas en **CameraDemo**: un conjunto de tiles marcados como rampa se trata, a nivel de físicas en `PlatformerActor`, como una única pendiente continua definida sobre el tilemap.
  - El cálculo usa únicamente aritmética sencilla (interpolación lineal en X/Y) y se mantiene como feature opcional y experimental; el mini juego tipo Mario 1‑1 planificado en la Fase 6 no depende de rampas para su jugabilidad básica.

**Criterios de éxito**
- El motor puede soportar un prototipo de plataformas lateral con:
  - Niveles más largos, algunos cambios de altura y enemigos básicos.
- Todas estas features se mantienen opcionales y no rompen juegos existentes que no las usan.

---

## Fase 6 – Mini juego de un solo nivel (prototipo tipo Mario 1-1)

**Objetivos**
- Validar de extremo a extremo las funcionalidades implementadas en las fases anteriores con un mini juego completo.
- Construir un nivel lateral único, inspirado en el diseño del mundo 1-1 de Super Mario Bros (NES), adaptado a las limitaciones de PixelRoot32 y ESP32.

**Tareas**
- Diseño del nivel:
  - Crear un tilemap para un único nivel lateral que combine:
    - Secciones planas, pequeños desniveles y plataformas.
    - Algunos “bloques especiales” (por ejemplo, bloques que dan puntos o power-ups simples).
  - Mantener la longitud del nivel dentro de los límites razonables para RAM y rendimiento definidos en la Fase 0.
- Protagonista:
  - Implementar un personaje jugable basado en `PlatformerActor` con:
    - Carreras, saltos y colisiones con el terreno ya ajustadas en Fase 3.
    - Un esquema de control simple (mover izquierda/derecha, saltar).
- Enemigos y elementos interactivos:
  - Añadir al menos 1–2 tipos de enemigos básicos que patrullen plataformas y causen daño por contacto.
  - Incluir elementos coleccionables (monedas, puntos) y una meta clara de fin de nivel (bandera, puerta, etc.).
- Scroll y experiencia visual:
  - Usar la cámara de Fase 1 para seguir al protagonista a lo largo de todo el nivel.
  - Integrar, si es posible, una capa de parallax simple definida en Fase 4.
- Validación en hardware:
  - Ejecutar el mini juego en ESP32 con la resolución objetivo.
  - Medir FPS aproximado y uso de RAM para confirmar que se mantienen dentro de los objetivos.

**Secciones inspiradas en el mapa 1-1 (adaptadas)**
- Sección A – Inicio plano:
  - Suelo recto con algunos bloques/bloques especiales bajos para introducir el salto.
- Sección B – Tuberías:
  - Varias tuberías de distinta altura que cortan el camino y obligan a saltar.
- Sección C – Plataformas flotantes:
  - Bloques elevados aislados que requieren saltos más precisos.
- Sección D – Escaleras de bloques:
  - Estructuras de bloques en forma de escalera para validar subidas y bajadas por escalones.
- Sección E – Meta:
  - Gran escalera final y un tile/estructura de “bandera” o “castillo” que marca el fin del nivel.

**Criterios de éxito**
- El mini juego se puede jugar de principio a fin en un solo nivel lateral, con scroll suave, físicas de plataformas estables y enemigos básicos.
- No se observan caídas graves de rendimiento ni problemas de memoria en ESP32.
- El ejercicio sirve como referencia práctica y de validación para futuros juegos de plataformas construidos sobre PixelRoot32.
