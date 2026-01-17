# EDGE Engine: Plan de Integración del Sistema de Audio NES-like

Este documento define la hoja de ruta técnica para incorporar un sistema de audio sintético estilo NES al motor, manteniendo compatibilidad con ESP32 y con el backend nativo (SDL2) para pruebas en Windows.

El foco es lograr un subsistema de audio determinista, de bajo coste en CPU y RAM, que respete la arquitectura actual del motor (core, drivers, examples) y su guía de estilo.

---

## 1. Objetivo Global y Alcance

- Añadir un subsistema de audio con:
  - Cuatro canales fijos: dos pulse, uno triangle y uno noise.
  - Mezcla por software y salida mono.
  - Control de frecuencia, volumen, duración y parámetros básicos de forma de onda.
- Proporcionar una API de audio agnóstica de plataforma que:
  - Sea coherente con Engine, Renderer e InputManager.
  - Tenga una implementación para ESP32 (I2S/DAC/PWM) y otra para SDL2 en Windows.
- Mantener determinismo y simplicidad:
  - Sin emulación exacta de APU.
  - Sin librerías de audio complejas adicionales.
  - Sin afectar ni romper los juegos existentes en examples.

---

## 2. Fase 1 – Diseño del Subsistema de Audio y API Pública

### Objetivo

Definir la arquitectura del AudioSubsystem / AudioEngine y su interfaz pública, sin acoplarla aún a ningún backend concreto. El código debe compilar en todas las plataformas con una implementación “no-op” por defecto.

### Cambios arquitectónicos

- Introducir un nuevo módulo público de audio dentro del motor:
  - Nuevo espacio de nombres audio dentro de pixelroot32.
  - Encabezados públicos en include/audio para el subsistema.
- Definir las responsabilidades del AudioEngine:
  - Inicializar el sistema de audio de la plataforma.
  - Actualizar el estado de los canales en función del tiempo de juego.
  - Exponer un conjunto reducido de operaciones de alto nivel (por ejemplo: reproducir efectos cortos, notas y patrones).
- Definir una interfaz minimalista de backend de audio:
  - El backend se responsabiliza únicamente de aceptar muestras PCM ya mezcladas o de pedir a AudioEngine que las genere cuando lo necesite.
  - Debe ser posible tener una implementación “dummy” que no produzca sonido pero conserve el mismo flujo lógico.
- Decidir la integración con Engine:
  - Engine mantiene una referencia o miembro AudioEngine.
  - AudioEngine puede recibir deltaTime desde el bucle principal, pero la generación final de muestras se hará en un contexto de mayor frecuencia (callback/timer) descrito en fases posteriores.

### Componentes o módulos afectados

- include/core:
  - Engine: nueva dependencia hacia el subsistema de audio, manteniendo constructores compatibles con el código actual (añadir sobrecargas o parámetros opcionales).
- include/audio (nuevo):
  - Definiciones públicas del subsistema, tipos de canal, parámetros y configuración.
- src/core:
  - Implementación de la integración de Engine con AudioEngine (inicialización y actualización).
- API_REFERENCE.md:
  - Añadir una sección de alto nivel que describa la existencia del módulo de audio (no detallar todavía la API completa).

### Riesgos técnicos

- Riesgo de romper firmas de constructores existentes:
  - Mitigar manteniendo sobrecargas actuales y añadiendo nuevas variantes para audio.
- Riesgo de crecer demasiado la API:
  - Limitar la primera versión a un conjunto mínimo de operaciones centradas en efectos de sonido simples y patrones rítmicos cortos.

---

## 3. Fase 2 – Modelo Interno de Canales NES-like y Mezclador

### Objetivo

Definir la representación interna de los cuatro canales (dos pulse, triangle, noise) y el mezclador software, de forma que el núcleo de generación de audio sea completamente agnóstico del backend hardware.

### Cambios arquitectónicos

- Modelar los canales como estructuras de datos estáticas:
  - Estado por canal: frecuencia efectiva, fase/fase acumulada, volumen, parámetros de forma de onda (duty para pulse, comportamiento de triangle, parámetros del generador de ruido), banderas de activo y tiempo restante.
  - No usar asignación dinámica en tiempo de ejecución.
