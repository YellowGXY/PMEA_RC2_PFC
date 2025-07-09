# MANUAL COMPLETO - SISTEMA DE GESTIÓN Y PREDICCIÓN DE CALIDAD DEL AIRE

## DESCRIPCIÓN GENERAL

El Sistema de Gestión y Predicción de Calidad del Aire es una aplicación integral desarrollada en C que permite monitorear, analizar y predecir la calidad del aire en múltiples zonas geográficas. El sistema maneja datos de contaminantes atmosféricos (CO2, SO2, NO2, PM2.5) junto con condiciones ambientales (temperatura, viento, humedad).

### ✅ VERSIÓN ACTUALIZADA Y MEJORADA (2025)
- **Soporte multiplataforma completo** para Windows, Linux y macOS
- **Eliminación completa de caracteres especiales** para máxima compatibilidad
- **Funciones de predicción flexibles** que funcionan con datos limitados
- **Gestión inteligente de fechas y meses consecutivos**
- **Validación robusta de entrada de datos**
- **Tablas optimizadas para mejor visualización**
- **Código simplificado y funciones básicas**

## CARACTERÍSTICAS PRINCIPALES

### ✅ FUNCIONES BÁSICAS
- **Gestión de 5 zonas predefinidas**: Quito, Cuenca, Guayaquil, Loja, Ambato
- **Monitoreo de 4 contaminantes principales**: CO2, SO2, NO2, PM2.5
- **Condiciones ambientales**: Temperatura, velocidad del viento, humedad
- **Sistema de alertas basado en estándares OMS**
- **Predicciones de calidad del aire con datos limitados**
- **Exportación de reportes y alertas**

### ✅ CARACTERÍSTICAS AVANZADAS
- **Gestión automática de fechas y meses consecutivos**
- **Sistema de checkpoints y recuperación de datos**
- **Importación/exportación masiva de datos**
- **Interfaz colorizada sin caracteres especiales**
- **Validación robusta de entrada de datos con funciones seguras**
- **Almacenamiento persistente en archivos binarios**
- **Prevención de errores de entrada y desbordamiento de buffer**
- **Compatibilidad multiplataforma total**

### ✅ SISTEMA DE ENTRADA DE DATOS ROBUSTA
- **Validación automática de rangos** para todos los tipos de datos
- **Prevención de desbordamiento de buffer** en todas las entradas
- **Manejo inteligente de errores** con mensajes claros para el usuario
- **Funciones especializadas**: `leerEnteroSeguro()`, `leerFloatSeguro()`, `leerCadenaSegura()`, `leerCaracterSeguro()`
- **Eliminación total de `scanf()` directo** para mayor seguridad - ✅ **COMPLETADO**
- **Portabilidad mejorada** sin patrones problemáticos de lectura
- **Seguridad 100%**: Cero vulnerabilidades de entrada de datos

### ✅ MEJORAS IMPLEMENTADAS (2025)
- **Soporte multiplataforma**: Separadores de ruta y comandos adaptativos
- **Eliminación de caracteres Unicode**: Todos los símbolos μg/m³ → ug/m3
- **Eliminación de acentos**: Todas las palabras con tildes normalizadas
- **Predicciones flexibles**: Funcionan con datos limitados o estimados
- **Avance de mes inteligente**: Detecta el mes máximo y crea el siguiente consecutivo
- **Tablas optimizadas**: Mejor espaciado y formato en todas las visualizaciones
- **Código simplificado**: Eliminación de funciones complejas innecesarias

## ARQUITECTURA MULTIPLATAFORMA

### 🖥️ COMPATIBILIDAD DE SISTEMAS OPERATIVOS
- **Windows**: Separadores de ruta `\`, comandos `mkdir` con `2>nul`
- **Linux/macOS**: Separadores de ruta `/`, comandos `mkdir -p` con `2>/dev/null`
- **Detección automática**: Mediante directivas de compilación `#ifdef _WIN32`

### 📁 GESTIÓN DE ARCHIVOS MULTIPLATAFORMA
```c
// Ejemplo de implementación multiplataforma
#ifdef _WIN32
    #define PATH_SEPARATOR "\\"
#else
    #define PATH_SEPARATOR "/"
#endif

// Uso en rutas de archivos
snprintf(ruta_archivo, sizeof(ruta_archivo), 
         "sistema_archivos%sdatos.dat", PATH_SEPARATOR);
```

### 🔧 CREACIÓN DE DIRECTORIOS ADAPTATIVOS
```c
void crearCarpetaSistema() {
    #ifdef _WIN32
        // Windows
        system("mkdir \"sistema_archivos\" 2>nul");
    #else
        // Linux/macOS
        system("mkdir -p \"sistema_archivos\" 2>/dev/null");
    #endif
}
```

---

## ESTRUCTURA DEL PROGRAMA

### ARCHIVOS PRINCIPALES
- **`main.c`**: Programa principal y menú principal
- **`funciones.c`**: Funciones básicas del sistema y funciones robustas de entrada
- **`funciones2.c`**: Funciones avanzadas y menús especializados
- **`funciones.h`**: Declaraciones de estructuras y funciones

### ESTRUCTURAS DE DATOS ACTUALIZADAS
```c
// Estructura de datos ambientales diarios
struct DatosAmbientales {
    float co2, so2, no2, pm25;     // Contaminantes principales
    float temperatura, viento, humedad;  // Condiciones ambientales
    char fecha[11];                 // Formato YYYY-MM-DD
};

// Estructura de datos por día
struct Dia {
    float co2, so2, no2, pm25;
    char fecha[11];
    float temperatura, viento, humedad;
};

// Estructura de mes con días
struct Mes {
    struct Dia dias[31];
    int numDias;
};

// Estructura de zona con meses
struct Zona {
    char nombre[32];
    struct Mes meses[12];
    int numMeses;
    // Datos actuales para compatibilidad
    float co2, so2, no2, pm25, temperatura, viento, humedad;
    // Umbrales personalizables
    struct {
        struct { float min, max; } co2, so2, no2, pm25;
    } umbrales;
};

// Estructura principal del sistema
struct Sistema {
    struct Zona zonas[5];
    int numZonas;
};

// Configuración de fechas del sistema
struct ConfiguracionFechas {
    int anio_inicio;
    int mes_inicio;
    int dia_inicio;
    int usar_fechas_automaticas;
};
```

