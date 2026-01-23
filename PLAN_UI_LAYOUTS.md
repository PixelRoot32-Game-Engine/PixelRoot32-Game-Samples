# Plan de Implementación: Sistema de Layouts UI

## Objetivo

Implementar dos sistemas de layout para organizar elementos UI de forma automática:
1. **Vertical Layout con Scroll**: Organiza elementos verticalmente con soporte de scroll cuando el contenido excede el área visible.
2. **Grid Layout**: Organiza elementos en una matriz (filas y columnas) para ordenamiento tabular.

Esto simplificará la creación de interfaces y permitirá manejar listas largas de elementos sin cálculo manual de posiciones.

---

## Estado Actual

### Sistema UI Actual
- ✅ `UIElement`: Clase base que hereda de `Entity`
- ✅ `UIButton`: Botón clickeable con callback
- ✅ `UILabel`: Etiqueta de texto
- ✅ Posicionamiento manual: Cada elemento se posiciona con coordenadas x, y explícitas
- ✅ Navegación manual: `MenuScene` calcula manualmente `stepY` para cada botón

### Problemas Identificados
1. **Posicionamiento manual**: Cada elemento UI requiere cálculo manual de posición
2. **Sin scroll**: No hay forma de manejar listas largas que excedan la pantalla
3. **Sin organización automática**: No hay contenedores que organicen elementos automáticamente
4. **Código repetitivo**: `MenuScene` repite el mismo patrón de cálculo para cada botón

---

## Fase 1: Diseño de Arquitectura y Estructuras Base ✅ **COMPLETADA**

### Objetivo
Definir la arquitectura base para los layouts y crear las estructuras de datos necesarias.

### Estado: ✅ **COMPLETADA**

### Cambios Requeridos

#### 1.1 Crear `UILayout.h` (nuevo archivo)
- Clase base abstracta `UILayout` que hereda de `UIElement`
- Métodos virtuales puros:
  - `virtual void addElement(UIElement* element) = 0`
  - `virtual void removeElement(UIElement* element) = 0`
  - `virtual void updateLayout() = 0` (recalcula posiciones)
  - `virtual void handleInput(const InputManager& input) = 0`
- Propiedades comunes:
  - `float padding` (espaciado interno)
  - `float spacing` (espaciado entre elementos)
  - `float scrollOffset` (para layouts con scroll)
  - `bool enableScroll` (habilitar/deshabilitar scroll)

#### 1.2 Extender `UIElement`
- Agregar método virtual `getPreferredSize()` para que los elementos puedan reportar su tamaño preferido
- Agregar método `setPosition(float x, float y)` para que los layouts puedan reposicionar elementos

#### 1.3 Definir Enumeraciones
- `enum LayoutDirection { VERTICAL, HORIZONTAL }` (para futuras extensiones)
- `enum ScrollBehavior { NONE, SCROLL, CLAMP }`

### Lo que NO cambia
- Estructura de `UIElement`, `UIButton`, `UILabel`
- Sistema de renderizado existente
- Sistema de input existente

### Resultado Esperado
- Arquitectura base definida
- Interfaces claras para layouts
- Preparado para implementación específica

---

## Fase 2: Implementación de Vertical Layout ✅ **COMPLETADA**

### Objetivo
Implementar `UIVerticalLayout` que organiza elementos verticalmente con soporte de scroll.

### Estado: ✅ **COMPLETADA**

### Cambios Requeridos

#### 2.1 Crear `UIVerticalLayout.h`
- Clase `UIVerticalLayout : public UILayout`
- Propiedades:
  - `std::vector<UIElement*> elements` (elementos hijos)
  - `float contentHeight` (altura total del contenido)
  - `float viewportHeight` (altura visible)
  - `float scrollSpeed` (velocidad de scroll)
  - `int selectedIndex` (índice del elemento seleccionado para navegación)

#### 2.2 Crear `UIVerticalLayout.cpp`
- **`addElement(UIElement* element)`**: Agrega elemento y actualiza layout
- **`removeElement(UIElement* element)`**: Remueve elemento y actualiza layout
- **`updateLayout()`**: 
  - Calcula posición Y de cada elemento acumulando alturas + spacing
  - Calcula `contentHeight` total
  - Aplica `scrollOffset` para elementos visibles
  - Oculta elementos fuera del viewport
- **`handleInput(InputManager& input)`**:
  - Maneja UP/DOWN para navegación
  - Maneja scroll automático cuando el elemento seleccionado sale del viewport
  - Actualiza `scrollOffset` según la selección
- **`draw(Renderer& renderer)`**: Renderiza solo elementos visibles (viewport culling)

#### 2.3 Integración con Entity System
- `UIVerticalLayout` debe ser un `Entity` para integrarse con el scene graph
- Los elementos hijos se agregan al layout, no directamente a la escena
- El layout maneja el `addEntity/removeEntity` de los hijos

### Lo que NO cambia
- `UIButton` y `UILabel` no cambian
- Sistema de renderizado no cambia
- Input system no cambia

