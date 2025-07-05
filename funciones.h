#define MAX_ZONAS 5
#define MAX_ALERTAS 10
struct Zona {
    char nombre[32];
    float co2, so2, no2, pm25;
    float temperatura, viento, humedad;
};

struct Sistema {
    struct Zona zonas[MAX_ZONAS];
    int numZonas;
};

// Prototipos de funciones principales
void inicializarSistema(struct Sistema *s);
int cargarDatosHistoricos(struct Sistema *s, const char *ruta);
int guardarDatos(struct Sistema *s, const char *ruta);
void calcularPromedios(struct Sistema *s, float promedios[]);
void predecirContaminacion(struct Sistema *s, float prediccion[]);
void emitirAlertas(struct Sistema *s, float prediccion[], char alertas[][64], int *nAlertas);
void generarRecomendaciones(char alertas[][64], int nAlertas);
void mostrarTablaZonas(struct Sistema *s);
void mostrarMenuPrincipal();
void manejarOpcion(int opcion, struct Sistema *s);
int leerEntero(const char *prompt, int min, int max, int allowCancel);
float leerFloat(const char *prompt, float min, float max, int allowCancel);
int confirmar(const char *mensaje);
void ayudaMenu(const char *menuNombre);
