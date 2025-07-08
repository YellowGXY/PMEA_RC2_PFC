#define MAX_ZONAS 5
#define MAX_ALERTAS 10
#define MAX_SEMANAS 52
#define MAX_DIAS_SEMANA 7
#define MAX_NOMBRE_ZONA 32

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
    char fecha[11]; // YYYY-MM-DD
    float co2, so2, no2, pm25;
};

struct Semana {
    struct DatosAmbientales dias[MAX_DIAS_SEMANA];
    int numDias;
};

struct Zona {
    char nombre[MAX_NOMBRE_ZONA];
    float co2, so2, no2, pm25;
    float temperatura, viento, humedad;
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

// Muestreo y datos
void generarDatosAleatoriosSemana(struct Zona *zona, int numero_semana);
void ingresarDatosManualSemana(struct Zona *zona, int numero_semana);

// Reportes y recuperación
void mostrarReporteSemanal(struct Zona *zona, int numero_semana);
void guardarSemana(struct Zona *zona, int numero_semana);
void cargarSemana(struct Zona *zona, int numero_semana);

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
void ingresarDatosManualMejorado(struct Zona *zona, int semana, struct ConfiguracionFechas *config);
int validarEntradaNumerica(char *entrada, float *resultado, float min, float max);
void limpiarBuffer();
void menuSiguienteSemana(struct Zona zonas[], int numZonas, int semanaActual[]);

// Funciones para gestión de archivos temporales
void guardarSemanaTemporal(struct Zona *zona, int numero_semana);
void cargarSemanaTemporal(struct Zona *zona, int numero_semana);
void limpiarDatosTemporales(struct Zona *zona, int numero_semana);
void vaciarDatosTemporales();

// Funciones de carga masiva de datos
void exportarPlantillaDatos(const char *nombre_archivo);
int importarDatosDesdeArchivo(struct Zona zonas[], int numero_zonas, const char *nombre_archivo);

// Funciones para gestión de configuración de fechas
int guardarConfiguracionFechas(struct ConfiguracionFechas *config);
int cargarConfiguracionFechas(struct ConfiguracionFechas *config);