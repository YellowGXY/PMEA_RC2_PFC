#define MAX_ZONAS 5
#define MAX_ALERTAS 10
#define MAX_MESES 12
#define MAX_DIAS_MES 31
#define MAX_NOMBRE_ZONA 32
#define MAX_SEMANAS 52
#define MAX_DIAS_SEMANA 7

// Constantes para tipos de contaminantes
#define TIPO_CO2 0
#define TIPO_SO2 1
#define TIPO_NO2 2
#define TIPO_PM25 3

// Estructura para gestión de fechas del sistema
struct ConfiguracionFechas {
    int anio_inicio;
    int mes_inicio;
    int dia_inicio;
    int usar_fechas_automaticas;  // 1 = usar fechas automáticas, 0 = pedir fecha cada vez
};

// Variable global para configuración de fechas
extern struct ConfiguracionFechas config_fechas;

struct Umbral {
    float min;
    float max;
};

struct Umbrales {
    struct Umbral co2;
    struct Umbral so2;
    struct Umbral no2;
    struct Umbral pm25;
};

struct DatosAmbientales {
    char fecha[32]; // YYYY-MM-DD with plenty of space to avoid truncation warnings
    float co2, so2, no2, pm25;
};

struct Semana {
    struct DatosAmbientales dias[MAX_DIAS_SEMANA];
    int numDias;
};

struct Mes {
    struct DatosAmbientales dias[MAX_DIAS_MES];
    int numDias;
};

struct Zona {
    char nombre[MAX_NOMBRE_ZONA];
    float co2, so2, no2, pm25;
    float temperatura, viento, humedad;
    struct Mes meses[MAX_MESES];
    int numMeses;
    struct Semana semanas[MAX_SEMANAS];
    int numSemanas;
    struct Umbrales umbrales;
};

struct Sistema {
    struct Zona zonas[MAX_ZONAS];
    int numZonas;
    char fecha[11]; 
    struct ConfiguracionFechas config_fechas;  // Nueva configuración de fechas
};

void inicializarSistema(struct Sistema *sistema);
int cargarDatosHistoricos(struct Sistema *sistema, char *ruta_archivo);
int guardarDatos(struct Sistema *sistema, char *ruta_archivo);
void calcularPromedios(struct Sistema *sistema, float promedios[]);
void predecirContaminacion(struct Sistema *sistema, float prediccion[]);
void emitirAlertas(struct Sistema *sistema, float prediccion[], char alertas[][64], int *numero_alertas);
void generarRecomendaciones(char alertas[][64], int numero_alertas);
void mostrarTablaZonas(struct Sistema *sistema);
void mostrarMenuPrincipal();
void manejarOpcion(int opcion_seleccionada, struct Sistema *sistema);
int leerEntero(char *mensaje_prompt, int valor_minimo, int valor_maximo, int permitir_cancelar);
float leerFloat(char *mensaje_prompt, float valor_minimo, float valor_maximo, int permitir_cancelar);
int confirmar(char *mensaje_confirmacion);
void ayudaMenu(char *menuNombre);
void actualizarFechaManualmente(struct Sistema *sistema);
char* calcularICA(float pm25);
void mostrar_alerta(float pm25, char* zona);
void registrarPredicciones(struct Sistema *sistema, float prediccion[]);
void mostrarHistorialZonas();
void mostrarDetalleZona(struct Sistema *sistema);
void buscarZonaPorNombre(struct Sistema *sistema);

// Funciones de configuración y gestión
void inicializarZonas(struct Zona zonas[], int *numero_zonas);
void inicializarConfiguracionFechas(struct ConfiguracionFechas *config);
void menuConfiguracionZona(struct Zona zonas[], int numero_zonas);
void editarUmbrales(struct Umbrales *umbrales);
void cambiarNombreZona(char *nombreZona);
void menuConfiguracion(struct Zona zonas[], int *numZonasPtr, int mesActual[]);

// Muestreo y datos
void generarDatosAleatoriosMes(struct Zona *zona, int numero_mes);
void ingresarDatosManualMes(struct Zona *zona, int numero_mes, struct ConfiguracionFechas *config);
void ingresarDatosManualMejorado(struct Zona *zona, int semana, struct ConfiguracionFechas *config);

// Reportes y recuperación
void mostrarReporteMensual(struct Zona *zona, int numero_mes);
void guardarMes(struct Zona *zona, int numero_mes);
void cargarMes(struct Zona *zona, int numero_mes);
void mostrarAlertasYRecomendaciones(struct Zona zonas[], int numZonas);
void exportarReporteTabla(struct Zona zonas[], int numZonas);
void exportarAlertasYRecomendaciones(struct Zona zonas[], int numZonas);

// Alertas (solo funciones que devuelven string)
char* alertaPM25(float valor);
char* alertaNO2(float valor);
char* alertaCO2(float valor, struct Umbral umbral);
char* alertaSO2(float valor, struct Umbral umbral);

#define ALERTA_VERDE   0
#define ALERTA_AMARILLA 1
#define ALERTA_NARANJA 2
#define ALERTA_ROJA    3

int calcularAlertaPM25(float valor, struct Umbral umbral);
int calcularAlertaNO2(float valor, struct Umbral umbral);
int calcularAlertaCO2(float valor, struct Umbral umbral);
int calcularAlertaSO2(float valor, struct Umbral umbral);
char* nombreAlerta(int nivel);