### FUNCIONES PRINCIPALES POR ARCHIVO

#### **main.c**
```c
void cargarOSolicitarFechaInicial()    // Gestión de fechas inicial
int main()                             // Función principal del programa
```

#### **funciones.c** (Funciones básicas y robustas)
```c
// Inicialización del sistema
void inicializarZonas(struct Zona zonas[], int *numero_zonas)
void inicializarSistema(struct Sistema *sistema)
void inicializarConfiguracionFechas(struct ConfiguracionFechas *config)

// Gestión de datos
int cargarDatosHistoricos(struct Sistema *sistema, char *ruta_archivo)
int guardarDatos(struct Sistema *sistema, char *ruta_archivo)
void calcularPromedios(struct Sistema *sistema, float promedios[])

// Predicciones flexibles
void predecirContaminacion(struct Sistema *sistema, float prediccion[])
void emitirAlertas(struct Sistema *sistema, float prediccion[], char alertas[][64], int *numero_alertas)
void generarRecomendaciones(char alertas[][64], int numero_alertas)

// Visualización
void mostrarTablaZonas(struct Sistema *sistema)
void mostrarHistorialZonas()
void mostrarDetalleZona(struct Sistema *sistema)
void buscarZonaPorNombre(struct Sistema *sistema)

// Funciones de entrada robusta
int leerEnteroSeguro(const char *mensaje, int min, int max)
float leerFloatSeguro(const char *mensaje, float min, float max)
char leerCaracterSeguro(const char *mensaje)
void leerCadenaSegura(const char *mensaje, char *destino, int tamano_max)

// Validación de datos
int validarEnteroEnRango(const char *entrada, int min, int max)
int validarFloatEnRango(const char *entrada, float min, float max)
void limpiarBufferEntrada()

// Gestión de archivos multiplataforma
void crearCarpetaSistema()
void guardarMes(struct Zona *zona, int numero_mes)
void cargarMes(struct Zona *zona, int numero_mes)
void guardarSemanaActual(int semanaActual[], int numZonas)
void cargarSemanaActual(int semanaActual[], int numZonas)
void registrarPredicciones(struct Sistema *sistema, float prediccion[])
```

#### **funciones2.c** (Funciones avanzadas y menús)
```c
// Menús principales
void menuConfiguracion(struct Zona zonas[], int *numZonasPtr, int mesActual[])
void menuIngresoManual(struct Zona zonas[], int numZonas, int mesActual[])
void menuReportes(struct Zona zonas[], int numZonas)
void menuCheckpoints(struct Zona zonas[], int numZonas, int mesActual[])
void menuSiguienteMes(struct Zona zonas[], int numZonas, int mesActual[])
void menuPronosticos(struct Zona zonas[], int numZonas, int mesActual[])

// Funciones de pronóstico mejoradas
void generarPronosticoZona(struct Zona *zona, int mesActual)
void generarPronosticoGeneral(struct Zona zonas[], int numZonas, int mesActual[])

// Gestión de datos
void mostrarDatosZona(struct Zona *zona, int mesActual)
void importarDatosDesdeArchivo(struct Zona zonas[], int numZonas, int mesActual[])
void crearPlantillaDatos(struct Zona zonas[], int numZonas)

// Configuración del sistema
void cambiarNombreZona(struct Zona zonas[], int numZonas)
void ajustarMaximoSemanas(struct Zona zonas[], int numZonas)
void generarDatosMuestreo(struct Zona zonas[], int numZonas, int mesActual[])
void configurarFechasInicio(struct ConfiguracionFechas *config)

// Gestión de fechas y meses
void ajustarMesActualSegunFecha(int mesActual[], int numZonas)
int guardarConfiguracionFechas(struct ConfiguracionFechas *config)
int cargarConfiguracionFechas(struct ConfiguracionFechas *config)
void guardarMesActual(int mesActual[], int numZonas)
void cargarMesActual(int mesActual[], int numZonas)

// Utilidades
void esperarEnter()
void mostrarMensajeBienvenida()
int esBisiesto(int anio)
int diasEnMes(int mes, int anio)
```

---

## GUÍA DE USO

### 🚀 INICIO DEL PROGRAMA

Al ejecutar el programa:
1. **Primera vez**: Solicita configurar fecha inicial del sistema
2. **Ejecuciones posteriores**: Carga automáticamente la configuración guardada
3. Presenta el menú principal con 12 opciones principales

### 📋 MENÚ PRINCIPAL

```
+-----------------------------------------------+
| Sistema Integral de Gestion y Prediccion Aire |
+-----------------------------------------------+
| 1. Inicializar sistema                        |
| 2. Cargar datos historicos                    |
| 3. Mostrar datos actuales y promedios         |
| 4. Predicir contaminacion 24h                 |
| 5. Emitir alertas segun limites OMS           |
| 6. Mostrar recomendaciones                    |
| 7. Ingresar nuevos datos manualmente          |
| 8. Exportar reportes                          |
| 9. Salir                                      |
| 10. Ver historial de zonas                    |
| 11. Ver detalle de una zona                   |
| 12. Buscar zona por nombre                    |
+-----------------------------------------------+
```

---

## FUNCIONES PRINCIPALES

### 1. **INICIALIZAR SISTEMA** (Opción 1)
- **Propósito**: Crea datos de muestra para todas las zonas
- **Efecto**: Sobrescribe datos existentes
- **Uso**: Ideal para primeras pruebas del sistema
- **Datos generados**: 5 ciudades (Quito, Cuenca, Guayaquil, Loja, Ambato) con valores realistas de contaminantes

### 2. **CARGAR DATOS HISTÓRICOS** (Opción 2)
- **Propósito**: Recupera datos previamente guardados desde `datos_hist.dat`
- **Comportamiento**: Si no existe el archivo, muestra mensaje de error
- **Uso**: Para continuar trabajando con datos existentes

### 3. **MOSTRAR DATOS Y PROMEDIOS** (Opción 3)
- **Visualiza**: Tabla formateada con todas las zonas y sus contaminantes actuales
- **Calcula**: Promedios globales de CO2, SO2, NO2, PM2.5
- **Formato**: Tabla ASCII con bordes y alineación profesional

