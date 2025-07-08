#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"

#define ANSI_RESET   "\x1b[0m"
#define ANSI_NEGRITA "\x1b[1m"
#define ANSI_CIAN    "\x1b[36m"
#define ANSI_AMARILLO "\x1b[33m"
#define ANSI_VERDE   "\x1b[32m"
#define ANSI_ROJO    "\x1b[31m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_AZUL    "\x1b[34m"

// Variable global para configuracion de fechas
struct ConfiguracionFechas config_fechas = {2024, 1, 1, 1};

// Funcion para cargar o solicitar fecha inicial al usuario
void cargarOSolicitarFechaInicial() {
    // Intentar cargar configuración existente
    if (cargarConfiguracionFechas(&config_fechas)) {
        // Configuración cargada exitosamente
        printf(ANSI_VERDE "Configuracion de fechas cargada exitosamente.\n" ANSI_RESET);
        printf(ANSI_CIAN "Fecha del sistema: %02d/%02d/%d\n" ANSI_RESET,
               config_fechas.dia_inicio, config_fechas.mes_inicio, config_fechas.anio_inicio);
        return;
    }
    
    // Si no existe configuración, solicitar al usuario
    printf(ANSI_NEGRITA ANSI_CIAN "\n+-----------------------------------------------+\n");
    printf("|        CONFIGURACION INICIAL DEL SISTEMA     |\n");
    printf("+-----------------------------------------------+\n" ANSI_RESET);
    
    printf(ANSI_AMARILLO "Primera ejecucion detectada.\n");
    printf("Por favor, ingrese la fecha actual del sistema:\n" ANSI_RESET);
    printf("Anio (ej. 2024): ");
    scanf("%d", &config_fechas.anio_inicio);
    
    printf("Mes (1-12): ");
    scanf("%d", &config_fechas.mes_inicio);
    
    printf("Dia (1-31): ");
    scanf("%d", &config_fechas.dia_inicio);
    
    // Validacion basica
    if (config_fechas.mes_inicio < 1 || config_fechas.mes_inicio > 12) {
        printf(ANSI_ROJO "Mes invalido, estableciendo enero.\n" ANSI_RESET);
        config_fechas.mes_inicio = 1;
    }
    if (config_fechas.dia_inicio < 1 || config_fechas.dia_inicio > 31) {
        printf(ANSI_ROJO "Dia invalido, estableciendo dia 1.\n" ANSI_RESET);
        config_fechas.dia_inicio = 1;
    }
    
    config_fechas.usar_fechas_automaticas = 1;
    
    printf(ANSI_VERDE "Fecha establecida: %02d/%02d/%d\n" ANSI_RESET, 
           config_fechas.dia_inicio, config_fechas.mes_inicio, config_fechas.anio_inicio);
    
    // Guardar la configuración para futuras ejecuciones
    guardarConfiguracionFechas(&config_fechas);
    printf(ANSI_CIAN "Continuando al menu principal...\n" ANSI_RESET);
}

// Prototipos exportados a funciones.c o funciones2.c
void menuConfiguracion(struct Zona zonas[], int *numZonasPtr);
void menuIngresoManual(struct Zona zonas[], int numZonas, int semanaActual[]);
void menuReportes(struct Zona zonas[], int numZonas);
void menuCheckpoints(struct Zona zonas[], int numZonas, int semanaActual[]);
void menuSiguienteSemana(struct Zona zonas[], int numZonas, int semanaActual[]);
void guardarSemana(struct Zona *zona, int semana);
void inicializarZonas(struct Zona zonas[], int *numZonas);

int g_max_semanas = MAX_SEMANAS;
int g_max_dias_semana = MAX_DIAS_SEMANA;

int main() {
    struct Zona zonas[MAX_ZONAS];
    int numZonas = MAX_ZONAS;
    int semanaActual[MAX_ZONAS] = {0};

    // Cargar configuración de fechas o solicitarla si es la primera vez
    cargarOSolicitarFechaInicial();

    inicializarZonas(zonas, &numZonas);

    while (1) {
        printf(ANSI_NEGRITA ANSI_AZUL "\n+----------------------------------------------------------+\n");
        printf("|        SISTEMA INTEGRAL DE GESTION Y PREDICCION AIRE     |\n");
        printf("+----------------------------------------------------------+\n");
        printf("| 1. Configuracion                                         |\n");
        printf("| 2. Ingreso manual de datos                               |\n");
        printf("| 3. Reportes                                              |\n");
        printf("| 4. Checkpoints (guardar/recuperar semana)                |\n");
        printf("| 5. Siguiente semana                                      |\n");
        printf("| 6. Salir                                                 |\n");
        printf("+----------------------------------------------------------+\n" ANSI_RESET);
        printf("Seleccione opcion: ");
        int opcion;
        scanf("%d", &opcion);

        if (opcion == 1) {
            menuConfiguracion(zonas, &numZonas);
        } else if (opcion == 2) {
            menuIngresoManual(zonas, numZonas, semanaActual);
        } else if (opcion == 3) {
            menuReportes(zonas, numZonas);
        } else if (opcion == 4) {
            menuCheckpoints(zonas, numZonas, semanaActual);
        } else if (opcion == 5) {
            menuSiguienteSemana(zonas, numZonas, semanaActual);
        } else if (opcion == 6) {
            // Guardar todas las semanas antes de salir
            for (int i = 0; i < numZonas; i++) {
                guardarSemana(&zonas[i], semanaActual[i]);
            }
            // Guardar configuración de fechas al salir
            guardarConfiguracionFechas(&config_fechas);
            printf(ANSI_MAGENTA "Datos y configuracion guardados. Saliendo del sistema.\n" ANSI_RESET);
            break;
        } else {
            printf(ANSI_ROJO "Opcion invalida.\n" ANSI_RESET);
        }
    }
    return 0;
}
