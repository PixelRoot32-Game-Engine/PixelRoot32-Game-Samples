# Plan por Fases – Subsistema de Melodías NES‑like sobre AudioEngine

Este documento define la hoja de ruta para añadir soporte de **melodías** (tracks y canciones)
encima del subsistema de audio NES‑like existente, manteniendo:

- Bajo coste en CPU y RAM.
- Ausencia de asignación dinámica en tiempo de juego.
- Compatibilidad con ESP32 e implementación nativa (SDL2).
- Estilo y arquitectura actuales del motor PixelRoot32.

El objetivo **no** es emular por completo la APU de NES, sino ofrecer una capa ligera para
melodías y música de fondo simple (1–3 voces) usando los 4 canales fijos actuales.

---

## Fase 1 – Tipos de nota y tabla de frecuencias

### Objetivo

Permitir trabajar con notas musicales (C, D, E, etc.) y octavas en lugar de usar solo
frecuencias crudas en Hz, sin modificar el núcleo de AudioEngine.

### Cambios propuestos

- Añadir tipos básicos en el módulo `audio`:
  - `enum class Note : uint8_t { C, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B };`
  - Función helper:
    - `float noteToFrequency(Note note, int octave);`
- Implementación:
  - Tabla base `constexpr` de 12 semitonos para una octava de referencia (por ejemplo, 4).
  - Cálculo del resto de octavas mediante factores de 2.
  - Sin `std::vector`, `std::map` ni otras estructuras dinámicas.

### Impacto

- No se modifica `AudioEngine` ni `AudioEvent`.
- Los juegos pueden seguir usando `frequency` directamente o apoyarse en `noteToFrequency`
  para crear SFX y, en fases posteriores, melodías.

---

## Fase 2 – Track lineal de una sola voz (MusicTrack + MusicPlayer)

### Objetivo

Reproducir una **secuencia lineal de notas** (una voz melódica) utilizando el AudioEngine
existente, sincronizada con el game loop mediante `deltaTime`.

### Cambios propuestos

- Nuevos tipos ligeros en `audio` (por ejemplo `AudioMusicTypes.h`):

  ```cpp
  struct MusicNoteEvent {
      WaveType wave;
      Note note;
      int8_t octave;
      float durationBeats;
      float volume;
      float duty;
  };

  struct MusicTrack {
      const MusicNoteEvent* steps;
      int length;
      float bpm;
      bool loop;
  };
  ```

- Nuevo componente `MusicPlayer`:

  - Estado mínimo:
    - Puntero `const MusicTrack* currentTrack`.
    - Índice de nota actual.
    - Tiempo acumulado en la nota actual (ms).
    - Flag `playing`.
  - API:
    - `void play(const MusicTrack* track);`
    - `void stop();`
    - `void update(unsigned long deltaTime, AudioEngine& audio);`

### Modelo de tiempo

- `bpm` define la duración de un beat: `beatMs = 60000.0f / bpm`.
- Cada `MusicNoteEvent` indica su duración en beats (`durationBeats`).
- `MusicPlayer::update`:
  - Acumula `deltaTime` sobre la nota actual.
  - Cuando el tiempo excede `durationBeats * beatMs`, avanza al siguiente paso.
  - Al entrar en una nota:
    - Convierte `Note + octave` a frecuencia con `noteToFrequency`.
    - Construye un `AudioEvent`.
    - Llama a `audio.playEvent(event)`.

### Restricciones

- Sin asignación dinámica en tiempo de ejecución:
  - Los `MusicTrack` son arrays `constexpr` definidos por el juego.
- Sin hilos ni timers nuevos:
  - Toda la lógica corre en el `update` de juego, igual que el resto del motor.

---

## Fase 3 – Canciones multi‑canal (melodía + bajo + percusión)

### Objetivo

Soportar una **canción** con varias voces concurrentes (por ejemplo melodía, bajo y “percusión”
de ruido) reutilizando los 4 canales fijos de AudioEngine.

### Cambios propuestos

- Definir un tipo `MusicSong`:

  ```cpp
  struct MusicSong {
      const MusicTrack* tracks;
      int trackCount;
  };
  ```

  o bien una versión con campos fijos:

  ```cpp
  struct MusicSong {
      const MusicTrack* pulseLead;
      const MusicTrack* pulseHarmony;
      const MusicTrack* triangleBass;
      const MusicTrack* noiseDrums;
  };
  ```