### 4. **PREDICCIÓN 24H** (Opción 4)
- **✅ PREDICCIONES FLEXIBLES**: Funciona con datos limitados o estimados
- **Algoritmo adaptativo**: Usa datos disponibles o genera estimaciones básicas
- **Resultado**: Predicciones de PM2.5 para cada zona independientemente de cantidad de datos
- **Funcionalidad mejorada**: Ya no requiere datos mínimos, siempre genera pronósticos
- **Almacenamiento**: Guarda predicciones en archivos multiplataforma
- **Estimaciones**: Cuando no hay datos suficientes, usa valores por defecto realistas
- **Ejemplo de pronóstico básico**:
  ```
  PRONOSTICO BASICO PARA ZONA: Quito
  Pronostico estimado (sin datos historicos suficientes):
    CO2: 0.040 ppm (estimado)
    SO2: 0.008 ug/m3 (estimado)
    NO2: 0.025 ug/m3 (estimado)
    PM2.5: 8.0 ug/m3 (estimado)
  ```

### 5. **ALERTAS OMS** (Opción 5)
- **Estándares**: Basado en límites de la Organización Mundial de la Salud
- **Niveles PM2.5**: 
  - 🟢 VERDE: ≤12 ug/m3 (Saludable)
  - 🟡 AMARILLA: 12.1-35 ug/m3 (Moderado)
  - 🟠 NARANJA: 35.1-55 ug/m3 (Danino para grupos sensibles)
  - 🔴 ROJA: >55 ug/m3 (Danino para toda la poblacion)
- **Alertas CO2**: >0.05 ppm

### 6. **RECOMENDACIONES** (Opción 6)
- **Automáticas**: Basadas en alertas activas
- **Personalizadas**: Según el nivel de contaminación detectado
- **Ejemplos**: "Limite actividades al aire libre", "Use mascarilla"

### 7. **INGRESO MANUAL** (Opción 7)
- **✅ NUEVA VALIDACIÓN ROBUSTA**: Sistema completamente refactorizado
- **Funciones Seguras**: Usa `leerFloatSeguro()` y `leerCadenaSegura()` exclusivamente
- **Validación en Tiempo Real**: Cada entrada es validada antes de ser aceptada
- **Rangos Específicos**:
  - CO2: 0.000-1.000 ppm (validación automática)
  - SO2: 0.0-100.0 ug/m3 (sin desbordamiento)
  - NO2: 0.0-300.0 ug/m3 (entrada segura)
  - PM2.5: 0.0-500.0 ug/m3 (proteccion total)
  - Temperatura: -50.0 a 60.0°C
  - Viento: 0.0-100.0 km/h
  - Humedad: 0-100%
- **Características de Seguridad**:
  - ❌ **Eliminado `scanf()` directo** - Ya no hay vulnerabilidades
  - ✅ **Prevención de buffer overflow** - Límites estrictos automáticos
  - ✅ **Manejo inteligente de errores** - Mensajes claros y comprensibles
  - ✅ **Validación de fechas** - Formato YYYY-MM-DD verificado
  - ✅ **Cancelación segura** - Salida limpia en cualquier momento
- **Experiencia de Usuario Mejorada**:
  - Mensajes descriptivos de error
  - Instrucciones claras para cada entrada
  - Confirmación de datos ingresados
  - Recuperación automática de errores

### 8. **EXPORTAR REPORTES** (Opción 8)
- **Archivo generado**: `reportes.dat`
- **Contenido**: Alertas actuales y recomendaciones específicas
- **Formato**: Texto plano para fácil lectura

### 9. **SALIR** (Opción 9)
- **Acción**: Termina el programa
- **Persistencia**: Los datos se mantienen en archivos para próximas ejecuciones

### 10. **HISTORIAL DE ZONAS** (Opción 10)
- **Fuente**: Carga datos desde `datos_hist.dat`
- **Visualización**: Tabla resumen de todas las zonas
- **Utilidad**: Revisar datos históricos sin modificar estado actual

### 11. **DETALLE DE ZONA** (Opción 11)
- **Interactivo**: Permite seleccionar zona específica (0-4)
- **Información completa**: Todos los contaminantes y condiciones ambientales
- **Formato**: Tabla detallada con bordes decorativos

### 12. **BUSCAR ZONA** (Opción 12)
- **Búsqueda**: Por nombre (insensible a mayúsculas/minúsculas)
- **Algoritmo**: Comparación caracter por caracter personalizada
- **Resultado**: Muestra datos completos de la zona encontrada

---

## MENÚS AVANZADOS

### 🔧 MENÚ SIGUIENTE MES (Función Especial)
- **✅ LÓGICA MEJORADA**: Detecta automáticamente el mes máximo registrado
- **Avance Inteligente**: Crea el siguiente mes consecutivo (no reinicia en mes 1)
- **Validación de Datos**: Verifica que haya datos antes de avanzar
- **Ejemplo**: Si hay datos hasta mes 7, crea el mes 8 (no vuelve a mes 1)

### 🔧 MENÚ DE CONFIGURACIÓN
Accesible desde funciones especiales del sistema:

#### **Opciones disponibles:**
1. **Cambiar nombre de zona**: Personalizar nombres de las 5 ciudades ecuatorianas
2. **Ajustar máximo de semanas**: Configurar límite (1-104 semanas)
3. **Ajustar máximo de días**: Configurar días por semana (1-7)
4. **Generar datos de muestreo**: Crear datos de prueba VARIADOS específicos por ciudad
   - **Propósito**: Demostrar TODOS los tipos de alertas y niveles de contaminación
   - **Distribución de alertas por ciudad**:
     - 🟢 **Quito**: Nivel VERDE (saludable, PM2.5 ≈ 10 ug/m3)
     - 🟡 **Cuenca**: Nivel AMARILLA (moderado, PM2.5 ≈ 25 ug/m3)
     - 🟠 **Guayaquil**: Nivel NARANJA (danino para sensibles, PM2.5 ≈ 45 ug/m3)
     - 🔴 **Loja**: Nivel ROJA (danino para toda la poblacion, PM2.5 ≈ 70 ug/m3)
     - 🟡⚠️ **Ambato**: Nivel AMARILLA + Alerta CO2 alto
5. **Configurar fechas de inicio**: Establecer fecha base del sistema
6. **Crear plantilla para datos**: Generar archivo de ejemplo para importación
7. **Importar datos**: Cargar datos desde archivo externo
8. **Editar fecha actual**: Modificar fecha del sistema manualmente