- Definir un mezclador de software mono:
  - Función interna que, dado un número fijo de muestras y un estado de los canales, genera un bloque de audio PCM.
  - Mezcla simple por suma y clamping o escalado fijo, usando aritmética entera o de punto fijo.
- Definir un modelo de tiempo interno:
  - Frecuencia de muestreo fija (por ejemplo, una frecuencia moderada que equilibre calidad y coste de CPU).
  - Paso de fase calculado una única vez por evento de cambio de frecuencia para cada canal.
  - Las duraciones se contabilizan en muestras o en unidades de “ticks de audio” derivadas de la frecuencia de muestreo.
- Añadir soporte básico de “notas” y “eventos”:
  - El juego emite eventos de alto nivel (por ejemplo, reproducir impulso corto en canal pulse, reproducir burst de ruido) que el AudioEngine traduce a cambios en el estado de los canales.

### Componentes o módulos afectados

- include/audio:
  - Definiciones de tipos de canal, parámetros de configuración por canal y API para disparar eventos de audio (sin detalles de implementación).
- src/audio (nuevo):
  - Implementación del núcleo de generación de forma de onda y mezcla.
  - Implementación del AudioEngine que manipula el estado de los canales y genera bloques de muestras para los backends.

### Riesgos técnicos

- Coste de CPU:
  - Riesgo de que la frecuencia de muestreo o el tamaño del bloque de mezcla sean demasiado exigentes para el ESP32.
  - Mitigar comenzando con una frecuencia moderada y bloques pequeños, midiendo el coste en ciclos antes de subir calidad.
- Calidad sonora limitada:
  - La mezcla y cuantización simple pueden introducir distorsión, pero se prioriza consistencia y baja carga de CPU sobre fidelidad perfecta.

---

## 4. Fase 3 – Backend de Audio para SDL2 (Windows)

### Objetivo

Implementar un backend de audio para la plataforma nativa basado en SDL2, reutilizando la infraestructura ya empleada para vídeo/eventos, con el fin de probar y depurar el núcleo de audio en escritorio.

### Cambios arquitectónicos

- Introducir un driver de audio nativo en el espacio de nombres de drivers:
  - Nuevo backend de audio en la rama native dentro del árbol de drivers.
  - El backend gestiona el dispositivo de audio de SDL2 y un buffer circular o mecanismo equivalente.
- Conectar el backend con AudioEngine:
  - El backend solicita bloques de muestras al AudioEngine en un callback de audio o en un bucle dedicado sincronizado con SDL2.
  - La lógica de mezcla y generación de onda sigue residiendo en el módulo de audio, no en el backend.
- Integración en el flujo de inicialización nativo:
  - Engine, al ejecutarse en plataforma nativa, crea e inicializa también el backend de audio SDL2.
  - Se asegura que el ciclo de vida de SDL2 audio se coordina con el del renderer SDL2.

### Componentes o módulos afectados

- include/drivers/native:
  - Definición de la interfaz pública mínima del backend de audio SDL2.
- src/drivers/native:
  - Implementación del backend SDL2, apertura/cierre del dispositivo y puente con AudioEngine.
- src/core:
  - Inicialización del backend en el flujo de arranque de Engine cuando se compila para plataforma nativa.
- examples:
  - Opcionalmente, añadir pruebas sencillas en escenas existentes para disparar sonidos básicos en respuesta a input, respetando la compatibilidad con builds sin audio.

### Riesgos técnicos

- Diferencias de timing entre SDL2 audio y el game loop:
  - El callback de audio ejecuta a frecuencia fija mientras el game loop tiene su propio deltaTime.
  - Mitigar manteniendo el AudioEngine independiente del ritmo del loop de render y utilizando colas de eventos de audio thread-safe para comunicar juego y audio.
- Ruido o glitching por buffers desincronizados:
  - Elegir tamaños de buffer que equilibren latencia y estabilidad (priorizar estabilidad para debugging).

---

## 5. Fase 4 – Backend de Audio para ESP32 (I2S / DAC / PWM)

### Objetivo