- Extender `MusicPlayer` para manejar varias voces:
  - Internamente mantiene un pequeño array de `TrackState` (uno por voz).
  - Cada `TrackState` replica la lógica de Fase 2 (índice de nota, tiempo acumulado, etc.).
  - Cada voz se asocia a un `WaveType` fijo:
    - Pulse 0 → melodía principal.
    - Pulse 1 → acompañamiento / armonía.
    - Triangle → bajo.
    - Noise → pseudo percusión.

### Asignación de canales

- Política simple:
  - Música ocupa hasta 3–4 canales según la canción.
  - SFX que lleguen vía `AudioEngine::playEvent` pueden:
    - Reutilizar canales de música (voice stealing, ideal para SFX muy breves), o
    - Reservar un canal (por ejemplo Pulse1) si se desea priorizar SFX sobre armonía.
- La lógica de asignación se puede mantener fuera del núcleo:
  - `MusicPlayer` utiliza siempre el `WaveType` acordado para cada track.
  - El núcleo de AudioEngine sigue siendo ciego a si un `AudioEvent` proviene de música o de SFX.

---

## Fase 4 – API de alto nivel y helpers para juegos

### Objetivo

Hacer que usar melodías sea tan sencillo como usar efectos de sonido, sin exponer demasiados detalles
internos de `MusicPlayer` en los juegos.

### Cambios propuestos

- Helpers en el espacio de nombres `audio`:
  - Funciones de conveniencia:

    ```cpp
    void playSong(const MusicSong* song);
    void stopSong();
    ```

  - Estas funciones pueden delegar en una instancia global o en un `MusicPlayer` gestionado
    por el engine o por la propia escena.

- Convenios para definir música:
  - Macros/helpers para escribir pistas de forma más legible:

    ```cpp
    #define NOTE_EVENT(wave, note, oct, beats, vol, duty) \
        { wave, Note::note, oct, beats, vol, duty }
    ```

  - Los juegos definen arrays `constexpr MusicNoteEvent[]` para cada pista de la canción.

- Integración mínima en examples:
  - Añadir una melodía sencilla en la escena de menú o en un ejemplo de prueba:
    - Al entrar en la escena → `playSong(&mainMenuSong);`
    - Al salir o pausar → `stopSong();`

---

## Fase 5 – Optimización, pruebas y límites

### Objetivo

Validar que la capa de melodías no degrada el rendimiento ni la memoria, especialmente en ESP32,
y definir límites razonables.

### Consideraciones de rendimiento

- CPU:
  - `MusicPlayer::update` es O(número de voces activas).
  - El trabajo por voz es ligero: acumulación de tiempo, comparaciones y llamadas puntuales
    a `playEvent` cuando cambia la nota.
  - La parte costosa (mezcla de muestras) sigue siendo responsabilidad de `AudioEngine`.

- Memoria:
  - Los tracks (`MusicTrack`, `MusicNoteEvent`) viven en flash (`constexpr`).
  - El estado por voz (`TrackState`) ocupa unas pocas variables básicas (índices y contadores).

### Estrategia de pruebas

- SDL2 (entorno nativo):
  - Escena de prueba que reproduce una canción multi‑voz y dispara varios SFX al mismo tiempo.
  - Verificar que no hay glitches ni artefactos de clipping excesivo.
- ESP32:
  - Misma escena compilada para hardware real.
  - Verificar:
    - Ausencia de resets de watchdog.
    - Latencia razonable.
    - Sonido estable con I2S + TFT dibujando a 60 FPS.

### Límites recomendados

- Número máximo de voces simultáneas: 3–4 (acorde con los 4 canales disponibles).
- Longitud de las canciones:
  - Pistas pensadas para bucles cortos (15–60 s), típicos de juegos retro.
  - Evitar canciones muy largas codificadas como arrays gigantes.

---

## Resumen

Este plan añade soporte de melodías al motor de forma incremental:

- **Fase 1–2**: notas musicales y un `MusicPlayer` de una sola voz, suficiente para melodías simples.
- **Fase 3**: canciones multi‑canal (melodía + bajo + percusión de ruido).
- **Fase 4**: una API de alto nivel cómoda para juegos (`playSong`, `stopSong`, helpers).
- **Fase 5**: validación en SDL2 y ESP32 para asegurar que el coste sigue siendo aceptable.

Todo ello se construye encima de `AudioEngine` y `AudioEvent` sin romper el diseño actual y
manteniendo el espíritu retro y embedded‑friendly del motor PixelRoot32.
