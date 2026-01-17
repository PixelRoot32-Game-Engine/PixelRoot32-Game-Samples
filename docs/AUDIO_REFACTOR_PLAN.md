# Plan de Refactorización del Sistema de Audio

## Objetivo
Proporcionar múltiples implementaciones de backend de audio para la plataforma ESP32 dentro de la librería `PixelRoot32-Game-Engine`, permitiendo al usuario elegir fácilmente entre hardware I2S (MAX98357A) o DAC interno (PAM8302A).

## Situación Actual
- La interfaz `AudioBackend` está definida en `lib/PixelRoot32-Game-Engine/include/audio/`.
- Existen dos implementaciones concretas en `lib/PixelRoot32-Game-Engine/src/drivers/esp32/`:
    - `ESP32_I2S_AudioBackend`: Implementación de alta calidad usando periférico I2S (para DACs externos como MAX98357A).
    - `ESP32_DAC_AudioBackend`: Implementación retro/simple usando `dacWrite` en los pines GPIO 25/26 (para amplificadores analógicos directos como PAM8302A).

## Principios de Diseño: Backend vs Mixer

Es crucial mantener la separación de responsabilidades:

- **AudioEngine (Mixer)**:
    - Contiene toda la lógica de mezcla, canales, SFX, música y prioridades.
    - Decide *qué* sonido se produce.
    - Genera las muestras finales mediante `generateSamples()`.

- **AudioBackend (Driver)**:
    - **NO** mezcla ni procesa lógica de juego.
    - **NO** conoce qué es un efecto de sonido o una nota musical.
    - Solo se encarga de recibir muestras (vía `init` o callbacks) y enviarlas al hardware.
    - Métodos clave: `init()`, `getSampleRate()`, y su bucle interno de salida (tarea FreeRTOS).

## Implementación Realizada

### 1. Renombrado y Clarificación
- `ESP32_AudioBackend` ha sido renombrado a `ESP32_I2S_AudioBackend` para reflejar explícitamente su dependencia del protocolo I2S.
- Ubicación: `lib/.../include/drivers/esp32/ESP32_I2S_AudioBackend.h`.

### 2. Nuevo Backend DAC
- Se ha creado `ESP32_DAC_AudioBackend` para soporte de DAC interno.
- Utiliza `dacWrite()` directo en una tarea de alta prioridad.
- Diseñado para estabilidad en 8-bit (recomendado sample rate bajo, e.g., 11025 Hz).
- Ubicación: `lib/.../include/drivers/esp32/ESP32_DAC_AudioBackend.h`.

### 3. Configuración en el Proyecto (`main.cpp`)
El usuario selecciona el backend deseado instanciando la clase correspondiente y pasándola a la configuración del motor:

```cpp
// Opción A: I2S (MAX98357A)
// pr32::drivers::esp32::ESP32_I2S_AudioBackend audioBackend(I2S_BCLK, I2S_LRCK, I2S_DOUT, 22050);

// Opción B: DAC Interno (PAM8302A)
pr32::drivers::esp32::ESP32_DAC_AudioBackend audioBackend(DAC_PIN, 11025);

// Configuración común
pr32::audio::AudioConfig audioConfig(&audioBackend, audioBackend.getSampleRate());
pr32::core::Engine engine(config, inputConfig, audioConfig);
```

## Próximos Pasos Sugeridos
1. Verificar la salida de audio con el hardware real.
2. Si se requiere audio PCM continuo vía DAC interno (no `dacWrite`), implementar `ESP32_I2S_InternalDAC_Backend` por separado.