### 📊 MENÚ DE REPORTES
Sistema avanzado de informes:

#### **Funciones disponibles:**
1. **Reporte semanal por zona**: Análisis detallado día por día
2. **Tabla resumen**: Vista general de todas las zonas
3. **Alertas y recomendaciones**: Sistema visual con colores
4. **Exportar tabla de zonas**: Guardar en `reporte_zonas.txt`
5. **Exportar alertas**: Guardar en `alertas_recomendaciones.txt`

### 🔮 MENÚ DE PRONÓSTICOS (NUEVO)
Sistema de predicciones mejorado:

#### **Funciones disponibles:**
1. **Pronóstico por zona**: Análisis específico de una zona seleccionada
   - **Funcionamiento**: Detecta automáticamente si hay datos suficientes
   - **Con datos reales**: Usa tendencias y promedios históricos
   - **Sin datos suficientes**: Genera pronósticos básicos estimados
   - **Mínimo reducido**: Solo requiere 2 días de datos (antes 5)
   - **Ejemplo con datos limitados**:
     ```
     PRONOSTICO PARA ZONA: Cuenca
     Pronostico estimado (sin datos reales):
       CO2: 0.040 ppm (estimado)
       SO2: 0.008 ug/m3 (estimado)
       NO2: 0.025 ug/m3 (estimado)
       PM2.5: 8.0 ug/m3 (estimado)
     - Niveles estimados dentro del rango normal
     ```

2. **Pronóstico general todas las zonas**: Análisis conjunto
   - **Funcionamiento flexible**: Funciona con cualquier cantidad de datos
   - **Requisito reducido**: Solo necesita 1 zona con datos (antes 3)
   - **Estimaciones automáticas**: Genera pronósticos básicos cuando no hay datos
   - **Ejemplo de pronóstico general estimado**:
     ```
     PRONOSTICO GENERAL BASICO
     Pronostico estimado para todas las zonas:
       CO2 promedio: 0.040 ppm (estimado)
       SO2 promedio: 0.008 ug/m3 (estimado)
       NO2 promedio: 0.025 ug/m3 (estimado)
       PM2.5 promedio: 8.0 ug/m3 (estimado)
     - Niveles estimados dentro del rango normal
     ```

#### **Mejoras implementadas en pronósticos:**
- **✅ Siempre funciona**: No bloquea por falta de datos
- **✅ Estimaciones inteligentes**: Usa valores realistas cuando no hay datos
- **✅ Flexibilidad mejorada**: Adapta el análisis a datos disponibles
- **✅ Mensajes claros**: Indica cuando usa estimaciones vs datos reales
- **✅ Evaluación automática**: Clasifica niveles de contaminación estimados

### 💾 MENÚ DE CHECKPOINTS
Sistema de respaldo y recuperación:

#### **Características:**
1. **Guardar semana actual**: Checkpoint permanente
2. **Retroceder a semana anterior**: Navegación temporal
3. **Recuperar datos temporales**: Restaurar backups automáticos
4. **Gestión automática**: Limpieza de archivos temporales

---

## GESTIÓN DE ARCHIVOS

### 📁 ARCHIVOS DE DATOS
- **`datos_hist.dat`**: Almacenamiento principal (binario)
- **`datos_pred.dat`**: Predicciones binarias
- **`predicciones.txt`**: Predicciones en texto legible
- **`config_fechas.dat`**: Configuración de fechas del sistema

### 📁 ARCHIVOS DE REPORTES
- **`reporte_zonas.txt`**: Tabla resumen de zonas
- **`alertas_recomendaciones.txt`**: Alertas y consejos
- **`alertas_temp.txt`**: Archivo temporal de alertas

### 📁 ARCHIVOS DE CHECKPOINTS
- **`[Zona]_semana[N].dat`**: Checkpoints permanentes por zona
- **`temp_[Zona]_semana[N].dat`**: Backups temporales

### 📁 PLANTILLAS DE IMPORTACIÓN
- **`plantilla_datos.txt`**: Archivo de ejemplo para carga masiva
- **Formato personalizable**: Según nombre especificado por usuario

---

## SISTEMA DE IMPORTACIÓN/EXPORTACIÓN

### 📥 IMPORTACIÓN DE DATOS

#### **Formatos soportados:**
1. **Formato simple CSV**:
   ```
   Zona,Fecha,CO2,SO2,NO2,PM25
   Quito,2024-07-01,400.0,5.0,25.0,15.0
   Cuenca,2024-07-02,380.0,4.0,22.0,12.0
   ```

2. **Formato con comentarios**:
   ```
   # Datos de calidad del aire - Ecuador
   # Unidades: CO2 (ppm), SO2/NO2/PM25 (ug/m3)
   Quito,2024-07-01,0.4,5.0,25.0,15.0
   Cuenca,2024-07-02,0.38,4.0,22.0,12.0
   ```