Implementar un backend de audio orientado a ESP32 que utilice periféricos de audio del chip (preferentemente I2S con DMA; alternativamente DAC o PWM) para reproducir las muestras generadas por el núcleo NES-like.

### Cambios arquitectónicos

- Nuevo backend de audio específico para ESP32:
  - Ubicado bajo el árbol de drivers correspondiente a ESP32.
  - Responsable de configurar el periférico elegido (I2S, DAC o PWM) y de alimentar sus buffers con bloques de muestras proporcionados por AudioEngine.
- Configuración explícita de GPIO para el speaker:
  - Definir una estructura de configuración de audio (similar a DisplayConfig) que incluya los pines GPIO relevantes para salida de audio.
  - Para I2S: pines de reloj de bit, reloj de palabra y datos (por ejemplo, BCLK, LRCK/WS, DATA).
  - Para DAC/PWM: pin GPIO del canal de salida asignado al speaker.
  - Esta configuración se establecerá en tiempo de inicialización del backend y será responsabilidad del juego/proyecto indicar los pines correctos según el diseño del hardware.
- Gestión de buffers y DMA:
  - Diseñar un esquema de doble o triple buffer para minimizar cortes en audio.
  - El backend mantiene un buffer circular o varias regiones de DMA que se rellenan desde el núcleo de mezcla.
- Integración con tareas y timers:
  - Utilizar timers hardware o callbacks de final de transferencia para disparar la generación de nuevos bloques de audio.
  - Mantener el trabajo pesado de mezcla en un contexto controlado (por ejemplo, una tarea dedicada de baja prioridad) para reducir interferencias con el bucle de juego.
- Configuración compile-time:
  - Permitir seleccionar vía configuración o macros si el backend de audio ESP32 está activado y cuál periférico utiliza.
  - Proveer una implementación nula cuando el audio está desactivado para ahorrar recursos en proyectos que no lo usan.

### Componentes o módulos afectados

- include/drivers/esp32:
  - Definición pública del backend de audio para ESP32.
- src/drivers/esp32:
  - Implementación de la inicialización del periférico, configuración de DMA y lógica de rellenado de buffers.
- src/core:
  - Inicialización del backend de audio al arrancar Engine en plataforma ESP32, con rutas de código protegidas por macros de compilación.

### Riesgos técnicos

- Contención de CPU y recursos:
  - Riesgo de que el coste de mezcla y gestión de DMA degrade el rendimiento del game loop.
  - Mitigar fijando frecuencias de muestreo y tamaños de bloque moderados, y midiendo tiempos de ejecución.
- Interacción con otros periféricos:
  - Compartición de SPI, bus de memoria y DMA con el subsistema de vídeo.
  - Mitigar coordinando el uso de DMA y evitando configuraciones que compitan con TFT_eSPI de forma agresiva.
- Complejidad de configuración:
  - Riesgo de que la configuración de pines y periféricos sea demasiado rígida.
  - Mitigar externalizando parámetros clave (pines y modos) a una estructura de configuración de audio similar a DisplayConfig, sin añadir automatismos opacos.

---

## 6. Fase 5 – Sincronización con Game Loop y API de Alto Nivel para Juegos

### Objetivo

Definir cómo se sincroniza el subsistema de audio con el game loop existente y ofrecer una API de alto nivel práctica para los juegos, garantizando al mismo tiempo que los títulos existentes sigan funcionando sin cambios.

### Cambios arquitectónicos

- Sincronización con el game loop:
  - Engine notifica al AudioEngine cada frame con el deltaTime actual para que éste avance decoradores temporales (duración de notas, envelopes, patrones).
  - La generación de muestras a alta frecuencia continúa gestionada por los backends, que consultan el estado actualizado del AudioEngine.
- API de alto nivel para juegos:
  - Proporcionar un conjunto reducido de funciones para:
    - Lanzar efectos de sonido preconfigurados (salto, golpe, rebote, etcétera).
    - Disparar notas o secuencias sencillas sobre canales específicos.
  - Mantener la API alineada con el estilo del motor (interfaces claras, sin excepciones, sin asignaciones dinámicas).
