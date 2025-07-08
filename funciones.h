#define MAX_ZONAS 5
#define MAX_ALERTAS 10
#define MAX_SEMANAS 52
#define MAX_DIAS_SEMANA 7
#define MAX_NOMBRE_ZONA 32

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
};

typedef struct {
    float min;
    float max;
} Umbral;

typedef struct {
    Umbral co2;
    Umbral so2;
    Umbral no2;
    Umbral pm25;
} Umbrales;

typedef struct {
    char fecha[11]; // YYYY-MM-DD
    float co2, so2, no2, pm25;
} DatosAmbientales;

typedef struct {
    DatosAmbientales dias[MAX_DIAS_SEMANA];
    int numDias;
} Semana;

void inicializarSistema(struct Sistema *s);
int cargarDatosHistoricos(struct Sistema *s, char *ruta);
int guardarDatos(struct Sistema *s, char *ruta);
void calcularPromedios(struct Sistema *s, float promedios[]);
void predecirContaminacion(struct Sistema *s, float prediccion[]);
void emitirAlertas(struct Sistema *s, float prediccion[], char alertas[][64], int *nAlertas);
void generarRecomendaciones(char alertas[][64], int nAlertas);
void mostrarTablaZonas(struct Sistema *s);
void mostrarMenuPrincipal();
void manejarOpcion(int opcion, struct Sistema *s);
int leerEntero(char *prompt, int min, int max, int allowCancel);
float leerFloat(char *prompt, float min, float max, int allowCancel);
int confirmar(char *mensaje);
void ayudaMenu(char *menuNombre);
void actualizarFechaManualmente(struct Sistema *s);
// Funciones de ICA y alertas mejoradas
char* calcularICA(float pm25);
void mostrar_alerta(float pm25, char* zona);
void registrarPredicciones(struct Sistema *s, float prediccion[]);

// Funciones de configuración y gestión
void inicializarZonas(Zona zonas[], int *numZonas);
void menuConfiguracionZona(Zona zonas[], int numZonas);
void editarUmbrales(Umbrales *umbrales);
void cambiarNombreZona(char *nombreZona);

// Muestreo y datos
void generarDatosAleatoriosSemana(Zona *zona, int semana);
void ingresarDatosManualSemana(Zona *zona, int semana);

// Reportes y recuperación
void mostrarReporteSemanal(Zona *zona, int semana);
void guardarSemana(Zona *zona, int semana);
void cargarSemana(Zona *zona, int semana);

// Alertas
char* alertaPM25(float valor);
char* alertaNO2(float valor);
char* alertaCO2(float valor, struct Umbral umbral);
char* alertaSO2(float valor, struct Umbral umbral);
    ALERTA_ROJA
} NivelAlerta;

NivelAlerta calcularAlertaPM25(float valor, Umbral umbral);
NivelAlerta calcularAlertaNO2(float valor, Umbral umbral);
NivelAlerta calcularAlertaCO2(float valor, Umbral umbral);
NivelAlerta calcularAlertaSO2(float valor, Umbral umbral);
const char* nombreAlerta(NivelAlerta nivel);
