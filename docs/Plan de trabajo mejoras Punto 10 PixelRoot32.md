# Plan de trabajo por fases – Mejoras del Punto 10 (PixelRoot32, ESP32)

Este plan organiza en fases las mejoras propuestas en el punto 10, priorizando **ROI alto**, bajo impacto en CPU/RAM del ESP32 y manteniendo la identidad del motor como **retro, eficiente y determinista**.

---

## Fase 0 – Fundaciones y criterios de éxito

**Objetivos**
- Definir objetivos medibles de rendimiento y memoria para ESP32 (FPS, uso máximo de RAM, tamaño de binario).
- Alinear estilo y restricciones: sin asignaciones en el Game Loop salvo a través de pools/arenas.

**Entregables**
- Lista de métricas objetivo (ej.: 60 FPS estables, <80% RAM interna usada por el motor+juego tipo).
- Pequeña guía interna: “qué está permitido y qué no” en cuanto a asignaciones, uso de floats, formatos de sprites.

**Notas ESP32**
- Validar objetivos con un juego ejemplo (Space Invaders u otro) como benchmark base.

---

## Fase 1 – Memoria y entidades críticas (COMPLETADA)

Foco en las mejoras con mayor impacto inmediato en **estabilidad y memoria**, sin cambiar todavía el modelo visual.

**Incluye**
- Object pooling para entidades de alta rotación.
- Primeros pasos hacia arena allocators (aún opcionales).

**Tareas**
- Identificar tipos de entidades de alta rotación (proyectiles, partículas, enemigos pequeños).
- Diseñar una API mínima de pool que respete las restricciones del estilo actual (sin nuevas dependencias pesadas).
- Reemplazar en un ejemplo de referencia las creaciones/destrucciones dinámicas por obtención/liberación desde el pool.
- Definir estructura de arena por escena (reserva al inicio, sin frees hasta salir de la escena), pero dejarla desactivada por defecto mediante macro de compilación.

**Criterios de éxito**
- El ejemplo de referencia no realiza nuevas asignaciones durante el Game Loop (verificado con logs o herramientas de profiling).
- Sin regresiones en FPS ni en tamaño de binario apreciables.

---

## Fase 2 – Render: capas lógicas y tilemaps (COMPLETADA)

Fase focalizada en mejorar la **escalabilidad visual** sin romper el coste de CPU.

**Incluye**
- Capas lógicas de render.
- Sistema básico de tilemaps.

**Tareas**
- Definir un modelo de capas simple (por ejemplo: fondo, entidades, UI) basado en listas ordenadas, sin z-buffer.
- Ajustar el pipeline de render para iterar por capas en lugar de un único listado plano.
- Diseñar una estructura de tilemap para fondos (tiles 8x8/8x16) con datos compactos, manteniendo el formato de sprite 1bpp.
- Integrar un ejemplo simple de fondo basado en tiles en uno de los juegos de ejemplo.

**Criterios de éxito**
- El cambio a capas no introduce caídas de FPS apreciables en ESP32.
- Un fondo tileado sencillo consume menos RAM que un bitmap equivalente y no rompe el tiempo de dibujo por frame.

---

## Fase 3 – Física: sweep tests y primitivas ligeras

Una vez estabilizados memoria y render, se mejora la **robustez de colisiones** sin introducir un motor pesado.

**Incluye**
- Sweep tests simples.
- Primitivas físicas adicionales ligeras (círculos, segmentos).

**Tareas**
- Implementar pruebas de colisión “barriendo” entre la posición anterior y la nueva para proyectiles rápidos, con un número limitado de checks adicionales.
- Añadir soporte interno para primitivas círculo y segmento con aritmética simple (floats, multiplicaciones), evitando funciones trigonométricas pesadas en el hot path.
- Exponer una API mínima para que el usuario pueda optar por estas primitivas solo en entidades que realmente lo requieran.
- Validar con un ejemplo práctico (por ejemplo, disparos muy rápidos o láseres) que no se producen colisiones perdidas (“tunneling”).

**Criterios de éxito**
- Sin regresión en FPS en escenarios típicos.
- Mejora perceptible en precisión de colisiones para los casos que usan sweep tests.

---

## Fase 4 – Modos visuales avanzados (>1bpp) y consolidación de arenas

Fase opcional y de mayor riesgo, pensada para proyectos que necesiten **más expresividad visual** y un control aún más fino de la memoria.

**Incluye**
- Modos de sprite >1bpp opcionales.
- Activación y consolidación de arena allocators.

**Tareas**
- Definir formatos de sprites 2bpp/4bpp como opciones de compilación, manteniendo 1bpp como modo por defecto del motor.
- Medir impacto en RAM y ancho de banda de dibujo para distintos tamaños de sprite y resoluciones de pantalla.
- Activar las arenas de memoria por defecto en uno de los ejemplos y medir estabilidad y fragmentación a largo plazo.
- Documentar casos de uso recomendados para modos >1bpp (pantallas estáticas, UIs, juegos con pocos sprites grandes).

**Criterios de éxito**
- El motor sigue cumpliendo los objetivos de FPS y uso de RAM definidos en la Fase 0 en el modo estándar (1bpp, sin arenas obligatorias).
- Los modos avanzados y arenas pueden activarse selectivamente por proyecto sin cambios intrusivos en la API de alto nivel.

---

## Fase 5 – Revisión y limpieza

**Objetivos**
- Revisar la coherencia de las nuevas features con la identidad del motor.
- Evitar “overengineering estilo engine de PC” y abstractions que oculten el coste real al usuario.

**Tareas**
- Revisar la documentación de las nuevas opciones (pools, arenas, tilemaps, primitivas físicas y modos visuales) resaltando costes y beneficios.
- Eliminar APIs experimentales que no hayan demostrado valor en benchmarks reales sobre ESP32.
- Congelar una versión “LTS” del motor para proyectos que busquen máxima estabilidad.

**Resultado esperado**
- Un PixelRoot32 Engine reforzado en su papel de motor embebido retro, eficiente y determinista, con mejoras introducidas de forma incremental y controlada.

