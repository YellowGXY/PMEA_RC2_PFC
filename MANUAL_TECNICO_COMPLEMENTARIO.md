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
#ifdef _WIN32
    #define PATH_SEPARATOR "\\"
#else
    #define PATH_SEPARATOR "/"
#endif
snprintf(ruta_archivo, sizeof(ruta_archivo), "sistema_archivos%sdatos.dat", PATH_SEPARATOR);
```

### 🔧 CREACIÓN DE DIRECTORIOS ADAPTATIVOS
```c
void crearCarpetaSistema() {
    #ifdef _WIN32
        system("mkdir \"sistema_archivos\" 2>nul");
    #else
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
struct Dia {
    float co2, so2, no2, pm25;
    float temperatura, viento, humedad;
    char fecha[11]; // "YYYY-MM-DD"
};
struct Mes {
    struct Dia dias[31];
    int numDias;
};
struct Zona {
    char nombre[32];
    struct Mes meses[12];
    int numMeses;
    float co2, so2, no2, pm25, temperatura, viento, humedad;
    struct { struct { float min, max; } co2, so2, no2, pm25; } umbrales;
};
struct Sistema {
    struct Zona zonas[5];
    int numZonas;
};
struct ConfiguracionFechas {
    int anio_inicio, mes_inicio, dia_inicio;
    int usar_fechas_automaticas;
};
```

### FUNCIONES PRINCIPALES POR ARCHIVO

#### **main.c**
```c
void cargarOSolicitarFechaInicial();
int main();
```
#### **funciones.c**
```c
void inicializarZonas(struct Zona zonas[], int *numero_zonas);
void inicializarSistema(struct Sistema *sistema);
void inicializarConfiguracionFechas(struct ConfiguracionFechas *config);
int cargarDatosHistoricos(struct Sistema *sistema, char *ruta_archivo);
int guardarDatos(struct Sistema *sistema, char *ruta_archivo);
void calcularPromedios(struct Sistema *sistema, float promedios[]);
void predecirContaminacion(struct Sistema *sistema, float prediccion[]);
void emitirAlertas(struct Sistema *sistema, float prediccion[], char alertas[][64], int *numero_alertas);
void generarRecomendaciones(char alertas[][64], int numero_alertas);
void mostrarTablaZonas(struct Sistema *sistema);
void mostrarHistorialZonas();
void mostrarDetalleZona(struct Sistema *sistema);
void buscarZonaPorNombre(struct Sistema *sistema);
int leerEnteroSeguro(const char *mensaje, int min, int max);
float leerFloatSeguro(const char *mensaje, float min, float max);
char leerCaracterSeguro(const char *mensaje);
void leerCadenaSegura(const char *mensaje, char *destino, int tamano_max);
int validarEnteroEnRango(const char *entrada, int min, int max);
int validarFloatEnRango(const char *entrada, float min, float max);
void limpiarBufferEntrada();
void crearCarpetaSistema();
void guardarMes(struct Zona *zona, int numero_mes);
void cargarMes(struct Zona *zona, int numero_mes);
void guardarSemanaActual(int semanaActual[], int numZonas);
void cargarSemanaActual(int semanaActual[], int numZonas);
void registrarPredicciones(struct Sistema *sistema, float prediccion[]);
```
#### **funciones2.c**
```c
void menuConfiguracion(struct Zona zonas[], int *numZonasPtr, int mesActual[]);
void menuIngresoManual(struct Zona zonas[], int numZonas, int mesActual[]);
void menuReportes(struct Zona zonas[], int numZonas);
void menuCheckpoints(struct Zona zonas[], int numZonas, int mesActual[]);
void menuSiguienteMes(struct Zona zonas[], int numZonas, int mesActual[]);
void menuPronosticos(struct Zona zonas[], int numZonas, int mesActual[]);
void generarPronosticoZona(struct Zona *zona, int mesActual);
void generarPronosticoGeneral(struct Zona zonas[], int numZonas, int mesActual[]);
void mostrarDatosZona(struct Zona *zona, int mesActual);
void importarDatosDesdeArchivo(struct Zona zonas[], int numZonas, int mesActual[]);
void crearPlantillaDatos(struct Zona zonas[], int numZonas);
void cambiarNombreZona(struct Zona zonas[], int numZonas);
void ajustarMaximoSemanas(struct Zona zonas[], int numZonas);
void generarDatosMuestreo(struct Zona zonas[], int numZonas, int mesActual[]);
void configurarFechasInicio(struct ConfiguracionFechas *config);
void ajustarMesActualSegunFecha(int mesActual[], int numZonas);
int guardarConfiguracionFechas(struct ConfiguracionFechas *config);
int cargarConfiguracionFechas(struct ConfiguracionFechas *config);
void guardarMesActual(int mesActual[], int numZonas);
void cargarMesActual(int mesActual[], int numZonas);
void esperarEnter();
void mostrarMensajeBienvenida();
int esBisiesto(int anio);
int diasEnMes(int mes, int anio);
```

---

## FUNCIONAMIENTO GENERAL

El sistema inicia solicitando la fecha inicial si es la primera ejecución, o carga la configuración guardada. Presenta un menú principal con 12 opciones para inicializar, cargar, mostrar, predecir, alertar, recomendar, ingresar datos, exportar, ver historial, detalle, buscar zonas, configuración avanzada y checkpoints.

### Menús principales:
- Inicialización, carga/guardado de datos, visualización de tablas, predicción, alertas, recomendaciones, ingreso manual, exportación, historial, detalle/búsqueda de zonas, configuración avanzada, checkpoints.

---

## VALIDACIÓN Y SEGURIDAD

- **Entrada robusta**: Todas las entradas usan funciones seguras (`leerFloatSeguro`, `leerEnteroSeguro`, etc.), sin uso de `scanf` directo.
- **Rangos estrictos**: Cada contaminante y variable ambiental tiene límites validados.
- **Buffer seguro**: No hay posibilidad de desbordamiento en cadenas o números.
- **Mensajes claros**: El sistema informa exactamente el error y cómo corregirlo.
- **Cancelación segura**: El usuario puede cancelar cualquier operación.

---

## PREDICCIÓN Y ALGORITMOS

- Si hay datos suficientes: usa promedios y tendencias históricas.
- Si hay pocos datos: combina datos reales y estimaciones.
- Si no hay datos: usa valores estándar realistas (CO2: 0.040 ppm, SO2: 0.008 ug/m3, NO2: 0.025 ug/m3, PM2.5: 8.0 ug/m3).
- Siempre genera un pronóstico, nunca bloquea por falta de datos.

### Alertas:
- Basadas en estándares OMS para PM2.5 y CO2.
- Clasificación por colores (solo en estados relevantes, usando ANSI si el terminal lo soporta).
- Recomendaciones automáticas según nivel de alerta.

---

## IMPORTACIÓN Y EXPORTACIÓN

- Soporta CSV simple y con comentarios.
- Ignora encabezados y líneas con `#`.
- Valida automáticamente todos los datos.
- Actualiza fecha del sistema al último registro importado.
- Reportes y alertas en archivos de texto plano.
- Nombres únicos para evitar sobrescritura.
- Plantillas de importación generadas con instrucciones y ejemplos.

---

## GESTIÓN DE ARCHIVOS Y MULTIPLATAFORMA

- **Separadores de ruta**: `\` en Windows, `/` en Linux/macOS, detectados automáticamente.
- **Creación de carpetas**: `mkdir` adaptativo según SO.
- **Archivos binarios**: Para datos históricos, predicciones y configuración.
- **Archivos de texto**: Para reportes, alertas y plantillas.
- **Checkpoints**: Archivos por semana y zona, temporales y permanentes.

---

## COMPILACIÓN Y EJECUCIÓN

### Compilación:
```bash
# Windows (MinGW-w64)
gcc -o sistema_aire.exe main.c funciones.c funciones2.c -lm
# Linux/macOS
gcc -o sistema_aire main.c funciones.c funciones2.c -lm
```
### Ejecución:
- Windows: `sistema_aire.exe`
- Linux/macOS: `./sistema_aire`

---

## EJEMPLOS DE USO

### Ingreso seguro:
```c
float co2 = leerFloatSeguro("Ingrese CO2 (0.0-1.0 ppm): ", 0.0, 1.0);
```
### Importación CSV:
```
Quito,2024-07-01,0.4,5.0,25.0,15.0
```
### Exportación de reporte:
- `reporte_zonas.txt` con tabla resumen.
- `alertas_recomendaciones.txt` con alertas activas.

---

## RESOLUCIÓN DE PROBLEMAS

- **No se pudo cargar datos**: Inicializar sistema (opción 1).
- **Entrada inválida**: Revisar mensaje, corregir formato o rango.
- **Colores no se muestran**: Usar terminal compatible ANSI.
- **Archivos corruptos**: Eliminar y reiniciar con datos limpios.

---

## EXTENSIÓN Y PERSONALIZACIÓN

- Nombres de zonas, límites de semanas/días, umbrales de alerta y formatos de importación son configurables en el código fuente.
- El sistema es modular y fácilmente ampliable para nuevas zonas, contaminantes o algoritmos.

---

## ESPECIFICACIONES TÉCNICAS

- **Zonas**: 5 (fijo, pero renombrables)
- **Semanas**: 52 por zona (configurable)
- **Días por semana**: 7 (configurable)
- **Tamaño de nombre de zona**: 31 caracteres + terminador
- **Archivos**: Binarios y texto, multiplataforma
- **Compilación**: C99, sin dependencias externas

---

## SOPORTE Y CONTACTO

- **Windows**: Usar MinGW-w64, Windows Terminal o PowerShell 7+.
- **Linux/macOS**: GCC estándar, terminal moderno.
- **Problemas**: Revisar mensajes de error, consultar este manual, limpiar archivos temporales si es necesario.

---

**Versión del Manual**: 4.1 (Extendido y complementado)
**Fecha**: Julio 2025
**Compatibilidad**: Windows 10+, Linux, macOS
**Compilación**: `gcc -o sistema_aire main.c funciones.c funciones2.c -lm`
**Estado**: Sistema robusto, seguro y multiplataforma.