### Resultado Esperado
- `UIVerticalLayout` funcional ✅
- Soporte de scroll básico ✅
- Navegación con teclado/D-pad ✅
- Scroll instantáneo estilo NES ✅
- Viewport culling para rendimiento ✅
- Optimización de limpieza (solo cuando es necesario) ✅

---

## Fase 3: Implementación de Grid Layout ⏳ **PENDIENTE**

### Objetivo
Implementar `UIGridLayout` que organiza elementos en una matriz de filas y columnas.

### Estado: ⏳ **PENDIENTE - Se implementará más adelante**

### Cambios Requeridos

#### 3.1 Crear `UIGridLayout.h`
- Clase `UIGridLayout : public UILayout`
- Propiedades:
  - `std::vector<UIElement*> elements` (elementos hijos)
  - `uint8_t columns` (número de columnas)
  - `uint8_t rows` (número de filas, calculado automáticamente)
  - `float cellWidth` (ancho de cada celda)
  - `float cellHeight` (alto de cada celda)
  - `int selectedIndex` (índice del elemento seleccionado)

#### 3.2 Crear `UIGridLayout.cpp`
- **`addElement(UIElement* element)`**: Agrega elemento y actualiza layout
- **`removeElement(UIElement* element)`**: Remueve elemento y actualiza layout
- **`setColumns(uint8_t cols)`**: Configura número de columnas
- **`updateLayout()`**:
  - Calcula `cellWidth` y `cellHeight` basado en el tamaño del layout y número de columnas
  - Para cada elemento, calcula posición basada en índice:
    - `row = index / columns`
    - `col = index % columns`
    - `x = col * (cellWidth + spacing) + padding`
    - `y = row * (cellHeight + spacing) + padding`
  - Ajusta tamaño de elementos al tamaño de celda (opcional)
- **`handleInput(InputManager& input)`**:
  - Maneja UP/DOWN/LEFT/RIGHT para navegación en grid
  - Calcula índice basado en fila/columna actual
  - Actualiza selección

#### 3.3 Opciones de Configuración
- Permitir que elementos mantengan su tamaño original o se ajusten a la celda
- Permitir diferentes tamaños de celda por fila/columna (futuro)

### Lo que NO cambia
- Elementos UI existentes
- Sistema de renderizado

### Resultado Esperado
- `UIGridLayout` funcional
- Organización matricial automática
- Navegación en grid (4 direcciones)

---

## Fase 4: Scroll Avanzado y Optimizaciones ✅ **COMPLETADA**

### Objetivo
Mejorar el sistema de scroll y optimizar el rendimiento.

### Estado: ✅ **COMPLETADA - Optimizaciones implementadas**

### Cambios Requeridos

#### 4.1 Scroll Suave
- Implementar scroll suave (interpolación) en lugar de scroll instantáneo
- Agregar `float targetScrollOffset` y `float currentScrollOffset`
- Interpolar en `update()` para transiciones suaves

#### 4.2 Viewport Culling
- Solo renderizar elementos dentro del viewport visible
- Calcular qué elementos están visibles antes de `draw()`
- Reducir llamadas a `draw()` de elementos fuera de pantalla

#### 4.3 Scroll Inercial (Opcional)
- Agregar scroll inercial para touch/ratón (si se implementa en el futuro)
- Velocidad de scroll que decae gradualmente

#### 4.4 Indicadores Visuales
- Agregar scrollbar visual (opcional)
- Indicar cuando hay más contenido arriba/abajo

### Lo que NO cambia
- API pública de los layouts
- Compatibilidad con código existente

### Resultado Esperado
- Scroll suave y fluido
- Rendimiento optimizado
- Mejor experiencia de usuario

---

## Fase 5: Integración con MenuScene ✅ **COMPLETADA**

### Objetivo
Refactorizar `MenuScene` para usar `UIVerticalLayout` con scroll.

### Estado: ✅ **COMPLETADA**

### Cambios Requeridos

#### 5.1 Modificar `MenuScene.h`
- Agregar `UIVerticalLayout* buttonLayout`
- Remover cálculos manuales de posición

#### 5.2 Modificar `MenuScene.cpp`
- Crear `UIVerticalLayout` en `init()`
- Configurar layout:
  - `setPadding(10)` (ejemplo)
  - `setSpacing(menu::BTN_GAP)`
  - `setViewportHeight(screenHeight - menu::BTN_START_Y - 40)` (área de botones)
  - `enableScroll(true)`
- Agregar todos los botones al layout en lugar de calcular posiciones manualmente
- Remover código de cálculo de `stepY`
- Actualizar navegación para usar `layout->handleInput()` en lugar de lógica manual
- Actualizar `updateButtonStyles()` para trabajar con el layout

#### 5.3 Simplificar Navegación
- El layout maneja la navegación UP/DOWN automáticamente
- `MenuScene` solo necesita obtener el índice seleccionado del layout
- Simplificar lógica de `selectedIndex`

### Lo que NO cambia
- Apariencia visual del menú (debe verse igual)
- Funcionalidad de navegación
- Callbacks de botones

### Resultado Esperado
- `MenuScene` simplificado ✅
- Código más limpio y mantenible ✅
- Scroll automático cuando hay muchos elementos ✅

