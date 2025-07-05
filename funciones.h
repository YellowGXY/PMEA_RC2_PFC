#ifndef FUNCIONES_H
#define FUNCIONES_H

#define MAX_ZONAS 5
#define MAX_ALERTAS 10

typedef struct {
    char nombre[32];
    float co2, so2, no2, pm25;
    float temperatura, viento, humedad;
} Zona;

typedef struct {
    Zona zonas[MAX_ZONAS];
    int numZonas;
} Sistema;

// Prototipos de funciones principales
void inicializarSistema(Sistema *s);
int cargarDatosHistoricos(Sistema *s, const char *ruta);
int guardarDatos(Sistema *s, const char *ruta);
void calcularPromedios(Sistema *s, float promedios[]);
void predecirContaminacion(Sistema *s, float prediccion[]);
void emitirAlertas(Sistema *s, float prediccion[], char alertas[][64], int *nAlertas);
void generarRecomendaciones(char alertas[][64], int nAlertas);
void mostrarTablaZonas(Sistema *s);
void mostrarMenuPrincipal();
void manejarOpcion(int opcion, Sistema *s);
int leerEntero(const char *prompt, int min, int max, int allowCancel);
float leerFloat(const char *prompt, float min, float max, int allowCancel);
int confirmar(const char *mensaje);
void ayudaMenu(const char *menuNombre);

#endif