- Estrategia de integración gradual con examples:
  - Añadir llamadas a la API de audio en ejemplos existentes de forma opcional y encapsulada.
  - Asegurarse de que el código de los ejemplos se compila y ejecuta igual cuando el audio está desactivado.

### Componentes o módulos afectados

- include/audio:
  - Definición de la API de alto nivel orientada a juegos (tipos de efectos, atajos para reproducir sonidos frecuentes).
- examples:
  - Añadir hooks de audio en momentos clave (por ejemplo, colisiones en Pong o saltos en GeometryJump), respetando la arquitectura de actores y escenas.
- Documentación:
  - Actualizar la referencia del motor para incluir la manera recomendada de usar el audio desde escenas y actores, manteniendo la documentación en línea con el estilo actual.

### Riesgos técnicos

- Uso incorrecto del subsistema desde juegos:
  - Riesgo de abuso del API que genere demasiados eventos simultáneos.
  - Mitigar definiendo límites claros (número máximo de eventos en cola, número de canales fijo) y documentándolos.
- Crecimiento de complejidad:
  - Riesgo de que el subsistema de audio derive en un motor de música completo.
  - Mitigar manteniendo el alcance centrado en efectos de sonido y patrones breves, dejando fuera composición avanzada.

---

## 7. Fase 6 – Optimización, Medición y Estrategia de Testing

### Objetivo

Validar el impacto en rendimiento y memoria, ajustar parámetros de muestreo y buffers, y definir una estrategia de pruebas reproducible en Windows y ESP32.

### Cambios arquitectónicos

- Instrumentación básica:
  - Añadir mediciones opcionales del tiempo dedicado a la mezcla por bloque de audio, tanto en ESP32 como en el backend SDL2.
  - Permitir compilar estas mediciones sólo en builds de desarrollo.
- Ajuste de parámetros:
  - Experimentar con distintas frecuencias de muestreo y tamaños de buffer para encontrar el mejor compromiso entre calidad y uso de CPU.
  - Verificar que el subsistema funciona correctamente bajo diferentes cargas de juego (muchos actores, colisiones frecuentes).
- Estrategia de testing en Windows:
  - Definir una escena de prueba específica que ejercite todos los canales (pulse, triangle, noise) y situaciones típicas (colisiones rápidas, loops de fondo).
  - Permitir ejecutar esta escena en entorno nativo y escuchar el resultado a través del backend SDL2.
- Estrategia de testing en ESP32:
  - Probar la misma escena de audio en hardware real, verificando estabilidad, ausencia de watchdog resets y ausencia de artefactos graves de sonido.

### Componentes o módulos afectados

- src/audio:
  - Incorporación opcional de contadores de tiempo y estadísticas básicas de mezcla.
- src/drivers/native y src/drivers/esp32:
  - Puntos de integración para mediciones y verificación de uso de CPU.
- examples:
  - Ejemplo o escena de estrés enfocada en audio para pruebas de regresión.

### Riesgos técnicos

- Diferencias entre comportamiento en SDL2 y en hardware real:
  - Riesgo de que un sistema suene estable en Windows y presente problemas en ESP32 debido a latencias y restricciones de recursos distintas.
  - Mitigar manteniendo el mismo núcleo de generación de audio en ambas plataformas y ajustando únicamente la capa de backend y parámetros de buffer.

---

## 8. Resumen de Impacto en Rendimiento y Memoria

- Memoria:
  - Consumo adicional fijo por el estado de los canales (estructuras pequeñas).
  - Buffers de audio de tamaño moderado (doble o triple buffer de muestras) preasignados.
  - Sin asignaciones dinámicas en el bucle principal ni en callbacks de audio.
- CPU:
  - Coste proporcional al número de muestras generadas por segundo y al número de canales activos.
  - Se prioriza una implementación entera/punto fijo y tablas precalculadas cuando sea necesario para reducir operaciones costosas.
- Determinismo:
  - El núcleo lógico de audio (estado de canales, eventos y mezclador) se diseña determinista respecto a la secuencia de eventos y al deltaTime que recibe del juego.
  - Los backends de salida pueden introducir ligeras variaciones de latencia, pero la evolución del estado interno seguirá siendo reproducible para una entrada dada.