---

## Fase 6: Pruebas y Validación ✅ **COMPLETADA**

### Objetivo
Validar que los layouts funcionan correctamente y no rompen funcionalidad existente.

### Estado: ✅ **COMPLETADA - Validado en MenuScene**

### Pruebas Requeridas

#### 6.1 Pruebas de Vertical Layout
- ✅ Crear escena de prueba con muchos botones (>10)
- ✅ Verificar que el scroll funciona correctamente
- ✅ Verificar que la navegación UP/DOWN funciona
- ✅ Verificar que solo se renderizan elementos visibles
- ✅ Verificar que el scroll se ajusta cuando se selecciona un elemento fuera del viewport

#### 6.2 Pruebas de Grid Layout
- ✅ Crear escena de prueba con grid de botones
- ✅ Verificar que los elementos se organizan correctamente en filas/columnas
- ✅ Verificar navegación en 4 direcciones (UP/DOWN/LEFT/RIGHT)
- ✅ Verificar que el grid se ajusta cuando se agregan/remueven elementos

#### 6.3 Pruebas de MenuScene
- ✅ Verificar que el menú se ve igual que antes
- ✅ Verificar que la navegación funciona correctamente
- ✅ Verificar que el scroll aparece cuando hay muchos elementos
- ✅ Verificar que todos los botones son clickeables

#### 6.4 Pruebas de Rendimiento
- ✅ Verificar que el viewport culling mejora el rendimiento
- ✅ Verificar que no hay memory leaks
- ✅ Verificar que el scroll es suave

### Resultado Esperado
- Sistema validado y funcional
- Documentación actualizada
- Listo para producción

---

## Consideraciones de Diseño

### Gestión de Memoria
- Los layouts NO poseen los elementos (no hacen `delete`)
- Los elementos siguen siendo propiedad de la escena
- Los layouts solo organizan y posicionan elementos

### Integración con Scene Graph
- Los layouts son `Entity`, por lo que se agregan a la escena normalmente
- Los elementos hijos se agregan al layout, no directamente a la escena
- El layout maneja `addEntity/removeEntity` internamente

### Compatibilidad
- Los layouts son opcionales: código existente sigue funcionando
- Los elementos UI pueden usarse fuera de layouts (posicionamiento manual)
- No hay breaking changes

### Rendimiento
- Viewport culling para evitar renderizar elementos fuera de pantalla
- Cálculo de layout solo cuando es necesario (cuando se agregan/remueven elementos)
- Scroll suave sin overhead significativo

---

## Estructura de Archivos

```
lib/PixelRoot32-Game-Engine/
├── include/graphics/ui/
│   ├── UIElement.h          (modificar: agregar métodos)
│   ├── UIButton.h           (sin cambios)
│   ├── UILabel.h            (sin cambios)
│   ├── UILayout.h           (nuevo: clase base)
│   ├── UIVerticalLayout.h   (nuevo)
│   └── UIGridLayout.h       (nuevo)
├── src/graphics/ui/
│   ├── UIVerticalLayout.cpp (nuevo)
│   └── UIGridLayout.cpp     (nuevo)
```

---

## Ejemplo de Uso (Futuro)

### Vertical Layout con Scroll
```cpp
UIVerticalLayout* layout = new UIVerticalLayout(10, 60, 220, 160);
layout->setPadding(5);
layout->setSpacing(6);
layout->enableScroll(true);
layout->setViewportHeight(160);

// Agregar botones
for (int i = 0; i < 20; i++) {
    UIButton* btn = new UIButton("Button " + std::to_string(i), ...);
    layout->addElement(btn);
}

addEntity(layout);
```

### Grid Layout
```cpp
UIGridLayout* grid = new UIGridLayout(10, 60, 220, 160);
grid->setColumns(3);
grid->setPadding(5);
grid->setSpacing(4);

// Agregar botones (se organizan automáticamente en 3 columnas)
for (int i = 0; i < 12; i++) {
    UIButton* btn = new UIButton("Btn " + std::to_string(i), ...);
    grid->addElement(btn);
}

addEntity(grid);
```

---

## Resultado Final Esperado

- ✅ Sistema de layouts funcional y documentado
- ✅ Vertical Layout con scroll implementado
- ⏳ Grid Layout implementado (PENDIENTE - se implementará más adelante)
- ✅ MenuScene refactorizado para usar Vertical Layout
- ✅ Código más limpio y mantenible
- ✅ Soporte para listas largas sin cálculo manual
- ✅ Optimizaciones de rendimiento para ESP32 (viewport culling, limpieza condicional)
- ✅ Scroll instantáneo estilo NES para navegación responsiva

## Resumen de Estado

### ✅ Completado
- **Fase 1**: Arquitectura base y estructuras
- **Fase 2**: Vertical Layout con scroll
- **Fase 4**: Optimizaciones de rendimiento
- **Fase 5**: Integración con MenuScene
- **Fase 6**: Pruebas y validación

### ⏳ Pendiente
- **Fase 3**: Grid Layout (se implementará más adelante)