// Funciones para gestión mejorada de fechas y entrada de datos
void configurarFechasInicio(struct ConfiguracionFechas *config);
void calcularFechaAutomatica(struct ConfiguracionFechas *config, int dia_relativo, char *fecha_resultado);
void ingresarDatosManualMejorado(struct Zona *zona, int mes, struct ConfiguracionFechas *config);
int validarEntradaNumerica(char *entrada, float *resultado, float min, float max);
void limpiarBuffer();
void menuSiguienteMes(struct Zona zonas[], int numZonas, int mesActual[]);

// Funciones para gestión de archivos temporales
void guardarMesTemporal(struct Zona *zona, int numero_mes);
void cargarMesTemporal(struct Zona *zona, int numero_mes);
void limpiarDatosTemporales(struct Zona *zona, int numero_mes);
void vaciarDatosTemporales();

// Funciones de carga masiva de datos
void exportarPlantillaDatos(const char *nombre_archivo);
int importarDatosDesdeArchivo(struct Zona zonas[], int numero_zonas, const char *nombre_archivo);

// Funciones para gestión de configuración de fechas
int guardarConfiguracionFechas(struct ConfiguracionFechas *config);
int cargarConfiguracionFechas(struct ConfiguracionFechas *config);

// Funciones de entrada de datos robustas
void limpiarBufferEntrada();
int validarEnteroEnRango(const char *entrada, int min, int max);
int validarFloatEnRango(const char *entrada, float min, float max);
int leerEnteroSeguro(const char *mensaje, int min, int max);
float leerFloatSeguro(const char *mensaje, float min, float max);
char leerCaracterSeguro(const char *mensaje);
void leerCadenaSegura(const char *mensaje, char *destino, int tamano_max);

// Funciones de manejo de semanas
void ingresarDatosManualSemana(struct Zona *zona, int numero_semana);
void guardarSemana(struct Zona *zona, int numero_semana);
void cargarSemana(struct Zona *zona, int numero_semana);
void guardarSemanaActual(int semanaActual[], int numZonas);
void cargarSemanaActual(int semanaActual[], int numZonas);
void cargarDatosZonas(struct Zona zonas[], int numZonas);
void guardarMesActual(int mesActual[], int numZonas);
void cargarMesActual(int mesActual[], int numZonas);

// Funciones de análisis de calidad del aire
const char* obtenerColorContaminante(float valor, int tipo_contaminante);
char* obtenerNivelPeligrosidad(float valor, int tipo_contaminante);

// Funciones para gestión de estado de meses actuales
void guardarMesActual(int mesActual[], int numZonas);
void cargarMesActual(int mesActual[], int numZonas);
void cargarDatosZonas(struct Zona zonas[], int numZonas);

// Funciones de colores para niveles de peligrosidad
const char* obtenerColorContaminante(float valor, int tipo_contaminante);
char* obtenerNivelPeligrosidad(float valor, int tipo_contaminante);

// Tipos de contaminantes para coloreo
#define TIPO_CO2 0
#define TIPO_SO2 1
#define TIPO_NO2 2
#define TIPO_PM25 3

// Funciones para manejo de datos temporales y checkpoints
void cargarSemanaTemporal(struct Zona *zona, int semana);
void guardarSemanaTemporal(struct Zona *zona, int semana);
void limpiarDatosTemporales(struct Zona *zona, int semana);
void cargarMesTemporal(struct Zona *zona, int mes);
void guardarMesTemporal(struct Zona *zona, int mes);

// Funciones para manejo de configuración de fechas
int cargarConfiguracionFechas(struct ConfiguracionFechas *config);
int guardarConfiguracionFechas(struct ConfiguracionFechas *config);

// Funciones para manejo de estado de semanas/meses actuales
void cargarSemanaActual(int semanaActual[], int numZonas);
void guardarSemanaActual(int semanaActual[], int numZonas);
void cargarMesActual(int mesActual[], int numZonas);
void guardarMesActual(int mesActual[], int numZonas);

// Funciones para manejo de carpetas del sistema
void crearCarpetaSistema();

// Función para actualizar fecha del sistema según datos importados
void actualizarFechaSegunDatos(struct Zona zonas[], int numZonas);

// Funciones de pronósticos
void menuPronosticos(struct Zona zonas[], int numZonas, int mesActual[]);
void generarPronosticoZona(struct Zona *zona, int mesActual);
void generarPronosticoGeneral(struct Zona zonas[], int numZonas, int mesActual[]);
void predecirAlertasPM25(struct Zona zonas[], int numZonas, int mesActual[]);
void mostrarTendenciasContaminacion(struct Zona zonas[], int numZonas, int mesActual[]);

// Función para ajustar mes actual según fecha del sistema
void ajustarMesActualSegunFecha(int mesActual[], int numZonas);

// Función para determinar mes actual inteligente según datos importados
int determinarMesActualInteligente(struct Zona zonas[], int numZonas, int meses_detectados[]);

// Funciones para validación y manejo de datos importados
int validarDatosImportados(struct Zona zonas[], int numZonas, int meses_detectados[]);
void generarResumenImportacion(struct Zona zonas[], int numZonas, int meses_detectados[], 
                              int total_importados, int total_errores, int total_duplicados);
int completarDiasFaltantes(struct Zona zonas[], int numZonas, int meses_detectados[]);
void manejarDiasFaltantes(struct Zona zonas[], int numZonas, int meses_detectados[]);

// Funciones para manejo de carpetas de reportes
void crearCarpetasReportes();
void solicitarNombreArchivo(const char* tipo_reporte, char* nombre_usuario);
void generarNombreUnico(const char* carpeta, const char* nombre_base, const char* extension, char* nombre_final);
