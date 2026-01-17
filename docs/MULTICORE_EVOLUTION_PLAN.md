# 🚀 EDGE Engine: Plan de Evolución a Arquitectura Multi-Core (ESP32)

Este documento detalla la hoja de ruta técnica para transformar el motor **EDGE** (anteriormente PixelRoot32) de un ciclo único secuencial a una arquitectura paralela de alto rendimiento aprovechando el hardware dual-core del ESP32.

---

## 1. 🎯 Objetivo y Filosofía

El objetivo es desacoplar **Lógica** (Simulación) de **Renderizado** (Presentación) para alcanzar:

1. **FPS Estables:** La lógica compleja no debe frenar el dibujado.
2. **Determinismo:** La física corre a paso fijo independiente de los FPS visuales.
3. **Máximo Rendimiento:** Uso simultáneo de Core 0 y Core 1.

**Modelo propuesto:** *Producer-Consumer con Doble Buffer.*

---

## 2. 🏗️ Arquitectura Propuesta

### Distribución de Cores

| Core | Rol | Responsabilidades | Frecuencia |
| :--- | :--- | :--- | :--- |
| **Core 0** | **Lógica (Producer)** | Input, Física, IA, Actualización de Estado, Generación de `RenderPacket`. | Fija (ej. 60 TPS) |
| **Core 1** | **Render (Consumer)** | Limpieza de pantalla, Interpretación de `RenderPacket`, Comandos DMA (TFT_eSPI). | Variable (Max FPS) |

### Estructura de Datos Compartida: `RenderPacket`

Para evitar *Race Conditions* (condiciones de carrera) donde el render lee una posición mientras la física la modifica, **no compartiremos punteros directos a Entidades**. En su lugar, usaremos un sistema de **Snapshots** (instantáneas).

```cpp
struct RenderCommand {
    enum Type { RECT, CIRCLE, SPRITE, TEXT };
    Type type;
    int16_t x, y;
    uint16_t color;
    // ... otros datos visuales
};

struct RenderPacket {
    RenderCommand commands[MAX_COMMANDS]; // Array estático para evitar fragmentación
    uint16_t commandCount;
};
```

---

## 3. 📅 Plan de Implementación por Fases

### Fase 1: Desacoplamiento Lógico (Refactor Inicial)

*Antes de tocar hilos, debemos separar el "Qué dibujar" del "Cómo dibujar".*

1. **Eliminar `draw()` de las Entidades:** Las entidades ya no deben llamar a `renderer.drawRect()`.
2. **Crear `RenderQueue`:** Implementar una cola de comandos simple.
3. **Nuevo flujo en Single-Core:**
    * `Scene::update()` -> Actualiza posiciones.
    * `Scene::generateRenderCommands()` -> Llena la `RenderQueue`.
    * `Engine::draw()` -> Lee la `RenderQueue` y ejecuta los comandos gráficos.

> **Por qué:** Esto asegura que la lógica no dependa del renderizador inmediato y prepara el terreno para enviar esos datos a otro core.

### Fase 2: Implementación de Tasks FreeRTOS

*Levantar la infraestructura paralela sin activar el renderizado aún.*

1. **Crear `LogicTask` (Core 0):** Mover el bucle de `update()` a una tarea pinned al Core 0.
2. **Crear `RenderTask` (Core 1):** Mover el bucle de `draw()` a una tarea pinned al Core 1.
3. **Sincronización Básica:** Usar `xTaskNotify` o Semáforos para que Render espere a Lógica (aún secuencial pero en hilos distintos).

### Fase 3: Doble Buffering (Parallelism Real)

*Habilitar la ejecución simultánea.*

1. **Implementar Doble Buffer de `RenderPacket`:**
    * `Packet A` (Escribiendo Lógica)
    * `Packet B` (Leyendo Render)
2. **Swap Atómico:** Al terminar un frame lógico, intercambiar punteros.
3. **Mecanismo:**
    * Core 0 llena Buffer A.
    * Al finalizar, si Core 1 terminó Buffer B, intercambian.
    * Si Core 1 sigue ocupado, Core 0 puede esperar o saltar frame (frame skip).

### Fase 4: Optimización y DMA

1. **DMA Async:** Asegurar que `TFT_eSPI` use transferencias DMA no bloqueantes.
2. **Interpolación (Opcional):** Si la lógica va a 30Hz y Render a 60Hz, el Render puede interpolar posiciones entre dos Snapshots.

---

## 4. ⚠️ Riesgos Técnicos y Mitigación

### 1. Condiciones de Carrera (Race Conditions)

* **Riesgo:** Core 1 lee una posición `x` mientras Core 0 la sobrescribe.
* **Solución:** **NUNCA** acceder a `Entity` desde Core 1. Solo leer del `RenderPacket` que es inmutable durante el renderizado.

### 2. Fragmentación de Memoria

* **Riesgo:** Crear/destruir objetos `RenderCommand` dinámicamente cada frame colapsará el Heap del ESP32.
* **Solución:** Usar **Memory Pools** o Arrays Estáticos (`std::array` o C-style arrays) pre-reservados. Evitar `new`/`malloc` en el bucle principal.

### 3. Watchdog Timer (WDT) Reset

* **Riesgo:** Si una tarea acapara la CPU sin ceder control, el ESP32 se reinicia.
* **Solución:** Incluir `vTaskDelay(1)` o `yield()` en los bucles infinitos de las tareas para alimentar al Watchdog.

### 4. Bus Contention (Conflicto de Bus)

* **Riesgo:** Ambos cores intentando acceder a RAM/Flash intensivamente.
* **Solución:** Mantener los `RenderPacket` pequeños y compactos. Usar IRAM para código crítico si es posible.

---

## 5. 🛠️ Buenas Prácticas para Motores en Hardware Limitado

1. **Struct of Arrays (SoA) vs Array of Structs (AoS):** Para cache, a veces es mejor tener todos los `pos_x` juntos. Para simplicidad inicial, AoS (`Entity` objects) está bien, pero el `RenderPacket` debería ser plano.
2. **Fixed Point Math:** Si el FPU es cuello de botella (raro en ESP32, tiene FPU, pero bueno saberlo), usar matemáticas de punto fijo.
3. **Object Pooling:** Nunca hacer `new Bullet()` en tiempo de juego. Tener un pool de 100 balas y activarlas/desactivarlas.
4. **Dirty Flags:** Solo regenerar comandos de render para objetos que se movieron (avanzado, para UI estática).

---

## 6. 📝 Siguientes Pasos Inmediatos

Recomiendo comenzar con la **Fase 1 (Desacoplamiento Lógico)** en la rama actual antes de crear tareas.

1. Definir `struct RenderCommand` en un nuevo header.
2. Modificar `Entity::draw(Renderer&)` para que sea `Entity::getRenderCommand() -> RenderCommand`.
3. Adaptar `Scene::draw` para acumular comandos y luego ejecutarlos.