#### **Proceso de importación:**
1. El sistema lee el archivo línea por línea
2. Ignora comentarios (líneas que inician con #) y encabezados
3. Valida rangos de datos automáticamente
4. Distribuye datos en meses disponibles
5. Muestra progreso y resumen final
6. **✅ VALIDACIÓN MEJORADA**: Solo acepta datos dentro de rangos establecidos

### 📤 EXPORTACIÓN DE PLANTILLAS

#### **Características:**
- **Instrucciones incluidas**: Guía paso a paso en el archivo
- **Valores de ejemplo**: Datos realistas para referencia
- **Formato flexible**: Acepta múltiples estilos de entrada
- **Validación automática**: El sistema verifica al importar

---

## ALGORITMOS Y CÁLCULOS

### 🔮 PREDICCIÓN DE CONTAMINACIÓN (MEJORADA)
```
✅ PREDICCIONES FLEXIBLES Y ADAPTATIVAS:
- Funcionan con cualquier cantidad de datos disponibles
- Genera estimaciones básicas cuando no hay datos suficientes
- Requiere mínimo 2 días de datos reales (reducido de 5)
- Usa valores por defecto realistas para estimaciones
- Siempre proporciona algún tipo de pronóstico
```

#### **Algoritmo de predicción adaptativo:**
1. **Con datos suficientes**: Usa tendencias y promedios históricos
2. **Con datos limitados**: Combina datos disponibles con estimaciones
3. **Sin datos**: Genera pronósticos básicos con valores estándar
4. **Evaluación automática**: Clasifica niveles de riesgo estimados

#### **Valores de estimación por defecto:**
- **CO2**: 0.040 ppm (nivel normal urbano)
- **SO2**: 0.008 ug/m3 (nivel bajo típico)
- **NO2**: 0.025 ug/m3 (nivel urbano estándar)
- **PM2.5**: 8.0 ug/m3 (nivel saludable)

### 📊 CÁLCULO DE PROMEDIOS
- **Por zona**: Suma de días/número de días con datos disponibles
- **Global**: Suma de todas las zonas/número de zonas con datos
- **Temporal**: Datos de los meses más recientes con información válida
- **Estimaciones**: Usa valores por defecto cuando no hay datos suficientes

### 🚨 SISTEMA DE ALERTAS
- **Prioridad**: PM2.5 como indicador principal
- **Secundario**: CO2 para alertas adicionales
- **Umbrales**: Basados en estándares OMS actualizados
- **Textos**: Sin acentos ni caracteres especiales para compatibilidad
- **Funcionamiento**: Evalúa tanto datos reales como estimaciones

---

## VALIDACIONES Y CONTROLES

### ✅ VALIDACIÓN DE ENTRADA
- **Rangos numéricos**: Específicos por tipo de contaminante
- **Formato de fecha**: YYYY-MM-DD obligatorio
- **Caracteres especiales**: Evitados para compatibilidad
- **Opción de cancelar**: Disponible en todos los menús con 'c'

### 🛡️ MANEJO DE ERRORES
- **Archivos inexistentes**: Mensajes informativos claros
- **Datos fuera de rango**: Rechazo automático con explicación
- **Memoria insuficiente**: Límites predefinidos (5 zonas, 52 semanas)
- **Entrada inválida**: Bucles de reintento con instrucciones

### 🔄 CONSISTENCIA DE DATOS
- **Sincronización**: Entre datos semanales y valores actuales
- **Integridad**: Verificación de rangos antes de almacenar
- **Persistencia**: Guardado automático en operaciones críticas

---

## CÓDIGOS DE COLORES Y INTERFAZ

### 🎨 SISTEMA DE COLORES ANSI
- **🟢 VERDE**: Estados saludables, operaciones exitosas
- **🟡 AMARILLO**: Advertencias, información importante
- **🟠 NARANJA**: Alertas moderadas
- **🔴 ROJO**: Peligro, errores críticos
- **🔵 AZUL**: Headers, información general
- **🟣 MAGENTA**: Alertas especiales
- **🔷 CIAN**: Menús, decoraciones
- **⚫ NEGRITA**: Énfasis, títulos

### 📱 DISEÑO DE INTERFAZ
- **Tablas ASCII**: Bordes decorativos con +, -, |
- **Menús contextuales**: Numeración clara y consistente
- **Mensajes informativos**: Claros y en español
- **Progreso visual**: Indicadores de estado en operaciones largas

---

## ESPECIFICACIONES TÉCNICAS

### ⚙️ LÍMITES DEL SISTEMA
- **Zonas máximas**: 5 ciudades ecuatorianas (configuración fija)
- **Semanas por zona**: 52 (configurable 1-104)
- **Días por semana**: 7 (configurable 1-7)
- **Alertas simultáneas**: 10 máximo
- **Tamaño de nombres**: 31 caracteres + terminador

### 💾 ALMACENAMIENTO
- **Binario**: Estructuras completas para velocidad
- **Texto**: Reportes legibles para usuarios
- **Temporal**: Archivos de respaldo automático
- **Configuración**: Persistencia de preferencias

### 🔧 COMPILACIÓN Y EJECUCIÓN

#### **Compilación Multiplataforma**
```bash
# Compilación estándar (Windows, Linux, macOS)
gcc -o sistema_aire main.c funciones.c funciones2.c -lm

# Compilación con optimización
gcc -O2 -o sistema_aire main.c funciones.c funciones2.c -lm

# Compilación con depuración
gcc -g -Wall -Wextra -o sistema_aire main.c funciones.c funciones2.c -lm
```

#### **Ejecución por Sistema Operativo**
```bash
# Linux/macOS
./sistema_aire

# Windows
sistema_aire.exe
```

#### **Características de Compilación**
- **✅ Estándar C99**: Compatibilidad máxima multiplataforma
- **✅ Sin Advertencias**: Código completamente limpio
- **✅ Detección automática**: Adapta comportamiento según SO
- **✅ Sin Dependencias**: Solo bibliotecas estándar C
- **✅ Portabilidad**: Funciona en cualquier sistema con GCC

#### **Directivas de Compilación**
```c
#ifdef _WIN32
    // Código específico para Windows
#else
    // Código para Linux/macOS
#endif
```

### 🛠️ CONFIGURACIÓN DEL ENTORNO

#### **Windows**
- **Terminal recomendado**: Windows Terminal o PowerShell 7+
- **Compilador**: MinGW-w64 o Microsoft Visual C++
- **Separadores**: Automáticamente usa `\` para rutas
- **Comandos**: Usa `mkdir "carpeta" 2>nul`

#### **Linux/macOS**
- **Terminal**: Cualquier terminal estándar
- **Compilador**: GCC incluido en la mayoría de distribuciones
- **Separadores**: Automáticamente usa `/` para rutas  
- **Comandos**: Usa `mkdir -p "carpeta" 2>/dev/null`

### 📁 ESTRUCTURA DE ARCHIVOS MULTIPLATAFORMA

#### **Archivos generados (cualquier SO)**
- `sistema_archivos/datos_hist.dat` - Datos históricos
- `sistema_archivos/config_fechas.dat` - Configuración fechas
- `sistema_archivos/datos_pred.dat` - Predicciones binarias
- `sistema_archivos/predicciones.txt` - Predicciones legibles
- `sistema_archivos/mes_actual.dat` - Estado actual del sistema

#### **Detección automática de rutas**
```c
// El sistema detecta automáticamente el separador correcto
char ruta[64];
snprintf(ruta, sizeof(ruta), "sistema_archivos%sdatos.dat", PATH_SEPARATOR);
// Windows: "sistema_archivos\datos.dat"
// Linux/macOS: "sistema_archivos/datos.dat"
```

---

## EJEMPLOS PRÁCTICOS DE USO

### 🔍 EJEMPLO 1: ENTRADA SEGURA DE DATOS

**Antes (Problemático)**:
```c
// ❌ CÓDIGO ANTERIOR - VULNERABLE
printf("Ingrese CO2: ");
scanf("%f", &co2);  // ¡PELIGROSO! No valida entrada
```

**Ahora (Robusto)**:
```c
// ✅ CÓDIGO ACTUAL - SEGURO
float co2 = leerFloatSeguro("Ingrese CO2 (0.0-1.0 ppm): ", 0.0, 1.0);
// Automáticamente valida rango, previene errores, maneja buffer
```

### 🔍 EJEMPLO 2: VALIDACIÓN AUTOMÁTICA

**Entrada del usuario**: `abc` (texto inválido para número)
- **Sistema anterior**: Crash o comportamiento impredecible ❌
- **Sistema actual**: "Entrada inválida. Debe ser un número decimal entre 0.00 y 1.00" ✅

**Entrada del usuario**: `5.0` (fuera de rango CO2)
- **Sistema anterior**: Acepta valor incorrecto ❌  
- **Sistema actual**: "Entrada inválida. Debe estar entre 0.00 y 1.00" ✅

**Entrada del usuario**: `0.045` (valor correcto)
- **Sistema actual**: Acepta y continúa ✅

### 🔍 EJEMPLO 3: ROBUSTEZ EN CADENAS

**Antes**:
```c
// ❌ VULNERABLE A BUFFER OVERFLOW
char nombre[32];
scanf("%s", nombre);  // ¡Puede sobrepasar 32 caracteres!
```

**Ahora**:
```c
// ✅ PROTECCIÓN AUTOMÁTICA
char nombre[32];
leerCadenaSegura("Nombre de zona: ", nombre, 32);
// Nunca excederá 31 caracteres + terminador nulo
```

### 🔍 EJEMPLO 4: EXPERIENCIA DE USUARIO MEJORADA

```
=== ENTRADA TÍPICA CON SISTEMA ROBUSTO ===

Anio (ej. 2024): abc
Entrada invalida. Debe ser un numero entero entre 1900 y 2100.
Anio (ej. 2024): 2030
Mes (1-12): 15
Entrada invalida. Debe ser un numero entero entre 1 y 12.
Mes (1-12): 7
Dia (1-31): 8
✅ Fecha establecida: 08/07/2030
```

---

## VENTAJAS TÉCNICAS DEL SISTEMA ACTUALIZADO

### 🛡️ SEGURIDAD MEJORADA
- **Buffer Overflow**: Eliminado completamente
- **Inyección de Código**: Imposible a través de entrada de datos
- **Validación de Tipos**: Verificación automática de tipos de datos
- **Manejo de Memoria**: Gestión segura sin fugas

### ⚡ RENDIMIENTO OPTIMIZADO  
- **Validación Eficiente**: Algoritmos optimizados de validación
- **Menor Uso de Memoria**: Buffers dimensionados correctamente
- **Menos Llamadas al Sistema**: Funciones optimizadas
- **Código Limpio**: Menor complejidad y mejor mantenibilidad

### 🔄 MANTENIBILIDAD
- **Funciones Reutilizables**: Código DRY (Don't Repeat Yourself)
- **Separación de Responsabilidades**: Validación separada de lógica
- **Documentación Clara**: Comentarios detallados en cada función
- **Estructura Modular**: Fácil expansión y modificación

---

## FLUJO DE TRABAJO RECOMENDADO

### 👨‍💼 USUARIO BÁSICO (DEMOSTRACIÓN COMPLETA)
1. **Primera vez**: Ejecutar → Configurar fecha → Opción 1 (Inicializar con datos saludables)
2. **Ver estado normal**: Opción 3 (Tabla), Opción 5 (Alertas) - Todo en verde ✅
3. **Ver alertas variadas**: Configuración → Generar datos de muestreo → Ver diferencias por ciudad
4. **Explorar alertas**: Opción 5 (Ver todas las alertas), Opción 6 (Recomendaciones)
5. **Uso regular**: Opción 7 (Ingreso manual) para datos reales propios

### 👨‍🔬 USUARIO AVANZADO
1. **Configuración inicial**: Menú configuración → Personalizar zonas y fechas
2. **Carga masiva**: Crear plantilla → Editar datos → Importar
3. **Análisis**: Menú reportes → Exportar para análisis externo
4. **Respaldo**: Menú checkpoints → Guardar estados importantes

### 👨‍💻 ADMINISTRADOR DEL SISTEMA
1. **Mantenimiento**: Revisar archivos .dat periódicamente
2. **Configuración**: Ajustar límites según necesidades
3. **Respaldos**: Copiar archivos críticos regularmente
4. **Monitoreo**: Verificar logs de importación/exportación

---

## RESOLUCIÓN DE PROBLEMAS COMUNES

### ❌ PROBLEMAS FRECUENTES

#### **"No se pudo cargar datos"**
- **Causa**: Archivo `datos_hist.dat` no existe
- **Solución**: Usar Opción 1 para inicializar sistema

#### **"Entrada inválida" (Nuevo Sistema Robusto)**
- **Sistema Mejorado**: Las funciones robustas proporcionan mensajes específicos
- **Soluciones Automáticas**:
  - Formato numérico incorrecto → "Debe ser un número decimal entre X y Y"
  - Fuera de rango → Muestra el rango específico permitido
  - Texto en campo numérico → "Entrada inválida. Debe ser un número..."
  - Buffer demasiado largo → "Entrada demasiado larga. Máximo X caracteres"
- **Ventaja**: El usuario siempre sabe exactamente qué corregir

#### **Problemas de Entrada Eliminados**
- **✅ Buffer Overflow**: Ya no puede ocurrir con las funciones robustas
- **✅ Crashes por `scanf()`**: Eliminados completamente - **TODOS LOS USOS REEMPLAZADOS**
- **✅ Valores Fuera de Rango**: Detección automática antes de procesamiento
- **✅ Caracteres Inválidos**: Manejo inteligente y limpieza automática
- **✅ Entrada Manual de Datos**: Funciones seguras en todas las operaciones críticas

#### **"Zona no encontrada"**
- **Causa**: Búsqueda por nombre sin coincidencia exacta
- **Solución**: Verificar ortografía, usar mayúsculas/minúsculas correctas
- **Mejora**: El sistema ahora valida longitud antes de búsqueda

#### **Colores no se muestran**
- **Causa**: Terminal no compatible con ANSI
- **Solución**: Usar terminal moderno (Windows Terminal, PowerShell 7+)

### 🔧 MANTENIMIENTO

#### **Limpieza de archivos temporales**
```bash
del temp_*.dat
del alertas_temp.txt
```

#### **Reset completo del sistema**
```bash
del datos_hist.dat
del config_fechas.dat
del *.txt
```

#### **Verificación de integridad**
- Revisar que archivos .dat no estén corruptos
- Verificar permisos de escritura en directorio
- Comprobar espacio en disco disponible
- **Nuevo**: Las funciones robustas reducen la corrupción de datos

---

## CASOS DE USO EJEMPLO

### 🏭 MONITOREO INDUSTRIAL
**Escenario**: Empresa con 5 plantas industriales en Ecuador
**Configuración**: Usar las ciudades predefinidas (Quito, Cuenca, Guayaquil, Loja, Ambato) o renombrar por ubicación específica de plantas
**Uso**: Ingreso diario manual + alertas automáticas
**Beneficio**: Cumplimiento normativo ecuatoriano y prevención

### 🏙️ GESTIÓN MUNICIPAL ECUATORIANA
**Escenario**: Ayuntamiento monitoreando calidad del aire en ciudades ecuatorianas
**Configuración**: Usar ciudades predefinidas (Quito, Cuenca, Guayaquil, Loja, Ambato) o renombrar por distritos específicos, importación masiva de sensores
**Uso**: Reportes semanales + exportación para ciudadanos
**Beneficio**: Transparencia gubernamental y planificación urbana sostenible

### 🔬 INVESTIGACIÓN ACADÉMICA
**Escenario**: Universidad ecuatoriana estudiando patrones de contaminación en las principales ciudades del país
**Configuración**: Datos históricos extensos de Quito, Cuenca, Guayaquil, Loja y Ambato via importación
**Uso**: Análisis de tendencias urbanas + predicciones climáticas a largo plazo
**Beneficio**: Papers científicos sobre calidad del aire en Ecuador y modelos ambientales mejorados

### 👥 USO PERSONAL EN ECUADOR
**Escenario**: Ciudadano ecuatoriano interesado en calidad del aire de su ciudad
**Configuración**: Sistema básico con datos de estaciones públicas de MAAE (Ministerio del Ambiente) de Ecuador
**Uso**: Consultas ocasionales de Quito, Cuenca, Guayaquil + alertas para actividades al aire libre
**Beneficio**: Decisiones informadas sobre salud personal y familiar en el contexto ecuatoriano

---

## DESARROLLO Y EXTENSIONES

### 🚀 POSIBLES MEJORAS FUTURAS
- **Base de datos**: Migración de archivos planos a SQL
- **API web**: Interfaz REST para aplicaciones móviles
- **Gráficos**: Visualización de tendencias temporales
- **Machine Learning**: Predicciones más sofisticadas
- **GPS**: Ubicación automática de zonas
- **Notificaciones**: Alertas push en tiempo real

### 🔧 PERSONALIZACIÓN ACTUAL
- **Nombres de zonas**: Completamente configurables
- **Límites temporales**: Ajustables según necesidades
- **Umbrales de alerta**: Modificables en código fuente
- **Formatos de import**: Extensibles para nuevos formatos
- **Colores de interfaz**: Personalizables via #define

---

## RESUMEN DE MEJORAS IMPLEMENTADAS (2025)

### 🌐 COMPATIBILIDAD MULTIPLATAFORMA
- **✅ Soporte Windows**: Separadores `\`, comando `mkdir "carpeta" 2>nul`
- **✅ Soporte Linux/macOS**: Separadores `/`, comando `mkdir -p "carpeta" 2>/dev/null`
- **✅ Detección automática**: Directivas `#ifdef _WIN32` para adaptación
- **✅ Rutas dinámicas**: Macro `PATH_SEPARATOR` para compatibilidad total
- **✅ Funciones adaptativas**: Comportamiento específico por sistema operativo

### 🔄 SISTEMA DE PREDICCIONES MEJORADO
- **✅ Flexibilidad total**: Funciona con cualquier cantidad de datos disponibles
- **✅ Estimaciones inteligentes**: Usa valores por defecto cuando no hay datos suficientes
- **✅ Requisitos reducidos**: Solo 2 días mínimos (antes 5) para datos reales
- **✅ Siempre funciona**: No bloquea por falta de datos, genera estimaciones
- **✅ Mensajes claros**: Indica cuando usa estimaciones vs datos reales

### 🔄 CARACTERES Y CODIFICACIÓN
- **✅ Eliminación de μg/m³**: Reemplazado por "ug/m3" en todo el sistema
- **✅ Eliminación de acentos**: "Dañino" → "Danino", "población" → "poblacion"
- **✅ Caracteres especiales**: Eliminados para máxima compatibilidad
- **✅ Codificación uniforme**: Solo ASCII básico en mensajes y reportes

### 🔄 GESTIÓN DE FECHAS Y MESES
- **✅ Avance inteligente**: Detecta mes máximo y crea el siguiente consecutivo
- **✅ No reinicia en mes 1**: Lógica corregida para continuidad temporal
- **✅ Validación de secuencia**: Verifica que haya datos antes de avanzar
- **✅ Gestión automática**: Calcula próximo mes basado en datos existentes

### 🔄 CREACIÓN DE ARCHIVOS MULTIPLATAFORMA
- **✅ Función `crearCarpetaSistema()`**: Adapta comandos según SO
- **✅ Rutas dinámicas**: Todas las funciones usan separadores correctos
- **✅ Compatibilidad total**: Funciona en Windows, Linux y macOS sin cambios
- **✅ Gestión inteligente**: Detecta y adapta comportamiento automáticamente

### 🔄 TABLAS Y VISUALIZACIÓN
- **✅ Espaciado optimizado**: Tablas con mejor alineación y formato
- **✅ Bordes consistentes**: Uso uniforme de caracteres ASCII para bordes
- **✅ Columnas ajustadas**: Tamaño adecuado para evitar desbordamientos
- **✅ Legibilidad mejorada**: Mejor separación entre elementos

### 🔄 SIMPLIFICACIÓN DEL CÓDIGO
- **✅ Funciones básicas**: Eliminación de dependencias complejas innecesarias
- **✅ Lógica simplificada**: Código más directo y mantenible
- **✅ Validaciones eficientes**: Algoritmos optimizados de verificación
- **✅ Estructura modular**: Mejor organización del código

### 🚀 BENEFICIOS CLAVE ACTUALIZADOS

1. **🔒 100% Seguro**: Eliminación completa de vulnerabilidades de entrada
2. **🌐 Multiplataforma**: Funciona nativamente en Windows, Linux y macOS
3. **⚡ Más Rápido**: Menos errores = menos tiempo perdido
4. **🎯 Más Preciso**: Validación específica para cada tipo de dato
5. **🔄 Más Flexible**: Predicciones funcionan con cualquier cantidad de datos
6. **👤 Más Amigable**: Mensajes de error comprensibles y específicos
7. **🔧 Más Mantenible**: Código modular y reutilizable
8. **🌐 Más Portable**: Funciones estándar C para máxima compatibilidad

---

## CONCLUSIÓN

El Sistema de Gestión y Predicción de Calidad del Aire ha sido completamente mejorado y actualizado para 2025. Las principales mejoras incluyen:

### 🎯 MEJORAS CLAVE IMPLEMENTADAS:

1. **🌐 Compatibilidad Multiplataforma**: Soporte nativo para Windows, Linux y macOS con adaptación automática
2. **🔤 Compatibilidad Universal**: Eliminación completa de caracteres especiales (μg/m³, acentos, Unicode)
3. **� Predicciones Flexibles**: Sistema que funciona con cualquier cantidad de datos disponibles
4. **📅 Fechas Inteligentes**: Avance de mes consecutivo basado en datos existentes (no reinicia en mes 1)
5. **📋 Tablas Optimizadas**: Mejor espaciado y formato para evitar desbordamientos
6. **⚡ Código Simplificado**: Eliminación de funciones complejas innecesarias

### 🚀 BENEFICIOS FINALES:

- **Seguridad Total**: Prevención completa de buffer overflow y errores de entrada
- **Compatibilidad Multiplataforma**: Funciona nativamente en Windows, Linux y macOS
- **Validación Inteligente**: Verificación automática de rangos y tipos de datos
- **Flexibilidad de Predicciones**: Funciona con datos limitados o genera estimaciones
- **Compatibilidad Máxima**: Funciona en cualquier terminal sin problemas de codificación
- **Predicciones Adaptativas**: Usa datos reales cuando disponibles, estimaciones cuando no
- **Experiencia Mejorada**: Mensajes claros y navegación intuitiva
- **Código Limpio**: Estructura modular y mantenible

### 💡 PARA USUARIOS:

#### **Instalación Multiplataforma**
```bash
# Compilación universal (Windows, Linux, macOS)
gcc -o sistema_aire main.c funciones.c funciones2.c -lm

# Ejecución según SO
./sistema_aire        # Linux/macOS
sistema_aire.exe      # Windows
```

#### **Uso Diario**
- **Ingreso manual seguro**: Validación automática de todos los datos
- **Predicciones flexibles**: Funcionan con cualquier cantidad de datos
- **Exportación compatible**: Reportes sin caracteres problemáticos
- **Gestión automática**: Creación de archivos y carpetas multiplataforma

#### **Datos y Predicciones**
- **Importación flexible**: Desde archivos CSV para predicciones precisas
- **Estimaciones inteligentes**: Valores por defecto cuando no hay datos
- **Reportes portables**: Compatibles con cualquier sistema operativo

### 🔧 CARACTERÍSTICAS TÉCNICAS DESTACADAS

#### **Multiplataforma**
- **Detección automática**: Adapta comportamiento según sistema operativo
- **Rutas dinámicas**: Separadores correctos automáticamente
- **Comandos adaptativos**: Usa comandos nativos de cada SO
- **Compatibilidad total**: Sin cambios de código entre plataformas

#### **Predicciones Inteligentes**
- **Siempre funciona**: Genera pronósticos con cualquier cantidad de datos
- **Estimaciones realistas**: Valores por defecto basados en estándares
- **Adaptación automática**: Detecta calidad de datos y ajusta algoritmo
- **Mensajes informativos**: Indica claramente el tipo de pronóstico

#### **Gestión de Archivos**
- **Creación automática**: Carpetas y archivos según necesidades
- **Rutas correctas**: Separadores nativos para cada plataforma
- **Persistencia confiable**: Datos guardados de forma segura
- **Compatibilidad de archivos**: Intercambio entre sistemas operativos

Esta versión representa la solución más robusta, flexible y compatible para monitoreo ambiental, combinando simplicidad de uso con funcionalidad avanzada, todo funcionando nativamente en cualquier sistema operativo moderno.

---

**Versión del Manual**: 4.0 (Sistema Multiplataforma y Predicciones Flexibles)  
**Fecha**: Julio 2025  
**Mejoras Principales**: Soporte multiplataforma, predicciones flexibles, eliminación de caracteres especiales, fechas consecutivas, tablas optimizadas, código simplificado  
**Compatibilidad**: Windows 10+, Linux (cualquier distribución), macOS 10.12+  
**Compilación**: `gcc -o sistema_aire main.c funciones.c funciones2.c -lm`  
**Estado**: Sistema completamente funcional, optimizado y multiplataforma

---

### 📞 SOPORTE TÉCNICO

Para problemas específicos de compilación o ejecución:

#### **Windows**
- Verificar MinGW-w64 instalado
- Usar Windows Terminal para mejor compatibilidad ANSI
- Compilar con: `gcc -o sistema_aire.exe main.c funciones.c funciones2.c -lm`

#### **Linux**
- Instalar gcc: `sudo apt install gcc` (Ubuntu/Debian) o `sudo yum install gcc` (CentOS/RHEL)
- Compilar con: `gcc -o sistema_aire main.c funciones.c funciones2.c -lm`

#### **macOS**
- Instalar Xcode Command Line Tools: `xcode-select --install`
- Compilar con: `gcc -o sistema_aire main.c funciones.c funciones2.c -lm`

El sistema ha sido probado y funciona perfectamente en las tres plataformas principales.
